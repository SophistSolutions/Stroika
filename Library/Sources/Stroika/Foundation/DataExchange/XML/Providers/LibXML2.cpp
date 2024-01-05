/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/CString/Utilities.h"
#include "../../../Characters/String.h"
#include "../../../DataExchange/BadFormatException.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/Throw.h"
#include "../../../Memory/Common.h"
#include "../../../Memory/MemoryAllocator.h"
#include "Stroika/Foundation/Streams/ToSeekableInputStream.h"

#include "LibXML2.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::DOM;
using namespace Stroika::Foundation::DataExchange::XML::Schema;
using namespace Stroika::Foundation::DataExchange::XML::Providers::LibXML2;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;

using std::byte;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

static_assert (qHasFeature_libxml2, "Don't compile this file if qHasFeature_libxml2 not set");

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::DataExchange::XML, qHasFeature_libxml2, qHasFeature_libxml2);

#if qCompilerAndStdLib_clangWithLibStdCPPStringConstexpr_Buggy
namespace {
    inline std::u16string clang_string_BWA_ (const char16_t* a, const char16_t* b)
    {
        return {a, b};
    }
}
#endif

namespace {
    struct SchemaRep_ : ILibXML2SchemaRep {
        SchemaRep_ (const optional<URI>& targetNamespace, const Memory::BLOB& targetNamespaceData,
                    const Sequence<SourceComponent>& sourceComponents, const NamespaceDefinitionsList& namespaceDefinitions)
            : fTargetNamespace{targetNamespace}
            , fSourceComponents{sourceComponents}
            , fNamespaceDefinitions{namespaceDefinitions}
        {
            if (targetNamespace) {
                fSourceComponents.push_back (SourceComponent{.fBLOB = targetNamespaceData, .fNamespace = *targetNamespace});
            }
            xmlSchemaParserCtxt* schemaParseContext = xmlSchemaNewMemParserCtxt (reinterpret_cast<const char*> (targetNamespaceData.data ()),
                                                                                 static_cast<int> (targetNamespaceData.size ()));
            fCompiledSchema = xmlSchemaParse (schemaParseContext);
            xmlSchemaFreeParserCtxt (schemaParseContext);
            Execution::ThrowIfNull (fCompiledSchema);
        }
        SchemaRep_ (const SchemaRep_&) = delete;
        virtual ~SchemaRep_ ()
        {
            xmlSchemaFree (fCompiledSchema);
        }
        optional<URI>             fTargetNamespace;
        Sequence<SourceComponent> fSourceComponents;
        NamespaceDefinitionsList  fNamespaceDefinitions;
        xmlSchema*                fCompiledSchema{nullptr};

        virtual const Providers::ISchemaProvider* GetProvider () const override
        {
            return &XML::Providers::LibXML2::kDefaultProvider;
        }
        virtual optional<URI> GetTargetNamespace () const override
        {
            return fTargetNamespace; // should get from READING the schema itself! I THINK --LGP 2023-12-18
        }
        virtual NamespaceDefinitionsList GetNamespaceDefinitions () const override
        {
            return fNamespaceDefinitions;
        }
        virtual Sequence<SourceComponent> GetSourceComponents () override
        {
            return fSourceComponents;
        }
        virtual xmlSchema* GetSchemaLibRep () override
        {
            AssertNotNull (fCompiledSchema);
            return fCompiledSchema;
        }
    };
}

namespace {
    struct SAXReader_ {
        using Name = StructuredStreamEvents::Name;
        xmlSAXHandler                      flibXMLSaxHndler_{};
        StructuredStreamEvents::IConsumer& fCallback_;
        SAXReader_ (StructuredStreamEvents::IConsumer& callback)
            : fCallback_{callback}
        {
            flibXMLSaxHndler_.initialized   = XML_SAX2_MAGIC;
            flibXMLSaxHndler_.startDocument = [] (void* ctx) {
                SAXReader_* thisReader = reinterpret_cast<SAXReader_*> (ctx);
                Assert (thisReader->flibXMLSaxHndler_.initialized == XML_SAX2_MAGIC); // assure ctx ptr passed through properly
                thisReader->fCallback_.StartDocument ();
            };
            flibXMLSaxHndler_.endDocument = [] (void* ctx) {
                SAXReader_* thisReader = reinterpret_cast<SAXReader_*> (ctx);
                Assert (thisReader->flibXMLSaxHndler_.initialized == XML_SAX2_MAGIC); // assure ctx ptr passed through properly
                thisReader->fCallback_.EndDocument ();
            };
            flibXMLSaxHndler_.startElementNs = [] (void* ctx, const xmlChar* localname, [[maybe_unused]] const xmlChar* prefix,
                                                   const xmlChar* URI, [[maybe_unused]] int nb_namespaces, const xmlChar** namespaces,
                                                   int nb_attributes, [[maybe_unused]] int nb_defaulted, const xmlChar** attributes) {
                SAXReader_* thisReader = reinterpret_cast<SAXReader_*> (ctx);
                Assert (thisReader->flibXMLSaxHndler_.initialized == XML_SAX2_MAGIC); // assure ctx ptr passed through properly
                Mapping<Name, String> attrs;
                if (attributes != nullptr) {
                    // Crazy way to decode attribute arguments - I would have never guessed --
                    // https://stackoverflow.com/questions/2075894/how-to-get-the-name-and-value-of-attributes-from-xml-when-using-libxml2-sax-pars
                    auto ai = attributes;
                    for (int i = 0; i < nb_attributes; i++) {
                        attrs.Add (libXMLString2String (ai[0]), libXMLString2String (ai[3], static_cast<int> (ai[4] - ai[3])));
                        ai += 5;
                    }
                }
                if (URI == nullptr) {
                    thisReader->fCallback_.StartElement (Name{libXMLString2String (localname)}, attrs);
                }
                else {
                    thisReader->fCallback_.StartElement (Name{libXMLString2String (URI), libXMLString2String (localname)}, attrs);
                }
            };
            flibXMLSaxHndler_.endElementNs = [] (void* ctx, const xmlChar* localname, [[maybe_unused]] const xmlChar* prefix, const xmlChar* URI) {
                SAXReader_* thisReader = reinterpret_cast<SAXReader_*> (ctx);
                if (URI == nullptr) {
                    thisReader->fCallback_.EndElement (Name{libXMLString2String (localname)});
                }
                else {
                    thisReader->fCallback_.EndElement (Name{libXMLString2String (URI), libXMLString2String (localname)});
                }
            };
            ;
            flibXMLSaxHndler_.characters = [] (void* ctx, const xmlChar* ch, int len) {
                SAXReader_* thisReader = reinterpret_cast<SAXReader_*> (ctx);
                // unclear how this handles 'continuations' and partial characters - may need a more sophisticated approach
                thisReader->fCallback_.TextInsideElement (String::FromUTF8 (span{reinterpret_cast<const char*> (ch), static_cast<size_t> (len)}));
            };
        }
        SAXReader_ (const SAXReader_&) = delete;
    };
}

namespace {
    class DocRep_ : public ILibXML2DocRep {
    public:
        DocRep_ (const String& name, const optional<URI>& ns)
        {
            AssertNotImplemented ();
        }
        DocRep_ (const Streams::InputStream::Ptr<byte>& in)
        {
            xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt (NULL, NULL, nullptr, 0, "in-stream.xml" /*filename*/);
            Execution::ThrowIfNull (ctxt);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFreeParserCtxt (ctxt); });
            byte                    buf[1024];
            while (auto n = in.Read (span{buf}).size ()) {
                if (xmlParseChunk (ctxt, reinterpret_cast<char*> (buf), static_cast<int> (n), 0)) {
                    xmlParserError (ctxt, "xmlParseChunk"); // todo read up on what this does but trnaslate to throw
                                                            // return 1;
                }
            }
            xmlParseChunk (ctxt, nullptr, 0, 1); // indicate the parsing is finished
            if (not ctxt->wellFormed) {
                Execution::Throw (BadFormatException{"not well formed"sv}); // get good error message and throw that BadFormatException
            }
            fLibRep_ = ctxt->myDoc;
        }
        DocRep_ (const DocRep_& from)
        {
            AssertNotImplemented ();
        }
        ~DocRep_ ()
        {
            AssertNotNull (fLibRep_);
            xmlFreeDoc (fLibRep_);
        }
        virtual xmlDoc* GetLibXMLDocRep () override
        {
            return fLibRep_;
        }
        virtual void Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const override
        {
            TraceContextBumper ctx{"LibXML2::Doc::Write"};
            AssertNotImplemented ();
        }
        virtual Iterable<Node::Ptr> GetChildren () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotImplemented ();
            return {};
        }
        virtual void Validate (const Schema::Ptr& schema) const override
        {
            TraceContextBumper ctx{"LibXML2::Doc::Validate"};
            RequireNotNull (schema);
            Require (schema.GetRep ()->GetProvider () == &XML::Providers::LibXML2::kDefaultProvider);
            xmlSchema* libxml2Schema = dynamic_pointer_cast<ILibXML2SchemaRep> (schema.GetRep ())->GetSchemaLibRep ();
            RequireNotNull (libxml2Schema);
            xmlSchemaValidCtxtPtr   validateCtx = xmlSchemaNewValidCtxt (libxml2Schema);
            [[maybe_unused]] auto&& cleanup     = Execution::Finally ([&] () noexcept { xmlSchemaFreeValidCtxt (validateCtx); });
            struct ValCB_ {
                static void warnFun ([[maybe_unused]] void* ctx, [[maybe_unused]] const char* msg, ...)
                {
                    // ignored for now
                }
                static void errFun (void* ctx, const char* msg, ...)
                {
                    // Keep first error - and guess NarrowSDK2Wide does right charset mapping
                    auto useCtx = reinterpret_cast<ValCB_*> (ctx);
                    if (useCtx->msg.empty ()) {
                        va_list argsList;
                        va_start (argsList, msg);
                        auto b = Characters::CString::FormatV (msg, argsList);
                        va_end (argsList);
                        useCtx->msg = String{"Failed schema validation: "_k + NarrowSDK2Wide (b, Characters::eIgnoreErrors)}.Trim ();
                    }
                }
                String msg;
            };
            ValCB_ validationCB;
            xmlSchemaSetValidErrors (validateCtx, &ValCB_::errFun, &ValCB_::warnFun, &validationCB);
            int r = xmlSchemaValidateDoc (validateCtx, fLibRep_);
            if (r != 0) {
                Assert (not validationCB.msg.empty ()); // guessing we only get validation error if error callback called?
                Execution::Throw (BadFormatException{validationCB.msg});
            }
        }
        xmlDoc*                                                        fLibRep_{nullptr};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
}

/*
 ********************************************************************************
 ******************* Provider::Xerces::libXMLString2String **********************
 ********************************************************************************
 */
String Providers::LibXML2::libXMLString2String (const xmlChar* s, int len)
{
    return String{span{reinterpret_cast<const char*> (s), static_cast<size_t> (len)}};
}
String Providers::LibXML2::libXMLString2String (const xmlChar* t)
{
    return String::FromUTF8 (reinterpret_cast<const char*> (t));
}

/*
 ********************************************************************************
 ***************** Provider::Xerces::Providers::LibXML2::Provider ***************
 ********************************************************************************
 */
Providers::LibXML2::Provider::Provider ()
{
    TraceContextBumper ctx{"LibXML2::Provider::CTOR"};
#if qDebug
    static unsigned int sNProvidersCreated_{0}; // don't create multiple of these - will lead to confusion
    Assert (++sNProvidersCreated_ == 1);
#endif
    LIBXML_TEST_VERSION;
}

Providers::LibXML2::Provider::~Provider ()
{
    TraceContextBumper ctx{"LibXML2::Provider::DTOR"};
    xmlCleanupParser ();
#if qDebug
    xmlMemoryDump ();
#endif
}

shared_ptr<Schema::IRep> Providers::LibXML2::Provider::SchemaFactory (const optional<URI>& targetNamespace, const BLOB& targetNamespaceData,
                                                                      const Sequence<Schema::SourceComponent>& sourceComponents,
                                                                      const NamespaceDefinitionsList&          namespaceDefinitions) const
{
    return make_shared<SchemaRep_> (targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions);
}

shared_ptr<DOM::Document::IRep> Providers::LibXML2::Provider::DocumentFactory (const String& documentElementName, const optional<URI>& ns) const
{
    return make_shared<DocRep_> (documentElementName, ns);
}

shared_ptr<DOM::Document::IRep> Providers::LibXML2::Provider::DocumentFactory (const Streams::InputStream::Ptr<byte>& in,
                                                                               const Schema::Ptr& schemaToValidateAgainstWhileReading) const
{
    auto r = make_shared<DocRep_> (in);
    if (schemaToValidateAgainstWhileReading != nullptr) {
        r->Validate (schemaToValidateAgainstWhileReading);
    }
    return r;
}

void Providers::LibXML2::Provider::SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                                             const Schema::Ptr& schema) const
{
    Streams::InputStream::Ptr<byte>   useInput = in;
    optional<Streams::SeekOffsetType> seek2;
    if (schema != nullptr and callback != nullptr) {
        // at least for now this is needed - cuz we read twice - maybe can fix...
        useInput = Streams::ToSeekableInputStream::New (in);
        seek2    = useInput.GetOffset ();
    }
    if (schema != nullptr) {
        DocRep_ dr{useInput};
        dr.Validate (schema);
    }
    if (callback != nullptr) {
        SAXReader_       handler{*callback};
        xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt (&handler.flibXMLSaxHndler_, &handler, nullptr, 0, nullptr);
        Execution::ThrowIfNull (ctxt);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFreeParserCtxt (ctxt); });
        byte                    buf[1024];
        if (seek2) {
            useInput.Seek (*seek2);
        }
        while (auto n = useInput.Read (span{buf}).size ()) {
            if (xmlParseChunk (ctxt, reinterpret_cast<char*> (buf), static_cast<int> (n), 0)) {
                xmlParserError (ctxt, "xmlParseChunk"); // todo read up on what this does but translate to throw
            }
        }
        xmlParseChunk (ctxt, nullptr, 0, 1);
    }
}

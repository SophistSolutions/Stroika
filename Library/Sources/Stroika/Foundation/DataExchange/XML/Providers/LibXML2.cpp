/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/String.h"
#include "../../../DataExchange/BadFormatException.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/Throw.h"
#include "../../../Memory/Common.h"
#include "../../../Memory/MemoryAllocator.h"

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

        virtual const Providers::ISchemaProvider* GetProvider () const
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
        DocRep_ (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReading)
        {
            xmlParserCtxtPtr  ctxt = xmlCreatePushParserCtxt (NULL, NULL, nullptr, 0, "in-stream.xml" /*filename*/);
            Execution::ThrowIfNull (ctxt);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFreeParserCtxt (ctxt); });
            byte buf[1024];
            while (auto n = in.Read (span{buf}).size ()) {
                if (xmlParseChunk (ctxt, reinterpret_cast<char*> (buf), static_cast<int> (n), 0)) {
                    xmlParserError (ctxt, "xmlParseChunk"); // todo read up on what this does but trnaslate to throw
                                                            // return 1;
                }
            }
            xmlParseChunk (ctxt, nullptr, 0, 1);        // indicate the parsing is finished
            if (not ctxt->wellFormed) {
                Execution::Throw ("");  // get good error message and throw that BadFormatException
            }
            fLibRep_ = ctxt->myDoc;
        }
        DocRep_ (const DocRep_& from)
        {
            AssertNotImplemented ();
        }
        ~DocRep_()
        {
            AssertNotNull (fLibRep_);
            xmlFreeDoc (fLibRep_);
        }
        virtual xmlDoc* GetLibXMLDocRep() override
        {
            return fLibRep_;
        }
        virtual void Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const override
        {
            TraceContextBumper                             ctx{"LibXML2::Doc::Write"};
            AssertNotImplemented ();
        }
        virtual Iterable<Node::Ptr> GetChildren () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotImplemented ();
        }
        virtual void Validate (const Schema::Ptr& schema) const override
        {
            TraceContextBumper                             ctx{"LibXML2::Doc::Validate"};
            AssertNotImplemented ();
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
    return make_shared<DocRep_> (in, schemaToValidateAgainstWhileReading);
}

void Providers::LibXML2::Provider::SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                                             const Schema::Ptr& schema) const
{
    // @todo must clone/copy the stream - read it into ram so can be used twice...
    if (schema != nullptr) {
// @todo THIS MUST VALIDATE if schema != nullptr
// xmlSchemaValidateStream
// // seems avlidation with limx2ml happens wtih DOC, not SAX
// xmlSchemaValidateDoc     (xmlSchemaValidCtxtPtr ctxt,
///                  xmlDocPtr doc)
#if 0
        // https://web.mit.edu/ghudson/dev/nokrb/third/libxml2/doc/html/libxml-xmlschemas.html#xmlSchemaValidateStream
        SAXReader_              handler{callback};
        xmlParserCtxtPtr        ctxt    = xmlCreatePushParserCtxt (&handler.flibXMLSaxHndler_, &handler, nullptr, 0, nullptr);
        Execution::ThrowIfNull (ctxt);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFreeParserCtxt (ctxt); });
        byte                    buf[1024];
        while (auto n = in.Read (span{buf}).size ()) {
            if (xmlParseChunk (ctxt, reinterpret_cast<char*> (buf), static_cast<int> (n), 0)) {
                xmlParserError (ctxt, "xmlParseChunk"); // todo read up on what this does but trnaslate to throw
                                                        // return 1;
            }
        }
        xmlParseChunk (ctxt, nullptr, 0, 1);
#endif
    }
    if (callback != nullptr) {
        SAXReader_              handler{*callback};
        xmlParserCtxtPtr        ctxt    = xmlCreatePushParserCtxt (&handler.flibXMLSaxHndler_, &handler, nullptr, 0, nullptr);
        Execution::ThrowIfNull (ctxt);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFreeParserCtxt (ctxt); });
        byte                    buf[1024];
        while (auto n = in.Read (span{buf}).size ()) {
            if (xmlParseChunk (ctxt, reinterpret_cast<char*> (buf), static_cast<int> (n), 0)) {
                xmlParserError (ctxt, "xmlParseChunk"); // todo read up on what this does but translate to throw
            }
        }
        xmlParseChunk (ctxt, nullptr, 0, 1);
    }
}

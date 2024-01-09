/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/MemoryAllocator.h"
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
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
        static inline atomic<unsigned int> sLiveCnt{0};
#endif
        SchemaRep_ (const Memory::BLOB& schemaData, const Sequence<SourceComponent>& sourceComponents, const NamespaceDefinitionsList& namespaceDefinitions)
            : fTargetNamespace{}
            , fSourceComponents{sourceComponents}
            , fNamespaceDefinitions{namespaceDefinitions}
        {
            xmlSchemaParserCtxt* schemaParseContext =
                xmlSchemaNewMemParserCtxt (reinterpret_cast<const char*> (schemaData.data ()), static_cast<int> (schemaData.size ()));
            fCompiledSchema = xmlSchemaParse (schemaParseContext);
            xmlSchemaFreeParserCtxt (schemaParseContext);
            Execution::ThrowIfNull (fCompiledSchema);
            if (fCompiledSchema->targetNamespace != nullptr) {
                fTargetNamespace = URI{libXMLString2String (fCompiledSchema->targetNamespace)};
            }
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            ++sLiveCnt;
#endif
        }
        SchemaRep_ (const SchemaRep_&) = delete;
        virtual ~SchemaRep_ ()
        {
            xmlSchemaFree (fCompiledSchema);
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            Assert (sLiveCnt > 0);
            --sLiveCnt;
#endif
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
            flibXMLSaxHndler_.startElementNs = [] (void* ctx, const xmlChar* localname, [[maybe_unused]] const xmlChar* prefix, const xmlChar* URI,
                                                   [[maybe_unused]] int nb_namespaces, [[maybe_unused]] const xmlChar** namespaces,
                                                   int nb_attributes, [[maybe_unused]] int nb_defaulted, const xmlChar** attributes) {
                SAXReader_* thisReader = reinterpret_cast<SAXReader_*> (ctx);
                Assert (thisReader->flibXMLSaxHndler_.initialized == XML_SAX2_MAGIC); // assure ctx ptr passed through properly
                Mapping<Name, String> attrs;
                if (attributes != nullptr) {
                    // Crazy way to decode attribute arguments - I would have never guessed --
                    // https://stackoverflow.com/questions/2075894/how-to-get-the-name-and-value-of-attributes-from-xml-when-using-libxml2-sax-pars
                    auto ai = attributes;
                    for (int i = 0; i < nb_attributes; ++i) {
                        // @todo fix must grab namespace, probably from namespaces, but will take some research to figure out how!!!
                        attrs.Add (Name{"", libXMLString2String (ai[0]), Name::eAttribute},
                                   libXMLString2String (ai[3], static_cast<int> (ai[4] - ai[3])));
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
#if qDebug
    bool ValidNewNodeName_ (const String& n)
    {
        if (n.empty ()) {
            return false;
        }
        if (n.find (':') != wstring::npos) { // if triggered, you probably used XPath as arg for CreateElement call!!!
            return false;
        }
        return true;
    }
#endif
}

namespace {
    class NodeRep_ : public ILibXML2NodeRep, Memory::UseBlockAllocationIfAppropriate<NodeRep_> {
    public:
        NodeRep_ (xmlNode* n)
            : fNode_{n}
        {
            RequireNotNull (n);
        }
        virtual bool Equals (const IRep* rhs) const override
        {
            RequireNotNull (fNode_);
            RequireNotNull (rhs);
            return fNode_ == Debug::UncheckedDynamicCast<const NodeRep_*> (rhs)->fNode_;
        }
        virtual Node::Type GetNodeType () const override
        {
            AssertNotNull (fNode_);
            switch (fNode_->type) {
                case XML_ELEMENT_NODE:
                    return Node::eElementNT;
                case XML_ATTRIBUTE_NODE:
                    return Node::eAttributeNT;
                case XML_TEXT_NODE:
                    return Node::eTextNT;
                case XML_COMMENT_NODE:
                    return Node::eCommentNT;
                default:
                    return Node::eOtherNT;
            }
        }
        virtual NameWithNamespace GetName () const override
        {
            AssertNotNull (fNode_);
            Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
            switch (fNode_->type) {
                case XML_ATTRIBUTE_NODE:
                case XML_ELEMENT_NODE: {
                    const xmlChar* ns = fNode_->ns == nullptr ? nullptr : fNode_->ns->href;
                    return NameWithNamespace{ns == nullptr ? optional<URI>{} : URI{libXMLString2String (ns)}, libXMLString2String (fNode_->name)};
                }
                default:
                    AssertNotReached ();
                    return NameWithNamespace{""};
            }
        }
        virtual void SetName (const NameWithNamespace& name) override
        {
            AssertNotNull (fNode_);
#if qDebug
            Require (ValidNewNodeName_ (name.fName));
#endif
            if (name.fNamespace) {
                AssertNotReached (); // pretty sure this is wrong...
                // see genNS2Use_
                // see SetAttribtues - simple now...
                // NOT totally clear, but this seems to be it...
                xmlNodeSetBase (fNode_, BAD_CAST name.fNamespace->As<String> ().AsUTF8 ().c_str ());
            }
            xmlNodeSetName (fNode_, BAD_CAST name.fName.AsUTF8 ().c_str ());
        }
        virtual String GetValue () const override
        {
            AssertNotNull (fNode_);
            auto                    r       = xmlNodeGetContent (fNode_);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFree (r); });
            return libXMLString2String (r);
        }
        virtual void SetValue (const String& v) override
        {
            AssertNotNull (fNode_);
            xmlNodeSetContent (fNode_, BAD_CAST v.AsUTF8 ().c_str ());
        }
        virtual optional<String> GetAttribute (const NameWithNamespace& attrName) const override
        {
            auto r = attrName.fNamespace ? xmlGetNsProp (fNode_, BAD_CAST attrName.fName.AsUTF8 ().c_str (),
                                                         BAD_CAST attrName.fNamespace->As<String> ().AsUTF8 ().c_str ())
                                         : xmlGetProp (fNode_, BAD_CAST attrName.fName.AsUTF8 ().c_str ());
            if (r == nullptr) {
                return nullopt;
            }
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFree (r); });
            return libXMLString2String (r);
        }
        virtual void SetAttribute (const NameWithNamespace& attrName, const optional<String>& v) override
        {
            RequireNotNull (fNode_);
            Require (GetNodeType () == Node::eElementNT);
            if (attrName.fNamespace) {
                // Lookup the argument ns and either add it to this node or use the existing one
                xmlSetNsProp (fNode_, genNS2Use_ (fNode_, *attrName.fNamespace), BAD_CAST attrName.fName.AsUTF8 ().c_str (),
                              v == nullopt ? nullptr : (BAD_CAST v->AsUTF8 ().c_str ()));
            }
            else {
                xmlSetProp (fNode_, BAD_CAST attrName.fName.AsUTF8 ().c_str (), v == nullopt ? nullptr : (BAD_CAST v->AsUTF8 ().c_str ()));
            }
        }
        virtual Node::Ptr InsertElement (const NameWithNamespace& eltName, const Node::Ptr& afterNode) override
        {
#if qDebug
            Require (ValidNewNodeName_ (eltName.fName));
#endif
            Require (afterNode == nullptr or this->GetChildren ().Contains (afterNode));
            xmlNode* newNode =
                xmlNewNode (eltName.fNamespace ? genNS2Use_ (fNode_, *eltName.fNamespace) : nullptr, BAD_CAST eltName.fName.AsUTF8 ().c_str ());
            NodeRep_* afterNodeRep = afterNode == nullptr ? nullptr : Debug::UncheckedDynamicCast<NodeRep_*> (afterNode.GetRep ().get ());
            if (afterNodeRep == nullptr) {
                // unfortunately complicated - no prepend api (just append). Can say xmlAddPrevSibling for first child though which amounts
                // to same thing (unless there is no first child)
                if (fNode_->children == nullptr) {
                    xmlAddChild (fNode_->parent, newNode); // append=prepend
                }
                else {
                    xmlAddPrevSibling (fNode_->children, newNode);
                }
            }
            else {
                xmlAddNextSibling (afterNodeRep->fNode_, newNode);
            }
            return Node::Ptr{Memory::MakeSharedPtr<NodeRep_> (newNode)};
        }
        virtual Node::Ptr AppendElement (const NameWithNamespace& eltName) override
        {
#if qDebug
            Require (ValidNewNodeName_ (eltName.fName));
#endif
            xmlNode* newNode =
                xmlNewNode (eltName.fNamespace ? genNS2Use_ (fNode_, *eltName.fNamespace) : nullptr, BAD_CAST eltName.fName.AsUTF8 ().c_str ());
            xmlAddChild (fNode_, newNode);
            return Node::Ptr{Memory::MakeSharedPtr<NodeRep_> (newNode)};
        }
        virtual void DeleteNode () override
        {
            RequireNotNull (fNode_);
            xmlUnlinkNode (fNode_);
            xmlFreeNode (fNode_);
            fNode_ = nullptr;
        }
        virtual Node::Ptr GetParentNode () const override
        {
            RequireNotNull (fNode_);
            return Node::Ptr{Memory::MakeSharedPtr<NodeRep_> (fNode_->parent)};
        }
        virtual Iterable<Node::Ptr> GetChildren () const override
        {
            RequireNotNull (fNode_);
            // No reference counting possible here because these notes - i THINK - are owned by document, and the linked list not
            // reference counted (elts) - so just count on no changes during iteration
            return Traversal::CreateGenerator<Node::Ptr> ([curChild = fNode_->children] () mutable -> optional<Node::Ptr> {
                if (curChild == nullptr) {
                    return optional<Node::Ptr>{};
                }
                Node::Ptr r = Node::Ptr{Memory::MakeSharedPtr<NodeRep_> (curChild)};
                curChild    = curChild->next;
                return r;
            });
        }
        virtual void Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const override
        {
            xmlBufferPtr            xmlBuf  = xmlBufferCreate ();
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlBufferFree (xmlBuf); });
            if (int dumpRes = xmlNodeDump (xmlBuf, fNode_->doc, fNode_, 0, options.fPrettyPrint); dumpRes == -1) {
                Execution::Throw (Execution::RuntimeErrorException{"failed dumping node to text"});
            }
            const xmlChar* t = xmlBufferContent (xmlBuf);
            AssertNotNull (t);
            to.Write (span{reinterpret_cast<const byte*> (t), static_cast<size_t> (::strlen (reinterpret_cast<const char*> (t)))});
        }
        virtual xmlNode* GetInternalTRep () override
        {
            return fNode_;
        }
        static xmlNsPtr genNS2Use_ (xmlNode* n, const URI& ns)
        {
            xmlNsPtr              ns2Use = xmlSearchNsByHref (n->doc, n, BAD_CAST ns.As<String> ().AsUTF8 ().c_str ());
            basic_string<xmlChar> prefix2Try{BAD_CAST "a"};
            while (ns2Use == nullptr) {
                // Need to hang the namespace declaration someplace? Could do it just on this element (xmlNewNs)
                // Or on the root doc (xmlNewGlobalNs).
                // For now - do on DOC, so we end up with a more terse overall document.
                // Also - sadly - must cons up SOME prefix, which doesn't conflict. No good way I can see todo that, so do a bad way.
                // OK - can do still manually using docs root elt - maybe - but do this way for now... cuz xmlNewGlobalNs deprecated
                ns2Use = xmlNewNs (n, BAD_CAST ns.As<String> ().AsUTF8 ().c_str (), prefix2Try.c_str ());
                if (ns2Use == nullptr) {
                    ++prefix2Try[0]; // if 'a' didn't work, try 'b' // @todo this could use better error handling, but pragmatically probably OK
                }
            }
            return ns2Use;
        }

    private:
        // must carefully think out mem managment here - cuz not ref counted - around as long as owning doc...
        xmlNode* fNode_;
    };
}

namespace {
    struct DocRep_ : ILibXML2DocRep {
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
        static inline atomic<unsigned int> sLiveCnt{0};
#endif
    public:
        DocRep_ (const NameWithNamespace& documentElementName)
        {
            // Roughly based on http://www.xmlsoft.org/examples/io2.c
            xmlDocPtr doc = xmlNewDoc (BAD_CAST "1.0");
            xmlNodePtr n = xmlNewNode (NULL, BAD_CAST documentElementName.fName.AsUTF8 ().c_str ()); // @todo NOT clear what characterset/encoding to use here!
            xmlDocSetRootElement (doc, n);
            // AND not super clear how to create the namespace if needed?
            if (documentElementName.fNamespace) {
                (void)xmlNewNs (n, BAD_CAST documentElementName.fNamespace->As<String> ().AsUTF8 ().c_str (), nullptr); // very unsure of this --LGP 2024-01-05
            }
            fLibRep_ = doc;
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            ++sLiveCnt;
#endif
        }
        DocRep_ (const Streams::InputStream::Ptr<byte>& in)
        {
            xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt (NULL, NULL, nullptr, 0, "in-stream.xml" /*filename*/);
            Execution::ThrowIfNull (ctxt);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFreeParserCtxt (ctxt); });
            byte                    buf[1024];
            while (auto n = in.Read (span{buf}).size ()) {
                if (xmlParseChunk (ctxt, reinterpret_cast<char*> (buf), static_cast<int> (n), 0)) {
                    xmlParserError (ctxt, "xmlParseChunk"); // @todo read up on what this does but trnaslate to throw
                                                            // return 1;
                }
            }
            xmlParseChunk (ctxt, nullptr, 0, 1); // indicate the parsing is finished
            if (not ctxt->wellFormed) {
                Execution::Throw (BadFormatException{"not well formed"sv}); // get good error message and throw that BadFormatException
            }
            fLibRep_ = ctxt->myDoc;
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            ++sLiveCnt;
#endif
        }
        DocRep_ (const DocRep_& from)
        {
            fLibRep_ = xmlCopyDoc (from.fLibRep_, 1);
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            ++sLiveCnt;
#endif
        }
        ~DocRep_ ()
        {
            AssertNotNull (fLibRep_);
            xmlFreeDoc (fLibRep_);
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            Assert (sLiveCnt > 0);
            --sLiveCnt;
#endif
        }
        virtual xmlDoc* GetLibXMLDocRep () override
        {
            return fLibRep_;
        }
        virtual void Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const override
        {
            TraceContextBumper ctx{"LibXML2::Doc::Write"};
            xmlChar*           xmlBuffer{nullptr};
            int                bufferSize{};
            xmlDocDumpFormatMemoryEnc (fLibRep_, &xmlBuffer, &bufferSize, "UTF-8", options.fPrettyPrint);
            Assert (strlen ((char*)xmlBuffer) == static_cast<size_t> (bufferSize)); // misnomer cuz really number of valid not nul-term characters (so actual allocated size must be one more)
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFree (xmlBuffer); });
            to.Write (span{reinterpret_cast<const byte*> (xmlBuffer), static_cast<size_t> (bufferSize)});
        }
        virtual Iterable<Node::Ptr> GetChildren () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            return Traversal::CreateGenerator<Node::Ptr> ([curChild = fLibRep_->children] () mutable -> optional<Node::Ptr> {
                if (curChild == nullptr) {
                    return optional<Node::Ptr>{};
                }
                Node::Ptr r = Node::Ptr{Memory::MakeSharedPtr<NodeRep_> (curChild)};
                curChild    = curChild->next;
                return r;
            });
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
                optional<unsigned int> lineNumber;
                optional<unsigned int> columnNumber;
                optional<uint64_t>     fileOffset;
                if (const xmlError* err = xmlGetLastError ()) {
                    lineNumber = static_cast<unsigned int> (err->line);
                    if (static_cast<unsigned int> (err->int2) != 0) {
                        columnNumber = static_cast<unsigned int> (err->int2);
                    }
                }
                Execution::Throw (BadFormatException{validationCB.msg, lineNumber, columnNumber, fileOffset});
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
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
    Require (SchemaRep_::sLiveCnt == 0); // Check for leaks but better/clearer than memory leaks check below
    Require (DocRep_::sLiveCnt == 0);    // ""
#endif
    xmlCleanupParser ();
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
    xmlMemoryDump ();
#endif
}

shared_ptr<Schema::IRep> Providers::LibXML2::Provider::SchemaFactory (const BLOB& schemaData, const Sequence<Schema::SourceComponent>& sourceComponents,
                                                                      const NamespaceDefinitionsList& namespaceDefinitions) const
{
    return make_shared<SchemaRep_> (schemaData, sourceComponents, namespaceDefinitions);
}

shared_ptr<DOM::Document::IRep> Providers::LibXML2::Provider::DocumentFactory (const NameWithNamespace& documentElementName) const
{
    return make_shared<DocRep_> (documentElementName);
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
        // @todo --- at least for now this is needed - cuz we read twice - maybe can fix...
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
                xmlParserError (ctxt, "xmlParseChunk"); // @todo read up on what this does but translate to throw
            }
        }
        xmlParseChunk (ctxt, nullptr, 0, 1);
    }
}

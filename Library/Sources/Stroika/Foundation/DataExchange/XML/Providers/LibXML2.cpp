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
#include "../../../Memory/BlockAllocated.h"
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
    class NodeRep_ : public Node::IRep, Memory::UseBlockAllocationIfAppropriate<NodeRep_> {
    public:
        NodeRep_ (xmlNode* n)
            : fNode_{n}
        {
            RequireNotNull (n);
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
        virtual optional<URI> GetNamespace () const override
        {
            AssertNotNull (fNode_);
            Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
            switch (fNode_->type) {
                case XML_ELEMENT_NODE: {
                    _xmlEntity* e = reinterpret_cast<_xmlEntity*> (fNode_);
                    return e->URI == nullptr ? optional<URI>{} : libXMLString2String (e->URI);
                }
                case XML_ATTRIBUTE_NODE: {
                    _xmlAttr* e = reinterpret_cast<_xmlAttr*> (fNode_);
                    return e->ns == nullptr ? optional<URI>{} : libXMLString2String (e->ns->href);
                }
                default:
                    AssertNotReached ();
                    return nullopt;
            }
        }
        virtual String GetName () const override
        {
            AssertNotNull (fNode_);
            Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
            switch (fNode_->type) {
                case XML_ELEMENT_NODE: {
                    _xmlEntity* e = reinterpret_cast<_xmlEntity*> (fNode_);
                    return libXMLString2String (e->name);
                }
                case XML_ATTRIBUTE_NODE: {
                    _xmlAttr* a = reinterpret_cast<_xmlAttr*> (fNode_);
                    return libXMLString2String (a->name);
                }
                default:
                    AssertNotReached ();
                    return {};
            }
        }
        virtual void SetName (const String& name) override
        {
            AssertNotNull (fNode_);
#if qDebug
            Require (ValidNewNodeName_ (name));
#endif
            xmlNodeSetName (fNode_, BAD_CAST name.AsUTF8 ().c_str ());	
        }
        virtual String GetValue () const override
        {
            AssertNotNull (fNode_);
            auto r = xmlNodeGetContent (fNode_);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFree (r); });
            return libXMLString2String (r);
        }
        virtual void SetValue (const String& v) override
        {
            AssertNotNull (fNode_);
            xmlNodeSetContent (fNode_, BAD_CAST v.AsUTF8 ().c_str ());
        }
        virtual void SetAttribute (const String& attrName, const String& v) override
        {
            RequireNotNull (fNode_);
            Require (GetNodeType () == Node::eElementNT);
            // @todo handle namespaces on attributes
            xmlNewProp (fNode_, BAD_CAST attrName.AsUTF8 ().c_str (), BAD_CAST v.AsUTF8 ().c_str ());
        }
        virtual bool HasAttribute (const String& attrName, const String* value) const override
        {
            RequireNotNull (fNode_);
            Require (GetNodeType () == Node::eElementNT);
            return xmlHasProp (fNode_, BAD_CAST attrName.AsUTF8 ().c_str ());
        }
        virtual optional<String> GetAttribute (const String& attrName) const override
        {
            // @todo handle ns properly..
            auto                    r       = xmlGetProp (fNode_, BAD_CAST attrName.AsUTF8 ().c_str ());
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { xmlFree (r); });
            return libXMLString2String (r);
        }
        virtual Node::Ptr GetFirstAncestorNodeWithAttribute (const String& attrName) const override
        {
            AssertNotNull (fNode_);
            return nullptr;
#if 0
            START_LIB_EXCEPTION_MAPPER_
            {
                for (DOMNode* p = fNode_; p != nullptr; p = p->getParentNode ()) {
                    if (p->getNodeType () == DOMNode::ELEMENT_NODE) {
                        AssertMember (p, DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                        const DOMElement* elt = reinterpret_cast<const DOMElement*> (p);
                        if (elt->hasAttribute (attrName.As<u16string> ().c_str ())) {
                            return WrapImpl_ (p);
                        }
                    }
                }
                return nullptr;
            }
            END_LIB_EXCEPTION_MAPPER_
#endif
        }
        virtual Node::Ptr InsertChild (const String& name, const optional<URI>& ns, const Node::Ptr& afterNode) override
        {
            return nullptr;
#if 0
#if qDebug
            Require (ValidNewNodeName_ (name));
#endif
            START_LIB_EXCEPTION_MAPPER_
            {
                xercesc::DOMDocument* doc = fNode_->getOwnerDocument ();
                // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                DOMNode* child = doc->createElementNS ((ns == nullopt) ? fNode_->getNamespaceURI () : ns->As<String> ().As<u16string> ().c_str (),
                                                       name.As<u16string> ().c_str ());
                DOMNode* refChildNode = nullptr;
                if (afterNode == nullptr) {
                    // this means PREPEND.
                    // If there is a first element, then insert before it. If no elements, then append is the same thing.
                    refChildNode = fNode_->getFirstChild ();
                }
                else {
                    refChildNode = GetInternalRep_ (GetRep4Node (afterNode).get ())->getNextSibling ();
                }
                DOMNode* childx = fNode_->insertBefore (child, refChildNode);
                ThrowIfNull (childx);
                return WrapImpl_ (childx);
            }
            END_LIB_EXCEPTION_MAPPER_
#endif
        }
        virtual Node::Ptr AppendChild (const String& name, const optional<URI>& ns) override
        {
            return nullptr;
#if 0
#if qDebug
            Require (ValidNewNodeName_ (name));
#endif
            START_LIB_EXCEPTION_MAPPER_
            {
                xercesc::DOMDocument* doc = fNode_->getOwnerDocument ();
                DOMNode*              child{};
                if (ns) {
                    u16string namespaceURI = ns->As<String> ().As<u16string> ();
                    child                  = doc->createElementNS (namespaceURI.c_str (), name.As<u16string> ().c_str ());
                }
                else {
                    const XMLCh* namespaceURI = fNode_->getNamespaceURI ();
                    child                     = doc->createElementNS (namespaceURI, name.As<u16string> ().c_str ());
                }
                DOMNode* childx = fNode_->appendChild (child);
                ThrowIfNull (childx);
                return WrapImpl_ (childx);
            }
            END_LIB_EXCEPTION_MAPPER_
#endif
        }
        virtual void DeleteNode () override
        {
#if 0
            START_LIB_EXCEPTION_MAPPER_
            {
                DOMNode* selNode = fNode_;
                ThrowIfNull (selNode);
                DOMNode* parentNode = selNode->getParentNode ();
                if (parentNode == nullptr) {
                    // This happens if the selected node is an attribute
                    if (fNode_ != nullptr) {
                        const XMLCh* ln = selNode->getNodeName ();
                        AssertNotNull (ln);
                        DOMElement* de = dynamic_cast<DOMElement*> (fNode_);
                        de->removeAttribute (ln);
                    }
                }
                else {
                    (void)parentNode->removeChild (selNode);
                }
            }
            END_LIB_EXCEPTION_MAPPER_
#endif
        }
        virtual Node::Ptr ReplaceNode () override
        {
            return nullptr;
#if 0
            RequireNotNull (fNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                xercesc::DOMDocument* doc = fNode_->getOwnerDocument ();
                ThrowIfNull (doc);
                DOMNode* selNode = fNode_;
                ThrowIfNull (selNode); // perhaps this should be an assertion?
                DOMNode* parentNode = selNode->getParentNode ();
                ThrowIfNull (parentNode);
                DOMElement* n = doc->createElementNS (selNode->getNamespaceURI (), selNode->getNodeName ());
                (void)parentNode->replaceChild (n, selNode);
                return WrapImpl_ (n);
            }
            END_LIB_EXCEPTION_MAPPER_
#endif
        }
        virtual Node::Ptr GetParentNode () const override
        {
            RequireNotNull (fNode_);
            return Node::Ptr{Memory::MakeSharedPtr<NodeRep_> (fNode_->parent)};
        }
        virtual Iterable<Node::Ptr> GetChildren () const override
        {
            AssertNotNull (fNode_);
            return Iterable<Node::Ptr>{};
#if 0
            START_LIB_EXCEPTION_MAPPER_
            {
                return Traversal::CreateGenerator<Node::Ptr> (
                    [sni = SubNodeIterator_{Memory::MakeSharedPtr<SubNodeIteratorOver_SiblingList_Rep_> (fNode_)}] () mutable -> optional<Node::Ptr> {
                        if (sni.IsAtEnd ()) {
                            return optional<Node::Ptr>{};
                        }
                        Node::Ptr r = *sni;
                        ++sni;
                        return r;
                    });
            }
            END_LIB_EXCEPTION_MAPPER_
#endif
        }
        virtual Node::Ptr GetChildNodeByID (const String& id) const override
        {
            return nullptr;
            AssertNotNull (fNode_);
#if 0
            START_LIB_EXCEPTION_MAPPER_
            {
                for (DOMNode* i = fNode_->getFirstChild (); i != nullptr; i = i->getNextSibling ()) {
                    if (i->getNodeType () == DOMNode::ELEMENT_NODE) {
                        AssertMember (i, DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                        DOMElement*  elt = reinterpret_cast<DOMElement*> (i);
                        const XMLCh* s   = elt->getAttribute (u"id");
                        AssertNotNull (s);
                        if (CString::Equals (s, id.As<u16string> ().c_str ())) {
                            return WrapImpl_ (i);
                        }
                    }
                }
                return nullptr;
            }
            END_LIB_EXCEPTION_MAPPER_
#endif
        }
        virtual void* GetInternalTRep () override
        {
            return fNode_;
        }

    private:
#if 0
        nonvirtual XercesDocRep_& GetAssociatedDoc_ () const
        {
            AssertNotNull (fNode_);
            xercesc::DOMDocument* doc = fNode_->getOwnerDocument ();
            AssertNotNull (doc);
            void* docData = doc->getUserData (kXerces2XMLDBDocumentKey_);
            AssertNotNull (docData);
            return *reinterpret_cast<XercesDocRep_*> (docData);
        }
#endif
        // must carefully think out mem managment here - cuz not ref counted - around as long as owning doc...
        xmlNode* fNode_;
    };
}

namespace {
    class DocRep_ : public ILibXML2DocRep {
    public:
        DocRep_ (const String& name, const optional<URI>& ns)
        {
            // Roughly based on http://www.xmlsoft.org/examples/io2.c
            xmlDocPtr  doc = xmlNewDoc (BAD_CAST "1.0");
            xmlNodePtr n   = xmlNewNode (NULL, BAD_CAST name.AsUTF8 ().c_str ()); // @todo NOT clear what characterset/encoding to use here!
            xmlDocSetRootElement (doc, n);
            // AND not super clear how to create the namespace if needed?
            if (ns) {
                (void)xmlNewNs (n, BAD_CAST ns->As<String> ().AsUTF8 ().c_str (), nullptr); // very unsure of this --LGP 2024-01-05
            }
            fLibRep_ = doc;
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
                xmlParserError (ctxt, "xmlParseChunk"); // todo read up on what this does but translate to throw
            }
        }
        xmlParseChunk (ctxt, nullptr, 0, 1);
    }
}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <fstream>

#include <xercesc/validators/schema/identity/XPathException.hpp>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/IO/FileSystem/TemporaryFile.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/MemoryAllocator.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Streams/TextToByteReader.h"
#include "Stroika/Foundation/Traversal/Generator.h"

#include "Xerces.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::DOM;
using namespace Stroika::Foundation::DataExchange::XML::Schema;
using namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Streams;

using std::byte;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

static_assert (qHasFeature_Xerces, "Don't compile this file if qHasFeature_Xerces not set");

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::DataExchange::XML, qHasFeature_Xerces, qHasFeature_Xerces);

namespace {
    // From https://www.w3.org/TR/xml-names/
    //      In a namespace declaration, the URI reference is the normalized value of the attribute, so replacement of XML
    //      character and entity references has already been done before any comparison.
    //
    //  Not 100% sure, but I think that means decode %x stuff too (at least that fixes bug I'm encountering with ASTM-CCR files)
    //      --LGP 2024-01-31
    constexpr auto kUseURIEncodingFlag_ = URI::StringPCTEncodedFlag::eDecoded;
}

/*
 */
#define START_LIB_EXCEPTION_MAPPER_ try {
#define END_LIB_EXCEPTION_MAPPER_                                                                                                          \
    }                                                                                                                                      \
    catch (const xercesc_3_2::OutOfMemoryException&)                                                                                       \
    {                                                                                                                                      \
        Execution::Throw (bad_alloc{}, "xerces OutOfMemoryException - throwing bad_alloc");                                                \
    }                                                                                                                                      \
    catch (...)                                                                                                                            \
    {                                                                                                                                      \
        Execution::ReThrow ();                                                                                                             \
    }

#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
/*
 *  A helpful class to isolate Xerces (etc) memory management calls. Could be the basis
 *  of future performance/memory optimizations, but for now, just a helpful debugging/tracking
 *  class.
 */
struct Provider::MyXercesMemMgr_ : public MemoryManager {
public:
    MyXercesMemMgr_ ()
        : fAllocator{fBaseAllocator}
    {
    }
    ~MyXercesMemMgr_ ()
    {
#if qDebug
        auto snapshot = fAllocator.GetSnapshot ();
        Assert (snapshot.fAllocations.empty ()); // else we have a memory leak which can be perused here in the debugger
#endif
    }

public:
    Memory::SimpleAllocator_CallLIBCNewDelete             fBaseAllocator;
    Memory::LeakTrackingGeneralPurposeAllocator           fAllocator;
    mutex                                                 fLastSnapshot_CritSection;
    Memory::LeakTrackingGeneralPurposeAllocator::Snapshot fLastSnapshot;

public:
    void DUMPCurMemStats ()
    {
        TraceContextBumper          ctx{"Xerces::MyXercesMemMgr_::DUMPCurMemStats"};
        [[maybe_unused]] lock_guard critSec{fLastSnapshot_CritSection};
        fAllocator.DUMPCurMemStats (fLastSnapshot);
        // now copy current map to prev for next time this gets called
        fLastSnapshot = fAllocator.GetSnapshot ();
    }

public:
    virtual MemoryManager* getExceptionMemoryManager () override
    {
        return this;
    }
    virtual void* allocate (XMLSize_t size) override
    {
        try {
            return fAllocator.Allocate (size);
        }
        catch (...) {
            // NB: use throw not Exception::Throw () since that requires its a subclass of exception (or SilentException)
            throw (OutOfMemoryException{}); // quirk cuz this is the class Xerces expects and catches internally (why not bad_alloc?) - sigh...
        }
    }
    virtual void deallocate (void* p) override
    {
        if (p != nullptr) {
            return fAllocator.Deallocate (p);
        }
    }
};
#endif

namespace {
    struct MySchemaResolver_ : public XMLEntityResolver {
    private:
        Resource::ResolverPtr fResolver_;

    public:
        MySchemaResolver_ (const Resource::ResolverPtr& resolver)
            : fResolver_{resolver}
        {
        }
        virtual InputSource* resolveEntity (XMLResourceIdentifier* resourceIdentifier) override
        {
            // @todo consider exposting this API outside the module, and/or providing option to wget missing namespaces, or have option for where to fetch from?
            TraceContextBumper ctx{"Xerces::{}::MySchemaResolver_::resolveEntity"};
            RequireNotNull (resourceIdentifier);

            if (fResolver_ != nullptr) {
                if (auto o = fResolver_.Lookup (Resource::Name{.fNamespace = xercesString2String (resourceIdentifier->getNameSpace ()),
                                                               .fPublicID  = xercesString2String (resourceIdentifier->getPublicId ()),
                                                               .fSystemID  = xercesString2String (resourceIdentifier->getSystemId ())})) {
                    return mkMemInputSrc_ (o->fData);
                }
            }
            return nullptr;
        }

    private:
        static InputSource* mkMemInputSrc_ (const Memory::BLOB& schemaData)
        {
            if (schemaData.empty ()) [[unlikely]] {
                // not sure this is useful case? Should assert/throw?
                return new MemBufInputSource{nullptr, 0, "", true};
            }
            else {
                XMLByte* useBuf = new XMLByte[schemaData.GetSize ()];
                memcpy (useBuf, schemaData.begin (), schemaData.GetSize ());
                return new MemBufInputSource{useBuf, schemaData.GetSize (), "", true};
            }
        }
    };
}

namespace {
    void SetupCommonParserFeatures_ (SAX2XMLReader& reader)
    {
        reader.setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
        reader.setFeature (XMLUni::fgXercesDynamic, false);
        reader.setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, false); // false:  * *Do not report attributes used for Namespace declarations, and optionally do not report original prefixed names
    }
    void SetupCommonParserFeatures_ (SAX2XMLReader& reader, bool validatingWithSchema)
    {
        reader.setFeature (XMLUni::fgXercesSchema, validatingWithSchema);
        reader.setFeature (XMLUni::fgSAX2CoreValidation, validatingWithSchema);

        // The purpose of this maybe to find errors with the schema itself, in which case,
        // we shouldn't bother (esp for release builds)
        //  (leave for now til we performance test)
        //      -- LGP 2007-06-21
        reader.setFeature (XMLUni::fgXercesSchemaFullChecking, validatingWithSchema);
        reader.setFeature (XMLUni::fgXercesUseCachedGrammarInParse, validatingWithSchema);
        reader.setFeature (XMLUni::fgXercesIdentityConstraintChecking, validatingWithSchema);

        // we only want to use loaded schemas - don't save any more into the grammar cache, since that
        // is global/shared.
        reader.setFeature (XMLUni::fgXercesCacheGrammarFromParse, false);
    }
}

namespace {
    struct Map2StroikaExceptionsErrorReporter_ : public XMLErrorReporter, public ErrorHandler {
    public:
        virtual void error ([[maybe_unused]] const unsigned int errCode, [[maybe_unused]] const XMLCh* const errDomain,
                            [[maybe_unused]] const ErrTypes type, const XMLCh* const errorText, [[maybe_unused]] const XMLCh* const systemId,
                            [[maybe_unused]] const XMLCh* const publicId, const XMLFileLoc lineNum, const XMLFileLoc colNum) override
        {
            Execution::Throw (BadFormatException{errorText, static_cast<unsigned int> (lineNum), static_cast<unsigned int> (colNum), 0});
        }
        virtual void resetErrors () override
        {
        }
        virtual void warning ([[maybe_unused]] const SAXParseException& exc) override
        {
            // ignore
        }
        virtual void error (const SAXParseException& exc) override
        {
            Execution::Throw (BadFormatException{exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()),
                                                 static_cast<unsigned int> (exc.getColumnNumber ()), 0});
        }
        virtual void fatalError (const SAXParseException& exc) override
        {
            Execution::Throw (BadFormatException{exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()),
                                                 static_cast<unsigned int> (exc.getColumnNumber ()), 0});
        }
    };
}

namespace {
    struct SchemaRep_ : IXercesSchemaRep {
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
        static inline atomic<unsigned int> sLiveCnt{0};
#endif
        SchemaRep_ (const Streams::InputStream::Ptr<byte>& schemaData, const Resource::ResolverPtr& resolver)
            : fTargetNamespace{}
            , fResolver{resolver}
            , fSchemaData{schemaData.ReadAll ()}
        {
            AssertNotNull (XMLPlatformUtils::fgMemoryManager);
            XMLGrammarPoolImpl* grammarPool = new (XMLPlatformUtils::fgMemoryManager) XMLGrammarPoolImpl{XMLPlatformUtils::fgMemoryManager};
            try {
                Require (not fSchemaData.empty ());
                MemBufInputSource mis{reinterpret_cast<const XMLByte*> (fSchemaData.begin ()), fSchemaData.GetSize (), u""};

                MySchemaResolver_ mySchemaResolver{resolver};
                // Directly construct SAX2XMLReaderImpl so we can use XMLEntityResolver - which passes along namespace (regular
                // EntityResolve just passes systemID
                //      shared_ptr<SAX2XMLReader>   reader = shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, grammarPool));
                //
                shared_ptr<SAX2XMLReaderImpl> reader = shared_ptr<SAX2XMLReaderImpl> (
                    new (XMLPlatformUtils::fgMemoryManager) SAX2XMLReaderImpl{XMLPlatformUtils::fgMemoryManager, grammarPool});
                reader->setXMLEntityResolver (&mySchemaResolver);

                SetupCommonParserFeatures_ (*reader, true);

                // Reset fgXercesCacheGrammarFromParse to TRUE so we actually load the XSD here
                reader->setFeature (XMLUni::fgXercesCacheGrammarFromParse, true);
                reader->setErrorHandler (&fErrorReporter_);
                xercesc_3_2::Grammar* g = reader->loadGrammar (mis, Grammar::SchemaGrammarType, true);
                AssertNotNull (g);
                const XMLCh* ts = g->getTargetNamespace ();
                if (ts and *ts) {
                    fTargetNamespace = URI{xercesString2String (ts)};
                }
            }
            catch (...) {
                delete grammarPool;
                Execution::ReThrow ();
            }
            fCachedGrammarPool = grammarPool;
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            ++sLiveCnt;
#endif
        }
        SchemaRep_ (const SchemaRep_&) = delete;
        virtual ~SchemaRep_ ()
        {
            delete fCachedGrammarPool;
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            Assert (sLiveCnt > 0);
            --sLiveCnt;
#endif
        }
        optional<URI>                       fTargetNamespace;
        Resource::ResolverPtr               fResolver;
        Memory::BLOB                        fSchemaData;
        xercesc_3_2::XMLGrammarPool*        fCachedGrammarPool{nullptr};
        Map2StroikaExceptionsErrorReporter_ fErrorReporter_;

        virtual const Providers::ISchemaProvider* GetProvider () const override
        {
            return &XML::Providers::Xerces::kDefaultProvider;
        }
        virtual optional<URI> GetTargetNamespace () const override
        {
            return fTargetNamespace;
        }
        virtual Memory::BLOB GetData () override
        {
            return fSchemaData;
        }
        // not super useful, except if you want to clone
        virtual Resource::ResolverPtr GetResolver () override
        {
            return fResolver;
        }
        virtual xercesc_3_2::XMLGrammarPool* GetCachedGrammarPool () override
        {
            return fCachedGrammarPool;
        }
    };
}

namespace {
    class StdIStream_InputSource_ : public InputSource {
    protected:
        class StdIStream_InputStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream {
        public:
            StdIStream_InputStream (InputStream::Ptr<byte> in)
                : fSource{in}
            {
            }
            ~StdIStream_InputStream () = default;

        public:
            virtual XMLFilePos curPos () const override
            {
                return fSource.GetOffset ();
            }
            virtual XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
            {
                return fSource.Read (span{reinterpret_cast<byte*> (toFill), maxToRead}).size ();
            }
            virtual const XMLCh* getContentType () const override
            {
                return nullptr;
            }

        protected:
            InputStream::Ptr<byte> fSource;
        };

    public:
        StdIStream_InputSource_ (InputStream::Ptr<byte> in, const XMLCh* const bufId = nullptr)
            : InputSource{bufId}
            , fSource{in}
        {
        }
        virtual BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) StdIStream_InputStream{fSource};
        }

    protected:
        InputStream::Ptr<byte> fSource;
    };
}

namespace {
    class SAX2PrintHandlers_ : public DefaultHandler {
    private:
        StructuredStreamEvents::IConsumer* fCallback_;

    public:
        SAX2PrintHandlers_ (StructuredStreamEvents::IConsumer* callback)
            : fCallback_{callback}
        {
        }

    public:
        virtual void startDocument () override
        {
            if (fCallback_ != nullptr) {
                fCallback_->StartDocument ();
            }
        }
        virtual void endDocument () override
        {
            if (fCallback_ != nullptr) {
                fCallback_->EndDocument ();
            }
        }
        virtual void startElement (const XMLCh* const uri, const XMLCh* const localName, const XMLCh* const /*qname*/, const Attributes& attributes) override
        {
            Require (uri != nullptr);
            Require (localName != nullptr);
            if (fCallback_ != nullptr) {
                using Name = StructuredStreamEvents::Name;
                Mapping<Name, String> useAttrs;
                size_t                attributesLen = attributes.getLength ();
                for (XMLSize_t i = 0; i < attributesLen; ++i) {
                    Name attrName{xercesString2String (attributes.getURI (i)), xercesString2String (attributes.getLocalName (i)), Name::eAttribute};
                    useAttrs.Add (attrName, xercesString2String (attributes.getValue (i)));
                }
                fCallback_->StartElement (Name{xercesString2String (uri), xercesString2String (localName)}, useAttrs);
            }
        }
        virtual void endElement (const XMLCh* const uri, const XMLCh* const localName, [[maybe_unused]] const XMLCh* const qname) override
        {
            Require (uri != nullptr);
            Require (localName != nullptr);
            Require (qname != nullptr);
            if (fCallback_ != nullptr) {
                fCallback_->EndElement (StructuredStreamEvents::Name{xercesString2String (uri), xercesString2String (localName)});
            }
        }
        virtual void characters (const XMLCh* const chars, const XMLSize_t length) override
        {
            Require (chars != nullptr);
            Require (length != 0);
            if (fCallback_ != nullptr) {
                fCallback_->TextInsideElement (xercesString2String (chars, chars + length));
            }
        }
    };
}

namespace {
    /*
     *  Short lifetime. Don't save these iterator objects. Just use them to enumerate a collection and then let them
     *  go. They (could) become invalid after a call to update the database.
     */
    class SubNodeIterator_ {
    public:
        class Rep {
        public:
            Rep ()          = default;
            virtual ~Rep () = default;

        public:
            virtual bool      IsAtEnd () const   = 0;
            virtual void      Next ()            = 0;
            virtual Node::Ptr Current () const   = 0;
            virtual size_t    GetLength () const = 0;
        };
        explicit SubNodeIterator_ (const shared_ptr<Rep>& from)
            : fRep{from}
        {
        }

    public:
        nonvirtual bool NotDone () const
        {
            return not fRep->IsAtEnd ();
        }
        nonvirtual bool IsAtEnd () const
        {
            return fRep->IsAtEnd ();
        }
        nonvirtual void Next ()
        {
            fRep->Next ();
        }
        nonvirtual Node::Ptr Current () const
        {
            return fRep->Current ();
        }
        nonvirtual size_t GetLength () const
        {
            return fRep->GetLength ();
        }
        nonvirtual void operator++ ()
        {
            Next ();
        }
        nonvirtual void operator++ (int)
        {
            Next ();
        }
        nonvirtual Node::Ptr operator* () const
        {
            return Current ();
        }

    protected:
        shared_ptr<Rep> fRep;
    };
}

namespace {
    template <class TYPE>
    class AutoRelease_ {
    public:
        AutoRelease_ (TYPE* p)
            : p_ (p)
        {
        }
        AutoRelease_ (const AutoRelease_<TYPE>&)                  = delete;
        AutoRelease_<TYPE>& operator= (const AutoRelease_<TYPE>&) = delete;
        ~AutoRelease_ ()
        {
            if (p_ != 0)
                p_->release ();
        }

        TYPE& operator* () const
        {
            return *p_;
        }
        TYPE* operator->() const
        {
            return p_;
        }
        operator TYPE* () const
        {
            return p_;
        }
        TYPE* get () const
        {
            return p_;
        }
        TYPE* adopt ()
        {
            TYPE* tmp = p_;
            p_        = 0;
            return tmp;
        }
        TYPE* swap (TYPE* p)
        {
            TYPE* tmp = p_;
            p_        = p;
            return tmp;
        }
        void set (TYPE* p)
        {
            if (p_ != 0)
                p_->release ();
            p_ = p;
        }

    private:
        TYPE* p_;
    };
}

namespace {
    DOMImplementation& GetDOMIMPL_ ()
    {
        static constexpr XMLCh kDOMImplFeatureDeclaration_[] = u"Core";
        // safe to save in a static var? -- LGP 2007-05-20
        // from perusing implementation - this appears safe to cache and re-use in differnt threads
        static DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation (kDOMImplFeatureDeclaration_);
        AssertNotNull (impl);
        return *impl;
    }

    constexpr bool qDumpXMLOnValidationError_ = qDebug;

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
    void DoWrite2Stream_ (xercesc::DOMNode* node2Write, const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options)
    {
        AutoRelease_<DOMLSOutput> theOutputDesc = GetDOMIMPL_ ().createLSOutput ();
        theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
        AutoRelease_<DOMLSSerializer> writer = GetDOMIMPL_ ().createLSSerializer ();
        DOMConfiguration*             dc     = writer->getDomConfig ();
        dc->setParameter (XMLUni::fgDOMWRTFormatPrettyPrint, options.fPrettyPrint);
        dc->setParameter (XMLUni::fgDOMWRTBOM, true);
        class myOutputter : public XMLFormatTarget {
        public:
            Streams::OutputStream::Ptr<byte> fOut;
            myOutputter (const Streams::OutputStream::Ptr<byte>& to)
                : fOut{to}
            {
            }
            virtual void writeChars (const XMLByte* const toWrite, const XMLSize_t count, [[maybe_unused]] XMLFormatter* const formatter) override
            {
                fOut.Write (span<const byte>{reinterpret_cast<const byte*> (toWrite), count});
            }
            virtual void flush () override
            {
                fOut.Flush ();
            }
        };
        myOutputter dest{to};
        theOutputDesc->setByteStream (&dest);
        writer->write (node2Write, theOutputDesc);
    }
    // Currently unused but maybe needed again if we support 'moving' nodes from one doc to another
    DOMNode* RecursivelySetNamespace_ (DOMNode* n, const XMLCh* namespaceURI)
    {
        RequireNotNull (n);
        // namespaceURI CAN be nullptr
        switch (n->getNodeType ()) {
            case DOMNode::ELEMENT_NODE: {
                xercesc::DOMDocument* doc = n->getOwnerDocument ();
                AssertNotNull (doc);
                n = doc->renameNode (n, namespaceURI, n->getNodeName ());
                ThrowIfNull (n);
                for (DOMNode* child = n->getFirstChild (); child != nullptr; child = child->getNextSibling ()) {
                    child = RecursivelySetNamespace_ (child, namespaceURI);
                }
            } break;
        }
        return n;
    }
    constexpr XMLCh* kXerces2XMLDBDocumentKey_ = nullptr; // just a unique key to lookup our doc object from the xerces doc object.
        // Could use real str, then xerces does strcmp() - but this appears slightly faster
        // so long as no conflict....
    String GetTextForDOMNode_ (const DOMNode* node)
    {
        RequireNotNull (node);
        if (node->getNodeType () == DOMNode::COMMENT_NODE) {
            // The below hack doesn't seem to work for comment nodes - at least in one case - they had
            // no children nodes so just returned empty string...
            //
            // The above comemnt about comment-nodes not working is old, and undated. Revisit this at some point. Though this backup mode isn't so bad.
            goto BackupMode;
        }

        {
            // try quick impl if all children are textnodes
            //
            // This trick is from:
            //      http://www.codesynthesis.com/~boris/blog/category/xerces-c/
            //      2007-06-13, because default impl appears to allocate strings and never frees (til owning doc freed)
            //      for getTextContent calls...
            //
            using xercesc::DOMNode;
            using xercesc::DOMText;
            StringBuilder r;
            for (DOMNode* n = node->getFirstChild (); n != nullptr; n = n->getNextSibling ()) {
                switch (n->getNodeType ()) {
                    case DOMNode::TEXT_NODE:
                    case DOMNode::CDATA_SECTION_NODE: {
                        DOMText* t (static_cast<DOMText*> (n));
                        // Note - we don't do the usual 'reserve' trick here because the string is generally made up of a single text node
                        r += t->getData ();
                        break;
                    }
                    case DOMNode::ELEMENT_NODE: {
                        goto BackupMode;
                    }
                }
            }
            return r.str ();
        }
    BackupMode:
        /*
         * Note that this  is SOMETHING OF A (temporary) memory leak. Xerces does free the memory when the document is freed.
         */
        DbgTrace ("WARNING: GetTextForDOMNode_::BackupMode used"_f);
        return node->getTextContent ();
    }

    Node::Ptr    WrapXercesNodeInStroikaNode_ (DOMNode* n);
    Element::Ptr WrapXercesNodeInStroikaNode_ (DOMElement* n);
}

namespace {
    class SubNodeIteratorOver_SiblingList_Rep_ : public SubNodeIterator_::Rep,
                                                 Memory::UseBlockAllocationIfAppropriate<SubNodeIteratorOver_SiblingList_Rep_> {
    public:
        // Called iterates over CHILDREN of given parentNode
        SubNodeIteratorOver_SiblingList_Rep_ (DOMNode* nodeParent)
            : fParentNode{nodeParent}
            , fCachedMainListLen{static_cast<size_t> (-1)}
        {
            RequireNotNull (nodeParent);
            START_LIB_EXCEPTION_MAPPER_
            {
                fCurNode_ = nodeParent->getFirstChild ();
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual bool IsAtEnd () const override
        {
            return fCurNode_ == nullptr;
        }
        virtual void Next () override
        {
            Require (not IsAtEnd ());
            AssertNotNull (fCurNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                fCurNode_ = fCurNode_->getNextSibling ();
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual Node::Ptr Current () const override
        {
            return WrapXercesNodeInStroikaNode_ (fCurNode_);
        }
        virtual size_t GetLength () const override
        {
            if (fCachedMainListLen == static_cast<size_t> (-1)) {
                size_t n = 0;
                START_LIB_EXCEPTION_MAPPER_
                {
                    for (DOMNode* i = fParentNode->getFirstChild (); i != nullptr; (i = i->getNextSibling ()), ++n)
                        ;
                }
                END_LIB_EXCEPTION_MAPPER_
                fCachedMainListLen = n;
            }
            return fCachedMainListLen;
        }

    private:
        DOMNode*       fParentNode{nullptr};
        DOMNode*       fCurNode_{nullptr};
        mutable size_t fCachedMainListLen{};
    };
}

namespace {
    struct NodeRep_ : IXercesNodeRep, Memory::UseBlockAllocationIfAppropriate<NodeRep_> {
        NodeRep_ (DOMNode* n)
            : fNode_{n}
        {
            RequireNotNull (n);
        }
        virtual const Providers::IDOMProvider* GetProvider () const override
        {
            return &Providers::Xerces::kDefaultProvider;
        }
        virtual bool Equals (const IRep* rhs) const override
        {
            RequireNotNull (fNode_);
            RequireNotNull (rhs);
            return fNode_ == dynamic_cast<const NodeRep_*> (rhs)->fNode_;
        }
        virtual Node::Type GetNodeType () const override
        {
            AssertNotNull (fNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                switch (fNode_->getNodeType ()) {
                    case DOMNode::ELEMENT_NODE:
                        return Node::eElementNT;
                    case DOMNode::ATTRIBUTE_NODE:
                        return Node::eAttributeNT;
                    case DOMNode::TEXT_NODE:
                        return Node::eTextNT;
                    case DOMNode::COMMENT_NODE:
                        return Node::eCommentNT;
                    default:
                        return Node::eOtherNT;
                }
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual NameWithNamespace GetName () const override
        {
            AssertNotNull (fNode_);
            Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
            START_LIB_EXCEPTION_MAPPER_
            {
                AssertNotNull (fNode_->getNodeName ());
                const XMLCh* n = fNode_->getNamespaceURI ();
                return NameWithNamespace{n == nullptr ? optional<URI>{} : URI{xercesString2String (n)}, fNode_->getNodeName ()};
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual void SetName (const NameWithNamespace& name) override
        {
            AssertNotNull (fNode_);
#if qDebug
            Require (ValidNewNodeName_ (name.fName));
#endif
            START_LIB_EXCEPTION_MAPPER_
            {
                xercesc::DOMDocument* doc = fNode_->getOwnerDocument ();
                AssertNotNull (doc);
                fNode_ = doc->renameNode (
                    fNode_, name.fNamespace == nullopt ? nullptr : name.fNamespace->As<String> (kUseURIEncodingFlag_).As<u16string> ().c_str (),
                    name.fName.As<u16string> ().c_str ());
                AssertNotNull (fNode_);
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual String GetValue () const override
        {
            AssertNotNull (fNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                return GetTextForDOMNode_ (fNode_);
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual void SetValue (const String& v) override
        {
            AssertNotNull (fNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                fNode_->setTextContent (v.empty () ? nullptr : v.As<u16string> ().c_str ());
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual void DeleteNode () override
        {
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
        }
        virtual Node::Ptr GetParentNode () const override
        {
            AssertNotNull (fNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                auto p = fNode_->getParentNode ();
                return p == nullptr ? nullptr : WrapXercesNodeInStroikaNode_ (p);
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual void Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const override
        {
            START_LIB_EXCEPTION_MAPPER_
            {
                DoWrite2Stream_ (fNode_, to, options);
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual xercesc_3_2::DOMNode* GetInternalTRep () override
        {
            return fNode_;
        }
        // must carefully think out mem managment here - cuz not ref counted - around as long as owning doc...
        DOMNode* fNode_;
    };
}

namespace {
    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    struct ElementRep_ : Element::IRep, Memory::InheritAndUseBlockAllocationIfAppropriate<ElementRep_, NodeRep_> {
        using inherited = Memory::InheritAndUseBlockAllocationIfAppropriate<ElementRep_, NodeRep_>;
        ElementRep_ (DOMNode* n)
            : inherited{n}
        {
            RequireNotNull (n);
            Require (n->getNodeType () == DOMNode::ELEMENT_NODE);
        }
        virtual Node::Type GetNodeType () const override
        {
            AssertNotNull (fNode_);
            Assert (fNode_->getNodeType () == DOMNode::ELEMENT_NODE);
            return Node::eElementNT;
        }
        virtual optional<String> GetAttribute (const NameWithNamespace& attrName) const override
        {
            AssertNotNull (fNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                if (fNode_->getNodeType () == DOMNode::ELEMENT_NODE) {
                    DOMElement*  elt = Debug::UncheckedDynamicCast<DOMElement*> (fNode_);
                    const XMLCh* s   = attrName.fNamespace
                                           ? elt->getAttributeNS (attrName.fNamespace->As<String> (kUseURIEncodingFlag_).As<u16string> ().c_str (),
                                                                  attrName.fName.As<u16string> ().c_str ())
                                           : elt->getAttribute (attrName.fName.As<u16string> ().c_str ());
                    AssertNotNull (s);
                    if (*s != '\0') {
                        return s;
                    }
                }
                return nullopt;
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual void SetAttribute (const NameWithNamespace& attrName, const optional<String>& v) override
        {
            Require (GetNodeType () == Node::eElementNT);
            AssertNotNull (fNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                DOMElement* element = dynamic_cast<DOMElement*> (fNode_);
                ThrowIfNull (element);
                /* 
                     * SINCE STROIKA V3.0d5 - if no namespace given on attrName, we use nullptr as the namespace (not do any inheritance)
                     * attributes usually have no namespace!!!!
                     */
                if (v) {
                    /*
                     * For reasons that elude maybe (maybe because it was standard for XML early on)
                     * all my attributes are free of namespaces. So why use setAttributeNS? Because otherwise
                     * the XQilla code fails to match on the attribute names at all in its XPath stuff.
                     * Considered copying the namespace from the parent element (fNode_->getNamespaceURI()),
                     * but XQilla didnt like that either (maybe then I needed M: on xpath).
                     * A different subclass object of DOMAttrNode is created - one that doesnt have a getLocalName,
                     * or something like that. Anyhow - this appears to do the right thing for now...
                     *      -- LGP 2007-06-13
                     * 
                     *  Not using XQilla anymore. And may have been another misunderstanding anyhow. So have attributes
                     *  inherit the namespace of the element they are from, by default --LGP 2024-01-09
                     * 
                     *  MAYBE related to  http://stroika-bugs.sophists.com/browse/STK-999 - diff symptoms but similar workaround
                     */
                    element->setAttributeNS (attrName.fNamespace ? attrName.fNamespace->As<String> (kUseURIEncodingFlag_).As<u16string> ().c_str () : nullptr,
                                             attrName.fName.As<u16string> ().c_str (), v->As<u16string> ().c_str ());
                }
                else {
                    element->removeAttributeNS (
                        attrName.fNamespace ? attrName.fNamespace->As<String> (kUseURIEncodingFlag_).As<u16string> ().c_str () : nullptr,
                        attrName.fName.As<u16string> ().c_str ());
                }
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual Element::Ptr InsertElement (const NameWithNamespace& eltName, const Element::Ptr& afterNode) override
        {
#if qDebug
            Require (ValidNewNodeName_ (eltName.fName));
#endif
            START_LIB_EXCEPTION_MAPPER_
            {
                xercesc::DOMDocument* doc = fNode_->getOwnerDocument ();
                // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                // unclear if we should set namespace to existing node namespace or nullptr here?
                DOMNode* child = doc->createElementNS ((eltName.fNamespace == nullopt)
                                                           ? fNode_->getNamespaceURI ()
                                                           : eltName.fNamespace->As<String> (kUseURIEncodingFlag_).As<u16string> ().c_str (),
                                                       eltName.fName.As<u16string> ().c_str ());
                DOMNode* refChildNode = nullptr;
                if (afterNode == nullptr) {
                    // this means PREPEND.
                    // If there is a first element, then insert before it. If no elements, then append is the same thing.
                    refChildNode = fNode_->getFirstChild ();
                }
                else {
                    refChildNode = dynamic_cast<NodeRep_&> (*afterNode.GetRep ()).GetInternalTRep ()->getNextSibling ();
                }
                DOMNode* childx = fNode_->insertBefore (child, refChildNode);
                ThrowIfNull (childx);
                return WrapXercesNodeInStroikaNode_ (childx);
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual Element::Ptr AppendElement (const NameWithNamespace& eltName) override
        {
#if qDebug
            Require (ValidNewNodeName_ (eltName.fName));
#endif
            START_LIB_EXCEPTION_MAPPER_
            {
                xercesc::DOMDocument* doc = fNode_->getOwnerDocument ();
                DOMNode*              child{};
                if (eltName.fNamespace) {
                    u16string namespaceURI = eltName.fNamespace->As<String> (kUseURIEncodingFlag_).As<u16string> ();
                    child                  = doc->createElementNS (namespaceURI.c_str (), eltName.fName.As<u16string> ().c_str ());
                }
                else {
                    const XMLCh* namespaceURI = fNode_->getNamespaceURI (); //? or should be null?
                    child                     = doc->createElementNS (namespaceURI, eltName.fName.As<u16string> ().c_str ());
                }
                DOMNode* childx = fNode_->appendChild (child);
                ThrowIfNull (childx);
                return WrapXercesNodeInStroikaNode_ (childx);
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual Iterable<Node::Ptr> GetChildren () const override
        {
            AssertNotNull (fNode_);
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
        }
        struct XPathQueryHelper_ {
            optional<AutoRelease_<DOMXPathNSResolver>> resolver;
            DOMXPathResult::ResultType                 rt{};
            optional<AutoRelease_<DOMXPathExpression>> expr;
            XPathQueryHelper_ (DOMNode* n, const XPath::Expression& e, bool firstOnly)
            {
                xercesc_3_2::DOMDocument* doc = n->getOwnerDocument ();
                resolver.emplace (doc->createNSResolver (nullptr));
                auto namespaceDefs = e.GetOptions ().fNamespaces;
                if (namespaceDefs.GetDefaultNamespace ()) {
                    (*resolver)->addNamespaceBinding (
                        u"", namespaceDefs.GetDefaultNamespace ()->As<String> (kUseURIEncodingFlag_).As<u16string> ().c_str ());
                }
                for (Common::KeyValuePair ni : namespaceDefs.GetPrefixedNamespaces ()) {
                    (*resolver)->addNamespaceBinding (ni.fKey.As<u16string> ().c_str (),
                                                      ni.fValue.As<String> (kUseURIEncodingFlag_).As<u16string> ().c_str ());
                }
                try {
                    expr.emplace (doc->createExpression (e.GetExpression ().As<u16string> ().c_str (), *resolver));
                }
                catch (const xercesc_3_2::DOMXPathException&) {
                    // MANY basic things are not supported in Xerces XPath - like a[1] - brackets not supported.
                    Execution::Throw (XPath::XPathExpressionNotSupported::kThe);
                }
                switch (e.GetOptions ().fResultTypeIndex.value_or (DOMXPathResult::ANY_TYPE)) {
                    case XPath::ResultTypeIndex_v<Node::Ptr>: {
                        auto o = e.GetOptions ();
                        if (firstOnly) {
                            rt = e.GetOptions ().fOrdered ? DOMXPathResult::FIRST_ORDERED_NODE_TYPE : DOMXPathResult::ANY_UNORDERED_NODE_TYPE;
                        }
                        else if (o.fSnapshot) {
                            rt = e.GetOptions ().fOrdered ? DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE : DOMXPathResult::UNORDERED_NODE_SNAPSHOT_TYPE;
                        }
                        else {
                            // Would make sense given docs, but appears unsupported by Xerces... --LGP 2024-01-12
                            //          rt = e.GetOptions ().fOrdered ? DOMXPathResult::ORDERED_NODE_ITERATOR_TYPE : DOMXPathResult::UNORDERED_NODE_ITERATOR_TYPE;
                            rt = e.GetOptions ().fOrdered ? DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE : DOMXPathResult::UNORDERED_NODE_SNAPSHOT_TYPE;
                        }
                    } break;
                    default:
                        AssertNotImplemented ();
                }
            }
            static optional<XPath::Result> ToResult_ (const xercesc_3_2::DOMXPathResult* r)
            {
                RequireNotNull (r);
                switch (r->getResultType ()) {
                    case DOMXPathResult::NUMBER_TYPE:
                        return XPath::Result{r->getNumberValue ()};
                    case DOMXPathResult::BOOLEAN_TYPE:
                        return XPath::Result{r->getBooleanValue ()};
                    case DOMXPathResult::STRING_TYPE:
                        return XPath::Result{xercesString2String (r->getStringValue ())};
                    case DOMXPathResult::ANY_UNORDERED_NODE_TYPE:
                    case DOMXPathResult::FIRST_ORDERED_NODE_TYPE:
                    case DOMXPathResult::UNORDERED_NODE_ITERATOR_TYPE:
                    case DOMXPathResult::ORDERED_NODE_ITERATOR_TYPE:
                    case DOMXPathResult::UNORDERED_NODE_SNAPSHOT_TYPE:
                    case DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE: {
                        auto n = r->getNodeValue ();
                        return n == nullptr ? optional<XPath::Result>{} : XPath::Result{Node::Ptr{WrapXercesNodeInStroikaNode_ (n)}};
                    }
                    default:
                        AssertNotImplemented ();
                }
                return nullopt;
            }
        };
        virtual optional<XPath::Result> LookupOne (const XPath::Expression& e) override
        {
            if constexpr (false) {
                // quickie impl just to test Lookup Code
                for (auto i : Lookup (e)) {
                    return i;
                }
                return nullopt;
            }
            START_LIB_EXCEPTION_MAPPER_
            {
                XPathQueryHelper_                         xpHelp{fNode_, e, true};
                AutoRelease_<xercesc_3_2::DOMXPathResult> r = (*xpHelp.expr)->evaluate (fNode_, xpHelp.rt, nullptr);
                return XPathQueryHelper_::ToResult_ (r);
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual Traversal::Iterable<XPath::Result> Lookup (const XPath::Expression& e) override
        {
            if (e.GetOptions ().fSnapshot) {
                XPath::Expression::Options e2o = e.GetOptions ();
                e2o.fSnapshot                  = false;
                return Sequence<XPath::Result>{this->Lookup (XPath::Expression{e.GetExpression (), e2o})};
            }
            shared_ptr<XPathQueryHelper_>                         xpHelp = make_shared<XPathQueryHelper_> (fNode_, e, false);
            shared_ptr<AutoRelease_<xercesc_3_2::DOMXPathResult>> r =
                make_shared<AutoRelease_<xercesc_3_2::DOMXPathResult>> ((*xpHelp->expr)->evaluate (fNode_, xpHelp->rt, nullptr));
            Assert (not e.GetOptions ().fSnapshot);
            if (xpHelp->rt == DOMXPathResult::UNORDERED_NODE_ITERATOR_TYPE or xpHelp->rt == DOMXPathResult::ORDERED_NODE_ITERATOR_TYPE) [[unlikely]] {
                return Traversal::CreateGenerator<XPath::Result> ([xpHelp, r, firstTime = true] () mutable -> optional<XPath::Result> {
                    if (firstTime) {
                        firstTime = false;
                        return XPathQueryHelper_::ToResult_ (*r);
                    }
                    if ((*r)->iterateNext () == false) {
                        return nullopt;
                    }
                    return XPathQueryHelper_::ToResult_ (*r);
                });
            }
            if (xpHelp->rt == DOMXPathResult::UNORDERED_NODE_SNAPSHOT_TYPE or xpHelp->rt == DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE) [[likely]] {
                return Traversal::CreateGenerator<XPath::Result> ([xpHelp, r, snapIdx = 0] () mutable -> optional<XPath::Result> {
                    if (not(*r)->snapshotItem (snapIdx)) {
                        return nullopt;
                    }
                    ++snapIdx;
                    return XPathQueryHelper_::ToResult_ (*r);
                });
            }
            AssertNotImplemented ();
            return Traversal::Iterable<XPath::Result>{};
        }
        virtual Element::Ptr GetChildElementByID (const String& id) const override
        {
            AssertNotNull (fNode_);
            START_LIB_EXCEPTION_MAPPER_
            {
                for (DOMNode* i = fNode_->getFirstChild (); i != nullptr; i = i->getNextSibling ()) {
                    if (i->getNodeType () == DOMNode::ELEMENT_NODE) {
                        DOMElement*  elt = Debug::UncheckedDynamicCast<DOMElement*> (i);
                        const XMLCh* s   = elt->getAttribute (u"id");
                        AssertNotNull (s);
                        if (CString::Equals (s, id.As<u16string> ().c_str ())) {
                            return WrapXercesNodeInStroikaNode_ (elt);
                        }
                    }
                }
                return Element::Ptr{nullptr};
            }
            END_LIB_EXCEPTION_MAPPER_
        }
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning
}

namespace {
    inline void MakeXMLDoc_ (shared_ptr<xercesc::DOMDocument>& newXMLDoc)
    {
        Require (newXMLDoc == nullptr);
        newXMLDoc = shared_ptr<xercesc::DOMDocument> (GetDOMIMPL_ ().createDocument (0, nullptr, 0));
        newXMLDoc->setXmlStandalone (true);
    }
}

namespace {
    class MyMaybeSchemaDOMParser_ {
    public:
        Map2StroikaExceptionsErrorReporter_ myErrReporter;
        shared_ptr<XercesDOMParser>         fParser;
        Schema::Ptr                         fSchema{nullptr};

        MyMaybeSchemaDOMParser_ ()                               = delete;
        MyMaybeSchemaDOMParser_ (const MyMaybeSchemaDOMParser_&) = delete;
        MyMaybeSchemaDOMParser_ (const Schema::Ptr& schema)
            : fSchema{schema}
        {
            shared_ptr<IXercesSchemaRep> accessSchema = dynamic_pointer_cast<IXercesSchemaRep> (schema.GetRep ());
            if (accessSchema != nullptr) {
                fParser = Memory::MakeSharedPtr<XercesDOMParser> (nullptr, XMLPlatformUtils::fgMemoryManager, accessSchema->GetCachedGrammarPool ());
                fParser->cacheGrammarFromParse (false);
                fParser->useCachedGrammarInParse (true);
                fParser->setDoSchema (true);
                fParser->setValidationScheme (AbstractDOMParser::Val_Always);
                fParser->setValidationSchemaFullChecking (true);
                fParser->setIdentityConstraintChecking (true);
            }
            else {
                fParser = Memory::MakeSharedPtr<XercesDOMParser> ();
            }
            fParser->setDoNamespaces (true);
            fParser->setErrorHandler (&myErrReporter);

            // @todo make load-external DTD OPTION specified in NEW for document!!! - parser! --LGP 2023-12-16

            // LGP added 2009-09-07 - so must test carefully!
            {
                // I THINK this prevents loading URL-based DTDs - like the one refered to in http://demo.healthframeowrks.com/ when I load the xhmtl as xml
                // (it tkaes forever)
                fParser->setLoadExternalDTD (false);
                // I THINK this prevents loading URL-based schemas (not sure if/how that would have ever happened so I'm not sure that
                // this is for) - guessing a bit...
                //  -- LGP 2009-09-04
                fParser->setLoadSchema (false);
            }
        }
    };
}

namespace {
    Element::Ptr WrapXercesNodeInStroikaNode_ (DOMElement* n);
}
namespace {
    struct DocRep_ : DataExchange::XML::DOM::Document::IRep {
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
        static inline atomic<unsigned int> sLiveCnt{0};
#endif
        //
        // If this function is passed a nullptr exceptionResult - it will throw on bad validation.
        // If it is passed a non-nullptr exceptionResult - then it will map BadFormatException to being ignored, but filling in this
        // parameter with the exception details. This is used to allow 'advisory' read xsd validation failure, without actually fully
        // failing the read (for http://bugzilla/show_bug.cgi?id=513).
        //
        DocRep_ (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schema)
        {
            [[maybe_unused]] int ignoreMe = 0; // workaround quirk in clang-format
            START_LIB_EXCEPTION_MAPPER_
            {
                MakeXMLDoc_ (fXMLDoc);
                fXMLDoc->setUserData (kXerces2XMLDBDocumentKey_, this, nullptr);
                if (in != nullptr) {
                    MyMaybeSchemaDOMParser_ myDOMParser{schema};
                    myDOMParser.fParser->parse (StdIStream_InputSource_{in, u"XMLDB"});
                    fXMLDoc.reset ();
                    fXMLDoc = shared_ptr<xercesc::DOMDocument>{myDOMParser.fParser->adoptDocument ()};
                    fXMLDoc->setXmlStandalone (true);
                    fXMLDoc->setUserData (kXerces2XMLDBDocumentKey_, this, nullptr);
                }
            }
            END_LIB_EXCEPTION_MAPPER_
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            ++sLiveCnt;
#endif
        }
        DocRep_ (const DocRep_& from)
        {
            START_LIB_EXCEPTION_MAPPER_
            {
                fXMLDoc = shared_ptr<xercesc::DOMDocument> (dynamic_cast<xercesc::DOMDocument*> (from.fXMLDoc->cloneNode (true)));
                fXMLDoc->setXmlStandalone (true);
                fXMLDoc->setUserData (kXerces2XMLDBDocumentKey_, this, nullptr);
            }
            END_LIB_EXCEPTION_MAPPER_
            EnsureNotNull (fXMLDoc);
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            ++sLiveCnt;
#endif
        }
        virtual ~DocRep_ ()
        {
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            Assert (sLiveCnt > 0);
            --sLiveCnt;
#endif
        }
        virtual const Providers::IDOMProvider* GetProvider () const override
        {
            return &Providers::Xerces::kDefaultProvider;
        }
        virtual bool GetStandalone () const override
        {
            AssertNotNull (fXMLDoc);
            return fXMLDoc->getXmlStandalone ();
        }
        virtual void SetStandalone (bool standalone) override
        {
            AssertNotNull (fXMLDoc);
            fXMLDoc->setXmlStandalone (standalone);
        }
        virtual Iterable<Node::Ptr> GetChildren () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotNull (fXMLDoc);
            START_LIB_EXCEPTION_MAPPER_
            return Traversal::CreateGenerator<Node::Ptr> (
                [sni = SubNodeIterator_{Memory::MakeSharedPtr<SubNodeIteratorOver_SiblingList_Rep_> (fXMLDoc.get ())}] () mutable -> optional<Node::Ptr> {
                    if (sni.IsAtEnd ()) {
                        return optional<Node::Ptr>{};
                    }
                    Node::Ptr r = *sni;
                    ++sni;
                    return r;
                });
            END_LIB_EXCEPTION_MAPPER_
        }
        virtual Element::Ptr ReplaceRootElement (const NameWithNamespace& newEltName, bool childrenInheritNS) override
        {
            DOMElement* n = newEltName.fNamespace == nullopt
                                ? fXMLDoc->createElement (newEltName.fName.As<u16string> ().c_str ())
                                : fXMLDoc->createElementNS (newEltName.fNamespace->As<String> (kUseURIEncodingFlag_).As<u16string> ().c_str (),
                                                            newEltName.fName.As<u16string> ().c_str ());
            AssertNotNull (n);
            DOMElement* oldRoot = fXMLDoc->getDocumentElement ();
            if (oldRoot == nullptr) {
                (void)fXMLDoc->insertBefore (n, nullptr);
            }
            else {
                (void)fXMLDoc->replaceChild (n, oldRoot);
                /*
                 * I THOGUHT this was a memory leak, but that appears to have been wrong. First, the
                 * DOMNode objects get associated with the document, and when the document is destroyed
                 * this is cleaned up. Secondly, there are enough other memory leaks - its unclear if this
                 * actually helped. Plus the memory management pattern used by Xerces - with its own sub-chunking etc,
                 * makes it hard to tell.
                 *
                 * More importantly - this caused a regression in HealthFrame - which I didn't debug. The OHSD reports
                 * like AAFP CCR report - will be rejected by our 'valid HTML' tester. Unclear if that's cuz we generate
                 * different HTML, but more likely a bug with the load/checker code. Still - not worth worrying
                 * about why at this stage (especially as we are about to upgrade our Xerces version - could get fixed
                 * by that?).
                 *
                 *      -- LGP 2009-05-15
                 *
                 *          oldRoot->release ();
                 */
            }
            Assert (fXMLDoc->getDocumentElement () == n);
            Element::Ptr r{WrapXercesNodeInStroikaNode_ (n)};
            if (childrenInheritNS and newEltName.fNamespace) {
                r.SetAttribute (kXMLNS, newEltName.fNamespace->As<String> ());
            }
            return r;
        }
        virtual void Write (const Streams::OutputStream::Ptr<byte>& to, const SerializationOptions& options) const override
        {
            TraceContextBumper                             ctx{"Xerces::DocRep_::Write"};
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotNull (fXMLDoc);
            START_LIB_EXCEPTION_MAPPER_
            {
                DoWrite2Stream_ (fXMLDoc.get (), to, options);
            }
            END_LIB_EXCEPTION_MAPPER_
        }
#if qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy
        Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif
            virtual void
            Validate (const Schema::Ptr& schema) const override
        {
            TraceContextBumper                             ctx{"Xerces::DocRep_::Validate"};
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            RequireNotNull (schema);
            START_LIB_EXCEPTION_MAPPER_
            {
                try {
                    DbgTrace ("Validating against schema (target namespace '{}')"_f, schema.GetTargetNamespace ());
                    // As this CAN be expensive - especially if we need to externalize the file, and re-parse it!!! - just shortcut by
                    // checking the top-level DOM-node and assure that has the right namespace. At least quickie first check that works when
                    // reading files (doesn't help in pre-save check, of course)
                    DOMNode* docNode = fXMLDoc->getDocumentElement ();
                    if (docNode == nullptr) [[unlikely]] {
                        Execution::Throw (BadFormatException{"No document", 0, 0, 0});
                    }
                    optional<URI> docURI = docNode->getNamespaceURI () == nullptr ? optional<URI>{} : docNode->getNamespaceURI ();
                    if (docURI != schema.GetTargetNamespace ()) {
                        Execution::Throw (BadFormatException{
                            Format ("Wrong document namespace (found '{}' and expected '{}')"_f, docURI, schema.GetTargetNamespace ()), 0, 0, 0});
                    }

                    // EXTERNALIZE, AND THEN RE-PARSE USING CACHED SAX PARSER WTIH LOADED GRAMMAR
                    {
                        MemBufFormatTarget destination;
                        {
                            AutoRelease_<DOMLSOutput> theOutputDesc = GetDOMIMPL_ ().createLSOutput ();
                            theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
                            AutoRelease_<DOMLSSerializer> writer = GetDOMIMPL_ ().createLSSerializer ();
                            theOutputDesc->setByteStream (&destination);
                            theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
                            Assert (fXMLDoc->getXmlStandalone ());
                            writer->write (fXMLDoc.get (), theOutputDesc);
                        }
                        MemBufInputSource readReadSrc{destination.getRawBuffer (), destination.getLen (), u"tmp"};
                        readReadSrc.setEncoding (XMLUni::fgUTF8EncodingString);
                        shared_ptr<IXercesSchemaRep> accessSchema = dynamic_pointer_cast<IXercesSchemaRep> (schema.GetRep ());
                        {
                            AssertNotNull (accessSchema); // for now only rep supported
                            shared_ptr<SAX2XMLReader> parser = shared_ptr<SAX2XMLReader> (
                                XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema->GetCachedGrammarPool ()));
                            SetupCommonParserFeatures_ (*parser, true);
                            Map2StroikaExceptionsErrorReporter_ myErrorReporter;
                            parser->setErrorHandler (&myErrorReporter);
                            parser->parse (readReadSrc);
                        }
                    }
                }
                catch (...) {
                    if constexpr (qDumpXMLOnValidationError_) {
#if !qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy
                        // Generate temp file (each with differnet names), and write out the bad XML.
                        // Then - re-validate (with line#s) - and print the results of the validation to ANOTHER
                        // temporary file
                        //
                        filesystem::path tmpFileName = IO::FileSystem::AppTmpFileManager::sThe.GetTmpFile ("FAILED_VALIDATION_.xml");
                        DbgTrace ("Error validating - so writing out temporary file = '{}'"_f, tmpFileName);
                        Write (IO::FileSystem::FileOutputStream::New (tmpFileName), SerializationOptions{.fPrettyPrint = true, .fIndent = 4});
                        try {
                            ValidateFile (tmpFileName, schema);
                        }
                        catch (const BadFormatException& vf) {
                            String   tmpFileNameStr = IO::FileSystem::FromPath (tmpFileName);
                            size_t   idx            = tmpFileNameStr.find (".xml");
                            String   newTmpFile     = tmpFileNameStr.substr (0, idx) + "_MSG.txt";
                            ofstream msgOut{newTmpFile.AsNarrowSDKString ().c_str ()};
                            msgOut << "Reason:" << vf.GetDetails () << endl;
                            optional<unsigned int> lineNum;
                            optional<unsigned int> colNumber;
                            optional<uint64_t>     fileOffset;
                            vf.GetPositionInfo (&lineNum, &colNumber, &fileOffset);
                            if (lineNum) {
                                msgOut << "Line:" << *lineNum << endl;
                            }
                            if (colNumber) {
                                msgOut << "Col: " << *colNumber << endl;
                            }
                            if (fileOffset) {
                                msgOut << "FilePos: " << *fileOffset << endl;
                            }
                        }
                        catch (...) {
                        }
#endif
                    }
                    Execution::ReThrow ();
                }
            }
            END_LIB_EXCEPTION_MAPPER_
        }
        shared_ptr<xercesc::DOMDocument>                               fXMLDoc;
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
}

namespace {
    Node::Ptr WrapXercesNodeInStroikaNode_ (DOMNode* n)
    {
        RequireNotNull (n);
        if (n->getNodeType () == DOMNode::ELEMENT_NODE) {
            return Node::Ptr{Memory::MakeSharedPtr<ElementRep_> (n)};
        }
        else {
            return Node::Ptr{Memory::MakeSharedPtr<NodeRep_> (n)};
        }
    }
    Element::Ptr WrapXercesNodeInStroikaNode_ (DOMElement* n)
    {
        RequireNotNull (n);
        return Element::Ptr{Memory::MakeSharedPtr<ElementRep_> (n)};
    }
}

/*
 ********************************************************************************
 ********************* Provider::Xerces::xercesString2String ********************
 ********************************************************************************
 */
String Providers::Xerces::xercesString2String (const XMLCh* s, const XMLCh* e)
{
    if constexpr (same_as<XMLCh, char16_t>) {
        return String{span{s, e}};
    }
    // nb: casts required cuz Xerces doesn't (currently) use wchar_t/char16_t/char32_t but something the sizeof char16_t
    // --LGP 2016-07-29
    if constexpr (sizeof (XMLCh) == sizeof (char16_t)) {
        return String{span{reinterpret_cast<const char16_t*> (s), reinterpret_cast<const char16_t*> (e)}};
    }
    else if constexpr (sizeof (XMLCh) == sizeof (char32_t)) {
        return String{span{reinterpret_cast<const char32_t*> (s), reinterpret_cast<const char32_t*> (e)}};
    }
    else {
        AssertNotReached ();
        return String{};
    }
}

String Providers::Xerces::xercesString2String (const XMLCh* t)
{
    if constexpr (same_as<XMLCh, char16_t>) {
        return String{t};
    }
    // nb: casts required cuz Xerces doesn't (currently) use wchar_t/char16_t/char32_t but something the sizeof char16_t
    // --LGP 2016-07-29
    if constexpr (sizeof (XMLCh) == sizeof (char16_t)) {
        return String{reinterpret_cast<const char16_t*> (t)};
    }
    else if constexpr (sizeof (XMLCh) == sizeof (char32_t)) {
        return String{reinterpret_cast<const char32_t*> (t)};
    }
    else {
        AssertNotReached ();
        return String{};
    }
}

/*
 ********************************************************************************
 ************************* XML::Providers::LibXML2::Provider ********************
 ********************************************************************************
 */
Providers::Xerces::Provider::Provider ()
{
    TraceContextBumper ctx{"Xerces::Provider::CTOR"};
#if qDebug
    static unsigned int sNProvidersCreated_{0}; // don't create multiple of these - will lead to confusion
    Assert (++sNProvidersCreated_ == 1);
#endif
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
    fUseXercesMemoryManager_ = new MyXercesMemMgr_{};
    XMLPlatformUtils::Initialize (XMLUni::fgXercescDefaultLocale, 0, 0, fUseXercesMemoryManager_);
#else
    XMLPlatformUtils::Initialize (XMLUni::fgXercescDefaultLocale, 0, 0);
#endif
}

Providers::Xerces::Provider::~Provider ()
{
    TraceContextBumper ctx{"Xerces::Provider::DTOR"};
    XMLPlatformUtils::Terminate ();
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
    Require (SchemaRep_::sLiveCnt == 0); // Check for leaks but better/clearer than memory leaks check below
    Require (DocRep_::sLiveCnt == 0);    // ""
    delete fUseXercesMemoryManager_;     // checks for leaks
#endif
}

shared_ptr<Schema::IRep> Providers::Xerces::Provider::SchemaFactory (const InputStream::Ptr<byte>& schemaData, const Resource::ResolverPtr& resolver) const
{
    return Memory::MakeSharedPtr<SchemaRep_> (schemaData, resolver);
}

shared_ptr<DOM::Document::IRep> Providers::Xerces::Provider::DocumentFactory (const Streams::InputStream::Ptr<byte>& in,
                                                                              const Schema::Ptr& schemaToValidateAgainstWhileReading) const
{
    return Memory::MakeSharedPtr<DocRep_> (in, schemaToValidateAgainstWhileReading);
}

void Providers::Xerces::Provider::SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                                            const Schema::Ptr& schema) const
{
    SAX2PrintHandlers_           handler{callback};
    shared_ptr<IXercesSchemaRep> accessSchema;
    if (schema != nullptr) {
        accessSchema = dynamic_pointer_cast<IXercesSchemaRep> (schema.GetRep ());
    }
    shared_ptr<SAX2XMLReader> parser{XMLReaderFactory::createXMLReader (
        XMLPlatformUtils::fgMemoryManager, accessSchema == nullptr ? nullptr : accessSchema->GetCachedGrammarPool ())};
    SetupCommonParserFeatures_ (*parser, accessSchema != nullptr);
    parser->setContentHandler (&handler);
    Map2StroikaExceptionsErrorReporter_ mErrorReproter_;
    parser->setErrorHandler (&mErrorReproter_);
    parser->parse (StdIStream_InputSource_{in});
}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <fstream>
#include <limits>
#include <set>
#include <sstream>

#include "Stroika/Foundation/Cache/LRUCache.h"
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Configuration/StroikaVersion.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/IO/FileSystem/TemporaryFile.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/MemoryAllocator.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Schema.h"

#include "DOM.h"

#if qStroika_Foundation_DataExchange_XML_SupportDOM

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using std::byte;

using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::DOM;

namespace {
    // from XQilla
    template <class TYPE>
    class AutoRelease {
    public:
        AutoRelease (TYPE* p)
            : p_ (p)
        {
        }
        ~AutoRelease ()
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
        AutoRelease (const AutoRelease<TYPE>&)                  = delete;
        AutoRelease<TYPE>& operator= (const AutoRelease<TYPE>&) = delete;

        TYPE* p_;
    };
}

// avoid namespace conflcit with some Xerces code
#undef Assert

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/parsers/SAX2XMLReaderImpl.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#if qDebug
#define Assert(c)                                                                                                                          \
    (!!(c) || (Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Assert", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_), false))
#else
#define Assert(c) ((void)0)
#endif
XERCES_CPP_NAMESPACE_USE

/*
 *  UnderlyingXMLLibExcptionMapping layer
 */
#define START_LIB_EXCEPTION_MAPPER try {
#define END_LIB_EXCEPTION_MAPPER                                                                                                           \
    }                                                                                                                                      \
    catch (const OutOfMemoryException&)                                                                                                    \
    {                                                                                                                                      \
        Execution::Throw (bad_alloc{}, "xerces OutOfMemoryException - throwing bad_alloc");                                                \
    }                                                                                                                                      \
    catch (...)                                                                                                                            \
    {                                                                                                                                      \
        Execution::ReThrow ();                                                                                                             \
    }

namespace {
    const XMLCh        kDOMImplFeatureDeclaration[] = u"Core";
    DOMImplementation& GetDOMIMPL_ ()
    {
        // safe to save in a static var? -- LGP 2007-05-20
        // from perusing implementation - this appears safe to cache and re-use in differnt threads
        static DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation (kDOMImplFeatureDeclaration);
        AssertNotNull (impl);
        return *impl;
    }
}

namespace {
    constexpr bool qDumpXMLOnValidationError = qDebug;
}

// Simple 'roughly analagous' type wrappers - start with 'T_'
namespace {
    typedef XERCES_CPP_NAMESPACE::DOMNode T_DOMNode;

    typedef XERCES_CPP_NAMESPACE::DOMElement T_DOMElement;

    typedef XERCES_CPP_NAMESPACE::DOMDocument T_XMLDOMDocument;
    typedef shared_ptr<T_XMLDOMDocument>      T_XMLDOMDocumentSmartPtr;

    typedef XERCES_CPP_NAMESPACE::DOMNodeList T_DOMNodeList;
}

#if qDebug
namespace {
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
}
#endif

namespace {
    class StrmFmtTarget : public XMLFormatTarget {
    public:
        ostream& fOut;
        StrmFmtTarget (ostream& ostr)
            : fOut (ostr)
        {
        }
        virtual void writeChars (const XMLByte* const toWrite, const XMLSize_t count, [[maybe_unused]] XMLFormatter* const formatter) override
        {
            fOut.write (reinterpret_cast<const char*> (toWrite), sizeof (XMLByte) * count);
        }
        virtual void flush () override
        {
            fOut.flush ();
        }
    };

    // mostly for debugging purposes
    void DoWrite2File (T_XMLDOMDocument* doc, const String& fileName, bool prettyPrint)
    {
        AutoRelease<DOMLSOutput> theOutputDesc = GetDOMIMPL_ ().createLSOutput ();
        theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
        AutoRelease<DOMLSSerializer> writer = GetDOMIMPL_ ().createLSSerializer ();
        DOMConfiguration*            dc     = writer->getDomConfig ();
        dc->setParameter (XMLUni::fgDOMWRTFormatPrettyPrint, prettyPrint);
        dc->setParameter (XMLUni::fgDOMWRTBOM, true);
        LocalFileFormatTarget ftg (fileName.As<u16string> ().c_str ());
        theOutputDesc->setByteStream (&ftg);
        Assert (doc->getXmlStandalone ());
        writer->write (doc, theOutputDesc);
    }
    void DoWrite2Stream_ (T_XMLDOMDocument* doc, ostream& ostr, bool prettyPrint)
    {
        AutoRelease<DOMLSOutput> theOutputDesc = GetDOMIMPL_ ().createLSOutput ();
        theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
        AutoRelease<DOMLSSerializer> writer = GetDOMIMPL_ ().createLSSerializer ();
        DOMConfiguration*            dc     = writer->getDomConfig ();
        dc->setParameter (XMLUni::fgDOMWRTFormatPrettyPrint, prettyPrint);
        dc->setParameter (XMLUni::fgDOMWRTBOM, true);
        StrmFmtTarget dest (ostr);
        theOutputDesc->setByteStream (&dest);
        Assert (doc->getXmlStandalone ());
        writer->write (doc, theOutputDesc);
    }
    string DoWrite2UTF8String (T_XMLDOMDocument* doc, bool prettyPrint)
    {
        stringstream resultBuf (ios_base::in | ios_base::out | ios_base::binary);
        DoWrite2Stream_ (doc, resultBuf, prettyPrint);
        return resultBuf.str ();
    }
}

namespace {
    T_DOMNode* RecursivelySetNamespace_ (T_DOMNode* n, const XMLCh* namespaceURI)
    {
        RequireNotNull (n);
        // namespaceURI CAN be nullptr
        switch (n->getNodeType ()) {
            case DOMNode::ELEMENT_NODE: {
                T_XMLDOMDocument* doc = n->getOwnerDocument ();
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
}

namespace {
    constexpr XMLCh* kXerces2XMLDBDocumentKey = nullptr; // just a unique key to lookup our doc object from the xerces doc object.
                                                         // Could use real str, then xerces does strcmp() - but this appears slightly faster
                                                         // so long as no conflict....
}

namespace {
    String GetTextForDOMNode_ (const T_DOMNode* node)
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
        DbgTrace ("WARNING: GetTextForDOMNode_::BackupMode used");
        return node->getTextContent ();
    }
}

namespace {

#if 0
    class SubNodeIteratorOver_vectorDOMNODE_Rep_ : public SubNodeIterator::Rep,
                                                   Memory::UseBlockAllocationIfAppropriate<SubNodeIteratorOver_vectorDOMNODE_Rep_> {
    public:
        SubNodeIteratorOver_vectorDOMNODE_Rep_ (const vector<T_DOMNode*>& dns)
            : fDOMNodes (dns)
            , fCur (0)
        {
        }

    public:
        virtual bool IsAtEnd () const override
        {
            return fCur >= fDOMNodes.size ();
        }
        virtual void Next () override
        {
            Require (not IsAtEnd ());
            fCur++;
        }
        virtual Node   Current () const override;
        virtual size_t GetLength () const override
        {
            return fDOMNodes.size ();
        }

    private:
        vector<T_DOMNode*> fDOMNodes;
        size_t             fCur;
    };
#endif

#if 0

    class SubNodeIteratorOver_DOMNodeList_Rep : SubNodeIterator::Rep, Memory::UseBlockAllocationIfAppropriate<SubNodeIteratorOver_DOMNodeList_Rep> {
    public:
        SubNodeIteratorOver_DOMNodeList_Rep (T_DOMNodeList* dln);

    public:
        virtual bool   IsAtEnd () const override;
        virtual void   Next () override;
        virtual Node   Current () const override;
        virtual size_t GetLength () const override;

    private:
#if qHasFeature_Xerces
        T_DOMNodeList* fMainNodeList;
#endif
        size_t fMainListLen;
        size_t fAttrsListLen;
        size_t fCur;
    };
#endif

    class SubNodeIteratorOver_SiblingList_Rep : public SubNodeIterator::Rep,
                                                Memory::UseBlockAllocationIfAppropriate<SubNodeIteratorOver_SiblingList_Rep> {
    public:
        // Called iterates over CHILDREN of given parentNode
        SubNodeIteratorOver_SiblingList_Rep (T_DOMNode* nodeParent);

    public:
        virtual bool   IsAtEnd () const override;
        virtual void   Next () override;
        virtual Node   Current () const override;
        virtual size_t GetLength () const override;

    private:
        T_DOMNode*     fParentNode{nullptr};
        T_DOMNode*     fCurNode{nullptr};
        mutable size_t fCachedMainListLen{};
    };

}
namespace {
    Node WrapImpl_ (T_DOMNode* n);
}

namespace {
    class MyErrorReproter : public XMLErrorReporter, public ErrorHandler {
        // XMLErrorReporter
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

        // ErrorHandler
    public:
        virtual void warning ([[maybe_unused]] const SAXParseException& exc) override
        {
            // ignore
        }
        virtual void error (const SAXParseException& exc) override
        {
            Execution::Throw (BadFormatException (exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()),
                                                  static_cast<unsigned int> (exc.getColumnNumber ()), 0));
        }
        virtual void fatalError (const SAXParseException& exc) override
        {
            Execution::Throw (BadFormatException (exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()),
                                                  static_cast<unsigned int> (exc.getColumnNumber ()), 0));
        }
    };
    static MyErrorReproter sMyErrorReproter;
}

namespace {
    inline void SetupCommonParserFeatures_ (SAX2XMLReader& reader)
    {
        reader.setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
        reader.setFeature (XMLUni::fgXercesDynamic, false);
        reader.setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, false); // false:  * *Do not report attributes used for Namespace declarations, and optionally do not report original prefixed names
    }
    inline void SetupCommonParserFeatures_ (SAX2XMLReader& reader, bool validatingWithSchema)
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
    inline void MakeXMLDoc_ (T_XMLDOMDocumentSmartPtr& newXMLDoc)
    {
        Require (newXMLDoc == nullptr);
        newXMLDoc = T_XMLDOMDocumentSmartPtr (GetDOMIMPL_ ().createDocument (0, nullptr, 0));
        newXMLDoc->setXmlStandalone (true);
    }

}

RecordNotFoundException::RecordNotFoundException ()
    : Execution::RuntimeErrorException<> ("Record Not Found")
{
}

namespace {
    // These SHOULD be part of xerces! Perhaps someday post them?
    class BinaryInputStream_InputSource : public InputSource {
    protected:
        class _MyInputStrm : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream {
        public:
            _MyInputStrm (Streams::InputStream<byte>::Ptr in)
                : _fSource{in}
            {
            }
            ~_MyInputStrm () = default;

        public:
            virtual XMLFilePos curPos () const override
            {
                if (_fSource.IsSeekable ()) {
                    return _fSource.GetOffset ();
                }
                return 0;
            }
            virtual XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
            {
                return _fSource.Read (reinterpret_cast<byte*> (toFill), reinterpret_cast<byte*> (toFill + maxToRead));
            }
            virtual const XMLCh* getContentType () const override
            {
                return nullptr;
            }

        protected:
            Streams::InputStream<byte>::Ptr _fSource;
        };

    public:
        BinaryInputStream_InputSource (Streams::InputStream<byte>::Ptr in, const XMLCh* const bufId = nullptr)
            : InputSource (bufId)
            , fSource (in)
        {
        }
        virtual BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) _MyInputStrm (fSource);
        }

    protected:
        Streams::InputStream<byte>::Ptr fSource;
    };

    // my variations of StdIInputSrc with progresstracker callback
    class BinaryInputStream_InputSource_WithProgress : public BinaryInputStream_InputSource {
    protected:
        class _InStrWithProg : public BinaryInputStream_InputSource_WithProgress::_MyInputStrm {
        public:
            _InStrWithProg (Streams::InputStream<byte>::Ptr in, Execution::ProgressMonitor::Updater progressCallback)
                : _MyInputStrm (in)
                , fProgress (progressCallback, 0.0f, 1.0f)
                , fTotalSize (0.0f)
            {
                Require (in.IsSeekable ());
                streamoff start = in.GetOffset ();
                in.Seek (Streams::Whence::eFromEnd, 0);
                streamoff totalSize = in.GetOffset ();
                Assert (start <= totalSize);
                in.Seek (start);
                fTotalSize = static_cast<float> (totalSize);
            }

        public:
            virtual XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
            {
                float curOffset        = 0.0;
                bool  doProgressBefore = (maxToRead > 10 * 1024); // only bother calling both before & after if large read
                if (fTotalSize > 0.0f and doProgressBefore) {
                    curOffset = static_cast<float> (_fSource.GetOffset ());
                    fProgress.SetCurrentProgressAndThrowIfCanceled (curOffset / fTotalSize);
                }
                XMLSize_t result = _MyInputStrm::readBytes (toFill, maxToRead);
                if (fTotalSize > 0) {
                    curOffset = static_cast<float> (_fSource.GetOffset ());
                    fProgress.SetCurrentProgressAndThrowIfCanceled (curOffset / fTotalSize);
                }
                return result;
            }

        private:
            Execution::ProgressMonitor::Updater fProgress;
            float                               fTotalSize;
        };

    public:
        BinaryInputStream_InputSource_WithProgress (Streams::InputStream<byte>::Ptr in,
                                                    Execution::ProgressMonitor::Updater progressCallback, const XMLCh* const bufId = nullptr)
            : BinaryInputStream_InputSource (in, bufId)
            , fProgressCallback (progressCallback)
        {
        }
        virtual BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) _InStrWithProg (fSource, fProgressCallback);
        }

    private:
        Execution::ProgressMonitor::Updater fProgressCallback;
    };
}

class MyMaybeSchemaDOMParser {
public:
    shared_ptr<Schema::AccessCompiledXSD> fSchemaAccessor;
    shared_ptr<XercesDOMParser>           fParser;

    MyMaybeSchemaDOMParser (const Schema* schema)
    {
        if (schema != nullptr) {
            // REALLY need READLOCK - cuz this just prevents UPDATE of Schema (never happens anyhow) -- LGP 2009-05-19
            fSchemaAccessor = shared_ptr<Schema::AccessCompiledXSD> (new Schema::AccessCompiledXSD (*schema));
            fParser         = make_shared<XercesDOMParser> (nullptr, XMLPlatformUtils::fgMemoryManager, fSchemaAccessor->GetCachedTRep ());
            fParser->cacheGrammarFromParse (false);
            fParser->useCachedGrammarInParse (true);
            fParser->setDoSchema (true);
            fParser->setValidationScheme (AbstractDOMParser::Val_Always);
            fParser->setValidationSchemaFullChecking (true);
            fParser->setIdentityConstraintChecking (true);
            fParser->setErrorHandler (&sMyErrorReproter);
        }
        else {
            fParser = make_shared<XercesDOMParser> ();
        }
        fParser->setDoNamespaces (true);
        fParser->setErrorHandler (&sMyErrorReproter);

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

/*
 ********************************************************************************
 *********************************** Document ***********************************
 ********************************************************************************
 */
class DataExchange::XML::DOM::Document::Rep {
public:
    Rep (const Schema* schema)
        : fXMLDoc ()
        , fSchema (schema)
    {
        [[maybe_unused]] int ignoreMe = 0; // workaround quirk in clang-format
        START_LIB_EXCEPTION_MAPPER
        {
            MakeXMLDoc_ (fXMLDoc);
            fXMLDoc->setUserData (kXerces2XMLDBDocumentKey, this, nullptr);
        }
        END_LIB_EXCEPTION_MAPPER
    }

    Rep (const Rep& from)
        : fXMLDoc ()
        , fSchema (from.fSchema)
    {
        START_LIB_EXCEPTION_MAPPER
        {
            fXMLDoc = T_XMLDOMDocumentSmartPtr (dynamic_cast<T_XMLDOMDocument*> (from.fXMLDoc->cloneNode (true)));
            fXMLDoc->setXmlStandalone (true);
            fXMLDoc->setUserData (kXerces2XMLDBDocumentKey, this, nullptr);
        }
        END_LIB_EXCEPTION_MAPPER
        EnsureNotNull (fXMLDoc);
    }

    virtual ~Rep () = default;

public:
    nonvirtual recursive_mutex& GetLock () const
    {
        return fCriticalSection;
    }

public:
    nonvirtual const Schema* GetSchema () const
    {
        return fSchema;
    }

public:
    nonvirtual void SetSchema (const Schema* s)
    {
        fSchema = s;
    }

public:
    //
    // If this function is passed a nullptr exceptionResult - it will throw on bad validation.
    // If it is passed a non-nullptr exceptionResult - then it will map BadFormatException to being ignored, but filling in this
    // parameter with the exception details. This is used to allow 'advisory' read xsd validation failure, without actually fully
    // failing the read (for http://bugzilla/show_bug.cgi?id=513).
    //
    nonvirtual void Read (Streams::InputStream<byte>::Ptr& in, bool encrypted, shared_ptr<BadFormatException>* exceptionResult,
                          Execution::ProgressMonitor::Updater progressCallback)
    {
        TraceContextBumper ctx{"XMLDB::Document::Rep::Read"};
        AssertNotNull (fXMLDoc);

        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        START_LIB_EXCEPTION_MAPPER
        {
            MyMaybeSchemaDOMParser myDOMParser (fSchema);
            if (!encrypted) {
                try {
                    myDOMParser.fParser->parse (BinaryInputStream_InputSource_WithProgress{
                        in, Execution::ProgressMonitor::Updater (progressCallback, 0.1f, 0.8f), u"XMLDB"});
                }
                catch (const BadFormatException& vf) {
                    // Support  http://bugzilla/show_bug.cgi?id=513  and allowing partially valid inputs (like bad ccrs)
                    if (exceptionResult == nullptr) {
                        ReThrow ();
                        throw;
                    }
                    else {
                        DbgTrace ("Validation failure passed by through Read () function argument");
                        *exceptionResult = make_shared<BadFormatException> (vf);
                        // and ignore - fall through to completed parse.
                    }
                }
                goto CompletedParse;
            }

            {
                u16string xmlText = Streams::TextReader::New (in).ReadAll ().As<u16string> ();
                MemBufInputSource memBufIS (reinterpret_cast<const XMLByte*> (xmlText.c_str ()), xmlText.length () * sizeof (XMLCh), u"XMLDB");
                memBufIS.setEncoding (XMLUni::fgUTF16LEncodingString2);
                myDOMParser.fParser->parse (memBufIS);
            }

        CompletedParse:
            fXMLDoc.reset ();
            fXMLDoc = T_XMLDOMDocumentSmartPtr (myDOMParser.fParser->adoptDocument ());
            fXMLDoc->setXmlStandalone (true);
            fXMLDoc->setUserData (kXerces2XMLDBDocumentKey, this, nullptr);
        }
        END_LIB_EXCEPTION_MAPPER
        progressCallback.SetProgress (1.0f);
    }

public:
    nonvirtual void SetRootElement (const Node& newRoot)
    {
        TraceContextBumper          ctx{"XMLDB::Document::Rep::SetRootElement"};
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        AssertNotNull (fXMLDoc);
        Node replacementRoot = CreateDocumentElement (newRoot.GetName ());
        // next copy all children
        bool addedChildElts = false;
        for (SubNodeIterator i = newRoot.GetChildren (); i.NotDone (); ++i) {
            Node c = *i;
            switch (c.GetNodeType ()) {
                case Node::eElementNT: {
                    addedChildElts = true;
                    replacementRoot.AppendNode (c);
                } break;
                case Node::eAttributeNT: {
                    replacementRoot.SetAttribute (c.GetName (), c.GetValue ().As<String> ());
                } break;
                case Node::eTextNT: {
                    // Alas - this SetRootElement () is a bit buggy. Don't know how to do it right!
                    // This maybe OK -- LGP 2006-09-13
                    if (not addedChildElts) {
                        replacementRoot.SetValue (c.GetValue ());
                    }
                } break;
            }
        }
    }

public:
    nonvirtual Node CreateDocumentElement (const String& name)
    {
        TraceContextBumper ctx{"XMLDB::Document::Rep::CreateDocumentElement"};
        Require (ValidNewNodeName_ (name));
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        AssertNotNull (fXMLDoc);
        START_LIB_EXCEPTION_MAPPER
        {
            Node        result;
            DOMElement* n       = fSchema == nullptr ? fXMLDoc->createElement (name.As<u16string> ().c_str ())
                                                     : fXMLDoc->createElementNS (fSchema->GetTargetNamespace ().As<u16string> ().c_str (),
                                                                                 name.As<u16string> ().c_str ());
            DOMElement* oldRoot = fXMLDoc->getDocumentElement ();
            if (oldRoot == nullptr) {
                (void)fXMLDoc->insertBefore (n, nullptr);
            }
            else {
                (void)fXMLDoc->replaceChild (n, oldRoot);
                /*
                 * I THOGUHT this was a memory leak, but that appears to have been wrong. First, the
                 * DOMNode objects get associated with the document, and when the docment is destroyed
                 * this is cleaned up. Secondly, there are enough other memory leaks - its unclear if this
                 * actually helped. Plus the memory management pattern used by Xerces - with its own subchunking etc,
                 * makes it hard to tell.
                 *
                 * More importantly - this caused a regression in HelathFrame - which I didn't debug. The OHSD reports
                 * like AAFP CCR report - will be rejected by our 'valid HTML' tester. Unclear if thats cuz we generate
                 * different HTML, but more likely a bug with the load/checker code. Still - not worth worrying
                 * about why at this stage (especially as we are about to upgrade our Xerces version - could get fixed
                 * by that?).
                 *
                 *      -- LGP 2009-05-15
                 *
                 *          oldRoot->release ();
                 *
                 */
            }
            Assert (fXMLDoc->getDocumentElement () == n);
            result = WrapImpl_ (n);
            return result;
        }
        END_LIB_EXCEPTION_MAPPER
    }

public:
    nonvirtual void LoadXML (const String& xml)
    {
        TraceContextBumper          ctx{"XMLDB::Document::Rep::LoadXML"};
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        AssertNotNull (fXMLDoc);
        START_LIB_EXCEPTION_MAPPER
        {
            MyMaybeSchemaDOMParser myDOMParser (fSchema);
            MemBufInputSource memBufIS (reinterpret_cast<const XMLByte*> (xml.As<u16string> ().c_str ()), xml.length () * sizeof (XMLCh), u"XMLDB");
            memBufIS.setEncoding (XMLUni::fgUTF16LEncodingString2);
            myDOMParser.fParser->parse (memBufIS);
            fXMLDoc.reset ();
            fXMLDoc = T_XMLDOMDocumentSmartPtr (myDOMParser.fParser->adoptDocument ());
            fXMLDoc->setXmlStandalone (true);
            fXMLDoc->setUserData (kXerces2XMLDBDocumentKey, this, nullptr);
        }
        END_LIB_EXCEPTION_MAPPER
    }

public:
    nonvirtual void WritePrettyPrinted (ostream& out) const
    {
        TraceContextBumper          ctx{"XMLDB::Document::Rep::WritePrettyPrinted"};
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        AssertNotNull (fXMLDoc);
        START_LIB_EXCEPTION_MAPPER
        {
#if qHasFeature_Xerces
            DoWrite2Stream_ (fXMLDoc.get (), out, true);
#endif
        }
        END_LIB_EXCEPTION_MAPPER
    }

public:
    nonvirtual void WriteAsIs (ostream& out) const
    {
        TraceContextBumper          ctx{"XMLDB::Document::Rep::WriteAsIs"};
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        AssertNotNull (fXMLDoc);
        START_LIB_EXCEPTION_MAPPER
        {
            DoWrite2Stream_ (fXMLDoc.get (), out, false);
        }
        END_LIB_EXCEPTION_MAPPER
    }

public:
    nonvirtual SubNodeIterator GetChildren () const
    {
        //TraceContextBumper ctx (_T ("XMLDB::Document::Rep::GetChildren"));
        // really not enough - must pass critsection to iterator rep!
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        AssertNotNull (fXMLDoc);
        START_LIB_EXCEPTION_MAPPER
        {
            return SubNodeIterator{Memory::MakeSharedPtr<SubNodeIteratorOver_SiblingList_Rep> (fXMLDoc.get ())};
        }
        END_LIB_EXCEPTION_MAPPER
    }

public:
    nonvirtual void Validate () const
    {
        RequireNotNull (fSchema);
        TraceContextBumper          ctx{"XMLDB::Document::Rep::Validate"};
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);

        START_LIB_EXCEPTION_MAPPER
        {
            if (fSchema == nullptr) {
                return;
            }
            try {
                DbgTrace (L"Validating against target namespace '%s'", fSchema->GetTargetNamespace ().c_str ());
                // As this CAN be expensive - especially if we need to externalize the file, and re-parse it!!! - just shortcut by
                // checking the top-level DOM-node and assure that has the right namespace. At least quickie first check that works when
                // reading files (doesnt help in pre-save check, of course)
                T_DOMNode* docNode = fXMLDoc->getDocumentElement ();
                if (docNode == nullptr) {
                    Execution::Throw (BadFormatException (L"No document", 0, 0, 0));
                }
                String docURI = docNode->getNamespaceURI () == nullptr ? String{} : docNode->getNamespaceURI ();
                if (docURI != fSchema->GetTargetNamespace ()) {
                    Execution::Throw (BadFormatException (Format (L"Wrong document namespace (found '%s' and expected '%s')",
                                                                  docURI.c_str (), fSchema->GetTargetNamespace ().c_str ()),
                                                          0, 0, 0));
                }

                // EXTERNALIZE, AND THEN RE-PARSE USING CACHED SAX PARSER WTIH LOADED GRAMMAR
                {
                    MemBufFormatTarget destination;
                    {
                        AutoRelease<DOMLSOutput> theOutputDesc = GetDOMIMPL_ ().createLSOutput ();
                        theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
                        AutoRelease<DOMLSSerializer> writer = GetDOMIMPL_ ().createLSSerializer ();
                        theOutputDesc->setByteStream (&destination);
                        theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
                        Assert (fXMLDoc->getXmlStandalone ());
                        writer->write (fXMLDoc.get (), theOutputDesc);
                    }

                    MemBufInputSource readReadSrc (destination.getRawBuffer (), destination.getLen (), u"tmp");
                    readReadSrc.setEncoding (XMLUni::fgUTF8EncodingString);

                    {
                        Schema::AccessCompiledXSD accessSchema{*fSchema}; // REALLY need READLOCK - cuz this just prevents UPDATE of Scehma (never happens anyhow) -- LGP 2009-05-19
                        shared_ptr<SAX2XMLReader> parser = shared_ptr<SAX2XMLReader> (
                            XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema.GetCachedTRep ()));
                        SetupCommonParserFeatures_ (*parser, true);
                        parser->setErrorHandler (&sMyErrorReproter);
                        parser->parse (readReadSrc);
                    }
                }
            }
            catch (...) {
                if constexpr (qDumpXMLOnValidationError) {
                    // Generate temp file (each with differnet names), and write out the bad XML.
                    // Then - re-validate (with line#s) - and print the results of the validation to ANOTHER
                    // temporary file
                    //
                    filesystem::path tmpFileName = IO::FileSystem::AppTempFileManager::sThe.GetTempFile ("FAILED_VALIDATION_.xml");
                    DbgTrace (L"Error validating - so writing out temporary file = '%s'", Characters::ToString (tmpFileName).c_str ());
                    {
                        ofstream out (tmpFileName.c_str (), ios_base::out | ios_base::binary);
                        WritePrettyPrinted (out);
                    }
                    try {
                        if (fSchema != nullptr) {
                            ValidateExternalFile (tmpFileName.c_str (), *fSchema);
                        }
                    }
                    catch (const BadFormatException& vf) {
                        String   tmpFileNameStr = IO::FileSystem::FromPath (tmpFileName);
                        size_t   idx            = tmpFileNameStr.find (".xml");
                        String   newTmpFile     = tmpFileNameStr.substr (0, idx) + "_MSG.txt";
                        ofstream msgOut (newTmpFile.AsNarrowSDKString ().c_str ());
                        msgOut << "Reason:" << vf.GetDetails ().AsNarrowSDKString () << endl;
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
                }
                Execution::ReThrow ();
            }
        }
        END_LIB_EXCEPTION_MAPPER
    }

public:
    nonvirtual void Validate (const Schema* schema) const
    {
        TraceContextBumper          ctx{"XMLDB::Document::Rep::Validate/1"};
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        RequireNotNull (schema); // if you explicitly specify a schema - it makes no sense for it to be nullptr!!!

        /*
         *  This is SUPPOSED to be a CONST method, but there is no way I can see (cheaply) to validate
         *  against a different schema. I could copy - but that would be expensive. So - temporarily reset
         *  the schema, and set it back (careful of exceptions!)
         */
        const Schema* origSchema = GetSchema ();
        try {
            const_cast<Rep*> (this)->SetSchema (schema);
            Validate ();
            const_cast<Rep*> (this)->SetSchema (origSchema);
        }
        catch (...) {
            const_cast<Rep*> (this)->SetSchema (origSchema);
            Execution::ReThrow ();
        }
    }

public:
    nonvirtual NamespaceDefinitionsList GetNamespaceDefinitions () const
    {
        lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
        if (fSchema == nullptr) {
            return NamespaceDefinitionsList ();
        }
        else {
            return fSchema->GetNamespaceDefinitions ();
        }
    }

private:
    T_XMLDOMDocumentSmartPtr fXMLDoc;
    const Schema*            fSchema;
    mutable recursive_mutex  fCriticalSection;
};

Document::Document (const Schema* schema)
    : fRep (new Rep{schema})
{
}

Document::Document (Rep* rep)
    : fRep (rep)
{
    RequireNotNull (rep);
}

Document::~Document ()
{
}

shared_ptr<Document::Rep> Document::GetRep () const
{
    return fRep;
}

recursive_mutex& Document::GetLock () const
{
    return fRep->GetLock ();
}

const Schema* Document::GetSchema () const
{
    return fRep->GetSchema ();
}

void Document::SetSchema (const Schema* schema)
{
    fRep->SetSchema (schema);
}

void Document::WritePrettyPrinted (ostream& out) const
{
    /*
     * Write pretty printed XML - where we generate the whitespace around nodes - ignoring any text fragments - except in leaf nodes.
     */
    fRep->WritePrettyPrinted (out);
}

void Document::WriteAsIs (ostream& out) const
{
    /*
     * Write - respecting all the little #text fragment nodes throughout the XML node tree
     */
    fRep->WriteAsIs (out);
}

SubNodeIterator Document::GetChildren () const
{
    return fRep->GetChildren ();
}

Node Document::GetRootElement () const
{
    // Should only be one in an XML document.
    for (SubNodeIterator i = GetChildren (); i.NotDone (); ++i) {
        if ((*i).GetNodeType () == Node::eElementNT) {
            return *i;
        }
    }
    return Node ();
}

void Document::Validate () const
{
    RequireNotNull (GetSchema ());
    fRep->Validate ();
}

void Document::Validate (const Schema* schema) const
{
    RequireNotNull (schema);
    fRep->Validate (schema);
}

/*
 ********************************************************************************
 ******************************** RWDocument ************************************
 ********************************************************************************
 */
RWDocument::RWDocument (const Schema* schema)
    : Document (schema)
{
}

RWDocument::RWDocument (const Document& from)
    : Document (new Rep (*from.GetRep ()))
{
}

RWDocument::RWDocument (const RWDocument& from)
    : Document (new Rep (*from.fRep))
{
}

RWDocument& RWDocument::operator= (const Document& rhs)
{
    fRep = shared_ptr<Rep> (new Rep (*rhs.GetRep ()));
    return *this;
}

Node RWDocument::CreateDocumentElement (const String& name)
{
    return fRep->CreateDocumentElement (name);
}

void RWDocument::SetRootElement (const Node& newRoot)
{
    return fRep->SetRootElement (newRoot);
}

void RWDocument::Read (Streams::InputStream<byte>::Ptr& in, bool encrypted, Execution::ProgressMonitor::Updater progressCallback)
{
    fRep->Read (in, encrypted, nullptr, progressCallback);
}

void RWDocument::ReadAllowingInvalidSrc (Streams::InputStream<byte>::Ptr& in, bool encrypted, shared_ptr<BadFormatException>* exceptionResult,
                                         Execution::ProgressMonitor::Updater progressCallback)
{
    RequireNotNull (exceptionResult);
    exceptionResult->reset ();
    fRep->Read (in, encrypted, exceptionResult, progressCallback); // since we assert exceptionResult - that means that this will NOT throw BadFormatException
}

void RWDocument::LoadXML (const String& xml)
{
    fRep->LoadXML (xml);
}

namespace {
    T_DOMNode* GetInternalRep_ (Node::Rep* anr)
    {
        RequireNotNull (anr);
        AssertNotNull (anr->GetInternalTRep ());
        return reinterpret_cast<T_DOMNode*> (anr->GetInternalTRep ());
    }
}
namespace {
    class MyNodeRep : public Node::Rep, Memory::UseBlockAllocationIfAppropriate<MyNodeRep> {
    public:
        MyNodeRep (
            DOMNode* n
            )
            : fNode (n)
        {
            RequireNotNull (n);
        }

    public:
        virtual Node::NodeType GetNodeType () const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                switch (fNode->getNodeType ()) {
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
            END_LIB_EXCEPTION_MAPPER
        }
        virtual String GetNamespace () const override
        {
            AssertNotNull (fNode);
            Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
            START_LIB_EXCEPTION_MAPPER
            {
                AssertNotNull (fNode->getNamespaceURI ());
                return fNode->getNamespaceURI ();
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual String GetName () const override
        {
            AssertNotNull (fNode);
            Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
            START_LIB_EXCEPTION_MAPPER
            {
                AssertNotNull (fNode->getNodeName ());
                return fNode->getNodeName ();
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void SetName (const String& name) override
        {
            AssertNotNull (fNode);
            Require (ValidNewNodeName_ (name));
            START_LIB_EXCEPTION_MAPPER
            {
                T_XMLDOMDocument* doc = fNode->getOwnerDocument ();
                AssertNotNull (doc);
                fNode = doc->renameNode (fNode, fNode->getNamespaceURI (), name.As<u16string> ().c_str ());
                AssertNotNull (fNode);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual VariantValue GetValue () const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                return GetTextForDOMNode_ (fNode);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void SetValue (const VariantValue& v) override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                String tmpStr = v.As<String> ();
                fNode->setTextContent (tmpStr.empty () ? nullptr : tmpStr.As<u16string> ().c_str ());
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void SetAttribute (const String& attrName, const String& v) override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                DOMElement* element = dynamic_cast<DOMElement*> (fNode);
                ThrowIfNull (element);
                /*
                 * For reasons that elude maybe (maybe because it was standard for XML early on)
                 * all my attributes are free of namespaces. So why use setAttributeNS? Because otherwise
                 * the XQilla code fails to match on the attribute names at all in its XPath stuff.
                 * Considered copying the namespace from the parent element (fNode->getNamespaceURI()),
                 * but XQilla didnt like that either (maybe then I needed M: on xpath).
                 * A differnt subclass object of DOMAttrNode is created - one that doesnt have a getLocalName,
                 * or something like that. Anyhow - this appears to do the right thing for now...
                 *      -- LGP 2007-06-13
                 */
                element->setAttributeNS (nullptr, attrName.As<u16string> ().c_str (), v.As<u16string> ().c_str ());
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual bool HasAttribute (const String& attrName, const String* value) const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                if (fNode->getNodeType () == DOMNode::ELEMENT_NODE) {
                    AssertMember (fNode, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                    T_DOMElement* elt = reinterpret_cast<T_DOMElement*> (fNode);
                    if (elt->hasAttribute (attrName.As<u16string> ().c_str ())) {
                        if (value != nullptr) {
                            const XMLCh* s = elt->getAttribute (attrName.As<u16string> ().c_str ());
                            AssertNotNull (s);
                            return CString::Equals (s, value->As<u16string> ().c_str ());
                        }
                        return true;
                    }
                }
                return false;
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual String GetAttribute (const String& attrName) const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                if (fNode->getNodeType () == DOMNode::ELEMENT_NODE) {
                    AssertMember (fNode, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                    T_DOMElement* elt = reinterpret_cast<T_DOMElement*> (fNode);
                    const XMLCh*  s   = elt->getAttribute (attrName.As<u16string> ().c_str ());
                    AssertNotNull (s);
                    return s;
                }
                return String ();
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node GetFirstAncestorNodeWithAttribute (const String& attrName) const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                for (T_DOMNode* p = fNode; p != nullptr; p = p->getParentNode ()) {
                    if (p->getNodeType () == DOMNode::ELEMENT_NODE) {
                        AssertMember (p, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                        const T_DOMElement* elt = reinterpret_cast<const T_DOMElement*> (p);
                        if (elt->hasAttribute (attrName.As<u16string> ().c_str ())) {
                            return WrapImpl_ (p);
                        }
                    }
                }
                return Node{};
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node InsertChild (const String& name, const String* ns, Node afterNode) override
        {
            Require (ValidNewNodeName_ (name));
            START_LIB_EXCEPTION_MAPPER
            {
                T_XMLDOMDocument* doc = fNode->getOwnerDocument ();
                // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                T_DOMNode* child = doc->createElementNS ((ns == nullptr) ? fNode->getNamespaceURI () : ns->As<u16string> ().c_str (),
                                                         name.As<u16string> ().c_str ());
                T_DOMNode* refChildNode = nullptr;
                if (afterNode.IsNull ()) {
                    // this means PREPEND.
                    // If there is a first element, then insert before it. If no elements, then append is the same thing.
                    refChildNode = fNode->getFirstChild ();
                }
                else {
                    refChildNode = GetInternalRep_ (GetRep4Node (afterNode).get ())->getNextSibling ();
                }
                T_DOMNode* childx = fNode->insertBefore (child, refChildNode);
                ThrowIfNull (childx);
                return WrapImpl_ (childx);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node AppendChild (const String& name) override
        {
            Require (ValidNewNodeName_ (name));
            START_LIB_EXCEPTION_MAPPER
            {
                T_XMLDOMDocument* doc          = fNode->getOwnerDocument ();
                const XMLCh*      namespaceURI = fNode->getNamespaceURI ();
                // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                T_DOMNode* child  = doc->createElementNS (namespaceURI, name.As<u16string> ().c_str ());
                T_DOMNode* childx = fNode->appendChild (child);
                ThrowIfNull (childx);
                return WrapImpl_ (childx);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void AppendChild (const String& name, const String* ns, const VariantValue& v) override
        {
            Require (ValidNewNodeName_ (name));
            START_LIB_EXCEPTION_MAPPER
            {
                T_XMLDOMDocument* doc = fNode->getOwnerDocument ();
                // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                T_DOMNode* child = doc->createElementNS ((ns == nullptr) ? fNode->getNamespaceURI () : ns->As<u16string> ().c_str (),
                                                         name.As<u16string> ().c_str ());
                String tmpStr = v.As<String> (); // See http://bugzilla/show_bug.cgi?id=338
                child->setTextContent (tmpStr.empty () ? nullptr : tmpStr.As<u16string> ().c_str ());
                ThrowIfNull (fNode->appendChild (child));
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void AppendChildIfNotEmpty (const String& name, const String* ns, const VariantValue& v) override
        {
            Require (ValidNewNodeName_ (name));
            if (not v.empty ()) {
                AppendChild (name, ns, v);
            }
        }
        virtual Node InsertNode (const Node& n, const Node& afterNode, bool inheritNamespaceFromInsertionPoint) override
        {
            START_LIB_EXCEPTION_MAPPER
            {
                T_XMLDOMDocument* doc         = fNode->getOwnerDocument ();
                T_DOMNode*        nodeToClone = GetInternalRep_ (GetRep4Node (n).get ());
                T_DOMNode*        clone       = doc->importNode (nodeToClone, true);
                if (inheritNamespaceFromInsertionPoint and clone->getNodeType () == DOMNode::ELEMENT_NODE) {
                    clone = RecursivelySetNamespace_ (clone, fNode->getNamespaceURI ());
                }
                T_DOMNode* insertAfterNode = afterNode.IsNull () ? nullptr : GetInternalRep_ (GetRep4Node (afterNode).get ());
                T_DOMNode* clonex          = fNode->insertBefore (clone, insertAfterNode);
                ThrowIfNull (clonex);
                return WrapImpl_ (clonex);
            }
            END_LIB_EXCEPTION_MAPPER
        }

        virtual Node AppendNode (const Node& n, bool inheritNamespaceFromInsertionPoint) override
        {
            START_LIB_EXCEPTION_MAPPER
            {
                T_XMLDOMDocument* doc         = fNode->getOwnerDocument ();
                T_DOMNode*        nodeToClone = GetInternalRep_ (GetRep4Node (n).get ());
                T_DOMNode*        clone       = doc->importNode (nodeToClone, true);
                if (inheritNamespaceFromInsertionPoint and clone->getNodeType () == DOMNode::ELEMENT_NODE) {
                    clone = RecursivelySetNamespace_ (clone, fNode->getNamespaceURI ());
                }
                T_DOMNode* clonex = fNode->appendChild (clone);
                ThrowIfNull (clonex);
                return WrapImpl_ (clonex);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void DeleteNode () override
        {
            START_LIB_EXCEPTION_MAPPER
            {
                T_DOMNode* selNode = fNode;
                ThrowIfNull (selNode);
                T_DOMNode* parentNode = selNode->getParentNode ();
                if (parentNode == nullptr) {
                    // This happens if the selected node is an attribute
                    if (fNode != nullptr) {
                        const XMLCh* ln = selNode->getNodeName ();
                        AssertNotNull (ln);
                        DOMElement* de = dynamic_cast<DOMElement*> (fNode);
                        de->removeAttribute (ln);
                    }
                }
                else {
                    (void)parentNode->removeChild (selNode);
                }
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node ReplaceNode () override
        {
            RequireNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                T_XMLDOMDocument* doc = fNode->getOwnerDocument ();
                ThrowIfNull (doc);
                T_DOMNode* selNode = fNode;
                ThrowIfNull<RecordNotFoundException> (selNode);
                T_DOMNode* parentNode = selNode->getParentNode ();
                ThrowIfNull (parentNode);
                DOMElement* n = doc->createElementNS (selNode->getNamespaceURI (), selNode->getNodeName ());
                (void)parentNode->replaceChild (n, selNode);
                return WrapImpl_ (n);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node GetParentNode () const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
                return WrapImpl_ (fNode->getParentNode ());
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual SubNodeIterator GetChildren () const override;
        virtual Node            GetChildNodeByID (const String& id) const override;
        virtual void*           GetInternalTRep () override
        {
            return fNode;
        }

    private:
        nonvirtual Document::Rep& GetAssociatedDoc_ () const
        {
            AssertNotNull (fNode);
            T_XMLDOMDocument* doc = fNode->getOwnerDocument ();
            AssertNotNull (doc);
            void* docData = doc->getUserData (kXerces2XMLDBDocumentKey);
            AssertNotNull (docData);
            return *reinterpret_cast<Document::Rep*> (docData);
        }

    private:
        nonvirtual NamespaceDefinitionsList GetNamespaceDefinitions_ () const
        {
            AssertNotNull (fNode);
            return GetAssociatedDoc_ ().GetNamespaceDefinitions ();
        }

    private:
        // must carefully think out mem managment here - cuz not ref counted - around as long as owning doc...
        DOMNode* fNode;
    };
}

namespace {
    inline Node WrapImpl_ (T_DOMNode* n)
    {
        return n == nullptr ? Node{} : Node{make_shared<MyNodeRep> (n)};
    }
}

/*
 ********************************************************************************
 ********************************** MyNodeRep ***********************************
 ********************************************************************************
 */
SubNodeIterator MyNodeRep::GetChildren () const
{
    AssertNotNull (fNode);
    START_LIB_EXCEPTION_MAPPER
    {
        return SubNodeIterator{Memory::MakeSharedPtr<SubNodeIteratorOver_SiblingList_Rep> (fNode)};
    }
    END_LIB_EXCEPTION_MAPPER
}

Node MyNodeRep::GetChildNodeByID (const String& id) const
{
    AssertNotNull (fNode);
    START_LIB_EXCEPTION_MAPPER
    {
        for (T_DOMNode* i = fNode->getFirstChild (); i != nullptr; i = i->getNextSibling ()) {
            if (i->getNodeType () == DOMNode::ELEMENT_NODE) {
                AssertMember (i, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                T_DOMElement* elt = reinterpret_cast<T_DOMElement*> (i);
                const XMLCh*  s   = elt->getAttribute (u"id");
                AssertNotNull (s);
                if (CString::Equals (s, id.As<u16string> ().c_str ())) {
                    return WrapImpl_ (i);
                }
            }
        }
        return Node ();
    }
    END_LIB_EXCEPTION_MAPPER
}

/*
 ********************************************************************************
 ********************** SubNodeIteratorOver_SiblingList_Rep *********************
 ********************************************************************************
 */
SubNodeIteratorOver_SiblingList_Rep::SubNodeIteratorOver_SiblingList_Rep (T_DOMNode* nodeParent)
    : fParentNode{nodeParent}
    , fCachedMainListLen{static_cast<size_t> (-1)}
{
    RequireNotNull (nodeParent);
    START_LIB_EXCEPTION_MAPPER
    {
        fCurNode = nodeParent->getFirstChild ();
    }
    END_LIB_EXCEPTION_MAPPER
}

bool SubNodeIteratorOver_SiblingList_Rep::IsAtEnd () const
{
    return fCurNode == nullptr;
}

void SubNodeIteratorOver_SiblingList_Rep::Next ()
{
    Require (not IsAtEnd ());
    AssertNotNull (fCurNode);
    START_LIB_EXCEPTION_MAPPER
    {
        fCurNode = fCurNode->getNextSibling ();
    }
    END_LIB_EXCEPTION_MAPPER
}

Node SubNodeIteratorOver_SiblingList_Rep::Current () const
{
    return WrapImpl_ (fCurNode);
}

size_t SubNodeIteratorOver_SiblingList_Rep::GetLength () const
{
    if (fCachedMainListLen == static_cast<size_t> (-1)) {
        size_t n = 0;
        START_LIB_EXCEPTION_MAPPER
        {
            for (T_DOMNode* i = fParentNode->getFirstChild (); i != nullptr; (i = i->getNextSibling ()), ++n)
                ;
        }
        END_LIB_EXCEPTION_MAPPER
        fCachedMainListLen = n;
    }
    return fCachedMainListLen;
}
#endif
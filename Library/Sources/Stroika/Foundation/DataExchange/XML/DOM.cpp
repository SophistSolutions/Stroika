/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <fstream>
#include <limits>
#include <set>
#include <sstream>

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
#include "Stroika/Foundation/Streams/TextToByteReader.h"
#include "Stroika/Foundation/Traversal/Generator.h"

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

#if qHasFeature_Xerces
#include "Providers/Xerces.h"

using namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces;

#endif

/*
 *  Short lifetime. Don't save these iterator objects. Just use them to enumerate a collection and then let them
 *  go. They (could) become invalid after a call to update the database.
 */
class SubNodeIterator {
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
    explicit SubNodeIterator (const shared_ptr<Rep>& from);

public:
    nonvirtual bool NotDone () const;
    nonvirtual bool IsAtEnd () const;
    nonvirtual void Next ();
    nonvirtual Node::Ptr Current () const;
    nonvirtual size_t    GetLength () const;

public:
    nonvirtual void operator++ ();
    nonvirtual void operator++ (int);
    nonvirtual Node::Ptr operator* () const;

protected:
    shared_ptr<Rep> fRep;
};

inline SubNodeIterator::SubNodeIterator (const shared_ptr<Rep>& from)
    : fRep{from}
{
}
inline bool SubNodeIterator::NotDone () const
{
    return not fRep->IsAtEnd ();
}
inline bool SubNodeIterator::IsAtEnd () const
{
    return fRep->IsAtEnd ();
}
inline void SubNodeIterator::Next ()
{
    fRep->Next ();
}
inline Node::Ptr SubNodeIterator::Current () const
{
    return fRep->Current ();
}
inline size_t SubNodeIterator::GetLength () const
{
    return fRep->GetLength ();
}
inline void SubNodeIterator::operator++ ()
{
    Next ();
}
inline void SubNodeIterator::operator++ (int)
{
    Next ();
}
inline Node::Ptr SubNodeIterator::operator* () const
{
    return Current ();
}

namespace {
    constexpr XMLCh    kDOMImplFeatureDeclaration[] = u"Core";
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
    using T_DOMNode                = XERCES_CPP_NAMESPACE::DOMNode;
    using T_DOMElement             = XERCES_CPP_NAMESPACE::DOMElement;
    using T_XMLDOMDocument         = XERCES_CPP_NAMESPACE::DOMDocument;
    using T_XMLDOMDocumentSmartPtr = shared_ptr<T_XMLDOMDocument>;
    using T_DOMNodeList            = XERCES_CPP_NAMESPACE::DOMNodeList;
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
    class StrmFmtTarget_ : public XMLFormatTarget {
    public:
        ostream& fOut;
        StrmFmtTarget_ (ostream& ostr)
            : fOut{ostr}
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
    void DoWrite2Stream_ (T_XMLDOMDocument* doc, ostream& ostr, bool prettyPrint)
    {
        AutoRelease<DOMLSOutput> theOutputDesc = GetDOMIMPL_ ().createLSOutput ();
        theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
        AutoRelease<DOMLSSerializer> writer = GetDOMIMPL_ ().createLSSerializer ();
        DOMConfiguration*            dc     = writer->getDomConfig ();
        dc->setParameter (XMLUni::fgDOMWRTFormatPrettyPrint, prettyPrint);
        dc->setParameter (XMLUni::fgDOMWRTBOM, true);
        StrmFmtTarget_ dest{ostr};
        theOutputDesc->setByteStream (&dest);
        Assert (doc->getXmlStandalone ());
        writer->write (doc, theOutputDesc);
    }
    string DoWrite2UTF8String (T_XMLDOMDocument* doc, bool prettyPrint)
    {
        stringstream resultBuf{ios_base::in | ios_base::out | ios_base::binary};
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
    constexpr XMLCh* kXerces2XMLDBDocumentKey_ = nullptr; // just a unique key to lookup our doc object from the xerces doc object.
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
    class SubNodeIteratorOver_SiblingList_Rep_ : public SubNodeIterator::Rep,
                                                 Memory::UseBlockAllocationIfAppropriate<SubNodeIteratorOver_SiblingList_Rep_> {
    public:
        // Called iterates over CHILDREN of given parentNode
        SubNodeIteratorOver_SiblingList_Rep_ (T_DOMNode* nodeParent);

    public:
        virtual bool      IsAtEnd () const override;
        virtual void      Next () override;
        virtual Node::Ptr Current () const override;
        virtual size_t    GetLength () const override;

    private:
        T_DOMNode*     fParentNode{nullptr};
        T_DOMNode*     fCurNode_{nullptr};
        mutable size_t fCachedMainListLen{};
    };
}
namespace {
    Node::Ptr WrapImpl_ (T_DOMNode* n);
}

namespace {
    inline void MakeXMLDoc_ (T_XMLDOMDocumentSmartPtr& newXMLDoc)
    {
        Require (newXMLDoc == nullptr);
        newXMLDoc = T_XMLDOMDocumentSmartPtr (GetDOMIMPL_ ().createDocument (0, nullptr, 0));
        newXMLDoc->setXmlStandalone (true);
    }
}

namespace {
    // These SHOULD be part of xerces! Perhaps someday post them?
    class BinaryInputStream_InputSource_ : public InputSource {
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
        BinaryInputStream_InputSource_ (Streams::InputStream<byte>::Ptr in, const XMLCh* const bufId = nullptr)
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
}

class MyMaybeSchemaDOMParser_ {
public:
    Map2StroikaExceptionsErrorReporter myErrReporter;
    shared_ptr<XercesDOMParser>        fParser;
    Schema::Ptr                        fSchema{nullptr};

    MyMaybeSchemaDOMParser_ ()                               = delete;
    MyMaybeSchemaDOMParser_ (const MyMaybeSchemaDOMParser_&) = delete;
    MyMaybeSchemaDOMParser_ (const Schema::Ptr& schema)
        : fSchema{schema}
    {
        shared_ptr<IXercesSchemaRep> accessSchema = dynamic_pointer_cast<IXercesSchemaRep> (schema.GetRep ());
        if (accessSchema != nullptr) {
            fParser = make_shared<XercesDOMParser> (nullptr, XMLPlatformUtils::fgMemoryManager, accessSchema->GetCachedGrammarPool ());
            fParser->cacheGrammarFromParse (false);
            fParser->useCachedGrammarInParse (true);
            fParser->setDoSchema (true);
            fParser->setValidationScheme (AbstractDOMParser::Val_Always);
            fParser->setValidationSchemaFullChecking (true);
            fParser->setIdentityConstraintChecking (true);
        }
        else {
            fParser = make_shared<XercesDOMParser> ();
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

/*
 ********************************************************************************
 *********************************** Document ***********************************
 ********************************************************************************
 */
namespace {

    class XercesDocRep_ : public DataExchange::XML::DOM::Document::IRep {
    public:
        XercesDocRep_ ()
        {
            [[maybe_unused]] int ignoreMe = 0; // workaround quirk in clang-format
            START_LIB_EXCEPTION_MAPPER
            {
                MakeXMLDoc_ (fXMLDoc);
                fXMLDoc->setUserData (kXerces2XMLDBDocumentKey_, this, nullptr);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        XercesDocRep_ (const XercesDocRep_& from)
        {
            START_LIB_EXCEPTION_MAPPER
            {
                fXMLDoc = T_XMLDOMDocumentSmartPtr (dynamic_cast<T_XMLDOMDocument*> (from.fXMLDoc->cloneNode (true)));
                fXMLDoc->setXmlStandalone (true);
                fXMLDoc->setUserData (kXerces2XMLDBDocumentKey_, this, nullptr);
            }
            END_LIB_EXCEPTION_MAPPER
            EnsureNotNull (fXMLDoc);
        }

        //
        // If this function is passed a nullptr exceptionResult - it will throw on bad validation.
        // If it is passed a non-nullptr exceptionResult - then it will map BadFormatException to being ignored, but filling in this
        // parameter with the exception details. This is used to allow 'advisory' read xsd validation failure, without actually fully
        // failing the read (for http://bugzilla/show_bug.cgi?id=513).
        //
        virtual void Read (const Streams::InputStream<byte>::Ptr& in, const Schema::Ptr& schema) override
        {
            TraceContextBumper ctx{"XercesDocRep_::Read"};
            AssertNotNull (fXMLDoc);

            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_}; // write context cuz reading from stream, but writing to 'this'
            START_LIB_EXCEPTION_MAPPER
            {
                MyMaybeSchemaDOMParser_ myDOMParser{schema};
                myDOMParser.fParser->parse (BinaryInputStream_InputSource_{in, u"XMLDB"});
                fXMLDoc.reset ();
                fXMLDoc = T_XMLDOMDocumentSmartPtr{myDOMParser.fParser->adoptDocument ()};
                fXMLDoc->setXmlStandalone (true);
                fXMLDoc->setUserData (kXerces2XMLDBDocumentKey_, this, nullptr);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        Node::Ptr CreateDocumentElement (const String& name, const optional<URI>& ns)
        {
            TraceContextBumper ctx{"XercesDocRep_Rep::CreateDocumentElement"};
#if qDebug
            Require (ValidNewNodeName_ (name));
#endif
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotNull (fXMLDoc);
            START_LIB_EXCEPTION_MAPPER
            {
                DOMElement* n = ns == nullopt
                                    ? fXMLDoc->createElement (name.As<u16string> ().c_str ())
                                    : fXMLDoc->createElementNS (ns->As<String> ().As<u16string> ().c_str (), name.As<u16string> ().c_str ());
                AssertNotNull (n);
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
                return WrapImpl_ (n);
            }
            END_LIB_EXCEPTION_MAPPER
        }
#if 0
        virtual void LoadXML (const String& xml) override
        {
            TraceContextBumper                             ctx{"XercesDocRep_::LoadXML"};
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotNull (fXMLDoc);
            START_LIB_EXCEPTION_MAPPER
            {
                MyMaybeSchemaDOMParser_ myDOMParser{fSchema};
                MemBufInputSource memBufIS{reinterpret_cast<const XMLByte*> (xml.As<u16string> ().c_str ()), xml.length () * sizeof (XMLCh), u"XMLDB"};
                memBufIS.setEncoding (XMLUni::fgUTF16LEncodingString2);
                myDOMParser.fParser->parse (memBufIS);
                fXMLDoc.reset ();
                fXMLDoc = T_XMLDOMDocumentSmartPtr{myDOMParser.fParser->adoptDocument ()};
                fXMLDoc->setXmlStandalone (true);
                fXMLDoc->setUserData (kXerces2XMLDBDocumentKey_, this, nullptr);
            }
            END_LIB_EXCEPTION_MAPPER
        }
#endif
        virtual void WritePrettyPrinted (ostream& out) const override
        {
            TraceContextBumper                             ctx{"XercesDocRep_::WritePrettyPrinted"};
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotNull (fXMLDoc);
            START_LIB_EXCEPTION_MAPPER
            {
#if qHasFeature_Xerces
                DoWrite2Stream_ (fXMLDoc.get (), out, true);
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void WriteAsIs (ostream& out) const override
        {
            TraceContextBumper                             ctx{"XercesDocRep_::WriteAsIs"};
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotNull (fXMLDoc);
            START_LIB_EXCEPTION_MAPPER
            {
                DoWrite2Stream_ (fXMLDoc.get (), out, false);
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Iterable<Node::Ptr> GetChildren () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            AssertNotNull (fXMLDoc);
            START_LIB_EXCEPTION_MAPPER
            return Traversal::CreateGenerator<Node::Ptr> (
                [sni = SubNodeIterator{Memory::MakeSharedPtr<SubNodeIteratorOver_SiblingList_Rep_> (fXMLDoc.get ())}] () mutable -> optional<Node::Ptr> {
                    if (sni.IsAtEnd ()) {
                        return optional<Node::Ptr>{};
                    }
                    Node::Ptr r = *sni;
                    ++sni;
                    return r;
                });
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void Validate (const Schema::Ptr& schema) const override
        {
            TraceContextBumper                             ctx{"XercesDocRep_::Validate"};
            AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
            RequireNotNull (schema);
            START_LIB_EXCEPTION_MAPPER
            {
                try {
                    DbgTrace (L"Validating against target namespace '%s'", Characters::ToString (schema.GetTargetNamespace ()).c_str ());
                    // As this CAN be expensive - especially if we need to externalize the file, and re-parse it!!! - just shortcut by
                    // checking the top-level DOM-node and assure that has the right namespace. At least quickie first check that works when
                    // reading files (doesnt help in pre-save check, of course)
                    T_DOMNode* docNode = fXMLDoc->getDocumentElement ();
                    if (docNode == nullptr) {
                        Execution::Throw (BadFormatException (L"No document", 0, 0, 0));
                    }
                    optional<URI> docURI = docNode->getNamespaceURI () == nullptr ? optional<URI>{} : docNode->getNamespaceURI ();
                    if (docURI != schema.GetTargetNamespace ()) {
                        Execution::Throw (BadFormatException (Format (L"Wrong document namespace (found '%s' and expected '%s')",
                                                                      Characters::ToString (docURI).c_str (),
                                                                      Characters::ToString (schema.GetTargetNamespace ()).c_str ()),
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

                        MemBufInputSource readReadSrc{destination.getRawBuffer (), destination.getLen (), u"tmp"};
                        readReadSrc.setEncoding (XMLUni::fgUTF8EncodingString);

                        shared_ptr<IXercesSchemaRep> accessSchema = dynamic_pointer_cast<IXercesSchemaRep> (schema.GetRep ());
                        {
                            AssertNotNull (accessSchema); // for now only rep supported
                            shared_ptr<SAX2XMLReader> parser = shared_ptr<SAX2XMLReader> (
                                XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema->GetCachedGrammarPool ()));
                            SetupCommonParserFeatures (*parser, true);
                            Map2StroikaExceptionsErrorReporter myErrorReporter;
                            parser->setErrorHandler (&myErrorReporter);
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
                            ofstream out{tmpFileName, ios_base::out | ios_base::binary};
                            WritePrettyPrinted (out);
                        }
                        try {
                            ValidateFile (tmpFileName, schema);
                        }
                        catch (const BadFormatException& vf) {
                            String   tmpFileNameStr = IO::FileSystem::FromPath (tmpFileName);
                            size_t   idx            = tmpFileNameStr.find (".xml");
                            String   newTmpFile     = tmpFileNameStr.substr (0, idx) + "_MSG.txt";
                            ofstream msgOut{newTmpFile.AsNarrowSDKString ().c_str ()};
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
        T_XMLDOMDocumentSmartPtr                                       fXMLDoc;
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
}

Document::Ptr Document::New (const String& documentElementName, const optional<URI>& ns)
{
    auto p = make_shared<XercesDocRep_> ();
    p->CreateDocumentElement (documentElementName, ns); // @todo change this to CTOR arg overload - not method
    return Ptr{p};
}

Document::Ptr Document::New (const Streams::InputStream<byte>::Ptr& in, const Schema::Ptr& schema)
{
    Ptr p{make_shared<XercesDocRep_> ()};
    p.GetRep ()->Read (in, schema);
    return p;
}

Document::Ptr Document::New (const String& in, const Schema::Ptr& schema)
{
    return New (Streams::TextToByteReader::New (in), schema);
}

/////////////////////////

namespace {
    T_DOMNode* GetInternalRep_ (Node::IRep* anr)
    {
        RequireNotNull (anr);
        AssertNotNull (anr->GetInternalTRep ());
        return reinterpret_cast<T_DOMNode*> (anr->GetInternalTRep ());
    }
}
namespace {
    inline Node::Ptr WrapImpl_ (T_DOMNode* n)
    {
        class MyNodeRep_ : public Node::IRep, Memory::UseBlockAllocationIfAppropriate<MyNodeRep_> {
        public:
            MyNodeRep_ (DOMNode* n)
                : fNode_{n}
            {
                RequireNotNull (n);
            }
            virtual Node::Type GetNodeType () const override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
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
                END_LIB_EXCEPTION_MAPPER
            }
            virtual optional<URI> GetNamespace () const override
            {
                AssertNotNull (fNode_);
                Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
                START_LIB_EXCEPTION_MAPPER
                {
                    const XMLCh* n = fNode_->getNamespaceURI ();
                    return n == nullptr ? optional<URI>{} : URI{xercesString2String (n)};
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual String GetName () const override
            {
                AssertNotNull (fNode_);
                Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
                START_LIB_EXCEPTION_MAPPER
                {
                    AssertNotNull (fNode_->getNodeName ());
                    return fNode_->getNodeName ();
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual void SetName (const String& name) override
            {
                AssertNotNull (fNode_);
#if qDebug
                Require (ValidNewNodeName_ (name));
#endif
                START_LIB_EXCEPTION_MAPPER
                {
                    T_XMLDOMDocument* doc = fNode_->getOwnerDocument ();
                    AssertNotNull (doc);
                    fNode_ = doc->renameNode (fNode_, fNode_->getNamespaceURI (), name.As<u16string> ().c_str ());
                    AssertNotNull (fNode_);
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual String GetValue () const override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    return GetTextForDOMNode_ (fNode_);
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual void SetValue (const String& v) override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    fNode_->setTextContent (v.empty () ? nullptr : v.As<u16string> ().c_str ());
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual void SetAttribute (const String& attrName, const String& v) override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    DOMElement* element = dynamic_cast<DOMElement*> (fNode_);
                    ThrowIfNull (element);
                    /*
                 * For reasons that elude maybe (maybe because it was standard for XML early on)
                 * all my attributes are free of namespaces. So why use setAttributeNS? Because otherwise
                 * the XQilla code fails to match on the attribute names at all in its XPath stuff.
                 * Considered copying the namespace from the parent element (fNode_->getNamespaceURI()),
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
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    if (fNode_->getNodeType () == DOMNode::ELEMENT_NODE) {
                        AssertMember (fNode_, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                        T_DOMElement* elt = reinterpret_cast<T_DOMElement*> (fNode_);
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
            virtual optional<String> GetAttribute (const String& attrName) const override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    if (fNode_->getNodeType () == DOMNode::ELEMENT_NODE) {
                        AssertMember (fNode_, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                        T_DOMElement* elt = reinterpret_cast<T_DOMElement*> (fNode_);
                        const XMLCh*  s   = elt->getAttribute (attrName.As<u16string> ().c_str ());
                        AssertNotNull (s);
                        if (*s != '\0') {
                            return s;
                        }
                    }
                    return nullopt;
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual Node::Ptr GetFirstAncestorNodeWithAttribute (const String& attrName) const override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    for (T_DOMNode* p = fNode_; p != nullptr; p = p->getParentNode ()) {
                        if (p->getNodeType () == DOMNode::ELEMENT_NODE) {
                            AssertMember (p, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                            const T_DOMElement* elt = reinterpret_cast<const T_DOMElement*> (p);
                            if (elt->hasAttribute (attrName.As<u16string> ().c_str ())) {
                                return WrapImpl_ (p);
                            }
                        }
                    }
                    return nullptr;
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual Node::Ptr InsertChild (const String& name, const optional<URI>& ns, const Node::Ptr& afterNode) override
            {
#if qDebug
                Require (ValidNewNodeName_ (name));
#endif
                START_LIB_EXCEPTION_MAPPER
                {
                    T_XMLDOMDocument* doc = fNode_->getOwnerDocument ();
                    // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                    T_DOMNode* child =
                        doc->createElementNS ((ns == nullopt) ? fNode_->getNamespaceURI () : ns->As<String> ().As<u16string> ().c_str (),
                                              name.As<u16string> ().c_str ());
                    T_DOMNode* refChildNode = nullptr;
                    if (afterNode == nullptr) {
                        // this means PREPEND.
                        // If there is a first element, then insert before it. If no elements, then append is the same thing.
                        refChildNode = fNode_->getFirstChild ();
                    }
                    else {
                        refChildNode = GetInternalRep_ (GetRep4Node (afterNode).get ())->getNextSibling ();
                    }
                    T_DOMNode* childx = fNode_->insertBefore (child, refChildNode);
                    ThrowIfNull (childx);
                    return WrapImpl_ (childx);
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual Node::Ptr AppendChild (const String& name, const optional<URI>& ns) override
            {
#if qDebug
                Require (ValidNewNodeName_ (name));
#endif
                START_LIB_EXCEPTION_MAPPER
                {
                    T_XMLDOMDocument* doc = fNode_->getOwnerDocument ();
                    T_DOMNode*        child{};
                    if (ns) {
                        u16string namespaceURI = ns->As<String> ().As<u16string> ();
                        child                  = doc->createElementNS (namespaceURI.c_str (), name.As<u16string> ().c_str ());
                    }
                    else {
                        const XMLCh* namespaceURI = fNode_->getNamespaceURI ();
                        child                     = doc->createElementNS (namespaceURI, name.As<u16string> ().c_str ());
                    }
                    T_DOMNode* childx = fNode_->appendChild (child);
                    ThrowIfNull (childx);
                    return WrapImpl_ (childx);
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual void DeleteNode () override
            {
                START_LIB_EXCEPTION_MAPPER
                {
                    T_DOMNode* selNode = fNode_;
                    ThrowIfNull (selNode);
                    T_DOMNode* parentNode = selNode->getParentNode ();
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
                END_LIB_EXCEPTION_MAPPER
            }
            virtual Node::Ptr ReplaceNode () override
            {
                RequireNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    T_XMLDOMDocument* doc = fNode_->getOwnerDocument ();
                    ThrowIfNull (doc);
                    T_DOMNode* selNode = fNode_;
                    ThrowIfNull (selNode); // perhaps this should be an assertion?
                    T_DOMNode* parentNode = selNode->getParentNode ();
                    ThrowIfNull (parentNode);
                    DOMElement* n = doc->createElementNS (selNode->getNamespaceURI (), selNode->getNodeName ());
                    (void)parentNode->replaceChild (n, selNode);
                    return WrapImpl_ (n);
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual Node::Ptr GetParentNode () const override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    auto p = fNode_->getParentNode ();
                    return p == nullptr ? nullptr : WrapImpl_ (p);
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual Iterable<Node::Ptr> GetChildren () const override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    return Traversal::CreateGenerator<Node::Ptr> (
                        [sni = SubNodeIterator{Memory::MakeSharedPtr<SubNodeIteratorOver_SiblingList_Rep_> (fNode_)}] () mutable -> optional<Node::Ptr> {
                            if (sni.IsAtEnd ()) {
                                return optional<Node::Ptr>{};
                            }
                            Node::Ptr r = *sni;
                            ++sni;
                            return r;
                        });
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual Node::Ptr GetChildNodeByID (const String& id) const override
            {
                AssertNotNull (fNode_);
                START_LIB_EXCEPTION_MAPPER
                {
                    for (T_DOMNode* i = fNode_->getFirstChild (); i != nullptr; i = i->getNextSibling ()) {
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
                    return nullptr;
                }
                END_LIB_EXCEPTION_MAPPER
            }
            virtual void* GetInternalTRep () override
            {
                return fNode_;
            }

        private:
            nonvirtual XercesDocRep_& GetAssociatedDoc_ () const
            {
                AssertNotNull (fNode_);
                T_XMLDOMDocument* doc = fNode_->getOwnerDocument ();
                AssertNotNull (doc);
                void* docData = doc->getUserData (kXerces2XMLDBDocumentKey_);
                AssertNotNull (docData);
                return *reinterpret_cast<XercesDocRep_*> (docData);
            }

        private:
            // must carefully think out mem managment here - cuz not ref counted - around as long as owning doc...
            DOMNode* fNode_;
        };
        RequireNotNull (n);
        return Node::Ptr{make_shared<MyNodeRep_> (n)};
    }
}

/*
 ********************************************************************************
 ********************** SubNodeIteratorOver_SiblingList_Rep_ ********************
 ********************************************************************************
 */
SubNodeIteratorOver_SiblingList_Rep_::SubNodeIteratorOver_SiblingList_Rep_ (T_DOMNode* nodeParent)
    : fParentNode{nodeParent}
    , fCachedMainListLen{static_cast<size_t> (-1)}
{
    RequireNotNull (nodeParent);
    START_LIB_EXCEPTION_MAPPER
    {
        fCurNode_ = nodeParent->getFirstChild ();
    }
    END_LIB_EXCEPTION_MAPPER
}

bool SubNodeIteratorOver_SiblingList_Rep_::IsAtEnd () const
{
    return fCurNode_ == nullptr;
}

void SubNodeIteratorOver_SiblingList_Rep_::Next ()
{
    Require (not IsAtEnd ());
    AssertNotNull (fCurNode_);
    START_LIB_EXCEPTION_MAPPER
    {
        fCurNode_ = fCurNode_->getNextSibling ();
    }
    END_LIB_EXCEPTION_MAPPER
}

Node::Ptr SubNodeIteratorOver_SiblingList_Rep_::Current () const
{
    return WrapImpl_ (fCurNode_);
}

size_t SubNodeIteratorOver_SiblingList_Rep_::GetLength () const
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
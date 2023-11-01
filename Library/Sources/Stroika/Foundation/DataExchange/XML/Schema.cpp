/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "R4LLibPreComp.h"

#include <fstream>
#include <limits>
#include <set>
#include <sstream>

#include "Stroika/Foundation/Cache/LRUCache.h"
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Configuration/StroikaVersion.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/FileSystem/TemporaryFile.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/MemoryAllocator.h"
#include "Stroika/Foundation/Memory/SmallStackBuffer.h"
#include "Stroika/Foundation/Streams/InputStream.h"

#include "Encryption.h"

#include "XMLDB.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;

// Workaround bug/issue with XQilla 2.3.0 (and later?) - where certain XPath strings
// like those with index-of - only compile if you add "xs" to the list of namespaces,
// even though its not explicitly used in the xpath
//      -- LGP 2012-10-31
//
// Disabled (and used passed in arg with right namespaces to lookups) as of 2013-01-18
#define qRemapXQillaMissingNSExceptionToIncludeXSNamespace 0
#ifndef qRemapXQillaMissingNSExceptionToIncludeXSNamespace
#define qRemapXQillaMissingNSExceptionToIncludeXSNamespace 1
#endif

// See http://bugzilla/show_bug.cgi?id=339
#ifndef qBug339MeansDontBotherWithCRLFMapping
#define qBug339MeansDontBotherWithCRLFMapping 1
#endif

using namespace Progress;

// Generally this makes things run a bit faster, but to help with memleak debugging, its sometimes handy to
// turn off.
// Haven't actually tested this (for speedup) since XQilla 1.0.1, but I suspect it still helps.
//      -- LGP 2009-05-18
#define qAllowOLDCompiledXPathCaching_ 0
#ifndef qAllowOLDCompiledXPathCaching_
#define qAllowOLDCompiledXPathCaching_ 1
#endif

#define qAllowNEWCompiledXPathCaching_ 1
#ifndef qAllowNEWCompiledXPathCaching_
#define qAllowNEWCompiledXPathCaching_ 0
#endif

#if qXMLDBImpl_UseXerces

#define qXMLDBImpl_UseXQillaForXPath 1
#define qXMLDBImpl_UseXalanForXPath 0

#if qXMLDB_SupportXSLT
#define qXMLDBImpl_UseXalanForXSLT 1
#endif

#if qXMLDBImpl_UseXQillaForXPath
#if !defined(qXMLDBImpl_UseXQilla_DOM3API) || !defined(qXMLDBImpl_UseXQilla_DOM3API)
// DOM3API - had trouble with - see if SIMPLEAPI any 'simpler' ;-)
#define qXMLDBImpl_UseXQilla_SIMPLEAPI 1
//#define   qXMLDBImpl_UseXQilla_DOM3API    1
#endif
#endif
#if !defined(qXMLDBImpl_UseXQilla_DOM3API)
#define qXMLDBImpl_UseXQilla_DOM3API 0
#endif
#if !defined(qXMLDBImpl_UseXQilla_SIMPLEAPI)
#define qXMLDBImpl_UseXQilla_SIMPLEAPI 0
#endif

#endif

#if qXMLDBImpl_UseXerces
#ifndef qUseMyXMLDBMemManager
#define qUseMyXMLDBMemManager defined (_DEBUG)
//#define   qUseMyXMLDBMemManager       1
#endif
//#define   qXMLDBTrackAllocs   0
//#define   qXMLDBTrackAllocs   1
#ifndef qXMLDBTrackAllocs
#if qMemoryAllocator_GNUCCLib_MemoryAllocator_CompileBug
#define qXMLDBTrackAllocs 0
#endif
#endif
#ifndef qXMLDBTrackAllocs
// Default OFF for now - since this really slows things down and has been very stable for a very long time
//      -- LGP 2012-11-27
//#define   qXMLDBTrackAllocs       defined (_DEBUG)
#define qXMLDBTrackAllocs 0
#endif
#endif

#if qXMLDBImpl_UseXerces
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
#if defined(_DEBUG)
#define Assert(c)                                                                                                                          \
    {                                                                                                                                      \
        if (!(c)) {                                                                                                                        \
            Stroika::Foundation::Debug::Private::Debug_Trap_ ("Assert", #c, __FILE__, __LINE__, nullptr);                                  \
        }                                                                                                                                  \
    }
#else
#define Assert(c)
#endif
#elif qXMLDBImpl_UseMSXML4
#include <atlbase.h>
#include <msxml2.h>

#include "COMSupport.h"
#endif

#if qXMLDBImpl_UseXalanForXPath || qXMLDBImpl_UseXalanForXSLT
#pragma warning(push)
#pragma warning(4 : 4996)
#define XALAN_INMEM_MSG_LOADER
#include <xalanc/Include/PlatformDefinitions.hpp>
#pragma warning(pop)
#endif
#if qXMLDBImpl_UseXalanForXPath
#pragma warning(push)
#pragma warning(4 : 4996)
#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>
#include <xalanc/XPath/XObject.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#pragma warning(pop)
#endif
#if qXMLDBImpl_UseXalanForXSLT
#pragma warning(push)
#pragma warning(4 : 4996)

#include <xalanc/PlatformSupport/XSLException.hpp>
#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XalanDOM/XalanElement.hpp>
#include <xalanc/XalanTransformer/XalanDefaultParsedSource.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>

#pragma warning(pop)
#endif

#if qXMLDBImpl_UseXQillaForXPath
#if qXMLDBImpl_UseXQilla_DOM3API
#include <xqilla/xqilla-dom3.hpp>
#endif
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
#include <xqilla/xqilla-simple.hpp>
#endif
#endif

#if qXMLDBImpl_UseXerces
XERCES_CPP_NAMESPACE_USE
#endif

#if qXMLDBImpl_UseXalanForXPath || qXMLDBImpl_UseXalanForXSLT
XALAN_CPP_NAMESPACE_USE
#endif

#if qXMLDBImpl_UseXalanForXPath
#define USE_XPATH_NS_QUALIFIER XALAN_CPP_NAMESPACE_QUALIFIER
#endif

#if qXMLDBImpl_UseXerces
#if defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#if qDebug
#pragma comment(lib, "xerces-c_static_3d.lib")
#else
#pragma comment(lib, "xerces-c_static_3.lib")
#endif
#endif
#endif

#if qXMLDBImpl_UseXalanForXSLT
// this appears (for totally mysterios reasons) to work around a bug
// with printing dates (like the date at the top of the medicions summary report)
//      -- LGP 2007-06-26
#define qXalan_XSLT_CompileVIAFileBWA 1
#endif

#if qXMLDBImpl_UseXQillaForXPath
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
namespace {
    const XMLCh* kXercesDOMNodeInterfaceName = XercesConfiguration::gXerces;
    // workaround lack of namespaces!!!
    typedef ::Node::Ptr XQuilla_Node_Ptr;
#define Node XMLDB::Node
}
#endif
#endif

using namespace XMLDB;

#if qXMLDBImpl_UseMSXML4
/*
 *  MSXML version
 *
 *      We are using MSXML 4.0 SP1. Thats because it supports XSD's and earlier versions do not (so
 *  say the docs, and empirically - when I tried - I got E_FAIL messages).
 *
 *      For version 1.0 of ezPHR - we will live with this - and force users to install MSXML 4.0
 *  during the install process. For version 2.0 - we will probably rewrite our XML support using
 *  Xerces C++ XML support (so that we don't require installing any special DLLs). That will be important
 *  for running from an the PC-USB KEY.
 *
 *              -- LGP 2004-04-12
 *
 *      For HF 2.0 (and I think also 1.0.3) - we are using MSXML 4.0 SP2 (thats what we require in our
 *  manifest, and what we install in our installer.
 *
 */
#endif

/*
 *  UnderlyingXMLLibExcptionMapping layer
 */
#if qXMLDBImpl_UseXerces
#define START_LIB_EXCEPTION_MAPPER try {
#define END_LIB_EXCEPTION_MAPPER                                                                                                           \
    }                                                                                                                                      \
    catch (const OutOfMemoryException&)                                                                                                    \
    {                                                                                                                                      \
        Execution::Throw (bad_alloc (), "xerces OutOfMemoryException - throwing bad_alloc");                                               \
    }                                                                                                                                      \
    catch (...)                                                                                                                            \
    {                                                                                                                                      \
        Execution::ReThrow ();                                                                                                             \
    }
#else
#define START_LIB_EXCEPTION_MAPPER
#define END_LIB_EXCEPTION_MAPPER
#endif

#if qXMLDBImpl_UseXerces
namespace {
#if qXMLDBImpl_UseXQilla_DOM3API
    const XMLCh kDOMImplFeatureDeclaration[] = XMLStrL ("XPath2 3.0");
#else
    const XMLCh kDOMImplFeatureDeclaration[] = XMLStrL ("Core");
#endif
    DOMImplementation& GetDOMIMPL_ ()
    {
        // safe to save in a static var? -- LGP 2007-05-20
        // from perusing implementation - this appears safe to cache and re-use in differnt threads
        static DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation (kDOMImplFeatureDeclaration);
        AssertNotNull (impl);
        return *impl;
    }
}
#endif

#if qXMLDBImpl_UseMSXML4
// I read up on lots of stuff about xml:space = 'preserve' attribute, and used XML spy which added xs:whiteSpace facets, and none
// of that seemed to work. This hack was a bit of overkill - but did appear to work adequately for 1.0.
#ifndef qUseSpacePreservingIO
#define qUseSpacePreservingIO 1
#endif
#endif

#ifndef qDumpXMLOnValidationError
#if qDebug
#define qDumpXMLOnValidationError 1
#else
#define qDumpXMLOnValidationError 0
#endif
#endif

// Simple 'roughly analagous' type wrappers - start with 'T_'
namespace {
#if qXMLDBImpl_UseXerces
    typedef XERCES_CPP_NAMESPACE::DOMNode T_DOMNode;
#elif qXMLDBImpl_UseMSXML4
    typedef IXMLDOMNode               T_DOMNode;
#endif

#if qXMLDBImpl_UseXerces
    typedef XERCES_CPP_NAMESPACE::DOMElement T_DOMElement;
#elif qXMLDBImpl_UseMSXML4
    typedef IXMLDOMElement            T_DOMElement;
#endif

#if qXMLDBImpl_UseXerces
    typedef XERCES_CPP_NAMESPACE::DOMDocument T_XMLDOMDocument;
    typedef shared_ptr<T_XMLDOMDocument>      T_XMLDOMDocumentSmartPtr;
#elif qXMLDBImpl_UseMSXML4
    typedef IXMLDOMDocument2          T_XMLDOMDocument;
    typedef CComPtr<IXMLDOMDocument2> T_XMLDOMDocumentSmartPtr;
#endif

#if qXMLDBImpl_UseXerces
    typedef XERCES_CPP_NAMESPACE::DOMNodeList T_DOMNodeList;
#elif qXMLDBImpl_UseMSXML4
    typedef IXMLDOMNodeList           T_DOMNodeList;
#endif
}

#if qXMLDBImpl_UseXerces
namespace {
    /*
     *  A helpful class to isolete Xerces (etc) memory management calls. Could be the basis
     *  of future perfomance/memory optimizations, but for now, just a helpful debugging/tracking
     *  class.
     */
    class MyXercesMemMgr : public MemoryManager {
    public:
        MyXercesMemMgr ()
#if qXMLDBTrackAllocs
            : fBaseAllocator ()
            , fAllocator (fBaseAllocator)
            , fLastSnapshot ()
#endif
        {
        }
        ~MyXercesMemMgr ()
        {
        }

#if qXMLDBTrackAllocs
    public:
        Memory::SimpleAllocator_CallLIBCNewDelete             fBaseAllocator;
        Memory::LeakTrackingGeneralPurposeAllocator           fAllocator;
        recursive_mutex                                       fLastSnapshot_CritSection;
        Memory::LeakTrackingGeneralPurposeAllocator::Snapshot fLastSnapshot;
#endif

    public:
#if qXMLDBTrackAllocs
        void DUMPCurMemStats ()
        {
            TraceContextBumper          ctx (_T ("MyXercesMemMgr::DUMPCurMemStats"));
            lock_guard<recursive_mutex> enterCriticalSection (fLastSnapshot_CritSection);
            fAllocator.DUMPCurMemStats (fLastSnapshot);
            // now copy current map to prev for next time this gets called
            fLastSnapshot = fAllocator.GetSnapshot ();
        }
#endif

    public:
        virtual MemoryManager* getExceptionMemoryManager () override
        {
            return this;
        }
        virtual void* allocate (XMLSize_t size) override
        {
            try {
#if qXMLDBTrackAllocs
                return fAllocator.Allocate (size);
#else
                return ::operator new (size);
#endif
            }
            catch (...) {
                Execution::Throw (OutOfMemoryException ()); // quirk cuz this is the class Xerces expects and catches internally (why not bad_alloc?) - sigh...
            }
        }
        virtual void deallocate (void* p) override
        {
            if (p != nullptr) {
#if qXMLDBTrackAllocs
                return fAllocator.Deallocate (p);
#else
                ::operator delete (p);
#endif
            }
        }
    };
}
#endif

#if defined(_DEBUG)
namespace {
    bool ValidNewNodeName_ (const wstring& n)
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

#if qXMLDBImpl_UseXerces
namespace {

    class StrmFmtTarget : public XMLFormatTarget {
    public:
        ostream& fOut;
        StrmFmtTarget (ostream& ostr)
            : fOut (ostr)
        {
        }
        virtual void writeChars (const XMLByte* const toWrite, const XMLSize_t count, XMLFormatter* const formatter) override
        {
            fOut.write (reinterpret_cast<const char*> (toWrite), sizeof (XMLByte) * count);
        }
        virtual void flush () override
        {
            fOut.flush ();
        }
    };

    // mostly for debugging purposes
    void DoWrite2File (T_XMLDOMDocument* doc, const SDKString& fileName, bool prettyPrint)
    {
        AutoRelease<DOMLSOutput> theOutputDesc = GetDOMIMPL_ ().createLSOutput ();
        theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
        AutoRelease<DOMLSSerializer> writer = GetDOMIMPL_ ().createLSSerializer ();
        DOMConfiguration*            dc     = writer->getDomConfig ();
        dc->setParameter (XMLUni::fgDOMWRTFormatPrettyPrint, prettyPrint);
        dc->setParameter (XMLUni::fgDOMWRTBOM, true);
        LocalFileFormatTarget ftg (fileName.c_str ());
        theOutputDesc->setByteStream (&ftg);
        Assert (doc->getXmlStandalone ());
        writer->write (doc, theOutputDesc);
    }
    void DoWrite2Stream_ (T_XMLDOMDocument* doc, ostream& ostr, bool prettyPrint, LineTerminationMode ltm)
    {
        AutoRelease<DOMLSOutput> theOutputDesc = GetDOMIMPL_ ().createLSOutput ();
        theOutputDesc->setEncoding (XMLUni::fgUTF8EncodingString);
        AutoRelease<DOMLSSerializer> writer = GetDOMIMPL_ ().createLSSerializer ();
        DOMConfiguration*            dc     = writer->getDomConfig ();
        dc->setParameter (XMLUni::fgDOMWRTFormatPrettyPrint, prettyPrint);
        dc->setParameter (XMLUni::fgDOMWRTBOM, true);
#if !qBug339MeansDontBotherWithCRLFMapping
        if (ltm == eCRLF_LTM) {
            writer->setNewLine (L"\r\n");
        }
#endif
        StrmFmtTarget dest (ostr);
        theOutputDesc->setByteStream (&dest);
        Assert (doc->getXmlStandalone ());
        writer->write (doc, theOutputDesc);
    }
    string DoWrite2UTF8String (T_XMLDOMDocument* doc, bool prettyPrint)
    {
        stringstream resultBuf (ios_base::in | ios_base::out | ios_base::binary);
        DoWrite2Stream_ (doc, resultBuf, prettyPrint, eQuickest_LTM);
        return resultBuf.str ();
    }
}
#endif

#if qXMLDBImpl_UseXerces
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
#endif

namespace {
    string GetNodeAsXMLUTF8_ (const T_DOMNode* node)
    {
#if qXMLDBImpl_UseXerces
        // existing implementation from Xerces 2.7 didn't work for Xerces 3.0.1, and since we don't use this anyhow, just assert out!
        Assert (false); // NYI
        return string ();
#elif qXMLDBImpl_UseMSXML4
        CComBSTR attrText;
        ThrowIfErrorHRESULT (const_cast<T_DOMNode*> (node)->get_xml (&attrText));
        return WideStringToUTF8 (BSTR2wstring (attrText));
#endif
    }
}

#if qXMLDBImpl_UseXerces
namespace {
    const XMLCh* kXerces2XMLDBDocumentKey = nullptr; // just a unique key to lookup our doc object from the xerces doc object.
    // Could use real str, then xerces does strcmp() - but this appears slightly faster
    // so long as no conflict....
}
#endif

namespace {
    wstring GetTextForDOMNode_ (const T_DOMNode* node)
    {
        RequireNotNull (node);
#if qXMLDBImpl_UseXerces
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
            wstring r;
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
            return r;
        }
    BackupMode:
        /*
         * Note that this  is SOMETHING OF A (temporary) memory leak. Xerces does free the memory when the document is freed.
         */
        DbgTrace ("WARNING: GetTextForDOMNode_::BackupMode used");
        return node->getTextContent ();
#elif qXMLDBImpl_UseMSXML4
        // REDO using 'dataType' attribute once we have the XML Schema loaded!!!
        CComBSTR t;
        ThrowIfErrorHRESULT (const_cast<T_DOMNode*> (node)->get_text (&t));
        return BSTR2wstring (t);
#endif
    }
}

namespace {
    class SubNodeIteratorOver_vectorDOMNODE_Rep_ : public SubNodeIterator::Rep {
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

    public:
        DECLARE_USE_BLOCK_ALLOCATION (SubNodeIteratorOver_vectorDOMNODE_Rep_);

    private:
        vector<T_DOMNode*> fDOMNodes;
        size_t             fCur;
    };

    class SubNodeIteratorOver_DOMNodeList_Rep : SubNodeIterator::Rep {
    public:
        SubNodeIteratorOver_DOMNodeList_Rep (T_DOMNodeList* dln
#if qXMLDBImpl_UseXerces
#elif qXMLDBImpl_UseMSXML4
                                             ,
                                             IXMLDOMNamedNodeMap* attrs
#endif
        );

    public:
        virtual bool   IsAtEnd () const override;
        virtual void   Next () override;
        virtual Node   Current () const override;
        virtual size_t GetLength () const override;

    public:
        DECLARE_USE_BLOCK_ALLOCATION (SubNodeIteratorOver_DOMNodeList_Rep);

    private:
#if qXMLDBImpl_UseXerces
        T_DOMNodeList* fMainNodeList;
#elif qXMLDBImpl_UseMSXML4
        ATL::CComPtr<IXMLDOMNodeList>     fMainNodeList;
        ATL::CComPtr<IXMLDOMNamedNodeMap> fAttrsList;
#endif
        size_t fMainListLen;
        size_t fAttrsListLen;
        size_t fCur;
    };

    class SubNodeIteratorOver_SiblingList_Rep : public SubNodeIterator::Rep {
    public:
        // Called iterates over CHILDREN of given parentNode
        SubNodeIteratorOver_SiblingList_Rep (T_DOMNode* nodeParent);

    public:
        virtual bool   IsAtEnd () const override;
        virtual void   Next () override;
        virtual Node   Current () const override;
        virtual size_t GetLength () const override;

    public:
        DECLARE_USE_BLOCK_ALLOCATION (SubNodeIteratorOver_SiblingList_Rep);

    private:
        T_DOMNode*     fParentNode;
        T_DOMNode*     fCurNode;
        mutable size_t fCachedMainListLen;
    };

    Node WrapImpl_ (T_DOMNode* n);
}

namespace {
#if qXMLDBImpl_UseXerces
    class MyErrorReproter : public XMLErrorReporter, public ErrorHandler {
        // XMLErrorReporter
    public:
        virtual void error (const unsigned int errCode, const XMLCh* const errDomain, const ErrTypes type, const XMLCh* const errorText,
                            const XMLCh* const systemId, const XMLCh* const publicId, const XMLFileLoc lineNum, const XMLFileLoc colNum) override
        {
            Execution::Throw (BadFormatException (errorText, static_cast<unsigned int> (lineNum), static_cast<unsigned int> (colNum), 0));
        }
        virtual void resetErrors () override
        {
        }

        // ErrorHandler
    public:
        virtual void warning (const SAXParseException& exc) override
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
#elif qXMLDBImpl_UseMSXML4
    void TranslateDOMParseErrorToExceptionIfAny (CComPtr<IXMLDOMParseError> dpe)
    {
        if (dpe.p != nullptr) {
            long errorCode = 0;
            ThrowIfErrorHRESULT (dpe->get_errorCode (&errorCode));
            if (errorCode == 0) {
                return;
            }
            CComBSTR reason;
            ThrowIfErrorHRESULT (dpe->get_reason (&reason));
            CComBSTR srcText;
            ThrowIfErrorHRESULT (dpe->get_srcText (&srcText));
            long lineNumber = 0;
            ThrowIfErrorHRESULT (dpe->get_line (&lineNumber));
            long linePosition = 0;
            ThrowIfErrorHRESULT (dpe->get_linepos (&linePosition));
            long filePosition = 0;
            ThrowIfErrorHRESULT (dpe->get_filepos (&filePosition));
            wstring reasonText = Trim (BSTR2wstring (reason));
            Execution::Throw (BadFormatException (reasonText, lineNumber, linePosition, filePosition));
        }
    }
#endif
}

#if qXMLDBImpl_UseMSXML4
// this code is mostly working, but has some small bug with mixed content
// to reproduce, try using the XSL transform, and look at the results of AAFP stylesheet
// where you have foo<br>bar
//
// also, its probably slwoer than the DOMNode writer stuff, and probably less functional
// so dont bother fixing - just dont use...
//      -- LGP 2007-07-01
namespace {
    bool HasSubElements_ (T_DOMNode* n)
    {
        RequireNotNull (n);
        /*
         *  Check if n has any subnodes of type 'ELEMENT'
         */
#if qXMLDBImpl_UseXerces
        if (n->hasChildNodes ()) {
            for (T_DOMNode* cur = n->getFirstChild (); cur != nullptr; cur = cur->getNextSibling ()) {
                if (dynamic_cast<DOMElement*> (cur) != nullptr) {
                    return true;
                }
            }
        }
#elif qXMLDBImpl_UseMSXML4
        VARIANT_BOOL hasChildren = VARIANT_FALSE;
        ThrowIfErrorHRESULT (n->hasChildNodes (&hasChildren));
        if (hasChildren) {
            CComPtr<IXMLDOMNode> cur;
            ThrowIfErrorHRESULT (n->get_firstChild (&cur));
            while (cur.p != nullptr) {
                CComQIPtr<IXMLDOMElement> elt = cur;
                if (elt.p != nullptr) {
                    return true;
                }
                CComPtr<IXMLDOMNode> next;
                ThrowIfErrorHRESULT (cur->get_nextSibling (&next));
                cur = next;
            }
        }
#endif
        return false;
    }
    string WriteNodeToString (T_DOMNode* n, unsigned int indentLevel = 0)
    {
#if qXMLDBImpl_UseXerces
        // this code is mostly working, but has some small bug with mixed content
        // to reproduce, try using the XSL transform, and look at the results of AAFP stylesheet
        // where you have foo<br>bar
        //
        // also, its probably slwoer than the DOMNode writer stuff, and probably less functional
        // so dont bother fixing - just dont use...
        //      -- LGP 2007-07-01
        Assert (false);
#endif
        string r;
        r.reserve (100); // a good minimum (typical)

        if (indentLevel == 0) {
            r = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
        }

        for (unsigned int i = 0; i < indentLevel; ++i) {
            r += "\t";
        }
        size_t savedRLength = r.length ();

#if qXMLDBImpl_UseXerces
        wstring nodeName = n->getNodeName ();
#elif qXMLDBImpl_UseMSXML4
        wstring nodeName;
        {
            CComBSTR bstrNodeName;
            ThrowIfErrorHRESULT (n->get_nodeName (&bstrNodeName));
            nodeName = BSTR2wstring (bstrNodeName);
        }
#endif
        string nodeName_UTF8 = WideStringToUTF8 (nodeName);

#if qUseSpacePreservingIO
        const wstring kTEXTFRAG = L"#text";
        if (nodeName == kTEXTFRAG) {
            return string ();
        }
#endif
        Containers::ReserveSpeedTweekAddN (r, 2 * nodeName_UTF8.length () + 10);
        r += "<" + nodeName_UTF8;
        // Add attributes
        {
#if qXMLDBImpl_UseXerces
            DOMNamedNodeMap* attrMap = n->getAttributes ();
            if (attrMap != nullptr) {
                size_t len = attrMap->getLength ();
                for (size_t i = 0; i < len; ++i) {
                    T_DOMNode* attr = attrMap->item (i);
                    r += " " + GetNodeAsXMLUTF8_ (attr);
                }
            }
            if (indentLevel == 0 && n->getNamespaceURI () != nullptr) {
                r += string (" xmlns=\"") + WideStringToUTF8 (n->getNamespaceURI ()) + "\"";
            }
#elif qXMLDBImpl_UseMSXML4
            CComPtr<IXMLDOMNamedNodeMap> attrMap;
            ThrowIfErrorHRESULT (n->get_attributes (&attrMap));
            if (attrMap.p != nullptr) {
                long len = 0;
                ThrowIfErrorHRESULT (attrMap->get_length (&len));
                for (long i = 0; i < len; ++i) {
                    CComPtr<IXMLDOMNode> attr;
                    ThrowIfErrorHRESULT (attrMap->get_item (i, &attr));
                    r += " " + GetNodeAsXMLUTF8_ (attr);
                }
            }
#endif
        }
        r += ">";
        if (HasSubElements_ (n)) {
            bool loopFirstTime = true;
#if qXMLDBImpl_UseXerces
            for (T_DOMNode* cur = n->getFirstChild (); cur != nullptr; cur = cur->getNextSibling ()) {
                if (loopFirstTime) {
                    r += "\n";
                    loopFirstTime = false;
                }
                string tmp = WriteNodeToString (cur, indentLevel + 1);
                Containers::ReserveSpeedTweekAddN (r, tmp.length () + 10);
                r += tmp;
            }
#elif qXMLDBImpl_UseMSXML4
            CComPtr<IXMLDOMNode> cur;
            ThrowIfErrorHRESULT (n->get_firstChild (&cur));
            while (cur.p != nullptr) {
                if (loopFirstTime) {
                    r += "\n";
                    loopFirstTime = false;
                }
                string tmp = WriteNodeToString (cur, indentLevel + 1);
                Containers::ReserveSpeedTweekAddN (r, tmp.length () + 10);
                r += tmp;
                CComPtr<IXMLDOMNode> next;
                ThrowIfErrorHRESULT (cur->get_nextSibling (&next));
                cur = next;
            }
#endif
            if (savedRLength != r.length ()) {
                for (unsigned int i = 0; i < indentLevel; ++i) {
                    r += "\t";
                }
            }
            r += "</" + nodeName_UTF8 + ">\n";
        }
        else {
            size_t sizeAtStartOfText = r.length ();
            // TEXT Node
#if qXMLDBImpl_UseXerces
            string tmp = QuoteForXML (WideStringToUTF8 (GetTextForDOMNode (n)));
            Containers::ReserveSpeedTweekAddN (r, tmp.length () + nodeName_UTF8.length () + 5);
            r += tmp;
#elif qXMLDBImpl_UseMSXML4
            CComPtr<IXMLDOMNode> cur;
            ThrowIfErrorHRESULT (n->get_firstChild (&cur));
            while (cur.p != nullptr) {
                CComBSTR text;
                ThrowIfErrorHRESULT (n->get_text (&text));
                string tmp = QuoteForXML (BSTRStringToUTF8 (text));
                Containers::ReserveSpeedTweekAddN (r, tmp.length () + nodeName_UTF8.length () + 5);
                r += tmp;
                CComPtr<IXMLDOMNode> next;
                ThrowIfErrorHRESULT (cur->get_nextSibling (&next));
                cur = next;
            }
#endif
            if (sizeAtStartOfText == r.length ()) {
                r.erase (r.length () - 1); // erase close bracket
                r += "/>";
            }
            else {
                r += "</" + nodeName_UTF8 + ">";
            }
            r += "\n";
        }
        return r;
    }
}
#endif

namespace {
#if qXMLDBImpl_UseXerces
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
#endif
}

namespace {
    inline void MakeXMLDoc_ (T_XMLDOMDocumentSmartPtr& newXMLDoc)
    {
        Require (newXMLDoc == nullptr);
#if qXMLDBImpl_UseXerces
        newXMLDoc = T_XMLDOMDocumentSmartPtr (GetDOMIMPL_ ().createDocument (0, nullptr, 0));
        newXMLDoc->setXmlStandalone (true);
#elif qXMLDBImpl_UseMSXML4
        try {
            ThrowIfErrorHRESULT (newXMLDoc.CoCreateInstance (__uuidof (DOMDocument40)));
        }
        catch (Execution::Platform::Windows::HRESULTErrorException& hr) {
            DbgTrace ("MakeXMLDoc_ exception (0x%x)", static_cast<HRESULT> (hr));
            if (static_cast<HRESULT> (hr) == REGDB_E_CLASSNOTREG) {
                // I cannot imagine any other reason for this to fail -- LGP 2005-11-16
                Execution::Throw (RequiredComponentVersionMismatchException (L"MSXML", L"4.0SP2"));
            }
            Execution::ReThrow ();
        }
#endif
    }

}

#if qXMLDBImpl_UseXQillaForXPath
namespace {
#if qXMLDBImpl_UseXQillaForXPath && qXMLDBImpl_UseXQilla_SIMPLEAPI
    XQilla&              GetXQilla ();
    XercesConfiguration& GetXQillaXercesConfiguration ();
#endif

    /*
     *  The whole point of the CompiledXPath and CompiledXPathMgr is a speed tweek, without which, the Xqilla stuff
     *  is quite slow compared with MSXML.
     *      -- LGP 2007-06-16
     */
    class CompiledXPath {
    public:
        CompiledXPath ()                     = delete;
        CompiledXPath (const CompiledXPath&) = default;
        CompiledXPath (const wstring& xPathQuery, const NamespaceDefinitionsList& namespaces)
#if qXMLDBImpl_UseXQilla_DOM3API
            : fXPathQuery ()
#elif qXMLDBImpl_UseXQilla_SIMPLEAPI
            : fCompiledQuery ()
            , fDynamicContext ()
#endif
            , fCriticalSection (new recursive_mutex ())
        {
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
            XQilla& xq = GetXQilla ();

            DynamicContext* staticContext = nullptr; // dont delete cuz delted by XQQuery object
            {
                vector<NamespaceDefinition> nsDefs = namespaces.GetNamespaces ();
                for (vector<NamespaceDefinition>::const_iterator ndi = nsDefs.begin (); ndi != nsDefs.end (); ++ndi) {
                    if (not ndi->fPrefix.empty () and not ndi->fURI.empty ()) {
                        if (staticContext == nullptr) {
                            staticContext = xq.createContext (XQilla::XPATH2, &GetXQillaXercesConfiguration ());
                        }
                        AssertNotNull (staticContext);
                        staticContext->setNamespaceBinding (ndi->fPrefix.c_str (), ndi->fURI.c_str ());
                    }
                }
            }
            try {
                fCompiledQuery.reset (xq.parse (xPathQuery.c_str (), staticContext));
            }
            catch (...) {
#if qRemapXQillaMissingNSExceptionToIncludeXSNamespace
                ReThrow ("Failed parsing xPathQuery");
#endif
                AssertNotReached ();
            }
            ThrowIfNull (fCompiledQuery);
            fDynamicContext.reset (fCompiledQuery->createDynamicContext ());
            ThrowIfNull (fDynamicContext);
#endif
        }

    public:
        nonvirtual T_DOMNode* DoXPathSingleNodeQuery (T_DOMNode* startNode)
        {
            RequireNotNull (startNode);
            lock_guard<recursive_mutex> enterCriticalSection (*fCriticalSection);
            try {
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
                XercesConfiguration& cfg = GetXQillaXercesConfiguration ();
                XQuilla_Node_Ptr     p   = cfg.createNode (startNode, fDynamicContext.get ());
                fDynamicContext->setContextItem (p);
                Result r = fCompiledQuery->execute (fDynamicContext.get ());
                for (Item::Ptr i = r->next (fDynamicContext.get ()); i != nullptr; i = r->next (fDynamicContext.get ())) {
                    if (i->isNode ()) {
                        T_DOMNode* elemNode = reinterpret_cast<T_DOMNode*> (i->getInterface (kXercesDOMNodeInterfaceName));
                        AssertNotNull (elemNode);
                        fDynamicContext->clearDynamicContext ();
                        return elemNode;
                    }
                    else {
                        Assert (false);
                    }
                }
                fDynamicContext->clearDynamicContext ();
#elif qXMLDBImpl_UseXQilla_DOM3API
                AutoRelease<DOMXPathExpression> expression (const_cast<T_XMLDOMDocument*> (doc)->createExpression (fXPathQuery.c_str (), nullptr));
                AutoRelease<DOMXPathResult> result (expression->evaluate (startNode, DOMXPathResult::ITERATOR_RESULT_TYPE, 0));
                if (result->iterateNext ()) {
                    return result->getNodeValue ();
                }
#endif
            }
            catch (...) {
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
                fDynamicContext->clearDynamicContext ();
#endif
                Execution::ReThrow ();
            }
            return nullptr;
        }
        nonvirtual vector<T_DOMNode*> DoXPathMultiNodeQuery (T_DOMNode* startNode)
        {
            RequireNotNull (startNode);
            vector<T_DOMNode*>          v;
            lock_guard<recursive_mutex> enterCriticalSection (*fCriticalSection);
            try {
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
                XercesConfiguration& cfg = GetXQillaXercesConfiguration ();
                XQuilla_Node_Ptr     p   = cfg.createNode (startNode, fDynamicContext.get ());
                fDynamicContext->setContextItem (p);
                Result r = fCompiledQuery->execute (fDynamicContext.get ());
                for (Item::Ptr i = r->next (fDynamicContext.get ()); i != nullptr; i = r->next (fDynamicContext.get ())) {
                    if (i->isNode ()) {
                        T_DOMNode* elemNode = reinterpret_cast<T_DOMNode*> (i->getInterface (kXercesDOMNodeInterfaceName));
                        AssertNotNull (elemNode);
                        Containers::ReserveSpeedTweekAdd1 (v);
                        v.push_back (elemNode);
                    }
                    else {
                        Assert (false);
                    }
                }
                fDynamicContext->clearDynamicContext ();
#elif qXMLDBImpl_UseXQilla_DOM3API
                // redo using some other way to create the expression not associated with teh docuemnt!!!
                // so we can cache...
                AutoRelease<DOMXPathExpression> expression (const_cast<T_XMLDOMDocument*> (doc)->createExpression (fXPathQuery.c_str (), nullptr));
                AutoRelease<DOMXPathResult> result (expression->evaluate (startNode, DOMXPathResult::ITERATOR_RESULT_TYPE, 0));
                while (result->iterateNext ()) {
                    T_DOMNode* elemNode = result->getNodeValue ();
                    AssertNotNull (elemNode);
                    Containers::ReserveSpeedTweekAdd1 (v);
                    v.push_back (elemNode);
                }
#endif
            }
            catch (...) {
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
                fDynamicContext->clearDynamicContext ();
#endif
                Execution::ReThrow ();
            }
            return v;
        }

    private:
#if qXMLDBImpl_UseXQilla_DOM3API
        wstring fXPathQuery;
#endif
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
        shared_ptr<XQQuery>        fCompiledQuery;
        shared_ptr<DynamicContext> fDynamicContext;
#endif
        shared_ptr<recursive_mutex> fCriticalSection;
    };
    class CompiledXPathMgr {
    private:
        struct INFO_ {
            INFO_ (const wstring& xPathQuery, const NamespaceDefinitionsList& namespaces)
                : fXPathQuery (xPathQuery)
                , fMNamespaces (namespaces)
            {
            }
            wstring                  fXPathQuery;
            NamespaceDefinitionsList fMNamespaces;
        };
#if qAllowOLDCompiledXPathCaching_
        struct LRUCachedCompiledXPathElt_ : INFO_ {
            LRUCachedCompiledXPathElt_ ()
                : INFO_ (wstring (), NamespaceDefinitionsList ())
                , fCompiledXPath ()
            {
            }
            LRUCachedCompiledXPathElt_ (const wstring& xPathQuery, const NamespaceDefinitionsList& namespaces)
                : INFO_ (xPathQuery, namespaces)
                , fCompiledXPath (CompiledXPath (xPathQuery, namespaces))
            {
            }
            LRUCachedCompiledXPathElt_ (const wstring& xPathQuery, const NamespaceDefinitionsList& namespaces, CompiledXPath&& compiledXPath)
                : INFO_ (xPathQuery, namespaces)
                , fCompiledXPath (std::move (compiledXPath))
            {
            }
            Memory::Optional<CompiledXPath> fCompiledXPath;
        };
        struct CompiledXPath_LRUCachedTraits : Cache::DefaultTraits_LEGACY<LRUCachedCompiledXPathElt_, INFO_> {
            DEFINE_CONSTEXPR_CONSTANT (uint8_t, HASH_TABLE_SIZE, 11);
            static bool Equal (const KeyType& lhs, const KeyType& rhs)
            {
                return (lhs.fXPathQuery == rhs.fXPathQuery) and (lhs.fMNamespaces == rhs.fMNamespaces);
            }
            static size_t Hash (const KeyType& e)
            {
                size_t n = 0;
                for (auto i = e.fXPathQuery.begin (); i != e.fXPathQuery.end (); ++i) {
                    n += *i;
                }
                return n;
            }
        };
#endif

    private:
#if qAllowOLDCompiledXPathCaching_
        Cache::LRUCache_LEGACY<LRUCachedCompiledXPathElt_, CompiledXPath_LRUCachedTraits> fCache_;
        recursive_mutex                                                                   fCriticalSection;
        DEFINE_CONSTEXPR_CONSTANT (size_t, LRU_CACHE_SIZE_, 100); // for now decent guess... should test more...
#endif
#if qAllowNEWCompiledXPathCaching_
        DEFINE_CONSTEXPR_CONSTANT (size_t, LRU_CACHE_SIZE_, 100); // for now decent guess... should test more...
        struct MyINFO2CompiledXPathCacheTraits_ : Cache::LRUCacheSupport::DefaultTraits<INFO_, CompiledXPath, 43> {
            struct KeyEqualsCompareFunctionType {
                static bool Equals (const INFO_& lhs, const INFO_& rhs)
                {
                    return (lhs.fXPathQuery == rhs.fXPathQuery) and (lhs.fMNamespaces == rhs.fMNamespaces);
                }
            };
            static size_t Hash (const INFO_& e)
            {
                using Cryptography::Digest::Digester;
                using Cryptography::Digest::Algorithm::Jenkins;
                using USE_DIGESTER_ = Digester<Jenkins>;
                return Cryptography::Hash<USE_DIGESTER_, wstring, size_t> (e.fXPathQuery);
            }
        };
        Execution::Synchronized<Cache::LRUCache<INFO_, CompiledXPath, MyINFO2CompiledXPathCacheTraits_>> fCache_;
#endif

    public:
        CompiledXPathMgr ()
#if qAllowOLDCompiledXPathCaching_
            : fCache_ (LRU_CACHE_SIZE_)
            , fCriticalSection ()
#endif
#if qAllowNEWCompiledXPathCaching_
            : fCache_ (LRU_CACHE_SIZE_)
#endif
        {
        }

    public:
        nonvirtual CompiledXPath GetCompiledXPath (const wstring& xPathQuery, const NamespaceDefinitionsList& mNamespace)
        {
#if qAllowOLDCompiledXPathCaching_
            INFO_                       info = INFO_ (xPathQuery, mNamespace);
            lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
            LRUCachedCompiledXPathElt_* i = fCache_.LookupElement (info);
            if (i == nullptr) {
#if qRemapXQillaMissingNSExceptionToIncludeXSNamespace
                try {
                    LRUCachedCompiledXPathElt_ e = LRUCachedCompiledXPathElt_ (xPathQuery, mNamespace);
                    i                            = fCache_.AddNew (info);
                    *i                           = e;
                }
                catch (...) {
                    NamespaceDefinitionsList useNamespaceList = mNamespace;
                    useNamespaceList.Add (L"http://www.w3.org/2001/XMLSchema", L"xs");
                    LRUCachedCompiledXPathElt_ e = LRUCachedCompiledXPathElt_ (xPathQuery, useNamespaceList, useNamespaceList);
                    i                            = fCache_.AddNew (info);
                    *i                           = e;
                }
#else
                CompiledXPath e = CompiledXPath (xPathQuery, mNamespace);
                i               = fCache_.AddNew (info);
                *i              = LRUCachedCompiledXPathElt_ (xPathQuery, mNamespace, std::move (e));
#endif
            }
            AssertNotNull (i);
            return *i->fCompiledXPath;
#elif qAllowNEWCompiledXPathCaching_
            return fCache_->LookupValue (INFO_ (xPathQuery, mNamespace),
                                         [] (const INFO_& info) { return CompiledXPath (info.fXPathQuery, info.fMNamespaces); });
#else
            return CompiledXPath (xPathQuery, mNamespace);
#endif
        }
    };
}
#endif

/*
 ********************************************************************************
 ************************ Private::UsingModuleHelper ****************************
 ********************************************************************************
 */
namespace {
#if defined(_DEBUG)
    LONG sStdIStream_InputStream_COUNT = 0;
#endif

    /*
     *  Would be NICE to not need to do this, but due to the sad quirk in C++ about
     *  order of construction of static objects across .obj files, its helpful to abstract out
     *  these details and construct access to the libraries as late as possible (but still
     *  allow them to be destroyed automatically during static object destruction.
     *
     *  Note - this workaround to the bug may NOT be sufficeint. I believe it IS sufficient
     *  on the construction side (so long as we assure this module isn't accessed until after static
     *  construction (start of main).
     *
     *  But on destruction - it COULD be we destroy stuff too late. I THINK that's why I couldn't
     *  shutdown Xalan from here - but had to seperately when the number of objects hit
     *  zero. I suppose we could do the SAME trick with # Document::Rep objects.
     *
     *  But - this seems adequate for now...
     *
     *      -- LGP 2007-07-03
     */
    struct UsingLibInterHelper {
#if qXMLDBImpl_UseMSXML4
        struct UsingLibInterHelper_MSXML {
            UsingLibInterHelper_MSXML ()
                : fCOMInitializeHelper (nullptr)
            {
                fCOMInitializeHelper = new COMInitializeHelper (COMInitializeHelper::eApartmentThreaded);
            }
            ~UsingLibInterHelper_MSXML ()
            {
                delete fCOMInitializeHelper;
            }
            COMInitializeHelper* fCOMInitializeHelper;
        };
        UsingLibInterHelper_MSXML fMSXML;
#endif
#if qXMLDBImpl_UseXerces
        struct UsingLibInterHelper_XERCES {
            MyXercesMemMgr* fUseXercesMemoryManager;
            UsingLibInterHelper_XERCES ()
                : fUseXercesMemoryManager (nullptr)
            {
#if qUseMyXMLDBMemManager
                fUseXercesMemoryManager = new MyXercesMemMgr ();
#endif
                XMLPlatformUtils::Initialize (XMLUni::fgXercescDefaultLocale, 0, 0, fUseXercesMemoryManager);
            }
            ~UsingLibInterHelper_XERCES ()
            {
                TraceContextBumper ctx (_T ("~UsingLibInterHelper_XERCES"));
                XMLPlatformUtils::Terminate ();
                Assert (sStdIStream_InputStream_COUNT == 0);
#if qUseMyXMLDBMemManager
                delete fUseXercesMemoryManager;
#endif
            }
        };
        UsingLibInterHelper_XERCES fXERCES;
#endif
#if qXMLDBImpl_UseXQillaForXPath
        struct UsingLibInterHelper_XQillaForXPath {
            UsingLibInterHelper_XQillaForXPath (MemoryManager* memMgr = XMLPlatformUtils::fgMemoryManager)
                : //fXQillaBuf (),
                fCompiledXPathMgr (nullptr)
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
                , fXQilla (nullptr)
                , fXercesConf ()
#endif
            {
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
                // must use placement new for XQilla because it has XMemory as base-class which overrides operator new
                fXQilla = new (&fXQillaBuf) XQilla (memMgr);
#else
                XQillaPlatformUtils::initialize ();
#endif
                fCompiledXPathMgr = new CompiledXPathMgr ();
            }
            ~UsingLibInterHelper_XQillaForXPath ()
            {
                TraceContextBumper ctx (_T ("~UsingLibInterHelper_XQillaForXPath"));
                delete fCompiledXPathMgr;
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
                fXQilla->~XQilla ();
#else
                XQillaPlatformUtils::terminate ();
#endif
            }
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
            Byte fXQillaBuf[sizeof (XQilla)];
#endif
            CompiledXPathMgr* fCompiledXPathMgr;
#if qXMLDBImpl_UseXQilla_SIMPLEAPI
            XQilla*             fXQilla;
            XercesConfiguration fXercesConf;
#endif
        };
        UsingLibInterHelper_XQillaForXPath fXQillaForXPath;
#endif

        UsingLibInterHelper ();
    };
    UsingLibInterHelper::UsingLibInterHelper ()
        :
#if qXMLDBImpl_UseMSXML4
        fMSXML ()
#endif
#if qXMLDBImpl_UseXerces
            fXERCES ()
#endif
#if qXMLDBImpl_UseXQillaForXPath
        , fXQillaForXPath ()
#endif
    {
    }
}

namespace {
    static unsigned int  sUsingModuleHelper_Count;
    UsingLibInterHelper* sUsingLibInterHelper = nullptr;

#if qXMLDBImpl_UseXQillaForXPath
    inline CompiledXPathMgr& GetCompiledXPathMgr_ ()
    {
        AssertNotNull (sUsingLibInterHelper);
        EnsureNotNull (sUsingLibInterHelper->fXQillaForXPath.fCompiledXPathMgr);
        return *sUsingLibInterHelper->fXQillaForXPath.fCompiledXPathMgr;
    }
#if qXMLDBImpl_UseXQillaForXPath && qXMLDBImpl_UseXQilla_SIMPLEAPI
    XQilla& GetXQilla ()
    {
        AssertNotNull (sUsingLibInterHelper);
        EnsureNotNull (sUsingLibInterHelper->fXQillaForXPath.fXQilla);
        return *sUsingLibInterHelper->fXQillaForXPath.fXQilla;
    }
    XercesConfiguration& GetXQillaXercesConfiguration ()
    {
        AssertNotNull (sUsingLibInterHelper);
        return sUsingLibInterHelper->fXQillaForXPath.fXercesConf;
    }
#endif
#endif
}

XMLDB::Private::UsingModuleHelper::UsingModuleHelper ()
{
    TraceContextBumper ctx (_T ("XMLDB::Private::UsingModuleHelper::UsingModuleHelper"));
    Require (sUsingLibInterHelper == nullptr);
    sUsingLibInterHelper = new UsingLibInterHelper ();
}

XMLDB::Private::UsingModuleHelper::~UsingModuleHelper ()
{
    TraceContextBumper ctx (_T ("XMLDB::Private::UsingModuleHelper::~UsingModuleHelper"));
    RequireNotNull (sUsingLibInterHelper);
    delete sUsingLibInterHelper;
    sUsingLibInterHelper = nullptr;
}

#if qXMLDBImpl_UseXerces
namespace {
    // These SHOULD be part of xerces! Perhaps someday post them?
    class BinaryInputStream_InputSource : public InputSource {
    protected:
        class _MyInputStrm : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream {
        public:
            _MyInputStrm (Streams::InputStream<Byte> in)
                : _fSource (in)
            {
#if defined(_DEBUG)
                ::InterlockedIncrement (&sStdIStream_InputStream_COUNT);
#endif
            }
            ~_MyInputStrm ()
            {
#if defined(_DEBUG)
                ::InterlockedDecrement (&sStdIStream_InputStream_COUNT);
#endif
            }

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
                return _fSource.Read (toFill, toFill + maxToRead);
            }
            virtual const XMLCh* getContentType () const override
            {
                return nullptr;
            }

        protected:
            Streams::InputStream<Byte> _fSource;
        };

    public:
        BinaryInputStream_InputSource (Streams::InputStream<Byte> in, const XMLCh* const bufId = nullptr)
            : InputSource (bufId)
            , fSource (in)
        {
        }
        virtual BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) _MyInputStrm (fSource);
        }

    protected:
        Streams::InputStream<Byte> fSource;
    };

    // my variations of StdIInputSrc with progresstracker callback
    class BinaryInputStream_InputSource_WithProgress : public BinaryInputStream_InputSource {
    protected:
        class _InStrWithProg : public BinaryInputStream_InputSource_WithProgress::_MyInputStrm {
        public:
            _InStrWithProg (Streams::InputStream<Byte> in, ProgressStatusCallback* progressCallback)
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
                    fProgress.SetCurrentProgress (curOffset / fTotalSize);
                }
                XMLSize_t result = _MyInputStrm::readBytes (toFill, maxToRead);
                if (fTotalSize > 0) {
                    curOffset = static_cast<float> (_fSource.GetOffset ());
                    fProgress.SetCurrentProgress (curOffset / fTotalSize);
                }
                return result;
            }

        private:
            float           fTotalSize;
            ProgressSubTask fProgress;
        };

    public:
        BinaryInputStream_InputSource_WithProgress (Streams::InputStream<Byte> in, ProgressStatusCallback* progressCallback,
                                                    const XMLCh* const bufId = nullptr)
            : BinaryInputStream_InputSource (in, bufId)
            , fProgressCallback (progressCallback)
        {
        }
        virtual BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) _InStrWithProg (fSource, fProgressCallback);
        }

    private:
        ProgressStatusCallback* fProgressCallback;
    };
}
#endif

#if qXMLDBImpl_UseXQillaForXPath
namespace {
    T_DOMNode* DoXPathSingleNodeQuery_ (const NamespaceDefinitionsList& mNamespaces, T_DOMNode* startNode, const wstring& xPathQuery)
    {
        RequireNotNull (startNode);
        return GetCompiledXPathMgr_ ().GetCompiledXPath (xPathQuery, mNamespaces).DoXPathSingleNodeQuery (startNode);
    }
    vector<T_DOMNode*> DoXPathMultiNodeQuery_ (const NamespaceDefinitionsList& mNamespaces, T_DOMNode* startNode, const wstring& xPathQuery)
    {
        RequireNotNull (startNode);
        return GetCompiledXPathMgr_ ().GetCompiledXPath (xPathQuery, mNamespaces).DoXPathMultiNodeQuery (startNode);
    }
}
#endif

#if qXMLDBImpl_UseXerces
class XMLDB::Private::MyMaybeSchemaDOMParser {
public:
    shared_ptr<Schema::AccessCompiledXSD> fSchemaAccessor;
    shared_ptr<XercesDOMParser>           fParser;

    MyMaybeSchemaDOMParser (const Schema* schema)
        : fSchemaAccessor ()
        , fParser ()
    {
        if (schema != nullptr and schema->HasSchema ()) {
            // REALLY need READLOCK - cuz this just prevents UPDATE of Schema (never happens anyhow) -- LGP 2009-05-19
            fSchemaAccessor = shared_ptr<Schema::AccessCompiledXSD> (new Schema::AccessCompiledXSD (*schema));

            fParser = shared_ptr<XercesDOMParser> (new XercesDOMParser (nullptr, XMLPlatformUtils::fgMemoryManager, fSchemaAccessor->GetCachedTRep ()));
            fParser->cacheGrammarFromParse (false);
            fParser->useCachedGrammarInParse (true);
            fParser->setDoSchema (true);
            fParser->setValidationScheme (AbstractDOMParser::Val_Always);
            fParser->setValidationSchemaFullChecking (true);
            fParser->setIdentityConstraintChecking (true);
            fParser->setErrorHandler (&sMyErrorReproter);
        }
        else {
            fParser = shared_ptr<XercesDOMParser> (new XercesDOMParser ());
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
using XMLDB::Private::MyMaybeSchemaDOMParser;
#endif

/*
 ********************************************************************************
 ********************* XMLDB::Private::DumpDebugInfo ****************************
 ********************************************************************************
 */
#if qDefaultTracingOn
void XMLDB::Private::DumpDebugInfo ()
{
#if qXMLDBTrackAllocs
    AssertNotNull (sUsingLibInterHelper);
    AssertNotNull (sUsingLibInterHelper->fXERCES.fUseXercesMemoryManager);
    sUsingLibInterHelper->fXERCES.fUseXercesMemoryManager->DUMPCurMemStats ();
#endif
}
#endif

/*
 ********************************************************************************
 ************************** NamespaceDefinitionsList ****************************
 ********************************************************************************
 */
void NamespaceDefinitionsList::Add (const wstring& uri, const wstring& prefix)
{
#if defined(_DEBUG)
    for (vector<NamespaceDefinition>::const_iterator i = fNamespaces.begin (); i != fNamespaces.end (); ++i) {
        Assert (uri != i->fURI);
        Assert (prefix.empty () or prefix != i->fPrefix);
    }
#endif
    fNamespaces.push_back (NamespaceDefinition (uri, prefix));
}

bool XMLDB::operator< (const NamespaceDefinitionsList& lhs, const NamespaceDefinitionsList& rhs)
{
    vector<NamespaceDefinition>::const_iterator li = lhs.fNamespaces.begin ();
    vector<NamespaceDefinition>::const_iterator le = lhs.fNamespaces.end ();
    vector<NamespaceDefinition>::const_iterator ri = rhs.fNamespaces.begin ();
    vector<NamespaceDefinition>::const_iterator re = rhs.fNamespaces.end ();
    for (; li < le or ri < re;) {
        if (li < le and ri < re) {
            if (*li < *ri) {
                return true;
            }
            else if (not(*li == *ri)) {
                // cuz implies >
                return false;
            }
            // keep looking
        }
        else if (li < le) {
            // rhs ran out, so LHS is longer
            return false;
        }
        else if (ri < re) {
            // lhs ran out, so RHS is longer
            return true;
        }
        // keep looking
        Assert (li < le);
        Assert (ri < re);
        li++;
        ri++;
    }
    // both ran out at the same time
    return false;
}

bool XMLDB::operator== (const NamespaceDefinitionsList& lhs, const NamespaceDefinitionsList& rhs)
{
    vector<NamespaceDefinition>::const_iterator li = lhs.fNamespaces.begin ();
    vector<NamespaceDefinition>::const_iterator le = lhs.fNamespaces.end ();
    vector<NamespaceDefinition>::const_iterator ri = rhs.fNamespaces.begin ();
    vector<NamespaceDefinition>::const_iterator re = rhs.fNamespaces.end ();
    for (; li < le or ri < re;) {
        if (li < le and ri < re) {
            if (not(*li == *ri)) {
                return false;
            }
            // keep looking
        }
        else if (li < le) {
            // rhs ran out, so LHS is longer
            return false;
        }
        else if (ri < re) {
            // lhs ran out, so RHS is longer
            return false;
        }
        // keep looking
        Assert (li < le);
        Assert (ri < re);
        li++;
        ri++;
    }
    // both ran out at the same time
    return true;
}

/*
 ********************************************************************************
 *********************************** Schema *************************************
 ********************************************************************************
 */
#if qXMLDBImpl_UseXerces
namespace {
    class MySchemaResolver : public XMLEntityResolver {
    private:
        vector<Schema::SourceComponent> fSourceComponents;

    public:
        MySchemaResolver (const vector<Schema::SourceComponent>& sourceComponents)
            : fSourceComponents (sourceComponents)
        {
        }

    public:
        virtual InputSource* resolveEntity (XMLResourceIdentifier* resourceIdentifier) override
        {
            TraceContextBumper ctx (_T ("XMLDB::{}::MySchemaResolver::resolveEntity"));
            DbgTrace (L"ri->namespace = '%s'; ri-> sysID = '%s', ri->pubID = '%s'", resourceIdentifier->getNameSpace (),
                      resourceIdentifier->getSystemId (), resourceIdentifier->getPublicId ());
            RequireNotNull (resourceIdentifier);
            // Treat namespaces and publicids as higher-priority matchers
            for (vector<Schema::SourceComponent>::const_iterator i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {
                if (cmp_ (resourceIdentifier->getNameSpace (), i->fNamespace) or cmp_ (resourceIdentifier->getPublicId (), i->fPublicID)) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
            }
            // If no match on publicID or namespace, then match more flexibly (ignore path) on systemid
            for (vector<Schema::SourceComponent>::const_iterator i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {
                if (cmp_ (resourceIdentifier->getSystemId (), i->fSystemID)) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
            }
            for (vector<Schema::SourceComponent>::const_iterator i = fSourceComponents.begin (); i != fSourceComponents.end (); ++i) {
                const wchar_t* itemSysID = resourceIdentifier->getSystemId ();
                if (itemSysID != nullptr) {
                    const wchar_t* i = ::wcsrchr (itemSysID, '/');
                    if (i != nullptr) {
                        itemSysID = i + 1;
                    }
                }
                if (itemSysID != nullptr) {
                    const wchar_t* i = ::wcsrchr (itemSysID, '\\');
                    if (i != nullptr) {
                        itemSysID = i + 1;
                    }
                }
                if (cmp_ (itemSysID, i->fSystemID)) {
                    return mkMemInputSrc_ (i->fBLOB);
                }
            }
            Assert (false); // Should never happen - this is only used for entities which are pre-baked in
            return nullptr;
        }

    private:
        static bool cmp_ (const wchar_t* ridName, const wstring& myName)
        {
            if (ridName == nullptr or *ridName == '\0' or myName.empty ()) {
                return false;
            }
            return ridName == myName;
        }

    private:
        static InputSource* mkMemInputSrc_ (const Memory::BLOB& schemaData)
        {
            // copy RAM to C++ array - freed by MemBufInputSource - adopt flag....
            XMLByte* useBuf = new XMLByte[schemaData.GetSize ()];
            memcpy (useBuf, schemaData.begin (), schemaData.GetSize ());
            return new MemBufInputSource (useBuf, schemaData.GetSize (), "", true);
        }
    };
}
#endif
struct Schema::SchemaRep {
    SchemaRep ()
        : fTargetNamespace ()
        , fSourceComponents ()
        , fNamespaceDefinitions ()
#if qXMLDBImpl_UseXerces
        , fCachedGrammarPool (nullptr)
#elif qXMLDBImpl_UseMSXML4
        , fCachedDOM ()
#endif
        , fCompiledRepCriticalSection ()
    {
    }
    SchemaRep (const SchemaRep& from)
        : fTargetNamespace (from.fTargetNamespace)
        , fSourceComponents (from.fSourceComponents)
        , fNamespaceDefinitions (from.fNamespaceDefinitions)
#if qXMLDBImpl_UseXerces
        , fCachedGrammarPool (nullptr)
#elif qXMLDBImpl_UseMSXML4
        , fCachedDOM ()
#endif
        , fCompiledRepCriticalSection ()
    {
    }
    ~SchemaRep ()
    {
#if qXMLDBImpl_UseXerces
        delete fCachedGrammarPool;
#elif qXMLDBImpl_UseMSXML4
        /*
         *  Ingore exceptions destroying fCachedDOM - cuz could easily be because couninitialize called - cuz app is exiting
         */
        IgnoreExceptionsForCall (fCachedDOM.Release ());
        fCachedDOM.p = nullptr;
#endif
    }

    wstring                  fTargetNamespace;
    vector<SourceComponent>  fSourceComponents;
    NamespaceDefinitionsList fNamespaceDefinitions;
#if qXMLDBImpl_UseXerces
    T_CompiledXSDRep fCachedGrammarPool;
#elif qXMLDBImpl_UseMSXML4
    T_CompiledXSDRep fCachedDOM;
#endif
    recursive_mutex fCompiledRepCriticalSection;
};

Schema::AccessCompiledXSD::AccessCompiledXSD (const Schema& schema2Access)
    : fSchema2Access (schema2Access)
    , fEnterCritSection (schema2Access.fRep->fCompiledRepCriticalSection)
{
}

#if qXMLDBImpl_UseXerces || qXMLDBImpl_UseMSXML4
Schema::T_CompiledXSDRep Schema::AccessCompiledXSD::GetCachedTRep () const
{
    return fSchema2Access.GetCachedTRep_ ();
}
#endif

Schema::Schema (const Schema& from)
    : fRep (new SchemaRep (*from.fRep))
{
}

Schema::Schema (const wstring& targetNamespace, const vector<SourceComponent>& sources)
    : fRep (new SchemaRep ())
{
    fRep->fTargetNamespace  = targetNamespace;
    fRep->fSourceComponents = sources;
    if (not targetNamespace.empty ()) {
        fRep->fNamespaceDefinitions.Add (targetNamespace, L"M"); // M: is our default 'main' namespace prefix
    }
}

Schema::~Schema ()
{
}

Schema& Schema::operator= (const Schema& rhs)
{
    if (this != &rhs) {
        fRep = rhs.fRep;
    }
    return *this;
}

bool Schema::HasSchema () const
{
    return fRep->fSourceComponents.size () != 0;
}

vector<Schema::SourceComponent> Schema::GetSourceComponents () const
{
    return fRep->fSourceComponents;
}

void Schema::SetSourceComponents (const vector<Schema::SourceComponent>& sources)
{
    shared_ptr<SchemaRep>       tmp = fRep; // so critsection doesn't hold onto bad ptr
    lock_guard<recursive_mutex> enterCriticalSection (tmp->fCompiledRepCriticalSection);
    fRep                    = shared_ptr<SchemaRep> (new SchemaRep (*fRep)); // effectively clears any cache
    fRep->fSourceComponents = sources;
}

void Schema::SetSchemaData (const Memory::BLOB& source)
{
    if (source.empty ()) {
        SetSourceComponents (vector<Schema::SourceComponent> ());
    }
    else {
        SourceComponent sc;
        sc.fNamespace = GetTargetNamespace ();
        sc.fBLOB      = source;
        SetSourceComponents (vector<SourceComponent> ({sc}));
    }
}

String Schema::GetTargetNamespace () const
{
    return fRep->fTargetNamespace;
}

void Schema::SetTargetNamespace (const String& targetNamespace)
{
    shared_ptr<SchemaRep>       tmp = fRep; // so critsection doesn't hold onto bad ptr
    lock_guard<recursive_mutex> enterCriticalSection (tmp->fCompiledRepCriticalSection);
    fRep                   = shared_ptr<SchemaRep> (new SchemaRep (*fRep));
    fRep->fTargetNamespace = targetNamespace;
}

NamespaceDefinitionsList Schema::GetNamespaceDefinitions () const
{
    return fRep->fNamespaceDefinitions;
}

void Schema::SetNamespaceDefinitions (const NamespaceDefinitionsList& namespaceDefinitions)
{
    shared_ptr<SchemaRep>       tmp = fRep; // so critsection doesn't hold onto bad ptr
    lock_guard<recursive_mutex> enterCriticalSection (tmp->fCompiledRepCriticalSection);
    fRep                        = shared_ptr<SchemaRep> (new SchemaRep (*fRep));
    fRep->fNamespaceDefinitions = namespaceDefinitions;
}

#if qXMLDBImpl_UseXerces || qXMLDBImpl_UseMSXML4
Schema::T_CompiledXSDRep Schema::GetCachedTRep_ () const
{
    Require (HasSchema ());
    lock_guard<recursive_mutex> enterCriticalSection (fRep->fCompiledRepCriticalSection);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        if (fRep->fCachedGrammarPool == nullptr) {
            Memory::BLOB schemaData = GetSchemaData_ ();
            AssertNotNull (XMLPlatformUtils::fgMemoryManager);
            XMLGrammarPoolImpl* grammarPool = new (XMLPlatformUtils::fgMemoryManager) XMLGrammarPoolImpl (XMLPlatformUtils::fgMemoryManager);

            try {
                Require (not schemaData.empty ()); // checked above
                MemBufInputSource mis (schemaData.begin (), schemaData.GetSize (), GetTargetNamespace ().c_str ());

                MySchemaResolver mySchemaResolver (fRep->fSourceComponents);
                // Directly construct SAX2XMLReaderImpl so we can use XMLEntityResolver - which passes along namespace (regular
                // EntityResolve just passes systemID
                //      shared_ptr<SAX2XMLReader>   reader = shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, grammarPool));
                //
                shared_ptr<SAX2XMLReaderImpl> reader = shared_ptr<SAX2XMLReaderImpl> (
                    new (XMLPlatformUtils::fgMemoryManager) SAX2XMLReaderImpl (XMLPlatformUtils::fgMemoryManager, grammarPool));
                reader->setXMLEntityResolver (&mySchemaResolver);

                SetupCommonParserFeatures_ (*reader, true);

                // Reset fgXercesCacheGrammarFromParse to TRUE so we actually load the XSD here
                reader->setFeature (XMLUni::fgXercesCacheGrammarFromParse, true);

                reader->setErrorHandler (&sMyErrorReproter);

                reader->loadGrammar (mis, Grammar::SchemaGrammarType, true);
            }
            catch (...) {
                delete grammarPool;
                Execution::ReThrow ();
            }

            fRep->fCachedGrammarPool = grammarPool;
        }
        return fRep->fCachedGrammarPool;
#elif qXMLDBImpl_UseMSXML4
        if (fRep->fCachedDOM.p == nullptr) {
            T_XMLDOMDocumentSmartPtr schemaDoc;
            MakeXMLDoc_ (schemaDoc);
            ThrowIfErrorHRESULT (schemaDoc->put_async (VARIANT_FALSE));
            ThrowIfErrorHRESULT (schemaDoc->put_validateOnParse (VARIANT_FALSE));

            const vector<Byte>& schemaData   = GetSchemaData ();
            VARIANT_BOOL        isSuccessful = VARIANT_FALSE;
            if (schemaData.empty ()) {
                Execution::Throw (Execution::Platform::Windows::HRESULTErrorException (E_INVALIDARG));
            }
            ThrowIfErrorHRESULT (schemaDoc->loadXML (
                CComBSTR (schemaData.size (), reinterpret_cast<const char*> (Containers::Start (schemaData))), &isSuccessful));

            CComPtr<IXMLDOMSchemaCollection> schemaCollection;
            ThrowIfErrorHRESULT (schemaCollection.CoCreateInstance (__uuidof (XMLSchemaCache40)));
            ThrowIfErrorHRESULT (schemaCollection->add (CComBSTR (GetTargetNamespace ().c_str ()), CComVariant (CComQIPtr<IDispatch> (schemaDoc))));

            fRep->fCachedDOM = CComQIPtr<IDispatch> (schemaCollection);
        }
        return fRep->fCachedDOM;
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}
#endif

Memory::BLOB Schema::GetSchemaData_ () const
{
    wstring targetNS = GetTargetNamespace ();
    for (vector<Schema::SourceComponent>::const_iterator i = fRep->fSourceComponents.begin (); i != fRep->fSourceComponents.end (); ++i) {
        if (targetNS == i->fNamespace) {
            return i->fBLOB;
        }
    }
    Require (false); // must have provided data by now or we cannot construct the actual implementaiton object for the schema
    return Memory::BLOB ();
}

/*
 ********************************************************************************
 ********************************* TempSchemaSetter *****************************
 ********************************************************************************
 */
TempSchemaSetter::TempSchemaSetter (Document& doc, const Schema* s)
    : fDoc (&doc)
    , fOldSchema (doc.GetSchema ())
{
    if (fOldSchema == s) {
        fDoc = nullptr; // don't bother reseting here or our DTOR
    }
    else {
        doc.SetSchema (s);
    }
}

TempSchemaSetter::~TempSchemaSetter ()
{
    if (fDoc != nullptr) {
        try {
            fDoc->SetSchema (fOldSchema);
        }
        catch (...) {
            DbgTrace ("serious error - ~TempSchemaSetter exception caught - ignoring");
        }
    }
}

/*
 ********************************************************************************
 *********************************** Document ***********************************
 ********************************************************************************
 */
class Document::Rep {
public:
    Rep (const Schema* schema)
        : fXMLDoc ()
        , fSchema (nullptr){START_LIB_EXCEPTION_MAPPER{AssertNotNull (sUsingLibInterHelper);
    MakeXMLDoc_ (fXMLDoc);
    fXMLDoc->setUserData (kXerces2XMLDBDocumentKey, this, nullptr);
#if qXMLDBImpl_UseMSXML4
    ThrowIfErrorHRESULT (fXMLDoc->put_async (VARIANT_FALSE));
    ThrowIfErrorHRESULT (fXMLDoc->put_validateOnParse (VARIANT_TRUE));
    ThrowIfErrorHRESULT (fXMLDoc->setProperty (CComBSTR ("SelectionLanguage"), CComVariant ("XPath")));
#endif

    SetSchema (schema);

#if qXMLDBImpl_UseMSXML4
#if qUseSpacePreservingIO
    ThrowIfErrorHRESULT (fXMLDoc->put_preserveWhiteSpace (VARIANT_TRUE));
#endif
#endif
} END_LIB_EXCEPTION_MAPPER
}

Rep (const Rep& from)
    : fXMLDoc ()
    , fSchema (from.fSchema)
{
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        fXMLDoc = T_XMLDOMDocumentSmartPtr (dynamic_cast<T_XMLDOMDocument*> (from.fXMLDoc->cloneNode (true)));
        fXMLDoc->setXmlStandalone (true);
        fXMLDoc->setUserData (kXerces2XMLDBDocumentKey, this, nullptr);
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNode> clone;
        ThrowIfErrorHRESULT (from.fXMLDoc->cloneNode (VARIANT_TRUE, &clone));
        fXMLDoc = clone;
#endif
    }
    END_LIB_EXCEPTION_MAPPER
    EnsureNotNull (fXMLDoc);
}

virtual ~Rep ()
{
}

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

nonvirtual void SetSchema (const Schema* schema)
{
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    fSchema = schema;
    AssertNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseMSXML4
        if (schema == nullptr) {
            ThrowIfErrorHRESULT (fXMLDoc->setProperty (CComBSTR ("SelectionNamespaces"), CComVariant ()));
            ThrowIfErrorHRESULT (fXMLDoc->putref_schemas (CComVariant ()));
            ThrowIfErrorHRESULT (fXMLDoc->put_validateOnParse (VARIANT_FALSE));
        }
        else {
            /*
                 *  Would very much like to be able to set the DEFAULT namespace uses for unadorned XPath expressions (e.g. //PersonName) - but
                 *  haven't figured out how to do this. For now - we must use //M:PersonName.
                 */
            ThrowIfErrorHRESULT (fXMLDoc->setProperty (CComBSTR ("SelectionNamespaces"),
                                                       CComVariant (wstring (L"xmlns:M=\"" + schema->GetTargetNamespace () + L"\"").c_str ())));
            if (schema->GetCachedTRep () != nullptr) {
                ThrowIfErrorHRESULT (fXMLDoc->putref_schemas (CComVariant (schema->GetCachedTRep ())));
                ThrowIfErrorHRESULT (fXMLDoc->put_validateOnParse (VARIANT_TRUE));
            }
        }
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

private:
// For xerces - this is only used to load text as string - for load call. For
// UseMSXML4, the source may or may not be encrypted.
nonvirtual void ReadAsStr_ (Streams::InputStream<Byte>& in, bool encrypted, ProgressStatusCallback* progressCallback,
#if qXMLDBImpl_UseXerces
                            wstring* result
#elif qXMLDBImpl_UseMSXML4
                            CComBSTR* result
#endif
)
{
    RequireNotNull (result);
    Require (in.IsSeekable ());
    streamoff start = in.GetOffset ();
    in.Seek (Streams::Whence::eFromEnd, 0);
    streamoff end = in.GetOffset ();
    Assert (start <= end);
    if (end - start >= numeric_limits<size_t>::max ()) {
        Execution::Throw (StringException (L"stream too large"));
    }
    size_t len = static_cast<size_t> (end - start); // in range checked above
    ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.1f);
    if (encrypted) {
        SmallStackBuffer<Byte> buf (len);
        in.Seek (start);
        size_t xxx = in.Read (buf.begin (), buf.end ());
        ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.3f);
        Assert (xxx <= len);
        const char   kMagicHeaderForOldFmtPHRMODEL[]   = "Fmt#3\0\224\201\152"; // 1.0b7x and earlier format files... 2004-12-22
        const size_t kMagicHeaderForOldFmtPHRMODELSIZE = NEltsOf (kMagicHeaderForOldFmtPHRMODEL) - 1; // don't count extra NUL-term at end
        if (buf.GetSize () > kMagicHeaderForOldFmtPHRMODELSIZE and
            ::memcmp (kMagicHeaderForOldFmtPHRMODEL, buf.begin (), kMagicHeaderForOldFmtPHRMODELSIZE) == 0) {
            // Version 1.0b7x and earlier
            *result = Encryption::Decrypt<wchar_t> (vector<Byte> (buf.begin (), buf.end ())).c_str ();
        }
        else {
            *result = UTF8StringToWide (Encryption::Decrypt<char> (vector<Byte> (buf.begin (), buf.end ())));
        }
    }
    else {
        SmallStackBuffer<wchar_t> wideBuf (0);
        {
            SmallStackBuffer<char> buf (len + 1);
            in.Seek (start);

            /*
                 *  Read in smaller chunks, so we can call SafeSetProgressAndCheckCanceled () during read, and therefore
                 *  have progress dialog appear, and be cancelable.
                 */
            if (end != start) {
                char* pstart = buf;
                char* pend   = pstart + len;
                for (char* p = pstart; p != pend;) {
                    const size_t kChunkSize     = 512 * 1024; // 1/2MB increments seems reasonable...?
                    size_t       bytesRemaining = pend - p;
                    size_t       bytes2Read     = min (bytesRemaining, kChunkSize);
                    size_t       xxx            = in.Read (reinterpret_cast<Byte*> (p), reinterpret_cast<Byte*> (p) + bytes2Read);
                    Assert (xxx <= bytes2Read); // cannot read more than I asked for
                    if (xxx == 0) {
                        // EOF - not sure why - but this can happen before we've read as much as we were supposed to.
                        // So just adjust 'len' & pend accoringly...
                        // Maybe cuz of reading in TEXTMODE streams - mapping CRLF, etc...?
                        Assert (len > static_cast<size_t> (p - pstart));
                        len  = p - pstart;
                        pend = pstart + len;
                        break;
                    }
                    p += xxx;
                    ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.1f + (0.3f - 0.1f) * bytesRemaining / len);
                }
                (*pend) = '\0'; // nul-terminate string (we allocate 1 extra byte in SmallStackBuffer<> above
            }

            ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.3f);

            size_t outCharCount = len + 1;
            wideBuf.GrowToSize (outCharCount + 1);
            MapSBUnicodeTextWithMaybeBOMToUNICODE (buf, len, wideBuf, &outCharCount);
            Assert (outCharCount <= len);
            wideBuf[outCharCount] = '\0'; // if input was NUL-terminated, this is one past where needed, but OK cuz we allocated enough
            ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.4f);
        }

#if qXMLDBImpl_UseXerces
        *result = wideBuf;
#elif qXMLDBImpl_UseMSXML4
        result->Attach (CComBSTR (wideBuf).Detach ());
#endif
    }
    ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.5f);
}

public:
//
// If this function is passed a nullptr exceptionResult - it will throw on bad validation.
// If it is passed a non-nullptr exceptionResult - then it will map BadFormatException to being ignored, but filling in this
// parameter with the exception details. This is used to allow 'advisory' read xsd validation failure, without actually fully
// failing the read (for http://bugzilla/show_bug.cgi?id=513).
//
nonvirtual void Read (Streams::InputStream<Byte>& in, bool encrypted, shared_ptr<BadFormatException>* exceptionResult, ProgressStatusCallback* progressCallback)
{
    TraceContextBumper ctx (_T ("XMLDB::Document::Rep::Read"));
    AssertNotNull (fXMLDoc);

    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        MyMaybeSchemaDOMParser myDOMParser (fSchema);
#endif
#if qXMLDBImpl_UseXerces
        if (!encrypted) {
            try {
                myDOMParser.fParser->parse (BinaryInputStream_InputSource_WithProgress (in, ProgressSubTask (progressCallback, 0.1f, 0.8f), L"XMLDB"));
            }
            catch (const BadFormatException& vf) {
                // Support  http://bugzilla/show_bug.cgi?id=513  and allowing partially valid inputs (like bad ccrs)
                if (exceptionResult == nullptr) {
                    ReThrow ();
                    throw;
                }
                else {
                    DbgTrace ("Validation failure passed by through Read () function argument");
                    *exceptionResult = shared_ptr<BadFormatException> (new BadFormatException (vf));
                    // and ignore - fall through to completed parse.
                }
            }
            goto CompletedParse;
        }
#endif

        {
#if qXMLDBImpl_UseXerces
            wstring xmlText;
#elif qXMLDBImpl_UseMSXML4
            CComBSTR     xmlText;
#endif
            ReadAsStr_ (in, encrypted, progressCallback, &xmlText);

#if qXMLDBImpl_UseXerces
            MemBufInputSource memBufIS (reinterpret_cast<const XMLByte*> (xmlText.c_str ()), xmlText.length () * sizeof (XMLCh), L"XMLDB");
            memBufIS.setEncoding (XMLUni::fgUTF16LEncodingString2);
            myDOMParser.fParser->parse (memBufIS);
#elif qXMLDBImpl_UseMSXML4
            VARIANT_BOOL isSuccessful = 0;
            ThrowIfErrorHRESULT (fXMLDoc->loadXML (xmlText, &isSuccessful));
            ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 0.9f);
            if (not isSuccessful) {
                CComPtr<IXMLDOMParseError> dpe;
                ThrowIfErrorHRESULT (fXMLDoc->get_parseError (&dpe));
                TranslateDOMParseErrorToExceptionIfAny (dpe);
            }
            if (fSchema != nullptr) {
                Validate ();
            }
#endif
        }

#if qXMLDBImpl_UseXerces
    CompletedParse:
        fXMLDoc.reset ();
        fXMLDoc = T_XMLDOMDocumentSmartPtr (myDOMParser.fParser->adoptDocument ());
        fXMLDoc->setXmlStandalone (true);
        fXMLDoc->setUserData (kXerces2XMLDBDocumentKey, this, nullptr);
#endif
    }
    END_LIB_EXCEPTION_MAPPER
    ProgressStatusCallback::SafeSetProgressAndCheckCanceled (progressCallback, 1.0f);
}

public:
nonvirtual void SetRootElement (const Node& newRoot)
{
    TraceContextBumper          ctx (_T ("XMLDB::Document::Rep::SetRootElement"));
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
#if qXMLDBImpl_UseXerces
    // revist this for Xerces - better way???
#endif
    AssertNotNull (fXMLDoc);
    Node replacementRoot = CreateDocumentElement (newRoot.GetName ());
    // next copy all children
    bool addedChildElts = false;
    for (SubNodeIterator i = newRoot.GetChildren (true); i.NotDone (); ++i) {
        Node c = *i;
        switch (c.GetNodeType ()) {
            case Node::eElementNT: {
                addedChildElts = true;
                replacementRoot.AppendNode (c);
            } break;
            case Node::eAttributeNT: {
                replacementRoot.SetAttribute (c.GetName (), c.GetValue ());
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
nonvirtual Node CreateDocumentElement (const wstring& name)
{
    TraceContextBumper ctx (_T ("XMLDB::Document::Rep::CreateDocumentElement"));
    Require (ValidNewNodeName_ (name));
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    AssertNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        Node        result;
        DOMElement* n       = fSchema == nullptr ? fXMLDoc->createElement (name.c_str ())
                                                 : fXMLDoc->createElementNS (fSchema->GetTargetNamespace ().c_str (), name.c_str ());
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
#elif qXMLDBImpl_UseMSXML4
        wstring              namespaceURI = fSchema == nullptr ? L"" : fSchema->GetTargetNamespace ();
        CComPtr<IXMLDOMNode> docEltNode;
        ThrowIfErrorHRESULT (fXMLDoc->createNode (CComVariant (NODE_ELEMENT), CComBSTR (name.c_str ()), CComBSTR (namespaceURI.c_str ()), &docEltNode));
        CComQIPtr<IXMLDOMElement> docElt = docEltNode;

        ThrowIfErrorHRESULT (fXMLDoc->putref_documentElement (docElt));
        ThrowIfNull (docElt);
        Node result = WrapImpl_ (docElt);
        if (not namespaceURI.empty ()) {
            result.SetAttribute (L"xmlns", namespaceURI);
        }
#endif
        return result;
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual void LoadXML (const wstring& xml)
{
    TraceContextBumper          ctx (_T ("XMLDB::Document::Rep::LoadXML"));
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    AssertNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        MyMaybeSchemaDOMParser myDOMParser (fSchema);
        MemBufInputSource      memBufIS (reinterpret_cast<const XMLByte*> (xml.c_str ()), xml.length () * sizeof (XMLCh), L"XMLDB");
        memBufIS.setEncoding (XMLUni::fgUTF16LEncodingString2);
        myDOMParser.fParser->parse (memBufIS);
        fXMLDoc.reset ();
        fXMLDoc = T_XMLDOMDocumentSmartPtr (myDOMParser.fParser->adoptDocument ());
        fXMLDoc->setXmlStandalone (true);
        fXMLDoc->setUserData (kXerces2XMLDBDocumentKey, this, nullptr);
#elif qXMLDBImpl_UseMSXML4
        VARIANT_BOOL isSuccessful = 0;
        ThrowIfErrorHRESULT (fXMLDoc->loadXML (CComBSTR (xml.c_str ()), &isSuccessful));
        if (not isSuccessful) {
            CComPtr<IXMLDOMParseError> dpe;
            ThrowIfErrorHRESULT (fXMLDoc->get_parseError (&dpe));
            TranslateDOMParseErrorToExceptionIfAny (dpe);
        }
        if (fSchema != nullptr) {
            Validate ();
        }
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual void WriteEncrypted (ostream& out) const
{
    TraceContextBumper          ctx (_T ("XMLDB::Document::Rep::WriteEncrypted"));
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    AssertNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        string utf8NodeText = DoWrite2UTF8String (fXMLDoc.get (), false);
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMElement> docElt;
        ThrowIfErrorHRESULT (fXMLDoc->get_documentElement (&docElt));
        ThrowIfNull (docElt);
        string                  utf8NodeText = WriteNodeToString (CComQIPtr<IXMLDOMNode> (docElt));
#endif
        vector<Byte> encData = Encryption::Encrypt (utf8NodeText);
        out.write (reinterpret_cast<char*> (Containers::Start (encData)), encData.size ());
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual void WritePrettyPrinted (ostream& out, LineTerminationMode ltm) const
{
    TraceContextBumper          ctx (_T ("XMLDB::Document::Rep::WritePrettyPrinted"));
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    AssertNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        DoWrite2Stream_ (fXMLDoc.get (), out, true, ltm);
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMElement> docElt;
        ThrowIfErrorHRESULT (fXMLDoc->get_documentElement (&docElt));
        ThrowIfNull (docElt);
        string utf8NodeText = WriteNodeToString (CComQIPtr<IXMLDOMNode> (docElt));
        out.write (utf8NodeText.c_str (), utf8NodeText.length ());
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual void WriteAsIs (ostream& out) const
{
    TraceContextBumper          ctx (_T ("XMLDB::Document::Rep::WriteAsIs"));
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    AssertNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        DoWrite2Stream_ (fXMLDoc.get (), out, false, eQuickest_LTM);
#elif qXMLDBImpl_UseMSXML4
        using Stroika::Foundation::Memory::OS::Win32::StackBasedHandleLocker;
        CComPtr<IStream> pIStream;
        HGLOBAL          hGlobal = nullptr;
        ThrowIfErrorHRESULT (::CreateStreamOnHGlobal (hGlobal, true, &pIStream));
        ThrowIfErrorHRESULT (fXMLDoc->save (CComVariant (pIStream)));
        STATSTG statstg;
        (void)::memset (&statstg, 0, sizeof (statstg));
        ThrowIfErrorHRESULT (pIStream->Stat (&statstg, STATFLAG_NONAME));
        size_t len = statstg.cbSize.LowPart;
        ThrowIfErrorHRESULT (::GetHGlobalFromStream (pIStream, &hGlobal));
        out.write (reinterpret_cast<char*> (StackBasedHandleLocker (hGlobal).GetPointer ()), len);
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual Value GetValue (const wstring& xPathQuery) const
{
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    AssertNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXQillaForXPath
        DOMNode* docElt = fXMLDoc->getDocumentElement ();
        ThrowIfNull (docElt);
        DOMNode* docEltParent = docElt->getParentNode ();
        ThrowIfNull (docEltParent);
        T_DOMNode* n = DoXPathSingleNodeQuery_ (GetNamespaceDefinitions (), docEltParent, xPathQuery);
        if (n != nullptr) {
            return Value (GetTextForDOMNode_ (n));
        }
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNode> n;
        ThrowIfErrorHRESULT (fXMLDoc->selectSingleNode (CComBSTR (xPathQuery.c_str ()), &n));
        if (n.p != nullptr) {
            CComBSTR t;
            ThrowIfErrorHRESULT (n->get_text (&t));
            return Value (BSTR2wstring (t));
        }
#endif
    }
    END_LIB_EXCEPTION_MAPPER
    return Value ();
}

public:
nonvirtual SubNodeIterator GetChildren (bool /*includeAttributes*/) const
{
    //TraceContextBumper ctx (_T ("XMLDB::Document::Rep::GetChildren"));
    // really not enough - must pass critsection to iterator rep!
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    AssertNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        return SubNodeIterator (shared_ptr<SubNodeIterator::Rep> (new SubNodeIteratorOver_SiblingList_Rep (fXMLDoc.get ())));
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNodeList> childNodes;
        ThrowIfErrorHRESULT (fXMLDoc->get_childNodes (&childNodes));
        return SubNodeIterator (shared_ptr<SubNodeIterator::Rep> (new SubNodeIteratorOver_DOMNodeList_Rep (childNodes, nullptr)));
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual SubNodeIterator Lookup (const wstring& xPathQuery) const
{
    return Lookup (xPathQuery, GetNamespaceDefinitions ());
}

nonvirtual SubNodeIterator Lookup (const wstring& xPathQuery, const NamespaceDefinitionsList& ns) const
{
    //TraceContextBumper ctx (_T ("XMLDB::Document::Rep::Lookup"));
    // really not enough - must pass critsection to iterator rep!
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXQillaForXPath
        DOMNode* docElt = fXMLDoc->getDocumentElement ();
        ThrowIfNull (docElt);
        // KLUDGE - DONT UNDERSTAND WHY I NEED TO PASS PARENT NODE FOR SERACH - BUT OTHERWISE - RELATIVE TO TOP NODE SEARCHES FAIL???
        // --LGP 2007-06-09
        DOMNode* docEltParent = docElt->getParentNode ();
        ThrowIfNull (docEltParent);
        vector<T_DOMNode*> v = DoXPathMultiNodeQuery_ (ns, docEltParent, xPathQuery);
        return SubNodeIterator (shared_ptr<SubNodeIterator::Rep> (new SubNodeIteratorOver_vectorDOMNODE_Rep_ (v)));
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNodeList> dnl;
        ThrowIfErrorHRESULT (fXMLDoc->selectNodes (CComBSTR (xPathQuery.c_str ()), &dnl));
        return SubNodeIterator (shared_ptr<SubNodeIterator::Rep> (new SubNodeIteratorOver_DOMNodeList_Rep (dnl, nullptr)));
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual Node LookupOne (const wstring& xPathQuery) const
{
    return LookupOne (xPathQuery, GetNamespaceDefinitions ());
}

nonvirtual Node LookupOne (const wstring& xPathQuery, const NamespaceDefinitionsList& ns) const
{
    //TraceContextBumper ctx (_T ("XMLDB::Document::Rep::LookupOne"));
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);
    RequireNotNull (fXMLDoc);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXQillaForXPath
        DOMNode* docElt = fXMLDoc->getDocumentElement ();
        ThrowIfNull (docElt);
        DOMNode* docEltParent = docElt->getParentNode ();
        ThrowIfNull (docEltParent);
        return WrapImpl_ (DoXPathSingleNodeQuery_ (ns, docEltParent, xPathQuery));
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNode> n;
        ThrowIfErrorHRESULT (fXMLDoc->selectSingleNode (CComBSTR (xPathQuery.c_str ()), &n));
        return WrapImpl_ (n);
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual void Validate () const
{
    RequireNotNull (fSchema);
    TraceContextBumper          ctx (_T ("XMLDB::Document::Rep::Validate"));
    lock_guard<recursive_mutex> enterCriticalSection (fCriticalSection);

    START_LIB_EXCEPTION_MAPPER
    {
        if (not fSchema->HasSchema ()) {
            return;
        }
        try {
            DbgTrace (L"Validating against target namespace '%s'", fSchema->GetTargetNamespace ().c_str ());
#if qXMLDBImpl_UseXerces
            // As this CAN be expensive - especially if we need to externalize the file, and re-parse it!!! - just shortcut by
            // checking the top-level DOM-node and assure that has the right namespace. At least quickie first check that works when
            // reading files (doesnt help in pre-save check, of course)
            T_DOMNode* docNode = fXMLDoc->getDocumentElement ();
            if (docNode == nullptr) {
                Execution::Throw (BadFormatException (L"No document", 0, 0, 0));
            }
            wstring docURI = docNode->getNamespaceURI () == nullptr ? wstring () : docNode->getNamespaceURI ();
            if (docURI != fSchema->GetTargetNamespace ()) {
                Execution::Throw (BadFormatException (Format (L"Wrong document namespace (found '%s' and expected '%s')", docURI.c_str (),
                                                              fSchema->GetTargetNamespace ().c_str ()),
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

                MemBufInputSource readReadSrc (destination.getRawBuffer (), destination.getLen (), L"tmp");
                readReadSrc.setEncoding (XMLUni::fgUTF8EncodingString);

                {
                    Schema::AccessCompiledXSD accessSchema (*fSchema); // REALLY need READLOCK - cuz this just prevents UPDATE of Scehma (never happens anyhow) -- LGP 2009-05-19
                    shared_ptr<SAX2XMLReader> parser = shared_ptr<SAX2XMLReader> (
                        XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema.GetCachedTRep ()));
                    SetupCommonParserFeatures_ (*parser, true);
                    parser->setErrorHandler (&sMyErrorReproter);
                    parser->parse (readReadSrc);
                }
            }
#elif qXMLDBImpl_UseMSXML4
            CComPtr<IXMLDOMParseError> parseError;
            ThrowIfErrorHRESULT (fXMLDoc->validate (&parseError));
            TranslateDOMParseErrorToExceptionIfAny (parseError);
#endif
        }
        catch (...) {
#if qDumpXMLOnValidationError
            {
                // Generate temp file (each with differnet names), and write out the bad XML.
                // Then - re-validate (with line#s) - and print the results of the validation to ANOTHER
                // temporary file
                //
                SDKString tmpFileName = FileSystem::AppTempFileManager::Get ().GetTempFile (SDKSTR ("FAILED_VALIDATION_.xml")).AsSDKString ();
                DbgTrace (_T ("Error validating - so writing out temporary file = '%s'"), tmpFileName.c_str ());
                {
                    ofstream out (tmpFileName.c_str (), ios_base::out | ios_base::binary);
                    WritePrettyPrinted (out, eQuickest_LTM);
                }
                try {
                    ValidateExternalFile (tmpFileName.c_str (), fSchema);
                }
                catch (BadFormatException& vf) {
                    size_t    idx        = tmpFileName.find (_T (".xml"));
                    SDKString newTmpFile = tmpFileName.substr (0, idx) + SDKSTR ("_MSG.txt");
                    ofstream  msgOut (newTmpFile.c_str ());
                    msgOut << "Reason:" << WideStringToNarrowSDKString (vf.GetDetails ().As<wstring> ()) << endl;
                    Memory::Optional<unsigned int> lineNum;
                    Memory::Optional<unsigned int> colNumber;
                    Memory::Optional<uint64_t>     fileOffset;
                    vf.GetPositionInfo (&lineNum, &colNumber, &fileOffset);
                    if (lineNum.IsPresent ()) {
                        msgOut << "Line:" << *lineNum << endl;
                    }
                    if (colNumber.IsPresent ()) {
                        msgOut << "Col: " << *colNumber << endl;
                    }
                    if (fileOffset.IsPresent ()) {
                        msgOut << "FilePos: " << *fileOffset << endl;
                    }
                }
                catch (...) {
                }
            }
#endif
            Execution::ReThrow ();
        }
    }
    END_LIB_EXCEPTION_MAPPER
}

public:
nonvirtual void Validate (const Schema* schema) const
{
    TraceContextBumper          ctx (_T ("XMLDB::Document::Rep::Validate/1"));
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

#if qXMLDB_SupportXSLT
private:
friend class XSLTransformer;
#endif
}
;

Document::Document (const Schema* schema)
    : fRep (new Rep (schema))
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

void Document::WriteEncrypted (ostream& out) const
{
    /*
     * Like WriteAsIs - but encrypted.
     */
    fRep->WriteEncrypted (out);
}

void Document::WritePrettyPrinted (ostream& out, LineTerminationMode ltm) const
{
    /*
     * Write pretty printed XML - where we generate the whitespace around nodes - ignoring any text fragments - except in leaf nodes.
     */
    fRep->WritePrettyPrinted (out, ltm);
}

void Document::WriteAsIs (ostream& out) const
{
    /*
     * Write - respecting all the little #text fragment nodes throughout the XML node tree
     */
    fRep->WriteAsIs (out);
}

SubNodeIterator Document::GetChildren (bool includeAttributes) const
{
    return fRep->GetChildren (includeAttributes);
}

Node Document::GetRootElement () const
{
    // Should only be one in an XML document.
    for (SubNodeIterator i = GetChildren (false); i.NotDone (); ++i) {
        if ((*i).GetNodeType () == Node::eElementNT) {
            return *i;
        }
    }
    return Node ();
}

Value Document::GetValue (const wstring& xPathQuery) const
{
    return fRep->GetValue (xPathQuery);
}

SubNodeIterator Document::Lookup (const wstring& xPathQuery) const
{
    return fRep->Lookup (xPathQuery);
}

SubNodeIterator Document::Lookup (const wstring& xPathQuery, const NamespaceDefinitionsList& ns) const
{
    return fRep->Lookup (xPathQuery, ns);
}

Node Document::LookupOne (const wstring& xPathQuery) const
{
    return fRep->LookupOne (xPathQuery);
}

Node Document::LookupOne (const wstring& xPathQuery, const NamespaceDefinitionsList& ns) const
{
    return fRep->LookupOne (xPathQuery, ns);
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
    : Document (new Rep (*from.fRep))
{
}

RWDocument::RWDocument (const RWDocument& from)
    : Document (new Rep (*from.fRep))
{
}

const RWDocument& RWDocument::operator= (const Document& rhs)
{
    fRep = shared_ptr<Rep> (new Rep (*rhs.fRep));
    return *this;
}

Node RWDocument::CreateDocumentElement (const wstring& name)
{
    return fRep->CreateDocumentElement (name);
}

void RWDocument::SetRootElement (const Node& newRoot)
{
    return fRep->SetRootElement (newRoot);
}

void RWDocument::Read (Streams::InputStream<Byte>& in, bool encrypted, ProgressStatusCallback* progressCallback)
{
    fRep->Read (in, encrypted, nullptr, progressCallback);
}

void RWDocument::ReadAllowingInvalidSrc (Streams::InputStream<Byte>& in, bool encrypted, shared_ptr<BadFormatException>* exceptionResult,
                                         ProgressStatusCallback* progressCallback)
{
    RequireNotNull (exceptionResult);
    exceptionResult->reset ();
    fRep->Read (in, encrypted, exceptionResult, progressCallback); // since we assert exceptionResult - that means that this will NOT throw BadFormatException
}

void RWDocument::LoadXML (const wstring& xml)
{
    fRep->LoadXML (xml);
}

#if qXMLDB_SupportXSLT
/*
 ********************************************************************************
 ********************************** XSLTransformer ******************************
 ********************************************************************************
 */
#if qXMLDBImpl_UseXalanForXSLT
namespace {
    struct XalanXSLTIniter {
        XalanXSLTIniter ()
        {
            XalanTransformer::initialize ();
        }
        ~XalanXSLTIniter ()
        {
            XalanTransformer::terminate ();
        }
    };
    // sadly cannot be constructed in UsingModuleHelper, since order of obj global object loading
    // is inconsistent. But it appears it CAN be destroyed there. Just assure created before using
    // Xalan, and destory in UsingModuleHelper::DTOR ()
    //      -- LGP 2007-06-27
    static XalanXSLTIniter* sXalanIniter;
    static unsigned int     sXalanIniterCounter = 0;
}
#endif
struct XSLTransformer::Rep {
    Rep (const Document& transformDoc)
        :
#if qXMLDBImpl_UseXalanForXSLT
        fCompiledStylesheet (nullptr)
#else
        fTransformDoc (transformDoc)
#endif
    {
        START_LIB_EXCEPTION_MAPPER
        {
#if qXMLDBImpl_UseXalanForXSLT
            if (sXalanIniter == nullptr) {
                Assert (sXalanIniterCounter == 0);
                sXalanIniter = new XalanXSLTIniter ();
            }
            sXalanIniterCounter++;
#endif
#if qXMLDBImpl_UseXalanForXSLT
            T_XMLDOMDocument* llStylesheetDoc = transformDoc.GetRep ()->fXMLDoc.get ();

#if qXalan_XSLT_CompileVIAFileBWA
            SDKString tmpF = AppTempFileManager::Get ().GetTempFile (_T ("ss.xsl"));
            DoWrite2File (llStylesheetDoc, tmpF.c_str (), false);
#endif

#if 0
            //ifstream tmpBuf ("ss.xsl");
            stringstream tmpBuf;
            tmpBuf.write ((const char*)destination.getRawBuffer (), destination.getLen ());
#if 1
            //XSLTInputSource tmpInpSrc ("ss.xsl");
#else
            XSLTInputSource tmpInpSrc (L"inputSrcSysID");
            tmpInpSrc.setStream (&tmpBuf);
#endif
#endif

#if qXalan_XSLT_CompileVIAFileBWA
            XSLTInputSource tmpInpSrc (tmpF.c_str ());
#else
            XercesParserLiaison parserLiason;
            XSLTInputSource     tmpInpSrc (parserLiason.createDocument (llStylesheetDoc));
#endif
            if (fXalanTransformer.compileStylesheet (tmpInpSrc, fCompiledStylesheet) != 0) {
                Execution::Throw (L"BAD COMPILATION"); // fix exception
            }
#if qXalan_XSLT_CompileVIAFileBWA
            // SHOULD be UNNECEESARY BUT HARMLESS. In fact, it makes the BWA stop working!!!
            // AMAZING!!!
            //::DeleteFile (tmpF.c_str ());
#endif
            AssertNotNull (fCompiledStylesheet);
#endif
        }
        END_LIB_EXCEPTION_MAPPER
    }

    virtual ~Rep ()
    {
#if qXMLDBImpl_UseXalanForXSLT
        Assert (sXalanIniterCounter != 0);
        sXalanIniterCounter--;
        if (sXalanIniterCounter == 0) {
            delete sXalanIniter;
            sXalanIniter = nullptr;
        }
#endif
    }

#if qXMLDBImpl_UseXalanForXSLT
    XalanTransformer               fXalanTransformer;
    const XalanCompiledStylesheet* fCompiledStylesheet;
#else
    RWDocument fTransformDoc;
#endif

    friend struct XSLTransformer::Rep;
};

XSLTransformer::XSLTransformer (const Document& transformDoc)
    : fRep (new Rep (transformDoc))
{
}

XSLTransformer::~XSLTransformer ()
{
    // explicitly define here so that when codegen for fRep destructor call auto-generated,
    // we can 'see' the definition of XSLTransformer::Rep and get right virtual
    // function called -- LGP 2007-06-26
}

wstring XSLTransformer::Transform (const Document& srcDoc)
{
    START_LIB_EXCEPTION_MAPPER
    {
        T_XMLDOMDocumentSmartPtr llSrcDoc = srcDoc.GetRep ()->fXMLDoc;

#if qXMLDBImpl_UseXalanForXSLT
        XercesParserLiaison parserLiason;

        // SEE EMAIL 2007-06-22 in my inbox saying todo this - from xalan mailing list...
        XercesDOMSupport             domSupport;
        XercesDOMWrapperParsedSource xmlIn (llSrcDoc, parserLiason, domSupport);

        stringstream     resultBuf (ios_base::in | ios_base::out | ios_base::binary);
        XSLTResultTarget result (resultBuf, fRep->fXalanTransformer.getMemoryManager ());
        if (fRep->fXalanTransformer.transform (xmlIn, fRep->fCompiledStylesheet, result) != 0) {
            Execution::Throw (L"TRANSFORM FAILED"); // fix exception
        }
        // wrong - must fix this to detect codepage of output buf, and then transform accordingly!!!
        return UTF8StringToWide (resultBuf.str ());
#elif qXMLDBImpl_UseMSXML4
        T_XMLDOMDocumentSmartPtr llStylesheetDoc = fRep->fTransformDoc.fRep->fXMLDoc;
        CComBSTR                 tmpR;
        ThrowIfErrorHRESULT (llSrcDoc->transformNode (llStylesheetDoc, &tmpR));
        return BSTR2wstring (tmpR);
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}
#endif

/*
 ********************************************************************************
 ****************************** ValidateExternalFile ****************************
 ********************************************************************************
 */
void XMLDB::ValidateExternalFile (const filesystem::path& externalFileName, const Schema* schema)
{
    RequireNotNull (schema);
    START_LIB_EXCEPTION_MAPPER
    {
        if (not schema->HasSchema ()) {
            return;
        }
        Schema::AccessCompiledXSD accessSchema (*schema); // REALLY need READLOCK - cuz this just prevents UPDATE of Scehma (never happens anyhow) -- LGP 2009-05-19
        shared_ptr<SAX2XMLReader> parser =
            shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema.GetCachedTRep ()));
        SetupCommonParserFeatures_ (*parser, true);
        parser->setErrorHandler (&sMyErrorReproter);
        parser->parse (externalFileName);
    }
    END_LIB_EXCEPTION_MAPPER
}

const bool kThrowInWSNodesForFancyFormatting = false; // mostly useful for debugging - but always leave on for now

#if qXMLDBImpl_UseMSXML4
namespace {
    unsigned int CountDepth_ (CComPtr<IXMLDOMNode> n)
    {
        CComPtr<IXMLDOMNode> p;
        if (n->get_parentNode (&p) == S_OK) {
            return 1 + CountDepth_ (p);
        }
        else {
            return 0;
        }
    }
}
#endif

#if qXMLDBImpl_UseMSXML4
namespace {
    CComPtr<IXMLDOMNode> DeepCloneNode_ (CComPtr<IXMLDOMNode> n, const CComPtr<IXMLDOMDocument>& doc, const CComBSTR& useNamespace,
                                         const wchar_t* replacementName = nullptr)
    {
        CComPtr<IXMLDOMNode> clone;

        {
            DOMNodeType dnt = NODE_INVALID;
            ThrowIfErrorHRESULT (n->get_nodeType (&dnt));

            CComBSTR name;
            if (replacementName == nullptr) {
                ThrowIfErrorHRESULT (n->get_nodeName (&name));
            }
            else {
                name = CComBSTR (replacementName);
            }

            ThrowIfErrorHRESULT (doc->createNode (CComVariant (dnt), name, useNamespace, &clone));
        }

        {
            CComVariant value;
            ThrowIfErrorHRESULT (n->get_nodeValue (&value));
            if (value.vt != VT_EMPTY and value.vt != VT_NULL) {
                ThrowIfErrorHRESULT (clone->put_nodeValue (value));
            }
        }

        {
            CComPtr<IXMLDOMNodeList> children;
            ThrowIfErrorHRESULT (n->get_childNodes (&children));
            CComPtr<IXMLDOMNode> curChild;
            for (; SUCCEEDED (children->nextNode (&curChild)) and curChild.p != nullptr; curChild.Release ()) {
                CComPtr<IXMLDOMNode> clonedChild = DeepCloneNode_ (curChild, doc, useNamespace);
                CComPtr<IXMLDOMNode> ignored;
                ThrowIfErrorHRESULT (clone->appendChild (clonedChild, &ignored));
                Assert (clonedChild.p == ignored.p);
            }
        }

        {
            CComPtr<IXMLDOMNamedNodeMap> attributes;
            ThrowIfErrorHRESULT (n->get_attributes (&attributes));
            if (attributes.p != nullptr) {

                CComPtr<IXMLDOMNamedNodeMap> clonedAttributes;
                ThrowIfErrorHRESULT (clone->get_attributes (&clonedAttributes));

                CComPtr<IXMLDOMNode> curChild;
                for (; SUCCEEDED (attributes->nextNode (&curChild)) and curChild.p != nullptr; curChild.Release ()) {

                    CComBSTR attrName;
                    ThrowIfErrorHRESULT (curChild->get_nodeName (&attrName));
                    CComVariant attrValue;
                    ThrowIfErrorHRESULT (curChild->get_nodeValue (&attrValue));

                    CComPtr<IXMLDOMAttribute> newAttr;
                    ThrowIfErrorHRESULT (doc->createAttribute (attrName, &newAttr));
                    ThrowIfErrorHRESULT (newAttr->put_value (attrValue));

                    CComPtr<IXMLDOMNode> ignored;
                    ThrowIfErrorHRESULT (clonedAttributes->setNamedItem (newAttr, &ignored));
                    //Assert (newAttr.p == ignored.p);
                }
            }
        }

        return clone;
    }
}
#endif

#if qXMLDBImpl_UseMSXML4
namespace {
    void MS_BUGPROOF_REMOVENODE (CComPtr<IXMLDOMNode> parentNode, CComPtr<IXMLDOMNode> n)
    {
        // crazy stuff - but sometimes this crashes MSXML! - sign (just an exception raised).
        // For example - import an OHSD file with xsi:type attribute on subelement
        // like:
        //      <URL xsi:type="xsd:string">http://localhost:2641/OpenHealthServicesDelegationSampleServer/WebService.asmx</URL>
        //
        // Try manual recursive deletion in that case. Key seems to be that you need to
        // separately and manually delete the attributes. SIGH!!!!
        //      --   LGP 2006-09-06
        //
        try {
            CComPtr<IXMLDOMNode> ignored;
            ThrowIfErrorHRESULT (parentNode->removeChild (n, &ignored));
        }
        catch (...) {
            CComPtr<IXMLDOMNode> curChild;
            ThrowIfErrorHRESULT (n->get_firstChild (&curChild));
            while (curChild.p != nullptr) {
                CComPtr<IXMLDOMNode> nextNode;
                ThrowIfErrorHRESULT (curChild->get_nextSibling (&nextNode));
                MS_BUGPROOF_REMOVENODE (n, curChild);
                curChild = nextNode;
            }

            CComPtr<IXMLDOMNamedNodeMap> attrs;
            ThrowIfErrorHRESULT (n->get_attributes (&attrs));
            {
                long len = 0;
                for (attrs->get_length (&len); len != 0; attrs->get_length (&len)) {
                    CComPtr<IXMLDOMNode> nextAttr;
                    ThrowIfErrorHRESULT (attrs->get_item (0, &nextAttr));
                    CComBSTR attrName;
                    ThrowIfErrorHRESULT (nextAttr->get_nodeName (&attrName));
                    CComPtr<IXMLDOMNode> ignored;
                    attrs->removeNamedItem (attrName, &ignored);
                }
            }

            // now try again with no children
            CComPtr<IXMLDOMNode> ignored;
            ThrowIfErrorHRESULT (parentNode->removeChild (n, &ignored));
        }
    }
}
#endif

namespace {
    T_DOMNode* GetInternalRep_ (Node::Rep* anr)
    {
        RequireNotNull (anr);
        AssertNotNull (anr->GetInternalTRep ());
        return reinterpret_cast<T_DOMNode*> (anr->GetInternalTRep ());
    }
}
namespace {
    class MyNodeRep : public Node::Rep {
    public:
        MyNodeRep (
#if qXMLDBImpl_UseXerces
            DOMNode* n
#elif qXMLDBImpl_UseMSXML4
            IDispatch* n
#endif
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
#if qXMLDBImpl_UseXerces
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
#elif qXMLDBImpl_UseMSXML4
                DOMNodeType dnt = NODE_INVALID;
                ThrowIfErrorHRESULT (fNode->get_nodeType (&dnt));
                switch (dnt) {
                    case NODE_ELEMENT:
                        return Node::eElementNT;
                    case NODE_ATTRIBUTE:
                        return Node::eAttributeNT;
                    case NODE_TEXT:
                        return Node::eTextNT;
                    case NODE_COMMENT:
                        return Node::eCommentNT;
                    default:
                        return Node::eOtherNT;
                }
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual wstring GetNamespace () const override
        {
            AssertNotNull (fNode);
            Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                AssertNotNull (fNode->getNamespaceURI ());
                return fNode->getNamespaceURI ();
#elif qXMLDBImpl_UseMSXML4
                AssertNotImplemented ();
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual wstring GetName () const override
        {
            AssertNotNull (fNode);
            Require (GetNodeType () == Node::eElementNT or GetNodeType () == Node::eAttributeNT);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                AssertNotNull (fNode->getNodeName ());
                return fNode->getNodeName ();
#elif qXMLDBImpl_UseMSXML4
                CComBSTR name;
                ThrowIfErrorHRESULT (fNode->get_nodeName (&name));
                return Name (BSTR2wstring (name));
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void SetName (const wstring& name) override
        {
            AssertNotNull (fNode);
            Require (ValidNewNodeName_ (name));
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                T_XMLDOMDocument* doc = fNode->getOwnerDocument ();
                AssertNotNull (doc);
                fNode = doc->renameNode (fNode, fNode->getNamespaceURI (), name.c_str ());
                AssertNotNull (fNode);
#elif qXMLDBImpl_UseMSXML4
                // How PATHETIC. According to DOM3 - there is a renameNode () method - but this doesn't appear
                // to be supported by MSFT (this version of msxml anyhow). So - workaround!
                //
#if 1
                CComPtr<IXMLDOMDocument> doc;
                ThrowIfErrorHRESULT (fNode->get_ownerDocument (&doc));
                CComBSTR namespaceURI;
                ThrowIfErrorHRESULT (fNode->get_namespaceURI (&namespaceURI));
                CComPtr<IXMLDOMNode> newDOMNode = DeepCloneNode_ (fNode, doc, namespaceURI, name.c_str ());
                CComPtr<IXMLDOMNode> parentNode;
                ThrowIfErrorHRESULT (fNode->get_parentNode (&parentNode));
                CComPtr<IXMLDOMNode> childx;
                ThrowIfErrorHRESULT (parentNode->replaceChild (newDOMNode, fNode, &childx));
                fNode = childx;
                ThrowIfNull (childx.p);
#else
                ThrowIfErrorHRESULT (fNode->put_nodeName (CComBSTR (name.c_str ())));
#endif
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Value GetValue () const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                return GetTextForDOMNode_ (fNode);
#elif qXMLDBImpl_UseMSXML4
                // REDO using 'dataType' attribute once we have the XML Schema loaded!!!
                CComBSTR t;
                ThrowIfErrorHRESULT (fNode->get_text (&t));
                return Value (BSTR2wstring (t));
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Value GetValue (const wstring& xPathQuery) const override
        {
            Node n = LookupOne (xPathQuery);
            if (n.IsNull ()) {
                return Value ();
            }
            else {
                return n.GetValue ();
            }
        }
        virtual void SetValue (const Value& v) override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                wstring tmpStr = v.FormatXML (); // See http://bugzilla/show_bug.cgi?id=338
                fNode->setTextContent (tmpStr.empty () ? nullptr : tmpStr.c_str ());
#elif qXMLDBImpl_UseMSXML4
                ThrowIfErrorHRESULT (fNode->put_text (CComBSTR (v.FormatXML ().c_str ())));
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void SetAttribute (const wstring& attrName, const wstring& v) override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
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
                element->setAttributeNS (nullptr, attrName.c_str (), v.c_str ());
#elif qXMLDBImpl_UseMSXML4
                CComQIPtr<IXMLDOMElement> element = fNode;
                ThrowIfNull (element);
                ThrowIfErrorHRESULT (element->setAttribute (CComBSTR (attrName.c_str ()), CComVariant (v.c_str ())));
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual bool HasAttribute (const String& attrName, const String* value) const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                if (fNode->getNodeType () == DOMNode::ELEMENT_NODE) {
                    AssertMember (fNode, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                    T_DOMElement*  elt          = reinterpret_cast<T_DOMElement*> (fNode);
                    const wchar_t* attrNameCStr = attrName.c_str ();
                    if (elt->hasAttribute (attrNameCStr)) {
                        if (value != nullptr) {
                            const XMLCh* s = elt->getAttribute (attrNameCStr);
                            AssertNotNull (s);
                            return wcscmp (s, value->c_str ()) == 0;
                        }
                        return true;
                    }
                }
                return false;
#elif qXMLDBImpl_UseMSXML4
                AssertNotImplemented ();
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual String GetAttribute (const String& attrName) const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                if (fNode->getNodeType () == DOMNode::ELEMENT_NODE) {
                    AssertMember (fNode, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                    T_DOMElement* elt = reinterpret_cast<T_DOMElement*> (fNode);
                    const XMLCh*  s   = elt->getAttribute (attrName.c_str ());
                    AssertNotNull (s);
                    return s;
                }
                return String ();
#elif qXMLDBImpl_UseMSXML4
                AssertNotImplemented ();
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node GetFirstAncestorNodeWithAttribute (const String& attrName) const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                const wchar_t* attrNameCStr = attrName.c_str ();
                for (T_DOMNode* p = fNode; p != nullptr; p = p->getParentNode ()) {
                    if (p->getNodeType () == DOMNode::ELEMENT_NODE) {
                        AssertMember (p, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                        const T_DOMElement* elt = reinterpret_cast<const T_DOMElement*> (p);
                        if (elt->hasAttribute (attrNameCStr)) {
                            return WrapImpl_ (p);
                        }
                    }
                }
                return Node ();
#elif qXMLDBImpl_UseMSXML4
                AssertNotImplemented ();
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node InsertChild (const wstring& name, const wstring* ns, Node afterNode) override
        {
            Require (ValidNewNodeName_ (name));
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                T_XMLDOMDocument* doc          = fNode->getOwnerDocument ();
                const XMLCh*      namespaceURI = (ns == nullptr) ? fNode->getNamespaceURI () : ns->c_str ();
                // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                T_DOMNode* child        = doc->createElementNS (namespaceURI, name.c_str ());
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
#elif qXMLDBImpl_UseMSXML4
                CComPtr<IXMLDOMDocument> doc;
                ThrowIfErrorHRESULT (fNode->get_ownerDocument (&doc));
                CComPtr<IXMLDOMNode> child;
                CComBSTR             namespaceURI;
                ThrowIfErrorHRESULT (fNode->get_namespaceURI (&namespaceURI));
                ThrowIfErrorHRESULT (doc->createNode (CComVariant (NODE_ELEMENT), CComBSTR (name.c_str ()), namespaceURI, &child));
                CComPtr<IXMLDOMNode> childx;
                CComPtr<IXMLDOMNode> refChildNode;
                if (afterNode.IsNull ()) {
                    // this means PREPEND.
                    // If there is a first element, then insert before it. If no elements, then append is the same thing.
                    ThrowIfErrorHRESULT (fNode->get_firstChild (&refChildNode));
                }
                else {
                    Node::Rep* anr = GetRep4Node (afterNode);
                    ThrowIfErrorHRESULT (GetInternalRep_ (anr)->get_nextSibling (&refChildNode));
                }
                ThrowIfErrorHRESULT (fNode->insertBefore (child, CComVariant (refChildNode), &childx));
                return WrapImpl_ (childx);
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node AppendChild (const wstring& name) override
        {
            Require (ValidNewNodeName_ (name));
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                T_XMLDOMDocument* doc          = fNode->getOwnerDocument ();
                const XMLCh*      namespaceURI = fNode->getNamespaceURI ();
                // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                T_DOMNode* child  = doc->createElementNS (namespaceURI, name.c_str ());
                T_DOMNode* childx = fNode->appendChild (child);
                ThrowIfNull (childx);
                return WrapImpl_ (childx);
#elif qXMLDBImpl_UseMSXML4
                CComPtr<IXMLDOMDocument> doc;
                ThrowIfErrorHRESULT (fNode->get_ownerDocument (&doc));
                CComPtr<IXMLDOMNode> child;
                if (kThrowInWSNodesForFancyFormatting) {
                    CComPtr<IXMLDOMNode> t;
                    ThrowIfErrorHRESULT (doc->createNode (CComVariant (NODE_TEXT), CComBSTR (), CComBSTR (), &t));
                    {
                        wstring w = L"\n";
                        for (unsigned int i = CountDepth_ (fNode); i > 0; --i) {
                            w += L"\t";
                        }
                        t->put_text (CComBSTR (w.c_str ()));
                    }
                    CComPtr<IXMLDOMNode> ignore;
                    ThrowIfErrorHRESULT (fNode->appendChild (t, &ignore));
                }
                CComBSTR namespaceURI;
                ThrowIfErrorHRESULT (fNode->get_namespaceURI (&namespaceURI));
                ThrowIfErrorHRESULT (doc->createNode (CComVariant (NODE_ELEMENT), CComBSTR (name.c_str ()), namespaceURI, &child));
                CComPtr<IXMLDOMNode> childx;
                ThrowIfErrorHRESULT (fNode->appendChild (child, &childx));
                if (kThrowInWSNodesForFancyFormatting) {
                    CComPtr<IXMLDOMNode> t;
                    ThrowIfErrorHRESULT (doc->createNode (CComVariant (NODE_TEXT), CComBSTR (), CComBSTR (), &t));
                    t->put_text (CComBSTR (L"\n"));
                    CComPtr<IXMLDOMNode> ignore;
                    ThrowIfErrorHRESULT (fNode->appendChild (t, &ignore));
                }
                return WrapImpl_ (childx);
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void AppendChild (const wstring& name, const wstring* ns, const Value& v) override
        {
            Require (ValidNewNodeName_ (name));
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                T_XMLDOMDocument* doc          = fNode->getOwnerDocument ();
                const XMLCh*      namespaceURI = (ns == nullptr) ? fNode->getNamespaceURI () : ns->c_str ();
                // unsure if we should use smartpointer here - thinkout xerces & smart ptrs & mem management
                T_DOMNode* child  = doc->createElementNS (namespaceURI, name.c_str ());
                wstring    tmpStr = v.FormatXML (); // See http://bugzilla/show_bug.cgi?id=338
                child->setTextContent (tmpStr.empty () ? nullptr : tmpStr.c_str ());
                ThrowIfNull (fNode->appendChild (child));
#elif qXMLDBImpl_UseMSXML4
                CComPtr<IXMLDOMDocument> doc;
                ThrowIfErrorHRESULT (fNode->get_ownerDocument (&doc));
                CComPtr<IXMLDOMNode> child;
                if (kThrowInWSNodesForFancyFormatting) {
                    CComPtr<IXMLDOMNode> t;
                    ThrowIfErrorHRESULT (doc->createNode (CComVariant (NODE_TEXT), CComBSTR (), CComBSTR (), &t));
                    {
                        wstring w = L"\n";
                        for (unsigned int i = CountDepth_ (fNode); i > 0; --i) {
                            w += L"\t";
                        }
                        t->put_text (CComBSTR (w.c_str ()));
                    }
                    CComPtr<IXMLDOMNode> ignore;
                    ThrowIfErrorHRESULT (fNode->appendChild (t, &ignore));
                }
                CComBSTR namespaceURI;
                ThrowIfErrorHRESULT (fNode->get_namespaceURI (&namespaceURI));
                ThrowIfErrorHRESULT (doc->createNode (CComVariant (NODE_ELEMENT), CComBSTR (name.c_str ()), namespaceURI, &child));
                ThrowIfErrorHRESULT (child->put_text (CComBSTR (v.FormatXML ().c_str ())));
                CComPtr<IXMLDOMNode> childx;
                ThrowIfErrorHRESULT (fNode->appendChild (child, &childx));
                if (kThrowInWSNodesForFancyFormatting) {
                    CComPtr<IXMLDOMNode> t;
                    ThrowIfErrorHRESULT (doc->createNode (CComVariant (NODE_TEXT), CComBSTR (), CComBSTR (), &t));
                    t->put_text (CComBSTR (L"\n"));
                    CComPtr<IXMLDOMNode> ignore;
                    ThrowIfErrorHRESULT (fNode->appendChild (t, &ignore));
                }
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void AppendChildIfNotEmpty (const wstring& name, const wstring* ns, const Value& v) override
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
#if qXMLDBImpl_UseXerces
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
#elif qXMLDBImpl_UseMSXML4
                // really do a DEEP Clone of 'n' - and depending on 'inherit' flag - either we can use the builtin clone or
                // we must do ourselves (appars the only way to reset the namespace uri??)
                CComPtr<IXMLDOMNode> nodeToClone = GetInternalRep_ (GetRep4Node (n));
                if (inheritNamespaceFromInsertionPoint) {
                    CComPtr<IXMLDOMDocument> doc;
                    ThrowIfErrorHRESULT (fNode->get_ownerDocument (&doc));
                    CComBSTR namespaceURI;
                    ThrowIfErrorHRESULT (fNode->get_namespaceURI (&namespaceURI));
                    CComPtr<IXMLDOMNode> clone = DeepCloneNode_ (nodeToClone, doc, namespaceURI);
                    CComPtr<IXMLDOMNode> clonex;
                    CComPtr<IXMLDOMNode> insertAfterNode;
                    if (not afterNode.IsNull ()) {
                        insertAfterNode = GetInternalRep_ (GetRep4Node (afterNode));
                    }
                    ThrowIfErrorHRESULT (fNode->insertBefore (clone, CComVariant (insertAfterNode), &clonex));
                    return WrapImpl_ (clonex);
                }
                else {
                    CComPtr<IXMLDOMNode> clone;
                    ThrowIfErrorHRESULT (nodeToClone->cloneNode (VARIANT_TRUE, &clone));
                    CComPtr<IXMLDOMNode> clonex;
                    CComPtr<IXMLDOMNode> insertAfterNode;
                    if (not afterNode.IsNull ()) {
                        insertAfterNode = GetInternalRep_ (GetRep4Node (afterNode));
                    }
                    ThrowIfErrorHRESULT (fNode->insertBefore (clone, CComVariant (insertAfterNode), &clonex));
                    return WrapImpl_ (clonex);
                }
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }

        virtual Node AppendNode (const Node& n, bool inheritNamespaceFromInsertionPoint) override
        {
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                T_XMLDOMDocument* doc         = fNode->getOwnerDocument ();
                T_DOMNode*        nodeToClone = GetInternalRep_ (GetRep4Node (n).get ());
                T_DOMNode*        clone       = doc->importNode (nodeToClone, true);
                if (inheritNamespaceFromInsertionPoint and clone->getNodeType () == DOMNode::ELEMENT_NODE) {
                    clone = RecursivelySetNamespace_ (clone, fNode->getNamespaceURI ());
                }
                T_DOMNode* clonex = fNode->appendChild (clone);
                ThrowIfNull (clonex);
                return WrapImpl_ (clonex);
#elif qXMLDBImpl_UseMSXML4
                // really do a DEEP Clone of 'n' - and depending on 'inherit' flag - either we can use the builtin clone or
                // we must do ourselves (appars the only way to reset the namespace uri??)
                CComPtr<IXMLDOMNode> nodeToClone = GetInternalRep_ (GetRep4Node (n));
                if (inheritNamespaceFromInsertionPoint) {
                    CComPtr<IXMLDOMDocument> doc;
                    ThrowIfErrorHRESULT (fNode->get_ownerDocument (&doc));
                    CComBSTR namespaceURI;
                    ThrowIfErrorHRESULT (fNode->get_namespaceURI (&namespaceURI));
                    CComPtr<IXMLDOMNode> clone = DeepCloneNode_ (nodeToClone, doc, namespaceURI);
                    CComPtr<IXMLDOMNode> clonex;
                    ThrowIfErrorHRESULT (fNode->appendChild (clone, &clonex));
                    return WrapImpl_ (clonex);
                }
                else {
                    CComPtr<IXMLDOMNode> clone;
                    ThrowIfErrorHRESULT (nodeToClone->cloneNode (VARIANT_TRUE, &clone));
                    CComPtr<IXMLDOMNode> clonex;
                    ThrowIfErrorHRESULT (fNode->appendChild (clone, &clonex));
                    return WrapImpl_ (clonex);
                }
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual void DeleteNode (const wstring& xPathExp) override
        {
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                T_DOMNode* selNode = nullptr;
                if (xPathExp.empty ()) {
                    selNode = fNode;
                }
                else {
                    T_XMLDOMDocument* doc = fNode->getOwnerDocument ();
                    ThrowIfNull (doc);
                    selNode = DoXPathSingleNodeQuery_ (GetNamespaceDefinitions_ (), fNode, xPathExp);
                }
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
#elif qXMLDBImpl_UseMSXML4
                CComPtr<IXMLDOMNode> selNode;
                if (xPathExp.empty ()) {
                    selNode = fNode;
                }
                else {
                    ThrowIfErrorHRESULT (fNode->selectSingleNode (CComBSTR (xPathExp.c_str ()), &selNode));
                }
                ThrowIfNull (selNode);
                CComPtr<IXMLDOMNode> parentNode;
                ThrowIfErrorHRESULT (selNode->get_parentNode (&parentNode));
                if (parentNode.p == nullptr) {
                    // This happens if the selected node is an attribute
                    CComQIPtr<IXMLDOMElement>   elt  = fNode;
                    CComQIPtr<IXMLDOMAttribute> attr = selNode;
                    if (elt.p != nullptr and attr.p != nullptr) {
                        CComBSTR nodeName;
                        ThrowIfErrorHRESULT (selNode->get_nodeName (&nodeName));
                        ThrowIfErrorHRESULT (elt->removeAttribute (nodeName));
                        return;
                    }
                }
                AssertNotNull (parentNode.p); //  shouldn't happen - except in case handled above...

                try {
                    CComPtr<IXMLDOMNode> ignored;
                    ThrowIfErrorHRESULT (parentNode->removeChild (selNode, &ignored));
                }
                catch (...) {
                    MS_BUGPROOF_REMOVENODE (parentNode, selNode);
                }
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node ReplaceNode (const wstring& xPathExp) override
        {
            RequireNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                T_XMLDOMDocument* doc = fNode->getOwnerDocument ();
                ThrowIfNull (doc);
                T_DOMNode* selNode = DoXPathSingleNodeQuery_ (GetNamespaceDefinitions_ (), fNode, xPathExp);
                ThrowIfNull<RecordNotFoundException> (selNode);
                T_DOMNode* parentNode = selNode->getParentNode ();
                ThrowIfNull (parentNode);
                DOMElement* n = doc->createElementNS (selNode->getNamespaceURI (), selNode->getNodeName ());
                (void)parentNode->replaceChild (n, selNode);
                return WrapImpl_ (n);
#elif qXMLDBImpl_UseMSXML4
                CComPtr<IXMLDOMNode> selNode;
                ThrowIfErrorHRESULT (fNode->selectSingleNode (CComBSTR (xPathExp.c_str ()), &selNode));
                ThrowIfNull (selNode);
                CComPtr<IXMLDOMNode> parentNode;
                ThrowIfErrorHRESULT (selNode->get_parentNode (&parentNode));
                CComBSTR nodeName;
                ThrowIfErrorHRESULT (selNode->get_nodeName (&nodeName));
                CComPtr<IXMLDOMDocument> doc;
                ThrowIfErrorHRESULT (parentNode->get_ownerDocument (&doc));
                CComBSTR namespaceURI;
                ThrowIfErrorHRESULT (parentNode->get_namespaceURI (&namespaceURI));
                CComPtr<IXMLDOMNode> child;
                ThrowIfErrorHRESULT (doc->createNode (CComVariant (NODE_ELEMENT), nodeName, namespaceURI, &child));
                CComPtr<IXMLDOMNode> childx;
                ThrowIfErrorHRESULT (parentNode->replaceChild (child, selNode, &childx));
                return WrapImpl_ (child);
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual Node GetParentNode () const override
        {
            AssertNotNull (fNode);
            START_LIB_EXCEPTION_MAPPER
            {
#if qXMLDBImpl_UseXerces
                return WrapImpl_ (fNode->getParentNode ());
#elif qXMLDBImpl_UseMSXML4
                CComPtr<IXMLDOMNode> parentNode;
                ThrowIfErrorHRESULT (fNode->get_parentNode (&parentNode));
                return WrapImpl_ (parentNode);
#endif
            }
            END_LIB_EXCEPTION_MAPPER
        }
        virtual SubNodeIterator GetChildren (bool includeAttributes) const override;
        virtual SubNodeIterator Lookup (const wstring& xPathQuery) const override;
        virtual Node            LookupOne (const wstring& xPathQuery) const override;
        virtual Node            GetChildNodeByID (const ID& id) const override;
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

    public:
        DECLARE_USE_BLOCK_ALLOCATION (MyNodeRep);

    private:
#if qXMLDBImpl_UseXerces
        // must carefully think out mem managment here - cuz not ref counted - around as long as owning doc...
        DOMNode* fNode;
#elif qXMLDBImpl_UseMSXML4
        CComQIPtr<IXMLDOMNode>   fNode;
#endif
    };
}

namespace {
    inline Node WrapImpl_ (T_DOMNode* n)
    {
        return n == nullptr ? Node () : Node (shared_ptr<Node::Rep> (new MyNodeRep (n)));
    }
}

/*
 ********************************************************************************
 ********************************** MyNodeRep ***********************************
 ********************************************************************************
 */
SubNodeIterator MyNodeRep::GetChildren (bool includeAttributes) const
{
    AssertNotNull (fNode);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        return SubNodeIterator (shared_ptr<SubNodeIterator::Rep> (new SubNodeIteratorOver_SiblingList_Rep (fNode)));
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNodeList> childNodes;
        ThrowIfErrorHRESULT (fNode->get_childNodes (&childNodes));
        CComPtr<IXMLDOMNamedNodeMap> attrs;
        if (includeAttributes) {
            ThrowIfErrorHRESULT (fNode->get_attributes (&attrs));
        }
        return SubNodeIterator (shared_ptr<SubNodeIterator::Rep> (new SubNodeIteratorOver_DOMNodeList_Rep (childNodes, attrs)));
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

SubNodeIterator MyNodeRep::Lookup (const wstring& xPathQuery) const
{
    AssertNotNull (fNode);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXQillaForXPath
        return SubNodeIterator (shared_ptr<SubNodeIterator::Rep> (
            new SubNodeIteratorOver_vectorDOMNODE_Rep_ (DoXPathMultiNodeQuery_ (GetNamespaceDefinitions_ (), fNode, xPathQuery))));
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNodeList> dnl;
        ThrowIfErrorHRESULT (fNode->selectNodes (CComBSTR (xPathQuery.c_str ()), &dnl));
        return SubNodeIterator (shared_ptr<SubNodeIterator::Rep> (new SubNodeIteratorOver_DOMNodeList_Rep (dnl, nullptr)));
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

Node MyNodeRep::LookupOne (const wstring& xPathQuery) const
{
    AssertNotNull (fNode);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXQillaForXPath
        return WrapImpl_ (DoXPathSingleNodeQuery_ (GetNamespaceDefinitions_ (), fNode, xPathQuery));
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNode> n;
        ThrowIfErrorHRESULT (fNode->selectSingleNode (CComBSTR (xPathQuery.c_str ()), &n));
        return WrapImpl_ (n);
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

Node MyNodeRep::GetChildNodeByID (const ID& id) const
{
    AssertNotNull (fNode);
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXQillaForXPath
        const wchar_t* idStr = id.c_str ();
        for (T_DOMNode* i = fNode->getFirstChild (); i != nullptr; i = i->getNextSibling ()) {
            if (i->getNodeType () == DOMNode::ELEMENT_NODE) {
                AssertMember (i, T_DOMElement); // assert and then reinterpret_cast() because else dynamic_cast is 'slowish'
                T_DOMElement* elt = reinterpret_cast<T_DOMElement*> (i);
                const XMLCh*  s   = elt->getAttribute (L"id");
                AssertNotNull (s);
                if (::wcscmp (s, idStr) == 0) {
                    return WrapImpl_ (i);
                }
            }
        }
        return Node ();
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

/*
 ********************************************************************************
 ******************* SubNodeIteratorOver_vectorDOMNODE_Rep_ *********************
 ********************************************************************************
 */
Node SubNodeIteratorOver_vectorDOMNODE_Rep_::Current () const
{
    Require (not IsAtEnd ());
    return WrapImpl_ (fDOMNodes[fCur]);
}

/*
 ********************************************************************************
 ********************** SubNodeIteratorOver_DOMNodeList_Rep *********************
 ********************************************************************************
 */
SubNodeIteratorOver_DOMNodeList_Rep::SubNodeIteratorOver_DOMNodeList_Rep (T_DOMNodeList* dnl
#if qXMLDBImpl_UseXerces
#elif qXMLDBImpl_UseMSXML4
                                                                          ,
                                                                          IXMLDOMNamedNodeMap* attrs
#endif
                                                                          )
    : fMainNodeList (dnl)
    ,
#if qXMLDBImpl_UseXerces
#elif qXMLDBImpl_UseMSXML4
    fAttrsList (attrs)
    ,
#endif
    fMainListLen (0)
    , fAttrsListLen (0)
    , fCur (0)
{
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        fMainListLen = dnl->getLength ();
#elif qXMLDBImpl_UseMSXML4
        if (fMainNodeList.p != nullptr) {
            long len = 0;
            ThrowIfErrorHRESULT (fMainNodeList->get_length (&len));
            fMainListLen = len;
        }
        if (fAttrsList.p != nullptr) {
            long len = 0;
            ThrowIfErrorHRESULT (fAttrsList->get_length (&len));
            fAttrsListLen = len;
        }
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

bool SubNodeIteratorOver_DOMNodeList_Rep::IsAtEnd () const
{
    return fCur == (fAttrsListLen + fMainListLen);
}

void SubNodeIteratorOver_DOMNodeList_Rep::Next ()
{
    RequireNotNull (fMainNodeList);
    Require (not IsAtEnd ());
    Assert (fCur < GetLength ());
    ++fCur;
}

Node SubNodeIteratorOver_DOMNodeList_Rep::Current () const
{
    Require (fCur < GetLength ());
    START_LIB_EXCEPTION_MAPPER
    {
#if qXMLDBImpl_UseXerces
        AssertNotNull (fMainNodeList);
        return WrapImpl_ (fMainNodeList->item (fCur));
#elif qXMLDBImpl_UseMSXML4
        CComPtr<IXMLDOMNode> n;
        if (fCur < fMainListLen) {
            ThrowIfErrorHRESULT (fMainNodeList->get_item (fCur, &n));
        }
        else {
            size_t l = fCur - fMainListLen;
            ThrowIfErrorHRESULT (fAttrsList->get_item (l, &n));
        }
        return WrapImpl_ (n);
#endif
    }
    END_LIB_EXCEPTION_MAPPER
}

size_t SubNodeIteratorOver_DOMNodeList_Rep::GetLength () const
{
    return fAttrsListLen + fMainListLen;
}

/*
 ********************************************************************************
 ********************** SubNodeIteratorOver_SiblingList_Rep *********************
 ********************************************************************************
 */
SubNodeIteratorOver_SiblingList_Rep::SubNodeIteratorOver_SiblingList_Rep (T_DOMNode* nodeParent)
    : fCurNode (nullptr)
    , fParentNode (nodeParent)
    , fCachedMainListLen (static_cast<size_t> (-1))
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

/*
 ********************************************************************************
 ********************************* XMLDB::SAX::Parse ****************************
 ********************************************************************************
 */
namespace {
    class SAX2PrintHandlers : public DefaultHandler {
    private:
        XMLDB::SAX::CallbackInterface& fCallback;

    public:
        SAX2PrintHandlers (XMLDB::SAX::CallbackInterface& callback)
            : fCallback (callback)
        {
        }

    public:
        virtual void startDocument () override
        {
            fCallback.StartDocument ();
        }
        virtual void endDocument () override
        {
            fCallback.EndDocument ();
        }

    public:
        virtual void startElement (const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attributes) override
        {
            Require (uri != nullptr);
            Require (localname != nullptr);
            Require (qname != nullptr);
            map<wstring, Value> attrs;
            for (XMLSize_t i = 0; i < attributes.getLength (); i++) {
                const XMLCh* localAttrName = attributes.getLocalName (i);
                const XMLCh* val           = attributes.getValue (i);
                attrs.insert (map<wstring, Value>::value_type (localAttrName, val));
            }
            fCallback.StartElement (uri, localname, qname, attrs);
        }
        virtual void endElement (const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname) override
        {
            Require (uri != nullptr);
            Require (localname != nullptr);
            Require (qname != nullptr);
            fCallback.EndElement (uri, localname, qname);
        }
        virtual void characters (const XMLCh* const chars, const XMLSize_t length) override
        {
            Require (chars != nullptr);
            Require (length != 0);
            fCallback.CharactersInsideElement (wstring (chars, chars + length));
        }
    };
}

void SAX::Parse (Streams::InputStream<Byte>& in, CallbackInterface& callback, ProgressStatusCallback* progressCallback)
{
    SAX2PrintHandlers         handler (callback);
    shared_ptr<SAX2XMLReader> parser = shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager));
    SetupCommonParserFeatures_ (*parser, false);
    parser->setContentHandler (&handler);
    parser->setErrorHandler (&sMyErrorReproter);
    parser->parse (BinaryInputStream_InputSource_WithProgress (in, ProgressSubTask (progressCallback, 0.1f, 0.9f), L"XMLDB::SAX::Parse"));
}

void SAX::Parse (Streams::InputStream<Byte>& in, const Schema& schema, CallbackInterface& callback, ProgressStatusCallback* progressCallback)
{
    if (schema.HasSchema ()) {
        SAX2PrintHandlers handler (callback);
        Schema::AccessCompiledXSD accessSchema (schema); // REALLY need READLOCK - cuz this just prevents UPDATE of Scehma (never happens anyhow) -- LGP 2009-05-19
        shared_ptr<SAX2XMLReader> parser =
            shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema.GetCachedTRep ()));
        SetupCommonParserFeatures_ (*parser, true);
        parser->setContentHandler (&handler);
        parser->setErrorHandler (&sMyErrorReproter);
        parser->parse (BinaryInputStream_InputSource_WithProgress (in, ProgressSubTask (progressCallback, 0.1f, 0.9f), L"XMLDB::SAX::Parse"));
    }
    else {
        Parse (in, callback, progressCallback);
    }
}

/*
 ********************************************************************************
 ******************** XMLDB::Private::GetNetAllocationCount *********************
 ********************************************************************************
 */
size_t XMLDB::Private::GetNetAllocationCount ()
{
#if qXMLDBTrackAllocs
    const MyXercesMemMgr& myMemMgr = *sUsingLibInterHelper->fXERCES.fUseXercesMemoryManager;
    return myMemMgr.fAllocator.GetNetAllocationCount ();
#endif
    return 0;
}

/*
 ********************************************************************************
 ***************** XMLDB::Private::GetNetAllocatedByteCount *********************
 ********************************************************************************
 */
size_t XMLDB::Private::GetNetAllocatedByteCount ()
{
#if qXMLDBTrackAllocs
    const MyXercesMemMgr& myMemMgr = *sUsingLibInterHelper->fXERCES.fUseXercesMemoryManager;
    return myMemMgr.fAllocator.GetNetAllocatedByteCount ();
#endif
    return 0;
}

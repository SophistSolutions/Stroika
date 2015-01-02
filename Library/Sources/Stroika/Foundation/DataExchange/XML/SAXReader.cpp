/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <atomic>

#include    "../../Debug/Trace.h"
#include    "../../Execution/Common.h"
#include    "../../Execution/ProgressMonitor.h"
#include    "../../Execution/RequiredComponentMissingException.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/MemoryAllocator.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../BadFormatException.h"

#include    "SAXReader.h"



#if     qHasLibrary_Xerces
#ifndef qUseMyXMLDBMemManager
#define qUseMyXMLDBMemManager       qDebug
//#define   qUseMyXMLDBMemManager       1
#endif
//#define   qXMLDBTrackAllocs   0
//#define   qXMLDBTrackAllocs   1
#ifndef qXMLDBTrackAllocs
#define qXMLDBTrackAllocs       qDebug
#endif
#endif


#if     qHasLibrary_Xerces

// Not sure if we want this defined HERE or in the MAKEFILE/PROJECT FILE
#define XML_LIBRARY 1
#define XERCES_STATIC_LIBRARY   1

// avoid namespace conflcit with some Xerces code
#undef Assert

#include    <xercesc/util/PlatformUtils.hpp>
#include    <xercesc/util/XMLString.hpp>
#include    <xercesc/util/BinInputStream.hpp>
#include    <xercesc/dom/DOM.hpp>
#include    <xercesc/framework/MemBufInputSource.hpp>
#include    <xercesc/framework/MemBufFormatTarget.hpp>
#include    <xercesc/util/XMLUni.hpp>
#include    <xercesc/util/OutOfMemoryException.hpp>
#include    <xercesc/sax2/XMLReaderFactory.hpp>
#include    <xercesc/sax2/SAX2XMLReader.hpp>
#include    <xercesc/sax2/DefaultHandler.hpp>
#include    <xercesc/validators/common/Grammar.hpp>
#include    <xercesc/parsers/XercesDOMParser.hpp>
#include    <xercesc/framework/LocalFileFormatTarget.hpp>
#include    <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include    <xercesc/util/XMLEntityResolver.hpp>
#include    <xercesc/sax/InputSource.hpp>
#include    <xercesc/parsers/SAX2XMLReaderImpl.hpp>
#if     qDebug
#define Assert(c)           {if (!(c)) { Stroika::Foundation::Debug::Private::Debug_Trap_ ("Assert", #c, __FILE__, __LINE__, nullptr); }}
#else
#define Assert(c)
#endif
#endif



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Debug;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::XML;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Streams;


#if     qHasLibrary_Xerces
namespace {
    String xercesString2String_ (const XMLCh* t)
    {
        if (sizeof (XMLCh) == sizeof (char16_t)) {
            return String (reinterpret_cast<const char16_t*> (t));
        }
        else if (sizeof (XMLCh) == sizeof (wchar_t)) {
            return String (reinterpret_cast<const wchar_t*> (t));
        }
        else {
            AssertNotReached ();
            return String ();
        }
    }
}
#endif





#if     qHasLibrary_Xerces
XERCES_CPP_NAMESPACE_USE
#endif


namespace {
    //tmphack - move to headers eventually

    class   ValidationFailed {
    public:
        ValidationFailed (const String& reason, unsigned int lineNum, unsigned int linePos, unsigned int filePos);

    public:
        String          fReason;
        unsigned int    fLineNum;
        unsigned int    fLinePos;
        unsigned int    fFilePos;
    };

    /*
     ********************************************************************************
     ****************************** ValidationFailed ********************************
     ********************************************************************************
     */
    ValidationFailed::ValidationFailed (const String& reason, unsigned int lineNum, unsigned int linePos, unsigned int filePos)
        : fReason (reason)
        , fLineNum (lineNum)
        , fLinePos (linePos)
        , fFilePos (filePos)
    {
    }

}






#if     qHasLibrary_Xerces
namespace   {
    /*
     *  A helpful class to isolete Xerces (etc) memory management calls. Could be the basis
     *  of future perfomance/memory optimizations, but for now, just a helpful debugging/tracking
     *  class.
     */
    class   MyXercesMemMgr_ : public MemoryManager {
    public:
        MyXercesMemMgr_ ()
#if     qXMLDBTrackAllocs
            :
            fBaseAllocator (),
            fAllocator (fBaseAllocator),
            fLastSnapshot ()
#endif
        {
        }
        ~MyXercesMemMgr_ ()
        {
        }


#if     qXMLDBTrackAllocs
    public:
        Memory::SimpleAllocator_CallLIBCNewDelete               fBaseAllocator;
        Memory::LeakTrackingGeneralPurposeAllocator             fAllocator;
        recursive_mutex                                         fLastSnapshot_CritSection;
        Memory::LeakTrackingGeneralPurposeAllocator::Snapshot   fLastSnapshot;
#endif

    public:
#if     qXMLDBTrackAllocs
        void    DUMPCurMemStats ()
        {
            TraceContextBumper ctx (SDKSTR ("MyXercesMemMgr_::DUMPCurMemStats"));
            auto    critSec { Execution::make_unique_lock (fLastSnapshot_CritSection) };
            fAllocator.DUMPCurMemStats (fLastSnapshot);
            // now copy current map to prev for next time this gets called
            fLastSnapshot = fAllocator.GetSnapshot ();
        }
#endif

    public:
        virtual     MemoryManager* getExceptionMemoryManager () override
        {
            return this;
        }
        virtual     void*   allocate (XMLSize_t size) override
        {
            try {
#if     qXMLDBTrackAllocs
                return fAllocator.Allocate (size);
#else
                return ::operator new (size);
#endif
            }
            catch (...) {
                Execution::DoThrow (OutOfMemoryException ());   // quirk cuz this is the class Xerces expects and catches internally (why not bad_alloc?) - sigh...
            }
        }
        virtual     void    deallocate (void* p) override
        {
            if (p != nullptr) {
#if     qXMLDBTrackAllocs
                return fAllocator.Deallocate (p);
#else
                ::operator delete (p);
#endif
            }
        }
    };
}
#endif



#if     qHasLibrary_Xerces
namespace   {
    DOMImplementation&  GetDOMIMPL_ ()
    {
        //const XMLCh   kDOMImplFeatureDeclaration[]    =   L"Core";
        constexpr   XMLCh   kDOMImplFeatureDeclaration[]    =   { 'C', 'o', 'r', 'e', '\0'};
        // safe to save in a static var? -- LGP 2007-05-20
        // from perusing implementation - this appears safe to cache and re-use in different threads
        static  DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation (kDOMImplFeatureDeclaration);
        AssertNotNull (impl);
        return *impl;
    }
}
#endif



#if     qHasLibrary_Xerces
class   MyErrorReproter_ : public XMLErrorReporter, public ErrorHandler {
    // XMLErrorReporter
public:
    virtual     void    error
    (
        const   unsigned int        errCode
        , const XMLCh* const        errDomain
        , const ErrTypes            type
        , const XMLCh* const        errorText
        , const XMLCh* const        systemId
        , const XMLCh* const        publicId
        , const XMLFileLoc          lineNum
        , const XMLFileLoc          colNum
    ) override
    {
        Execution::DoThrow (ValidationFailed (xercesString2String_ (errorText), static_cast<unsigned int> (lineNum), static_cast<unsigned int> (colNum), 0));
    }
    virtual  void resetErrors () override
    {
    }

    // ErrorHandler
public:
    virtual  void warning (const SAXParseException& exc) override
    {
        // ignore
    }
    virtual  void error (const SAXParseException& exc) override
    {
        AssertNotImplemented ();
        //Execution::DoThrow (ValidationFailed (exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()), static_cast<unsigned int> (exc.getColumnNumber ()), 0));
    }
    virtual  void fatalError (const SAXParseException& exc) override
    {
        AssertNotImplemented ();
        //Execution::DoThrow (ValidationFailed (exc.getMessage (), static_cast<unsigned int> (exc.getLineNumber ()), static_cast<unsigned int> (exc.getColumnNumber ()), 0));
    }
};
static  MyErrorReproter_ sMyErrorReproter_;
#endif





namespace   {
#if     qHasLibrary_Xerces
    inline  void    SetupCommonParserFeatures_ (SAX2XMLReader& reader)
    {
        reader.setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
        reader.setFeature (XMLUni::fgXercesDynamic, false);
        reader.setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, false);         // false:  * *Do not report attributes used for Namespace declarations, and optionally do not report original prefixed names
    }
    inline  void    SetupCommonParserFeatures_ (SAX2XMLReader& reader, bool validatingWithSchema)
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






/*
 ********************************************************************************
 ************************ Private::UsingModuleHelper ****************************
 ********************************************************************************
 */
#if     qHasLibrary_Xerces
namespace   {
#if     qDebug
    atomic<uint32_t>    sStdIStream_InputStream_COUNT_ (0);
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
    struct  UsingLibInterHelper {
#if     qHasLibrary_Xerces
        struct  UsingLibInterHelper_XERCES {
            MyXercesMemMgr_* fUseXercesMemoryManager;
            UsingLibInterHelper_XERCES ():
                fUseXercesMemoryManager (nullptr)
            {
#if     qUseMyXMLDBMemManager
                fUseXercesMemoryManager = new MyXercesMemMgr_ ();
#endif
                XMLPlatformUtils::Initialize (XMLUni::fgXercescDefaultLocale, 0, 0, fUseXercesMemoryManager);
            }
            ~UsingLibInterHelper_XERCES ()
            {
                TraceContextBumper ctx (SDKSTR ("~UsingLibInterHelper_XERCES"));
                XMLPlatformUtils::Terminate ();
                Assert (sStdIStream_InputStream_COUNT_ == 0);
#if     qUseMyXMLDBMemManager
                delete fUseXercesMemoryManager;
#endif
            }
        };
        UsingLibInterHelper_XERCES  fXERCES;
#endif

        UsingLibInterHelper ();
    };
    UsingLibInterHelper::UsingLibInterHelper ():
#if     qHasLibrary_Xerces
        fXERCES ()
#endif
    {
    }
}


namespace   {
    UsingLibInterHelper*    sUsingLibInterHelper    =   nullptr;
}
#endif







/*
 ********************************************************************************
 **************** DataExcahnge::XML::SAXReader_ModuleInit_ **********************
 ********************************************************************************
 */
SAXReader_ModuleInit_::SAXReader_ModuleInit_ ()
#if     qDefaultTracingOn
    : fDebugTraceDependency (Debug::MakeModuleDependency_Trace ())
#endif
{
#if     qHasLibrary_Xerces
    TraceContextBumper ctx (SDKSTR ("XML::SAXReader_ModuleInit_::SAXReader_ModuleInit_"));
    Require (sUsingLibInterHelper == nullptr);
    sUsingLibInterHelper = new UsingLibInterHelper ();
#endif
}

SAXReader_ModuleInit_::~SAXReader_ModuleInit_ ()
{
#if     qHasLibrary_Xerces
    TraceContextBumper ctx (SDKSTR ("XML::SAXReader_ModuleInit_::~SAXReader_ModuleInit_"));
    RequireNotNull (sUsingLibInterHelper);
    delete sUsingLibInterHelper;
    sUsingLibInterHelper = nullptr;
#endif
}





#if     qHasLibrary_Xerces
namespace   {
    // These SHOULD be part of xerces! Perhaps someday post them?
    class   StdIStream_InputSource : public InputSource {
    protected:
        class StdIStream_InputStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream {
        public :
            StdIStream_InputStream (BinaryInputStream in)
                : fSource (in)
            {
#if     qDebug
                sStdIStream_InputStream_COUNT_++;
#endif
            }
            ~StdIStream_InputStream ()
            {
#if     qDebug
                sStdIStream_InputStream_COUNT_--;
#endif
            }
        public:
            virtual  XMLFilePos curPos () const override
            {
                return fSource.GetOffset ();
            }
            virtual     XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
            {
                return fSource.Read (toFill, toFill + maxToRead);
            }
            virtual     const XMLCh* getContentType () const override
            {
                return nullptr;
            }
        protected:
            BinaryInputStream    fSource;
        };

    public :
        StdIStream_InputSource (BinaryInputStream in, const XMLCh* const bufId = nullptr)
            : InputSource (bufId)
            , fSource (in)
        {
        }
        virtual     BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) StdIStream_InputStream (fSource);
        }
    protected:
        BinaryInputStream   fSource;
    };


    // my variations of StdIInputSrc with progresstracker callback
    class   StdIStream_InputSourceWithProgress : public StdIStream_InputSource {
    protected:
        class ISWithProg : public StdIStream_InputSource::StdIStream_InputStream {
        public :
            ISWithProg (const BinaryInputStream& in, ProgressMonitor::Updater progressCallback)
                : StdIStream_InputStream (in)
                , fProgress (progressCallback, 0.0f, 1.0f)
                , fTotalSize (0.0f)
            {
                // @todo - redo for if non-seekable streams - just set flag saying not seeakble and do right thing with progress. ....
                /// for now we raise exceptions
                SeekOffsetType   start   =   in.GetOffset ();
                in.Seek (Whence::eFromEnd, 0);
                SeekOffsetType totalSize =   in.GetOffset ();
                Assert (start <= totalSize);
                in.Seek (start);
                fTotalSize = static_cast<float> (totalSize);
            }
        public :
            virtual     XMLSize_t readBytes (XMLByte* const toFill, const XMLSize_t maxToRead) override
            {
                using   ProgressRangeType = ProgressMonitor::ProgressRangeType;
                ProgressRangeType   curOffset           =   0.0;
                bool                doProgressBefore    =   (maxToRead > 10 * 1024); // only bother calling both before & after if large read
                if (fTotalSize > 0.0f and doProgressBefore) {
                    //curOffset = fSource ? static_cast<float> (fSource.tellg ()) :  fTotalSize;
                    curOffset = static_cast<ProgressRangeType> (fSource.GetOffset ());
                    fProgress.SetCurrentProgressAndThrowIfCanceled (curOffset / fTotalSize);
                }

                //fSource.read (reinterpret_cast<char*> (toFill), maxToRead);
                //XMLSize_t   result  =   static_cast<XMLSize_t> (fSource.gcount ()); // safe cast cuz read maxToRead bytes
                XMLSize_t   result  =   fSource.Read (toFill, toFill + maxToRead);
                if (fTotalSize > 0) {
                    curOffset = static_cast<ProgressRangeType> (fSource.GetOffset ());
                    //curOffset = fSource ? static_cast<float> (fSource.tellg ()) :  fTotalSize;
                    fProgress.SetCurrentProgressAndThrowIfCanceled (curOffset / fTotalSize);
                }
                return result;
            }
        private:
            ProgressMonitor::Updater    fProgress;
            float                       fTotalSize;
        };

    public :
        StdIStream_InputSourceWithProgress (BinaryInputStream in, ProgressMonitor::Updater progressCallback, const XMLCh* const bufId = nullptr):
            StdIStream_InputSource (in, bufId),
            fProgressCallback (progressCallback)
        {
        }
        virtual     BinInputStream* makeStream () const override
        {
            return new (getMemoryManager ()) ISWithProg (fSource, fProgressCallback);
        }
    private:
        ProgressMonitor::Updater fProgressCallback;
    };

}
#endif




/*
 ********************************************************************************
 ******************************* SAXCallbackInterface ***************************
 ********************************************************************************
 */
void    SAXCallbackInterface::StartDocument ()
{
}

void    SAXCallbackInterface::EndDocument ()
{
}

void    SAXCallbackInterface::StartElement (const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs)
{
}

void    SAXCallbackInterface::EndElement (const String& uri, const String& localName, const String& qname)
{
}

void    SAXCallbackInterface::CharactersInsideElement (const String& text)
{
}







/*
 ********************************************************************************
 ************************************* SAXParse *********************************
 ********************************************************************************
 */

#if     qHasLibrary_Xerces
namespace   {
    class SAX2PrintHandlers_
        : public DefaultHandler {
    private:
        SAXCallbackInterface&   fCallback;

    public:
        SAX2PrintHandlers_ (SAXCallbackInterface& callback)
            : fCallback (callback)
        {
        }

    public:
        virtual     void startDocument () override
        {
            fCallback.StartDocument ();
        }
        virtual     void    endDocument () override
        {
            fCallback.EndDocument ();
        }

    public:
        virtual     void startElement (const XMLCh* const uri, const XMLCh* const localName, const XMLCh* const qname, const Attributes& attributes) override
        {
            Require (uri != nullptr);
            Require (localName != nullptr);
            Require (qname != nullptr);
            Mapping<String, VariantValue>    attrs;
            for (XMLSize_t i = 0; i < attributes.getLength(); i++) {
                const XMLCh* localAttrName = attributes.getLocalName (i);
                const XMLCh* val = attributes.getValue (i);
                attrs.Add (xercesString2String_ (localAttrName), xercesString2String_ (val).As<wstring> ());
            }
            fCallback.StartElement (xercesString2String_ (uri), xercesString2String_ (localName), xercesString2String_ (qname), attrs);
        }
        virtual     void    endElement (const XMLCh* const uri, const XMLCh* const localName, const XMLCh* const qname) override
        {
            Require (uri != nullptr);
            Require (localName != nullptr);
            Require (qname != nullptr);
            fCallback.EndElement (xercesString2String_ (uri), xercesString2String_ (localName), xercesString2String_ (qname));
        }
        virtual     void    characters (const XMLCh* const chars, const XMLSize_t length) override
        {
            Require (chars != nullptr);
            Require (length != 0);
            fCallback.CharactersInsideElement (wstring (chars, chars + length));
        }
    };
}
#endif

void    XML::SAXParse (const Streams::BinaryInputStream& in, SAXCallbackInterface& callback, Execution::ProgressMonitor::Updater progres)
{
#if     qHasLibrary_Xerces
    SAX2PrintHandlers_  handler (callback);
    shared_ptr<SAX2XMLReader>    parser  =   shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager));
    SetupCommonParserFeatures_ (*parser, false);
    parser->setContentHandler (&handler);
    parser->setErrorHandler (&sMyErrorReproter_);
    //const XMLCh kBufID[] = L"SAX::Parse";
    const XMLCh kBufID[] = {'S', 'A', 'X', ':', 'P', 'a', 'r', 's', 'e' , '\0' };
    parser->parse (StdIStream_InputSourceWithProgress (in, ProgressMonitor::Updater (progres, 0.1f, 0.9f), kBufID));
#else
    Execution::DoThrow (Execution::RequiredComponentMissingException (Execution::RequiredComponentMissingException::kSAXFactory));
#endif
}

#if 0
//SCHEMA NOT YET SUPPORTED
void    XML::SAXParse (istream& in, const Schema& schema, SAXCallbackInterface& callback, Execution::ProgressMontior* progres)
{
    if (schema.HasSchema ()) {
        SAX2PrintHandlers   handler (callback);
        Schema::AccessCompiledXSD   accessSchema (schema);// REALLY need READLOCK - cuz this just prevents UPDATE of Scehma (never happens anyhow) -- LGP 2009-05-19
        shared_ptr<SAX2XMLReader>    parser  =   shared_ptr<SAX2XMLReader> (XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager, accessSchema.GetCachedTRep ()));
        SetupCommonParserFeatures_ (*parser, true);
        parser->setContentHandler (&handler);
        parser->setErrorHandler (&sMyErrorReproter_);
        parser->parse (StdIStream_InputSourceWithProgress (in, ProgressSubTask (progressCallback, 0.1f, 0.9f), L"XMLDB::SAX::Parse"));
    }
    else {
        Parse (in, callback, progressCallback);
    }
}
#endif


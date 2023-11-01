/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Debug/Trace.h"
#include "../../Memory/Common.h"
#include "../../Memory/MemoryAllocator.h"

#include "Common.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qHasFeature_Xerces && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#if qDebug
#pragma comment(lib, "xerces-c_3d.lib")
#else
#pragma comment(lib, "xerces-c_3.lib")
#endif
#endif

#if qHasFeature_Xerces
#ifndef qUseMyXMLDBMemManager_
#define qUseMyXMLDBMemManager_ qDebug
//#define   qUseMyXMLDBMemManager_       1
#endif
//#define   qXMLDBTrackAllocs_   0
//#define   qXMLDBTrackAllocs_   1
#ifndef qXMLDBTrackAllocs_
#define qXMLDBTrackAllocs_ qDebug
#endif
#endif

#if qHasFeature_Xerces

// Not sure if we want this defined HERE or in the MAKEFILE/PROJECT FILE
#define XML_LIBRARY 1
#define XERCES_STATIC_LIBRARY 1

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
    {                                                                                                                                      \
        if (!(c)) {                                                                                                                        \
            Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Assert", #c, __FILE__, __LINE__, nullptr);                  \
        }                                                                                                                                  \
    }
#else
#define Assert(c)
#endif

XERCES_CPP_NAMESPACE_USE

namespace {
    /*
     *  A helpful class to isolete Xerces (etc) memory management calls. Could be the basis
     *  of future perfomance/memory optimizations, but for now, just a helpful debugging/tracking
     *  class.
     */
    class MyXercesMemMgr_ : public MemoryManager {
    public:
        MyXercesMemMgr_ ()
#if qXMLDBTrackAllocs_
            : fAllocator{fBaseAllocator}
#endif
        {
        }

#if qXMLDBTrackAllocs_
    public:
        Memory::SimpleAllocator_CallLIBCNewDelete             fBaseAllocator;
        Memory::LeakTrackingGeneralPurposeAllocator           fAllocator;
        recursive_mutex                                       fLastSnapshot_CritSection;
        Memory::LeakTrackingGeneralPurposeAllocator::Snapshot fLastSnapshot;
#endif

    public:
#if qXMLDBTrackAllocs_
        void DUMPCurMemStats ()
        {
            TraceContextBumper      ctx{"MyXercesMemMgr_::DUMPCurMemStats"};
            [[maybe_unused]] auto&& critSec = lock_guard{fLastSnapshot_CritSection};
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
#if qXMLDBTrackAllocs_
                return fAllocator.Allocate (size);
#else
                return ::operator new (size);
#endif
            }
            catch (...) {
                // NB: use throw not Exception::Throw () since that requires its a subclass of exception (or SilentException)
                throw (OutOfMemoryException ()); // quirk cuz this is the class Xerces expects and catches internally (why not bad_alloc?) - sigh...
            }
        }
        virtual void deallocate (void* p) override
        {
            if (p != nullptr) {
#if qXMLDBTrackAllocs_
                return fAllocator.Deallocate (p);
#else
                ::operator delete (p);
#endif
            }
        }
    };
}

/*
 ********************************************************************************
 **************************** DependencyLibraryInitializer **********************
 ********************************************************************************
 */
struct DependencyLibraryInitializer::LibXerces {
    MyXercesMemMgr_* fUseXercesMemoryManager;
    LibXerces ()
        : fUseXercesMemoryManager{nullptr}
    {
#if qUseMyXMLDBMemManager_
        fUseXercesMemoryManager = new MyXercesMemMgr_ ();
#endif
        XMLPlatformUtils::Initialize (XMLUni::fgXercescDefaultLocale, 0, 0, fUseXercesMemoryManager);
    }
    ~LibXerces ()
    {
        TraceContextBumper ctx{"~LibXerces"};
        XMLPlatformUtils::Terminate ();
#if qUseMyXMLDBMemManager_
        delete fUseXercesMemoryManager;
#endif
    }
};

DependencyLibraryInitializer::DependencyLibraryInitializer ()
    : fXERCES{make_shared<LibXerces> ()}
{
}
#endif

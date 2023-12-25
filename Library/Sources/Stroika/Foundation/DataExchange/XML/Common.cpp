/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Debug/Trace.h"
#include "../../Memory/Common.h"
#include "../../Memory/MemoryAllocator.h"

#include "Common.h"

//using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qHasFeature_Xerces
#include "Providers/Xerces.h"
using namespace Stroika::Foundation::DataExchange::XML::Providers::Xerces;
#endif

#if qHasFeature_libxml2
#include "Providers/LibXML2.h"
using namespace Stroika::Foundation::DataExchange::XML::Providers::LibXML2;
#endif

CompileTimeFlagChecker_SOURCE (Stroika::Foundation::DataExchange::XML, qHasFeature_Xerces, qHasFeature_Xerces);
CompileTimeFlagChecker_SOURCE (Stroika::Foundation::DataExchange::XML, qHasFeature_libxml2, qHasFeature_libxml2);

#if qHasFeature_Xerces
namespace {
    /*
     *  A helpful class to isolete Xerces (etc) memory management calls. Could be the basis
     *  of future perfomance/memory optimizations, but for now, just a helpful debugging/tracking
     *  class.
     */
    class MyXercesMemMgr_ : public MemoryManager {
    public:
        MyXercesMemMgr_ ()
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
            : fAllocator{fBaseAllocator}
#endif
        {
        }
        ~MyXercesMemMgr_ ()
        {
            Assert (fAllocator.GetSnapshot ().fAllocations.empty ()); // else we have a memory leak
        }

#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
    public:
        Memory::SimpleAllocator_CallLIBCNewDelete             fBaseAllocator;
        Memory::LeakTrackingGeneralPurposeAllocator           fAllocator;
        mutex                                                 fLastSnapshot_CritSection;
        Memory::LeakTrackingGeneralPurposeAllocator::Snapshot fLastSnapshot;
#endif

    public:
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
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
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
                return fAllocator.Allocate (size);
#else
                return ::operator new (size);
#endif
            }
            catch (...) {
                // NB: use throw not Exception::Throw () since that requires its a subclass of exception (or SilentException)
                throw (OutOfMemoryException{}); // quirk cuz this is the class Xerces expects and catches internally (why not bad_alloc?) - sigh...
            }
        }
        virtual void deallocate (void* p) override
        {
            if (p != nullptr) {
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
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
 ******************** DependencyLibraryInitializer::LibXerces *******************
 ********************************************************************************
 */
struct DependencyLibraryInitializer::LibXerces {
    MyXercesMemMgr_* fUseXercesMemoryManager;
    LibXerces ()
        : fUseXercesMemoryManager{nullptr}
    {
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
        fUseXercesMemoryManager = new MyXercesMemMgr_{};
#endif
        XMLPlatformUtils::Initialize (XMLUni::fgXercescDefaultLocale, 0, 0, fUseXercesMemoryManager);
    }
    ~LibXerces ()
    {
        TraceContextBumper ctx{"~LibXerces"};
        XMLPlatformUtils::Terminate ();
#if qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
        delete fUseXercesMemoryManager; // checks for leaks
#endif
    }
};
#endif

#if qHasFeature_libxml2
/*
 ********************************************************************************
 ******************* DependencyLibraryInitializer::LibXML2 **********************
 ********************************************************************************
 */
struct DependencyLibraryInitializer::LibXML2 {
    MyXercesMemMgr_* fUseXercesMemoryManager;
    LibXML2 ()
    {
        LIBXML_TEST_VERSION;
    }
    ~LibXML2 ()
    {
        TraceContextBumper ctx{"~LibXML2"};
        xmlCleanupParser ();
#if qDebug
        xmlMemoryDump ();
#endif
    }
};
#endif

#if qHasFeature_Xerces or qHasFeature_libxml2
void DependencyLibraryInitializer::UsingProvider_ (Provider p)
{
    // check outside of lock cuz once set, not unset, and then lock if needed
    switch (p) {
#if qHasFeature_Xerces
        case Provider::eXerces: {
            [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
            if (not fXERCES) {
                fXERCES = make_shared<LibXerces> ();
            }
        } break;
#endif
#if qHasFeature_libxml2
        case Provider::eLibXml2: {
            [[maybe_unused]] auto&& lock = lock_guard{fMutex_};
            if (not fLibXML2) {
                fLibXML2 = make_shared<LibXML2> ();
            }
        } break;
#endif
    }
}
#endif
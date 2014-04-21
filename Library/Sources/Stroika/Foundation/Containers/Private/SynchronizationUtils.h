/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_SynchronizationUtils_h_
#define _Stroika_Foundation_Containers_Private_SynchronizationUtils_h_

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"

#include    "../Common.h"



/**
 *  Private utilities to support building subtypes of Containers::Iterable<T>
 *
 * TODO:
 *      @todo   Consider somehow getting rid of this (or moving it) so thats a TRAIT object
 *              usable in backends. That way - it should  be easier todo different locking strategies
 *              within a concrete object type.
 *
 * Notes:
 *
 */



#define qContainersPrivateSyncrhonizationPolicy_StdMutex_                   1

//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3341.pdf
#define qContainersPrivateSyncrhonizationPolicy_StdCTMs_N3341_              2

#define qContainersPrivateSyncrhonizationPolicy_WinCriticalSectionMutex_    3


#ifndef qContainersPrivateSyncrhonizationPolicy_
#if     qPlatform_Windows
#define qContainersPrivateSyncrhonizationPolicy_    qContainersPrivateSyncrhonizationPolicy_WinCriticalSectionMutex_
#else
#define qContainersPrivateSyncrhonizationPolicy_    qContainersPrivateSyncrhonizationPolicy_StdMutex_
#endif
#endif


#if     qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdMutex_
#include    <mutex>
#elif   qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_WinCriticalSectionMutex_
#include    "../../Execution/Platform/Windows/CriticalSectionMutex.h"
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {


                struct  ContainerRepLockDataSupport_ {
#if     qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdMutex_
                    // NB: using std::mutex instead of std::recursive_mutex for performance reasons and because our use is
                    // simple enough we can probbaly get away with it, but condier a (templated) option to use recursive mutexs
                    // if its helpful for some containers
                    mutable std::mutex  fMutex_;
#elif   qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_WinCriticalSectionMutex_
                    // NB: using std::mutex instead of std::recursive_mutex for performance reasons and because our use is
                    // simple enough we can probbaly get away with it, but condier a (templated) option to use recursive mutexs
                    // if its helpful for some containers
                    mutable Execution::Platform::Windows::CriticalSectionMutex  fMutex_;
#endif
                    ContainerRepLockDataSupport_ () = default;
                    ContainerRepLockDataSupport_ (const ContainerRepLockDataSupport_&) = delete;
                    const ContainerRepLockDataSupport_& operator= (const ContainerRepLockDataSupport_&) = delete;
                };



#if     qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdMutex_
#define CONTAINER_LOCK_HELPER_START(CRLDS)\
    {\
        std::lock_guard<std::mutex> lg (CRLDS.fMutex_);\
        {
#define CONTAINER_LOCK_HELPER_END()\
}\
}
#elif     qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_WinCriticalSectionMutex_
#define CONTAINER_LOCK_HELPER_START(CRLDS)\
    {\
        std::lock_guard<Execution::Platform::Windows::CriticalSectionMutex> lg (CRLDS.fMutex_);\
        {
#define CONTAINER_LOCK_HELPER_END()\
}\
}
#elif   qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdCTMs_N3341_
#define CONTAINER_LOCK_HELPER_START(CRLDS)\
    {\
        synchronized_\
        {
#define CONTAINER_LOCK_HELPER_END()\
}\
}
#endif



            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SynchronizationUtils.inl"

#endif  /*_Stroika_Foundation_Containers_Private_SynchronizationUtils_h_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_SynchronizationUtils_h_
#define _Stroika_Foundation_Containers_Private_SynchronizationUtils_h_

#include    "../../StroikaPreComp.h"

#include    <mutex>

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
                    /*
                     *  We used std::mutex instead of std::recursive_mutex for performance reasons until v2.0a102.
                     *
                     *  However, this was a mistake. It CAN break black box modularity, because you can assign a container to
                     *  another and lose track of what 'rep' is being used, and end up with a deadlock (or at least easily
                     *  uneasily viewable blocking).
                     *
                     *  With very limited empirical testing, on WinDoze / VS2k13, it made no obvious performance differnce
                     *  anyhow.
                     */
                    mutable std::recursive_mutex  fMutex_;
#elif   qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_WinCriticalSectionMutex_
                    mutable Execution::Platform::Windows::CriticalSectionMutex  fMutex_;
#endif
                    ContainerRepLockDataSupport_ () = default;
                    ContainerRepLockDataSupport_ (const ContainerRepLockDataSupport_&) = delete;
                    const ContainerRepLockDataSupport_& operator= (const ContainerRepLockDataSupport_&) = delete;
                };



#if     qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdMutex_
#define CONTAINER_LOCK_HELPER_START(CRLDS)\
    {\
        std::lock_guard<std::recursive_mutex> lg (CRLDS.fMutex_);\
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

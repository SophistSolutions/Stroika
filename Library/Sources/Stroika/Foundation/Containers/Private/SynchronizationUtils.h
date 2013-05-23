/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
 *
 * Notes:
 *
 */



#define qContainersPrivateSyncrhonizationPolicy_StdMutex_   1

//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3341.pdf
#define qContainersPrivateSyncrhonizationPolicy_StdCTMs_N3341_  1


#ifndef qContainersPrivateSyncrhonizationPolicy_
#define qContainersPrivateSyncrhonizationPolicy_    qContainersPrivateSyncrhonizationPolicy_StdMutex_
#endif


#if     qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdMutex_
#include    <mutex>
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
#endif
#if     qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
                    ContainerRepLockDataSupport_ () = default;
#else
                    ContainerRepLockDataSupport_ () {}
#endif
                    NO_COPY_CONSTRUCTOR(ContainerRepLockDataSupport_);
                    NO_ASSIGNMENT_OPERATOR(ContainerRepLockDataSupport_);
                };



#if     qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdMutex_
#define CONTAINER_LOCK_HELPER_(CRLDS,CODEBLOCK)\
    {\
        std::lock_guard<std::mutex> lg (CRLDS.fMutex_);\
        {\
            CODEBLOCK;\
        }\
    }
#elif   qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdCTMs_N3341_
#define CONTAINER_LOCK_HELPER_(CRLDS,CODEBLOCK)\
    {\
        synchronized_\
        {\
            CODEBLOCK;\
        }\
    }
#endif


#if     qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdMutex_
#define CONTAINER_LOCK_HELPER_START(CRLDS)\
    {\
        std::lock_guard<std::mutex> lg (CRLDS.fMutex_);\
        {
#define CONTAINER_LOCK_HELPER_END(CRLDS)\
}\
}
#elif   qContainersPrivateSyncrhonizationPolicy_ == qContainersPrivateSyncrhonizationPolicy_StdCTMs_N3341_
#define CONTAINER_LOCK_HELPER_START(CRLDS)\
    {\
        synchronized_\
        {
#define CONTAINER_LOCK_HELPER_END(CRLDS)\
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

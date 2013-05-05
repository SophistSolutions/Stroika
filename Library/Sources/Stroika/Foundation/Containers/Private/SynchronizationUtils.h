/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_SynchronizationUtils_h_
#define _Stroika_Foundation_Containers_Private_SynchronizationUtils_h_

/**
 *  Private utilities to support building subtypes of Containers::Iterable<T>
 *
 * TODO:
 *
 * Notes:
 *
 */


#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"

#include    "../Common.h"


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
                    std::mutex  fMutex_;
#endif
                    ContainerRepLockDataSupport_ () {}
                    NO_COPY_CONSTRUCTOR(ContainerRepLockDataSupport_);
                    NO_ASSIGNMENT_OPERATOR(ContainerRepLockDataSupport_);
                };


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_Private_SynchronizationUtils_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SynchronizationUtils.inl"

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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






#include    "../../Debug/AssertExternallySynchronizedLock.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {


                struct  ContainerRepLockDataSupport_ {
                    /// testing more - but see comments above 'CAN break black box modularity' - may want to use recursive_mutex
                    mutable std::mutex                              fActiveIteratorsMutex_;
                    mutable Debug::AssertExternallySynchronizedLock fMutex_;
                    ContainerRepLockDataSupport_ () = default;
                    ContainerRepLockDataSupport_ (const ContainerRepLockDataSupport_&) = delete;
                    const ContainerRepLockDataSupport_& operator= (const ContainerRepLockDataSupport_&) = delete;
                };


#define CONTAINER_LOCK_HELPER_START(CRLDS)\
    {\
        std::shared_lock<Debug::AssertExternallySynchronizedLock> lg (CRLDS.fMutex_);\
        {
#define CONTAINER_LOCK_HELPER_END()\
}\
}
#define CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START(CRLDS)\
    {\
        std::lock_guard<std::mutex> lg (CRLDS.fActiveIteratorsMutex_);\
        {
#define CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END()\
}\
}



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

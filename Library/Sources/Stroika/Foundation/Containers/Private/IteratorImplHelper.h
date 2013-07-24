/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"

#include    "../Common.h"

#include    "SynchronizationUtils.h"



/**
 *  Private utilities to support building subtypes of Containers::Iterable<T>
 *
 * TODO:
 *
 * Notes:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {


                /**
                 */
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR = typename PATCHABLE_CONTAINER::ForwardIterator>
                class  IteratorImplHelper_ : public Iterator<T>::IRep {
                private:
                    typedef typename    Iterator<T>::IRep   inherited;

                public:
                    explicit IteratorImplHelper_ (ContainerRepLockDataSupport_* sharedLock, typename PATCHABLE_CONTAINER* data)
                        : inherited ()
                        , fLockSupport (*sharedLock)
                        , fIterator (*data) {
                        RequireNotNull (sharedLock);
                        RequireNotNull (data);
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorImplHelper_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport) {
                            return typename Iterator<T>::SharedIRepPtr (new IteratorImplHelper_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    More (T* current, bool advance) override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport) {
                            return (fIterator.More (current, advance));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                    ContainerRepLockDataSupport_&           fLockSupport;
                    mutable PATCHABLE_CONTAINER_ITERATOR    fIterator;
                };


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "IteratorImplHelper.inl"

#endif  /*_Stroika_Foundation_Containers_Private_IteratorImplHelper_h_ */

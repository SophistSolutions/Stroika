/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"

#include    "../Common.h"

#include    "SynchronizationUtils.h"



/**
 *  Private utility to support building of Traversal::Iterator<> objects for concrete Containers.
 *
 * TODO:
 *      @todo   Crazy temphack cuz current code assumes you must call++ before starting iteration! Crazy!
 *              Issue is way we implemented the 'CURRENT' stuff with iterators - filling in after teh first
 *              More()...
 *
 *              At this point - this appears to be restricted to ALL BACKEND support Iterator (ForwardIterator) classes.
 *              The Stroika Iterator API doesn't appear to have this quirk.
 *
 *
 * Notes:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {


                /**
                 *  There is no requirement that Stroika contcrete containers use this class. However, it
                 *  so far has appeared a handy code sharing utility.
                 */
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR = typename PATCHABLE_CONTAINER::ForwardIterator>
                class  IteratorImplHelper_ : public Iterator<T>::IRep {
                private:
                    using   inherited   =   typename    Iterator<T>::IRep;

                public:
                    using   SharedIRepPtr   =   typename Iterator<T>::SharedIRepPtr;
                    using   OwnerID         =   typename Iterator<T>::OwnerID;

                public:
                    explicit IteratorImplHelper_ (ContainerRepLockDataSupport_* sharedLock, PATCHABLE_CONTAINER* data);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorImplHelper_);

                    // Iterator<T>::IRep
                public:
                    virtual SharedIRepPtr Clone () const override;
                    virtual OwnerID       GetOwner () const override;
                    virtual void          More (Memory::Optional<T>* result, bool advance) override;
                    virtual bool          Equals (const typename Iterator<T>::IRep* rhs) const override;

                public:
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

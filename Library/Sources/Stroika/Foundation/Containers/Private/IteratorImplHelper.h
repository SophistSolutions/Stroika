/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"

#include    "../Common.h"



/**
 *  Private utility to support building of Traversal::Iterator<> objects for concrete Containers.
 *
 * TODO:
 *      @todo   Crazy temphack cuz current code assumes you must call++ before starting iteration! Crazy!
 *              Issue is way we implemented the 'CURRENT' stuff with iterators - filling in after the first
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


                using   Traversal::IteratorOwnerID;


                /**
                 *  There is no requirement that Stroika contcrete containers use this class. However, it
                 *  so far has appeared a handy code sharing utility.
                 *
                 *  Plus, its details are intimately tied to how the Stroika containers manage lifetime, so
                 *  its not likely well suited for use elsewhere.
                 */
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR = typename PATCHABLE_CONTAINER::ForwardIterator, typename PATCHABLE_CONTAINER_VALUE = T>
                class  IteratorImplHelper_ : public Iterator<T>::IRep {
                private:
                    using   inherited   =   typename    Iterator<T>::IRep;

                public:
                    using   SharedIRepPtr               =   typename Iterator<T>::SharedIRepPtr;
                    using   DataStructureImplValueType_ =   PATCHABLE_CONTAINER_VALUE;

                public:
                    IteratorImplHelper_ (const IteratorImplHelper_&) = default; // BE CAREFUL!!! - dont accidentally use without locks
                    explicit IteratorImplHelper_ (IteratorOwnerID owner, PATCHABLE_CONTAINER* data);

                public:
                    virtual ~IteratorImplHelper_ () = default;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorImplHelper_);

                    // Iterator<T>::IRep
                public:
                    virtual SharedIRepPtr       Clone () const override;
                    virtual IteratorOwnerID     GetOwner () const override;
                    virtual void                More (Memory::Optional<T>* result, bool advance) override;
                    virtual bool                Equals (const typename Iterator<T>::IRep* rhs) const override;

                private:
                    /*
                     *  More_SFINAE_ () trick is cuz if types are the same, we can just pass pointer, but if they differ, we need
                     *  a temporary, and to copy.
                     */
                    template    <typename CHECK_KEY = typename PATCHABLE_CONTAINER::value_type>
                    nonvirtual  void    More_SFINAE_ (Memory::Optional<T>* result, bool advance, typename std::enable_if<is_same<T, CHECK_KEY>::value>::type* = 0);
                    template    <typename CHECK_KEY = typename PATCHABLE_CONTAINER::value_type>
                    nonvirtual  void    More_SFINAE_ (Memory::Optional<T>* result, bool advance, typename std::enable_if < !is_same<T, CHECK_KEY>::value >::type* = 0);

                public:
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

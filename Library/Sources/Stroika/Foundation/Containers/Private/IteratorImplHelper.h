/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"

#include "../Common.h"

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

namespace Stroika::Foundation::Containers::Private {

    struct ContainerDebugChangeCounts_ {

        using ChangeCountType = unsigned int;

        // weird explicit def need here and cannot do = default? --LGP 2021-10-31
        ContainerDebugChangeCounts_ ()
        {
        }
        // weird explicit def need here and cannot do = default? --LGP 2021-10-31
        ContainerDebugChangeCounts_ ([[maybe_unused]] const ContainerDebugChangeCounts_& src)
#if qDebug
            // clang-format off
            : fChangeCount{src.fChangeCount.load ()}
        // clang-format on
#endif
        {
        }
#if qDebug
        atomic<ChangeCountType> fChangeCount{0};
#endif

        void PerformedChange ()
        {
#if qDebug
            fChangeCount++;
#endif
        }
    };

    /**
     *  \brief helper to wrap a low level 'DataStructure Container Iterator' into a 'Stroika' Iterator::IRep iterator.
     * 
     *  There is no requirement that Stroika contcrete containers use this class. However, it
     *  so far has appeared a handy code sharing utility.
     *
     *  Plus, its details are intimately tied to how the Stroika containers manage lifetime, so
     *  its not likely well suited for use elsewhere.
     */
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR = typename DATASTRUCTURE_CONTAINER::ForwardIterator, typename DATASTRUCTURE_CONTAINER_VALUE = T>
    class IteratorImplHelper_ : public Iterator<T>::IRep, public Memory::UseBlockAllocationIfAppropriate<IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>> {
    private:
        using inherited = typename Iterator<T>::IRep;

    public:
        using RepSmartPtr                 = typename Iterator<T>::RepSmartPtr;
        using DataStructureImplValueType_ = DATASTRUCTURE_CONTAINER_VALUE;

    public:
        IteratorImplHelper_ ()                           = delete;
        IteratorImplHelper_ (const IteratorImplHelper_&) = default;
        explicit IteratorImplHelper_ (const DATASTRUCTURE_CONTAINER* data, const ContainerDebugChangeCounts_* changeCounter = nullptr);

    public:
        virtual ~IteratorImplHelper_ () = default;

        // Iterator<T>::IRep
    public:
        virtual RepSmartPtr Clone () const override;
        virtual void        More (optional<T>* result, bool advance) override;
        virtual bool        Equals (const typename Iterator<T>::IRep* rhs) const override;

    public:
        /**
         * rarely used but in special cases when returning new dervied/pathed iterator*
         */
        void UpdateChangeCount ()
        {
#if qDebug
            if (fChangeCounter != nullptr) {
                fLastCapturedChangeCount = fChangeCounter->fChangeCount;
            }
#endif
        }
        void ValidateChangeCount () const
        {
#if qDebug
            if (fChangeCounter != nullptr) {
                Require (fChangeCounter->fChangeCount == fLastCapturedChangeCount); // if this fails, it almost certainly means you are using a stale iterator
            }
#endif
        }

    private:
        /*
         *  More_SFINAE_ () trick is cuz if types are the same, we can just pass pointer, but if they differ, we need
         *  a temporary, and to copy.
         */
        template <typename CHECK_KEY = typename DATASTRUCTURE_CONTAINER::value_type>
        nonvirtual void More_SFINAE_ (optional<T>* result, bool advance, enable_if_t<is_same_v<T, CHECK_KEY>>* = 0);
        template <typename CHECK_KEY = typename DATASTRUCTURE_CONTAINER::value_type>
        nonvirtual void More_SFINAE_ (optional<T>* result, bool advance, enable_if_t<!is_same_v<T, CHECK_KEY>>* = 0);

    public:
        mutable DATASTRUCTURE_CONTAINER_ITERATOR fIterator;
#if qDebug
        const ContainerDebugChangeCounts_*           fChangeCounter;
        ContainerDebugChangeCounts_::ChangeCountType fLastCapturedChangeCount;
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IteratorImplHelper.inl"

#endif /*_Stroika_Foundation_Containers_Private_IteratorImplHelper_h_ */

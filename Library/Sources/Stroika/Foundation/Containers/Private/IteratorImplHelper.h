/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Common.h"

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

    /**
     */
    struct ContainerDebugChangeCounts_ {
        using ChangeCountType = unsigned int;
#if qDebug
        static ChangeCountType mkInitial_ ();
#endif

        ContainerDebugChangeCounts_ ();
        ContainerDebugChangeCounts_ (const ContainerDebugChangeCounts_& src);
        ~ContainerDebugChangeCounts_ ();

#if qDebug
        // NOTE - might want to use weakly_shared_ptr (weakref) as safer 'debugging' check than this, but only
        // for debug checking, so not strictly needed (and might cost too much). CONSIDER going forward just
        // for DEBUG mode -- LGP 2021-11-11
        bool                    fDeleted{false};
        atomic<ChangeCountType> fChangeCount{0}; // overwritten with random# in CTOR
#endif

        /**
         */
        nonvirtual void PerformedChange ();
    };

    /**
     *  \brief helper to wrap a low level 'DataStructure Container Iterator' into a 'Stroika' Iterator::IRep iterator.
     * 
     *  There is no requirement that Stroika concrete containers use this class. However, it
     *  so far has appeared a handy code sharing utility.
     *
     *  Plus, its details are intimately tied to how the Stroika containers manage lifetime, so
     *  its not likely well suited for use elsewhere.
     * 
     *  \todo   This class is a bit kludgy/fragile. (e.g. the cases where you have to override More needing to also override Clone). Maybe use CRTP? And not
     *          good compiler error messages - maybe use more/better concepts usage; low priority since private impl helper method;
     *          -- LGP 2024-09-05
     */
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR = typename DATASTRUCTURE_CONTAINER::ForwardIterator, typename DATASTRUCTURE_CONTAINER_VALUE = T>
    class IteratorImplHelper_
        : public Iterator<T>::IRep,
          public Memory::UseBlockAllocationIfAppropriate<IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>> {
    private:
        using inherited = typename Iterator<T>::IRep;

    public:
        using DataStructureImplValueType_ = DATASTRUCTURE_CONTAINER_VALUE;

    public:
        IteratorImplHelper_ ()                               = delete;
        IteratorImplHelper_ (IteratorImplHelper_&&) noexcept = default;
        IteratorImplHelper_ (const IteratorImplHelper_&)     = default;
        template <typename... ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS>
        explicit IteratorImplHelper_ (const DATASTRUCTURE_CONTAINER* data, const ContainerDebugChangeCounts_* changeCounter = nullptr,
                                      ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS&&... args)
            requires (constructible_from<DATASTRUCTURE_CONTAINER_ITERATOR, const DATASTRUCTURE_CONTAINER*, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS...>);
        template <typename... ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS>
        explicit IteratorImplHelper_ (const ContainerDebugChangeCounts_* changeCounter = nullptr, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS&&... args)
            requires (constructible_from<DATASTRUCTURE_CONTAINER_ITERATOR, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS...>);

    public:
        virtual ~IteratorImplHelper_ () = default;

        // Iterator<T>::IRep
    public:
        virtual unique_ptr<typename Iterator<T>::IRep> Clone () const override;
        virtual void                                   More (optional<T>* result, bool advance) override;
        virtual bool                                   Equals (const typename Iterator<T>::IRep* rhs) const override;
#if qDebug
        /**
         */
        virtual void Invariant () const noexcept override;
#endif

    public:
        /**
         * rarely used but in special cases when returning new derived/pathed iterator*
         */
        nonvirtual void UpdateChangeCount ();

    public:
        /**
         *  This is used internally by Stroika to assure iterators are not used after the container they iterate over
         *  has been changed.
         */
        nonvirtual void ValidateChangeCount () const;

    public:
        mutable DATASTRUCTURE_CONTAINER_ITERATOR fIterator{};
#if qDebug
        const ContainerDebugChangeCounts_*           fChangeCounter{nullptr};
        ContainerDebugChangeCounts_::ChangeCountType fLastCapturedChangeCount{};
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

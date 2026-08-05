/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <random>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"

namespace Stroika::Foundation::Containers::Private {

    /*
     ********************************************************************************
     *************************** ContainerDebugChangeCounts_ ************************
     ********************************************************************************
     */
#if qStroika_Foundation_Debug_AssertionsChecked
    inline ContainerDebugChangeCounts_::ChangeCountType ContainerDebugChangeCounts_::mkInitial_ ()
    {
        // use random number so when we assign new object we are more likely to detect bad iterators (dangling)
        random_device                             rd;
        mt19937                                   gen{rd ()};
        uniform_int_distribution<ChangeCountType> distrib{1, 1000};
        return distrib (gen);
    }
#endif
    inline ContainerDebugChangeCounts_::ContainerDebugChangeCounts_ ()
#if qStroika_Foundation_Debug_AssertionsChecked
        : fChangeCount{mkInitial_ ()}
#endif
    {
    }
    inline ContainerDebugChangeCounts_::ContainerDebugChangeCounts_ ([[maybe_unused]] const ContainerDebugChangeCounts_& src)
#if qStroika_Foundation_Debug_AssertionsChecked
        : fDeleted{src.fDeleted}
        , fChangeCount{src.fChangeCount.load ()}
#endif
    {
    }
    inline ContainerDebugChangeCounts_::~ContainerDebugChangeCounts_ ()
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        fDeleted = true;
#endif
    }
    inline void ContainerDebugChangeCounts_::PerformedChange ()
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        ++fChangeCount;
#endif
    }

    /*
     ********************************************************************************
     ******* IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP> ******
     ********************************************************************************
     */
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    template <typename... ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS>
    inline IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::IteratorImplHelper_ (const DATASTRUCTURE_CONTAINER* data,
                                                                                                    [[maybe_unused]] const ContainerDebugChangeCounts_* changeCounter,
                                                                                                    ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS&&... args)
        requires (constructible_from<DATASTRUCTURE_CONTAINER_ITERATOR, const DATASTRUCTURE_CONTAINER*, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS...>)
        : fIterator{data, forward<ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS> (args)...}
#if qStroika_Foundation_Debug_AssertionsChecked
        , fChangeCounter{changeCounter}
        , fLastCapturedChangeCount{(changeCounter == nullptr) ? 0 : changeCounter->fChangeCount.load ()}
#endif
    {
        RequireNotNull (data);
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    template <typename... ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS>
    inline IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::IteratorImplHelper_ (
        [[maybe_unused]] const ContainerDebugChangeCounts_* changeCounter, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS&&... args)
        requires (constructible_from<DATASTRUCTURE_CONTAINER_ITERATOR, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS...>)
        : fIterator{forward<ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS> (args)...}
#if qStroika_Foundation_Debug_AssertionsChecked
        , fChangeCounter{changeCounter}
        , fLastCapturedChangeCount{(changeCounter == nullptr) ? 0 : changeCounter->fChangeCount.load ()}
#endif
    {
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    auto IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::Clone () const -> unique_ptr<typename Iterator<T>::IRep>
    {
        ValidateChangeCount ();
        // NOTE: guarded with if constexpr because when BASE_IREP != Iterator<T>::IRep (i.e. this class is being used
        // as the base of BidirectionalIteratorImplHelper_/RandomAccessIteratorImplHelper_, which add further pure
        // virtuals of their own), THIS particular instantiation of IteratorImplHelper_ is abstract, and the derived
        // class re-overrides Clone () to construct itself instead - so this branch must never actually be instantiated
        // for that case (virtual member functions of class templates are instantiated regardless of whether called).
        if constexpr (is_same_v<BASE_IREP, typename Iterator<T>::IRep>) {
            return make_unique<IteratorImplHelper_> (*this);
        }
        else {
            AssertNotReached ();
            return nullptr;
        }
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    auto IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::AtEnd () const -> bool
    {
        return fIterator.AtEnd ();
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    optional<T> IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::Current () const
    {
        if (fIterator.AtEnd ()) {
            return nullopt;
        }
        else {
            return TRAITS::ConvertDataStructureIterationResult2ContainerIteratorResult (*fIterator);
        }
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    optional<T> IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::More ()
    {
        Require (not fIterator.AtEnd ());
        ValidateChangeCount ();
        ++fIterator;
        if (fIterator.AtEnd ()) [[unlikely]] {
            return nullopt;
        }
        else {
            return TRAITS::ConvertDataStructureIterationResult2ContainerIteratorResult (*fIterator);
        }
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    bool IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::Equals (const typename Iterator<T>::IRep* rhs) const
    {
        RequireNotNull (rhs);
        using ActualIterImplType_       = IteratorImplHelper_;
        const ActualIterImplType_* rrhs = Debug::UncheckedDynamicCast<const ActualIterImplType_*> (rhs);
        return fIterator == rrhs->fIterator;
    }
#if qStroika_Foundation_Debug_AssertionsChecked
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::Invariant () const noexcept
    {
        ValidateChangeCount ();
    }
#endif
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    inline void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::UpdateChangeCount ()
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        if (fChangeCounter != nullptr) {
            fLastCapturedChangeCount = fChangeCounter->fChangeCount;
        }
#endif
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    inline void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::ValidateChangeCount () const
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        if (fChangeCounter != nullptr) {
            Require (not fChangeCounter->fDeleted); // if this is triggered, it means the container changed so drastically that its rep was deleted
            Require (fChangeCounter->fChangeCount == fLastCapturedChangeCount); // if this fails, it almost certainly means you are using a stale iterator
        }
#endif
    }

    /*
     ********************************************************************************
     * BidirectionalIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP> *
     ********************************************************************************
     */
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    auto BidirectionalIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::Clone () const -> unique_ptr<typename Iterator<T>::IRep>
    {
        this->ValidateChangeCount ();
        // see NOTE on IteratorImplHelper_::Clone () above - same reasoning: only valid to self-construct when
        // BASE_IREP is exactly BidirectionalIterator<T>::IRep (this class's own default/standalone use); when
        // used as the base of RandomAccessIteratorImplHelper_, that class re-overrides Clone () instead.
        if constexpr (is_same_v<BASE_IREP, typename Traversal::BidirectionalIterator<T>::IRep>) {
            return make_unique<BidirectionalIteratorImplHelper_> (*this);
        }
        else {
            AssertNotReached ();
            return nullptr;
        }
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    bool BidirectionalIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::AtStart () const
    {
        return this->fIterator.AtStart ();
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS, typename BASE_IREP>
    T BidirectionalIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS, BASE_IREP>::Back ()
    {
        Require (not this->fIterator.AtStart ());
        this->ValidateChangeCount ();
        --this->fIterator;
        return *this->fIterator;
    }

    /*
     ********************************************************************************
     ***** RandomAccessIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS> ******
     ********************************************************************************
     */
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS>
    auto RandomAccessIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS>::Clone () const -> unique_ptr<typename Iterator<T>::IRep>
    {
        this->ValidateChangeCount ();
        return make_unique<RandomAccessIteratorImplHelper_> (*this);
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS>
    void RandomAccessIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS>::Advance (ptrdiff_t i)
    {
        this->ValidateChangeCount ();
        this->fIterator += i;
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS>
    ptrdiff_t RandomAccessIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS>::Difference (const typename Traversal::RandomAccessIterator<T>::IRep* rhs) const
    {
        this->ValidateChangeCount ();
        if (rhs == nullptr) {
            // nullptr means 'end' - the backend iterator's own operator- knows how to handle a default-constructed
            // (sentinel/end) instance of itself on either side (see e.g. Array<T>::ForwardIterator).
            return this->fIterator - typename TRAITS::DataStructureIteratorT{};
        }
        using ActualIterImplType_       = RandomAccessIteratorImplHelper_;
        const ActualIterImplType_* rrhs = Debug::UncheckedDynamicCast<const ActualIterImplType_*> (rhs);
        return this->fIterator - rrhs->fIterator;
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename TRAITS>
    const T* RandomAccessIteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, TRAITS>::PeekAtElement (ptrdiff_t i) const
    {
        this->ValidateChangeCount ();
        return &this->fIterator[i];
    }

}

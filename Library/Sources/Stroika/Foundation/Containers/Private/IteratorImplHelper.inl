/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
     *IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR,DATASTRUCTURE_CONTAINER_VALUE> *
     ********************************************************************************
     */
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    template <typename... ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS>
    inline IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::IteratorImplHelper_ (
        const DATASTRUCTURE_CONTAINER* data, [[maybe_unused]] const ContainerDebugChangeCounts_* changeCounter,
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
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    template <typename... ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS>
    inline IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::IteratorImplHelper_ (
        [[maybe_unused]] const ContainerDebugChangeCounts_* changeCounter, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS&&... args)
        requires (constructible_from<DATASTRUCTURE_CONTAINER_ITERATOR, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS...>)
        : fIterator{forward<ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS> (args)...}
#if qStroika_Foundation_Debug_AssertionsChecked
        , fChangeCounter{changeCounter}
        , fLastCapturedChangeCount{(changeCounter == nullptr) ? 0 : changeCounter->fChangeCount.load ()}
#endif
    {
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    auto IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::Clone () const
        -> unique_ptr<typename Iterator<T>::IRep>
    {
        ValidateChangeCount ();
        return make_unique<IteratorImplHelper_> (*this);
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::More (optional<T>* result, bool advance)
    {
        if constexpr (convertible_to<decltype (*fIterator), T>) {
            RequireNotNull (result); // API says result ptr required
            ValidateChangeCount ();
            // Typically calls have advance = true
            if (advance) [[likely]] {
                Require (not fIterator.Done ());
                ++fIterator;
            }
            if (fIterator.Done ()) [[unlikely]] {
                *result = nullopt;
            }
            else {
                *result = *fIterator;
            }
        }
        else {
            RequireNotReached (); // "If this fails, you must override ::More, and provide it yourself"; and remember to override Clone() too!!!
        }
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    bool IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::Equals (
        const typename Iterator<T>::IRep* rhs) const
    {
        RequireNotNull (rhs);
        using ActualIterImplType_ = IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>;
        const ActualIterImplType_* rrhs = Debug::UncheckedDynamicCast<const ActualIterImplType_*> (rhs);
        return fIterator == rrhs->fIterator;
    }
#if qStroika_Foundation_Debug_AssertionsChecked
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::Invariant () const noexcept
    {
        ValidateChangeCount ();
    }
#endif
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    inline void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::UpdateChangeCount ()
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        if (fChangeCounter != nullptr) {
            fLastCapturedChangeCount = fChangeCounter->fChangeCount;
        }
#endif
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    inline void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::ValidateChangeCount () const
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        if (fChangeCounter != nullptr) {
            Require (not fChangeCounter->fDeleted); // if this is triggered, it means the container changed so drastically that its rep was deleted
            Require (fChangeCounter->fChangeCount == fLastCapturedChangeCount); // if this fails, it almost certainly means you are using a stale iterator
        }
#endif
    }

}

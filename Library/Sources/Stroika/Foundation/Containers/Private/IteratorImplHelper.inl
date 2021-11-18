/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ 1

#include <random>

#include "../../Debug/Assertions.h"
#include "../../Debug/Cast.h"

namespace Stroika::Foundation::Containers::Private {

    /*
     ********************************************************************************
     *************************** ContainerDebugChangeCounts_ ************************
     ********************************************************************************
     */
#if qDebug
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
#if qDebug
        // clang-format off
        : fChangeCount{mkInitial_ ()}
    // clang-format on
#endif
    {
    }
    inline ContainerDebugChangeCounts_::ContainerDebugChangeCounts_ ([[maybe_unused]] const ContainerDebugChangeCounts_& src)
#if qDebug
        // clang-format off
        : fDeleted{src.fDeleted}
        , fChangeCount{src.fChangeCount.load ()}
    // clang-format on
#endif
    {
    }
    inline ContainerDebugChangeCounts_::~ContainerDebugChangeCounts_ ()
    {
#if qDebug
        fDeleted = true;
#endif
    }
    inline void ContainerDebugChangeCounts_::PerformedChange ()
    {
#if qDebug
        ++fChangeCount;
#endif
    }

    /*
     ********************************************************************************
     *IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR,DATASTRUCTURE_CONTAINER_VALUE> *
     ********************************************************************************
     */
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    inline IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::IteratorImplHelper_ (const DATASTRUCTURE_CONTAINER* data, [[maybe_unused]] const ContainerDebugChangeCounts_* changeCounter)
        // clang-format off
        : fIterator{data}
#if qDebug
        , fChangeCounter{changeCounter}
        , fLastCapturedChangeCount { (changeCounter == nullptr) ? 0 : changeCounter->fChangeCount.load () }
#endif
    // clang-format on
    {
        RequireNotNull (data);
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    typename Iterator<T>::RepSmartPtr IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::Clone () const
    {
        ValidateChangeCount ();
        return Iterator<T>::template MakeSmartPtr<IteratorImplHelper_> (*this);
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::More (optional<T>* result, bool advance)
    {
        ValidateChangeCount ();
        // Typically calls have advance = true
        if (advance) [[LIKELY_ATTR]] {
            Require (not fIterator.Done ()); // new requirement since Stroika 2.1b14
            ++fIterator;
        }
        if (fIterator.Done ()) [[UNLIKELY_ATTR]] {
            *result = nullopt;
        }
        else {
            *result = fIterator.Current (); // better to use move()?
#if 0
            if constexpr (is_same_v<T, DataStructureImplValueType_>) {
            }
            else {
                // Sometimes we use KeyValuePair<A,B> and map that to pair<A,B> in STL container, and this 'adapter' masks that difference.
                *result = move (fIterator.Current ()); // better to use move()?
            }
#endif
        }
#if 0
        if constexpr (is_same_v<T, DataStructureImplValueType_>) {
            fIterator.More (result, advance);
        }
        else {
            // Sometimes we use KeyValuePair<A,B> and map that to pair<A,B> in STL container, and this 'adapter' masks that difference.
            optional<DataStructureImplValueType_> tmp;
            fIterator.More (&tmp, advance);
            *result = move (tmp);
        }
#endif
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    bool IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::Equals (const typename Iterator<T>::IRep* rhs) const
    {
        RequireNotNull (rhs);
        using ActualIterImplType_ = IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>;
        RequireMember (rhs, ActualIterImplType_);
        const ActualIterImplType_* rrhs = Debug::UncheckedDynamicCast<const ActualIterImplType_*> (rhs);
        AssertNotNull (rrhs);
        return fIterator.Equals (rrhs->fIterator);
    }
#if qDebug
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::Invariant () const
    {
        ValidateChangeCount ();
    }
#endif
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    inline void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::UpdateChangeCount ()
    {
#if qDebug
        if (fChangeCounter != nullptr) {
            fLastCapturedChangeCount = fChangeCounter->fChangeCount;
        }
#endif
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    inline void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::ValidateChangeCount () const
    {
#if qDebug
        if (fChangeCounter != nullptr) {
            Require (not fChangeCounter->fDeleted);                             // if this is triggered, it means the container changed so drastically that its rep was deleted
            Require (fChangeCounter->fChangeCount == fLastCapturedChangeCount); // if this fails, it almost certainly means you are using a stale iterator
        }
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ */

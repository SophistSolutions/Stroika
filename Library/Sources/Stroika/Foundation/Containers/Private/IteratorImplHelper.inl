/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::Private {






    /*
     ********************************************************************************
     ********************************* ContainerDebugChangeCounts_ ******************
     ********************************************************************************
     */
    inline  ContainerDebugChangeCounts_::ContainerDebugChangeCounts_ ()
    {
    }
    inline ContainerDebugChangeCounts_::ContainerDebugChangeCounts_ ([[maybe_unused]] const ContainerDebugChangeCounts_& src)
#if qDebug
        // clang-format off
        : fChangeCount{src.fChangeCount.load ()}
    // clang-format on
#endif
    {
    }
    inline void ContainerDebugChangeCounts_::PerformedChange ()
    {
#if qDebug
        fChangeCount++;
#endif
    }

    /*
     ********************************************************************************
     *IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR,DATASTRUCTURE_CONTAINER_VALUE> *
     ********************************************************************************
     */
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    inline IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::IteratorImplHelper_ (const DATASTRUCTURE_CONTAINER* data, [[maybe_unused]] const ContainerDebugChangeCounts_* changeCounter)
        : fIterator
    {
        data
    }
#if qDebug
    , fChangeCounter{changeCounter},
        fLastCapturedChangeCount
    {
        changeCounter == nullptr ? 0 : changeCounter->fChangeCount.load ()
    }
#endif
    {
        RequireNotNull (data);
        fIterator.More (static_cast<DataStructureImplValueType_*> (nullptr), true); //tmphack cuz current backend iterators require a first more() - fix that!
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
        RequireNotNull (result);
        // NOTE: the reason this is Debug::AssertExternallySynchronizedLock, is because we only modify data on the newly cloned (breakreferences)
        // iterator, and that must be in the thread (so externally synchronized) of the modifier
        //    shared_lock<const Debug::AssertExternallySynchronizedLock> lg (*fIterator.GetPatchableContainerHelper ());
        ValidateChangeCount ();
        More_SFINAE_ (result, advance);
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    template <typename CHECK_KEY>
    inline void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::More_SFINAE_ (optional<T>* result, bool advance, enable_if_t<is_same_v<T, CHECK_KEY>>*)
    {
        RequireNotNull (result);
        fIterator.More (result, advance);
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    template <typename CHECK_KEY>
    inline void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::More_SFINAE_ (optional<T>* result, bool advance, enable_if_t<!is_same_v<T, CHECK_KEY>>*)
    {
        RequireNotNull (result);
        optional<DataStructureImplValueType_> tmp;
        fIterator.More (&tmp, advance);
        if (tmp.has_value ()) {
            *result = *tmp;
        }
        else {
            *result = nullopt;
        }
    }
    template <typename T, typename DATASTRUCTURE_CONTAINER, typename DATASTRUCTURE_CONTAINER_ITERATOR, typename DATASTRUCTURE_CONTAINER_VALUE>
    bool IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::Equals (const typename Iterator<T>::IRep* rhs) const
    {
        RequireNotNull (rhs);
        using ActualIterImplType_ = IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>;
        RequireMember (rhs, ActualIterImplType_);
        const ActualIterImplType_* rrhs = dynamic_cast<const ActualIterImplType_*> (rhs);
        AssertNotNull (rrhs);
        //  ValidateChangeCount ();
        //  rhs->ValidateChangeCount ();
        //        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec1 (*fIterator.GetPatchableContainerHelper ());
        //      shared_lock<const Debug::AssertExternallySynchronizedLock> critSec2 (*rrhs->fIterator.GetPatchableContainerHelper ());
        return fIterator.Equals (rrhs->fIterator);
    }

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
    inline    void IteratorImplHelper_<T, DATASTRUCTURE_CONTAINER, DATASTRUCTURE_CONTAINER_ITERATOR, DATASTRUCTURE_CONTAINER_VALUE>::ValidateChangeCount () const
        {
#if qDebug
            if (fChangeCounter != nullptr) {
                Require (fChangeCounter->fChangeCount == fLastCapturedChangeCount); // if this fails, it almost certainly means you are using a stale iterator
            }
#endif
        }


}

#endif /* _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ */

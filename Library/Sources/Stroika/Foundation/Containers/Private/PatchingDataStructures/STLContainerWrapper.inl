/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_inl_ 1

#include <algorithm>

#include "../../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::Private::PatchingDataStructures {

    /*
     ********************************************************************************
     ******* PatchingDataStructures::STLContainerWrapper<STL_CONTAINER_OF_T> ********
     ********************************************************************************
     */
    template <typename STL_CONTAINER_OF_T>
    template <typename... CONTAINER_EXTRA_ARGS>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::STLContainerWrapper (CONTAINER_EXTRA_ARGS&&... stdContainerArgs)
        : inherited (inherited::eCreateNewConstructorSelector, forward<CONTAINER_EXTRA_ARGS> (stdContainerArgs)...)
    {
    }
    template <typename STL_CONTAINER_OF_T>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::STLContainerWrapper (STLContainerWrapper<STL_CONTAINER_OF_T>* rhs, IteratorOwnerID newOwnerID)
        : inherited (rhs, newOwnerID, (ForwardIterator*)nullptr)
    {
        RequireNotNull (rhs);
        rhs->Invariant ();
        Invariant ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename INSERT_VALUE_TYPE>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::insert_toVector_WithPatching (typename STL_CONTAINER_OF_T::iterator i, INSERT_VALUE_TYPE v)
    {
        Invariant ();
        Memory::SmallStackBuffer<size_t> patchOffsets (0);
        TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (&patchOffsets, i);
        this->insert (i, v);
        TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (patchOffsets);
        Invariant ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::erase_WithPatching (typename STL_CONTAINER_OF_T::iterator i)
    {
        Invariant ();
        Memory::SmallStackBuffer<ForwardIterator*> items2Patch (0);
        TwoPhaseIteratorPatcherPass1 (i, &items2Patch);
        auto newI = this->erase (i);
        TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
        Invariant ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::clear_WithPatching ()
    {
        Invariant ();
        this->clear ();
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([this] (ForwardIterator* ai) {
            ai->TwoPhaseIteratorPatcherPass2 (this->end ());
        });
        Invariant ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const
    {
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([oldI, items2Patch] (ForwardIterator* ai) {
            ai->TwoPhaseIteratorPatcherPass1 (oldI, items2Patch);
        });
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, typename STL_CONTAINER_OF_T::iterator newI)
    {
        for (size_t i = 0; i < items2Patch->GetSize (); ++i) {
            (*items2Patch)[i]->TwoPhaseIteratorPatcherPass2 (newI);
        }
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (Memory::SmallStackBuffer<size_t>* patchOffsets) const
    {
        RequireNotNull (patchOffsets);
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([this, patchOffsets] (ForwardIterator* ai) {
            size_t idx = ai->fStdIterator - this->begin ();
            patchOffsets->push_back (idx);
        });
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (Memory::SmallStackBuffer<size_t>* patchOffsets, typename STL_CONTAINER_OF_T::iterator incIfGreaterOrEqual) const
    {
        RequireNotNull (patchOffsets);
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([&] (ForwardIterator* ai) {
            typename STL_CONTAINER_OF_T::iterator aIt = ai->fStdIterator;
            size_t                                idx = ai->fStdIterator - this->begin ();
            if (incIfGreaterOrEqual <= aIt) {
                idx++;
            }
            patchOffsets->push_back (idx);
        });
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (const Memory::SmallStackBuffer<size_t>& patchOffsets)
    {
        size_t i = 0;
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([this, &patchOffsets, &i] (ForwardIterator* ai) {
            Assert (patchOffsets[i] < this->size ());
            ai->fStdIterator = this->begin () + patchOffsets[i];
            ++i;
        });
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::Invariant () const
    {
#if qDebug
        _Invariant ();
#endif
    }
#if qDebug
    template <typename STL_CONTAINER_OF_T>
    void STLContainerWrapper<STL_CONTAINER_OF_T>::_Invariant () const
    {
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([] (ForwardIterator* ai) {
            ai->Invariant ();
        });
    }
#endif

    /*
     ********************************************************************************
     * PatchingDataStructures::STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator *
     ********************************************************************************
     */
    template <typename STL_CONTAINER_OF_T>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (IteratorOwnerID ownerID, CONTAINER_TYPE* data)
        : inherited_DataStructure (data)
        , inherited_PatchHelper (const_cast<STLContainerWrapper<STL_CONTAINER_OF_T>*> (data), ownerID)
    {
        RequireNotNull (data);
        this->Invariant ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
        : inherited_DataStructure (from)
        , inherited_PatchHelper (from)
    {
        this->Invariant ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::~ForwardIterator ()
    {
        this->Invariant ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::RemoveCurrent ()
    {
        AssertNotNull (this->fData);
        static_cast<STLContainerWrapper<STL_CONTAINER_OF_T>*> (this->fData)->erase_WithPatching (this->fStdIterator);
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch)
    {
        if (this->fStdIterator == oldI) {
            items2Patch->push_back (this);
        }
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::TwoPhaseIteratorPatcherPass2 (typename STL_CONTAINER_OF_T::iterator newI)
    {
        this->SetCurrentLink (newI);
        this->fSuppressMore = true;
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Invariant () const
    {
#if qDebug
        _Invariant ();
#endif
    }
#if qDebug
    template <typename STL_CONTAINER_OF_T>
    void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::_Invariant () const
    {
        AssertNotNull (this->fData);
        // apparently pointless test is just to force triggering any check code in iterator classes for valid iterators
        Assert (this->fStdIterator == this->fData->end () or this->fStdIterator != this->fData->end ());
        // Find some way to check iterator traits to see if random access and do this test if possible
        //Assert ((fCurrent >= fStart) and (fCurrent <= fEnd));   // ANSI C requires this is always TRUE
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_inl_ */

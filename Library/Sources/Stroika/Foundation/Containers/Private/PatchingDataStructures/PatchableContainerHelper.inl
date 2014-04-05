/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_  1


#include    "../../../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Private {
                namespace   PatchingDataStructures {


                    /*
                    ********************************************************************************
                    ***** PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER> *******
                    ********************************************************************************
                    */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    template    <typename COMBINED_ITERATOR>
                    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::PatchableContainerHelper (PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>* rhs, IteratorOwnerID newOwnerID, COMBINED_ITERATOR* fakePtrForOverload)
                        : inherited (*rhs)
                        , fLockSupport ()
                        , fActiveIteratorsListHead (nullptr)
                    {
                        Require (not HasActiveIterators ());
Again:
                        for (auto v = rhs->fActiveIteratorsListHead; v != nullptr; v = v->fNextActiveIterator) {
                            if (v->fOwnerID == newOwnerID) {
                                COMBINED_ITERATOR*  x = static_cast<COMBINED_ITERATOR*> (v);
                                this->MoveIteratorAfterClone (x, rhs);
                                goto Again;
                            }
                        }
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::~PatchableContainerHelper ()
                    {
                        Require (not HasActiveIterators ()); // cannot destroy container with active iterators
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    template <typename ACTUAL_ITERATOR_TYPE>
                    inline  ACTUAL_ITERATOR_TYPE*   PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::GetFirstActiveIterator () const
                    {
                        return static_cast<ACTUAL_ITERATOR_TYPE*> (fActiveIteratorsListHead);
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                    {
#if     qDebug
                        AssertNoIteratorsReferenceOwner_ (oBeingDeleted);
#endif
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  bool    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::HasActiveIterators () const
                    {
                        return bool (fActiveIteratorsListHead != nullptr);
                    }
#if     qDebug
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::AssertNoIteratorsReferenceOwner_ (IteratorOwnerID oBeingDeleted) const
                    {
                        for (auto v = fActiveIteratorsListHead; v != nullptr; v = v->fNextActiveIterator) {
                            Assert (v->fOwnerID != oBeingDeleted);
                        }
                    }
#endif
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    template    <typename PATCHABLE_ITERATOR_MIXIN_SUBTYPE>
                    void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::MoveIteratorAfterClone (PATCHABLE_ITERATOR_MIXIN_SUBTYPE* pi, PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>* fromContainer)
                    {
                        fromContainer->RemoveIterator (pi);
                        this->AddIterator (pi);
                        this->MoveIteratorHereAfterClone (pi, fromContainer);
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::AddIterator (PatchableIteratorMixIn* pi)
                    {
                        RequireNotNull (pi);
                        Assert (pi->fNextActiveIterator == nullptr);
                        pi->fNextActiveIterator = fActiveIteratorsListHead;
                        fActiveIteratorsListHead = pi;
                        pi->fPatchableContainer = this;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::RemoveIterator (PatchableIteratorMixIn* pi)
                    {
                        RequireNotNull (pi);
                        if (fActiveIteratorsListHead == pi) {
                            fActiveIteratorsListHead = pi->fNextActiveIterator;
                        }
                        else {
                            PatchableIteratorMixIn*    v = fActiveIteratorsListHead;
                            for (; v->fNextActiveIterator != pi; v = v->fNextActiveIterator) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNextActiveIterator);
                            }
                            AssertNotNull (v);
                            Assert (v->fNextActiveIterator == pi);
                            v->fNextActiveIterator = pi->fNextActiveIterator;
                        }
                        pi->fPatchableContainer = nullptr;
                        pi->fNextActiveIterator = nullptr;      // unlink
                    }



                    /*
                    ********************************************************************************
                    * PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::PatchableIteratorMixIn *
                    ********************************************************************************
                    */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    template    <typename ACTUAL_ITERATOR_TYPE>
                    inline  ACTUAL_ITERATOR_TYPE*   PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::PatchableIteratorMixIn::GetNextActiveIterator () const
                    {
                        return static_cast<ACTUAL_ITERATOR_TYPE*> (fNextActiveIterator);
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  IteratorOwnerID PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::PatchableIteratorMixIn::GetOwner () const
                    {
                        return fOwnerID;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::PatchableIteratorMixIn::PatchableIteratorMixIn (PatchableContainerHelper* containerHelper, IteratorOwnerID ownerID)
                        : fPatchableContainer (containerHelper)
                        , fOwnerID (ownerID)
                        , fNextActiveIterator (containerHelper->fActiveIteratorsListHead)
                    {
                        RequireNotNull (containerHelper);
                        containerHelper->fActiveIteratorsListHead = this;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::PatchableIteratorMixIn::PatchableIteratorMixIn (const PatchableIteratorMixIn& from)
                        : fPatchableContainer (from.fPatchableContainer)
                        , fOwnerID (from.fOwnerID)
                        , fNextActiveIterator (from.fPatchableContainer->fActiveIteratorsListHead)
                    {
                        RequireNotNull (fPatchableContainer);
                        fPatchableContainer->fActiveIteratorsListHead = this;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::PatchableIteratorMixIn::~PatchableIteratorMixIn ()
                    {
#if     qStroika_Foundation_Traveral_IteratorHoldsSharedPtr_
                        if (fPatchableContainer != nullptr) {
                            AssertNotNull (fPatchableContainer);
                            fPatchableContainer->RemoveIterator (this);
                        }
#else
                        AssertNotNull (fPatchableContainer);
                        fPatchableContainer->RemoveIterator (this);
                        // could assert owner  - fPatchableContainer - doenst contian us in list
#endif
                        Assert (fNextActiveIterator == nullptr);
                        Assert (fPatchableContainer == nullptr);
                    }


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_ */

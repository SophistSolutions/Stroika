/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
                    ********* PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS> ***********
                    ********************************************************************************
                    */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template    <typename COMBINED_ITERATOR>
                    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableContainerHelper (PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>* rhs, IteratorOwnerID newOwnerID, COMBINED_ITERATOR* fakePtrForOverload)
                        : inherited ((RequireNotNull (rhs), *rhs))
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                        Assert (not HasActiveIterators ());
                        std::lock_guard<std::mutex> critSec (rhs->fActiveIteratorsMutex_);
Again:
                        for (auto v = rhs->fActiveIteratorsListHead_; v != nullptr; v = v->fNextActiveIterator) {
                            if (v->fOwnerID == newOwnerID) {
                                COMBINED_ITERATOR*  x = static_cast<COMBINED_ITERATOR*> (v);
                                this->MoveIteratorAfterClone_ (x, rhs);
                                goto Again;
                            }
                        }
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::~PatchableContainerHelper ()
                    {
                        Require (not HasActiveIterators ()); // cannot destroy container with active iterators
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template    <typename ACTUAL_ITERATOR_TYPE>
                    inline  ACTUAL_ITERATOR_TYPE*   PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::GetFirstActiveIterator () const
                    {
                        return static_cast<ACTUAL_ITERATOR_TYPE*> (fActiveIteratorsListHead_);
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                    {
#if     qDebug
                        AssertNoIteratorsReferenceOwner_ (oBeingDeleted);
#endif
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  bool    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::HasActiveIterators () const
                    {
                        return fActiveIteratorsListHead_ != nullptr;
                    }
#if     qDebug
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AssertNoIteratorsReferenceOwner_ (IteratorOwnerID oBeingDeleted) const
                    {
                        std::lock_guard<std::mutex> critSec (fActiveIteratorsMutex_);
                        for (auto v = fActiveIteratorsListHead_; v != nullptr; v = v->fNextActiveIterator) {
                            Assert (v->fOwnerID != oBeingDeleted);
                        }
                    }
#endif
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template    <typename PATCHABLE_ITERATOR_MIXIN_SUBTYPE>
                    void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::MoveIteratorAfterClone_ (PATCHABLE_ITERATOR_MIXIN_SUBTYPE* pi, PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>* fromContainer)
                    {
                        RequireNotNull (fromContainer);
                        RequireNotNull (pi);
                        fromContainer->RemoveIterator_ (pi);
                        this->AddIterator_ (pi);
                        this->MoveIteratorHereAfterClone (pi, fromContainer);
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AddIterator_ (PatchableIteratorMixIn* pi)
                    {
                        RequireNotNull (pi);
                        Assert (pi->fNextActiveIterator == nullptr);
                        pi->fNextActiveIterator = fActiveIteratorsListHead_;
                        fActiveIteratorsListHead_ = pi;
                        pi->fPatchableContainer = this;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::RemoveIterator_ (PatchableIteratorMixIn* pi)
                    {
                        RequireNotNull (pi);
                        if (fActiveIteratorsListHead_ == pi) {
                            fActiveIteratorsListHead_ = pi->fNextActiveIterator;
                        }
                        else {
                            PatchableIteratorMixIn*    v = fActiveIteratorsListHead_;
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
                     * PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn *
                     ********************************************************************************
                     */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template    <typename ACTUAL_ITERATOR_TYPE>
                    inline  ACTUAL_ITERATOR_TYPE*   PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::GetNextActiveIterator () const
                    {
                        return static_cast<ACTUAL_ITERATOR_TYPE*> (fNextActiveIterator);
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  auto PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::GetPatchableContainerHelper () const -> PatchableContainerHelper*
                    {
                        EnsureNotNull (fPatchableContainer);
                        return fPatchableContainer;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  IteratorOwnerID PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::GetOwner () const
                    {
                        return fOwnerID;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::PatchableIteratorMixIn (PatchableContainerHelper* containerHelper, IteratorOwnerID ownerID)
                        : fPatchableContainer (containerHelper)
                        , fOwnerID (ownerID)
                        , fNextActiveIterator {}    // fPatchableContainer must be read inside mutex
                    {
                        RequireNotNull (containerHelper);
                        std::lock_guard<std::mutex> critSec (containerHelper->fActiveIteratorsMutex_);
                        fNextActiveIterator = containerHelper->fActiveIteratorsListHead_;
                        containerHelper->fActiveIteratorsListHead_ = this;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::PatchableIteratorMixIn (const PatchableIteratorMixIn& from)
                        : fPatchableContainer (from.fPatchableContainer)
                        , fOwnerID (from.fOwnerID)
                        , fNextActiveIterator {}    // fPatchableContainer must be read inside mutex
                    {
                        RequireNotNull (fPatchableContainer);
                        std::lock_guard<std::mutex> critSec (fPatchableContainer->fActiveIteratorsMutex_);
                        fNextActiveIterator = from.fPatchableContainer->fActiveIteratorsListHead_;
                        fPatchableContainer->fActiveIteratorsListHead_ = this;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::~PatchableIteratorMixIn ()
                    {
                        AssertNotNull (fPatchableContainer);
                        AssertNotNull (fPatchableContainer);
                        std::lock_guard<std::mutex> critSec (fPatchableContainer->fActiveIteratorsMutex_);
                        fPatchableContainer->RemoveIterator_ (this);
                        Assert (fNextActiveIterator == nullptr);
                        Assert (fPatchableContainer == nullptr);
                    }


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_ */

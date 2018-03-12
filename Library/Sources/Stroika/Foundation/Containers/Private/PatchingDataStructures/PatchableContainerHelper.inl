/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_ 1

#include "../../../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Private {
                namespace PatchingDataStructures {

                    /*
                     ********************************************************************************
                     ********* PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS> ***********
                     ********************************************************************************
                     */
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template <typename COMBINED_ITERATOR>
                    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableContainerHelper (PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>* rhs, IteratorOwnerID newOwnerID, COMBINED_ITERATOR* fakePtrForOverload)
                        : inherited ((RequireNotNull (rhs), *rhs))
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                        Assert (not HasActiveIterators ());
                        std::lock_guard<std::mutex> critSec (rhs->fActiveIteratorsMutex_);
                    Again:
                        for (auto v = rhs->fActiveIteratorsListHead_; v != nullptr; v = v->fNextActiveIterator_) {
                            if (v->fOwnerID_ == newOwnerID) {
                                COMBINED_ITERATOR* x = static_cast<COMBINED_ITERATOR*> (v);
                                this->MoveIteratorAfterClone_ (x, rhs);
                                goto Again;
                            }
                        }
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::~PatchableContainerHelper ()
                    {
                        Require (not HasActiveIterators ()); // cannot destroy container with active iterators
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template <typename ACTUAL_ITERATOR_TYPE>
                    inline ACTUAL_ITERATOR_TYPE* PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::GetFirstActiveIterator_ () const
                    {
                        return static_cast<ACTUAL_ITERATOR_TYPE*> (fActiveIteratorsListHead_);
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline void PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                    {
#if qDebug
                        AssertNoIteratorsReferenceOwner_ (oBeingDeleted);
#endif
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline bool PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::HasActiveIterators () const
                    {
                        return fActiveIteratorsListHead_ != nullptr;
                    }
#if qDebug
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    void PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AssertNoIteratorsReferenceOwner_ (IteratorOwnerID oBeingDeleted) const
                    {
                        std::lock_guard<std::mutex> critSec (fActiveIteratorsMutex_);
                        for (auto v = fActiveIteratorsListHead_; v != nullptr; v = v->fNextActiveIterator_) {
                            Assert (v->fOwnerID_ != oBeingDeleted);
                        }
                    }
#endif
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template <typename PATCHABLE_ITERATOR_MIXIN_SUBTYPE>
                    void PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::MoveIteratorAfterClone_ (PATCHABLE_ITERATOR_MIXIN_SUBTYPE* pi, PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>* fromContainer)
                    {
                        RequireNotNull (fromContainer);
                        RequireNotNull (pi);
                        fromContainer->RemoveIterator_ (pi);
                        this->AddIterator_ (pi);
                        this->MoveIteratorHereAfterClone (pi, fromContainer);
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline void PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AddIterator_ (PatchableIteratorMixIn* pi)
                    {
                        RequireNotNull (pi);
                        Assert (pi->fNextActiveIterator_ == nullptr);
                        pi->fNextActiveIterator_  = fActiveIteratorsListHead_;
                        fActiveIteratorsListHead_ = pi;
                        pi->fPatchableContainer_  = this;
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline void PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::RemoveIterator_ (PatchableIteratorMixIn* pi)
                    {
                        RequireNotNull (pi);
                        if (fActiveIteratorsListHead_ == pi) {
                            fActiveIteratorsListHead_ = pi->fNextActiveIterator_;
                        }
                        else {
                            PatchableIteratorMixIn* v = fActiveIteratorsListHead_;
                            for (; v->fNextActiveIterator_ != pi; v = v->fNextActiveIterator_) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNextActiveIterator_);
                            }
                            AssertNotNull (v);
                            Assert (v->fNextActiveIterator_ == pi);
                            v->fNextActiveIterator_ = pi->fNextActiveIterator_;
                        }
                        pi->fPatchableContainer_ = nullptr;
                        pi->fNextActiveIterator_ = nullptr; // unlink
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template <typename ACTIVE_ITERATOR_SUBTYPE, typename FUNCTION>
                    inline void PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::_ApplyToEachOwnedIterator (FUNCTION f) const
                    {
                        std::lock_guard<std::mutex> critSec (fActiveIteratorsMutex_);
                        for (auto ai = this->template GetFirstActiveIterator_<ACTIVE_ITERATOR_SUBTYPE> (); ai != nullptr; ai = ai->template GetNextActiveIterator_<ACTIVE_ITERATOR_SUBTYPE> ()) {
                            f (ai);
                        }
                    }

                    /*
                     ********************************************************************************
                     * PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn *
                     ********************************************************************************
                     */
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template <typename ACTUAL_ITERATOR_TYPE>
                    inline ACTUAL_ITERATOR_TYPE* PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::GetNextActiveIterator_ () const
                    {
                        return static_cast<ACTUAL_ITERATOR_TYPE*> (fNextActiveIterator_);
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline auto PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::GetPatchableContainerHelper () const -> PatchableContainerHelper*
                    {
                        EnsureNotNull (fPatchableContainer_);
                        return fPatchableContainer_;
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline IteratorOwnerID PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::GetOwner () const
                    {
                        return fOwnerID_;
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::PatchableIteratorMixIn (PatchableContainerHelper* containerHelper, IteratorOwnerID ownerID)
                        : fPatchableContainer_ (containerHelper)
                        , fOwnerID_ (ownerID)
                        , fNextActiveIterator_{} // fPatchableContainer_ must be read inside mutex
                    {
                        RequireNotNull (containerHelper);
                        std::lock_guard<std::mutex> critSec (containerHelper->fActiveIteratorsMutex_);
                        fNextActiveIterator_                       = containerHelper->fActiveIteratorsListHead_;
                        containerHelper->fActiveIteratorsListHead_ = this;
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::PatchableIteratorMixIn (const PatchableIteratorMixIn& from)
                        : fPatchableContainer_ (from.fPatchableContainer_)
                        , fOwnerID_ (from.fOwnerID_)
                        , fNextActiveIterator_{} // fPatchableContainer_ must be read inside mutex
                    {
                        RequireNotNull (fPatchableContainer_);
                        std::lock_guard<std::mutex> critSec (fPatchableContainer_->fActiveIteratorsMutex_);
                        fNextActiveIterator_                            = from.fPatchableContainer_->fActiveIteratorsListHead_;
                        fPatchableContainer_->fActiveIteratorsListHead_ = this;
                    }
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn::~PatchableIteratorMixIn ()
                    {
                        AssertNotNull (fPatchableContainer_);
                        std::lock_guard<std::mutex> critSec (fPatchableContainer_->fActiveIteratorsMutex_);
                        fPatchableContainer_->RemoveIterator_ (this);
                        Assert (fNextActiveIterator_ == nullptr);
                        Assert (fPatchableContainer_ == nullptr);
                    }
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_ */

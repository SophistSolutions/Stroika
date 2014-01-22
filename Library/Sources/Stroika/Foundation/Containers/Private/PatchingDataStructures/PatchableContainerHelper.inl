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
                    ********* PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS> ***********
                    ********************************************************************************
                    */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableContainerHelper (PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>* rhs, IteratorOwnerID newOwnerID)
                        : inherited (*rhs)
                        , fActiveIteratorsListHead (nullptr)
                    {
                        Require (not HasActiveIterators ());
Again:
                        for (auto v = rhs->fActiveIteratorsListHead; v != nullptr; v = v->fNextActiveIterator) {
                            if (v->fOwnerID == newOwnerID) {
                                // must move it
                                rhs->RemoveIterator (v);
                                this->AddIterator (v);
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
                    template <typename ACTUAL_ITERATOR_TYPE>
                    inline  ACTUAL_ITERATOR_TYPE*   PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::GetFirstActiveIterator () const
                    {
                        return static_cast<ACTUAL_ITERATOR_TYPE*> (fActiveIteratorsListHead);
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted)
                    {
#if     qDebug
                        AssertNoIteratorsReferenceOwner_ (oBeingDeleted);
#endif
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  bool    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::HasActiveIterators () const
                    {
                        return bool (fActiveIteratorsListHead != nullptr);
                    }
#if     qDebug
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AssertNoIteratorsReferenceOwner_ (IteratorOwnerID oBeingDeleted)
                    {
                        for (auto v = fActiveIteratorsListHead; v != nullptr; v = v->fNextActiveIterator) {
                            Assert (v->fOwnerID != oBeingDeleted);
                        }
                    }
#endif
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::AddIterator (PatchableIteratorMinIn* pi)
                    {
                        RequireNotNull (pi);
                        Assert (pi->fNextActiveIterator == nullptr);
                        pi->fNextActiveIterator = fActiveIteratorsListHead;
                        fActiveIteratorsListHead = pi;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  void    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::RemoveIterator (PatchableIteratorMinIn* pi)
                    {
                        RequireNotNull (pi);
                        if (fActiveIteratorsListHead == pi) {
                            fActiveIteratorsListHead = pi->fNextActiveIterator;
                        }
                        else {
                            PatchableIteratorMinIn*    v = fActiveIteratorsListHead;
                            for (; v->fNextActiveIterator != pi; v = v->fNextActiveIterator) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNextActiveIterator);
                            }
                            AssertNotNull (v);
                            Assert (v->fNextActiveIterator == pi);
                            v->fNextActiveIterator = pi->fNextActiveIterator;
                        }
                    }



                    /*
                    ********************************************************************************
                    * PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn *
                    ********************************************************************************
                    */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    template    <typename ACTUAL_ITERATOR_TYPE>
                    inline  ACTUAL_ITERATOR_TYPE*   PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn::GetNextActiveIterator () const
                    {
                        return static_cast<ACTUAL_ITERATOR_TYPE*> (fNextActiveIterator);
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  IteratorOwnerID PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn::GetOwner () const
                    {
                        return fOwnerID;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn::PatchableIteratorMinIn (PatchableContainerHelper* containerHelper, IteratorOwnerID ownerID)
                        : fPatchableContainer (containerHelper)
                        , fOwnerID (ownerID)
                        , fNextActiveIterator (containerHelper->fActiveIteratorsListHead)
                    {
                        RequireNotNull (containerHelper);
                        containerHelper->fActiveIteratorsListHead = this;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn::PatchableIteratorMinIn (const PatchableIteratorMinIn& from)
                        : fPatchableContainer (from.fPatchableContainer)
                        , fOwnerID (from.fOwnerID)
                        , fNextActiveIterator (from.fPatchableContainer->fActiveIteratorsListHead)
                    {
                        RequireNotNull (fPatchableContainer);
                        fPatchableContainer->fActiveIteratorsListHead = this;
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    inline  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn::~PatchableIteratorMinIn ()
                    {
                        AssertNotNull (fPatchableContainer);
                        fPatchableContainer->RemoveIterator (this);
                    }


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_ */

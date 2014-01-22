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
                    {
                        //tmpahck - for now dont split iteators
                        Require (not HasActiveIterators ()); //....
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
                        if (fPatchableContainer->fActiveIteratorsListHead == this) {
                            fPatchableContainer->fActiveIteratorsListHead = fNextActiveIterator;
                        }
                        else {
                            auto    v = fPatchableContainer->fActiveIteratorsListHead;
                            for (; v->fNextActiveIterator != this; v = v->fNextActiveIterator) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNextActiveIterator);
                            }
                            AssertNotNull (v);
                            Assert (v->fNextActiveIterator == this);
                            v->fNextActiveIterator = fNextActiveIterator;
                        }
                    }
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    typename PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn&    PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn::operator= (const PatchableIteratorMinIn& rhs)
                    {
                        AssertNotNull (fPatchableContainer);
                        /*
                         *      If the fData field has not changed, then we can leave alone our iterator linkage.
                         *  Otherwise, we must remove ourselves from the old, and add ourselves to the new.
                         */
                        if (fPatchableContainer != &rhs->fPatchableContainer) {
                            /*
                             * Remove from old.
                             */
                            if (fPatchableContainer->fActiveIteratorsListHead == this) {
                                fPatchableContainer->fActiveIteratorsListHead = fNextActiveIterator;
                            }
                            else {
                                auto v = fPatchableContainer->fActiveIteratorsListHead;
                                for (; v->fNextActiveIterator != this; v = v->fNextActiveIterator) {
                                    AssertNotNull (v);
                                    AssertNotNull (v->fNextActiveIterator);
                                }
                                AssertNotNull (v);
                                Assert (v->fNextActiveIterator == this);
                                v->fNextActiveIterator = fNextActiveIterator;
                            }

                            /*
                             * Add to new.
                             */
                            fNextActiveIterator = rhs.fPatchableContainer->fActiveIteratorsListHead;
                            fPatchableContainer->fActiveIteratorsListHead = this;
                        }

                        fOwnerID = rhs.fOwnerID;
                        EnsureNotNull (fPatchableContainer);
                        return *this;
                    }


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_inl_ */

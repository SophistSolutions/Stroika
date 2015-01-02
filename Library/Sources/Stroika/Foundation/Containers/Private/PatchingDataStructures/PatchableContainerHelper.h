/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Memory/SmallStackBuffer.h"
#include    "../../../Traversal/Iterator.h"



/**
 * TODO:
 *      @todo   DOCUMENT THIS CLASS/LAYER MUCH BETTER
 *
 *              THIS CONTAINS PRIVATE IMPL DETAILS of Stroika container stuff. It is VERY much subject to change.
 *              It is just machinery for code sharing among my early concrete container implementations - so they
 *              are threadsafe, and can track running iterators to patch them as the underlying things they
 *              are iterating over change.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    using   Traversal::IteratorOwnerID;


                    /*
                    // Note
                    //  fPatchableContainer back point is a bit of a waste, in that PatchableIteratorMixIn is mixed in with
                    //  something else that has this. But the only other way to get the pointer when we need it is some
                    //  extra level of indirection someplace (virtuals) - whihc owuld have more overhead.
                    //  At least this seems OK for now.
                    //
                    // ROUGH FIRST DRAFT
                    //  @todo - GetNextActiveIterator - ....docs why template (couldt see ebtter way to mix this feature class in)
                    */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    struct  PatchableContainerHelper : public NON_PATCHED_DATA_STRUCTURE_CLASS {
                        using inherited = NON_PATCHED_DATA_STRUCTURE_CLASS;

                    public:
                        struct  PatchableIteratorMixIn;

                    public:
                        LOCKER   fLockSupport;

                    public:
                        PatchableIteratorMixIn*    fActiveIteratorsListHead =   nullptr;

                    public:
                        PatchableContainerHelper () = default;
                        PatchableContainerHelper (const PatchableContainerHelper&) = delete;

                    protected:
                        template    <typename COMBINED_ITERATOR>
                        PatchableContainerHelper (PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>* rhs, IteratorOwnerID newOwnerID, COMBINED_ITERATOR* fakePtrForOverload);
                    public:
                        ~PatchableContainerHelper ();
                    public:
                        nonvirtual  PatchableContainerHelper& operator= (const PatchableContainerHelper& rhs) = delete;

                    public:
                        template    <typename   PATCHABLE_ITERATOR_MIXIN_SUBTYPE>
                        nonvirtual  void    MoveIteratorAfterClone (PATCHABLE_ITERATOR_MIXIN_SUBTYPE* pi, PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>* fromContainer);

                    public:
                        nonvirtual  void    AddIterator (PatchableIteratorMixIn* pi);

                    public:
                        nonvirtual  void    RemoveIterator (PatchableIteratorMixIn* pi);

                    public:
                        template    <typename ACTUAL_ITERATOR_TYPE>
                        nonvirtual  ACTUAL_ITERATOR_TYPE*   GetFirstActiveIterator () const;

                    public:
                        nonvirtual void    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const;

                    public:
                        //  are there any iterators to be patched?
                        nonvirtual  bool    HasActiveIterators () const;

#if     qDebug
                    public:
                        virtual  void    AssertNoIteratorsReferenceOwner_ (IteratorOwnerID oBeingDeleted) const;
#endif
                    };


                    /*
                     *  When converting a non-patchable container backend (like DataStructures::Array) to a
                     *  PatchableDataStructues (like PatchableDataStructues::Array) you must mix something into
                     *  each associated iterator class (
                     *      eg. Array<T...>::_ArrayIteratorBase
                     *          : public DataStructures::Array<T, TRAITS>::_ArrayIteratorBase
                     *          , public PatchableContainerHelper<DataStructures::Array<T, TRAITS>, LOCKER>::PatchableIteratorMixIn
                     *
                     */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS, typename LOCKER>
                    struct  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS, LOCKER>::PatchableIteratorMixIn {
                        PatchableContainerHelper*   fPatchableContainer;
                        IteratorOwnerID             fOwnerID;
                        PatchableIteratorMixIn*     fNextActiveIterator;

                        PatchableIteratorMixIn (PatchableContainerHelper* containerHelper, IteratorOwnerID ownerID);
                        PatchableIteratorMixIn (const PatchableIteratorMixIn& from);
                        ~PatchableIteratorMixIn ();

                        template    <typename ACTUAL_ITERATOR_TYPE>
                        nonvirtual  ACTUAL_ITERATOR_TYPE*   GetNextActiveIterator () const;

                        nonvirtual  IteratorOwnerID GetOwner () const;
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "PatchableContainerHelper.inl"

#endif  /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_h_ */

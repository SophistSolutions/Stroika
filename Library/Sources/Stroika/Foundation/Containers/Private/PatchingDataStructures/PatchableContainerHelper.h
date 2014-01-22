/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Memory/SmallStackBuffer.h"
#include    "../../../Traversal/Iterator.h"



/**
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    using   Traversal::IteratorOwnerID;


                    /*
                    // Note
                    //  fPatchableContainer back point is a bit of a waste, in that PatchableIteratorMinIn is mixed in with
                    //  something else that has this. But the only other way to get the pointer when we need it is some
                    //  extra level of indirection someplace (virtuals) - whihc owuld have more overhead.
                    //  At least this seems OK for now.
                    //
                    // ROUGH FIRST DRAFT
                    //  @todo - GetNextActiveIterator - ....docs why template (couldt see ebtter way to mix this feature class in)
                    */
                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    struct  PatchableContainerHelper : public NON_PATCHED_DATA_STRUCTURE_CLASS {
                        using inherited = NON_PATCHED_DATA_STRUCTURE_CLASS;

                    public:
                        struct  PatchableIteratorMinIn;

                    public:
                        PatchableIteratorMinIn*    fActiveIteratorsListHead = nullptr;

                    public:
                        PatchableContainerHelper () = default;
                        PatchableContainerHelper (const PatchableContainerHelper&) = delete;

                    protected:
                        PatchableContainerHelper (PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>* rhs, IteratorOwnerID newOwnerID);
                    public:
                        ~PatchableContainerHelper ();
                    public:
                        nonvirtual  PatchableContainerHelper& operator= (const PatchableContainerHelper& rhs) = delete;

                    public:
                        template <typename ACTUAL_ITERATOR_TYPE>
                        nonvirtual  ACTUAL_ITERATOR_TYPE*   GetFirstActiveIterator () const;

                    public:
                        nonvirtual void    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted);

                    public:
                        //  are there any iterators to be patched?
                        nonvirtual  bool    HasActiveIterators () const;

#if     qDebug
                    public:
                        virtual  void    AssertNoIteratorsReferenceOwner_ (IteratorOwnerID oBeingDeleted);
#endif
                    };


                    template    <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    struct  PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMinIn {
                        PatchableContainerHelper*   fPatchableContainer;
                        IteratorOwnerID             fOwnerID;
                        PatchableIteratorMinIn*     fNextActiveIterator;

                        PatchableIteratorMinIn (PatchableContainerHelper* containerHelper, IteratorOwnerID ownerID);
                        PatchableIteratorMinIn (const PatchableIteratorMinIn& from);
                        nonvirtual  ~PatchableIteratorMinIn ();
                        nonvirtual  PatchableIteratorMinIn&    operator= (const PatchableIteratorMinIn& rhs);

                        template    <typename ACTUAL_ITERATOR_TYPE>
                        ACTUAL_ITERATOR_TYPE*   GetNextActiveIterator () const;

                        IteratorOwnerID GetOwner () const;
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

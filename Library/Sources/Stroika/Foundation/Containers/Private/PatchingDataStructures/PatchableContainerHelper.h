/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_h_

#include "../../../StroikaPreComp.h"

#include "../../../Memory/SmallStackBuffer.h"
#include "../../../Traversal/Iterator.h"

/**
 * TODO:
 *      @todo   DOCUMENT THIS CLASS/LAYER MUCH BETTER
 *
 *              THIS CONTAINS PRIVATE IMPL DETAILS of Stroika container stuff. It is VERY much subject to change.
 *              It is just machinery for code sharing among my early concrete container implementations - so they
 *              are threadsafe, and can track running iterators to patch them as the underlying things they
 *              are iterating over change.
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Private {
                namespace PatchingDataStructures {

                    using Traversal::IteratorOwnerID;

                    /*
                    // Note
                    //  fPatchableContainer_ back point is a bit of a waste, in that PatchableIteratorMixIn is mixed in with
                    //  something else that has this. But the only other way to get the pointer when we need it is some
                    //  extra level of indirection someplace (virtuals) - whihc owuld have more overhead.
                    //  At least this seems OK for now.
                    //
                    // ROUGH FIRST DRAFT
                    //  @todo - GetFirstActiveIterator_ - ....docs why template (couldt see ebtter way to mix this feature class in)
                    */
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    struct PatchableContainerHelper : public NON_PATCHED_DATA_STRUCTURE_CLASS {
                        using inherited = NON_PATCHED_DATA_STRUCTURE_CLASS;

                    public:
                        struct PatchableIteratorMixIn;

                    public:
                        PatchableContainerHelper ()                                = default;
                        PatchableContainerHelper (const PatchableContainerHelper&) = delete;

                    protected:
                        enum _CreateNewConstructorSelector { eCreateNewConstructorSelector };
                        template <typename... CONTAINER_EXTRA_ARGS>
                        PatchableContainerHelper (_CreateNewConstructorSelector, CONTAINER_EXTRA_ARGS&&... stdContainerArgs);
                        template <typename COMBINED_ITERATOR>
                        PatchableContainerHelper (PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>* rhs, IteratorOwnerID newOwnerID, COMBINED_ITERATOR* fakePtrForOverload);

                    public:
                        ~PatchableContainerHelper ();

                    public:
                        nonvirtual PatchableContainerHelper& operator= (const PatchableContainerHelper& rhs) = delete;

                    private:
                        template <typename PATCHABLE_ITERATOR_MIXIN_SUBTYPE>
                        nonvirtual void MoveIteratorAfterClone_ (PATCHABLE_ITERATOR_MIXIN_SUBTYPE* pi, PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>* fromContainer);

                    private:
                        nonvirtual void AddIterator_ (PatchableIteratorMixIn* pi);

                    private:
                        nonvirtual void RemoveIterator_ (PatchableIteratorMixIn* pi);

                    private:
                        template <typename ACTUAL_ITERATOR_TYPE>
                        nonvirtual ACTUAL_ITERATOR_TYPE* GetFirstActiveIterator_ () const;

                    public:
                        nonvirtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const;

                    public:
                        //  are there any iterators to be patched?
                        nonvirtual bool HasActiveIterators () const;

#if qDebug
                    public:
                        virtual void AssertNoIteratorsReferenceOwner_ (IteratorOwnerID oBeingDeleted) const;
#endif

                    protected:
                        /*
                         *  Apply the given function (with the argument type) to each iterator. The lock is only intended to be so the interation works,
                         *  but there is no obvious way to do tha twithout holding the lock during the call to 'f'. So the f's should be short!
                         */
                        template <typename ACTIVE_ITERATOR_SUBTYPE, typename FUNCTION>
                        nonvirtual void _ApplyToEachOwnedIterator (FUNCTION f) const;

                    private:
                        mutable std::mutex      fActiveIteratorsMutex_;
                        PatchableIteratorMixIn* fActiveIteratorsListHead_ = nullptr;
                    };

                    /*
                     *  When converting a non-patchable container backend (like DataStructures::Array) to a
                     *  PatchableDataStructues (like PatchableDataStructues::Array) you must mix something into
                     *  each associated iterator class (
                     *      eg. Array<T...>::_ArrayIteratorBase
                     *          : public DataStructures::Array<T, TRAITS>::_ArrayIteratorBase
                     *          , public PatchableContainerHelper<DataStructures::Array<T, TRAITS>>::PatchableIteratorMixIn
                     *
                     */
                    template <typename NON_PATCHED_DATA_STRUCTURE_CLASS>
                    struct PatchableContainerHelper<NON_PATCHED_DATA_STRUCTURE_CLASS>::PatchableIteratorMixIn {
                    private:
                        PatchableContainerHelper* fPatchableContainer_;
                        IteratorOwnerID           fOwnerID_;
                        PatchableIteratorMixIn*   fNextActiveIterator_;

                    protected:
                        PatchableIteratorMixIn (PatchableContainerHelper* containerHelper, IteratorOwnerID ownerID);
                        PatchableIteratorMixIn (const PatchableIteratorMixIn& from);
                        PatchableIteratorMixIn () = delete;

                    protected:
                        // no need for virtual DTOR since never called through ptr to PatchableIteratorMixIn and concrete subclass
                        // that effectively calls this base DTOR
                        ~PatchableIteratorMixIn ();

                    public:
                        nonvirtual PatchableIteratorMixIn& operator= (const PatchableIteratorMixIn&) = delete;

                    private:
                        template <typename ACTUAL_ITERATOR_TYPE>
                        nonvirtual ACTUAL_ITERATOR_TYPE* GetNextActiveIterator_ () const;

                    public:
                        nonvirtual IteratorOwnerID GetOwner () const;

                    public:
                        nonvirtual PatchableContainerHelper* GetPatchableContainerHelper () const;

                    private:
                        friend struct PatchableContainerHelper;
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
#include "PatchableContainerHelper.inl"

#endif /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_PatchableContainerHelper_h_ */

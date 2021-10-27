/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/LinkedList.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ****************** SortedCollection_LinkedList<T>::IImplRepBase_ ***************
     ********************************************************************************
     */
    template <typename T>
    class SortedCollection_LinkedList<T>::IImplRepBase_ : public SortedCollection<T>::_IRep {
    };

    /*
     ********************************************************************************
     *********************** SortedCollection_LinkedList<T>::Rep_ *******************
     ********************************************************************************
     */
    template <typename T>
    template <typename INORDER_COMPARER>
    class SortedCollection_LinkedList<T>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const INORDER_COMPARER& inorderComparer)
            : fInorderComparer_{inorderComparer}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fInorderComparer_{from->fInorderComparer_}
            , fData_{from->fData_}
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        [[NO_UNIQUE_ADDRESS_ATTR]] const INORDER_COMPARER fInorderComparer_;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            return Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
        }
        virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<T> (Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
        }
        virtual size_t GetLength () const override
        {
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.IsEmpty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using SHARED_REP_TYPE = Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_>;
            auto iLink            = fData_.FindFirstThat (doToElement);
            if (iLink == nullptr) {
                return Iterator<value_type>::GetEmptyIterator ();
            }
            Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
            return Iterator<value_type>{move (resultRep)};
        }

        // Collection<T>::_IRep overrides
    public:
        virtual _CollectionRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> (fInorderComparer_);
        }
        virtual _CollectionRepSharedPtr CloneAndPatchIterator (Iterator<T>* i, IteratorOwnerID obsoleteForIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            auto                                                      result = Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), obsoleteForIterableEnvelope);
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            AddWithoutLocks_ (item);
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue) override
        {
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // equals might examine a subset of the object and we still want to update the whole object, but
            // if its not already equal, the sort order could have changed so we must simulate with a remove/add
            if (Common::EqualsComparerAdapter{fInorderComparer_}(mir.fIterator.Current (), newValue)) {
                fData_.SetAt (mir.fIterator, newValue);
            }
            else {
                fData_.RemoveAt (mir.fIterator);
                AddWithoutLocks_ (newValue);
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            if (nextI != nullptr) {
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //      fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

        // SortedCollection<T>::_IRep overrides
    public:
        virtual InOrderComparerType GetInOrderComparer () const override
        {
            return InOrderComparerType{fInorderComparer_};
        }
        virtual bool Equals ([[maybe_unused]] const typename Collection<T>::_IRep& rhs) const override
        {
            AssertNotImplemented ();
            return false;
            //return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<value_type> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.Lookup (item, Common::EqualsComparerAdapter{fInorderComparer_}) != nullptr;
        }
        virtual void Remove (ArgByValueType<value_type> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.Remove (item, Common::EqualsComparerAdapter{fInorderComparer_});
        }

    private:
        nonvirtual void AddWithoutLocks_ (ArgByValueType<value_type> item)
        {
            using Traversal::kUnknownIteratorOwnerID;
            typename Rep_::DataStructureImplType_::ForwardIterator it{&fData_};
            // skip the smaller items
            while (it.More (nullptr, true) and fInorderComparer_ (it.Current (), item))
                ;
            // at this point - we are pointing at the first link >= item, so insert before it
            fData_.AddBefore (it, item);
        }

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<T>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ********************* SortedCollection_LinkedList<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList ()
        : SortedCollection_LinkedList{less<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename INORDER_COMPARER>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (const INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<INORDER_COMPARER>> (inorderComparer))
    {
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedCollection");
        AssertRepValidType_ ();
    }
    template <typename T>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (const T* start, const T* end)
        : SortedCollection_LinkedList{}
    {
        Require ((start == end) or (start != nullptr and end != nullptr));
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    SortedCollection_LinkedList<T>::SortedCollection_LinkedList (const SortedCollection<T>& src)
        : SortedCollection_LinkedList ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void SortedCollection_LinkedList<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_ */

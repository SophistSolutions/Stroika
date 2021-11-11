/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <set>

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"
#include "../STL/Compare.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********************** SortedSet_stdset<T>::IImplRepBase_ *********************
     ********************************************************************************
     */
    template <typename T>
    class SortedSet_stdset<T>::IImplRepBase_ : public SortedSet<T>::_IRep {
    };

    /*
     ********************************************************************************
     *************************** SortedSet_stdset<T>::Rep_ **************************
     ********************************************************************************
     */
    template <typename T>
    template <typename INORDER_COMPARER>
    class SortedSet_stdset<T>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Invariant ();
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Invariant ();
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_FindFirstThat (doToElement);
        }

        // Set<T>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return ElementEqualityComparerType{Common::EqualsComparerAdapter{fData_.key_comp ()}};
        }
        virtual _SetSharedPtrIRep CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data in clone
        }
        virtual _SetSharedPtrIRep CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename Iterable<value_type>::_IRep& rhs) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<value_type> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.Contains (item);
        }
        virtual optional<value_type> Lookup (ArgByValueType<value_type> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        i = fData_.find (item);
            return (i == fData_.end ()) ? optional<value_type>{} : optional<value_type>{*i};
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.insert (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (ArgByValueType<value_type> item) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.Invariant ();
            auto i = fData_.find (item);
            if (i != fData_.end ()) {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto&                                                 mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            auto nextIResult = fData_.erase (mir.fIterator.GetCurrentSTLIterator ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                auto resultRep = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_);
                resultRep->fIterator.SetCurrentSTLIterator (nextIResult);
                *nextI = Iterator<value_type>{move (resultRep)};
            }
        }

        // SortedSet<T>::_IRep overrides
    public:
        virtual ElementInOrderComparerType GetInOrderComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return ElementInOrderComparerType{fData_.key_comp ()};
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<set<value_type, INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ****************************** SortedSet_stdset<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    inline SortedSet_stdset<T>::SortedSet_stdset ()
        : SortedSet_stdset{less<value_type>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename INORDER_COMPARER>
    inline SortedSet_stdset<T>::SortedSet_stdset (const INORDER_COMPARER& inorderComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<INORDER_COMPARER>> (inorderComparer))
    {
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedSet");
        AssertRepValidType_ ();
    }
    template <typename T>
    SortedSet_stdset<T>::SortedSet_stdset (const initializer_list<value_type>& src)
        : SortedSet_stdset{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    SortedSet_stdset<T>::SortedSet_stdset (const ElementInOrderComparerType& inOrderComparer, const initializer_list<value_type>& src)
        : SortedSet_stdset (inOrderComparer)
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Set<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    SortedSet_stdset<T>::SortedSet_stdset (CONTAINER_OF_ADDABLE&& src)
        : SortedSet_stdset{}
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    SortedSet_stdset<T>::SortedSet_stdset (const ElementInOrderComparerType& inOrderComparer, CONTAINER_OF_ADDABLE&& src)
        : SortedSet_stdset (inOrderComparer)
    {
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline SortedSet_stdset<T>::SortedSet_stdset (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : SortedSet_stdset{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline SortedSet_stdset<T>::SortedSet_stdset (const ElementInOrderComparerType& inOrderComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : SortedSet_stdset (inOrderComparer)
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void SortedSet_stdset<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_inl_ */

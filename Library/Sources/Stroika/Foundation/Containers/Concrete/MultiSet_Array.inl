/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_Array_inl_
#define _Stroika_Foundation_Containers_MultiSet_Array_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/Array.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ********************* MultiSet_Array<T, TRAITS>::IImplRepBase_ *****************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    class MultiSet_Array<T, TRAITS>::IImplRepBase_ : public MultiSet<T, TRAITS>::_IRep {
    public:
        virtual size_t capacity () const             = 0;
        virtual void   reserve (size_t slotsAlloced) = 0;
        virtual void   shrink_to_fit ()              = 0;
    };

    /*
     ********************************************************************************
     ************************** MultiSet_Array<T, TRAITS>::Rep_ *********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER>
    class MultiSet_Array<T, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        static_assert (not is_reference_v<EQUALS_COMPARER>);

    public:
        Rep_ (const EQUALS_COMPARER& equalsComparer)
            : fEqualsComparer_{equalsComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        [[NO_UNIQUE_ADDRESS_ATTR]] const EQUALS_COMPARER fEqualsComparer_;

        // Iterable<T>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return ElementEqualityComparerType{fEqualsComparer_};
        }
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual size_t size () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.size () == 0;
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            size_t                                                      i = fData_.Find (that);
            if (i == fData_.size ()) {
                return nullptr;
            }
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, i)};
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v) const override
        {
            return this->_Find_equal_to_default_implementation (v);
        }

        // MultiSet<T, TRAITS>::_IRep overrides
    public:
        virtual _MultiSetRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fEqualsComparer_); // keep same comparer, but no data
        }
        virtual _MultiSetRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<T> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            value_type                                                  tmp{item};
            return Find_ (tmp) != kNotFound_;
        }
        virtual void Add (ArgByValueType<T> item, CounterType count) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            value_type                                            tmp{item, count};
            size_t                                                index = Find_ (tmp);
            if (index == kNotFound_) {
                fData_.InsertAt (fData_.size (), tmp);
            }
            else {
                tmp.fCount += count;
                fData_.SetAt (index, tmp);
            }
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t RemoveIf (ArgByValueType<T> item, CounterType count) override
        {
            Require (count > 0);
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            value_type                                            tmp (item);
            size_t                                                index = Find_ (tmp);
            if (index != kNotFound_) {
                Assert (index < fData_.size ());
                size_t result; // intentionally uninitialized
                if (tmp.fCount > count) {
                    tmp.fCount -= count;
                    Assert (tmp.fCount > 0);
                    fData_.SetAt (index, tmp);
                    result = count;
                }
                else {
                    fData_.RemoveAt (index);
                    result = tmp.fCount;
                }
                fChangeCounts_.PerformedChange ();
                return result;
            }
            return 0;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            optional<size_t>                                      savedUnderlyingIndex;
            static_assert (is_same_v<size_t, typename DataStructureImplType_::UnderlyingIteratorRep>); // else must do slightly differently
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }
        virtual void UpdateCount (const Iterator<value_type>& i, CounterType newCount, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto&                                                 mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (newCount == 0) {
                if (nextI != nullptr) {
                    *nextI = i;
                    ++(*nextI);
                }
                fData_.RemoveAt (mir.fIterator);
            }
            else {
                value_type c = mir.fIterator.Current ();
                c.fCount     = newCount;
                fData_.SetAt (mir.fIterator, c);
                if (nextI != nullptr) {
                    *nextI = i;
                }
            }
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual CounterType OccurrencesOf (ArgByValueType<T> item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            value_type                                                  tmp (item);
            size_t                                                      index = Find_ (tmp);
            if (index == kNotFound_) {
                return 0;
            }
            Assert (index >= 0);
            Assert (index < fData_.size ());
            return fData_[index].fCount;
        }
        virtual Iterable<T> Elements (const typename MultiSet<T, TRAITS>::_IRepSharedPtr& rep) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Elements_Reference_Implementation (rep);
        }
        virtual Iterable<T> UniqueElements (const typename MultiSet<T, TRAITS>::_IRepSharedPtr& rep) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_UniqueElements_Reference_Implementation (rep);
        }

        // MultiSet_Array<T, TRAITS>::_IRep overrides
    public:
        virtual size_t capacity () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.capacity ();
        }
        virtual void reserve (size_t slotsAlloced) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.reserve (slotsAlloced);
            fChangeCounts_.PerformedChange ();
        }
        virtual void shrink_to_fit () override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> critSec{fData_};
            fData_.shrink_to_fit ();
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        static constexpr size_t kNotFound_ = (size_t)-1;

    private:
        nonvirtual size_t Find_ (value_type& item) const
        {
            // this code assumes locking done by callers
            size_t length = fData_.size ();
            for (size_t i = 0; i < length; ++i) {
                if (fEqualsComparer_ (fData_.GetAt (i).fValue, item.fValue)) {
                    item = fData_.GetAt (i);
                    return i;
                }
            }
            return kNotFound_;
        }

    private:
        friend class MultiSet_Array<T, TRAITS>;
    };

    /*
     ********************************************************************************
     ***************************** MultiSet_Array<T, TRAITS> ************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    MultiSet_Array<T, TRAITS>::MultiSet_Array ()
        : MultiSet_Array{equal_to<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>*>
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer)
        : inherited{inherited::template MakeSmartPtr<Rep_<Configuration::remove_cvref_t<EQUALS_COMPARER>>> (forward<EQUALS_COMPARER> (equalsComparer))}
    {
        static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "MultiSet_Array constructor with EQUALS_COMPARER - comparer not valid EqualsComparer- see ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool(T, T)>");
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<MultiSet_Array<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array (ITERABLE_OF_ADDABLE&& src)
        : MultiSet_Array{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        if constexpr (Configuration::has_size_v<ITERABLE_OF_ADDABLE>) {
            reserve (src.size ());
        }
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src)
        : MultiSet_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        reserve (src.size ());
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<T>& src)
        : MultiSet_Array{}
    {
        reserve (src.size ());
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>*>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src)
        : MultiSet_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        reserve (src.size ());
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<value_type>& src)
        : MultiSet_Array{}
    {
        reserve (src.size ());
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>*>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src)
        : MultiSet_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        reserve (src.size ());
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : MultiSet_Array{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        if constexpr (Configuration::has_minus_v<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                reserve (end - start);
            }
        }
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : MultiSet_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        if constexpr (Configuration::has_minus_v<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                reserve (end - start);
            }
        }
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline size_t MultiSet_Array<T, TRAITS>::capacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<IImplRepBase_>;
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().capacity ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::reserve (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().reserve (slotsAlloced);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().shrink_to_fit ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_MultiSet_Array_inl_ */

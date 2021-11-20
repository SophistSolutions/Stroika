/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        virtual size_t GetCapacity () const              = 0;
        virtual void   SetCapacity (size_t slotsAlloced) = 0;
        virtual void   shrink_to_fit ()                  = 0;
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
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.GetLength () == 0;
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
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            size_t                                                      i = fData_.FindFirstThat (doToElement);
            if (i == fData_.GetLength ()) {
                return nullptr;
            }
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, i)};
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
                fData_.InsertAt (fData_.GetLength (), tmp);
            }
            else {
                tmp.fCount += count;
                fData_.SetAt (index, tmp);
            }
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (ArgByValueType<T> item, CounterType count) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            value_type                                            tmp (item);
            size_t                                                index = Find_ (tmp);
            if (index != kNotFound_) {
                Assert (index < fData_.GetLength ());
                Assert (tmp.fCount >= count);
                tmp.fCount -= count;
                if (tmp.fCount == 0) {
                    fData_.RemoveAt (index);
                }
                else {
                    fData_.SetAt (index, tmp);
                }
                fChangeCounts_.PerformedChange ();
            }
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
            Assert (index < fData_.GetLength ());
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
        virtual size_t GetCapacity () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.GetCapacity ();
        }
        virtual void SetCapacity (size_t slotsAlloced) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.SetCapacity (slotsAlloced);
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
            size_t length = fData_.GetLength ();
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
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array ()
        : inherited{equal_to<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>*>
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array (const EQUALS_COMPARER& equalsComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<EQUALS_COMPARER>> (equalsComparer))
    {
        static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "Equals comparer required with MultiSet_Array");
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <typename COPY_FROM_ITERATOR>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
        : MultiSet_Array{}
    {
        SetCapacity (end - start);
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array (const MultiSet<T, TRAITS>& src)
        : MultiSet_Array{}
    {
        SetCapacity (src.GetLength ());
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<T>& src)
        : MultiSet_Array{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<value_type>& src)
        : MultiSet_Array{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline size_t MultiSet_Array<T, TRAITS>::GetCapacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<IImplRepBase_>;
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().GetCapacity ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::SetCapacity (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().SetCapacity (slotsAlloced);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().shrink_to_fit ();
    }
    template <typename T, typename TRAITS>
    inline size_t MultiSet_Array<T, TRAITS>::capacity () const
    {
        return GetCapacity ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::reserve (size_t slotsAlloced)
    {
        SetCapacity (slotsAlloced);
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

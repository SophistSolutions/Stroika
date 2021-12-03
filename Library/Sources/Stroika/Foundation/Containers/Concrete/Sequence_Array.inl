/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"
#include "../Common.h"

#include "../DataStructures/Array.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /**
     */
    template <typename T>
    class Sequence_Array<T>::IImplRep_ : public Sequence<T>::_IRep {
    public:
        virtual void   shrink_to_fit ()                  = 0;
        virtual size_t GetCapacity () const              = 0;
        virtual void   SetCapacity (size_t slotsAlloced) = 0;
    };

    /**
     */
    template <typename T>
    class Sequence_Array<T>::Rep_ : public Sequence_Array<T>::IImplRep_, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Sequence_Array<T>::IImplRep_;

    protected:
        static constexpr size_t _kSentinalLastItemIndex = inherited::_kSentinalLastItemIndex;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<T>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<T> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
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
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            size_t                                                      i = fData_.Find (that);
            if (i == fData_.GetLength ()) {
                return nullptr;
            }
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, i)};
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v) const override
        {
            return this->_Find_equal_to_default_implementation (v);
        }

        // Sequence<T>::_IRep overrides
    public:
        virtual _SequenceRepSharedPtr CloneEmpty () const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> ();
        }
        virtual _SequenceRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            return result;
        }
        virtual value_type GetAt (size_t i) const override
        {
            Require (not IsEmpty ());
            Require (i == _kSentinalLastItemIndex or i < GetLength ());
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            if (i == _kSentinalLastItemIndex) {
                i = fData_.GetLength () - 1;
            }
            return fData_.GetAt (i);
        }
        virtual void SetAt (size_t i, ArgByValueType<value_type> item) override
        {
            Require (i < GetLength ());
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.SetAt (i, item);
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t IndexOf (const Iterator<value_type>& i) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto&                                                       mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            return mir.fIterator.CurrentIndex ();
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
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            optional<size_t>                                      savedUnderlyingIndex;
            static_assert (is_same_v<size_t, typename DataStructureImplType_::UnderlyingIteratorRep>); // else must do slightly differently
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.CurrentIndex ();
            }
            fData_.SetAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator, newValue);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }
        virtual void Insert (size_t at, const value_type* from, const value_type* to) override
        {
            Require (at == _kSentinalLastItemIndex or at <= GetLength ());
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            if (at == _kSentinalLastItemIndex) {
                at = fData_.GetLength ();
            }
            // quickie poor impl
            // @todo use                        ReserveSpeedTweekAddN (fData_, (to - from));
            // when we fix names
            {
                size_t curLen = fData_.GetLength ();
                size_t curCap = fData_.GetCapacity ();
                size_t newLen = curLen + (to - from);
                if (newLen > curCap) {
                    newLen *= 6;
                    newLen /= 5;
                    if constexpr (sizeof (T) < 100) {
                        newLen = Stroika::Foundation::Math::RoundUpTo (newLen, static_cast<size_t> (64)); //?
                    }
                    fData_.SetCapacity (newLen);
                }
            }
#if 0
            size_t  desiredCapacity     =   fData_.GetLength () + (to - from);
            desiredCapacity = max (desiredCapacity, fData_.GetCapacity ());
            fData_.SetCapacity (desiredCapacity);
#endif
            for (auto i = from; i != to; ++i) {
                fData_.InsertAt (at++, *i);
            }
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (size_t from, size_t to) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            // quickie poor impl
            for (size_t i = from; i < to; ++i) {
                fData_.RemoveAt (from);
            }
            fChangeCounts_.PerformedChange ();
        }

        // Sequence_Array<T>::IImplRep_
    public:
        virtual void shrink_to_fit () override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.shrink_to_fit ();
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t GetCapacity () const override
        {
            return fData_.GetCapacity ();
        }
        virtual void SetCapacity (size_t slotsAlloced) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.SetCapacity (slotsAlloced);
            fChangeCounts_.PerformedChange ();
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ****************************** Sequence_Array<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    inline Sequence_Array<T>::Sequence_Array ()
        : inherited{inherited::template MakeSmartPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Sequence_Array<T>::Sequence_Array (const initializer_list<value_type>& src)
        : Sequence_Array{}
    {
        this->AppendAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Sequence_Array<T>::Sequence_Array (const vector<value_type>& src)
        : Sequence_Array{}
    {
        this->AppendAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Sequence_Array<T>, decay_t<CONTAINER_OF_ADDABLE>>>*>
    Sequence_Array<T>::Sequence_Array (CONTAINER_OF_ADDABLE&& src)
        : Sequence_Array{}
    {
        this->AppendAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Sequence_Array<T>::Sequence_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Sequence_Array{}
    {
        this->AppendAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Sequence_Array<T>::shrink_to_fit ()
    {
        typename inherited::template _SafeReadWriteRepAccessor<IImplRep_>{this}._GetWriteableRep ().shrink_to_fit ();
    }
    template <typename T>
    inline size_t Sequence_Array<T>::GetCapacity () const
    {
        return typename inherited::template _SafeReadRepAccessor<IImplRep_>{this}._ConstGetRep ().GetCapacity ();
    }
    template <typename T>
    inline void Sequence_Array<T>::SetCapacity (size_t slotsAlloced)
    {
        typename inherited::template _SafeReadWriteRepAccessor<IImplRep_>{this}._GetWriteableRep ().SetCapacity (slotsAlloced);
    }
    template <typename T>
    inline size_t Sequence_Array<T>::capacity () const
    {
        return GetCapacity ();
    }
    template <typename T>
    inline void Sequence_Array<T>::reserve (size_t slotsAlloced)
    {
        SetCapacity (slotsAlloced);
    }
    template <typename T>
    inline void Sequence_Array<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_ */

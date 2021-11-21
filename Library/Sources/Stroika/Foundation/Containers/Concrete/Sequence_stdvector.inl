/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <vector>

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class Sequence_stdvector<T>::Rep_ : public Sequence<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Sequence<T>::_IRep;

    protected:
        static constexpr size_t _kSentinalLastItemIndex = inherited::_kSentinalLastItemIndex;

    public:
        Rep_ ()                = default;
        Rep_ (const Rep_& src) = default;
        Rep_ (vector<T>&& src)
            : fData_{move (src)}
        {
        }

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
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        iLink = const_cast<DataStructureImplType_&> (fData_).FindFirstThat (doToElement);
            if (iLink == fData_.end ()) {
                return nullptr;
            }
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
        }

        // Sequence<T>::_IRep overrides
    public:
        virtual _SequenceRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> ();
        }
        virtual _SequenceRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            result->fData_.MoveIteratorHereAfterClone (&Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ()).fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual value_type GetAt (size_t i) const override
        {
            Require (not IsEmpty ());
            Require (i == _kSentinalLastItemIndex or i < GetLength ());
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            if (i == _kSentinalLastItemIndex) {
                i = fData_.size () - 1;
            }
            return fData_[i];
        }
        virtual void SetAt (size_t i, ArgByValueType<value_type> item) override
        {
            Require (i < GetLength ());
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_[i] = item;
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t IndexOf (const Iterator<value_type>& i) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.CurrentIndex ();
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            auto&                                                 mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            auto newI = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, newI)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex>            writeLock{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            fData_.Invariant ();
            *fData_.remove_constness (mir.fIterator.GetUnderlyingIteratorRep ()) = newValue;
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
            fData_.Invariant ();
        }
        virtual void Insert (size_t at, const value_type* from, const value_type* to) override
        {
            Require (at == _kSentinalLastItemIndex or at <= GetLength ());
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            if (at == _kSentinalLastItemIndex) {
                at = fData_.size ();
            }
            fData_.insert (fData_.begin () + at, from, to);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (size_t from, size_t to) override
        {
            Require ((from <= to) and (to <= this->GetLength ()));
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.erase (fData_.begin () + from, fData_.begin () + to);
            fChangeCounts_.PerformedChange ();
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<vector<value_type>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ******************************* Sequence_stdvector<T> **************************
     ********************************************************************************
     */
    template <typename T>
    inline Sequence_stdvector<T>::Sequence_stdvector ()
        : inherited{inherited::template MakeSmartPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Sequence_stdvector<T>::Sequence_stdvector (vector<value_type>&& src)
        : inherited{inherited::template MakeSmartPtr<Rep_> (move (src))}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Sequence_stdvector<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Sequence_stdvector<T>::Sequence_stdvector (CONTAINER_OF_ADDABLE&& src)
        : Sequence_stdvector{}
    {
        this->AppendAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Sequence_stdvector<T>::Sequence_stdvector (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Sequence_stdvector{}
    {
        this->AppendAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline auto Sequence_stdvector<T>::operator= (const Sequence_stdvector& rhs) -> Sequence_stdvector&
    {
        AssertRepValidType_ ();
        inherited::operator= (rhs);
        AssertRepValidType_ ();
        return *this;
    }
    template <typename T>
    inline void Sequence_stdvector<T>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                                   accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedMutex> lg (accessor._ConstGetRep ().fData_);
        accessor._GetWriteableRep ().fData_.shrink_to_fit ();
        accessor._GetWriteableRep ().fChangeCounts_.PerformedChange ();
    }
    template <typename T>
    inline size_t Sequence_stdvector<T>::GetCapacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_>;
        _SafeReadRepAccessor                                        accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{accessor._ConstGetRep ().fData_};
        return accessor._ConstGetRep ().fData_.capacity ();
    }
    template <typename T>
    void Sequence_stdvector<T>::SetCapacity (size_t slotsAlloced)
    {
        Require (slotsAlloced >= this->GetLength ());
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                                  accessor{this};
        lock_guard<const Debug::AssertExternallySynchronizedMutex> writeLock{accessor._ConstGetRep ().fData_};
        if (accessor._ConstGetRep ().fData_.capacity () < slotsAlloced) {
            accessor._GetWriteableRep ().fData_.reserve (slotsAlloced);
            accessor._GetWriteableRep ().fChangeCounts_.PerformedChange ();
        }
        else if (accessor._ConstGetRep ().fData_.capacity () > slotsAlloced) {
            accessor._GetWriteableRep ().fData_.shrink_to_fit ();
            accessor._GetWriteableRep ().fData_.reserve (slotsAlloced);
            accessor._GetWriteableRep ().fChangeCounts_.PerformedChange ();
        }
    }
    template <typename T>
    inline size_t Sequence_stdvector<T>::capacity () const
    {
        return GetCapacity ();
    }
    template <typename T>
    inline void Sequence_stdvector<T>::reserve (size_t slotsAlloced)
    {
        SetCapacity (slotsAlloced);
    }
    template <typename T>
    inline void Sequence_stdvector<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_ */

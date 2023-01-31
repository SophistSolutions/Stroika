/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
#include "../Support/ReserveTweaks.h"

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
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator ([[maybe_unused]] const _IterableRepSharedPtr& thisSharedPtr) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find ([[maybe_unused]] const _IterableRepSharedPtr&           thisSharedPtr,
                                           const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  iLink = const_cast<DataStructureImplType_&> (fData_).Find (that);
            if (iLink == fData_.end ()) {
                return nullptr;
            }
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
        }
        virtual Iterator<value_type> Find_equal_to (const _IterableRepSharedPtr& thisSharedPtr, const ArgByValueType<value_type>& v) const override
        {
            return this->_Find_equal_to_default_implementation (thisSharedPtr, v);
        }

        // Sequence<T>::_IRep overrides
    public:
        virtual _SequenceRepSharedPtr CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> ();
        }
        virtual _SequenceRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            result->fData_.MoveIteratorHereAfterClone (&Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ()).fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual value_type GetAt (size_t i) const override
        {
            Require (not empty ());
            Require (i == _kSentinalLastItemIndex or i < size ());
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (i == _kSentinalLastItemIndex) {
                i = fData_.size () - 1;
            }
            return fData_[i];
        }
        virtual void SetAt (size_t i, ArgByValueType<value_type> item) override
        {
            Require (i < size ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_[i] = item;
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t IndexOf (const Iterator<value_type>& i) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.CurrentIndex ();
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            auto newI = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, newI)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext           declareWriteContext{fData_};
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
            Require (at == _kSentinalLastItemIndex or at <= size ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (at == _kSentinalLastItemIndex) {
                at = fData_.size ();
            }
            Support::ReserveTweaks::Reserve4AddN (fData_, to - from);
            fData_.insert (fData_.begin () + at, from, to);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (size_t from, size_t to) override
        {
            Require ((from <= to) and (to <= this->size ()));
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.erase (fData_.begin () + from, fData_.begin () + to);
            fChangeCounts_.PerformedChange ();
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<vector<value_type>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        friend class Sequence_stdvector;
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
    inline Sequence_stdvector<T>::Sequence_stdvector (const initializer_list<value_type>& src)
        : Sequence_stdvector{}
    {
        reserve (src.size ());
        this->AppendAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Sequence_stdvector<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Sequence_stdvector<T>::Sequence_stdvector (ITERABLE_OF_ADDABLE&& src)
        : Sequence_stdvector{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        if constexpr (Configuration::has_size_v<ITERABLE_OF_ADDABLE>) {
            reserve (src.size ());
        }
        this->AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Sequence_stdvector<T>::Sequence_stdvector (std::vector<T>&& src)
        : inherited{inherited::template MakeSmartPtr<Rep_> (move (src))}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline Sequence_stdvector<T>::Sequence_stdvector (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Sequence_stdvector{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        if constexpr (Configuration::has_minus_v<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                reserve (end - start);
            }
        }
        this->AppendAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Sequence_stdvector<T>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                             accessor{this};
        Debug::AssertExternallySynchronizedMutex::ReadContext lg{accessor._ConstGetRep ().fData_};
        accessor._GetWriteableRep ().fData_.shrink_to_fit ();
        accessor._GetWriteableRep ().fChangeCounts_.PerformedChange ();
    }
    template <typename T>
    inline size_t Sequence_stdvector<T>::capacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_>;
        _SafeReadRepAccessor                                  accessor{this};
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{accessor._ConstGetRep ().fData_};
        return accessor._ConstGetRep ().fData_.capacity ();
    }
    template <typename T>
    void Sequence_stdvector<T>::reserve (size_t slotsAlloced)
    {
        Require (slotsAlloced >= this->size ());
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                              accessor{this};
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{accessor._GetWriteableRep ().fData_};
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
    inline void Sequence_stdvector<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_ */

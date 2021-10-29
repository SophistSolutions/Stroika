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
        Rep_ (const Rep_& src) = delete;
        Rep_ (vector<T>&& src)
            : fData_ (move (src))
        {
        }
        Rep_ (const Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fData_{from->fData_}
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> (this, forIterableEnvelope);
        }
        virtual Iterator<value_type> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &fData_)};
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.empty ();
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
            auto                                                       iLink = const_cast<DataStructureImplType_&> (fData_).FindFirstThat (doToElement);
            if (iLink == fData_.end ()) {
                return nullptr;
            }
            Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_> resultRep = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            return Iterator<value_type>{move (resultRep)};
        }

        // Sequence<T>::_IRep overrides
    public:
        virtual _SequenceRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<value_type>::template MakeSmartPtr<Rep_> ();
        }
        virtual _SequenceRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i, IteratorOwnerID obsoleteForIterableEnvelope) const override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                      result = Iterable<value_type>::template MakeSmartPtr<Rep_> (this, obsoleteForIterableEnvelope);
            auto&                                                     mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual value_type GetAt (size_t i) const override
        {
            Require (not IsEmpty ());
            Require (i == _kSentinalLastItemIndex or i < GetLength ());
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (i == _kSentinalLastItemIndex) {
                i = fData_.size () - 1;
            }
            return fData_[i];
        }
        virtual void SetAt (size_t i, ArgByValueType<value_type> item) override
        {
            Require (i < GetLength ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_[i] = item;
        }
        virtual size_t IndexOf (const Iterator<value_type>& i) const override
        {
            auto&                                                      mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return mir.fIterator.CurrentIndex ();
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.fData == &fData_);
            auto newI = fData_.erase (mir.fIterator.fStdIterator);
            if (nextI != nullptr) {
                IteratorOwnerID suggestedOwner = nullptr; //tmphack
                auto            resultRep      = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &this->fData_);
                resultRep->fIterator.SetCurrentLink (newI);
                *nextI = Iterator<value_type>{move (resultRep)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            fData_.Invariant ();
            *mir.fIterator.fStdIterator = newValue;
            fData_.Invariant ();
        }
        virtual void Insert (size_t at, const value_type* from, const value_type* to) override
        {
            Require (at == _kSentinalLastItemIndex or at <= GetLength ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (at == _kSentinalLastItemIndex) {
                at = fData_.size ();
            }
            fData_.insert (fData_.begin () + at, from, to);
        }
        virtual void Remove (size_t from, size_t to) override
        {
            // quickie poor impl (patch once, not multiple times...)
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (size_t i = from; i < to; ++i) {
                fData_.erase (fData_.begin () + from);
            }
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //    fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<vector<value_type>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
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
        : inherited (inherited::template MakeSmartPtr<Rep_> (move (src)))
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
    inline Sequence_stdvector<T>& Sequence_stdvector<T>::operator= (const Sequence_stdvector& rhs)
    {
        AssertRepValidType_ ();
        inherited::operator= (rhs);
        AssertRepValidType_ ();
        return *this;
    }
    template <typename T>
    inline void Sequence_stdvector<T>::Compact ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                                  accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedLock> lg (accessor._ConstGetRep ().fData_);
        if (accessor._ConstGetRep ().fData_.capacity () != accessor._ConstGetRep ().fData_.size ()) {
            Memory::SmallStackBuffer<size_t> patchOffsets;
            accessor._GetWriteableRep ().fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (&patchOffsets);
            accessor._GetWriteableRep ().fData_.reserve (accessor._ConstGetRep ().fData_.size ());
            if (patchOffsets.GetSize () != 0) {
                accessor._GetWriteableRep ().fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (patchOffsets);
            }
        }
    }
    template <typename T>
    inline size_t Sequence_stdvector<T>::GetCapacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_>;
        _SafeReadRepAccessor                                       accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{accessor._ConstGetRep ().fData_};
        return accessor._ConstGetRep ().fData_.capacity ();
    }
    template <typename T>
    inline void Sequence_stdvector<T>::SetCapacity (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                                  accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedLock> lg (accessor._ConstGetRep ().fData_);
        if (accessor._ConstGetRep ().fData_.capacity () != slotsAlloced) {
            Memory::SmallStackBuffer<size_t> patchOffsets;
            accessor._GetWriteableRep ().fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (&patchOffsets);
            accessor._GetWriteableRep ().fData_.reserve (slotsAlloced);
            if (patchOffsets.GetSize () != 0) {
                accessor._GetWriteableRep ().fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (patchOffsets);
            }
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

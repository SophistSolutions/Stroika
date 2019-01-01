/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <vector>

#include "../../Memory/BlockAllocated.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/STLContainerWrapper.h"

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class Sequence_stdvector<T>::Rep_ : public Sequence<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Sequence<T>::_IRep;

    public:
        using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
        using _SequenceRepSharedPtr = typename inherited::_SequenceRepSharedPtr;
        using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;

    protected:
        static constexpr size_t _kSentinalLastItemIndex = inherited::_kSentinalLastItemIndex;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
            : inherited ()
            , fData_ (&from->fData_, forIterableEnvelope)
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

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
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.empty ();
        }
        virtual void Apply (_APPLY_ARGTYPE doToElement) const override
        {
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using RESULT_TYPE     = Iterator<T>;
            using SHARED_REP_TYPE = Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_>;
            auto iLink            = const_cast<DataStructureImplType_&> (fData_).FindFirstThat (doToElement);
            if (iLink == fData_.end ()) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            // const cast needed because STLContainerWrapper needs a non-const iterator
            Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
            return RESULT_TYPE (move (resultRep));
        }

        // Sequence<T>::_IRep overrides
    public:
        virtual _SequenceRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
        {
            if (fData_.HasActiveIterators ()) {
                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                auto r = Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                r->fData_.clear_WithPatching ();
                return r;
            }
            else {
                return Iterable<T>::template MakeSmartPtr<Rep_> ();
            }
        }
        virtual T GetAt (size_t i) const override
        {
            Require (not IsEmpty ());
            Require (i == _kSentinalLastItemIndex or i < GetLength ());
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (i == _kSentinalLastItemIndex) {
                i = fData_.size () - 1;
            }
            return fData_[i];
        }
        virtual void SetAt (size_t i, ArgByValueType<T> item) override
        {
            Require (i < GetLength ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_[i] = item;
        }
        virtual size_t IndexOf (const Iterator<T>& i) const override
        {
            const typename Iterator<T>::IRep& ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto&                                                      mir = dynamic_cast<const IteratorRep_&> (ir);
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return mir.fIterator.CurrentIndex ();
        }
        virtual void Remove (const Iterator<T>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const typename Iterator<T>::IRep&                         ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            mir.fIterator.RemoveCurrent ();
        }
        virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const typename Iterator<T>::IRep&                         ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            fData_.Invariant ();
            *mir.fIterator.fStdIterator = newValue;
            fData_.Invariant ();
        }
        virtual void Insert (size_t at, const T* from, const T* to) override
        {
            Require (at == _kSentinalLastItemIndex or at <= GetLength ());
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (at == _kSentinalLastItemIndex) {
                at = fData_.size ();
            }
            // quickie poor impl. Could do save / patch once, not multiple times...
            {
                size_t capacity{ReserveSpeedTweekAddNCapacity (fData_, to - from)};
                if (capacity != static_cast<size_t> (-1)) {
                    Memory::SmallStackBuffer<size_t> patchOffsets (0);
                    fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (&patchOffsets);
                    fData_.reserve (capacity);
                    if (patchOffsets.GetSize () != 0) {
                        fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (patchOffsets);
                    }
                }
            }
            for (auto i = from; i != to; ++i) {
                fData_.insert_toVector_WithPatching (fData_.begin () + at, *i);
                at++;
            }
        }
        virtual void Remove (size_t from, size_t to) override
        {
            // quickie poor impl (patch once, not multiple times...)
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (size_t i = from; i < to; ++i) {
                fData_.erase_WithPatching (fData_.begin () + from);
            }
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<vector<T>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

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
        : inherited (inherited::template MakeSmartPtr<Rep_> ())
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Sequence_stdvector<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Sequence_stdvector<T>::Sequence_stdvector (CONTAINER_OF_ADDABLE&& src)
        : Sequence_stdvector ()
    {
        this->AppendAll (forward<CONTAINER_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Sequence_stdvector<T>::Sequence_stdvector (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Sequence_stdvector ()
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

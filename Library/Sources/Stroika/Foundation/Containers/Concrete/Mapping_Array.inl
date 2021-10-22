/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/Array.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********** Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ **********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    };

    /*
     ********************************************************************************
     ************ Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ ******************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER>
    class Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        using _IterableRepSharedPtr        = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IterableRepSharedPtr;
        using _MappingRepSharedPtr         = typename inherited::_IRepSharedPtr;
        using _APPLY_ARGTYPE               = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE          = typename inherited::_APPLYUNTIL_ARGTYPE;
        using KeyEqualsCompareFunctionType = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::KeyEqualsCompareFunctionType;

    public:
        Rep_ (const KEY_EQUALS_COMPARER& keyEqualsComparer)
            : fKeyEqualsComparer_{keyEqualsComparer}
        {
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fKeyEqualsComparer_{from->fKeyEqualsComparer_}
            , fData_{from->fData_}
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        [[NO_UNIQUE_ADDRESS_ATTR]] const KEY_EQUALS_COMPARER fKeyEqualsComparer_;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
        }
        virtual size_t GetLength () const override
        {
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.GetLength () == 0;
        }
        virtual void Apply (_APPLY_ARGTYPE doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using RESULT_TYPE     = Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;
            using SHARED_REP_TYPE = Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_>;
            size_t i              = fData_.FindFirstThat (doToElement);
            if (i == fData_.GetLength ()) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            SHARED_REP_TYPE resultRep      = Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
            resultRep->fIterator.SetIndex (i);
            // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
            return RESULT_TYPE (move (resultRep));
        }

        // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            return KeyEqualsCompareFunctionType{fKeyEqualsComparer_};
        }
        virtual _MappingRepSharedPtr CloneEmpty ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (fKeyEqualsComparer_);
        }
        virtual _MappingRepSharedPtr CloneAndPatchIterator ([[maybe_unused]] Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>* i, IteratorOwnerID obsoleteForIterableEnvelope) const
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            auto                                                      result = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), obsoleteForIterableEnvelope);
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto&                                                     mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            return result;
        }
        virtual Iterable<KEY_TYPE> Keys () const override
        {
            return this->_Keys_Reference_Implementation ();
        }
        virtual Iterable<MAPPED_VALUE_TYPE> MappedValues () const override
        {
            return this->_Values_Reference_Implementation ();
        }
        virtual bool Lookup (ArgByValueType<KEY_TYPE> key, optional<MAPPED_VALUE_TYPE>* item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    if (item != nullptr) {
                        *item = it.Current ().fValue;
                    }
                    return true;
                }
            }
            if (item != nullptr) {
                *item = nullopt;
            }
            return false;
        }
        virtual bool Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt, AddReplaceMode addReplaceMode) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    switch (addReplaceMode) {
                        case AddReplaceMode::eAddReplaces:
                            fData_[it.CurrentIndex ()].fValue = newElt;
                            break;
                        case AddReplaceMode::eAddIfMissing:
                            break;
                        default:
                            AssertNotReached ();
                    }
                    return false;
                }
            }
            fData_.InsertAt (fData_.GetLength (), KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>{key, newElt});
            return true;
        }
        virtual void Remove (ArgByValueType<KEY_TYPE> key) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    fData_.RemoveAt (it.CurrentIndex ());
                    return;
                }
            }
        }
        virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
        }
        virtual void PatchIteratorBeforeRemove (const optional<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>>& adjustmentAt, Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>* i) const override
        {
            RequireNotNull (i);
            auto iRep = Debug::UncheckedDynamicCast<const IteratorRep_*> (&i->ConstGetRep ());
            if (adjustmentAt) {
                iRep->fIterator.PatchBeforeRemove (&Debug::UncheckedDynamicCast<const IteratorRep_&> (adjustmentAt->ConstGetRep ()).fIterator);
            }
            else {
                iRep->fIterator.PatchBeforeRemove (nullptr);
            }
        }

#if qDebug
        virtual void AssertNoIteratorsReferenceOwner ([[maybe_unused]] IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            //            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using NonPatchingDataStructureImplType_ = DataStructures::Array<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;
        using DataStructureImplType_            = NonPatchingDataStructureImplType_;

    private:
        using IteratorRep_ = typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ***************** Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE> *******************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_Array ()
        : Mapping_Array{equal_to<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>*>
    Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_Array (const KEY_EQUALS_COMPARER& keyEqualsComparer)
        : inherited{inherited::template MakeSmartPtr<Rep_<KEY_EQUALS_COMPARER>> (keyEqualsComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>*>>*>
    Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_Array (const CONTAINER_OF_ADDABLE& src)
        : Mapping_Array{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_KEYVALUE>
    Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_Array (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
        : Mapping_Array{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Compact ()
    {
        using _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                                  accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{accessor._ConstGetRep ().fData_};
        accessor._GetWriteableRep ().fData_.Compact ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline size_t Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::GetCapacity () const
    {
        using _SafeReadRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadRepAccessor<Rep_>;
        _SafeReadRepAccessor                                       accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{accessor._ConstGetRep ().fData_};
        return accessor._ConstGetRep ().fData_.GetCapacity ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::SetCapacity (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor                                  accessor{this};
        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{accessor._ConstGetRep ().fData_};
        accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline size_t Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::capacity () const
    {
        return GetCapacity ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::reserve (size_t slotsAlloced)
    {
        SetCapacity (slotsAlloced);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_ */

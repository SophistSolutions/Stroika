/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/BlockAllocated.h"

#include "../Private/PatchingDataStructures/Array.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /*
                 ********************************************************************************
                 ******* Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::Rep_ ***************
                 ********************************************************************************
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                class Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::Rep_ : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep {
                private:
                    using inherited = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep;

                public:
                    using _IterableRepSharedPtr = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IterableRepSharedPtr;
                    using _MappingRepSharedPtr  = typename inherited::_MappingRepSharedPtr;
                    using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;

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

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
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
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    virtual Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        using RESULT_TYPE     = Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        size_t i              = fData_.FindFirstThat (doToElement);
                        if (i == fData_.GetLength ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetIndex (i);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (resultRep);
                    }

                    // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep overrides
                public:
                    virtual _MappingRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template MakeSharedPtr<Rep_> ();
                        }
                    }
                    virtual Iterable<KEY_TYPE> Keys () const override
                    {
                        return this->_Keys_Reference_Implementation ();
                    }
                    virtual Iterable<MAPPED_VALUE_TYPE> MappedValues () const override
                    {
                        return this->_Values_Reference_Implementation ();
                    }
                    virtual bool Lookup (ArgByValueType<KEY_TYPE> key, Memory::Optional<MAPPED_VALUE_TYPE>* item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                                if (item != nullptr) {
                                    *item = it.Current ().fValue;
                                }
                                return true;
                            }
                        }
                        if (item != nullptr) {
                            item->clear ();
                        }
                        return false;
                    }
                    virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                                fData_[it.CurrentIndex ()].fValue = newElt;
                                return;
                            }
                        }
                        fData_.InsertAt (fData_.GetLength (), KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE> (key, newElt));
                    }
                    virtual void Remove (ArgByValueType<KEY_TYPE> key) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                                fData_.RemoveAt (it.CurrentIndex ());
                                return;
                            }
                        }
                    }
                    virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock>            critSec{fData_};
                        const typename Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::IRep& ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto& mir = dynamic_cast<const IteratorRep_&> (ir);
                        fData_.RemoveAt (mir.fIterator);
                    }
#if qDebug
                    virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                public:
                    using KeyEqualsCompareFunctionType = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType>::KeyEqualsCompareFunctionType;

                private:
                    using NonPatchingDataStructureImplType_ = DataStructures::Array<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;
                    using DataStructureImplType_            = Private::PatchingDataStructures::Array<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;

                private:
                    using IteratorRep_ = typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 *********** Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> *****************
                 ********************************************************************************
                 */
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::Mapping_Array ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::Mapping_Array (const Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
                inline Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::Mapping_Array (const CONTAINER_OF_PAIR_KEY_T& src)
                    : Mapping_Array ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                template <typename COPY_FROM_ITERATOR_KEY_T>
                Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::Mapping_Array (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                    : Mapping_Array ()
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::operator= (const Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (rhs);
                    AssertRepValidType_ ();
                    return *this;
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline void Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::Compact ()
                {
                    using _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadWriteRepAccessor<Rep_>;
                    _SafeReadWriteRepAccessor                                       accessor{this};
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{accessor._ConstGetRep ().fData_};
                    accessor._GetWriteableRep ().fData_.Compact ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline size_t Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::GetCapacity () const
                {
                    using _SafeReadRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadRepAccessor<Rep_>;
                    _SafeReadRepAccessor                                            accessor{this};
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{accessor._ConstGetRep ().fData_};
                    return accessor._ConstGetRep ().fData_.GetCapacity ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline void Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    using _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template _SafeReadWriteRepAccessor<Rep_>;
                    _SafeReadWriteRepAccessor                                       accessor{this};
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{accessor._ConstGetRep ().fData_};
                    accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline size_t Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::capacity () const
                {
                    return GetCapacity ();
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline void Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::reserve (size_t slotsAlloced)
                {
                    SetCapacity (slotsAlloced);
                }
                template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE, typename TRAITS>
                inline void Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/Array.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ***** Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_InternalSync_ ***********
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_InternalSync_ : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_InternalSync_ () = default;
                    Rep_InternalSync_ (const Rep_InternalSync_& from) = delete;
                    Rep_InternalSync_ (Rep_InternalSync_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual  Rep_InternalSync_& operator= (const Rep_InternalSync_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_InternalSync_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep                              Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return  Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> (tmpRep);
                    }
                    virtual size_t                                              GetLength () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.GetLength ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                                                IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.GetLength () == 0;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                                                Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                            // use iterator (which currently implies lots of locks) with this->_Apply ()
                            fData_.Apply (doToElement);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using   RESULT_TYPE     =   Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        SHARED_REP_TYPE resultRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            size_t i = fData_.FindFirstThat (doToElement);
                            if (i == fData_.GetLength ()) {
                                return RESULT_TYPE::GetEmptyIterator ();
                            }
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetIndex (i);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep overrides
                public:
                    virtual _SharedPtrIRep          CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                                r->fData_.RemoveAll ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<Rep_InternalSync_> ();
                        }
                    }
                    virtual Iterable<KEY_TYPE>      Keys () const override
                    {
                        return this->_Keys_Reference_Implementation ();
                    }
                    virtual Iterable<VALUE_TYPE>    Values () const override
                    {
                        return this->_Values_Reference_Implementation ();
                    }
                    virtual bool                    Lookup (ArgByValueType<KEY_TYPE> key, Memory::Optional<VALUE_TYPE>* item) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
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
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                    Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<VALUE_TYPE> newElt) override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                                if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                                    fData_[it.CurrentIndex ()].fValue = newElt;
                                    return;
                                }
                            }
                            fData_.InsertAt (fData_.GetLength (), KeyValuePair<KEY_TYPE, VALUE_TYPE> (key, newElt));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                    Remove (ArgByValueType<KEY_TYPE> key) override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                                if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                                    fData_.RemoveAt (it.CurrentIndex ());
                                    return;
                                }
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                    Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i) override
                    {
                        const typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.RemoveAt (mir.fIterator);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#if     qDebug
                    virtual void                    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#endif

                public:
                    using   KeyEqualsCompareFunctionType        =   typename Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::KeyEqualsCompareFunctionType;

                private:
                    using   NonPatchingDataStructureImplType_   =   DataStructures::Array <KeyValuePair<KEY_TYPE, VALUE_TYPE>>;
                    using   DataStructureImplType_              =   Private::PatchingDataStructures::Array <KeyValuePair<KEY_TYPE, VALUE_TYPE>, Private::ContainerRepLockDataSupport_>;

                private:
                    using   IteratorRep_                        =   typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, VALUE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ***************** Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS> ******************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_Array (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_InternalSync_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_Array (const Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
                inline  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_Array (const CONTAINER_OF_PAIR_KEY_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Mapping_Array (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_Array (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Mapping_Array (containerUpdateSafetyPolicy)
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>&   Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (rhs);
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Compact ()
                {
                    using   _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template _SafeReadWriteRepAccessor<Rep_InternalSync_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        accessor._GetWriteableRep ().fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  size_t  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::GetCapacity () const
                {
                    using   _SafeReadRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template _SafeReadRepAccessor<Rep_InternalSync_>;
                    _SafeReadRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        return accessor._ConstGetRep ().fData_.GetCapacity ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    using   _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template _SafeReadWriteRepAccessor<Rep_InternalSync_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_ */

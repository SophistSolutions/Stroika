/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_Array_inl_

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
                 * Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::UpdateSafeIterationContainerRep_
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::UpdateSafeIterationContainerRep_ : public Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    UpdateSafeIterationContainerRep_ () = default;
                    UpdateSafeIterationContainerRep_ (const UpdateSafeIterationContainerRep_& from) = delete;
                    UpdateSafeIterationContainerRep_ (UpdateSafeIterationContainerRep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual  UpdateSafeIterationContainerRep_& operator= (const UpdateSafeIterationContainerRep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (UpdateSafeIterationContainerRep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep                              Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> (const_cast<UpdateSafeIterationContainerRep_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            UpdateSafeIterationContainerRep_*   NON_CONST_THIS  =   const_cast<UpdateSafeIterationContainerRep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
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
                        SHARED_REP_TYPE     resultRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            size_t i = fData_.FindFirstThat (doToElement);
                            if (i == fData_.GetLength ()) {
                                return RESULT_TYPE::GetEmptyIterator ();
                            }
                            UpdateSafeIterationContainerRep_*   NON_CONST_THIS  =   const_cast<UpdateSafeIterationContainerRep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetIndex (i);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto    r = Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> (const_cast<UpdateSafeIterationContainerRep_*> (this), forIterableEnvelope);
                                r->fData_.RemoveAll ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> ();
                        }
                    }
                    virtual Iterable<KEY_TYPE>  Keys () const override
                    {
                        return this->_Keys_Reference_Implementation ();
                    }
                    virtual bool                Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const override
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
                    virtual void                Add (KEY_TYPE key, VALUE_TYPE newElt) override
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
                    virtual void                Remove (KEY_TYPE key) override
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
                    virtual void                Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i) override
                    {
                        const typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&   mir =   dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.RemoveAt (mir.fIterator);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#endif

                public:
                    using   KeyEqualsCompareFunctionType        =   typename Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::KeyEqualsCompareFunctionType;

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
                 ************** Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS> *****************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_Array (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<UpdateSafeIterationContainerRep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_Array (const Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_Array (const CONTAINER_OF_PAIR_KEY_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Association_Array (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_Array (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Association_Array (containerUpdateSafetyPolicy)
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>&   Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (rhs);
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Compact ()
                {
                    using   _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template _SafeReadWriteRepAccessor<UpdateSafeIterationContainerRep_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        accessor._GetWriteableRep ().fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  size_t  Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::GetCapacity () const
                {
                    using   _SafeReadRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template _SafeReadRepAccessor<UpdateSafeIterationContainerRep_>;
                    _SafeReadRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        return accessor._ConstGetRep ().fData_.GetCapacity ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    using   _SafeReadWriteRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template _SafeReadWriteRepAccessor<UpdateSafeIterationContainerRep_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Association_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<UpdateSafeIterationContainerRep_> tmp { this };   // for side-effect of AssertMember
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_Array_inl_ */

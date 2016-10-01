/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/LinkedList.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                using   Traversal::IteratorOwnerID;


                /*
                 ********************************************************************************
                 *** Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_InternalSync_*****
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_InternalSync_ : public Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep;

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
                            tmpRep = typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> (tmpRep);
                    }
                    virtual size_t                                              GetLength () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return (fData_.GetLength ());
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                                                IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.IsEmpty ();
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
                        using   RESULT_TYPE =   Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        SHARED_REP_TYPE resultRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            auto iLink = fData_.FindFirstThat (doToElement);
                            if (iLink == nullptr) {
                                return RESULT_TYPE::GetEmptyIterator ();
                            }
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetCurrentLink (iLink);
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
                    virtual Iterable<KEY_TYPE>  Keys () const override
                    {
                        return this->_Keys_Reference_Implementation ();
                    }
                    virtual bool                Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructures::LinkedList<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                                if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                                    if (item != nullptr) {
                                        *item = it.Current ().fValue;
                                    }
                                    return true;
                                }
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                        if (item != nullptr) {
                            item->clear ();
                        }
                        return false;
                    }
                    virtual void                Add (KEY_TYPE key, VALUE_TYPE newElt) override
                    {
                        using   Traversal::kUnknownIteratorOwnerID;
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                                if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                                    fData_.SetAt (it, KeyValuePair<KEY_TYPE, VALUE_TYPE> (key, newElt));
                                    return;
                                }
                            }
                            fData_.Append (KeyValuePair<KEY_TYPE, VALUE_TYPE> (key, newElt));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                Remove (KEY_TYPE key) override
                    {
                        using   Traversal::kUnknownIteratorOwnerID;
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                                if (KeyEqualsCompareFunctionType::Equals (it.Current ().fKey, key)) {
                                    fData_.RemoveAt (it);
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
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
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
                    using   KeyEqualsCompareFunctionType    =   typename Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::KeyEqualsCompareFunctionType;

                private:
                    using   DataStructureImplType_          =   Private::PatchingDataStructures::LinkedList<KeyValuePair<KEY_TYPE, VALUE_TYPE>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_                    =   Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, VALUE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 *********** Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS> ***************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_LinkedList (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_InternalSync_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_LinkedList (const Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_LinkedList (const CONTAINER_OF_PAIR_KEY_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Association_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Association_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Association_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_

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


                /*
                 ********************************************************************************
                 *** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_InternalSync_****
                 ********************************************************************************
                 */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                class   Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_InternalSync_ : public Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_SharedPtrIRep;
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
                    virtual _IterableSharedPtrIRep                      Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>     MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> (tmpRep);
                    }
                    virtual size_t                                      GetLength () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.GetLength ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                                        IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.IsEmpty ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                                        Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                            // use iterator (which currently implies lots of locks) with this->_Apply ()
                            fData_.Apply (doToElement);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>     FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using   RESULT_TYPE     =   Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        SHARED_REP_TYPE resultRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            auto iLink = fData_.FindFirstThat (doToElement);
                            if (iLink == nullptr) {
                                return RESULT_TYPE::GetEmptyIterator ();
                            }
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetCurrentLink (iLink);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep overrides
                public:
                    virtual _SharedPtrIRep          CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                                r->fData_.RemoveAll ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<Rep_InternalSync_> ();
                        }
                    }
                    virtual bool                    Equals (const typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual Iterable<DOMAIN_TYPE>   Preimage () const override
                    {
                        AssertNotImplemented ();
                        return *(Iterable<DOMAIN_TYPE>*)nullptr;
                    }
                    virtual Iterable<RANGE_TYPE>    Image () const override
                    {
                        AssertNotImplemented ();
                        return *(Iterable<RANGE_TYPE>*)nullptr;
                    }
                    virtual bool                    Lookup (ArgByValueType<DOMAIN_TYPE> key, Memory::Optional<RANGE_TYPE>* item) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                                if (DomainEqualsCompareFunctionType::Equals (it.Current ().first, key)) {
                                    if (item != nullptr) {
                                        *item = it.Current ().second;
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
                    virtual bool                    InverseLookup (ArgByValueType<RANGE_TYPE> key, Memory::Optional<DOMAIN_TYPE>* item) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                                if (RangeEqualsCompareFunctionType::Equals (it.Current ().second, key)) {
                                    if (item != nullptr) {
                                        *item = it.Current ().first;
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
                    virtual void                    Add (ArgByValueType<DOMAIN_TYPE> key, ArgByValueType<RANGE_TYPE> newElt) override
                    {
                        using   Traversal::kUnknownIteratorOwnerID;
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                                if (DomainEqualsCompareFunctionType::Equals (it.Current ().first, key)) {
                                    fData_.SetAt (it, pair<DOMAIN_TYPE, RANGE_TYPE> (key, newElt));
                                    return;
                                }
                            }
                            fData_.Append (pair<DOMAIN_TYPE, RANGE_TYPE> (key, newElt));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                    RemoveDomainElement (ArgByValueType<DOMAIN_TYPE> d) override
                    {
                        using   Traversal::kUnknownIteratorOwnerID;
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                                if (DomainEqualsCompareFunctionType::Equals (it.Current ().first, d)) {
                                    fData_.RemoveAt (it);
                                    return;
                                }
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                    RemoveRangeElement (ArgByValueType<RANGE_TYPE> r) override
                    {
                        using   Traversal::kUnknownIteratorOwnerID;
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (nullptr, true);) {
                                if (RangeEqualsCompareFunctionType::Equals (it.Current ().second, r)) {
                                    fData_.RemoveAt (it);
                                    return;
                                }
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                    Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i) override
                    {
                        const typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&   mir =   dynamic_cast<const IteratorRep_&> (ir);
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
                    using   DomainEqualsCompareFunctionType =   typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::DomainEqualsCompareFunctionType;
                    using   RangeEqualsCompareFunctionType  =   typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::RangeEqualsCompareFunctionType;

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   Private::IteratorImplHelper_<pair<DOMAIN_TYPE, RANGE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 *********** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS> **************
                 ********************************************************************************
                 */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_InternalSync_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (const Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (const CONTAINER_OF_PAIR_KEY_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Bijection_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Bijection_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&   Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator= (const Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (rhs);
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_ */

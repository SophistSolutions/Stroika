/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                 ********* Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_***********
                 ********************************************************************************
                 */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                class   Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_ : public Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ () = default;
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep                      Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>     MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                                      GetLength () const override;
                    virtual bool                                        IsEmpty () const override;
                    virtual void                                        Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>     FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep overrides
                public:
                    virtual _SharedPtrIRep          CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual bool                    Equals (const typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep& rhs) const override;
                    virtual Iterable<DOMAIN_TYPE>   Preimage () const override;
                    virtual Iterable<RANGE_TYPE>    Image () const override;
                    virtual bool                    Lookup (DOMAIN_TYPE key, Memory::Optional<RANGE_TYPE>* item) const override;
                    virtual bool                    InverseLookup (RANGE_TYPE key, Memory::Optional<DOMAIN_TYPE>* item) const override;
                    virtual void                    Add (DOMAIN_TYPE key, RANGE_TYPE newElt) override;
                    virtual void                    RemoveDomainElement (DOMAIN_TYPE d) override;
                    virtual void                    RemoveRangeElement (RANGE_TYPE r) override;
                    virtual void                    Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i) override;
#if     qDebug
                    virtual void                    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
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
                ******** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_ ***********
                ********************************************************************************
                */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                typename Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::_IterableSharedPtrIRep  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        tmpRep = Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ());
#else
                        tmpRep = Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
#endif
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> (tmpRep);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                size_t  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.GetLength ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                bool  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.IsEmpty ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void      Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>     Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE =   Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>;
                    shared_ptr<IteratorRep_> resultRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        auto iLink = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        resultRep = Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ());
#else
                        resultRep = Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
#endif
                        resultRep->fIterator.SetCurrentLink (iLink);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
#if         qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy
                    return RESULT_TYPE (typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::SharedIRepPtr (resultRep));
#else
                    return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
#endif
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                typename Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::_SharedPtrIRep  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
                {
                    if (fData_.HasActiveIterators ()) {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Traversal::IterableBase::SharedPtrImplementationTemplate<Rep_> (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    else {
                        return _SharedPtrIRep (new Rep_ ());
                    }
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                bool    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Equals (const typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                Iterable<DOMAIN_TYPE>    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Preimage () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<DOMAIN_TYPE>*)nullptr;
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                Iterable<RANGE_TYPE>    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Image () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<RANGE_TYPE>*)nullptr;
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                bool    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Lookup (DOMAIN_TYPE key, Memory::Optional<RANGE_TYPE>* item) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        for (typename ExternallySynchronizedDataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                bool    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::InverseLookup (RANGE_TYPE key, Memory::Optional<DOMAIN_TYPE>* item) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        for (typename ExternallySynchronizedDataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Add (DOMAIN_TYPE key, RANGE_TYPE newElt)
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
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::RemoveDomainElement (DOMAIN_TYPE d)
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
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::RemoveRangeElement (RANGE_TYPE r)
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
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i)
                {
                    const typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     qDebug
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                ********************************************************************************
                *********** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS> **************
                ********************************************************************************
                */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (inherited::template MakeSharedPtr<Rep_> ()))
                {
                    AssertRepValidType_ ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (const Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertRepValidType_ ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (const CONTAINER_OF_PAIR_KEY_T& src)
                    : inherited (typename inherited::_SharedPtrIRep (inherited::template MakeSharedPtr<Rep_> ()))
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                    : inherited (typename inherited::_SharedPtrIRep (inherited::template MakeSharedPtr<Rep_> ()))
                {
                    AssertRepValidType_ ();
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
                    AssertMember (&inherited::_ConstGetRep (), Rep_);
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_ */

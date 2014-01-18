/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                 ********* Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::***************
                 ********************************************************************************
                 */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                class   Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_ : public Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep;

                public:
                    using   _SharedPtrIRep = typename Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using   _IteratorOwnerID = typename inherited::_IteratorOwnerID;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep                              Clone () const override;
                    virtual Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>     MakeIterator (_IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                                      GetLength () const override;
                    virtual bool                                        IsEmpty () const override;
                    virtual void                                        Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>     ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep overrides
                public:
                    virtual bool                    Equals (const typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep& rhs) const override;
                    virtual void                    RemoveAll () override;
                    virtual Iterable<DOMAIN_TYPE>   Preimage () const override;
                    virtual Iterable<RANGE_TYPE>    Image () const override;
                    virtual bool                    Lookup (DOMAIN_TYPE key, Memory::Optional<RANGE_TYPE>* item) const override;
                    virtual bool                    InverseLookup (RANGE_TYPE key, Memory::Optional<DOMAIN_TYPE>* item) const override;
                    virtual void                    Add (DOMAIN_TYPE key, RANGE_TYPE newElt) override;
                    virtual void                    RemoveDomainElement (DOMAIN_TYPE d) override;
                    virtual void                    RemoveRangeElement (RANGE_TYPE r) override;
                    virtual void                    Remove (Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> i) override;
#if     qDebug
                    virtual void                    AssertNoIteratorsReferenceOwner (_IteratorOwnerID oBeingDeleted)    override;
#endif

                public:
                    using   DomainEqualsCompareFunctionType =   typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::DomainEqualsCompareFunctionType;
                    using   RangeEqualsCompareFunctionType  =   typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::RangeEqualsCompareFunctionType;

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>;
                    using   IteratorRep_            =   Private::IteratorImplHelper_<pair<DOMAIN_TYPE, RANGE_TYPE>, DataStructureImplType_>;

                private:
                    Private::ContainerRepLockDataSupport_       fLockSupport_;
                    DataStructureImplType_                      fData_;
                };


                /*
                ********************************************************************************
                ******** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_ ***********
                ********************************************************************************
                */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                typename Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::_SharedPtrIRep  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Clone () const
                {
                    return _SharedPtrIRep (new Rep_ (*this));       // no lock needed cuz src locked in Rep_ CTOR
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::MakeIterator (_IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::SharedIRepPtr (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> (tmpRep);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                size_t  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.GetLength ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                bool  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.IsEmpty ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void      Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>     Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                bool    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Equals (const typename Bijection<DOMAIN_TYPE, RANGE_TYPE, typename TRAITS::BijectionTraitsType>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename Private::DataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename Private::DataStructures::LinkedList<pair<DOMAIN_TYPE, RANGE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (RangeEqualsCompareFunctionType::Equals (it.Current ().second, r)) {
                                fData_.RemoveAt (it);
                                return;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::Remove (Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> i)
                {
                    const typename Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     qDebug
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                void    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (_IteratorOwnerID oBeingDeleted)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        //fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                ********************************************************************************
                *********** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS> *******************
                ********************************************************************************
                */
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (const Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& m)
                    : inherited (static_cast<const inherited&> (m))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (cp);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (start, end);
                }
                template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                inline  Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&   Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator= (const Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs)
                {
                    inherited::operator= (rhs);
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_ */

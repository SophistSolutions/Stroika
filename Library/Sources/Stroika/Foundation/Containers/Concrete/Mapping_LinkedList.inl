/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_inl_

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
                 ************ Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::*****************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ : public Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep;

                public:
                    using   _SharedPtrIRep  =   typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep                                      Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual void                                                Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep overrides
                public:
                    virtual bool                Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep& rhs) const override;
                    virtual void                RemoveAll () override;
                    virtual Iterable<KEY_TYPE>  Keys () const override;
                    virtual bool                Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const override;
                    virtual void                Add (KEY_TYPE key, VALUE_TYPE newElt) override;
                    virtual void                Remove (KEY_TYPE key) override;
                    virtual void                Remove (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i) override;
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) override;
#endif

                public:
                    using   KeyEqualsCompareFunctionType    =   typename Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::KeyEqualsCompareFunctionType;

                private:
                    using   DataStructureImplType_          =   Private::PatchingDataStructures::LinkedList<KeyValuePair<KEY_TYPE, VALUE_TYPE>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_                    =   Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, VALUE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                ********************************************************************************
                ******** Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ ****************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fData_ ()
                {
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                typename Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::_SharedPtrIRep  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return _SharedPtrIRep (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_.fLockSupport, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> (tmpRep);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                size_t  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void      Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>     Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterable<KEY_TYPE>    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Keys () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<KEY_TYPE>*)nullptr;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        for (typename Private::DataStructures::LinkedList<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Add (KEY_TYPE key, VALUE_TYPE newElt)
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
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Remove (KEY_TYPE key)
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
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Remove (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i)
                {
                    const typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     qDebug
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted)
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                ********************************************************************************
                *********** Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS> *******************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_LinkedList (const Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& m)
                    : inherited (static_cast<const inherited&> (m))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_LinkedList (const CONTAINER_OF_PAIR_KEY_T& cp)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (cp);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_LinkedList (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (start, end);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>&   Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
                {
                    inherited::operator= (rhs);
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_inl_ */

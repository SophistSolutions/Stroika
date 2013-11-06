/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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


                /*
                 ********************************************************************************
                 ************ Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::*****************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ : public Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep {
                private:
                    typedef typename    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep  inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
#if     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                    virtual typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep  Clone () const override {
                        return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep (new Rep_ (*this));
                    }
#else
                    virtual typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep     Clone () const override;
#endif
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>                              MakeIterator () const override;
                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual void                                                Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep overrides
                public:
                    virtual bool                Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep& rhs) const override;
                    virtual void                RemoveAll () override;
                    virtual Iterable<KEY_TYPE>  Keys () const override;
                    virtual bool                Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const override;
                    virtual void                Add (KEY_TYPE key, VALUE_TYPE newElt) override;
                    virtual void                Remove (KEY_TYPE key) override;
                    virtual void                Remove (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i) override;

                public:
                    typedef typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::KeyEqualsCompareFunctionType    KeyEqualsCompareFunctionType;

                private:
                    typedef Private::PatchingDataStructures::LinkedList<KeyValuePair<KEY_TYPE, VALUE_TYPE>>             DataStructureImplType_;
                    typedef Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, VALUE_TYPE>, DataStructureImplType_>    IteratorRep_;

                private:
                    Private::ContainerRepLockDataSupport_       fLockSupport_;
                    DataStructureImplType_                      fData_;
                };


                /*
                ********************************************************************************
                ******** Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ ****************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (new IteratorRep_ (&NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> (tmpRep);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                size_t  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool  Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void      Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>     Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_LinkedList<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
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
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


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
                    : inherited (m)
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
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

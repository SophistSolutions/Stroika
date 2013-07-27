/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
                 ************ Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ *****************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ : public Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep {
                private:
                    typedef typename    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep  inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);
                    NO_ASSIGNMENT_OPERATOR(Rep_);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
#if     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                    virtual typename Iterable<pair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep  Clone () const override {
                        return Iterable<pair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep (new Rep_ (*this));
                    }
#else
                    virtual typename Iterable<pair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep     Clone () const override;
#endif
                    virtual Iterator<pair<KEY_TYPE, VALUE_TYPE>>                MakeIterator () const override;
                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual void                                                Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<pair<KEY_TYPE, VALUE_TYPE>>                ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep overrides
                public:
                    virtual bool                Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep& rhs) const override;
                    virtual void                RemoveAll () override;
                    virtual  Iterable<KEY_TYPE> Keys () const override;
                    virtual  bool               Lookup (KEY_TYPE key, VALUE_TYPE* item) const override;
                    virtual  void               Add (KEY_TYPE key, VALUE_TYPE newElt) override;
                    virtual  void               Remove (KEY_TYPE key) override;
                    virtual  void               Remove (Iterator<pair<KEY_TYPE, VALUE_TYPE>> i) override;

                public:
                    typedef typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::KeyEqualsCompareFunctionType    KeyEqualsCompareFunctionType;

                private:
                    typedef Private::DataStructures::Array <
                    pair<KEY_TYPE, VALUE_TYPE>
                    >
                    NonPatchingDataStructureImplType_;
                    typedef Private::PatchingDataStructures::Array_Patch <
                    pair<KEY_TYPE, VALUE_TYPE>
                    >
                    DataStructureImplType_;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;

                private:
                    friend  class   IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ******* Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::IteratorRep_ **************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::IteratorRep_ : public Iterator<pair<KEY_TYPE, VALUE_TYPE>>::IRep {
                private:
                    typedef typename Iterator<pair<KEY_TYPE, VALUE_TYPE>>::IRep    inherited;

                public:
                    explicit IteratorRep_ (typename Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_& owner)
                        : inherited ()
                        , fLockSupport_ (owner.fLockSupport_)
                        , fIterator_ (&owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual shared_ptr<typename Iterator<pair<KEY_TYPE, VALUE_TYPE>>::IRep>     Clone () const override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return typename Iterator<pair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (new IteratorRep_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    More (pair<KEY_TYPE, VALUE_TYPE>* current, bool advance) override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return (fIterator_.More (current, advance));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    StrongEquals (const typename Iterator<pair<KEY_TYPE, VALUE_TYPE>>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    Private::ContainerRepLockDataSupport_&                          fLockSupport_;
                    mutable typename Rep_::DataStructureImplType_::ForwardIterator  fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ************ Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ *****************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ (const Rep_& from)
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
                typename Iterable<pair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<pair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<pair<KEY_TYPE, VALUE_TYPE>>  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<pair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<pair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<pair<KEY_TYPE, VALUE_TYPE>> tmp = Iterator<pair<KEY_TYPE, VALUE_TYPE>> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                size_t  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void      Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<pair<KEY_TYPE, VALUE_TYPE>>     Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterable<KEY_TYPE>    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Keys () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<KEY_TYPE>*)nullptr;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Lookup (KEY_TYPE key, VALUE_TYPE* item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (KeyEqualsCompareFunctionType::Equals (it.Current ().first, key)) {
                                if (item != nullptr) {
                                    *item = it.Current ().second;
                                }
                                return true;
                            }
                        }
                        return false;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Add (KEY_TYPE key, VALUE_TYPE newElt)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (KeyEqualsCompareFunctionType::Equals (it.Current ().first, key)) {
                                fData_[it.CurrentIndex ()].second = newElt;
                                return;
                            }
                        }
                        fData_.InsertAt (fData_.GetLength (), pair<KEY_TYPE, VALUE_TYPE> (key, newElt));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Remove (KEY_TYPE key)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true);) {
                            if (KeyEqualsCompareFunctionType::Equals (it.Current ().first, key)) {
                                fData_.RemoveAt (it.CurrentIndex ());
                                return;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Remove (Iterator<pair<KEY_TYPE, VALUE_TYPE>> i)
                {
                    const typename Iterator<pair<KEY_TYPE, VALUE_TYPE>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::IteratorRep_&       mir =   dynamic_cast<const typename Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator_);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ***************** Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS> ******************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_Array ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_Array (const Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>&   Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
                {
                    inherited::operator= (rhs);
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  const typename Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_&  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return static_cast<const Rep_&> (inherited::_GetRep ());
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  typename Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_&    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return static_cast<Rep_&> (inherited::_GetRep ());
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  size_t  Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::GetCapacity () const
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        return (GetRep_ ().fData_.GetCapacity ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Mapping_Array<KEY_TYPE, VALUE_TYPE, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_ */

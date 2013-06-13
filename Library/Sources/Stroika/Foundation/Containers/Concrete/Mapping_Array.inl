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
                 ********************* Mapping_Array<Key, T>::Rep_ ******************************
                 ********************************************************************************
                 */
                template    <typename Key, typename T>
                class   Mapping_Array<Key, T>::Rep_ : public Mapping<Key, T>::_IRep {
                private:
                    typedef typename    Mapping<Key, T>::_IRep  inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);
                    NO_ASSIGNMENT_OPERATOR(Rep_);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
#if     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                    virtual typename Iterable<pair<Key, T>>::_SharedPtrIRep  Clone () const override {
                        return Iterable<pair<Key, T>>::_SharedPtrIRep (new Rep_ (*this));
                    }
#else
                    virtual typename Iterable<pair<Key, T>>::_SharedPtrIRep     Clone () const override;
#endif
                    virtual Iterator<pair<Key, T>>                              MakeIterator () const override;
                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual void                                                Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<pair<Key, T>>                              ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Mapping<Key, T>::_IRep overrides
                public:
                    virtual bool            Equals (const typename Mapping<Key, T>::_IRep& rhs) const override;
                    virtual void            RemoveAll () override;
                    virtual  Iterable<Key>  Keys () const override;
                    virtual  bool           Lookup (Key key, T* item) const override;
                    virtual  void           Add (Key key, T newElt) override;
                    virtual  void           Remove (Key key) override;
                    virtual  void           Remove (Iterator<pair<Key, T>> i) override;

                private:
                    Private::ContainerRepLockDataSupport_                       fLockSupport_;
                    Private::PatchingDataStructures::Array_Patch<pair<Key, T>>  fData_;

                private:
                    friend  class Mapping_Array<Key, T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ********************* Mapping_Array<Key, T>::IteratorRep_ **********************
                 ********************************************************************************
                 */
                template    <typename Key, typename T>
                class  Mapping_Array<Key, T>::IteratorRep_ : public Iterator<pair<Key, T>>::IRep {
                private:
                    typedef typename Iterator<pair<Key, T>>::IRep    inherited;

                public:
                    explicit IteratorRep_ (typename Mapping_Array<Key, T>::Rep_& owner)
                        : inherited ()
                        , fLockSupport_ (owner.fLockSupport_)
                        , fIterator_ (owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual shared_ptr<typename Iterator<pair<Key, T>>::IRep>     Clone () const override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return typename Iterator<pair<Key, T>>::SharedIRepPtr (new IteratorRep_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    More (pair<Key, T>* current, bool advance) override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return (fIterator_.More (current, advance));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    StrongEquals (const typename Iterator<pair<Key, T>>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    Private::ContainerRepLockDataSupport_&                                              fLockSupport_;
                    mutable Private::PatchingDataStructures::ForwardArrayMutator_Patch<pair<Key, T>>    fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ********************* Mapping_Array<Key, T>::Rep_ ******************************
                ********************************************************************************
                */
                template    <typename Key, typename T>
                inline  Mapping_Array<Key, T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename Key, typename T>
                inline  Mapping_Array<Key, T>::Rep_::Rep_ (const Rep_& from)
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
                template    <typename Key, typename T>
                typename Iterable<pair<Key, T>>::_SharedPtrIRep  Mapping_Array<Key, T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<pair<Key, T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename Key, typename T>
                Iterator<pair<Key, T>>  Mapping_Array<Key, T>::Rep_::MakeIterator () const
                {
                    typename Iterator<pair<Key, T>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<pair<Key, T>>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<pair<Key, T>> tmp = Iterator<pair<Key, T>> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename Key, typename T>
                size_t  Mapping_Array<Key, T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                bool  Mapping_Array<Key, T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                void      Mapping_Array<Key, T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename Key, typename T>
                Iterator<pair<Key, T>>     Mapping_Array<Key, T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename Key, typename T>
                bool    Mapping_Array<Key, T>::Rep_::Equals (const typename Mapping<Key, T>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename Key, typename T>
                void    Mapping_Array<Key, T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                Iterable<Key>    Mapping_Array<Key, T>::Rep_::Keys () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<Key>*)nullptr;
                }
                template    <typename Key, typename T>
                bool    Mapping_Array<Key, T>::Rep_::Lookup (Key key, T* item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (Private::DataStructures::Array<pair<Key, T>>::ForwardArrayIterator it (fData_); it.More (nullptr, true);) {
                            if (it.Current ().first == key) {
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
                template    <typename Key, typename T>
                void    Mapping_Array<Key, T>::Rep_::Add (Key key, T newElt)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (Private::DataStructures::Array<pair<Key, T>>::ForwardArrayIterator it (fData_); it.More (nullptr, true);) {
                            if (it.Current ().first == key) {
                                fData_[it.CurrentIndex ()].second = newElt;
                                return;
                            }
                        }
                        fData_.InsertAt (pair<Key, T> (key, newElt), fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                void    Mapping_Array<Key, T>::Rep_::Remove (Key key)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (Private::DataStructures::Array<pair<Key, T>>::ForwardArrayIterator it (fData_); it.More (nullptr, true);) {
                            if (it.Current ().first == key) {
                                fData_.RemoveAt (it.CurrentIndex ());
                                return;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                void    Mapping_Array<Key, T>::Rep_::Remove (Iterator<pair<Key, T>> i)
                {
                    const typename Iterator<pair<Key, T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Mapping_Array<Key, T>::IteratorRep_&       mir =   dynamic_cast<const typename Mapping_Array<Key, T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ***************************** Mapping_Array<Key, T> ****************************
                ********************************************************************************
                */
                template    <typename Key, typename T>
                Mapping_Array<Key, T>::Mapping_Array ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename Key, typename T>
                inline  Mapping_Array<Key, T>::Mapping_Array (const Mapping_Array<Key, T>& m)
                // static_cast<> so matches proper base class CTOR
                    : inherited (static_cast<const inherited&> (m))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename Key, typename T>
                inline  Mapping_Array<Key, T>&   Mapping_Array<Key, T>::operator= (const Mapping_Array<Key, T>& m)
                {
                    inherited::operator= (m);
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }
                template    <typename Key, typename T>
                inline  const typename Mapping_Array<Key, T>::Rep_&  Mapping_Array<Key, T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename Key, typename T>
                inline  typename Mapping_Array<Key, T>::Rep_&    Mapping_Array<Key, T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }
                template    <typename Key, typename T>
                inline  void    Mapping_Array<Key, T>::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                inline  size_t  Mapping_Array<Key, T>::GetCapacity () const
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        return (GetRep_ ().fData_.GetCapacity ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                inline  void    Mapping_Array<Key, T>::SetCapacity (size_t slotsAlloced)
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

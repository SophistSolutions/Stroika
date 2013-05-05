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

#include    "../Private/DataStructures/Array.h"
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
                    virtual void            RemoveAll () override;
                    virtual  Iterable<Key>  Keys () const override;
                    virtual  bool           Lookup (Key key, T* item) const override;
                    virtual  void           Add (Key key, T newElt) override;
                    virtual  void           Remove (Key key) override;
                    virtual  void           Remove (Iterator<pair<Key, T>> i) override;

                private:
                    Private::ContainerRepLockDataSupport_               fLockSupport_;
                    Private::DataStructures::Array_Patch<pair<Key, T>>  fData_;
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
                    explicit IteratorRep_ (typename Mapping_Array<Key, T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual shared_ptr<typename Iterator<pair<Key, T>>::IRep>     Clone () const override;
                    virtual bool                            More (pair<Key, T>* current, bool advance) override;
                    virtual bool                            StrongEquals (const typename Iterator<pair<Key, T>>::IRep* rhs) const override;

                private:
                    mutable Private::DataStructures::ForwardArrayMutator_Patch<pair<Key, T>>    fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ******************** Mapping_Array<Key, T>::IteratorRep_ ***********************
                ********************************************************************************
                */
                template    <typename Key, typename T>
                Mapping_Array<Key, T>::IteratorRep_::IteratorRep_ (typename Mapping_Array<Key, T>::Rep_& owner)
                    : Iterator<pair<Key, T>>::IRep ()
                                          , fIterator_ (owner.fData_)
                {
                }
                template    <typename Key, typename T>
                bool    Mapping_Array<Key, T>::IteratorRep_::More (pair<Key, T>* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename Key, typename T>
                bool    Mapping_Array<Key, T>::IteratorRep_::StrongEquals (const typename Iterator<pair<Key, T>>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename Key, typename T>
                shared_ptr<typename Iterator<pair<Key, T>>::IRep>  Mapping_Array<Key, T>::IteratorRep_::Clone () const
                {
                    return shared_ptr<typename Iterator<pair<Key, T>>::IRep> (new IteratorRep_ (*this));
                }


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
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_ = from.fData_;
                    });
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
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<pair<Key, T>> tmp = Iterator<pair<Key, T>> (typename Iterator<pair<Key, T>>::SharedByValueRepType (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename Key, typename T>
                size_t  Mapping_Array<Key, T>::Rep_::GetLength () const
                {
                    return (fData_.GetLength ());
                }
                template    <typename Key, typename T>
                bool  Mapping_Array<Key, T>::Rep_::IsEmpty () const
                {
                    return (fData_.GetLength () == 0);
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
                void    Mapping_Array<Key, T>::Rep_::RemoveAll ()
                {
                    fData_.RemoveAll ();
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
                    for (Private::DataStructures::ForwardArrayIterator<pair<Key, T>> it (fData_); it.More (nullptr, true);) {
                        if (it.Current ().first == key) {
                            if (item != nullptr) {
                                *item = it.Current ().second;
                            }
                            return true;
                        }
                    }
                    return false;
                }
                template    <typename Key, typename T>
                void    Mapping_Array<Key, T>::Rep_::Add (Key key, T newElt)
                {
                    for (Private::DataStructures::ForwardArrayIterator<pair<Key, T>> it (fData_); it.More (nullptr, true);) {
                        if (it.Current ().first == key) {
                            fData_[it.CurrentIndex ()].second = newElt;
                            return;
                        }
                    }
                    fData_.InsertAt (pair<Key, T> (key, newElt), fData_.GetLength ());
                }
                template    <typename Key, typename T>
                void    Mapping_Array<Key, T>::Rep_::Remove (Key key)
                {
                    for (Private::DataStructures::ForwardArrayIterator<pair<Key, T>> it (fData_); it.More (nullptr, true);) {
                        if (it.Current ().first == key) {
                            fData_.RemoveAt (it.CurrentIndex ());
                            return;
                        }
                    }
                }
                template    <typename Key, typename T>
                void    Mapping_Array<Key, T>::Rep_::Remove (Iterator<pair<Key, T>> i)
                {
                    const typename Iterator<pair<Key, T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Mapping_Array<Key, T>::IteratorRep_&       mir =   dynamic_cast<const typename Mapping_Array<Key, T>::IteratorRep_&> (ir);
                    mir.fIterator_.RemoveCurrent ();
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
                }
                template    <typename Key, typename T>
                inline  Mapping_Array<Key, T>::Mapping_Array (const Mapping_Array<Key, T>& m)
                    : inherited (m)
                {
                }
                template    <typename Key, typename T>
                inline  Mapping_Array<Key, T>&   Mapping_Array<Key, T>::operator= (const Mapping_Array<Key, T>& m)
                {
                    Mapping<Key, T>::operator= (m);
                    return *this;
                }
                template    <typename Key, typename T>
                inline  const typename Mapping_Array<Key, T>::Rep_&  Mapping_Array<Key, T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    return (static_cast<const Rep_&> (Mapping<Key, T>::_GetRep ()));
                }
                template    <typename Key, typename T>
                inline  typename Mapping_Array<Key, T>::Rep_&    Mapping_Array<Key, T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    return (static_cast<const Rep_&> (Mapping<Key, T>::_GetRep ()));
                }
                template    <typename Key, typename T>
                inline  void    Mapping_Array<Key, T>::Compact ()
                {
                    GetRep_ ().fData_.Compact ();
                }
                template    <typename Key, typename T>
                inline  size_t  Mapping_Array<Key, T>::GetCapacity () const
                {
                    return (GetRep_ ().fData_.GetCapacity ());
                }
                template    <typename Key, typename T>
                inline  void    Mapping_Array<Key, T>::SetCapacity (size_t slotsAlloced)
                {
                    GetRep_ ().fData_.SetCapacity (slotsAlloced);
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_Array_inl_ */

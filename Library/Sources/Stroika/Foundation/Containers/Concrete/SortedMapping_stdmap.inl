/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <map>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/DataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ******************* SortedMapping_stdmap<Key, T>::Rep_ *************************
                 ********************************************************************************
                 */
                template    <typename Key, typename T>
                class   SortedMapping_stdmap<Key, T>::Rep_ : public SortedMapping<Key, T>::_IRep {
                private:
                    typedef typename    SortedMapping<Key, T>::_IRep  inherited;

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
                    Private::ContainerRepLockDataSupport_                                               fLockSupport_;
                    Private::DataStructures::Patching::STLContainerWrapper<pair<Key, T>, map<Key, T>>   fData_;

                private:
                    friend  class SortedMapping_stdmap<Key, T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ******************* SortedMapping_stdmap<Key, T>::IteratorRep_ *****************
                 ********************************************************************************
                 */
                template    <typename Key, typename T>
                class  SortedMapping_stdmap<Key, T>::IteratorRep_ : public Iterator<pair<Key, T>>::IRep {
                private:
                    typedef typename Iterator<pair<Key, T>>::IRep    inherited;

                public:
                    explicit IteratorRep_ (typename SortedMapping_stdmap<Key, T>::Rep_& owner)
                        : inherited ()
                        , fIterator_ (&owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<pair<Key, T>>::SharedIRepPtr     Clone () const override {
                        return typename Iterator<pair<Key, T>>::SharedIRepPtr (new IteratorRep_ (*this));
                    }
                    virtual bool   More (pair<Key, T>* current, bool advance) override {
                        return (fIterator_.More (current, advance));
                    }
                    virtual bool   StrongEquals (const typename Iterator<pair<Key, T>>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    mutable typename Private::DataStructures::Patching::STLContainerWrapper<pair<Key, T>, map<Key, T>>::BasicForwardIterator   fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ******************* SortedMapping_stdmap<Key, T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename Key, typename T>
                inline  SortedMapping_stdmap<Key, T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    fData_.Invariant ();
                }
                template    <typename Key, typename T>
                inline  SortedMapping_stdmap<Key, T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_.Invariant ();
                        from.fData_.Invariant ();
                        fData_ = from.fData_;
                        from.fData_.Invariant ();
                        fData_.Invariant ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename Key, typename T>
                typename Iterable<pair<Key, T>>::_SharedPtrIRep  SortedMapping_stdmap<Key, T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<pair<Key, T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename Key, typename T>
                Iterator<pair<Key, T>>  SortedMapping_stdmap<Key, T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<pair<Key, T>> tmp = Iterator<pair<Key, T>> (typename Iterator<pair<Key, T>>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename Key, typename T>
                size_t  SortedMapping_stdmap<Key, T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        return (fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                bool  SortedMapping_stdmap<Key, T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        return (fData_.empty () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                void      SortedMapping_stdmap<Key, T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename Key, typename T>
                Iterator<pair<Key, T>>     SortedMapping_stdmap<Key, T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename Key, typename T>
                bool    SortedMapping_stdmap<Key, T>::Rep_::Equals (const typename Mapping<Key, T>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename Key, typename T>
                void    SortedMapping_stdmap<Key, T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.clear_WithPatching ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                Iterable<Key>    SortedMapping_stdmap<Key, T>::Rep_::Keys () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<Key>*)nullptr;
                }
                template    <typename Key, typename T>
                bool    SortedMapping_stdmap<Key, T>::Rep_::Lookup (Key key, T* item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        auto i = fData_.find (key);
                        if (i == fData_.end ()) {
                            return false;
                        }
                        else {
                            if (item != nullptr) {
                                *item = i->second;
                            }
                            return true;
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return false;
                }
                template    <typename Key, typename T>
                void    SortedMapping_stdmap<Key, T>::Rep_::Add (Key key, T newElt)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        auto i = fData_.find (key);
                        if (i == fData_.end ()) {
                            i = fData_.insert (pair<Key, T> (key, newElt)).first;
                            // no need to patch map<>
                        }
                        else {
                            i->second = newElt;
                        }
                        fData_.Invariant ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                void    SortedMapping_stdmap<Key, T>::Rep_::Remove (Key key)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        auto i = fData_.find (key);
                        if (i != fData_.end ()) {
                            fData_.erase_WithPatching (i);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename Key, typename T>
                void    SortedMapping_stdmap<Key, T>::Rep_::Remove (Iterator<pair<Key, T>> i)
                {
                    const typename Iterator<pair<Key, T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename SortedMapping_stdmap<Key, T>::IteratorRep_&       mir =   dynamic_cast<const typename SortedMapping_stdmap<Key, T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ************************ SortedMapping_stdmap<Key, T> **************************
                ********************************************************************************
                */
                template    <typename Key, typename T>
                SortedMapping_stdmap<Key, T>::SortedMapping_stdmap ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename Key, typename T>
                inline  SortedMapping_stdmap<Key, T>::SortedMapping_stdmap (const SortedMapping_stdmap<Key, T>& m)
                // static_cast<> so we pick the right base class CTOR that doesn't copy
                    : inherited (static_cast<const SortedMapping<Key, T>&> (m))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename Key, typename T>
                inline  SortedMapping_stdmap<Key, T>&   SortedMapping_stdmap<Key, T>::operator= (const SortedMapping_stdmap<Key, T>& m)
                {
                    inherited::operator= (static_cast<const inherited&> (m));
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }
                template    <typename Key, typename T>
                inline  const typename SortedMapping_stdmap<Key, T>::Rep_&  SortedMapping_stdmap<Key, T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename Key, typename T>
                inline  typename SortedMapping_stdmap<Key, T>::Rep_&    SortedMapping_stdmap<Key, T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_ */

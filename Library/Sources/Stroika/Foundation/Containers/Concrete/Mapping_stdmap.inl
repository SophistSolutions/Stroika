/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <map>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/DataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************* Mapping_stdmap<Key, T>::Rep_ *************************
                 ********************************************************************************
                 */
                template    <typename Key, typename T>
                class   Mapping_stdmap<Key, T>::Rep_ : public Mapping<Key, T>::_IRep {
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
                    Private::ContainerRepLockDataSupport_                                   fLockSupport_;
                    Private::DataStructures::STLContainerWrapper<pair<Key, T>, map<Key, T>> fData_;

                private:
                    friend  class Mapping_stdmap<Key, T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 *********************** Mapping_stdmap<Key, T>::IteratorRep_ *******************
                 ********************************************************************************
                 */
                template    <typename Key, typename T>
                class  Mapping_stdmap<Key, T>::IteratorRep_ : public Iterator<pair<Key, T>>::IRep {
                private:
                    typedef typename Iterator<pair<Key, T>>::IRep    inherited;

                public:
                    explicit IteratorRep_ (typename Mapping_stdmap<Key, T>::Rep_& owner)
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
                    mutable typename Private::DataStructures::STLContainerWrapper<pair<Key, T>, map<Key, T>>::IteratorPatchHelper   fIterator_;

                private:
                    friend  class   Rep_;
                };




                /*
                ********************************************************************************
                ************************* Mapping_stdmap<Key, T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename Key, typename T>
                inline  Mapping_stdmap<Key, T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename Key, typename T>
                inline  Mapping_stdmap<Key, T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_ (from.fLockSupport_, {
                        fData_ = from.fData_;
                    });
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename Key, typename T>
                typename Iterable<pair<Key, T>>::_SharedPtrIRep  Mapping_stdmap<Key, T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<pair<Key, T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename Key, typename T>
                Iterator<pair<Key, T>>  Mapping_stdmap<Key, T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<pair<Key, T>> tmp = Iterator<pair<Key, T>> (typename Iterator<pair<Key, T>>::SharedByValueRepType (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename Key, typename T>
                size_t  Mapping_stdmap<Key, T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.size ());
                    });
                }
                template    <typename Key, typename T>
                bool  Mapping_stdmap<Key, T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.empty () == 0);
                    });
                }
                template    <typename Key, typename T>
                void      Mapping_stdmap<Key, T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename Key, typename T>
                Iterator<pair<Key, T>>     Mapping_stdmap<Key, T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename Key, typename T>
                void    Mapping_stdmap<Key, T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.clear ();
                        fData_.PatchAfter_clear ();
                    });
                }
                template    <typename Key, typename T>
                Iterable<Key>    Mapping_stdmap<Key, T>::Rep_::Keys () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<Key>*)nullptr;
                }
                template    <typename Key, typename T>
                bool    Mapping_stdmap<Key, T>::Rep_::Lookup (Key key, T* item) const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
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
                    });
                    return false;
                }
                template    <typename Key, typename T>
                void    Mapping_stdmap<Key, T>::Rep_::Add (Key key, T newElt)
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        auto i = fData_.find (key);
                        if (i == fData_.end ()) {
                            i = fData_.insert (pair<Key, T> (key, newElt)).first;
                            fData_.PatchAfter_insert (i);
                        }
                        else {
                            i->second = newElt;
                        }
                    });
                }
                template    <typename Key, typename T>
                void    Mapping_stdmap<Key, T>::Rep_::Remove (Key key)
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        auto i = fData_.find (key);
                        if (i != fData_.end ()) {
                            fData_.PatchBefore_erase (i);
                            fData_.erase (i);
                        }
                    });
                }
                template    <typename Key, typename T>
                void    Mapping_stdmap<Key, T>::Rep_::Remove (Iterator<pair<Key, T>> i)
                {
                    const typename Iterator<pair<Key, T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Mapping_stdmap<Key, T>::IteratorRep_&       mir =   dynamic_cast<const typename Mapping_stdmap<Key, T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        auto i = mir.fIterator_.fStdIterator;
                        fData_.PatchBefore_erase (i);
                        fData_.erase (i);
                    });
                }


                /*
                ********************************************************************************
                ****************************** Mapping_stdmap<Key, T> **************************
                ********************************************************************************
                */
                template    <typename Key, typename T>
                Mapping_stdmap<Key, T>::Mapping_stdmap ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename Key, typename T>
                inline  Mapping_stdmap<Key, T>::Mapping_stdmap (const Mapping_stdmap<Key, T>& m)
                    : inherited (static_cast<const Mapping<Key, T>&> (m))
                {
                }
                template    <typename Key, typename T>
                inline  Mapping_stdmap<Key, T>&   Mapping_stdmap<Key, T>::operator= (const Mapping_stdmap<Key, T>& m)
                {
                    Mapping<Key, T>::operator= (m);
                    return *this;
                }
                template    <typename Key, typename T>
                inline  const typename Mapping_stdmap<Key, T>::Rep_&  Mapping_stdmap<Key, T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename Key, typename T>
                inline  typename Mapping_stdmap<Key, T>::Rep_&    Mapping_stdmap<Key, T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_ */

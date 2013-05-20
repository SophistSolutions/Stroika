/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <set>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/DataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************* SortedSet_stdset<T>::Rep_ ****************************
                 ********************************************************************************
                 */
                template    <typename T>
                class   SortedSet_stdset<T>::Rep_ : public SortedSet<T>::_IRep {
                private:
                    typedef typename    SortedSet<T>::_IRep  inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);
                    NO_ASSIGNMENT_OPERATOR(Rep_);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override;
                    virtual Iterator<T>                             MakeIterator () const override;
                    virtual size_t                                  GetLength () const override;
                    virtual bool                                    IsEmpty () const override;
                    virtual void                                    Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // SortedSet<T>::_IRep overrides
                public:
                    virtual bool    Equals (const typename Set<T>::_IRep& rhs) const override;
                    virtual bool    Contains (T item) const override;
                    virtual void    RemoveAll () override;
                    virtual void    Add (T item) override;
                    virtual void    Remove (T item) override;
                    virtual void    Remove (const Iterator<T>& i) override;

                private:
                    Private::ContainerRepLockDataSupport_                   fLockSupport_;
                    Private::DataStructures::STLContainerWrapper<T, set<T>> fData_;

                private:
                    friend  class SortedSet_stdset<T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ************************** SortedSet_stdset<T>::IteratorRep_ *******************
                 ********************************************************************************
                 */
                template    <typename T>
                class  SortedSet_stdset<T>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename Iterator<T>::IRep    inherited;

                public:
                    explicit IteratorRep_ (typename SortedSet_stdset<T>::Rep_& owner)
                        : inherited ()
                        , fIterator_ (&owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::SharedIRepPtr     Clone () const override {
                        return typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*this));
                    }
                    virtual bool   More (T* current, bool advance) override {
                        return (fIterator_.More (current, advance));
                    }
                    virtual bool   StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    mutable typename Private::DataStructures::STLContainerWrapper<T, set<T>>::IteratorPatchHelper   fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ************************** SortedSet_stdset<T>::Rep_ ***************************
                ********************************************************************************
                */
                template    <typename T>
                inline  SortedSet_stdset<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  SortedSet_stdset<T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_ (from.fLockSupport_, {
                        fData_ = from.fData_;
                    });
                }
                template    <typename T>
                typename Iterable<T>::_SharedPtrIRep  SortedSet_stdset<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  SortedSet_stdset<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<T> tmp = Iterator<T> (typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                size_t  SortedSet_stdset<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.Invariant ();
                        return (fData_.size ());
                    });
                }
                template    <typename T>
                bool  SortedSet_stdset<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.Invariant ();
                        return (fData_.empty () == 0);
                    });
                }
                template    <typename T>
                void      SortedSet_stdset<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     SortedSet_stdset<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                bool    SortedSet_stdset<T>::Rep_::Equals (const typename Set<T>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T>
                bool    SortedSet_stdset<T>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return fData_.find (item) != fData_.end ();
                    });
                }
                template    <typename T>
                void    SortedSet_stdset<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.clear ();
                        // must patch!!!
                    });
                }
                template    <typename T>
                void    SortedSet_stdset<T>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.insert (item);
                        // must patch!!!
                    });
                }
                template    <typename T>
                void    SortedSet_stdset<T>::Rep_::Remove (T item)
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.Invariant ();
                        auto i = fData_.find (item);
                        if (i != fData_.end ()) {
                            fData_.PatchBefore_erase (i);
                            fData_.erase (i);
                            fData_.Invariant ();
                        }
                    });
                }
                template    <typename T>
                void    SortedSet_stdset<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename SortedSet_stdset<T>::IteratorRep_&       mir =   dynamic_cast<const typename SortedSet_stdset<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        mir.fIterator_.RemoveCurrent ();
                    });
                }


                /*
                ********************************************************************************
                ****************************** SortedSet_stdset<T> *****************************
                ********************************************************************************
                */
                template    <typename T>
                SortedSet_stdset<T>::SortedSet_stdset ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                inline  SortedSet_stdset<T>::SortedSet_stdset (const SortedSet_stdset<T>& m)
                    : inherited (m)
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T>
                inline  SortedSet_stdset<T>::SortedSet_stdset (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    this->AddAll (s);
                }
                template    <typename T>
                inline  SortedSet_stdset<T>&   SortedSet_stdset<T>::operator= (const SortedSet_stdset<T>& m)
                {
                    inherited::operator= (m);
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }
                template    <typename T>
                inline  const typename SortedSet_stdset<T>::Rep_&  SortedSet_stdset<T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SortedSetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T>
                inline  typename SortedSet_stdset<T>::Rep_&    SortedSet_stdset<T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SortedSetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_inl_ */

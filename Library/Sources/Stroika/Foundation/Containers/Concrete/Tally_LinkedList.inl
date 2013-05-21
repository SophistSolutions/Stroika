/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_    1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/DataStructures/LinkedList.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************** Tally_LinkedList<T>::Rep_ ***************************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Tally_LinkedList<T>::Rep_ : public Tally<T>::_IRep {
                private:
                    typedef typename Tally<T>::_IRep   inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);
                    NO_ASSIGNMENT_OPERATOR(Rep_);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
#if     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                    virtual typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Clone () const override {
                        return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                    }
#else
                    virtual typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Clone () const override;
#endif
                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual Iterator<TallyEntry<T>>                             MakeIterator () const override;
                    virtual void                                                Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<TallyEntry<T>>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Tally<T>::_IRep overrides
                public:
                    virtual bool                                Equals (const typename Tally<T>::_IRep& rhs) const override;
                    virtual bool                                Contains (T item) const override;
                    virtual void                                RemoveAll () override;
                    virtual void                                Add (T item, size_t count) override;
                    virtual void                                Remove (T item, size_t count) override;
                    virtual void                                Remove (const Iterator<TallyEntry<T>>& i) override;
                    virtual void                                UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount) override;
                    virtual size_t                              TallyOf (T item) const override;
                    virtual Iterator<T>                         MakeBagIterator () const override;

                private:
                    Private::ContainerRepLockDataSupport_                       fLockSupport_;
                    Private::DataStructures::LinkedList_Patch<TallyEntry<T>>    fData_;

                    friend  class Tally_LinkedList<T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 *********************** Tally_LinkedList<T>::IteratorRep_ **********************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Tally_LinkedList<T>::IteratorRep_ : public Iterator<TallyEntry<T>>::IRep {
                private:
                    typedef typename Iterator<TallyEntry<T>>::IRep  inherited;
                public:
                    IteratorRep_ (typename Tally_LinkedList<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                public:
                    virtual bool                                                More (TallyEntry<T>* current, bool advance) override;
                    virtual bool                                                StrongEquals (const typename Iterator<TallyEntry<T> >::IRep* rhs) const override;
                    virtual shared_ptr<typename Iterator<TallyEntry<T>>::IRep>  Clone () const override;

                private:
                    mutable Private::DataStructures::LinkedListMutator_Patch<TallyEntry<T>> fIterator_;

                    friend class Tally_LinkedList<T>::Rep_;
                };


                /*
                 ********************************************************************************
                 ********************* Tally_LinkedList<T>::IteratorRep_ ************************
                 ********************************************************************************
                 */
                template    <typename T>
                Tally_LinkedList<T>::IteratorRep_::IteratorRep_ (Rep_& owner)
                    : fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                bool    Tally_LinkedList<T>::IteratorRep_::More (TallyEntry<T>* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Tally_LinkedList<T>::IteratorRep_::StrongEquals (const typename Iterator<TallyEntry<T>>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                shared_ptr<typename Iterator<TallyEntry<T>>::IRep> Tally_LinkedList<T>::IteratorRep_::Clone () const
                {
                    return shared_ptr<typename Iterator<TallyEntry<T>>::IRep> (new IteratorRep_ (*this));
                }


                /*
                 ********************************************************************************
                 ************************* Tally_LinkedList<T>::Rep_ ****************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Tally_LinkedList<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Tally_LinkedList<T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_ (from.fLockSupport_, {
                        fData_ = from.fData_;
                    });
                }
                template    <typename T>
                size_t  Tally_LinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.GetLength ());
                    });
                }
                template    <typename T>
                bool  Tally_LinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.GetLength () == 0);
                    });
                }
                template    <typename T>
                Iterator<TallyEntry<T>> Tally_LinkedList<T>::Rep_::MakeIterator () const
                {
                    // const cast cuz this mutator won't really be used to change anything - except stuff like
                    // link list of owned iterators
                    Iterator<TallyEntry<T>> tmp = Iterator<TallyEntry<T>> (typename Iterator<TallyEntry<T>>::SharedIRepPtr (new IteratorRep_ (*const_cast<Rep_*> (this))));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                void      Tally_LinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<TallyEntry<T>>     Tally_LinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                bool    Tally_LinkedList<T>::Rep_::Equals (const typename Tally<T>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T>
                bool   Tally_LinkedList<T>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        TallyEntry<T>   c;
                        for (Private::DataStructures::LinkedListIterator<TallyEntry<T>> it (fData_); it.More (&c, true); ) {
                            if (c.fItem == item) {
                                Assert (c.fCount != 0);
                                return (true);
                            }
                        }
                        return (false);
                    });
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename T>
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep   Tally_LinkedList<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::Add (T item, size_t count)
                {
                    if (count != 0) {
                        TallyEntry<T>   current (item);
                        CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                            for (Private::DataStructures::LinkedListMutator_Patch<TallyEntry<T> > it (fData_); it.More (&current, true); ) {
                                if (current.fItem == item) {
                                    current.fCount += count;
                                    it.UpdateCurrent (current);
                                    return;
                                }
                            }
                            fData_.Prepend (TallyEntry<T> (item, count));
                        });
                    }
                }
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::Remove (T item, size_t count)
                {
                    if (count != 0) {
                        TallyEntry<T>   current (item);
                        CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                            for (Private::DataStructures::LinkedListMutator_Patch<TallyEntry<T> > it (fData_); it.More (&current, true); ) {
                                if (current.fItem == item) {
                                    if (current.fCount > count) {
                                        current.fCount -= count;
                                    }
                                    else {
                                        current.fCount = 0;     // Should this be an underflow excpetion, assertion???
                                    }
                                    if (current.fCount == 0) {
                                        it.RemoveCurrent ();
                                    }
                                    else {
                                        it.UpdateCurrent (current);
                                    }
                                    break;
                                }
                            }
                        });
                    }
                }
                template    <typename T>
                void    Tally_LinkedList<T>::Rep_::Remove (const Iterator<TallyEntry<T>>& i)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Tally_LinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Tally_LinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        mir.fIterator_.RemoveCurrent ();
                    });
                }
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.RemoveAll ();
                    });
                }
                template    <typename T>
                void    Tally_LinkedList<T>::Rep_::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Tally_LinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Tally_LinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        if (newCount == 0) {
                            mir.fIterator_.RemoveCurrent ();
                        }
                        else {
                            TallyEntry<T>   c   =   mir.fIterator_.Current ();
                            c.fCount = newCount;
                            mir.fIterator_.UpdateCurrent (c);
                        }
                    });
                }
                template    <typename T>
                size_t Tally_LinkedList<T>::Rep_::TallyOf (T item) const
                {
                    TallyEntry<T>   c;
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        for (Private::DataStructures::LinkedListIterator<TallyEntry<T>> it (fData_); it.More (&c, true); ) {
                            if (c.fItem == item) {
                                Ensure (c.fCount != 0);
                                return (c.fCount);
                            }
                        }
                    });
                    return (0);
                }
                template    <typename T>
                Iterator<T>    Tally_LinkedList<T>::Rep_::MakeBagIterator () const
                {
                    Iterator<T> tmp =   Iterator<T> (typename Iterator<T>::SharedIRepPtr (new typename Rep_::_TallyEntryToItemIteratorHelperRep (MakeIterator ())));
                    //tmphack - must fix to have iteratorrep dont proerply and not need to init owning itgerator object
                    tmp++;
                    return tmp;
                }


                /*
                 ********************************************************************************
                 **************************** Tally_LinkedList<T> *******************************
                 ********************************************************************************
                 */
                template    <typename T>
                Tally_LinkedList<T>::Tally_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>
                Tally_LinkedList<T>::Tally_LinkedList (const T* items, size_t size) :
                    Tally<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AddItems (items, size);
                }
                template    <typename T>
                Tally_LinkedList<T>::Tally_LinkedList (const Tally<T>& src) :
                    Tally<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    operator+= (src);
                }
                template    <typename T>
                inline Tally_LinkedList<T>::Tally_LinkedList (const Tally_LinkedList<T>& src) :
                    Tally<T> (src)
                {
                }
                template    <typename T>
                inline Tally_LinkedList<T>& Tally_LinkedList<T>::operator= (const Tally_LinkedList<T>& src)
                {
                    Tally<T>::operator= (src);
                    return (*this);
                }


            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_ */

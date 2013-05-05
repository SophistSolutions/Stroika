/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_

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


                template    <typename T>
                class   Sequence_LinkedList<T>::Rep_ : public Sequence<T>::_IRep {
                private:
                    typedef typename    Sequence<T>::_IRep  inherited;

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

                    // Sequence<T>::_IRep overrides
                public:
                    virtual T       GetAt (size_t i) const override;
                    virtual void    SetAt (size_t i, const T& item) override;
                    virtual size_t  IndexOf (const Iterator<T>& i) const override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Insert (size_t at, const T* from, const T* to) override;
                    virtual void    Remove (size_t from, size_t to) override;

                private:
                    Private::ContainerRepLockDataSupport_           fLockSupport_;
                    Private::DataStructures::LinkedList_Patch<T>    fData_;
                    friend  class Sequence_LinkedList<T>::IteratorRep_;
                };


                template    <typename T>
                class  Sequence_LinkedList<T>::IteratorRep_ : public Iterator<T>::IRep {
                public:
                    explicit IteratorRep_ (typename Sequence_LinkedList<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual shared_ptr<typename Iterator<T>::IRep>     Clone () const override;
                    virtual bool                            More (T* current, bool advance) override;
                    virtual bool                            StrongEquals (const typename Iterator<T>::IRep* rhs) const override;

                private:
                    //mutable ForwardLinkedListMutator_Patch<T>    fIterator_;
                    mutable Private::DataStructures::LinkedListMutator_Patch<T>    fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ******************** Sequence_LinkedList<T>::IteratorRep_ **********************
                ********************************************************************************
                */
                template    <typename T>
                Sequence_LinkedList<T>::IteratorRep_::IteratorRep_ (typename Sequence_LinkedList<T>::Rep_& owner)
                    : Iterator<T>::IRep ()
                    , fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                bool    Sequence_LinkedList<T>::IteratorRep_::More (T* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Sequence_LinkedList<T>::IteratorRep_::StrongEquals (const typename Iterator<T>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                shared_ptr<typename Iterator<T>::IRep>  Sequence_LinkedList<T>::IteratorRep_::Clone () const
                {
                    return shared_ptr<typename Iterator<T>::IRep> (new IteratorRep_ (*this));
                }


                /*
                ********************************************************************************
                ************************* Sequence_LinkedList<T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Sequence_LinkedList<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Sequence_LinkedList<T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_ = from.fData_;
                    });
                }
                template    <typename T>
                typename Iterable<T>::_SharedPtrIRep  Sequence_LinkedList<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Sequence_LinkedList<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<T> tmp = Iterator<T> (typename Iterator<T>::SharedByValueRepType (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                size_t  Sequence_LinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.GetLength ());
                    });
                }
                template    <typename T>
                bool  Sequence_LinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.GetLength () == 0);
                    });
                }
                template    <typename T>
                void      Sequence_LinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Sequence_LinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                T    Sequence_LinkedList<T>::Rep_::GetAt (size_t i) const
                {
                    Require (not IsEmpty ());
                    Require (i == kBadSequenceIndex or i < GetLength ());
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        if (i == kBadSequenceIndex) {
                            i = GetLength () - 1;
                        }
                        return fData_.GetAt (i);
                    });
                }
                template    <typename T>
                void    Sequence_LinkedList<T>::Rep_::SetAt (size_t i, const T& item)
                {
                    Require (i < GetLength ());
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {fData_.SetAt (item, i);});
                }
                template    <typename T>
                size_t    Sequence_LinkedList<T>::Rep_::IndexOf (const Iterator<T>& i) const
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_LinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_LinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {return mir.fIterator_.CurrentIndex ();});
                }
                template    <typename T>
                void    Sequence_LinkedList<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_LinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_LinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {mir.fIterator_.RemoveCurrent ();});
                }
                template    <typename T>
                void    Sequence_LinkedList<T>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_LinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_LinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {mir.fIterator_.UpdateCurrent (newValue);});
                }
                template    <typename T>
                void    Sequence_LinkedList<T>::Rep_::Insert (size_t at, const T* from, const T* to)
                {
                    Require (at == kBadSequenceIndex or at <= GetLength ());
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        if (at == kBadSequenceIndex) {
                            at = fData_.GetLength ();
                        }
                        // quickie poor impl
                        // See Stroika v1 - much better - handling cases of remove near start or end of linked list
                        if (at == 0) {
                            size_t len = to - from;
                            for (size_t i = (to - from); i > 0; --i) {
                                fData_.Prepend (from[i - 1]);
                            }
                        }
                        else if (at == fData_.GetLength ()) {
                            for (const T* p = from; p != to; ++p) {
                                fData_.Append (*p);
                            }
                        }
                        else {
                            size_t index = at;
                            T tmphack;
                            for (Private::DataStructures::LinkedListMutator_Patch<T> it (fData_); it.More (&tmphack, true); ) {
                                if (--index == 0) {
                                    for (const T* p = from; p != to; ++p) {
                                        it.AddBefore (*p);
                                    }
                                    break;
                                }
                            }
                            //Assert (not it.Done ());      // cuz that would mean we never added
                        }
                    });
                }
                template    <typename T>
                void    Sequence_LinkedList<T>::Rep_::Remove (size_t from, size_t to)
                {
                    // quickie poor impl
                    // See Stroika v1 - much better - handling cases of remove near start or end of linked list
                    size_t index = from;
                    size_t amountToRemove = (to - from);
                    T tmphack;
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        for (Private::DataStructures::LinkedListMutator_Patch<T> it (fData_); it.More (&tmphack, true); ) {
                            if (index-- == 0) {
                                while (amountToRemove-- != 0) {
                                    it.RemoveCurrent ();
                                }
                                break;
                            }
                        }
                    });
                }


                /*
                ********************************************************************************
                *************************** Sequence_LinkedList<T> *****************************
                ********************************************************************************
                */
                template    <typename T>
                Sequence_LinkedList<T>::Sequence_LinkedList ()
                    : Sequence<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T>
                inline  Sequence_LinkedList<T>::Sequence_LinkedList (const CONTAINER_OF_T& s)
                    : Sequence<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    InsertAll (0, s);
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR>
                inline Sequence_LinkedList<T>::Sequence_LinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                    : Sequence<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Append (start, end);
                }
                template    <typename T>
                inline  Sequence_LinkedList<T>::Sequence_LinkedList (const Sequence_LinkedList<T>& s)
                    : Sequence<T> (s)
                {
                }
                template    <typename T>
                inline  Sequence_LinkedList<T>&   Sequence_LinkedList<T>::operator= (const Sequence_LinkedList<T>& s)
                {
                    Sequence<T>::operator= (s);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_ */

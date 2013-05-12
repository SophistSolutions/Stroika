/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/DataStructures/DoublyLinkedList.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Deque_DoublyLinkedList<T>::Rep_ : public Deque<T>::_IRep {
                private:
                    typedef typename    Deque<T>::_IRep  inherited;

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

                    // Queue<T>::_IRep overrides
                public:
                    virtual void        AddTail (T item) override;
                    virtual T           RemoveHead () override;
                    virtual T           Head () const override;
                    virtual void        RemoveAll () override;

                    // Deque<T>::_IRep overrides
                public:
                    virtual void        AddHead (T item) override;
                    virtual T           RemoveTail () override;
                    virtual T           Tail () const override;

                private:
                    Private::ContainerRepLockDataSupport_               fLockSupport_;
                    Private::DataStructures::DoublyLinkedList_Patch<T>  fData_;
                    friend  class Deque_DoublyLinkedList<T>::IteratorRep_;
                };


                template    <typename T>
                class  Deque_DoublyLinkedList<T>::IteratorRep_ : public Iterator<T>::IRep {
                public:
                    explicit IteratorRep_ (typename Deque_DoublyLinkedList<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override;
                    virtual bool                                More (T* current, bool advance) override;
                    virtual bool                                StrongEquals (const typename Iterator<T>::IRep* rhs) const override;

                private:
                    //mutable ForwardDoublyLinkedListMutator_Patch<T>    fIterator_;
                    mutable Private::DataStructures::DoublyLinkedListMutator_Patch<T>    fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ************** Deque_DoublyLinkedList<T>::IteratorRep_ **********************
                ********************************************************************************
                */
                template    <typename T>
                Deque_DoublyLinkedList<T>::IteratorRep_::IteratorRep_ (typename Deque_DoublyLinkedList<T>::Rep_& owner)
                    : Iterator<T>::IRep ()
                    , fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                bool    Deque_DoublyLinkedList<T>::IteratorRep_::More (T* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Deque_DoublyLinkedList<T>::IteratorRep_::StrongEquals (const typename Iterator<T>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                typename Iterator<T>::SharedIRepPtr  Deque_DoublyLinkedList<T>::IteratorRep_::Clone () const
                {
                    return typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*this));
                }


                /*
                ********************************************************************************
                ******************* Deque_DoublyLinkedList<T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Deque_DoublyLinkedList<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Deque_DoublyLinkedList<T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_ (from.fLockSupport_, {
                        fData_ = from.fData_;
                    });
                }
                template    <typename T>
                typename Iterable<T>::_SharedPtrIRep  Deque_DoublyLinkedList<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Deque_DoublyLinkedList<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<T> tmp = Iterator<T> (typename Iterator<T>::SharedByValueRepType (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                size_t  Deque_DoublyLinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.GetLength ());
                    });
                }
                template    <typename T>
                bool  Deque_DoublyLinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.GetLength () == 0);
                    });
                }
                template    <typename T>
                void      Deque_DoublyLinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Deque_DoublyLinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                void    Deque_DoublyLinkedList<T>::Rep_::AddTail (T item)
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.Append (item);
                    });
                }
                template    <typename T>
                T    Deque_DoublyLinkedList<T>::Rep_::RemoveHead ()
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        T   item =  fData_.GetFirst ();
                        fData_.RemoveFirst ();
                        return (item);
                    });
                }
                template    <typename T>
                T    Deque_DoublyLinkedList<T>::Rep_::Head () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.GetFirst ());
                    });
                }
                template    <typename T>
                void    Deque_DoublyLinkedList<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.RemoveAll ();
                    });
                }
                template    <typename T>
                void    Deque_DoublyLinkedList<T>::Rep_::AddHead (T item)
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        fData_.Append (item);
                    });
                }
                template    <typename T>
                T    Deque_DoublyLinkedList<T>::Rep_::RemoveTail ()
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        T   item =  fData_.GetFirst ();
                        fData_.RemoveLast ();
                        return (item);
                    });
                }
                template    <typename T>
                T    Deque_DoublyLinkedList<T>::Rep_::Tail () const
                {
                    CONTAINER_LOCK_HELPER_ (fLockSupport_, {
                        return (fData_.GetLast ());
                    });
                }


                /*
                ********************************************************************************
                ************************ Deque_DoublyLinkedList<T> *****************************
                ********************************************************************************
                */
                template    <typename T>
                Deque_DoublyLinkedList<T>::Deque_DoublyLinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>
                inline  Deque_DoublyLinkedList<T>::Deque_DoublyLinkedList (const Deque_DoublyLinkedList<T>& s)
                    : inherited (s)
                {
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T>
                inline  Deque_DoublyLinkedList<T>::Deque_DoublyLinkedList (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    InsertAll (0, s);
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR>
                inline Deque_DoublyLinkedList<T>::Deque_DoublyLinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Append (start, end);
                }
                template    <typename T>
                inline  Deque_DoublyLinkedList<T>&   Deque_DoublyLinkedList<T>::operator= (const Deque_DoublyLinkedList<T>& q)
                {
                    inherited::operator= (static_cast<const inherited&> (q));
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_inl_ */

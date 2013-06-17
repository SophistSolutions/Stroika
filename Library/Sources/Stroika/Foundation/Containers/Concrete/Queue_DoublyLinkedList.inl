/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/DoublyLinkedList.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Queue_DoublyLinkedList<T>::Rep_ : public Queue<T>::_IRep {
                private:
                    typedef typename    Queue<T>::_IRep  inherited;

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

                private:
                    Private::ContainerRepLockDataSupport_                       fLockSupport_;
                    Private::PatchingDataStructures::DoublyLinkedList_Patch<T>  fData_;
                    friend  class Queue_DoublyLinkedList<T>::IteratorRep_;
                };


                template    <typename T>
                class  Queue_DoublyLinkedList<T>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename    Iterator<T>::IRep   inherited;

                public:
                    explicit IteratorRep_ (typename Queue_DoublyLinkedList<T>::Rep_& owner)
                        : inherited ()
                        , fLockSupport_ (owner.fLockSupport_)
                        , fIterator_ (owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    More (T* current, bool advance) override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return (fIterator_.More (current, advance));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    Private::ContainerRepLockDataSupport_&                                      fLockSupport_;
                    mutable Private::PatchingDataStructures::DoublyLinkedListMutator_Patch<T>   fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ********************* Queue_DoublyLinkedList<T>::Rep_ **************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Queue_DoublyLinkedList<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Queue_DoublyLinkedList<T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                typename Iterable<T>::_SharedPtrIRep  Queue_DoublyLinkedList<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Queue_DoublyLinkedList<T>::Rep_::MakeIterator () const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<T> tmp = Iterator<T> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                size_t  Queue_DoublyLinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Queue_DoublyLinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Queue_DoublyLinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Queue_DoublyLinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                void    Queue_DoublyLinkedList<T>::Rep_::AddTail (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Append (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                T    Queue_DoublyLinkedList<T>::Rep_::RemoveHead ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        T   item =  fData_.GetFirst ();
                        fData_.RemoveFirst ();
                        return (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                T    Queue_DoublyLinkedList<T>::Rep_::Head () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetFirst ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Queue_DoublyLinkedList<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ************************ Queue_DoublyLinkedList<T> *****************************
                ********************************************************************************
                */
                template    <typename T>
                Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>
                inline  Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (const Queue_DoublyLinkedList<T>& q)
                    : inherited (static_cast<const inherited&> (q))
                {
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T>
                inline  Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertNotImplemented ();
                    //InsertAll (0, s);
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Append (start, end);
                }
                template    <typename T>
                inline  Queue_DoublyLinkedList<T>&   Queue_DoublyLinkedList<T>::operator= (const Queue_DoublyLinkedList<T>& q)
                {
                    inherited::operator= (static_cast<const inherited&> (q));
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_ */

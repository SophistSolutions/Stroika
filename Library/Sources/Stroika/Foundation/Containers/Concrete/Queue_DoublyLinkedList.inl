/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/DoublyLinkedList.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T, typename TRAITS>
                class   Queue_DoublyLinkedList<T, TRAITS>::Rep_ : public Queue<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    Queue<T, TRAITS>::_IRep;

                public:
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using   _IteratorOwnerID = typename inherited::_IteratorOwnerID;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override;
                    virtual Iterator<T>                             MakeIterator (_IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                                  GetLength () const override;
                    virtual bool                                    IsEmpty () const override;
                    virtual void                                    Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                             ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Queue<T, TRAITS>::_IRep overrides
                public:
                    virtual void                AddTail (T item) override;
                    virtual T                   RemoveHead () override;
                    virtual Memory::Optional<T> RemoveHeadIf () override;
                    virtual T                   Head () const override;
                    virtual Memory::Optional<T> HeadIf () const override;
                    virtual void                RemoveAll () override;

                private:
                    typedef Private::PatchingDataStructures::DoublyLinkedList<T>        DataStructureImplType_;
                    typedef Private::IteratorImplHelper_<T, DataStructureImplType_>     IteratorRep_;

                private:
                    Private::ContainerRepLockDataSupport_       fLockSupport_;
                    DataStructureImplType_                      fData_;
                };


                /*
                ********************************************************************************
                ***************** Queue_DoublyLinkedList<T, TRAITS>::Rep_ **********************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Queue_DoublyLinkedList<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Queue_DoublyLinkedList<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename Iterable<T>::_SharedPtrIRep  Queue_DoublyLinkedList<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Queue_DoublyLinkedList<T, TRAITS>::Rep_::MakeIterator (_IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (&NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                size_t  Queue_DoublyLinkedList<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Queue_DoublyLinkedList<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      Queue_DoublyLinkedList<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Queue_DoublyLinkedList<T, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                void    Queue_DoublyLinkedList<T, TRAITS>::Rep_::AddTail (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Append (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                T    Queue_DoublyLinkedList<T, TRAITS>::Rep_::RemoveHead ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        T   item =  fData_.GetFirst ();
                        fData_.RemoveFirst ();
                        return (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Memory::Optional<T>    Queue_DoublyLinkedList<T, TRAITS>::Rep_::RemoveHeadIf ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (fData_.IsEmpty ()) {
                            return Memory::Optional<T> ();
                        }
                        T   item =  fData_.GetFirst ();
                        fData_.RemoveFirst ();
                        return item;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                T    Queue_DoublyLinkedList<T, TRAITS>::Rep_::Head () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetFirst ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Memory::Optional<T>    Queue_DoublyLinkedList<T, TRAITS>::Rep_::HeadIf () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (fData_.IsEmpty ()) {
                            return Memory::Optional<T> ();
                        }
                        return fData_.GetFirst ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Queue_DoublyLinkedList<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ************************ Queue_DoublyLinkedList<T, TRAITS> *********************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Queue_DoublyLinkedList<T, TRAITS>::Queue_DoublyLinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename TRAITS>
                inline  Queue_DoublyLinkedList<T, TRAITS>::Queue_DoublyLinkedList (const Queue_DoublyLinkedList<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                inline  Queue_DoublyLinkedList<T, TRAITS>::Queue_DoublyLinkedList (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertNotImplemented ();        // @todo - use new EnqueueAll()
                    //InsertAll (0, s);
                }
                template    <typename T, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Queue_DoublyLinkedList<T, TRAITS>::Queue_DoublyLinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Append (start, end);
                }
                template    <typename T, typename TRAITS>
                inline  Queue_DoublyLinkedList<T, TRAITS>&   Queue_DoublyLinkedList<T, TRAITS>::operator= (const Queue_DoublyLinkedList<T, TRAITS>& rhs)
                {
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_ */

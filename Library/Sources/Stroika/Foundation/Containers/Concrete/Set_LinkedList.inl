/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_

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
                 *************************** Set_LinkedList<T>::Rep_ ****************************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Set_LinkedList<T>::Rep_ : public Set<T>::_IRep {
                private:
                    typedef typename    Set<T>::_IRep  inherited;

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

                    // Set<T>::_IRep overrides
                public:
                    virtual bool    Equals (const typename Set<T>::_IRep& rhs) const override;
                    virtual bool    Contains (T item) const override;
                    virtual void    RemoveAll () override;
                    virtual void    Add (T item) override;
                    virtual void    Remove (T item) override;
                    virtual void    Remove (const Iterator<T>& i) override;

                private:
                    Private::ContainerRepLockDataSupport_           fLockSupport_;
                    Private::DataStructures::LinkedList_Patch<T>    fData_;

                private:
                    friend  class Set_LinkedList<T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 **************************** Set_LinkedList<T>::IteratorRep_ *******************
                 ********************************************************************************
                 */
                template    <typename T>
                class  Set_LinkedList<T>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename Iterator<T>::IRep    inherited;

                public:
                    explicit IteratorRep_ (typename Set_LinkedList<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override;
                    virtual bool                                More (T* current, bool advance) override;
                    virtual bool                                StrongEquals (const typename Iterator<T>::IRep* rhs) const override;

                private:
                    mutable Private::DataStructures::LinkedListMutator_Patch<T>    fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ******************** Set_LinkedList<T>::IteratorRep_ ***************************
                ********************************************************************************
                */
                template    <typename T>
                Set_LinkedList<T>::IteratorRep_::IteratorRep_ (typename Set_LinkedList<T>::Rep_& owner)
                    : inherited ()
                    , fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                bool    Set_LinkedList<T>::IteratorRep_::More (T* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Set_LinkedList<T>::IteratorRep_::StrongEquals (const typename Iterator<T>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                typename Iterator<T>::SharedIRepPtr  Set_LinkedList<T>::IteratorRep_::Clone () const
                {
                    return typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*this));
                }


                /*
                ********************************************************************************
                ************************** Set_LinkedList<T>::Rep_ *****************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Set_LinkedList<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Set_LinkedList<T>::Rep_::Rep_ (const Rep_& from)
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
                typename Iterable<T>::_SharedPtrIRep  Set_LinkedList<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Set_LinkedList<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<T> tmp = Iterator<T> (typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                size_t  Set_LinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Set_LinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Set_LinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Set_LinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                bool    Set_LinkedList<T>::Rep_::Equals (const typename Set<T>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T>
                bool    Set_LinkedList<T>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (Private::DataStructures::LinkedListIterator_Patch<T> it (fData_); it.More (nullptr, true);) {
                            if (it.Current () == item) {
                                return true;
                            }
                        }
                        return false;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Set_LinkedList<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Set_LinkedList<T>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (Private::DataStructures::LinkedListIterator_Patch<T> it (fData_); it.More (nullptr, true);) {
                            if (it.Current () == item) {
                                return;
                            }
                        }
                        fData_.Append (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Set_LinkedList<T>::Rep_::Remove (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (Private::DataStructures::LinkedListMutator_Patch<T> it (fData_); it.More (nullptr, true);) {
                            if (it.Current () == item) {
                                it.RemoveCurrent ();
                                return;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Set_LinkedList<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Set_LinkedList<T>::IteratorRep_&       mir =   dynamic_cast<const typename Set_LinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ******************************** Set_LinkedList<T> *****************************
                ********************************************************************************
                */
                template    <typename T>
                Set_LinkedList<T>::Set_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                inline  Set_LinkedList<T>::Set_LinkedList (const Set_LinkedList<T>& m)
                    : inherited (m)
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T>
                inline  Set_LinkedList<T>::Set_LinkedList (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    this->AddAll (s);
                }
                template    <typename T>
                inline  Set_LinkedList<T>&   Set_LinkedList<T>::operator= (const Set_LinkedList<T>& m)
                {
                    inherited::operator= (m);
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }
                template    <typename T>
                inline  const typename Set_LinkedList<T>::Rep_&  Set_LinkedList<T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T>
                inline  typename Set_LinkedList<T>::Rep_&    Set_LinkedList<T>::GetRep_ ()
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
#endif /* _Stroika_Foundation_Containers_Concrete_Set_LinkedList_inl_ */

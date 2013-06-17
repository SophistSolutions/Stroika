/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/LinkedList.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ******************************* Bag_LinkedList<T>::Rep_ ************************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Bag_LinkedList<T>::Rep_ : public Bag<T>::_IRep {
                private:
                    typedef typename    Bag<T>::_IRep   inherited;

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

                    // Bag<T>::_IRep overrides
                public:
                    virtual bool    Equals (const typename Bag<T>::_IRep& rhs) const override;
                    virtual bool    Contains (T item) const override;
                    virtual size_t  TallyOf (T item) const override;
                    virtual void    Add (T item) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Remove (T item) override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    RemoveAll () override;

                private:
                    Private::ContainerRepLockDataSupport_                   fLockSupport_;
                    Private::PatchingDataStructures::LinkedList_Patch<T>    fData_;

                private:
                    friend  class   Bag_LinkedList<T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ************************* Bag_LinkedList<T>::IteratorRep_ **********************
                 ********************************************************************************
                 */
                // One rep for BOTH iterator and mutator - to save code - mutator just adds invisible functionality
                template    <typename T>
                class  Bag_LinkedList<T>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename    Iterator<T>::IRep   inherited;

                public:
                    explicit IteratorRep_ (typename Bag_LinkedList<T>::Rep_& owner)
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
                    Private::ContainerRepLockDataSupport_&                                  fLockSupport_;
                    mutable Private::PatchingDataStructures::LinkedListMutator_Patch<T>     fIterator_;

                private:
                    friend  class   Bag_LinkedList<T>::Rep_;
                };


                /*
                ********************************************************************************
                ****************************** Bag_LinkedList<T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Bag_LinkedList<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Bag_LinkedList<T>::Rep_::Rep_ (const Rep_& from)
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
                typename Iterable<T>::_SharedPtrIRep  Bag_LinkedList<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Bag_LinkedList<T>::Rep_::MakeIterator () const
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
                size_t  Bag_LinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Bag_LinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Bag_LinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Bag_LinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                bool    Bag_LinkedList<T>::Rep_::Equals (const typename Bag<T>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename  T>
                bool    Bag_LinkedList<T>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.Contains (item));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                size_t    Bag_LinkedList<T>::Rep_::TallyOf (T item) const
                {
                    return this->_TallyOf_Reference_Implementation (item);
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Prepend (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_LinkedList<T>::IteratorRep_&      mir =   dynamic_cast<const typename Bag_LinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.UpdateCurrent (newValue);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Remove (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Remove (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_LinkedList<T>::IteratorRep_&      mir =   dynamic_cast<const typename Bag_LinkedList<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ********************************* Bag_LinkedList<T> ****************************
                ********************************************************************************
                */
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    Add (start, end);
                }
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList (const Bag<T>& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    inherited::operator+= (src);
                }
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList (const Bag_LinkedList<T>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename T>
                inline  Bag_LinkedList<T>& Bag_LinkedList<T>::operator= (const Bag_LinkedList<T>& bag)
                {
                    inherited::operator= (bag);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_inl_ */

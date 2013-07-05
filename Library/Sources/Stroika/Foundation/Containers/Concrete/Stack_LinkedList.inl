/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_

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
                 ************************* Stack_LinkedList<T>::Rep_ ****************************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Stack_LinkedList<T>::Rep_ : public Stack<T>::_IRep {
                private:
                    typedef typename    Stack<T>::_IRep  inherited;

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

                    // Stack<T>::_IRep overrides
                public:
                    virtual void    Push (T item) override;
                    virtual T       Pop () override;
                    virtual T       Top () const override;
                    virtual void    RemoveAll () override;

                private:
                    Private::ContainerRepLockDataSupport_         fLockSupport_;
                    Private::PatchingDataStructures::LinkedList<T>  fData_;

                private:
                    friend  class Stack_LinkedList<T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ************************** Stack_LinkedList<T>::IteratorRep_ *******************
                 ********************************************************************************
                 */
                template    <typename T>
                class  Stack_LinkedList<T>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename Iterator<T>::IRep    inherited;

                public:
                    explicit IteratorRep_ (typename Stack_LinkedList<T>::Rep_& owner)
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
                    Private::ContainerRepLockDataSupport_&                                              fLockSupport_;
                    mutable typename Private::PatchingDataStructures::LinkedList<T>::ForwardIterator    fIterator_;

                private:
                    friend  class   Rep_;
                };



                /*
                ********************************************************************************
                ************************ Stack_LinkedList<T>::Rep_ *****************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Stack_LinkedList<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Stack_LinkedList<T>::Rep_::Rep_ (const Rep_& from)
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
                typename Iterable<T>::_SharedPtrIRep  Stack_LinkedList<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Stack_LinkedList<T>::Rep_::MakeIterator () const
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
                size_t  Stack_LinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Stack_LinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Stack_LinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Stack_LinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                void    Stack_LinkedList<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Stack_LinkedList<T>::Rep_::Push (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Append (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                T    Stack_LinkedList<T>::Rep_::Pop ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        T   result  =   fData_.GetFirst ();
                        fData_.RemoveFirst ();
                        // FIX/PATCH
                        return result;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                T    Stack_LinkedList<T>::Rep_::Top () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.GetFirst ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ****************************** Stack_LinkedList<T> *****************************
                ********************************************************************************
                */
                template    <typename T>
                Stack_LinkedList<T>::Stack_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                inline  Stack_LinkedList<T>::Stack_LinkedList (const Stack_LinkedList<T>& s)
                    : inherited (static_cast<const inherited&> (s))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                inline  Stack_LinkedList<T>&   Stack_LinkedList<T>::operator= (const Stack_LinkedList<T>& s)
                {
                    inherited::operator= (static_cast<const inherited&> (s));
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_ */

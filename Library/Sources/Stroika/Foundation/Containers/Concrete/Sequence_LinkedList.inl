/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/LinkedList.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T, typename TRAITS>
                class   Sequence_LinkedList<T, TRAITS>::Rep_ : public Sequence<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    Sequence<T, TRAITS>::_IRep;

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

                    // Sequence<T, TRAITS>::_IRep overrides
                public:
                    virtual T       GetAt (size_t i) const override;
                    virtual void    SetAt (size_t i, const T& item) override;
                    virtual size_t  IndexOf (const Iterator<T>& i) const override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Insert (size_t at, const T* from, const T* to) override;
                    virtual void    Remove (size_t from, size_t to) override;

                private:
                    typedef Private::PatchingDataStructures::LinkedList<T>                      DataStructureImplType_;
                    typedef typename Private::IteratorImplHelper_<T, DataStructureImplType_>    IteratorRep_;

                private:
                    Private::ContainerRepLockDataSupport_       fLockSupport_;
                    DataStructureImplType_                      fData_;
                };


                /*
                ********************************************************************************
                ******************* Sequence_LinkedList<T, TRAITS>::Rep_ ***********************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Sequence_LinkedList<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Sequence_LinkedList<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
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
                typename Iterable<T>::_SharedPtrIRep  Sequence_LinkedList<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Sequence_LinkedList<T, TRAITS>::Rep_::MakeIterator (_IteratorOwnerID suggestedOwner) const
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
                size_t  Sequence_LinkedList<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Sequence_LinkedList<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      Sequence_LinkedList<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Sequence_LinkedList<T, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                T    Sequence_LinkedList<T, TRAITS>::Rep_::GetAt (size_t i) const
                {
                    Require (not IsEmpty ());
                    Require (i == kBadSequenceIndex or i < GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (i == kBadSequenceIndex) {
                            i = GetLength () - 1;
                        }
                        return fData_.GetAt (i);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Sequence_LinkedList<T, TRAITS>::Rep_::SetAt (size_t i, const T& item)
                {
                    Require (i < GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.SetAt (item, i);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t    Sequence_LinkedList<T, TRAITS>::Rep_::IndexOf (const Iterator<T>& i) const
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return mir.fIterator.CurrentIndex ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Sequence_LinkedList<T, TRAITS>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Sequence_LinkedList<T, TRAITS>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.SetAt (mir.fIterator, newValue);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Sequence_LinkedList<T, TRAITS>::Rep_::Insert (size_t at, const T* from, const T* to)
                {
                    Require (at == kBadSequenceIndex or at <= GetLength ());
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
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
                            for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true); ) {
                                if (--index == 0) {
                                    for (const T* p = from; p != to; ++p) {
                                        fData_.AddBefore (it, *p);
                                    }
                                    break;
                                }
                            }
                            //Assert (not it.Done ());      // cuz that would mean we never added
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Sequence_LinkedList<T, TRAITS>::Rep_::Remove (size_t from, size_t to)
                {
                    // quickie poor impl
                    // See Stroika v1 - much better - handling cases of remove near start or end of linked list
                    size_t index = from;
                    size_t amountToRemove = (to - from);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename DataStructureImplType_::ForwardIterator it (&fData_); it.More (nullptr, true); ) {
                            if (index-- == 0) {
                                while (amountToRemove-- != 0) {
                                    fData_.RemoveAt (it);
                                }
                                break;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ************************* Sequence_LinkedList<T, TRAITS> ***********************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Sequence_LinkedList<T, TRAITS>::Sequence_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                inline  Sequence_LinkedList<T, TRAITS>::Sequence_LinkedList (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    this->AppendAll (s);
                }
                template    <typename T, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Sequence_LinkedList<T, TRAITS>::Sequence_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    this->AppendAll (start, end);
                }
                template    <typename T, typename TRAITS>
                inline  Sequence_LinkedList<T, TRAITS>::Sequence_LinkedList (const Sequence_LinkedList<T, TRAITS>& s)
                    : inherited (s)
                {
                }
                template    <typename T, typename TRAITS>
                inline  Sequence_LinkedList<T, TRAITS>&   Sequence_LinkedList<T, TRAITS>::operator= (const Sequence_LinkedList<T, TRAITS>& s)
                {
                    inherited::operator= (s);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_LinkedList_inl_ */

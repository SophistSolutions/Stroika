/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_

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


                /*
                 ********************************************************************************
                 ************ SortedCollection_LinkedList<T, TRAITS>::Rep_ **********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   SortedCollection_LinkedList<T, TRAITS>::Rep_ : public SortedCollection<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    SortedCollection<T, TRAITS>::_IRep;

                public:
                    using   _SharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      Clone () const override;
                    virtual Iterator<T>         MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t              GetLength () const override;
                    virtual bool                IsEmpty () const override;
                    virtual void                Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>         ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Collection<T>::_IRep overrides
                public:
                    virtual void    Add (T item) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    RemoveAll () override;
#if     qDebug
                    virtual void    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) override;
#endif

                    // SortedCollection<T>::_IRep overrides
                public:
                    virtual bool    Equals (const typename Collection<T>::_IRep& rhs) const override;
                    virtual bool    Contains (T item) const override;
                    virtual void    Remove (T item) override;

                private:
                    nonvirtual  void    AddWithoutLocks_ (T item);

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::LinkedList <T, Private::DataStructures::LinkedList_DefaultTraits<T, typename TRAITS::EqualsCompareFunctionType>>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;
                };


                /*
                ********************************************************************************
                *************** SortedCollection_LinkedList<T, TRAITS>::Rep_ *******************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  SortedCollection_LinkedList<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  SortedCollection_LinkedList<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
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
                typename Iterable<T>::_SharedPtrIRep  SortedCollection_LinkedList<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  SortedCollection_LinkedList<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                size_t  SortedCollection_LinkedList<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  SortedCollection_LinkedList<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      SortedCollection_LinkedList<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     SortedCollection_LinkedList<T, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                bool    SortedCollection_LinkedList<T, TRAITS>::Rep_::Equals (const typename Collection<T>::_IRep& rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                    //return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    SortedCollection_LinkedList<T, TRAITS>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.Lookup (item) != nullptr;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        AddWithoutLocks_ (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto     mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        // equals might examine a subset of the object and we still want to update the whole object, but
                        // if its not already equal, the sort order could have changed so we must simulate with a remove/add
                        if (TRAITS::EqualsCompareFunctionType::Equals (mir.fIterator.Current (), newValue)) {
                            fData_.SetAt (mir.fIterator, newValue);
                        }
                        else {
                            fData_.RemoveAt (mir.fIterator);
                            AddWithoutLocks_ (newValue);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::Remove (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Remove (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto      mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::AddWithoutLocks_ (T item)
                {
                    using   Traversal::kUnknownIteratorOwnerID;
                    typename Rep_::DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_);
                    // skip the smaller items
                    while (it.More (nullptr, true) and TRAITS::WellOrderCompareFunctionType::Compare (it.Current (), item) < 0) {
                    }
                    // at this point - we are pointing at the first link >= item, so insert before it
                    fData_.AddBefore (it, item);
                }


                /*
                ********************************************************************************
                ******************* SortedCollection_LinkedList<T, TRAITS> ****************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    this->AddAll (start, end);
                }
                template    <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const SortedCollection<T, TRAITS>& src)
                    : SortedCollection<T, TRAITS> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    this->AddAll (src);
                }
                template    <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const SortedCollection_LinkedList<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename T, typename TRAITS>
                inline  SortedCollection_LinkedList<T, TRAITS>& SortedCollection_LinkedList<T, TRAITS>::operator= (const SortedCollection_LinkedList<T, TRAITS>& rhs)
                {
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_ */

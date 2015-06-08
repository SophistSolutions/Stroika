/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                    using   _IterableSharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename Collection<T>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ () = default;
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<T>                 MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                      GetLength () const override;
                    virtual bool                        IsEmpty () const override;
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                 FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // Collection<T>::_IRep overrides
                public:
                    virtual typename Collection<T>::_SharedPtrIRep  CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual void                                    Add (T item) override;
                    virtual void                                    Update (const Iterator<T>& i, T newValue) override;
                    virtual void                                    Remove (const Iterator<T>& i) override;
#if     qDebug
                    virtual void                                    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
#endif

                    // SortedCollection<T>::_IRep overrides
                public:
                    virtual bool    Equals (const typename Collection<T>::_IRep& rhs) const override;
                    virtual bool    Contains (T item) const override;
                    virtual void    Remove (T item) override;

                private:
                    nonvirtual  void    AddWithoutLocks_ (T item);

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::LinkedList <T, Private::ContainerRepLockDataSupport_, ExternallySynchronizedDataStructures::LinkedList_DefaultTraits<T, typename TRAITS::EqualsCompareFunctionType>>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                ********************************************************************************
                *************** SortedCollection_LinkedList<T, TRAITS>::Rep_ *******************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  SortedCollection_LinkedList<T, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                typename SortedCollection_LinkedList<T, TRAITS>::Rep_::_IterableSharedPtrIRep  SortedCollection_LinkedList<T, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return _IterableSharedPtrIRep (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  SortedCollection_LinkedList<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        using   IT = Iterator<T>;
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        tmpRep = typename IT::SharedIRepPtr (IT::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ()));
#else
                        tmpRep = typename IT::SharedIRepPtr (IT::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
#endif
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                size_t  SortedCollection_LinkedList<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  SortedCollection_LinkedList<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      SortedCollection_LinkedList<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     SortedCollection_LinkedList<T, TRAITS>::Rep_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE =   Iterator<T>;
                    shared_ptr<IteratorRep_> resultRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        auto iLink = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        resultRep = shared_ptr<IteratorRep_> (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ()));
#else
                        resultRep = shared_ptr<IteratorRep_> (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_));
#endif
                        resultRep->fIterator.SetCurrentLink (iLink);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
#if         qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy
                    return RESULT_TYPE (typename Iterator<T>::SharedIRepPtr (resultRep));
#else
                    return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
#endif
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
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.Lookup (item) != nullptr;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename Collection<T>::_SharedPtrIRep  SortedCollection_LinkedList<T, TRAITS>::Rep_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
                {
                    if (fData_.HasActiveIterators ()) {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Traversal::IterableBase::SharedPtrImplementationTemplate<Rep_> (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    else {
                        return _SharedPtrIRep (new Rep_ ());
                    }
                }
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
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
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
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
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
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
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    SortedCollection_LinkedList<T, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
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
                ***************** SortedCollection_LinkedList<T, TRAITS> ***********************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (inherited::template MakeSharedPtr<Rep_> ()))
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    AssertRepValidType_ ();
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const SortedCollection<T, TRAITS>& src)
                    : SortedCollection<T, TRAITS> (typename inherited::_SharedPtrIRep (inherited::template MakeSharedPtr<Rep_> ()))
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                SortedCollection_LinkedList<T, TRAITS>::SortedCollection_LinkedList (const SortedCollection_LinkedList<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  void    SortedCollection_LinkedList<T, TRAITS>::AssertRepValidType_ () const
                {
                    AssertMember (&inherited::_ConstGetRep (), Rep_);
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_ */

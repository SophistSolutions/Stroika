/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_    1

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
                 ********************* MultiSet_LinkedList<T, TRAITS>::Rep_ *********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   MultiSet_LinkedList<T, TRAITS>::Rep_ : public MultiSet<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename MultiSet<T, TRAITS>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
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
                    virtual _IterableSharedPtrIRep              Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual size_t                              GetLength () const override;
                    virtual bool                                IsEmpty () const override;
                    virtual Iterator<MultiSetEntry<T>>          MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual void                                Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<MultiSetEntry<T>>          FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual _SharedPtrIRep                      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual bool                                Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                                Contains (ArgByValueType<T> item) const override;
                    virtual void                                Add (ArgByValueType<T> item, size_t count) override;
                    virtual void                                Remove (ArgByValueType<T> item, size_t count) override;
                    virtual void                                Remove (const Iterator<MultiSetEntry<T>>& i) override;
                    virtual void                                UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount) override;
                    virtual size_t                              OccurrencesOf (ArgByValueType<T> item) const override;
                    virtual Iterable<T>                         Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override;
                    virtual Iterable<T>                         UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override;
#if     qDebug
                    virtual void                                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
#endif

                private:
                    using   NonPatchingDataStructureImplType_   =   ExternallySynchronizedDataStructures::LinkedList<MultiSetEntry<T>>;
                    using   DataStructureImplType_              =   Private::PatchingDataStructures::LinkedList<MultiSetEntry<T>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_                        =   typename Private::IteratorImplHelper_<MultiSetEntry<T>, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ********************* MultiSet_LinkedList<T, TRAITS>::Rep_ *********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  MultiSet_LinkedList<T, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                size_t  MultiSet_LinkedList<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  MultiSet_LinkedList<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.IsEmpty ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<MultiSetEntry<T>> MultiSet_LinkedList<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<MultiSetEntry<T>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        tmpRep = Iterator<MultiSetEntry<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ());
#else
                        tmpRep = Iterator<MultiSetEntry<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
#endif
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<MultiSetEntry<T>> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                void      MultiSet_LinkedList<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<MultiSetEntry<T>>     MultiSet_LinkedList<T, TRAITS>::Rep_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE =   Iterator<MultiSetEntry<T>>;
                    shared_ptr<IteratorRep_> resultRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        auto iLink = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        resultRep = Iterator<MultiSetEntry<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ());
#else
                        resultRep = Iterator<MultiSetEntry<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
#endif
                        resultRep->fIterator.SetCurrentLink (iLink);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                    return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                }
                template    <typename T, typename TRAITS>
                typename MultiSet_LinkedList<T, TRAITS>::Rep_::_SharedPtrIRep  MultiSet_LinkedList<T, TRAITS>::Rep_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
                {
                    if (fData_.HasActiveIterators ()) {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<MultiSetEntry<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    else {
                        return Iterable<MultiSetEntry<T>>::template MakeSharedPtr<Rep_> ();
                    }
                }
                template    <typename T, typename TRAITS>
                bool    MultiSet_LinkedList<T, TRAITS>::Rep_::Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool   MultiSet_LinkedList<T, TRAITS>::Rep_::Contains (ArgByValueType<T> item) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        MultiSetEntry<T>   c = item;
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (&c, true); ) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (c.fItem, item)) {
                                Assert (c.fCount != 0);
                                return (true);
                            }
                        }
                        return (false);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename MultiSet_LinkedList<T, TRAITS>::Rep_::_IterableSharedPtrIRep   MultiSet_LinkedList<T, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<MultiSetEntry<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void   MultiSet_LinkedList<T, TRAITS>::Rep_::Add (ArgByValueType<T> item, size_t count)
                {
                    using   Traversal::kUnknownIteratorOwnerID;
                    if (count != 0) {
                        MultiSetEntry<T>   current (item);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (&current, true); ) {
                                if (TRAITS::EqualsCompareFunctionType::Equals (current.fItem, item)) {
                                    current.fCount += count;
                                    fData_.SetAt (it, current);
                                    return;
                                }
                            }
                            fData_.Prepend (MultiSetEntry<T> (item, count));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                }
                template    <typename T, typename TRAITS>
                void   MultiSet_LinkedList<T, TRAITS>::Rep_::Remove (ArgByValueType<T> item, size_t count)
                {
                    using   Traversal::kUnknownIteratorOwnerID;
                    if (count != 0) {
                        MultiSetEntry<T>   current (item);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (&current, true); ) {
                                if (TRAITS::EqualsCompareFunctionType::Equals (current.fItem, item)) {
                                    if (current.fCount > count) {
                                        current.fCount -= count;
                                    }
                                    else {
                                        current.fCount = 0;     // Should this be an underflow excpetion, assertion???
                                    }
                                    if (current.fCount == 0) {
                                        fData_.RemoveAt (it);
                                    }
                                    else {
                                        fData_.SetAt (it, current);
                                    }
                                    break;
                                }
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_LinkedList<T, TRAITS>::Rep_::Remove (const Iterator<MultiSetEntry<T>>& i)
                {
                    const typename Iterator<MultiSetEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_LinkedList<T, TRAITS>::Rep_::UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<MultiSetEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        if (newCount == 0) {
                            fData_.RemoveAt (mir.fIterator);
                        }
                        else {
                            MultiSetEntry<T>   c   =   mir.fIterator.Current ();
                            c.fCount = newCount;
                            fData_.SetAt (mir.fIterator, c);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t MultiSet_LinkedList<T, TRAITS>::Rep_::OccurrencesOf (ArgByValueType<T> item) const
                {
                    MultiSetEntry<T>   c = item;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (&c, true); ) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (c.fItem, item)) {
                                Ensure (c.fCount != 0);
                                return c.fCount;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return 0;
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    MultiSet_LinkedList<T, TRAITS>::Rep_::Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_Elements_Reference_Implementation (rep);
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    MultiSet_LinkedList<T, TRAITS>::Rep_::UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_UniqueElements_Reference_Implementation (rep);
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    MultiSet_LinkedList<T, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                 ********************************************************************************
                 ************************ MultiSet_LinkedList<T, TRAITS> ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const T* start, const T* end)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const MultiSet<T, TRAITS>& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const MultiSet_LinkedList<T, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<T>& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<MultiSetEntry<T>>& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  void    MultiSet_LinkedList<T, TRAITS>::AssertRepValidType_ () const
                {
                    AssertMember (&inherited::_ConstGetRep (), Rep_);
                }


            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_ */

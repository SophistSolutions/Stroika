/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
                 ************** MultiSet_LinkedList<T, TRAITS>::Rep_InternalSync_ ***************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   MultiSet_LinkedList<T, TRAITS>::Rep_InternalSync_ : public MultiSet<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename MultiSet<T, TRAITS>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<CountedValue<T>>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using   CounterType = typename inherited::CounterType;

                public:
                    Rep_InternalSync_ () = default;
                    Rep_InternalSync_ (const Rep_InternalSync_& from) = delete;
                    Rep_InternalSync_ (Rep_InternalSync_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual  Rep_InternalSync_& operator= (const Rep_InternalSync_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_InternalSync_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep              Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual size_t                              GetLength () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return (fData_.GetLength ());
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                                IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.IsEmpty ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterator<CountedValue<T>>           MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<CountedValue<T>>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return Iterator<CountedValue<T>> (tmpRep);
                    }
                    virtual void                                Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                            // use iterator (which currently implies lots of locks) with this->_Apply ()
                            fData_.Apply (doToElement);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterator<CountedValue<T>>           FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using   RESULT_TYPE     =   Iterator<CountedValue<T>>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        SHARED_REP_TYPE resultRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            auto iLink = fData_.FindFirstThat (doToElement);
                            if (iLink == nullptr) {
                                return RESULT_TYPE::GetEmptyIterator ();
                            }
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetCurrentLink (iLink);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual _SharedPtrIRep                      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                                r->fData_.RemoveAll ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_InternalSync_> ();
                        }
                    }
                    virtual bool                                Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual bool                                Contains (ArgByValueType<T> item) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            CountedValue<T>   c = item;
                            for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (&c, true); ) {
                                if (TRAITS::EqualsCompareFunctionType::Equals (c.fValue, item)) {
                                    Assert (c.fCount != 0);
                                    return (true);
                                }
                            }
                            return (false);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                                Add (ArgByValueType<T> item, CounterType count) override
                    {
                        using   Traversal::kUnknownIteratorOwnerID;
                        if (count != 0) {
                            CountedValue<T>   current (item);
                            CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                                for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (&current, true); ) {
                                    if (TRAITS::EqualsCompareFunctionType::Equals (current.fValue, item)) {
                                        current.fCount += count;
                                        fData_.SetAt (it, current);
                                        return;
                                    }
                                }
                                fData_.Prepend (CountedValue<T> (item, count));
                            }
                            CONTAINER_LOCK_HELPER_END ();
                        }
                    }
                    virtual void                                Remove (ArgByValueType<T> item, CounterType count) override
                    {
                        using   Traversal::kUnknownIteratorOwnerID;
                        if (count != 0) {
                            CountedValue<T>   current (item);
                            CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                                for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (&current, true); ) {
                                    if (TRAITS::EqualsCompareFunctionType::Equals (current.fValue, item)) {
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
                    virtual void                                Remove (const Iterator<CountedValue<T>>& i) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.RemoveAt (mir.fIterator);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                                UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            if (newCount == 0) {
                                fData_.RemoveAt (mir.fIterator);
                            }
                            else {
                                CountedValue<T>   c   =   mir.fIterator.Current ();
                                c.fCount = newCount;
                                fData_.SetAt (mir.fIterator, c);
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual CounterType                         OccurrencesOf (ArgByValueType<T> item) const override
                    {
                        CountedValue<T>   c = item;
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (&c, true); ) {
                                if (TRAITS::EqualsCompareFunctionType::Equals (c.fValue, item)) {
                                    Ensure (c.fCount != 0);
                                    return c.fCount;
                                }
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                        return 0;
                    }
                    virtual Iterable<T>                         Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override
                    {
                        return this->_Elements_Reference_Implementation (rep);
                    }
                    virtual Iterable<T>                         UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override
                    {
                        return this->_UniqueElements_Reference_Implementation (rep);
                    }
#if     qDebug
                    virtual void                                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#endif

                private:
                    using   NonPatchingDataStructureImplType_   =   DataStructures::LinkedList<CountedValue<T>>;
                    using   DataStructureImplType_              =   Private::PatchingDataStructures::LinkedList<CountedValue<T>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_                        =   typename Private::IteratorImplHelper_<CountedValue<T>, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ************************ MultiSet_LinkedList<T, TRAITS> ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_InternalSync_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const T* start, const T* end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const MultiSet<T, TRAITS>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_LinkedList (containerUpdateSafetyPolicy)
                {
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
                inline  MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<CountedValue<T>>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_LinkedList (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  void    MultiSet_LinkedList<T, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_ */

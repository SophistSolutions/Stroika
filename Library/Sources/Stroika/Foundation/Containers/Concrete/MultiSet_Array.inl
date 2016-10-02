/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_Array_inl_
#define _Stroika_Foundation_Containers_MultiSet_Array_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/Array.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                using   Traversal::IteratorOwnerID;


                /*
                 ********************************************************************************
                 ************************** MultiSet_Array<T, TRAITS>::Rep_ *********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   MultiSet_Array<T, TRAITS>::Rep_ : public MultiSet<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename MultiSet<T, TRAITS>::_IRep;
                public:
                    using   _IterableSharedPtrIRep = typename Iterable<CountedValue<T>>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using   CounterType = typename inherited::CounterType;

                public:
                    Rep_ () = default;
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual  Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep          Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual size_t                          GetLength () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.GetLength ();
                    }
                    virtual bool                            IsEmpty () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.GetLength () == 0;
                    }
                    virtual Iterator<CountedValue<T>>       MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        // const cast cuz this mutator won't really be used to change anything - except stuff like
                        // link list of owned iterators
                        typename Iterator<CountedValue<T>>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return Iterator<CountedValue<T>> (tmpRep);
                    }
                    virtual void                            Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    virtual Iterator<CountedValue<T>>       FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using   RESULT_TYPE     =   Iterator<CountedValue<T>>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        SHARED_REP_TYPE resultRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            size_t i = fData_.FindFirstThat (doToElement);
                            if (i == fData_.GetLength ()) {
                                return RESULT_TYPE::GetEmptyIterator ();
                            }
                            Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<CountedValue<T>>::template MakeSharedPtr <IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetIndex (i);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual _SharedPtrIRep                          CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                                r->fData_.RemoveAll ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> ();
                        }
                    }
                    virtual bool                                    Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual bool                                    Contains (ArgByValueType<T> item) const override
                    {
                        CountedValue<T> tmp (item);
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return (bool (Find_ (tmp) != kNotFound_));
                    }
                    virtual void                                    Add (ArgByValueType<T> item, CounterType count) override
                    {
                        CountedValue<T> tmp (item, count);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        size_t index = Find_ (tmp);
                        if (index == kNotFound_) {
                            fData_.InsertAt (fData_.GetLength (), tmp);
                        }
                        else {
                            tmp.fCount += count;
                            fData_.SetAt (index, tmp);
                        }
                    }
                    virtual void                                    Remove (ArgByValueType<T> item, CounterType count) override
                    {
                        CountedValue<T> tmp (item);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        size_t index = Find_ (tmp);
                        if (index != kNotFound_) {
                            Assert (index < fData_.GetLength ());
                            Assert (tmp.fCount >= count);
                            tmp.fCount -= count;
                            if (tmp.fCount == 0) {
                                fData_.RemoveAt (index);
                            }
                            else {
                                fData_.SetAt (index, tmp);
                            }
                        }
                    }
                    virtual void                                    Remove (const Iterator<CountedValue<T>>& i) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        fData_.RemoveAt (mir.fIterator);
                    }
                    virtual void                                    UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        if (newCount == 0) {
                            fData_.RemoveAt (mir.fIterator);
                        }
                        else {
                            CountedValue<T>   c   =   mir.fIterator.Current ();
                            c.fCount = newCount;
                            fData_.SetAt (mir.fIterator, c);
                        }
                    }
                    virtual CounterType                             OccurrencesOf (ArgByValueType<T> item) const override
                    {
                        CountedValue<T> tmp (item);
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        size_t index = Find_ (tmp);
                        if (index == kNotFound_) {
                            return 0;
                        }
                        Assert (index >= 0);
                        Assert (index < fData_.GetLength ());
                        return fData_[index].fCount;
                    }
                    virtual Iterable<T>                             Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override
                    {
                        return this->_Elements_Reference_Implementation (rep);
                    }
                    virtual Iterable<T>                             UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override
                    {
                        return this->_UniqueElements_Reference_Implementation (rep);
                    }
#if     qDebug
                    virtual void                                    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                    // MultiSet_Array<T, TRAITS>::_IRep overrides
                public:
                    nonvirtual void                                 Compact ()
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.Compact ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::Array<CountedValue<T>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<CountedValue<T>, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;

                private:
                    static  constexpr size_t    kNotFound_ = (size_t) - 1;

                private:
                    nonvirtual  size_t  Find_ (CountedValue<T>& item) const
                    {
                        // this code assumes locking done by callers
                        size_t length = fData_.GetLength ();
                        for (size_t i = 0; i < length; i++) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (fData_.GetAt (i).fValue, item.fValue)) {
                                item = fData_.GetAt (i);
                                return i;
                            }
                        }
                        return kNotFound_;
                    }

                private:
                    friend  class   MultiSet_Array<T, TRAITS>;
                };


                /*
                 ********************************************************************************
                 ***************************** MultiSet_Array<T, TRAITS> ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  MultiSet_Array<T, TRAITS>::MultiSet_Array (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  MultiSet_Array<T, TRAITS>::MultiSet_Array (const T* start, const T* end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_Array (containerUpdateSafetyPolicy)
                {
                    SetCapacity (end - start);
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  MultiSet_Array<T, TRAITS>::MultiSet_Array (const MultiSet_Array<T, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  MultiSet_Array<T, TRAITS>::MultiSet_Array (const MultiSet<T, TRAITS>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_Array (containerUpdateSafetyPolicy)
                {
                    SetCapacity (src.GetLength ());
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_Array (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<CountedValue<T>>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_Array (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  MultiSet_Array<T, TRAITS>& MultiSet_Array<T, TRAITS>::operator= (const MultiSet_Array<T, TRAITS>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename T, typename TRAITS>
                inline  size_t  MultiSet_Array<T, TRAITS>::GetCapacity () const
                {
                    using   _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_>;
                    _SafeReadRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        return accessor._ConstGetRep ().fData_.GetCapacity ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  void    MultiSet_Array<T, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  void    MultiSet_Array<T, TRAITS>::Compact ()
                {
                    using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        accessor._GetWriteableRep ().Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  void    MultiSet_Array<T, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}


#endif  /* _Stroika_Foundation_Containers_MultiSet_Array_inl_ */



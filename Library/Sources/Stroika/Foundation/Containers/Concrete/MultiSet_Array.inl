/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                    virtual _IterableSharedPtrIRep          Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual size_t                          GetLength () const override;
                    virtual bool                            IsEmpty () const override;
                    virtual Iterator<MultiSetEntry<T>>      MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual void                            Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<MultiSetEntry<T>>      FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual _SharedPtrIRep                          CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual bool                                    Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                                    Contains (ArgByValueType<T> item) const override;
                    virtual void                                    Add (ArgByValueType<T> item, size_t count) override;
                    virtual void                                    Remove (ArgByValueType<T> item, size_t count) override;
                    virtual void                                    Remove (const Iterator<MultiSetEntry<T>>& i) override;
                    virtual void                                    UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount) override;
                    virtual size_t                                  OccurrencesOf (ArgByValueType<T> item) const override;
                    virtual Iterable<T>                             Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override;
                    virtual Iterable<T>                             UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override;
#if     qDebug
                    virtual void                                    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
#endif

                    // MultiSet_Array<T, TRAITS>::_IRep overrides
                public:
                    nonvirtual void                                 Compact ();

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::Array<MultiSetEntry<T>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<MultiSetEntry<T>, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;

                private:
                    DEFINE_CONSTEXPR_CONSTANT(size_t, kNotFound_, (size_t) - 1);

                    nonvirtual  size_t  Find_ (MultiSetEntry<T>& item) const;

                private:
                    friend  class   MultiSet_Array<T, TRAITS>;
                };


                /*
                 ********************************************************************************
                 *********************** MultiSet_Array<T, TRAITS>::Rep_ ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  MultiSet_Array<T, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                size_t  MultiSet_Array<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.GetLength ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  MultiSet_Array<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<MultiSetEntry<T>> MultiSet_Array<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    // const cast cuz this mutator won't really be used to change anything - except stuff like
                    // link list of owned iterators
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
                void      MultiSet_Array<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<MultiSetEntry<T>>     MultiSet_Array<T, TRAITS>::Rep_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE     =   Iterator<MultiSetEntry<T>>;
                    using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                    SHARED_REP_TYPE resultRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        size_t i = fData_.FindFirstThat (doToElement);
                        if (i == fData_.GetLength ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        resultRep = SHARED_REP_TYPE (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ()));
#else
                        resultRep = SHARED_REP_TYPE (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_));
#endif
                        resultRep->fIterator.SetIndex (i);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                    return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                }
                template    <typename T, typename TRAITS>
                typename MultiSet_Array<T, TRAITS>::Rep_::_SharedPtrIRep  MultiSet_Array<T, TRAITS>::Rep_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
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
                bool    MultiSet_Array<T, TRAITS>::Rep_::Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    MultiSet_Array<T, TRAITS>::Rep_::Contains (ArgByValueType<T> item) const
                {
                    MultiSetEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (bool (Find_ (tmp) != kNotFound_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_Array<T, TRAITS>::Rep_::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename MultiSet_Array<T, TRAITS>::Rep_::_IterableSharedPtrIRep    MultiSet_Array<T, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<MultiSetEntry<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_Array<T, TRAITS>::Rep_::Add (ArgByValueType<T> item, size_t count)
                {
                    MultiSetEntry<T> tmp (item, count);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        size_t index = Find_ (tmp);
                        if (index == kNotFound_) {
                            fData_.InsertAt (fData_.GetLength (), tmp);
                        }
                        else {
                            tmp.fCount += count;
                            fData_.SetAt (index, tmp);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_Array<T, TRAITS>::Rep_::Remove (ArgByValueType<T> item, size_t count)
                {
                    MultiSetEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
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
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_Array<T, TRAITS>::Rep_::Remove (const Iterator<MultiSetEntry<T>>& i)
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
                void    MultiSet_Array<T, TRAITS>::Rep_::UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount)
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
                size_t  MultiSet_Array<T, TRAITS>::Rep_::OccurrencesOf (ArgByValueType<T> item) const
                {
                    MultiSetEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        size_t index = Find_ (tmp);
                        if (index == kNotFound_) {
                            return 0;
                        }
                        Assert (index >= 0);
                        Assert (index < fData_.GetLength ());
                        return fData_[index].fCount;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    MultiSet_Array<T, TRAITS>::Rep_::Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_Elements_Reference_Implementation (rep);
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    MultiSet_Array<T, TRAITS>::Rep_::UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_UniqueElements_Reference_Implementation (rep);
                }
                template    <typename T, typename TRAITS>
                size_t  MultiSet_Array<T, TRAITS>::Rep_::Find_ (MultiSetEntry<T>& item) const
                {
                    // this code assumes locking done by callers
                    size_t length = fData_.GetLength ();
                    for (size_t i = 0; i < length; i++) {
                        if (TRAITS::EqualsCompareFunctionType::Equals (fData_.GetAt (i).fItem, item.fItem)) {
                            item = fData_.GetAt (i);
                            return (i);
                        }
                    }
                    return kNotFound_;
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    MultiSet_Array<T, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                 ********************************************************************************
                 ***************************** MultiSet_Array<T, TRAITS> ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                MultiSet_Array<T, TRAITS>::MultiSet_Array ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_Array<T, TRAITS>::MultiSet_Array (const T* start, const T* end)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
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
                MultiSet_Array<T, TRAITS>::MultiSet_Array (const MultiSet<T, TRAITS>& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    SetCapacity (src.GetLength ());
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<T>& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<MultiSetEntry<T>>& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
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
                    using   _SafeReadRepAccessor = typename Iterable<T>::template _SafeReadRepAccessor<Rep_>;
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
                    AssertMember (&inherited::_ConstGetRep (), Rep_);
                }


            }
        }
    }
}


#endif  /* _Stroika_Foundation_Containers_MultiSet_Array_inl_ */



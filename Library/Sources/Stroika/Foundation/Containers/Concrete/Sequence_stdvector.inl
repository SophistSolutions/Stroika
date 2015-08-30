/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <vector>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Sequence_stdvector<T>::Rep_ : public Sequence<T>::_IRep {
                private:
                    using   inherited   =   typename    Sequence<T>::_IRep;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
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
                    virtual _IterableSharedPtrIRep  Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<T>             MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                  GetLength () const override;
                    virtual bool                    IsEmpty () const override;
                    virtual void                    Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>             FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // Sequence<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual T                   GetAt (size_t i) const override;
                    virtual void                SetAt (size_t i, ArgByValueType<T> item) override;
                    virtual size_t              IndexOf (const Iterator<T>& i) const override;
                    virtual void                Remove (const Iterator<T>& i) override;
                    virtual void                Update (const Iterator<T>& i, ArgByValueType<T> newValue) override;
                    virtual void                Insert (size_t at, const T* from, const T* to) override;
                    virtual void                Remove (size_t from, size_t to) override;
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
#endif

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::STLContainerWrapper<vector<T>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_  fData_;
                };


                /*
                ********************************************************************************
                ************************* Sequence_stdvector<T>::Rep_ **************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Sequence_stdvector<T>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T>
                typename Sequence_stdvector<T>::Rep_::_IterableSharedPtrIRep  Sequence_stdvector<T>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                Iterator<T>  Sequence_stdvector<T>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        tmpRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ());
#else
                        tmpRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
#endif
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T>
                size_t  Sequence_stdvector<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.size ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Sequence_stdvector<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.empty ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Sequence_stdvector<T>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                Iterator<T>     Sequence_stdvector<T>::Rep_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE =   Iterator<T>;
                    shared_ptr<IteratorRep_> resultRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        auto iLink = fData_.FindFirstThat (doToElement);
                        if (iLink == fData_.end ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ());
#else
                        resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
#endif
                        resultRep->fIterator.SetCurrentLink (iLink);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                    return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                }
                template    <typename T>
                typename Sequence_stdvector<T>::Rep_::_SharedPtrIRep  Sequence_stdvector<T>::Rep_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
                {
                    if (fData_.HasActiveIterators ()) {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.clear_WithPatching ();
                            return r;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    else {
                        return Iterable<T>::template MakeSharedPtr<Rep_> ();
                    }
                }
                template    <typename T>
                T    Sequence_stdvector<T>::Rep_::GetAt (size_t i) const
                {
                    Require (not IsEmpty ());
                    Require (i == kBadSequenceIndex or i < GetLength ());
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        if (i == kBadSequenceIndex) {
                            i = fData_.size () - 1;
                        }
                        return fData_[i];
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::SetAt (size_t i, ArgByValueType<T> item)
                {
                    Require (i < GetLength ());
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_[i] = item;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                size_t    Sequence_stdvector<T>::Rep_::IndexOf (const Iterator<T>& i) const
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return mir.fIterator.CurrentIndex ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        mir.fIterator.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::Update (const Iterator<T>& i, ArgByValueType<T> newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.Invariant ();
                        *mir.fIterator.fStdIterator = newValue;
                        fData_.Invariant ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::Insert (size_t at, const T* from, const T* to)
                {
                    Require (at == kBadSequenceIndex or at <= GetLength ());
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        if (at == kBadSequenceIndex) {
                            at = fData_.size ();
                        }
                        // quickie poor impl. Could do save / patch once, not multiple times...
                        {
                            size_t  capacity    { ReserveSpeedTweekAddNCapacity (fData_, to - from) };
                            if (capacity != static_cast<size_t> (-1)) {
                                Memory::SmallStackBuffer<size_t>    patchOffsets (0);
                                fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (&patchOffsets);
                                fData_.reserve (capacity);
                                if (patchOffsets.GetSize () != 0) {
                                    fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (patchOffsets);
                                }
                            }
                        }
                        for (auto i = from; i != to; ++i) {
                            fData_.insert_toVector_WithPatching (fData_.begin () + at, *i);
                            at++;
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::Remove (size_t from, size_t to)
                {
                    // quickie poor impl (patch once, not multiple times...)
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        for (size_t i = from; i < to; ++i) {
                            fData_.erase_WithPatching (fData_.begin () + from);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     qDebug
                template    <typename T>
                void    Sequence_stdvector<T>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                ********************************************************************************
                ******************************* Sequence_stdvector<T> **************************
                ********************************************************************************
                */
                template    <typename T>
                Sequence_stdvector<T>::Sequence_stdvector ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Sequence_stdvector<T>::Sequence_stdvector (const Sequence_stdvector<T>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T, typename ENABLE_IF>
                inline  Sequence_stdvector<T>::Sequence_stdvector (const CONTAINER_OF_T& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AppendAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Sequence_stdvector<T>::Sequence_stdvector (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AppendAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Sequence_stdvector<T>&   Sequence_stdvector<T>::operator= (const Sequence_stdvector<T>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (rhs);
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename T>
                inline  void    Sequence_stdvector<T>::Compact ()
                {
                    using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        if (accessor._ConstGetRep ().fData_.capacity () != accessor._ConstGetRep ().fData_.size ()) {
                            Memory::SmallStackBuffer<size_t>    patchOffsets (0);
                            accessor._GetWriteableRep ().fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (&patchOffsets);
                            accessor._GetWriteableRep ().fData_.reserve (accessor._ConstGetRep ().fData_.size ());
                            if (patchOffsets.GetSize () != 0) {
                                accessor._GetWriteableRep ().fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (patchOffsets);
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  size_t  Sequence_stdvector<T>::GetCapacity () const
                {
                    using   _SafeReadRepAccessor = typename Iterable<T>::template _SafeReadRepAccessor<Rep_>;
                    _SafeReadRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        return accessor._ConstGetRep ().fData_.capacity ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  void    Sequence_stdvector<T>::SetCapacity (size_t slotsAlloced)
                {
                    using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        if (accessor._ConstGetRep ().fData_.capacity () != slotsAlloced) {
                            Memory::SmallStackBuffer<size_t>    patchOffsets (0);
                            accessor._GetWriteableRep ().fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass1 (&patchOffsets);
                            accessor._GetWriteableRep ().fData_.reserve (slotsAlloced);
                            if (patchOffsets.GetSize () != 0) {
                                accessor._GetWriteableRep ().fData_.TwoPhaseIteratorPatcherAll2FromOffsetsPass2 (patchOffsets);
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  void    Sequence_stdvector<T>::AssertRepValidType_ () const
                {
                    AssertMember (&inherited::_ConstGetRep (), Rep_);
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_ */

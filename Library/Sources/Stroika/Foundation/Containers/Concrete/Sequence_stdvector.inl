/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                    using   _SharedPtrIRep   = typename Iterable<T>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<T>         MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t              GetLength () const override;
                    virtual bool                IsEmpty () const override;
                    virtual void                Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>         FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // Sequence<T>::_IRep overrides
                public:
                    virtual T       GetAt (size_t i) const override;
                    virtual void    SetAt (size_t i, const T& item) override;
                    virtual size_t  IndexOf (const Iterator<T>& i) const override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Insert (size_t at, const T* from, const T* to) override;
                    virtual void    Remove (size_t from, size_t to) override;
#if     qDebug
                    virtual void    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
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
                inline  Sequence_stdvector<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Sequence_stdvector<T>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T>
                typename Iterable<T>::_SharedPtrIRep  Sequence_stdvector<T>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return _SharedPtrIRep (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                Iterator<T>  Sequence_stdvector<T>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorHoldsSharedPtr_
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ()));
#else
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_));
#endif
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T>
                size_t  Sequence_stdvector<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Sequence_stdvector<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.empty ());
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
#if     qStroika_Foundation_Traveral_IteratorHoldsSharedPtr_
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
                void    Sequence_stdvector<T>::Rep_::SetAt (size_t i, const T& item)
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
                void    Sequence_stdvector<T>::Rep_::Update (const Iterator<T>& i, T newValue)
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
                        // quickie poor impl
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
                    // quickie poor impl
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
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                inline  Sequence_stdvector<T>::Sequence_stdvector (const Sequence_stdvector<T>& s)
                    : inherited (static_cast<const inherited&> (s))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T>
                inline  Sequence_stdvector<T>::Sequence_stdvector (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AppendAll (s);
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Sequence_stdvector<T>::Sequence_stdvector (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AppendAll (start, end);
                }
                template    <typename T>
                inline  Sequence_stdvector<T>&   Sequence_stdvector<T>::operator= (const Sequence_stdvector<T>& s)
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    inherited::operator= (s);
                    return *this;
                }
                template    <typename T>
                inline  const typename Sequence_stdvector<T>::Rep_&  Sequence_stdvector<T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T>
                inline  typename Sequence_stdvector<T>::Rep_&    Sequence_stdvector<T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T>
                inline  void    Sequence_stdvector<T>::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fData_.fLockSupport) {
                        GetRep_ ().fData_.reserve (GetRep_ ().fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  size_t  Sequence_stdvector<T>::GetCapacity () const
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fData_.fLockSupport) {
                        return (GetRep_ ().fData_.capacity ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  void    Sequence_stdvector<T>::SetCapacity (size_t slotsAlloced)
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fData_.fLockSupport) {
                        GetRep_ ().fData_.reserve (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_inl_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Memory/BlockAllocated.h"
#include    "../Common.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/Array.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                using   Traversal::IteratorOwnerID;


                /*
                 */
                template    <typename T>
                class   Sequence_Array<T>::IImplRep_ : public Sequence<T>::_IRep {
                private:
                    using   inherited   =   typename Sequence<T>::_IRep;

                public:
                    virtual void    Compact () = 0;
                    virtual size_t  GetCapacity () const = 0;
                    virtual void    SetCapacity (size_t slotsAlloced) = 0;
                };


                /*
                 */
                template    <typename T>
                class   Sequence_Array<T>::Rep_ : public Sequence_Array<T>::IImplRep_ {
                private:
                    using   inherited   =   typename Sequence_Array<T>::IImplRep_;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

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
                    virtual _IterableSharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<T>                 MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<T> (Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
                    }
                    virtual size_t                      GetLength () const override
                    {
                        return fData_.GetLength ();
                    }
                    virtual bool                        IsEmpty () const override
                    {
                        return fData_.GetLength () == 0;
                    }
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    virtual Iterator<T>                 FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        using   RESULT_TYPE     =   Iterator<T>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        size_t i = fData_.FindFirstThat (doToElement);
                        if (i == fData_.GetLength ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetIndex (i);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // Sequence<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<T>::template MakeSharedPtr<Rep_> ();
                        }
                    }
                    virtual T                   GetAt (size_t i) const override
                    {
                        Require (not IsEmpty ());
                        Require (i == kBadSequenceIndex or i < GetLength ());
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        if (i == kBadSequenceIndex) {
                            i = fData_.GetLength () - 1;
                        }
                        return fData_.GetAt (i);
                    }
                    virtual void                SetAt (size_t i, ArgByValueType<T> item) override
                    {
                        Require (i < GetLength ());
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        fData_.SetAt (i, item);
                    }
                    virtual size_t              IndexOf (const Iterator<T>& i) const override
                    {
                        const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return mir.fIterator.CurrentIndex ();
                    }
                    virtual void                Remove (const Iterator<T>& i) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        fData_.RemoveAt (mir.fIterator);
                    }
                    virtual void                Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        fData_.SetAt (mir.fIterator, newValue);
                    }
                    virtual void                Insert (size_t at, const T* from, const T* to) override
                    {
                        Require (at == kBadSequenceIndex or at <= GetLength ());
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        if (at == kBadSequenceIndex) {
                            at = fData_.GetLength ();
                        }
                        // quickie poor impl
                        // @todo use                        ReserveSpeedTweekAddN (fData_, (to - from));
                        // when we fix names
                        {
                            size_t  curLen  =   fData_.GetLength ();
                            size_t  curCap  =   fData_.GetCapacity ();
                            size_t  newLen  =   curLen + (to - from);
                            if (newLen > curCap) {
                                newLen *= 6;
                                newLen /= 5;
                                if (sizeof (T) < 100) {
                                    newLen = Stroika::Foundation::Math::RoundUpTo (newLen, static_cast<size_t> (64));   //?
                                }
                                fData_.SetCapacity (newLen);
                            }
                        }
#if 0
                        size_t  desiredCapacity     =   fData_.GetLength () + (to - from);
                        desiredCapacity = max (desiredCapacity, fData_.GetCapacity ());
                        fData_.SetCapacity (desiredCapacity);
#endif
                        for (auto i = from; i != to; ++i) {
                            fData_.InsertAt (at++, *i);
                        }
                    }
                    virtual void                Remove (size_t from, size_t to) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        // quickie poor impl
                        for (size_t i = from; i < to; ++i) {
                            fData_.RemoveAt (from);
                        }
                    }
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                    // Sequence_Array<T>::IImplRep_
                public:
                    virtual void    Compact () override
                    {
                        fData_.Compact ();
                    }
                    virtual size_t  GetCapacity () const override
                    {
                        return fData_.GetCapacity ();
                    }
                    virtual void    SetCapacity (size_t slotsAlloced) override
                    {
                        fData_.SetCapacity (slotsAlloced);
                    }

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::Array<T>;
                    using   IteratorRep_            =   Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ****************************** Sequence_Array<T> *******************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Sequence_Array<T>::Sequence_Array ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Sequence_Array<T>::Sequence_Array (const Sequence_Array<T>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Sequence_Array<T>::Sequence_Array (const initializer_list<T>& src)
                    : Sequence_Array ()
                {
                    this->AppendAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Sequence_Array<T>::Sequence_Array (const vector<T>& src)
                    : Sequence_Array ()
                {
                    this->AppendAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T, typename ENABLE_IF>
                inline  Sequence_Array<T>::Sequence_Array (const CONTAINER_OF_T& src)
                    : Sequence_Array ()
                {
                    this->AppendAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Sequence_Array<T>::Sequence_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : Sequence_Array ()
                {
                    this->AppendAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Sequence_Array<T>&   Sequence_Array<T>::operator= (const Sequence_Array<T>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (rhs);
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename T>
                inline  void    Sequence_Array<T>::Compact ()
                {
                    typename inherited::template _SafeReadWriteRepAccessor<IImplRep_>  { this } ._GetWriteableRep ().Compact ();
                }
                template    <typename T>
                inline  size_t  Sequence_Array<T>::GetCapacity () const
                {
                    return typename inherited::template _SafeReadRepAccessor<IImplRep_> { this } ._ConstGetRep ().GetCapacity ();
                }
                template    <typename T>
                inline  void    Sequence_Array<T>::SetCapacity (size_t slotsAlloced)
                {
                    typename inherited::template _SafeReadWriteRepAccessor<IImplRep_>  { this } ._GetWriteableRep ().SetCapacity (slotsAlloced);
                }
                template    <typename T>
                inline  void    Sequence_Array<T>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<IImplRep_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_ */

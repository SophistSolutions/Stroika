/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_

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


                template    <typename T, typename TRAITS>
                class   Queue_Array<T, TRAITS>::Rep_ : public Queue<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    Queue<T, TRAITS>::_IRep;

                public:
                    using   _SharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
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

                    // Queue<T, TRAITS>::_IRep overrides
                public:
                    virtual void                AddTail (T item) override;
                    virtual T                   RemoveHead () override;
                    virtual Memory::Optional<T> RemoveHeadIf () override;
                    virtual T                   Head () const override;
                    virtual Memory::Optional<T> HeadIf () const override;
                    virtual void                RemoveAll () override;
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
#endif

                private:
                    using   DataStructureImplType_      =   Private::PatchingDataStructures::Array<T, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_                =   Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    //// UNCLEAR WHY THIS NEEDS TO BE PUBLIC - TODO - FIX!!!
                    /// @todo
                public:
                    DataStructureImplType_      fData_;
                };


                /*
                ********************************************************************************
                ************************* Queue_Array<T, TRAITS>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Queue_Array<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Queue_Array<T, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                typename Queue_Array<T, TRAITS>::Rep_::_SharedPtrIRep  Queue_Array<T, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return _SharedPtrIRep (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Queue_Array<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
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
                template    <typename T, typename TRAITS>
                size_t  Queue_Array<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Queue_Array<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      Queue_Array<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Queue_Array<T, TRAITS>::Rep_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE =   Iterator<T>;
                    shared_ptr<IteratorRep_> resultRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        size_t i = fData_.FindFirstThat (doToElement);
                        if (i == fData_.GetLength ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        resultRep = shared_ptr<IteratorRep_> (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_));
                        resultRep->fIterator.SetIndex (i);
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
                void    Queue_Array<T, TRAITS>::Rep_::AddTail (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.InsertAt (fData_.GetLength (), item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                T    Queue_Array<T, TRAITS>::Rep_::RemoveHead ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        T   item =  fData_.GetAt (0);
                        fData_.RemoveAt (0);
                        return (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Memory::Optional<T>    Queue_Array<T, TRAITS>::Rep_::RemoveHeadIf ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        if (fData_.GetLength () == 0) {
                            return Memory::Optional<T> ();
                        }
                        T   item =  fData_.GetAt (0);
                        fData_.RemoveAt (0);
                        return item;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                T    Queue_Array<T, TRAITS>::Rep_::Head () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.GetAt (0));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Memory::Optional<T>    Queue_Array<T, TRAITS>::Rep_::HeadIf () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        if (fData_.GetLength () == 0) {
                            return Memory::Optional<T> ();
                        }
                        return fData_.GetAt (0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Queue_Array<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    Queue_Array<T, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                ********************************************************************************
                ***************************** Queue_Array<T, TRAITS> ***************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Queue_Array<T, TRAITS>::Queue_Array ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename TRAITS>
                inline  Queue_Array<T, TRAITS>::Queue_Array (const Queue_Array<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                inline  Queue_Array<T, TRAITS>::Queue_Array (const CONTAINER_OF_T& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertNotImplemented ();        // @todo - use new EnqueueAll()
                    //InsertAll (0, s);
                }
                template    <typename T, typename TRAITS>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Queue_Array<T, TRAITS>::Queue_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Append (start, end);
                }
                template    <typename T, typename TRAITS>
                inline  Queue_Array<T, TRAITS>&   Queue_Array<T, TRAITS>::operator= (const Queue_Array<T, TRAITS>& rhs)
                {
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    return *this;
                }
                template    <typename T, typename TRAITS>
                inline  const typename Queue_Array<T, TRAITS>::Rep_&  Queue_Array<T, TRAITS>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T, typename TRAITS>
                inline  typename Queue_Array<T, TRAITS>::Rep_&    Queue_Array<T, TRAITS>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T, typename TRAITS>
                inline  void    Queue_Array<T, TRAITS>::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fData_.fLockSupport) {
                        GetRep_ ().fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  size_t  Queue_Array<T, TRAITS>::GetCapacity () const
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fData_.fLockSupport) {
                        return (GetRep_ ().fData_.GetCapacity ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  void    Queue_Array<T, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fData_.fLockSupport) {
                        GetRep_ ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_ */

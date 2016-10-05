/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Queue_Array<T>::UpdateSafeIterationContainerRep_ : public Queue<T>::_IRep {
                private:
                    using   inherited   =   typename    Queue<T>::_IRep;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    UpdateSafeIterationContainerRep_ () = default;
                    UpdateSafeIterationContainerRep_ (const UpdateSafeIterationContainerRep_& from) = delete;
                    UpdateSafeIterationContainerRep_ (UpdateSafeIterationContainerRep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual  UpdateSafeIterationContainerRep_& operator= (const UpdateSafeIterationContainerRep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (UpdateSafeIterationContainerRep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep  Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        std::lock_guard<std::mutex> critSec (fData_.fActiveIteratorsMutex_);
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<T>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> (const_cast<UpdateSafeIterationContainerRep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<T>             MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<T>::SharedIRepPtr tmpRep;
                        {
                            std::lock_guard<std::mutex> critSec (fData_.fActiveIteratorsMutex_);
                            UpdateSafeIterationContainerRep_*   NON_CONST_THIS  =   const_cast<UpdateSafeIterationContainerRep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        return Iterator<T> (tmpRep);
                    }
                    virtual size_t                  GetLength () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.GetLength ();
                    }
                    virtual bool                    IsEmpty () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.GetLength () == 0;
                    }
                    virtual void                    Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    virtual Iterator<T>             FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using   RESULT_TYPE     =   Iterator<T>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        size_t i = fData_.FindFirstThat (doToElement);
                        if (i == fData_.GetLength ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        SHARED_REP_TYPE resultRep;
                        {
                            std::lock_guard<std::mutex> critSec (fData_.fActiveIteratorsMutex_);
                            UpdateSafeIterationContainerRep_*   NON_CONST_THIS  =   const_cast<UpdateSafeIterationContainerRep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetIndex (i);
                        }
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // Queue<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            std::lock_guard<std::mutex> critSec (fData_.fActiveIteratorsMutex_);
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<T>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> (const_cast<UpdateSafeIterationContainerRep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<T>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> ();
                        }
                    }
                    virtual void                AddTail (ArgByValueType<T> item) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        fData_.InsertAt (fData_.GetLength (), item);
                    }
                    virtual T                   RemoveHead () override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        T   item =  fData_.GetAt (0);
                        fData_.RemoveAt (0);
                        return (item);
                    }
                    virtual Memory::Optional<T> RemoveHeadIf () override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        if (fData_.GetLength () == 0) {
                            return Memory::Optional<T> ();
                        }
                        T   item =  fData_.GetAt (0);
                        fData_.RemoveAt (0);
                        return item;
                    }
                    virtual T                   Head () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.GetAt (0);
                    }
                    virtual Memory::Optional<T> HeadIf () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        if (fData_.GetLength () == 0) {
                            return Memory::Optional<T> ();
                        }
                        return fData_.GetAt (0);
                    }
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                private:
                    using   DataStructureImplType_      =   Private::PatchingDataStructures::Array<T>;
                    using   IteratorRep_                =   Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    //// UNCLEAR WHY THIS NEEDS TO BE PUBLIC - TODO - FIX!!!
                    /// @todo
                public:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ********************************** Queue_Array<T> ******************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Queue_Array<T>::Queue_Array ()
                    : inherited (inherited::template MakeSharedPtr<UpdateSafeIterationContainerRep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Queue_Array<T>::Queue_Array (const Queue_Array<T>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                template    <typename CONTAINER_OF_T, typename ENABLE_IF>
                inline  Queue_Array<T>::Queue_Array (const CONTAINER_OF_T& src)
                    : Queue_Array ()
                {
                    AssertNotImplemented ();        // @todo - use new EnqueueAll()
                    //InsertAll (0, s);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Queue_Array<T>::Queue_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                    : Queue_Array ()
                {
                    Append (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  void    Queue_Array<T>::Compact ()
                {
                    using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<UpdateSafeIterationContainerRep_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    accessor._GetWriteableRep ().fData_.Compact ();
                }
                template    <typename T>
                inline  size_t  Queue_Array<T>::GetCapacity () const
                {
                    using   _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<UpdateSafeIterationContainerRep_>;
                    _SafeReadRepAccessor accessor { this };
                    return accessor._ConstGetRep ().fData_.GetCapacity ();
                }
                template    <typename T>
                inline  void    Queue_Array<T>::SetCapacity (size_t slotsAlloced)
                {
                    using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<UpdateSafeIterationContainerRep_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
                }
                template    <typename T>
                inline  void    Queue_Array<T>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<UpdateSafeIterationContainerRep_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_ */

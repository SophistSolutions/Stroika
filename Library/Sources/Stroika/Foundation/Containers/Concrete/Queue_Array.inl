/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Memory/BlockAllocated.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/Array.h"

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class Queue_Array<T>::Rep_ : public Queue<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Queue<T>::_IRep;

    public:
        using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
        using _QueueRepSharedPtr    = typename inherited::_QueueRepSharedPtr;
        using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
            : inherited ()
            , fData_ (&from->fData_, forIterableEnvelope)
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            return Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
        }
        virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<T> (Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.GetLength () == 0;
        }
        virtual void Apply (_APPLY_ARGTYPE doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            using RESULT_TYPE     = Iterator<T>;
            using SHARED_REP_TYPE = Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_>;
            size_t i              = fData_.FindFirstThat (doToElement);
            if (i == fData_.GetLength ()) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
            resultRep->fIterator.SetIndex (i);
            // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
            return RESULT_TYPE (move (resultRep));
        }

        // Queue<T>::_IRep overrides
    public:
        virtual _QueueRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
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
        virtual void AddTail (ArgByValueType<T> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.InsertAt (fData_.GetLength (), item);
        }
        virtual T RemoveHead () override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            T                                                         item = fData_.GetAt (0);
            fData_.RemoveAt (0);
            return (item);
        }
        virtual optional<T> RemoveHeadIf () override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (fData_.GetLength () == 0) {
                return optional<T> ();
            }
            T item = fData_.GetAt (0);
            fData_.RemoveAt (0);
            return item;
        }
        virtual T Head () const override
        {
            return fData_.GetAt (0);
        }
        virtual optional<T> HeadIf () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            if (fData_.GetLength () == 0) {
                return optional<T> ();
            }
            return fData_.GetAt (0);
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = Private::PatchingDataStructures::Array<T>;
        using IteratorRep_           = Private::IteratorImplHelper_<T, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ********************************** Queue_Array<T> ******************************
     ********************************************************************************
     */
    template <typename T>
    inline Queue_Array<T>::Queue_Array ()
        : inherited (inherited::template MakeSharedPtr<Rep_> ())
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Queue_Array<T>::Queue_Array (const Queue_Array<T>& src)
        : inherited (src)
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_convertible_v<const CONTAINER_OF_T*, const Queue_Array<T>*>>*>
    inline Queue_Array<T>::Queue_Array (const CONTAINER_OF_T& src)
        : Queue_Array ()
    {
        AssertNotImplemented (); // @todo - use new EnqueueAll()
        //InsertAll (0, s);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Queue_Array<T>::Queue_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Queue_Array ()
    {
        Append (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Queue_Array<T>::Compact ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().fData_.Compact ();
    }
    template <typename T>
    inline size_t Queue_Array<T>::GetCapacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_>;
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().fData_.GetCapacity ();
    }
    template <typename T>
    inline void Queue_Array<T>::SetCapacity (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
    }
    template <typename T>
    inline size_t Queue_Array<T>::capacity () const
    {
        return GetCapacity ();
    }
    template <typename T>
    inline void Queue_Array<T>::reserve (size_t slotsAlloced)
    {
        SetCapacity (slotsAlloced);
    }
    template <typename T>
    inline void Queue_Array<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_ */

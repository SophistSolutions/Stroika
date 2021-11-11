/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/Array.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class Queue_Array<T>::Rep_ : public Queue<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Queue<T>::_IRep;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<T>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.GetLength () == 0;
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            size_t                                                      i = fData_.FindFirstThat (doToElement);
            if (i == fData_.GetLength ()) {
                return nullptr;
            }
            Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_> resultRep = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_);
            resultRep->fIterator.SetIndex (i);
            return Iterator<value_type>{move (resultRep)};
        }

        // Queue<T>::_IRep overrides
    public:
        virtual _QueueRepSharedPtr CloneEmpty () const override
        {
            return Iterable<T>::template MakeSmartPtr<Rep_> ();
        }
        virtual void AddTail (ArgByValueType<value_type> item) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            fData_.InsertAt (fData_.GetLength (), item);
            fChangeCounts_.PerformedChange ();
        }
        virtual value_type RemoveHead () override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            T                                                     item = fData_.GetAt (0);
            fData_.RemoveAt (0);
            fChangeCounts_.PerformedChange ();
            return item;
        }
        virtual optional<value_type> RemoveHeadIf () override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            if (fData_.GetLength () == 0) {
                return optional<value_type>{};
            }
            T item = fData_.GetAt (0);
            fData_.RemoveAt (0);
            fChangeCounts_.PerformedChange ();
            return item;
        }
        virtual value_type Head () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.GetAt (0);
        }
        virtual optional<value_type> HeadIf () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            if (fData_.GetLength () == 0) {
                return optional<value_type>{};
            }
            return fData_.GetAt (0);
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ********************************** Queue_Array<T> ******************************
     ********************************************************************************
     */
    template <typename T>
    inline Queue_Array<T>::Queue_Array ()
        : inherited (inherited::template MakeSmartPtr<Rep_> ())
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
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Queue_Array<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Queue_Array<T>::Queue_Array (CONTAINER_OF_ADDABLE&& src)
        : Queue_Array{}
    {
        AssertNotImplemented (); // @todo - use new EnqueueAll()
        //InsertAll (0, s);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_T>
    inline Queue_Array<T>::Queue_Array (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
        : Queue_Array{}
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

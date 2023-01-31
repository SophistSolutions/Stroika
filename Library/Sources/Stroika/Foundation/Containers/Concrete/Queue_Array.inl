/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterable<T>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator (const _IterableRepSharedPtr& thisSharedPtr) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.size () == 0;
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const _IterableRepSharedPtr& thisSharedPtr, const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            size_t                                                i = fData_.Find (that);
            if (i == fData_.size ()) {
                return nullptr;
            }
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, i)};
        }
        virtual Iterator<value_type> Find_equal_to (const _IterableRepSharedPtr& thisSharedPtr, const ArgByValueType<value_type>& v) const override
        {
            return this->_Find_equal_to_default_implementation (thisSharedPtr, v);
        }

        // Queue<T>::_IRep overrides
    public:
        virtual _QueueRepSharedPtr CloneEmpty () const override { return Iterable<T>::template MakeSmartPtr<Rep_> (); }
        virtual void               AddTail (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.InsertAt (fData_.size (), item);
            fChangeCounts_.PerformedChange ();
        }
        virtual value_type RemoveHead () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            T                                                      item = fData_.GetAt (0);
            fData_.RemoveAt (0);
            fChangeCounts_.PerformedChange ();
            return item;
        }
        virtual optional<value_type> RemoveHeadIf () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (fData_.size () == 0) {
                return optional<value_type>{};
            }
            T item = fData_.GetAt (0);
            fData_.RemoveAt (0);
            fChangeCounts_.PerformedChange ();
            return item;
        }
        virtual value_type Head () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.GetAt (0);
        }
        virtual optional<value_type> HeadIf () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (fData_.size () == 0) {
                return optional<value_type>{};
            }
            return fData_.GetAt (0);
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ********************************** Queue_Array<T> ******************************
     ********************************************************************************
     */
    template <typename T>
    inline Queue_Array<T>::Queue_Array ()
        : inherited{inherited::template MakeSmartPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Queue_Array<T>::Queue_Array (const initializer_list<value_type>& src)
        : Queue_Array{}
    {
        reserve (src.size ());
        AddAllToTail (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Queue_Array<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Queue_Array<T>::Queue_Array (ITERABLE_OF_ADDABLE&& src)
        : Queue_Array{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        if constexpr (Configuration::has_size_v<ITERABLE_OF_ADDABLE>) {
            reserve (src.size ());
        }
        AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline Queue_Array<T>::Queue_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Queue_Array{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        if constexpr (Configuration::has_minus_v<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                reserve (end - start);
            }
        }
        AddAllToTail (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Queue_Array<T>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().fData_.shrink_to_fit ();
    }
    template <typename T>
    inline size_t Queue_Array<T>::capacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_>;
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().fData_.capacity ();
    }
    template <typename T>
    inline void Queue_Array<T>::reserve (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().fData_.reserve (slotsAlloced);
    }
    template <typename T>
    inline void Queue_Array<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Queue_Array_inl_ */

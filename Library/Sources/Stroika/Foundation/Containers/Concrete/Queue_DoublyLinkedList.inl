/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/DoublyLinkedList.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class Queue_DoublyLinkedList<T>::Rep_ : public Queue<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
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
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            if (auto iLink = fData_.Find (that)) {
                return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
            }
            return nullptr;
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v) const override
        {
            return this->_Find_equal_to_default_implementation (v);
        }

        // Queue<T>::_IRep overrides
    public:
        virtual _QueueRepSharedPtr CloneEmpty () const override
        {
            return Iterable<value_type>::template MakeSmartPtr<Rep_> ();
        }
        virtual void AddTail (ArgByValueType<T> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteLock writeLock{fData_};
            fData_.Append (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual value_type RemoveHead () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteLock writeLock{fData_};
            value_type                                          item = fData_.GetFirst ();
            fData_.RemoveFirst ();
            fChangeCounts_.PerformedChange ();
            return item;
        }
        virtual optional<value_type> RemoveHeadIf () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteLock writeLock{fData_};
            if (fData_.empty ()) {
                return optional<value_type>{};
            }
            value_type item = fData_.GetFirst ();
            fData_.RemoveFirst ();
            fChangeCounts_.PerformedChange ();
            return item;
        }
        virtual value_type Head () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            return fData_.GetFirst ();
        }
        virtual optional<value_type> HeadIf () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fData_};
            if (fData_.empty ()) {
                return optional<T>{};
            }
            return fData_.GetFirst ();
        }

    private:
        using DataStructureImplType_ = DataStructures::DoublyLinkedList<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     **************************** Queue_DoublyLinkedList<T> *************************
     ********************************************************************************
     */
    template <typename T>
    inline Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList ()
        : inherited{inherited::template MakeSmartPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (const initializer_list<value_type>& src)
        : Queue_DoublyLinkedList{}
    {
        AddAllToTail (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Queue_DoublyLinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src)
        : Queue_DoublyLinkedList{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Queue_DoublyLinkedList{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAllToTail (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Queue_DoublyLinkedList<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Queue_DoublyLinkedList_inl_ */

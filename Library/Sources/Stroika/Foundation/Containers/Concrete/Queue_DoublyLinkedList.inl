/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/DoublyLinkedList.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

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
        virtual shared_ptr<typename Iterable<T>::_IRep> Clone () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (auto iLink = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
            }
            return nullptr;
        }

        // Queue<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Queue<T>::_IRep> CloneEmpty () const override
        {
            return Memory::MakeSharedPtr<Rep_> ();
        }
        virtual void AddTail (ArgByValueType<T> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Append (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual value_type RemoveHead () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            value_type                                             item = fData_.GetFirst ();
            fData_.RemoveFirst ();
            fChangeCounts_.PerformedChange ();
            return item;
        }
        virtual optional<value_type> RemoveHeadIf () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
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
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.GetFirst ();
        }
        virtual optional<value_type> HeadIf () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
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
        : inherited{Memory::MakeSharedPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (const initializer_list<value_type>& src)
        : Queue_DoublyLinkedList{}
    {
        this->AddAllToTail (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Queue_DoublyLinkedList<T>>)
    inline Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src)
        : Queue_DoublyLinkedList{}
    {
        this->AddAllToTail (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Queue_DoublyLinkedList<T>::Queue_DoublyLinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Queue_DoublyLinkedList{}
    {
        this->AddAllToTail (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
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

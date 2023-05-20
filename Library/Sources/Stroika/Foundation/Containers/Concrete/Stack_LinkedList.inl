/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/LinkedList.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ************************* Stack_LinkedList<T>::Rep_ ****************************
     ********************************************************************************
     */
    template <typename T>
    class Stack_LinkedList<T>::Rep_ : public Stack<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Stack<T>::_IRep;

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
        virtual Iterator<value_type> MakeIterator ([[maybe_unused]] const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr) const override
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
        virtual Iterator<value_type> Find ([[maybe_unused]] const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr,
                                           const function<bool (ArgByValueType<value_type> item)>&         that,
                                           [[maybe_unused]] Execution::SequencePolicy                      seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (auto iLink = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
            }
            return nullptr;
        }

        // Stack<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Stack<T>::_IRep> CloneEmpty () const override { return Memory::MakeSharedPtr<Rep_> (); }
        virtual void                                 Push (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Prepend (item); // top of stack is first elt of linked list
            fChangeCounts_.PerformedChange ();
        }
        virtual value_type Pop () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            value_type                                             result = fData_.GetFirst ();
            fData_.RemoveFirst ();
            fChangeCounts_.PerformedChange ();
            return result;
        }
        virtual value_type Top () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.GetFirst ();
        }

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ******************************** Stack_LinkedList<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    inline Stack_LinkedList<T>::Stack_LinkedList ()
        : inherited{Memory::MakeSharedPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <ranges::range ITERABLE_OF_ADDABLE, enable_if_t<not is_base_of_v<Stack_LinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Stack_LinkedList<T>::Stack_LinkedList (ITERABLE_OF_ADDABLE&& src)
        : Stack_LinkedList{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        // sadly intrinsically expensive to copy an Iterable using the stack API
        // @todo find a more efficient way - for example - if there is a way to get a reverse-iterator from 'src' this can be much cheaper! - or at least copy ptrs
        vector<T> tmp;
        for (const auto& si : src) {
            tmp.push_back (si);
        }
        for (const auto& si : tmp) {
            Push (si);
        }
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline Stack_LinkedList<T>::Stack_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Stack_LinkedList{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        // sadly intrinsically expensive to copy an Iterable using the stack API
        // @todo find a more efficient way - for example - if there is a way to get a reverse-iterator from 'src' this can be much cheaper! - or at least copy PTRS
        vector<T> tmp;
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != end; ++i) {
            tmp.push_back (*i);
        }
        for (const auto& si : tmp) {
            Push (si);
        }
    }
    template <typename T>
    inline void Stack_LinkedList<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_ */

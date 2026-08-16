/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/DoublyLinkedList.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class Sequence_DoublyLinkedList<T>::Rep_ : public Sequence<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Sequence<T>::_IRep;

    protected:
        static constexpr size_t _kSentinelLastItemIndex = inherited::_kSentinelLastItemIndex;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<T>::_IRep> Clone () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (*this);
        }
        virtual Iterator<T> MakeIterator () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            // NOTE: O(N), but could easily be made faster caching the length
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find ([[maybe_unused]] bool findFirst, const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            if (auto iLink = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
            }
            return nullptr;
        }

        // Sequence<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Sequence<T>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> ();
        }
        virtual shared_ptr<typename Sequence<T>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            auto                                                    result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual value_type GetAt (size_t i) const override
        {
            Require (not empty ());
            Require (i == _kSentinelLastItemIndex or i < size ());
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            if (i == _kSentinelLastItemIndex) {
                i = size () - 1;
            }
            return fData_.GetAt (i);
        }
        virtual BidirectionalIterator<value_type> GetBidirectionalIterator () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return BidirectionalIterator<value_type>{make_unique<BidirectionalIteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual RandomAccessIterator<value_type> GetRandomAccessIterator () const override
        {
            // no efficient native random-access iterator is possible for a doubly-linked list - always use the generic
            // GetAt ()-based implementation
            return this->_MakeRandomAccessIterator_ViaGetAt ();
        }
        virtual void SetAt (size_t i, ArgByValueType<value_type> item) override
        {
            Require (i < size ());
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            fData_.SetAt (i, item);
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t IndexOf (const Iterator<value_type>& i) const override
        {
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return mir.fIterator.CurrentIndex (&fData_);
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (nextI == nullptr) {
                fData_.Remove (mir.fIterator);
                fChangeCounts_.PerformedChange ();
            }
            else {
                auto ret = fData_.erase (mir.fIterator);
                fChangeCounts_.PerformedChange ();
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, ret)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedChecker::WriteContext         declareWriteContext{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            const IteratorRep_& iteratorRep = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (nextI != nullptr) {
                savedUnderlyingIndex = iteratorRep.fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.SetAt (iteratorRep.fIterator, newValue);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }
        virtual void Insert (size_t at, const span<const value_type>& copyFrom) override
        {
            Require (at == _kSentinelLastItemIndex or at <= size ());
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            if (at == _kSentinelLastItemIndex) {
                fData_.push_back (copyFrom);
            }
            // quickie poor impl
            // See Stroika v1 - much better - handling cases of remove near start or end of linked list
            else if (at == 0) {
                fData_.push_front (copyFrom);
            }
            else if (at == fData_.size ()) {
                fData_.push_back (copyFrom);
            }
            else {
                size_t index = at;
                for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.AtEnd (); ++it) {
                    if (--index == 0) {
                        for (auto p = copyFrom.rbegin (); p != copyFrom.rend (); ++p) {
                            fData_.AddBefore (it, *p);
                        }
                        break;
                    }
                }
                //Assert (not it.AtEnd ());      // cuz that would mean we never added
            }
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (size_t from, size_t to) override
        {
            // quickie poor impl
            // See Stroika v1 - much better - handling cases of remove near start or end of linked list
            size_t index          = from;
            size_t amountToRemove = to - from;
            if (amountToRemove != 0) [[likely]] {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
                for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.AtEnd (); ++it) {
                    if (index-- == 0) {
                        while (amountToRemove-- != 0) {
                            it = fData_.erase (it);
                        }
                        break;
                    }
                }
                fChangeCounts_.PerformedChange ();
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::DoublyLinkedList<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        // same as IteratorImplHelper_DefaultTraits<value_type, DataStructureImplType_>, but pointing at
        // DoublyLinkedList<T>::BidirectionalIterator instead of its (forward-only) ForwardIterator
        struct BidirectionalIteratorTraits_ : Private::IteratorImplHelper_DefaultTraits<value_type, DataStructureImplType_> {
            using DataStructureIteratorT = typename DataStructureImplType_::BidirectionalIterator;
        };
        using BidirectionalIteratorRep_ = Private::BidirectionalIteratorImplHelper_<value_type, DataStructureImplType_, BidirectionalIteratorTraits_>;

    private:
        DataStructureImplType_                       fData_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************************* Sequence_DoublyLinkedList<T> *************************
     ********************************************************************************
     */
    template <typename T>
    inline Sequence_DoublyLinkedList<T>::Sequence_DoublyLinkedList ()
        : inherited{Memory::MakeSharedPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Sequence_DoublyLinkedList<T>::Sequence_DoublyLinkedList (const initializer_list<value_type>& src)
        : Sequence_DoublyLinkedList{}
    {
        this->AppendAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence_DoublyLinkedList<T>>)
    inline Sequence_DoublyLinkedList<T>::Sequence_DoublyLinkedList (ITERABLE_OF_ADDABLE&& src)
        : Sequence_DoublyLinkedList{}
    {
        this->AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Sequence_DoublyLinkedList<T>::Sequence_DoublyLinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Sequence_DoublyLinkedList{}
    {
        this->AppendAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Sequence_DoublyLinkedList<T>::AssertRepValidType_ () const
    {
        if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

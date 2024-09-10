/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/STLContainerWrapper.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /**
     */
    template <typename T, typename TRAITS>
    template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IThreeWayComparer<T>) COMPARER>
    class SortedMultiSet_SkipList<T, TRAITS>::Rep_ : public Private::SkipListBasedContainerRepImpl<Rep_<COMPARER>, IImplRepBase_>,
                                                     public Memory::UseBlockAllocationIfAppropriate<Rep_<COMPARER>> {
    private:
        using inherited = Private::SkipListBasedContainerRepImpl<Rep_<COMPARER>, IImplRepBase_>;

    public:
        static_assert (not is_reference_v<COMPARER>);

    public:
        Rep_ (const COMPARER& comparer)
            : fData_{comparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<typename TRAITS::CountedValueType>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<typename TRAITS::CountedValueType>::_IRep> Clone () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (*this);
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
        virtual Iterator<value_type> MakeIterator () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (that, seq); // @todo rewrite to use fData
        }

        // MultiSet<T, TRAITS>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Common::EqualsComparerAdapter<T, COMPARER>{fData_.key_comp ()};
        }
        virtual shared_ptr<typename MultiSet<T, TRAITS>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data
        }
        virtual shared_ptr<typename MultiSet<T, TRAITS>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<T> item) const override
        {
            value_type                                            tmp{item};
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.Find (item) != fData_.end ();
        }
        virtual void Add (ArgByValueType<T> item, CounterType count) override
        {
            if (count == 0) [[unlikely]] {
                return;
            }
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   i = fData_.Find (item);
            if (i == fData_.end ()) {
                fData_.Add ({item, count});
            }
            else {
                i.UpdateValue (i->fValue + count);
            }
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t RemoveIf (ArgByValueType<T> item, CounterType count) override
        {
            Require (count != 0);
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   i = fData_.Find (item);
            Require (i != fData_.end ());
            if (i != fData_.end ()) {
                size_t result; // intentionally uninitialized
                if (i->fValue > count) {
                    i.UpdateValue (i->fValue - count);
                    result = count;
                }
                else {
                    result = i->fValue;
                    fData_.erase (i);
                }
                fChangeCounts_.PerformedChange ();
                return result;
            }
            return 0;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            mir.fIterator.AssertDataMatches (&fData_);
            auto nextIRes = fData_.erase (mir.fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, nextIRes)};
            }
        }
        virtual void UpdateCount (const Iterator<value_type>& i, CounterType newCount, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (newCount == 0) {
                if (nextI == nullptr) {
                    fData_.Remove (mir.fIterator);
                }
                else {
                    auto nextIRes = fData_.erase (mir.fIterator);
                    *nextI        = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, nextIRes)};
                }
            }
            else {
                fData_.Update (mir.fIterator, newCount);
                if (nextI != nullptr) {
                    *nextI = i; // update doesn't affect iterators
                }
            }
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual CounterType OccurrencesOf (ArgByValueType<T> item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  i = fData_.Find (item);
            if (i == fData_.end ()) {
                return 0;
            }
            return i->fValue;
        }

        // SortedMultiSet<T,TRAITS>::_IRep overrides
    public:
        virtual ElementInOrderComparerType GetElementInOrderComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return ElementInOrderComparerType{Common::InOrderComparerAdapter<T, COMPARER>{fData_.key_comp ()}};
        }

    private:
        using DataStructureImplType_ = SKIPLIST<COMPARER>;
        struct IteratorRep_ : Private::IteratorImplHelper_<value_type, DataStructureImplType_> {
            using inherited = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;
            using inherited::inherited; // forward base class constructors
            // override to map just the key part to 'T'
            virtual void More (optional<value_type>* result, bool advance) override
            {
                RequireNotNull (result);
                this->ValidateChangeCount ();
                if (advance) [[likely]] {
                    Require (not this->fIterator.Done ());
                    ++this->fIterator;
                }
                if (this->fIterator.Done ()) [[unlikely]] {
                    *result = nullopt;
                }
                else {
                    *result = value_type{this->fIterator->fKey, this->fIterator->fValue};
                }
            }
            virtual auto Clone () const -> unique_ptr<typename Iterator<value_type>::IRep> override
            {
                this->ValidateChangeCount ();
                return make_unique<IteratorRep_> (*this);
            }
        };

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        friend inherited;
    };

    /*
     ********************************************************************************
     ********************** SortedMultiSet_SkipList<T, TRAITS> **********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList ()
        : SortedMultiSet_SkipList{compare_three_way{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER>
    inline SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (COMPARER&& comparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<COMPARER>>> (forward<COMPARER> (comparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (const initializer_list<T>& src)
        : SortedMultiSet_SkipList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER>
    SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (COMPARER&& comparer, const initializer_list<T>& src)
        : SortedMultiSet_SkipList{forward<COMPARER> (comparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (const initializer_list<value_type>& src)
        : SortedMultiSet_SkipList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER>
    SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (COMPARER&& comparer, const initializer_list<value_type>& src)
        : SortedMultiSet_SkipList{forward<COMPARER> (comparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename TRAITS>
    template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMultiSet_SkipList<T, TRAITS>>)
    inline SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet_SkipList{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    inline SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet_SkipList{forward<COMPARER> (comparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet_SkipList{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    SortedMultiSet_SkipList<T, TRAITS>::SortedMultiSet_SkipList (COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet_SkipList{forward<COMPARER> (comparer)}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline void SortedMultiSet_SkipList<T, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

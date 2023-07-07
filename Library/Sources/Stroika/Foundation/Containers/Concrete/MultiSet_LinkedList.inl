/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_ 1

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
     ******************** MultiSet_LinkedList<T, TRAITS>::IImplRepBase_ *************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    class MultiSet_LinkedList<T, TRAITS>::IImplRepBase_ : public MultiSet<T, TRAITS>::_IRep {};

    /*
     ********************************************************************************
     ********************** MultiSet_LinkedList<T, TRAITS>::Rep_ ********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    class MultiSet_LinkedList<T, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        static_assert (not is_reference_v<EQUALS_COMPARER>);

    public:
        Rep_ (const EQUALS_COMPARER& equalsComparer)
            : fEqualsComparer_{equalsComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        [[no_unique_address]] const EQUALS_COMPARER fEqualsComparer_;

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

        // MultiSet<T, TRAITS>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return ElementEqualityComparerType{fEqualsComparer_};
        }
        virtual shared_ptr<typename MultiSet<T, TRAITS>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fEqualsComparer_); // new rep with same comparer, but no data
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
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fEqualsComparer_ (it.Current ().fValue, item)) {
                    Assert (it.Current ().fCount != 0);
                    return true;
                }
            }
            return false;
        }
        virtual void Add (ArgByValueType<T> item, CounterType count) override
        {
            if (count != 0) {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
                for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                    auto current = it.Current ();
                    if (fEqualsComparer_ (current.fValue, item)) {
                        current.fCount += count;
                        fData_.SetAt (it, current);
                        fChangeCounts_.PerformedChange ();
                        return;
                    }
                }
                fData_.Prepend (value_type{item, count}); // order meaningless for collection, and prepend cheaper on linked list
                fChangeCounts_.PerformedChange ();
            }
        }
        virtual size_t RemoveIf (ArgByValueType<T> item, CounterType count) override
        {
            Require (count != 0);
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                auto current = it.Current ();
                if (fEqualsComparer_ (current.fValue, item)) {
                    size_t result; // intentionally uninitialized
                    if (current.fCount > count) {
                        current.fCount -= count;
                        fData_.SetAt (it, current);
                        result = count;
                    }
                    else {
                        result = current.fCount;
                        fData_.RemoveAt (it);
                    }
                    fChangeCounts_.PerformedChange ();
                    return result;
                }
            }
            return 0;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual void UpdateCount (const Iterator<value_type>& i, CounterType newCount, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (newCount == 0) {
                if (nextI != nullptr) {
                    *nextI = i;
                    ++(*nextI);
                }
                fData_.RemoveAt (mir.fIterator);
            }
            else {
                value_type c = mir.fIterator.Current ();
                c.fCount     = newCount;
                fData_.SetAt (mir.fIterator, c);
                if (nextI != nullptr) {
                    *nextI = i;
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
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                auto current = it.Current ();
                if (fEqualsComparer_ (current.fValue, item)) {
                    Ensure (current.fCount != 0);
                    return current.fCount;
                }
            }
            return 0;
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
     ************************ MultiSet_LinkedList<T, TRAITS> ************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList ()
        : MultiSet_LinkedList{equal_to<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<EQUALS_COMPARER>>> (forward<EQUALS_COMPARER> (equalsComparer))}
    {
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename TRAITS>
    template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, MultiSet_LinkedList<T, TRAITS>>)
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (ITERABLE_OF_ADDABLE&& src)
        : MultiSet_LinkedList{}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    inline MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src)
        : MultiSet_LinkedList{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<T>& src)
        : MultiSet_LinkedList{}
    {
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src)
        : MultiSet_LinkedList{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (const initializer_list<value_type>& src)
        : MultiSet_LinkedList{}
    {
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src)
        : MultiSet_LinkedList{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : MultiSet_LinkedList{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    MultiSet_LinkedList<T, TRAITS>::MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : MultiSet_LinkedList{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_LinkedList<T, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_inl_ */

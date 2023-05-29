/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /**
     */
    template <typename T, typename TRAITS>
    class SortedMultiSet_stdmap<T, TRAITS>::IImplRepBase_ : public SortedMultiSet<T, TRAITS>::_IRep {};

    /**
     */
    template <typename T, typename TRAITS>
    template <IInOrderComparer<T> INORDER_COMPARER>
    class SortedMultiSet_stdmap<T, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        static_assert (not is_reference_v<INORDER_COMPARER>);

    public:
        Rep_ (const INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<CountedValue<T>>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<CountedValue<T>>::_IRep> Clone () const override
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
        virtual Iterator<value_type> MakeIterator ([[maybe_unused]] const shared_ptr<typename Iterable<CountedValue<T>>::_IRep>& thisSharedPtr) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual Iterator<value_type> Find (const shared_ptr<typename Iterable<CountedValue<T>>::_IRep>& thisSharedPtr,
                                           const function<bool (ArgByValueType<value_type> item)>& that, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (thisSharedPtr, that, seq); // @todo rewrite to use fData
        }

        // MultiSet<T, TRAITS>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return ElementEqualityComparerType{Common::EqualsComparerAdapter{fData_.key_comp ()}};
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
            return fData_.find (item) != fData_.end ();
        }
        virtual void Add (ArgByValueType<T> item, CounterType count) override
        {
            if (count == 0) {
                return;
            }
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   i = fData_.find (item);
            if (i == fData_.end ()) {
                fData_.insert (typename map<T, CounterType>::value_type (item, count));
            }
            else {
                i->second += count;
            }
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t RemoveIf (ArgByValueType<T> item, CounterType count) override
        {
            Require (count != 0);
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   i = fData_.find (item);
            Require (i != fData_.end ());
            if (i != fData_.end ()) {
                size_t result; // intentionally uninitialized
                if (i->second > count) {
                    i->second -= count;
                    result = count;
                }
                else {
                    result = i->second;
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

            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }

            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            (void)fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
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
                (void)fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            }
            else {
                fData_.remove_constness (mir.fIterator.GetUnderlyingIteratorRep ())->second = newCount;
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
            auto                                                  i = fData_.find (item);
            if (i == fData_.end ()) {
                return 0;
            }
            return i->second;
        }

        // SortedMultiSet<T,TRAITS>::_IRep overrides
    public:
        virtual ElementInOrderComparerType GetElementInOrderComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return ElementInOrderComparerType{fData_.key_comp ()};
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDMAP<INORDER_COMPARER>>;
        using IteratorRep_ =
            Private::IteratorImplHelper_<value_type, DataStructureImplType_, typename DataStructureImplType_::ForwardIterator, pair<T, CounterType>>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************************ SortedMultiSet_stdmap<T, TRAITS> **********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap ()
        : SortedMultiSet_stdmap{less<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IInOrderComparer<T> INORDER_COMPARER>
    inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<INORDER_COMPARER>>> (forward<INORDER_COMPARER> (inorderComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const initializer_list<T>& src)
        : SortedMultiSet_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IInOrderComparer<T> INORDER_COMPARER>
    SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src)
        : SortedMultiSet_stdmap{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const initializer_list<value_type>& src)
        : SortedMultiSet_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IInOrderComparer<T> INORDER_COMPARER>
    SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src)
        : SortedMultiSet_stdmap{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <ranges::range ITERABLE_OF_ADDABLE, enable_if_t<not is_base_of_v<SortedMultiSet_stdmap<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet_stdmap{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IInOrderComparer<T> INORDER_COMPARER, ranges::range ITERABLE_OF_ADDABLE>
    inline SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet_stdmap{forward<INORDER_COMPARER> (inorderComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <input_iterator ITERATOR_OF_ADDABLE>
    SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet_stdmap{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IInOrderComparer<T> INORDER_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
    SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet_stdmap{forward<INORDER_COMPARER> (inorderComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline void SortedMultiSet_stdmap<T, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

#endif /* _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_ */

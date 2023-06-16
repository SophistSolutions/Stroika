/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_Array_inl_
#define _Stroika_Foundation_Containers_MultiSet_Array_inl_ 1

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

    /*
     ********************************************************************************
     ********************* MultiSet_Array<T, TRAITS>::IImplRepBase_ *****************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    class MultiSet_Array<T, TRAITS>::IImplRepBase_ : public MultiSet<T, TRAITS>::_IRep {
    public:
        virtual size_t capacity () const             = 0;
        virtual void   reserve (size_t slotsAlloced) = 0;
        virtual void   shrink_to_fit ()              = 0;
    };

    /*
     ********************************************************************************
     ************************** MultiSet_Array<T, TRAITS>::Rep_ *********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    class MultiSet_Array<T, TRAITS>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<EQUALS_COMPARER>> {
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
            return fData_.size () == 0;
        }
        virtual Iterator<value_type> MakeIterator ([[maybe_unused]] const shared_ptr<typename Iterable<CountedValue<T>>::_IRep>& thisSharedPtr) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement, seq);
        }
        virtual Iterator<value_type> Find ([[maybe_unused]] const shared_ptr<typename Iterable<CountedValue<T>>::_IRep>& thisSharedPtr,
                                           const function<bool (ArgByValueType<value_type> item)>&                       that,
                                           [[maybe_unused]] Execution::SequencePolicy                                    seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (optional<size_t> i = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *i)};
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
            return Memory::MakeSharedPtr<Rep_> (fEqualsComparer_); // keep same comparer, but no data
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
            value_type                                            tmp{item};
            return Find_ (tmp) != kNotFound_;
        }
        virtual void Add (ArgByValueType<T> item, CounterType count) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            value_type                                             tmp{item, count};
            size_t                                                 index = Find_ (tmp);
            if (index == kNotFound_) {
                fData_.push_back (tmp);
            }
            else {
                tmp.fCount += count;
                fData_.SetAt (index, tmp);
            }
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t RemoveIf (ArgByValueType<T> item, CounterType count) override
        {
            Require (count > 0);
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            value_type                                             tmp{item};
            size_t                                                 index = Find_ (tmp);
            if (index != kNotFound_) {
                Assert (index < fData_.size ());
                size_t result; // intentionally uninitialized
                if (tmp.fCount > count) {
                    tmp.fCount -= count;
                    Assert (tmp.fCount > 0);
                    fData_.SetAt (index, tmp);
                    result = count;
                }
                else {
                    fData_.RemoveAt (index);
                    result = tmp.fCount;
                }
                fChangeCounts_.PerformedChange ();
                return result;
            }
            return 0;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            optional<size_t>                                       savedUnderlyingIndex;
            static_assert (is_same_v<size_t, typename DataStructureImplType_::UnderlyingIteratorRep>); // else must do slightly differently
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
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
            value_type                                            tmp{item};
            size_t                                                index = Find_ (tmp);
            if (index == kNotFound_) {
                return 0;
            }
            Assert (index >= 0);
            Assert (index < fData_.size ());
            return fData_[index].fCount;
        }

        // MultiSet_Array<T, TRAITS>::_IRep overrides
    public:
        virtual size_t capacity () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.capacity ();
        }
        virtual void reserve (size_t slotsAlloced) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.reserve (slotsAlloced);
            fChangeCounts_.PerformedChange ();
        }
        virtual void shrink_to_fit () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.shrink_to_fit ();
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        static constexpr size_t kNotFound_ = (size_t)-1;

    private:
        nonvirtual size_t Find_ (value_type& item) const
        {
            // this code assumes locking done by callers
            size_t length = fData_.size ();
            for (size_t i = 0; i < length; ++i) {
                if (fEqualsComparer_ (fData_.GetAt (i).fValue, item.fValue)) {
                    item = fData_.GetAt (i);
                    return i;
                }
            }
            return kNotFound_;
        }

    private:
        friend class MultiSet_Array<T, TRAITS>;
    };

    /*
     ********************************************************************************
     ***************************** MultiSet_Array<T, TRAITS> ************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    MultiSet_Array<T, TRAITS>::MultiSet_Array ()
        : MultiSet_Array{equal_to<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<EQUALS_COMPARER>>> (forward<EQUALS_COMPARER> (equalsComparer))}
    {
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename TRAITS>
    template <IIterable<CountedValue<T>> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, MultiSet_Array<T, TRAITS>>)
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array (ITERABLE_OF_ADDABLE&& src)
        : MultiSet_Array{}
    {
        if constexpr (Configuration::has_size_v<ITERABLE_OF_ADDABLE>) {
            reserve (src.size ());
        }
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER, IIterable<CountedValue<T>> ITERABLE_OF_ADDABLE>
    inline MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src)
        : MultiSet_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        reserve (src.size ());
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<T>& src)
        : MultiSet_Array{}
    {
        reserve (src.size ());
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src)
        : MultiSet_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        reserve (src.size ());
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (const initializer_list<value_type>& src)
        : MultiSet_Array{}
    {
        reserve (src.size ());
        AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src)
        : MultiSet_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        reserve (src.size ());
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<CountedValue<T>> ITERATOR_OF_ADDABLE>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : MultiSet_Array{}
    {
        if constexpr (Configuration::has_minus_v<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                reserve (end - start);
            }
        }
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<CountedValue<T>> ITERATOR_OF_ADDABLE>
    MultiSet_Array<T, TRAITS>::MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : MultiSet_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        if constexpr (Configuration::has_minus_v<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                reserve (end - start);
            }
        }
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename TRAITS>
    inline size_t MultiSet_Array<T, TRAITS>::capacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<IImplRepBase_>;
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().capacity ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::reserve (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().reserve (slotsAlloced);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().shrink_to_fit ();
    }
    template <typename T, typename TRAITS>
    inline void MultiSet_Array<T, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_MultiSet_Array_inl_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_inl_
#define _Stroika_Foundation_Containers_MultiSet_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"
#include "../Traversal/IterableFromIterator.h"
#include "Factory/MultiSet_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************* MultiSet<T, TRAITS>::_IRep ***************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    bool MultiSet<T, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
    {
        if (this == &rhs) {
            return true;
        }
        if (this->size () != rhs.size ()) {
            return false;
        }
        for (auto i = this->MakeIterator (); not i.Done (); ++i) {
            if (i->fCount != rhs.OccurrencesOf (i->fValue)) {
                return false;
            }
        }
        return true;
    }

    /*
     ********************************************************************************
     ***************************** MultiSet<T, TRAITS> ******************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    MultiSet<T, TRAITS>::MultiSet ()
        requires (IEqualsComparer<equal_to<T>, T>)
        : MultiSet{equal_to<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    inline MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer)
        : inherited{Factory::MultiSet_Factory<T, TRAITS, remove_cvref_t<EQUALS_COMPARER>>::Default () (forward<EQUALS_COMPARER> (equalsComparer))}
    {
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename TRAITS>
    template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    inline MultiSet<T, TRAITS>::MultiSet (ITERABLE_OF_ADDABLE&& src)
        requires (IEqualsComparer<equal_to<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, MultiSet<T, TRAITS>>)
        : MultiSet{}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    inline MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src)
        : MultiSet{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline MultiSet<T, TRAITS>::MultiSet (const shared_ptr<_IRep>& rep) noexcept
        : inherited{(RequireExpression (rep != nullptr), rep)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline MultiSet<T, TRAITS>::MultiSet (shared_ptr<_IRep>&& rep) noexcept
        : inherited{(RequireExpression (rep != nullptr), move (rep))}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    MultiSet<T, TRAITS>::MultiSet (const initializer_list<T>& src)
        requires (IEqualsComparer<equal_to<T>, T>)
        : MultiSet{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src)
        : MultiSet{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    MultiSet<T, TRAITS>::MultiSet (const initializer_list<value_type>& src)
        : MultiSet{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src)
        : MultiSet{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    MultiSet<T, TRAITS>::MultiSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IEqualsComparer<equal_to<T>, T>)
        : MultiSet{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    MultiSet<T, TRAITS>::MultiSet (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : MultiSet{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    auto MultiSet<T, TRAITS>::TotalOccurrences () const -> CounterType
    {
        CounterType sum = 0;
        for (value_type i : *this) {
            sum += i.fCount;
        }
        return sum;
    }
    template <typename T, typename TRAITS>
    inline auto MultiSet<T, TRAITS>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<value_type> result{nullptr};
        this->Remove (i, &result);
        return result;
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::clear ()
    {
        RemoveAll ();
    }
    template <typename T, typename TRAITS>
    inline Iterable<T> MultiSet<T, TRAITS>::Elements () const
    {
        // Need explicit struct because we need to re-create the iterator on copies
        // Not just simple map cuz must pause and 'create' new extra elements
        struct Context_ {
            MultiSet<T, TRAITS>                         fOriginalMultiset;
            Iterator<typename TRAITS::CountedValueType> fCurrentIteratorOverOrig;
            size_t fIthAdvanceOfIterator{0}; // because not a random-accessor iterator so hard to compute without tracking
            size_t fIthOfCurrentIterator{0};
            Context_ (const Context_& rhs)
                : fOriginalMultiset{rhs.fOriginalMultiset}
                , fCurrentIteratorOverOrig{fOriginalMultiset.MakeIterator ()}
                , fIthAdvanceOfIterator{rhs.fIthAdvanceOfIterator}
                , fIthOfCurrentIterator{rhs.fIthOfCurrentIterator}
            {
                std::advance (fCurrentIteratorOverOrig, fIthAdvanceOfIterator);
            }
            Context_ (const MultiSet<T, TRAITS>& ms)
                : fOriginalMultiset{ms}
                , fCurrentIteratorOverOrig{fOriginalMultiset.MakeIterator ()}
            {
            }
            Context_& operator= (Context_&) = delete; // could implement but I think no need
        };
        function<optional<T> ()> getNext = [context = Context_{*this}] () mutable -> optional<T> {
            again:
                if (context.fCurrentIteratorOverOrig) {
                    if (context.fIthOfCurrentIterator < context.fCurrentIteratorOverOrig->fCount) {
                        ++context.fIthOfCurrentIterator;
                        return context.fCurrentIteratorOverOrig->fValue;
                    }
                    else {
                        ++context.fCurrentIteratorOverOrig;
                        ++context.fIthAdvanceOfIterator;
                        context.fIthOfCurrentIterator = 0;
                        goto again;
                    }
                }
                return nullopt;
        };
        return Traversal::CreateGenerator (getNext);
    }
    template <typename T, typename TRAITS>
    inline Iterable<T> MultiSet<T, TRAITS>::UniqueElements () const
    {
        return this->template Map<T> ([] (const typename TRAITS::CountedValueType& cv) { return cv.fValue; });
    }
    template <typename T, typename TRAITS>
    Iterable<typename TRAITS::CountedValueType> MultiSet<T, TRAITS>::Top () const
    {
        return this->inherited::Top ([] (const typename TRAITS::CountedValueType& lhs, const typename TRAITS::CountedValueType& rhs) {
            return lhs.fCount > rhs.fCount;
        });
    }
    template <typename T, typename TRAITS>
    Iterable<typename TRAITS::CountedValueType> MultiSet<T, TRAITS>::Top (size_t n) const
    {
        return this->inherited::Top (n, [] (const typename TRAITS::CountedValueType& lhs, const typename TRAITS::CountedValueType& rhs) {
            return lhs.fCount > rhs.fCount;
        });
    }
    template <typename T, typename TRAITS>
    Iterable<T> MultiSet<T, TRAITS>::TopElements () const
    {
        return Top ().template Map<T> ([] (const typename TRAITS::CountedValueType& cv) { return cv.fValue; });
    }
    template <typename T, typename TRAITS>
    Iterable<T> MultiSet<T, TRAITS>::TopElements (size_t n) const
    {
        return Top (n).template Map<T> ([] (const typename TRAITS::CountedValueType& cv) { return cv.fValue; });
    }
    template <typename T, typename TRAITS>
    inline auto MultiSet<T, TRAITS>::GetElementEqualsComparer () const -> ElementEqualityComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementEqualsComparer ();
    }
    template <typename T, typename TRAITS>
    inline bool MultiSet<T, TRAITS>::Contains (ArgByValueType<T> item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Contains (item);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T, typename TRAITS>
    inline size_t MultiSet<T, TRAITS>::RemoveAll (ArgByValueType<T> item)
    {
        return RemoveIf (item, OccurrencesOf (item));
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Add (ArgByValueType<T> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, 1);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Add (ArgByValueType<T> item, CounterType count)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, count);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Add (const value_type& item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item.fValue, item.fCount);
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    void MultiSet<T, TRAITS>::AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        for (ITERATOR_OF_ADDABLE i = forward<ITERATOR_OF_ADDABLE> (start); i != forward<ITERATOR_OF_ADDABLE> (end); ++i) {
            Add (*i);
        }
    }
    template <typename T, typename TRAITS>
    template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    void MultiSet<T, TRAITS>::AddAll (ITERABLE_OF_ADDABLE&& items)
    {
        // see https://stroika.atlassian.net/browse/STK-645
        if constexpr (std::is_convertible_v<remove_cvref_t<ITERABLE_OF_ADDABLE>*, const MultiSet<T, TRAITS>*>) {
            if (static_cast<const Iterable<value_type>*> (this) == static_cast<const Iterable<value_type>*> (&items)) [[unlikely]] {
                // very rare corner case
                vector<typename remove_cvref_t<ITERABLE_OF_ADDABLE>::value_type> copy{std::begin (items), std::end (items)}; // because you can not iterate over a container while modifying it
                for (const auto& i : copy) {
                    Add (i);
                }
                return;
            }
        }
        for (const auto& i : items) {
            Add (i);
        }
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Remove (ArgByValueType<T> item, CounterType count)
    {
        Verify (_SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (item, count) == count);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename T, typename TRAITS>
    inline size_t MultiSet<T, TRAITS>::RemoveIf (ArgByValueType<T> item, CounterType count)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (item, count);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::UpdateCount (const Iterator<value_type>& i, CounterType newCount, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->UpdateCount (patchedIterator, newCount, nextI);
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::SetCount (ArgByValueType<T> i, CounterType newCount)
    {
        CounterType cnt = OccurrencesOf (i);
        if (newCount > cnt) {
            Add (i, newCount - cnt);
        }
        else if (newCount < cnt) {
            Remove (i, cnt - newCount);
        }
    }
    template <typename T, typename TRAITS>
    inline auto MultiSet<T, TRAITS>::OccurrencesOf (ArgByValueType<T> item) const -> CounterType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().OccurrencesOf (item);
    }
    template <typename T, typename TRAITS>
    inline MultiSet<T, TRAITS>& MultiSet<T, TRAITS>::operator+= (ArgByValueType<T> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item, 1);
        return *this;
    }
    template <typename T, typename TRAITS>
    MultiSet<T, TRAITS>& MultiSet<T, TRAITS>::operator+= (const MultiSet<T, TRAITS>& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename T, typename TRAITS>
    auto MultiSet<T, TRAITS>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
    {
        Require (not i.Done ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        Iterator<value_type> patchedIterator = i;
        element_type* writableRep = this->_fRep.rwget ([&] (const element_type& prevRepPtr) -> typename inherited::_SharedByValueRepType::shared_ptr_type {
            return Debug::UncheckedDynamicCast<const _IRep&> (prevRepPtr).CloneAndPatchIterator (&patchedIterator);
        });
        AssertNotNull (writableRep);
        return make_tuple (Debug::UncheckedDynamicCast<_IRep*> (writableRep), move (patchedIterator));
    }
    template <typename T, typename TRAITS>
    inline void MultiSet<T, TRAITS>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T, typename TRAITS>
    inline bool MultiSet<T, TRAITS>::operator== (const MultiSet& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
    }

}

#endif /* _Stroika_Foundation_Containers_MultiSet_inl_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Factory/SortedMultiSet_Factory.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************ SortedMultiSet<T, TRAITS> *****************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet ()
        requires (Common::ITotallyOrderingComparer<less<T>, T>)
        : SortedMultiSet{less<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <Common::ITotallyOrderingComparer<T> COMPARER>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (COMPARER&& comparer)
        : inherited{Factory::SortedMultiSet_Factory<T, TRAITS, remove_cvref_t<COMPARER>>::Default () (forward<COMPARER> (comparer))}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<T>& src)
        requires (Common::ITotallyOrderingComparer<less<T>, T>)
        : SortedMultiSet{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <Common::ITotallyOrderingComparer<T> COMPARER>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (COMPARER&& comparer, const initializer_list<T>& src)
        : SortedMultiSet{forward<COMPARER> (comparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<value_type>& src)
        requires (Common::ITotallyOrderingComparer<less<T>, T>)
        : SortedMultiSet{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <Common::ITotallyOrderingComparer<T> COMPARER>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (COMPARER&& comparer, const initializer_list<value_type>& src)
        : SortedMultiSet{forward<COMPARER> (comparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename TRAITS>
    template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (ITERABLE_OF_ADDABLE&& src)
        requires (Common::ITotallyOrderingComparer<less<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMultiSet<T, TRAITS>>)
        : SortedMultiSet{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T, typename TRAITS>
    template <Common::ITotallyOrderingComparer<T> COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet{forward<COMPARER> (comparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (Common::ITotallyOrderingComparer<less<T>, T>)
        : SortedMultiSet{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<typename TRAITS::CountedValueType> COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet{forward<COMPARER> (comparer)}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    inline void SortedMultiSet<T, TRAITS>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T, typename TRAITS>
    inline auto SortedMultiSet<T, TRAITS>::GetElementInOrderComparer () const -> ElementInOrderComparerType
    {
        return Common::InOrderComparerAdapter<T, ElementThreeWayComparerType>{
            _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementThreeWayComparer ()};
    }
    template <typename T, typename TRAITS>
    inline auto SortedMultiSet<T, TRAITS>::GetElementThreeWayComparer () const -> ElementThreeWayComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementThreeWayComparer ();
    }
    template <typename T, typename TRAITS>
    template <typename RESULT_CONTAINER, invocable<T> ELEMENT_MAPPER>
    nonvirtual RESULT_CONTAINER SortedMultiSet<T, TRAITS>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, typename TRAITS::CountedValueType>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, typename TRAITS::CountedValueType>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedMultiSet>) {
            // clone the rep so we retain the ordering function
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default Iterable<> interpretation
        }
    }
    template <typename T, typename TRAITS>
    template <derived_from<Iterable<typename TRAITS::CountedValueType>> RESULT_CONTAINER, predicate<typename TRAITS::CountedValueType> INCLUDE_PREDICATE>
    inline RESULT_CONTAINER SortedMultiSet<T, TRAITS>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedMultiSet>) {
            // clone the rep so we retain the ordering function
            return inherited::template Map<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue)); // default inherited interpretation
        }
    }
    template <typename T, typename TRAITS>
    inline strong_ordering SortedMultiSet<T, TRAITS>::operator<=> (const SortedMultiSet& rhs) const
    {
        return typename Iterable<typename TRAITS::CountedValueType>::SequentialThreeWayComparer{GetElementThreeWayComparer ()}(*this, rhs);
    }

}

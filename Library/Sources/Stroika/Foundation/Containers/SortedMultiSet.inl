/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../Debug/Assertions.h"
#include "Factory/SortedMultiSet_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************ SortedMultiSet<T, TRAITS> *****************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet ()
        requires (Common::IInOrderComparer<less<T>, T>)
        : SortedMultiSet{less<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer)
        : inherited{Factory::SortedMultiSet_Factory<T, TRAITS, remove_cvref_t<INORDER_COMPARER>>::Default () (forward<INORDER_COMPARER> (inorderComparer))}
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
        requires (Common::IInOrderComparer<less<T>, T>)
        : SortedMultiSet{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<T>& src)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<value_type>& src)
        requires (Common::IInOrderComparer<less<T>, T>)
        : SortedMultiSet{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename TRAITS>
    template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (ITERABLE_OF_ADDABLE&& src)
        requires (Common::IInOrderComparer<less<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMultiSet<T, TRAITS>>)
        : SortedMultiSet{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T, typename TRAITS>
    template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
    inline SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (Common::IInOrderComparer<less<T>, T>)
        : SortedMultiSet{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename TRAITS>
    template <IInputIterator<typename TRAITS::CountedValueType> INORDER_COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
    SortedMultiSet<T, TRAITS>::SortedMultiSet (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMultiSet{forward<INORDER_COMPARER> (inorderComparer)}
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
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementInOrderComparer ();
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
        return typename Iterable<typename TRAITS::CountedValueType>::SequentialThreeWayComparer{
            Common::ThreeWayComparerAdapter{GetElementInOrderComparer ()}}(*this, rhs);
    }

}

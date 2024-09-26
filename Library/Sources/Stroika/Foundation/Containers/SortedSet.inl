/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Factory/SortedSet_Factory.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ********************************** SortedSet<T> ********************************
     ********************************************************************************
     */
    template <typename T>
    inline SortedSet<T>::SortedSet ()
        requires (ITotallyOrderingComparer<less<T>, T>)
        : SortedSet{less<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <ITotallyOrderingComparer<T> COMPARER>
    inline SortedSet<T>::SortedSet (COMPARER&& comparer)
        : inherited{Factory::SortedSet_Factory<T, remove_cvref_t<COMPARER>>::Default () (forward<COMPARER> (comparer))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedSet<T>::SortedSet (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedSet<T>::SortedSet (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline SortedSet<T>::SortedSet (const initializer_list<T>& src)
        requires (ITotallyOrderingComparer<less<T>, T>)
        : SortedSet{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <ITotallyOrderingComparer<T> COMPARER>
    inline SortedSet<T>::SortedSet (COMPARER&& comparer, const initializer_list<T>& src)
        : SortedSet{forward<COMPARER> (comparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
    inline SortedSet<T>::SortedSet (ITERABLE_OF_ADDABLE&& src)
        requires (ITotallyOrderingComparer<less<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedSet<T>>)
        : SortedSet{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename T>
    template <ITotallyOrderingComparer<T> COMPARER, IIterableOfTo<T> ITERABLE_OF_ADDABLE>
    inline SortedSet<T>::SortedSet (COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src)
        : SortedSet (forward<COMPARER> (comparer))
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
    inline SortedSet<T>::SortedSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
        requires (ITotallyOrderingComparer<less<T>, T>)
        : SortedSet{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE2> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    template <ITotallyOrderingComparer<T> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedSet<T>::SortedSet (COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedSet{forward<COMPARER> (comparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    inline void SortedSet<T>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T>
    inline auto SortedSet<T>::GetElementInOrderComparer () const -> ElementInOrderComparerType
    {
        return Common::InOrderComparerAdapter<T, ElementInOrderComparerType>{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementInOrderComparer ()};
    }
    template <typename T>
    inline auto SortedSet<T>::GetElementThreeWayComparer () const -> ElementThreeWayComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementThreeWayComparer ();
    }
    template <typename T>
    inline strong_ordering SortedSet<T>::operator<=> (const SortedSet& rhs) const
    {
        return typename Iterable<T>::SequentialThreeWayComparer{GetElementThreeWayComparer ()}(*this, rhs);
    }
    template <typename T>
    template <typename RESULT_CONTAINER, invocable<T> ELEMENT_MAPPER>
    inline RESULT_CONTAINER SortedSet<T>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedSet>) {
            // clone the rep so we retain the ordering function
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default Iterable<> interpretation
        }
    }
    template <typename T>
    template <derived_from<Iterable<T>> RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
    inline RESULT_CONTAINER SortedSet<T>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedSet>) {
            // clone the rep so we retain the ordering function
            return inherited::template Where<RESULT_CONTAINER> (
                forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue)); // default Iterable<> interpretation
        }
    }

}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Containers/Factory/SortedMapping_Factory.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     **************** SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE> ********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping ()
        requires (ITotallyOrderingComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedMapping{less<KEY_TYPE>{}}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_COMPARER&& keyComparer)
        : inherited{Factory::SortedMapping_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, remove_cvref_t<KEY_COMPARER>>::Default () (forward<KEY_COMPARER> (keyComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        requires (ITotallyOrderingComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedMapping{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_COMPARER&& keyComparer,
                                                                      const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedMapping{forward<KEY_COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOfTo<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (ITERABLE_OF_ADDABLE&& src)
        requires (ITotallyOrderingComparer<less<KEY_TYPE>, KEY_TYPE> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        : SortedMapping{}
    {
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER, IIterableOfTo<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMapping{forward<KEY_COMPARER> (keyComparer)}
    {
        _AssertRepValidType ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (ITotallyOrderingComparer<less<KEY_TYPE>, KEY_TYPE>)
        : SortedMapping{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <ITotallyOrderingComparer<KEY_TYPE> KEY_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMapping{forward<KEY_COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const shared_ptr<_IRep>& src) noexcept
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::GetKeyInOrderComparer () const -> KeyInOrderComparerType
    {
        return Common::InOrderComparerAdapter<KEY_TYPE, KeyThreeWayComparerType>{
            _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetKeyThreeWayComparer ()};
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::GetKeyThreeWayComparer () const -> KeyThreeWayComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetKeyThreeWayComparer ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename RESULT_CONTAINER, invocable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ELEMENT_MAPPER>
    inline RESULT_CONTAINER SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedMapping>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default Iterable<> implementation then...
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <derived_from<Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> RESULT_CONTAINER, typename INCLUDE_PREDICATE>
    inline RESULT_CONTAINER SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
        requires (predicate<INCLUDE_PREDICATE, KEY_TYPE> or predicate<INCLUDE_PREDICATE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedMapping>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Where<RESULT_CONTAINER> (
                forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue));
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline strong_ordering SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::operator<=> (const SortedMapping& rhs) const
    {
        // Iterate over both mappings at the same time, only looking at the keys. If any two elements are !=,
        // then use the value as a tie-breaker
        using SeqCmp = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::template SequentialThreeWayComparer<compare_three_way>;
        return SeqCmp{}(*this, rhs);
    }

}

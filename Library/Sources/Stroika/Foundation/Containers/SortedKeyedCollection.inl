/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedKeyedCollection_inl_
#define _Stroika_Foundation_Containers_SortedKeyedCollection_inl_

#include "../Debug/Assertions.h"

#include "Factory/SortedKeyedCollection_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     **************** SortedKeyedCollection<T, KEY_TYPE, TRAITS> ********************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection{KeyExtractorType{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer)
        : inherited (Factory::SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, remove_cvref_t<KEY_INORDER_COMPARER>>::Default () (
              keyExtractor, forward<KEY_INORDER_COMPARER> (keyComparer)))
    {
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (ITERABLE_OF_ADDABLE&& src)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedKeyedCollection<T, KEY_TYPE, TRAITS>>)
        : SortedKeyedCollection{}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (const KeyExtractorType& keyExtractor,
                                                                              KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedKeyedCollection{keyExtractor, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection{KeyExtractorType{}, KEY_INORDER_COMPARER{}}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (KEY_INORDER_COMPARER&& keyComparer,
                                                                              ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection{KeyExtractorType{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer,
                                                                              ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedKeyedCollection{keyExtractor, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (const _IRepSharedPtr& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS>::SortedKeyedCollection (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline auto SortedKeyedCollection<T, KEY_TYPE, TRAITS>::GetInOrderKeyComparer () const -> KeyInOrderKeyComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderKeyComparer ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename RESULT_CONTAINER, invocable<T> ELEMENT_MAPPER>
    inline RESULT_CONTAINER SortedKeyedCollection<T, KEY_TYPE, TRAITS>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedKeyedCollection>) {
            // clone the rep so we retain the ordering function etc
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default inherited interpretation
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <derived_from<Iterable<T>> RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
    inline RESULT_CONTAINER SortedKeyedCollection<T, KEY_TYPE, TRAITS>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
    {
        if constexpr (same_as<RESULT_CONTAINER, SortedKeyedCollection>) {
            // clone the rep so we retain the ordering function etc
            return inherited::template Where<RESULT_CONTAINER> (
                forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue)); // default inherited interpretation
        }
    }

}

#endif /* _Stroika_Foundation_Containers_SortedKeyedCollection_inl_ */

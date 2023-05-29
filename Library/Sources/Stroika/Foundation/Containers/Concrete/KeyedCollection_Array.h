/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../KeyedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers {

    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection;

}

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief KeyedCollection_Array<T> is an Array-based concrete implementation of the KeyedCollection<T> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_Array : public KeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = KeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using TraitsType                  = typename inherited::TraitsType;
        using KeyExtractorType            = typename inherited::KeyExtractorType;
        using KeyEqualityComparerType     = typename inherited::KeyEqualityComparerType;
        using KeyType                     = typename inherited::KeyType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on KeyedCollection<> constructor
         */
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        KeyedCollection_Array (KeyedCollection_Array&& src) noexcept      = default;
        KeyedCollection_Array (const KeyedCollection_Array& src) noexcept = default;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        template <ranges::range ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<not is_base_of_v<KeyedCollection_Array<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        KeyedCollection_Array (ITERABLE_OF_ADDABLE&& src);
#else
        template <ranges::range ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (ITERABLE_OF_ADDABLE&& src)
            requires (not is_base_of_v<KeyedCollection_Array<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>);
#endif
        template <ranges::range ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, ranges::range ITERABLE_OF_ADDABLE>
        KeyedCollection_Array (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <input_iterator ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <input_iterator ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
        KeyedCollection_Array (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                               ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual KeyedCollection_Array& operator= (KeyedCollection_Array&& rhs) noexcept = default;
        nonvirtual KeyedCollection_Array& operator= (const KeyedCollection_Array& rhs)     = default;

    private:
        class IImplRep_;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "KeyedCollection_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_h_ */

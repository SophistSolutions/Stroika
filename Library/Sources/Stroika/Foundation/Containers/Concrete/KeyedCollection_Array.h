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
        template <typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor,
                  enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        KeyedCollection_Array (KeyedCollection_Array&& src) noexcept      = default;
        KeyedCollection_Array (const KeyedCollection_Array& src) noexcept = default;
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_Array (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        template <typename ITERABLE_OF_ADDABLE, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection_Array<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and
                              Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_Array (ITERABLE_OF_ADDABLE&& src);
        template <typename ITERABLE_OF_ADDABLE, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection_Array<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and
                              Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE,
                  enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                              Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        KeyedCollection_Array (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                              Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        KeyedCollection_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename ITERATOR_OF_ADDABLE, typename KEY_EXTRACTOR = typename TraitsType::DefaultKeyExtractor, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                              Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE,
                  enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                              Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        KeyedCollection_Array (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual KeyedCollection_Array& operator= (KeyedCollection_Array&& rhs) noexcept = default;
        nonvirtual KeyedCollection_Array& operator= (const KeyedCollection_Array& rhs)     = default;

    private:
        class IImplRep_;
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
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

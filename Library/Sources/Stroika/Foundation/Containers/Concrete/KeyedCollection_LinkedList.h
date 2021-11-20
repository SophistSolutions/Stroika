/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../KeyedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronizaiton support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *
 */

namespace Stroika::Foundation::Containers {

    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection;

}

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief KeyedCollection_LinkedList<T> is an LinkedList-based concrete implementation of the KeyedCollection<T> container pattern.
     *
     * \note Performance Notes:
     *      o   GetLength () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_LinkedList : public KeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = KeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        using TraitsType              = typename inherited::TraitsType;
        using KeyExtractorType        = typename inherited::KeyExtractorType;
        using KeyEqualityComparerType = typename inherited::KeyEqualityComparerType;
        using KeyType                 = typename inherited::KeyType;
        using value_type              = typename inherited::value_type;

    public:
        /**
         */
        template <typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        template <typename KEY_EXTRACTOR,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_LinkedList (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        KeyedCollection_LinkedList (const KeyedCollection_LinkedList& src) noexcept = default;
        template <typename CONTAINER_OF_ADDABLE,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_LinkedList (CONTAINER_OF_ADDABLE&& src);
        template <typename CONTAINER_OF_ADDABLE,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>* = nullptr>
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>* = nullptr>
        KeyedCollection_LinkedList (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>* = nullptr>
        KeyedCollection_LinkedList (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  typename KEY_EXTRACTOR       = typename TraitsType::DefaultKeyExtractor,
                  typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  enable_if_t<
                      Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>* = nullptr>
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE,
                  enable_if_t<
                      KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        KeyedCollection_LinkedList (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    public:
        nonvirtual KeyedCollection_LinkedList& operator= (const KeyedCollection_LinkedList& rhs) = default;

    protected:
        using _IterableRepSharedPtr        = typename inherited::_IterableRepSharedPtr;
        using _KeyedCollectionRepSharedPtr = typename inherited::_IRepSharedPtr;

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
#include "KeyedCollection_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_h_ */

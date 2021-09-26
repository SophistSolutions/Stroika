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
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
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
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_LinkedList : public KeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = KeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        /**
         */
        using KeyExtractorType = typename inherited::KeyExtractorType;

    public:
        /**
         */
        using KeyEqualityComparerType = typename inherited::KeyEqualityComparerType;

    public:
        /**
         */
        using KeyType = typename inherited::KeyType;

    public:
        /**
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         */
        template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer);
        KeyedCollection_LinkedList (const KeyedCollection<T, KEY_TYPE, TRAITS>& src);
        KeyedCollection_LinkedList (const KeyedCollection_LinkedList& src) noexcept = default;
        KeyedCollection_LinkedList (KeyedCollection_LinkedList&& src) noexcept      = default;

    public:
        nonvirtual KeyedCollection_LinkedList& operator= (const KeyedCollection_LinkedList& rhs) = default;

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

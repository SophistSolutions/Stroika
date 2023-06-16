/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_LinkedList : public KeyedCollection<T, KEY_TYPE, TRAITS> {
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
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        KeyedCollection_LinkedList (KeyedCollection_LinkedList&& src) noexcept      = default;
        KeyedCollection_LinkedList (const KeyedCollection_LinkedList& src) noexcept = default;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        template <IIterable<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>>)
        KeyedCollection_LinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        {
            this->AddAll (src);
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IIterable<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterable<T> ITERABLE_OF_ADDABLE>
        KeyedCollection_LinkedList (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        KeyedCollection_LinkedList (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                    ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual KeyedCollection_LinkedList& operator= (KeyedCollection_LinkedList&& rhs) noexcept = default;
        nonvirtual KeyedCollection_LinkedList& operator= (const KeyedCollection_LinkedList& rhs)     = default;

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
#include "KeyedCollection_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_h_ */

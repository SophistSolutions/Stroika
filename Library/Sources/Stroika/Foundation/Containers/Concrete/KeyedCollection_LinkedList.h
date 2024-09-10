/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_LinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/KeyedCollection.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
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
     * \note Runtime performance/complexity:
     *      o   size () is O(N), but empty () is constant
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
         *  \see docs on KeyedCollection<> constructor
         */
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{})
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        KeyedCollection_LinkedList (KeyedCollection_LinkedList&&) noexcept      = default;
        KeyedCollection_LinkedList (const KeyedCollection_LinkedList&) noexcept = default;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>>)
        KeyedCollection_LinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : KeyedCollection_LinkedList{KeyExtractorType{}, equal_to<KEY_TYPE>{}}
        {
            this->AddAll (src);
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        KeyedCollection_LinkedList (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_LinkedList (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        KeyedCollection_LinkedList (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                    ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual KeyedCollection_LinkedList& operator= (KeyedCollection_LinkedList&&) noexcept = default;
        nonvirtual KeyedCollection_LinkedList& operator= (const KeyedCollection_LinkedList&)     = default;

    private:
        using IImplRep_ = typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IEqualsComparer<KEY_TYPE>) KEY_EQUALS_COMPARER>
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

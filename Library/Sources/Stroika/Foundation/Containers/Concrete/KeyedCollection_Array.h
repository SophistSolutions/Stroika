/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// @todo fix - this SB inside ifndef but fails to compile - debug!!!

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Containers/Private/ArraySupport.h"

#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_h_

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
     *  \brief KeyedCollection_Array<T> is an Array-based concrete implementation of the KeyedCollection<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      o performance good only if KeyedCollection remains small (probably < 10)
     *
     *  \todo   Consider overloading AddAll() to do a reserve if argument size known (so no need to 'performance fix' constructors that call AddAll todo this)
     *          maybe no need (document that if I decide so), since performance of this only decent if size small, so then no matter.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename KEY_TYPE, typename TRAITS = KeyedCollection_DefaultTraits<T, KEY_TYPE>>
    class KeyedCollection_Array
        : public Private::ArrayBasedContainer<KeyedCollection_Array<T, KEY_TYPE, TRAITS>, KeyedCollection<T, KEY_TYPE, TRAITS>, true> {
    private:
        using inherited = Private::ArrayBasedContainer<KeyedCollection_Array<T, KEY_TYPE, TRAITS>, KeyedCollection<T, KEY_TYPE, TRAITS>, true>;

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
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{})
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        KeyedCollection_Array (KeyedCollection_Array&&) noexcept      = default;
        KeyedCollection_Array (const KeyedCollection_Array&) noexcept = default;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer = KEY_EQUALS_COMPARER{});
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, KeyedCollection_Array<T, KEY_TYPE, TRAITS>>)
        KeyedCollection_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : KeyedCollection_Array{KeyExtractorType{}, equal_to<KEY_TYPE>{}}
        {
            this->AddAll (src);
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IIterableOf<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        KeyedCollection_Array (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        KeyedCollection_Array (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                               ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual KeyedCollection_Array& operator= (KeyedCollection_Array&&) noexcept = default;
        nonvirtual KeyedCollection_Array& operator= (const KeyedCollection_Array&)     = default;

    private:
        using IImplRepBase_ = Containers::Private::ArrayBasedContainerIRep<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep>;
        template <qCompilerAndStdLib_UseConceptOrTypename_BWA (IEqualsComparer<KEY_TYPE>) KEY_EQUALS_COMPARER>
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;

    private:
        friend inherited;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "KeyedCollection_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_h_ */

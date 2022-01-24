/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../MultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_ 1

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *
     * \note Performance Notes:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class MultiSet_Array : public MultiSet<T, TRAITS> {
    private:
        using inherited = MultiSet<T, TRAITS>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using CounterType                 = typename inherited::CounterType;
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on MultiSet<> constructor
         */
        MultiSet_Array ();
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>* = nullptr>
        explicit MultiSet_Array (EQUALS_COMPARER&& equalsComparer);
        MultiSet_Array (MultiSet_Array&& src) noexcept      = default;
        MultiSet_Array (const MultiSet_Array& src) noexcept = default;
        MultiSet_Array (const initializer_list<T>& src);
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>* = nullptr>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src);
        MultiSet_Array (const initializer_list<value_type>& src);
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>* = nullptr>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<MultiSet_Array<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit MultiSet_Array (ITERABLE_OF_ADDABLE&& src);
        template <typename EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        MultiSet_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual MultiSet_Array& operator= (MultiSet_Array&& rhs) noexcept = default;
        nonvirtual MultiSet_Array& operator= (const MultiSet_Array& rhs) = default;

    public:
        /*
         *  \brief Return the number of allocated vector/array elements.
         * 
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  \note alias GetCapacity ();
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         * This optional API allows pre-reserving space as an optimization.
         * 
         *  \note Alias SetCapacity ();
         * 
         *  \note Note that this does not affect the semantics of the MultiSet.
         * 
         *  \req slotsAllocated >= size ()
         */
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        nonvirtual void shrink_to_fit ();

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _MultiSetRepSharedPtr = typename inherited::_IRepSharedPtr;

    private:
        class IImplRepBase_;
        template <typename EQUALS_COMPARER>
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
#include "MultiSet_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_*/

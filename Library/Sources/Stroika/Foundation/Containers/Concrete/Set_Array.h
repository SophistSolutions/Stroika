/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Set_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Set_Array<T> is an Array-based concrete implementation of the Set<T> container pattern.
     *
     * \note Performance Notes:
     *      Set_stdset<T> is a compact representation, but ONLY reasonable for very small sets. Performance is O(N) as set grows.
     *
     *      o   size () is O(N)
     *      o   Additions and Removals are generally O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Set_Array : public Set<T> {
    private:
        using inherited = Set<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on Set<> constructor
         */
        Set_Array ();
        template <IEqualsComparer<T> EQUALS_COMPARER>
        explicit Set_Array (EQUALS_COMPARER&& equalsComparer);
        Set_Array (Set_Array&& src) noexcept      = default;
        Set_Array (const Set_Array& src) noexcept = default;
        Set_Array (const initializer_list<value_type>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        Set_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Set_Array<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Set_Array (ITERABLE_OF_ADDABLE&& src);
        template <IEqualsComparer<T> EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        Set_Array (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <input_iterator ITERATOR_OF_ADDABLE>
        Set_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<T> EQUALS_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
        Set_Array (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Set_Array& operator= (Set_Array&& rhs) noexcept = default;
        nonvirtual Set_Array& operator= (const Set_Array& rhs)     = default;

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
        /**
         *  \brief  Reduce the space used to store the Set<T> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete sequence, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

    private:
        class IImplRepBase_;
        template <IEqualsComparer<T> EQUALS_COMPARER>
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

#include "Set_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_Array_h_ */

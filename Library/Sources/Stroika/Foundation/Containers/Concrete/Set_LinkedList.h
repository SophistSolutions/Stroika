/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Set_LinkedList<T> is an LinkedList-based concrete implementation of the Set<T> container pattern.
     *
     * \note Performance Notes:
     *      Set_stdset<T> is a compact representation, but ONLY reasonable for very small sets. PErformance is O(N) as set grows.
     *
     *      o   GetLength () is O(N)
     *      o   Uses Memory::UseBlockAllocationIfAppropriate
     *      o   Additions and Removals are generally O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Set_LinkedList : public Set<T> {
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
        Set_LinkedList ();
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>* = nullptr>
        explicit Set_LinkedList (EQUALS_COMPARER&& equalsComparer);
        Set_LinkedList (Set_LinkedList&& src) noexcept      = default;
        Set_LinkedList (const Set_LinkedList& src) noexcept = default;
        Set_LinkedList (const initializer_list<value_type>& src);
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>* = nullptr>
        Set_LinkedList (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Set_LinkedList<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Set_LinkedList (ITERABLE_OF_ADDABLE&& src);
        template <typename EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        Set_LinkedList (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        Set_LinkedList (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);
        template <typename EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        Set_LinkedList (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    public:
        /**
         */
        nonvirtual Set_LinkedList& operator= (Set_LinkedList&& rhs) = default;
        nonvirtual Set_LinkedList& operator= (const Set_LinkedList& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _SetRepSharedPtr      = typename inherited::_IRepSharedPtr;

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

#include "Set_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_ */

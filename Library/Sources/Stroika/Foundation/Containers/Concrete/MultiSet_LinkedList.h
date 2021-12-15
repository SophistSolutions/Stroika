/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../MultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_ 1

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     * \note Performance Notes:
     *      o   GetLength () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class MultiSet_LinkedList : public MultiSet<T, TRAITS> {
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
        MultiSet_LinkedList ();
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>* = nullptr>
        explicit MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer);
        MultiSet_LinkedList (MultiSet_LinkedList&& src) noexcept      = default;
        MultiSet_LinkedList (const MultiSet_LinkedList& src) noexcept = default;
        MultiSet_LinkedList (const initializer_list<T>& src);
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>* = nullptr>
        MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src);
        MultiSet_LinkedList (const initializer_list<value_type>& src);
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> ()>* = nullptr>
        MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<MultiSet_LinkedList<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit MultiSet_LinkedList (ITERABLE_OF_ADDABLE&& src);
        template <typename EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        MultiSet_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<EQUALS_COMPARER, T> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual MultiSet_LinkedList& operator= (MultiSet_LinkedList&& rhs) noexcept = default;
        nonvirtual MultiSet_LinkedList& operator= (const MultiSet_LinkedList& rhs) = default;

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

#include "MultiSet_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_ */

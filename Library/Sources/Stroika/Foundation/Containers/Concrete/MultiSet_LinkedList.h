/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
     *      o   size () is O(N)
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
        template <IEqualsComparer<T> EQUALS_COMPARER>
        explicit MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer);
        MultiSet_LinkedList (MultiSet_LinkedList&& src) noexcept      = default;
        MultiSet_LinkedList (const MultiSet_LinkedList& src) noexcept = default;
        MultiSet_LinkedList (const initializer_list<T>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src);
        MultiSet_LinkedList (const initializer_list<value_type>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <ranges::range ITERABLE_OF_ADDABLE,
                  enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<MultiSet_LinkedList<T, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit MultiSet_LinkedList (ITERABLE_OF_ADDABLE&& src);
        template <IEqualsComparer<T> EQUALS_COMPARER, ranges::range ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <input_iterator ITERATOR_OF_ADDABLE>
        MultiSet_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<T> EQUALS_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
        MultiSet_LinkedList (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual MultiSet_LinkedList& operator= (MultiSet_LinkedList&& rhs) noexcept = default;
        nonvirtual MultiSet_LinkedList& operator= (const MultiSet_LinkedList& rhs)     = default;

    private:
        class IImplRepBase_;
        template <IEqualsComparer<T> EQUALS_COMPARER>
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;
    };

}

#include "MultiSet_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_ */

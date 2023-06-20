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
        template <IEqualsComparer<T> EQUALS_COMPARER>
        explicit MultiSet_Array (EQUALS_COMPARER&& equalsComparer);
        MultiSet_Array (MultiSet_Array&& src) noexcept      = default;
        MultiSet_Array (const MultiSet_Array& src) noexcept = default;
        MultiSet_Array (const initializer_list<T>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src);
        MultiSet_Array (const initializer_list<value_type>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <IIterable<CountedValue<T>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, MultiSet_Array<T, TRAITS>>)
        explicit MultiSet_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : MultiSet_Array{}
        {
            if constexpr (Configuration::IHasSize<ITERABLE_OF_ADDABLE>) {
                reserve (src.size ());
            }
            AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IEqualsComparer<T> EQUALS_COMPARER, IIterable<CountedValue<T>> ITERABLE_OF_ADDABLE>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<CountedValue<T>> ITERATOR_OF_ADDABLE>
        MultiSet_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<CountedValue<T>> ITERATOR_OF_ADDABLE>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual MultiSet_Array& operator= (MultiSet_Array&& rhs) noexcept = default;
        nonvirtual MultiSet_Array& operator= (const MultiSet_Array& rhs)     = default;

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
         *  \brief  Reduce the space used to store the Multiset<T> contents.
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
#include "MultiSet_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_*/

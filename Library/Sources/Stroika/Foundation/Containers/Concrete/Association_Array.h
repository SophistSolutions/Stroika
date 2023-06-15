/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Association.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Association_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Association_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an Array-based concrete implementation of the Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_Array : public Association<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v  = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \see docs on Association<> constructor
         */
        Association_Array ();
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        explicit Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer);
        Association_Array (Association_Array&& src) noexcept      = default;
        Association_Array (const Association_Array& src) noexcept = default;
        Association_Array (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        template <IIterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE,
                  enable_if_t<not derived_from<decay_t<ITERABLE_OF_ADDABLE>, Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>>>* = nullptr>
        explicit Association_Array (ITERABLE_OF_ADDABLE&& src);
#else
        template <IIterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        explicit Association_Array (ITERABLE_OF_ADDABLE&& src)
            requires (not derived_from<decay_t<ITERABLE_OF_ADDABLE>, Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>>);
#endif
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Association_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Association_Array& operator= (Association_Array&& rhs) noexcept = default;
        nonvirtual Association_Array& operator= (const Association_Array& rhs)     = default;

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
         *  \note Note that this does not affect the semantics of the Association.
         * 
         *  \req slotsAllocated >= size ()
         */
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        /**
         *  \brief  Reduce the space used to store the Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete Association, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

    private:
        class IImplRepBase_;
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

#include "Association_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_Array_h_ */

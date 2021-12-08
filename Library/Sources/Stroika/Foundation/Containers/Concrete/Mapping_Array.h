/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Array_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an Array-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
     *
     * \note Performance Notes:
     *      o   GetLength () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_Array : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v  = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \see docs on Mapping<> constructor
         */
        Mapping_Array ();
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        explicit Mapping_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer);
        Mapping_Array (Mapping_Array&& src) noexcept      = default;
        Mapping_Array (const Mapping_Array& src) noexcept = default;
        Mapping_Array (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        Mapping_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        Mapping_Array (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        Mapping_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Mapping_Array (ITERABLE_OF_ADDABLE&& src);
        template <typename KEY_EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        Mapping_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        Mapping_Array (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);
        template <typename KEY_EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        Mapping_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    public:
        nonvirtual Mapping_Array& operator= (Mapping_Array&& rhs) = default;
        nonvirtual Mapping_Array& operator= (const Mapping_Array& rhs) = default;

    public:
        /**
         *  \brief  Reduce the space used to store the Mapping_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> contents.
         *
         *  This has no semantics, no observable behavior. But depending on the representation of
         *  the concrete Mapping, calling this may save memory.
         */
        nonvirtual void shrink_to_fit ();

    public:
        /*
         * This optional API allows pre-reserving space as an optimization.
         */
        nonvirtual size_t GetCapacity () const;
        nonvirtual void   SetCapacity (size_t slotsAlloced);

    public:
        /**
         *  STL-ish alias for GetCapacity ();
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         *  STL-ish alias for SetCapacity ();
         */
        nonvirtual void reserve (size_t slotsAlloced);

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _MappingRepSharedPtr  = typename inherited::_IRepSharedPtr;

    private:
        class IImplRepBase_;
        template <typename KEY_EQUALS_COMPARER>
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

#include "Mapping_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_Array_h_ */

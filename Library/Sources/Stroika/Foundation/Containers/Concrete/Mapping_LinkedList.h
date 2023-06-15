/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Mapping.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an LinkedList-based concrete implementation of the Mapping<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::MappingTraitsType> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_LinkedList : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
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
        Mapping_LinkedList ();
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        explicit Mapping_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer);
        Mapping_LinkedList (Mapping_LinkedList&& src) noexcept      = default;
        Mapping_LinkedList (const Mapping_LinkedList& src) noexcept = default;
        Mapping_LinkedList (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        Mapping_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOfT<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE,
                  enable_if_t<not is_base_of_v<Mapping_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Mapping_LinkedList (ITERABLE_OF_ADDABLE&& src);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOfT<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        Mapping_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIteratorOfT<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIteratorOfT<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Mapping_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Mapping_LinkedList& operator= (Mapping_LinkedList&& rhs) noexcept = default;
        nonvirtual Mapping_LinkedList& operator= (const Mapping_LinkedList& rhs)     = default;

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

#include "Mapping_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_LinkedList_h_ */

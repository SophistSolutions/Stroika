/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Set_stdset<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::set-based concrete implementation of the Set<T> container pattern.
     *
     *  \note   \em Implemenation   Just indirect to SortedSet_stdset<>, which in turn mostly indirects to std::set<T>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Set_stdset : public Set<T> {
    private:
        using inherited = Set<T>;

    public:
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        Set_stdset ();
        template <typename INORDER_COMPARER>
        explicit Set_stdset (const INORDER_COMPARER& inorderComparer);
        Set_stdset (const Set_stdset& src) = default;
        Set_stdset (const initializer_list<value_type>& src);
        Set_stdset (const ElementEqualityComparerType& equalsComparer, const initializer_list<T>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_base_of_v<Set_stdset<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Set_stdset (CONTAINER_OF_ADDABLE&& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>>* = nullptr>
        Set_stdset (const ElementEqualityComparerType& equalsComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_T>
        Set_stdset (COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end);
        template <typename COPY_FROM_ITERATOR_T>
        Set_stdset (const ElementEqualityComparerType& equalsComparer, COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end);

    public:
        /**
         */
        nonvirtual Set_stdset& operator= (const Set_stdset& rhs) = default;

    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _SetRepSharedPtr      = typename inherited::_IRepSharedPtr;

    private:
        class IImplRepBase_;
        template <typename INORDER_COMPARER>
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
#include "Set_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_stdset_h_ */

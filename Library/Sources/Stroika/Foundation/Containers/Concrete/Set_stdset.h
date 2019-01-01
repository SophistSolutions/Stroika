/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Set_stdset : public Set<T> {
    private:
        using inherited = Set<T>;

    public:
        /**
         */
        using EqualityComparerType = typename inherited::EqualityComparerType;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        Set_stdset ();
        template <typename INORDER_COMPARER>
        explicit Set_stdset (const INORDER_COMPARER& inorderComparer);
        Set_stdset (const Set_stdset& src) = default;
        Set_stdset (const initializer_list<T>& src);
        Set_stdset (const EqualityComparerType& equalsComparer, const initializer_list<T>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_base_of_v<Set_stdset<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Set_stdset (CONTAINER_OF_ADDABLE&& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_base_of_v<Set_stdset<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Set_stdset (const EqualityComparerType& equalsComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_T>
        Set_stdset (COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end);
        template <typename COPY_FROM_ITERATOR_T>
        Set_stdset (const EqualityComparerType& equalsComparer, COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end);

    public:
        /**
         */
        nonvirtual Set_stdset& operator= (const Set_stdset& rhs) = default;

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

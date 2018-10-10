/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Set_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Set_LinkedList<T> is an LinkedList-based concrete implementation of the Set<T> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Set_LinkedList : public Set<T> {
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
        Set_LinkedList ();
        template <typename EQUALS_COMPARER>
        explicit Set_LinkedList (const EQUALS_COMPARER& equalsComparer);
        Set_LinkedList (const Set_LinkedList& src) = default;
        Set_LinkedList (const initializer_list<T>& src);
        Set_LinkedList (const EqualityComparerType& equalsComparer, const initializer_list<T>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Set_LinkedList<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Set_LinkedList (CONTAINER_OF_ADDABLE&& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Set_LinkedList<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        Set_LinkedList (const EqualityComparerType& equalsComparer, CONTAINER_OF_ADDABLE&& src);
        template <typename COPY_FROM_ITERATOR_OF_T>
        Set_LinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
        template <typename COPY_FROM_ITERATOR_OF_T>
        Set_LinkedList (const EqualityComparerType& equalsComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
        /**
         */
        nonvirtual Set_LinkedList& operator= (const Set_LinkedList& rhs) = default;

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

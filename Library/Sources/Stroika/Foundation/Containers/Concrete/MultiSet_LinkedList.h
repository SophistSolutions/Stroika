/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../MultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_LinkedList_h_ 1

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals ()
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class MultiSet_LinkedList : public MultiSet<T, TRAITS> {
    private:
        using inherited = MultiSet<T, TRAITS>;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        MultiSet_LinkedList ();
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>* = nullptr>
        explicit MultiSet_LinkedList (const EQUALS_COMPARER& equalsComparer);
        MultiSet_LinkedList (const MultiSet<T, TRAITS>& src);
        MultiSet_LinkedList (const MultiSet_LinkedList<T, TRAITS>& src);
        MultiSet_LinkedList (const initializer_list<T>& src);
        MultiSet_LinkedList (const initializer_list<CountedValue<T>>& src);
        template <typename COPY_FROM_ITERATOR>
        MultiSet_LinkedList (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

    public:
        nonvirtual MultiSet_LinkedList<T, TRAITS>& operator= (const MultiSet_LinkedList<T, TRAITS>& rhs) = default;

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

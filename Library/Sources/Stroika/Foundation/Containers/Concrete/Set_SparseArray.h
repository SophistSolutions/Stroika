/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_SparseArray_h_
#define _Stroika_Foundation_Containers_Concrete_Set_SparseArray_h_

/**
 *  \file
 *
 *  \version    NOT IMPLEMENTED
 *
 *  TODO:
 *      @todo   Set_SparseArray<T> (or Set_SparseLinkedList>??)
 *              Assumes you can do T a, b, c; if (a < b and c< d) etc.So good for T = int..
 *
 *              Then store data as linkledlist pair<lb, ub> and walk and see if in any of the ranges.
 *              Or better � a tree (set<>> with top- compare done by LHS.
 *
 *              Nice to add coalese feature on add remove. A good � day to impement properly.
 *
 *              Probably also AsRuns () -> Iterable<Range<T,T>>
 *
 *              @see DisjointRange<> and DisjointDiscreteRange<>
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     */
    template <typename T, typename TRAITS = DefaultTraits::Set<T>>
    class Set_SparseArray : public Set<T, typename TRAITS::SetTraitsType> {
    private:
        using inherited = Set<T, typename TRAITS::SetTraitsType>;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        Set_SparseArray ();
        Set_SparseArray (const Set_SparseArray<T, TRAITS>& src);
        Set_SparseArray (const initializer_list<T>& src);
        template <typename CONTAINER_OF_T>
        explicit Set_SparseArray (const CONTAINER_OF_T& src);
        template <typename COPY_FROM_ITERATOR_OF_T>
        explicit Set_SparseArray (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
        nonvirtual Set_SparseArray<T, TRAITS>& operator= (const Set_SparseArray<T, TRAITS>& rhs) = default;

    private:
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

//#include    "Set_SparseArray.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_SparseArray_h_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedMultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_ 1

/**
  *  \file
  *
  *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
  *
  *  TODO:
  *
  *      @todo   Get use of Private::IteratorImplHelper_ working
  *
  *      @todo   Correctly implement override of Iterator<T>::IRep::Equals ()
  */

namespace Stroika::Foundation::Containers::Concrete {

    /**
         *
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
         *
         */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class SortedMultiSet_stdmap : public SortedMultiSet<T, TRAITS> {
    private:
        using inherited = SortedMultiSet<T, TRAITS>;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        SortedMultiSet_stdmap ();
        template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>* = nullptr>
        explicit SortedMultiSet_stdmap (const INORDER_COMPARER& inorderComparer);
        SortedMultiSet_stdmap (const SortedMultiSet_stdmap& src) = default;
        SortedMultiSet_stdmap (const initializer_list<T>& src);
        SortedMultiSet_stdmap (const initializer_list<CountedValue<T>>& src);
        template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_convertible_v<const CONTAINER_OF_T*, const SortedMultiSet_stdmap<T, TRAITS>*>>* = nullptr>
        SortedMultiSet_stdmap (const CONTAINER_OF_T& src);
        template <typename COPY_FROM_ITERATOR_OF_T>
        SortedMultiSet_stdmap (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
        /**
         */
        nonvirtual SortedMultiSet_stdmap& operator= (const SortedMultiSet_stdmap& rhs) = default;

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
#include "SortedMultiSet_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMultiSet_stdmap_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../MultiSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   MUST re-implement (clone SortedMultiSet_stdmap<>) code to avoid deadly include empbrace so we
 *              have the option to use this in the factory.
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     */
    template <typename T, typename TRAITS = MultiSet<T>>
    class MultiSet_stdmap : public MultiSet<T, TRAITS> {
    private:
        using inherited = MultiSet<T, TRAITS>;

    public:
        /**
         */
        using TraitsType = TRAITS;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        MultiSet_stdmap ();
        template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>* = nullptr>
        explicit MultiSet_stdmap (const INORDER_COMPARER& inorderComparer);
        template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_convertible_v<const CONTAINER_OF_T*, const MultiSet_stdmap<T>*>>* = nullptr>
        MultiSet_stdmap (const CONTAINER_OF_T& src);
        MultiSet_stdmap (const MultiSet_stdmap& src) = default;
        MultiSet_stdmap (const initializer_list<T>& src);
        MultiSet_stdmap (const initializer_list<CountedValue<T>>& src);
        template <typename COPY_FROM_ITERATOR>
        MultiSet_stdmap (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

    public:
        nonvirtual MultiSet_stdmap& operator= (const MultiSet_stdmap& rhs) = default;

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
#include "MultiSet_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_ */

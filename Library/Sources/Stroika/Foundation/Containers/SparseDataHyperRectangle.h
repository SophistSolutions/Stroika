/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SparseDataHyperRectangle_h_
#define _Stroika_Foundation_Containers_SparseDataHyperRectangle_h_ 1

#include "../StroikaPreComp.h"

#include "DataHyperRectangle.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  A SparseDataHyperRectangle<> is a DataHyperRectangle where you specify a special 'default' value, which will appear
     *  in any cell you 'get' without first setting (as if the hyper-rectangle was pre-initialized to that value).
     *
     * But default values don't show up when iterating.
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *        o Same as DataHyperRectangle<> comparisons
     */
    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle : public DataHyperRectangle<T, INDEXES...> {
    private:
        using inherited = DataHyperRectangle<T, INDEXES...>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SparseDataHyperRectangle<T, INDEXES...>;

    public:
        /**
         */
        SparseDataHyperRectangle (Configuration::ArgByValueType<T> defaultItem = {});
        SparseDataHyperRectangle (const SparseDataHyperRectangle<T, INDEXES...>& src);

    protected:
        using _IRepSharedPtr = typename inherited::_IRepSharedPtr;

    protected:
        explicit SparseDataHyperRectangle (const _IRepSharedPtr& src) noexcept;
        explicit SparseDataHyperRectangle (_IRepSharedPtr&& src) noexcept;

    public:
        nonvirtual SparseDataHyperRectangle<T, INDEXES...>& operator= (const SparseDataHyperRectangle<T, INDEXES...>& rhs) = default;
    };

    /**
     *  using SparseDataHyperRectangleN = SparseDataHyperRectangle<T, size_t REPEATED N TIMES>
     */
    template <typename T, size_t N>
    using SparseDataHyperRectangleN = typename Private_DataHyperRectangle_::template NTemplate<T, SparseDataHyperRectangle>::template Helper_<make_index_sequence<N>>::CombinedType;

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SparseDataHyperRectangle.inl"

#endif /*_Stroika_Foundation_Containers_SparseDataHyperRectangle_h_ */

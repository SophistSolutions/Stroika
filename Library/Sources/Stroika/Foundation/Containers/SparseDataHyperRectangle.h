/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
     */
    template <typename T, typename... INDEXES>
    class SparseDataHyperRectangle : public DataHyperRectangle<T, INDEXES...> {
    private:
        using inherited = DataHyperRectangle<T, INDEXES...>;

    protected:
        using _DataHyperRectangleRepSharedPtr = typename inherited::_DataHyperRectangleRepSharedPtr;

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
        explicit SparseDataHyperRectangle (const _DataHyperRectangleRepSharedPtr& src) noexcept;
        explicit SparseDataHyperRectangle (_DataHyperRectangleRepSharedPtr&& src) noexcept;

    public:
        nonvirtual SparseDataHyperRectangle<T, INDEXES...>& operator= (const SparseDataHyperRectangle<T, INDEXES...>& rhs) = default;
    };

    /**
     *  using SparseDataHyperRectangleN = SparseDataHyperRectangle<T, size_t REPEATED N TIMES>
     */
    template <typename T, size_t N>
    using SparseDataHyperRectangleN = typename Private_DataHyperRectangle_::template NTemplate<T, SparseDataHyperRectangle>::template Helper_<make_index_sequence<N>>::CombinedType;

    /**
     *  @todo see if there is a way to define this genericly using templates/sequences - SparseDataHyperRectangleN<N>
     */
    template <typename T>
    using SparseDataHyperRectangle1 [[deprecated ("in Stroika v2.1d24 - use SparseDataHyperRectangleN instead")]] = SparseDataHyperRectangleN<T, 1>;
    template <typename T>
    using SparseDataHyperRectangle2 [[deprecated ("in Stroika v2.1d24 - use SparseDataHyperRectangleN instead")]] = SparseDataHyperRectangleN<T, 2>;
    template <typename T>
    using SparseDataHyperRectangle3 [[deprecated ("in Stroika v2.1d24 - use SparseDataHyperRectangleN instead")]] = SparseDataHyperRectangleN<T, 3>;
    template <typename T>
    using SparseDataHyperRectangle4 [[deprecated ("in Stroika v2.1d24 - use SparseDataHyperRectangleN instead")]] = SparseDataHyperRectangleN<T, 4>;

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SparseDataHyperRectangle.inl"

#endif /*_Stroika_Foundation_Containers_SparseDataHyperRectangle_h_ */

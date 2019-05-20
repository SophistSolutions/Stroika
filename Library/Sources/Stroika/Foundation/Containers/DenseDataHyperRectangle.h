/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DenseDataHyperRectangle_h_
#define _Stroika_Foundation_Containers_DenseDataHyperRectangle_h_ 1

#include "../StroikaPreComp.h"

#include "DataHyperRectangle.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 */

namespace Stroika::Foundation::Containers {

    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  A DenseDataHyperRectangle<> is a DataHyperRectangle where you specify the max value for each dimension, and it is
     *  a programming (assertion) error to check/add items outside that range. And when iterating (e.g. GetLength ()) you always
     *  find all items.
     */
    template <typename T, typename... INDEXES>
    class DenseDataHyperRectangle : public DataHyperRectangle<T, INDEXES...> {
    private:
        using inherited = DataHyperRectangle<T, INDEXES...>;

    protected:
        using _DataHyperRectangleRepSharedPtr = typename inherited::_DataHyperRectangleRepSharedPtr;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = DenseDataHyperRectangle<T, INDEXES...>;

    public:
        DenseDataHyperRectangle (INDEXES... dimensions);
        DenseDataHyperRectangle (const DenseDataHyperRectangle<T, INDEXES...>& src);

    protected:
        explicit DenseDataHyperRectangle (const _DataHyperRectangleRepSharedPtr& src) noexcept;
        explicit DenseDataHyperRectangle (_DataHyperRectangleRepSharedPtr&& src) noexcept;

    public:
        nonvirtual DenseDataHyperRectangle<T, INDEXES...>& operator= (const DenseDataHyperRectangle<T, INDEXES...>& rhs) = default;
    };

    /**
     *  using DenseDataHyperRectangleN = DenseDataHyperRectangle<T, size_t REPEATED N TIMES>
     */
    template <typename T, size_t N>
    using DenseDataHyperRectangleN = typename Private_DataHyperRectangle_::template NTemplate<T, DenseDataHyperRectangle>::template Helper_<make_index_sequence<N>>::CombinedType;

    /**
     *  @todo see if there is a way to define this genericly using templates/sequences - DenseDataHyperRectangleN<N>
     */
    template <typename T>
    using DenseDataHyperRectangle1 [[deprecated ("in Stroika v2.1d24 - use SparseDataHyperRectangleN instead")]] = DenseDataHyperRectangleN<T, 1>;
    template <typename T>
    using DenseDataHyperRectangle2 [[deprecated ("in Stroika v2.1d24 - use SparseDataHyperRectangleN instead")]] = DenseDataHyperRectangleN<T, 2>;
    template <typename T>
    using DenseDataHyperRectangle3 [[deprecated ("in Stroika v2.1d24 - use SparseDataHyperRectangleN instead")]] = DenseDataHyperRectangleN<T, 3>;
    template <typename T>
    using DenseDataHyperRectangle4 [[deprecated ("in Stroika v2.1d24 - use SparseDataHyperRectangleN instead")]] = DenseDataHyperRectangleN<T, 4>;

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "DenseDataHyperRectangle.inl"

#endif /*_Stroika_Foundation_Containers_DenseDataHyperRectangle_h_ */

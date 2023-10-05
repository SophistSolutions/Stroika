/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
     *  a programming (assertion) error to check/add items outside that range. And when iterating (e.g. size ()) you always
     *  find all items.
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      @see inherited from DataHyperRectangle<>
     */
    template <typename T, typename... INDEXES>
    class [[nodiscard]] DenseDataHyperRectangle : public DataHyperRectangle<T, INDEXES...> {
    private:
        using inherited = DataHyperRectangle<T, INDEXES...>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = DenseDataHyperRectangle<T, INDEXES...>;

    public:
        /**
         */
        DenseDataHyperRectangle (INDEXES... dimensions);
        DenseDataHyperRectangle (const DenseDataHyperRectangle<T, INDEXES...>& src);

    protected:
        explicit DenseDataHyperRectangle (const shared_ptr<typename inherited::_IRep>& src) noexcept;
        explicit DenseDataHyperRectangle (shared_ptr<typename inherited::_IRep>&& src) noexcept;

    public:
        nonvirtual DenseDataHyperRectangle<T, INDEXES...>& operator= (const DenseDataHyperRectangle<T, INDEXES...>& rhs) = default;
    };

    /**
     *  using DenseDataHyperRectangleN = DenseDataHyperRectangle<T, size_t REPEATED N TIMES>
     */
    template <typename T, size_t N>
    using DenseDataHyperRectangleN =
        typename Private_DataHyperRectangle_::template NTemplate<T, DenseDataHyperRectangle>::template Helper_<make_index_sequence<N>>::CombinedType;

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "DenseDataHyperRectangle.inl"

#endif /*_Stroika_Foundation_Containers_DenseDataHyperRectangle_h_ */

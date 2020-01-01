/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../DenseDataHyperRectangle.h"

#ifndef _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_h_
#define _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   DenseDataHyperRectangle_Vector<T, INDEXES...> is an Dense Vector-based concrete implementation of the DenseDataHyperRectangle<T> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename... INDEXES>
    class DenseDataHyperRectangle_Vector : public DenseDataHyperRectangle<T, INDEXES...> {
    private:
        using inherited = DenseDataHyperRectangle<T, INDEXES...>;

    public:
        DenseDataHyperRectangle_Vector (INDEXES... dimensions);
        DenseDataHyperRectangle_Vector (const DenseDataHyperRectangle_Vector<T, INDEXES...>& src);

    public:
        nonvirtual DenseDataHyperRectangle_Vector<T, INDEXES...>& operator= (const DenseDataHyperRectangle_Vector<T, INDEXES...>& rhs);

    private:
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;
    };

    /**
     *  using DenseDataHyperRectangle_VectorN = DenseDataHyperRectangle_Vector<T, size_t REPEATED N TIMES>
     */
    template <typename T, size_t N>
    using DenseDataHyperRectangle_VectorN = typename Private_DataHyperRectangle_::template NTemplate<T, DenseDataHyperRectangle_Vector>::template Helper_<make_index_sequence<N>>::CombinedType;

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "DenseDataHyperRectangle_Vector.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_h_ */

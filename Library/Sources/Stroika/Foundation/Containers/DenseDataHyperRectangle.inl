/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DenseDataHyperRectangle_inl_
#define _Stroika_Foundation_Containers_DenseDataHyperRectangle_inl_

#include "Factory/DenseDataHyperRectangle_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************** DenseDataHyperRectangle<T, INDEXES...> ********************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    DenseDataHyperRectangle<T, INDEXES...>::DenseDataHyperRectangle (INDEXES... dimensions)
        : inherited{Factory::DenseDataHyperRectangle_Factory<T, INDEXES...> () (forward<INDEXES> (dimensions)...)}
    {
    }
    template <typename T, typename... INDEXES>
    inline DenseDataHyperRectangle<T, INDEXES...>::DenseDataHyperRectangle (const DenseDataHyperRectangle<T, INDEXES...>& src)
        : inherited{static_cast<const inherited&> (src)}
    {
    }
    template <typename T, typename... INDEXES>
    inline DenseDataHyperRectangle<T, INDEXES...>::DenseDataHyperRectangle (const shared_ptr<typename inherited::_IRep>& src) noexcept
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        this->_AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline DenseDataHyperRectangle<T, INDEXES...>::DenseDataHyperRectangle (shared_ptr<typename inherited::_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        this->_AssertRepValidType ();
    }

}

#endif /* _Stroika_Foundation_Containers_DenseDataHyperRectangle_inl_ */

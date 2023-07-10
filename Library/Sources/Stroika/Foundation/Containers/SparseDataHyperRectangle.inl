/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_
#define _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_

#include "Factory/SparseDataHyperRectangle_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************** SparseDataHyperRectangle<T, INDEXES...> *******************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (Configuration::ArgByValueType<T> defaultItem)
        : inherited{Factory::SparseDataHyperRectangle_Factory<T, INDEXES...>::Default () (defaultItem)}
    {
        this->_AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (const SparseDataHyperRectangle<T, INDEXES...>& src)
        : inherited{static_cast<const inherited&> (src)}
    {
        this->_AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (const shared_ptr<typename inherited::_IRep>& src) noexcept
        : inherited{(RequireNotNull (src), src)}
    {
        this->_AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (shared_ptr<typename inherited::_IRep>&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        this->_AssertRepValidType ();
    }

}

#endif /* _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_ */

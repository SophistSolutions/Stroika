/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
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
        : inherited{(RequireExpression (src != nullptr), src)}
    {
        this->_AssertRepValidType ();
    }
    template <typename T, typename... INDEXES>
    inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (shared_ptr<typename inherited::_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        this->_AssertRepValidType ();
    }

}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_
#define _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_

#include "Factory/SparseDataHyperRectangle_Factory.h"

namespace Stroika::Foundation {
    namespace Containers {

        /*
            ********************************************************************************
            ******************** SparseDataHyperRectangle<T, INDEXES...> *******************
            ********************************************************************************
            */
        template <typename T, typename... INDEXES>
        SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (Configuration::ArgByValueType<T> defaultItem)
            : inherited (move (Factory::SparseDataHyperRectangle_Factory<T, INDEXES...> () (defaultItem)))
        {
            this->_AssertRepValidType ();
        }
        template <typename T, typename... INDEXES>
        inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (const SparseDataHyperRectangle<T, INDEXES...>& src)
            : inherited (static_cast<const inherited&> (src))
        {
            this->_AssertRepValidType ();
        }
        template <typename T, typename... INDEXES>
        inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (const _DataHyperRectangleRepSharedPtr& src) noexcept
            : inherited ((RequireNotNull (src), src))
        {
            this->_AssertRepValidType ();
        }
        template <typename T, typename... INDEXES>
        inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (_DataHyperRectangleRepSharedPtr&& src) noexcept
            : inherited ((RequireNotNull (src), move (src)))
        {
            this->_AssertRepValidType ();
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_
#define _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_

#include "Factory/SparseDataHyperRectangle_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ******************** SparseDataHyperRectangle<T, INDEXES...> *******************
             ********************************************************************************
             */
            template <typename T, typename... INDEXES>
            SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (Configuration::ArgByValueType<T> defaultItem)
                : inherited (move (Concrete::SparseDataHyperRectangle_Factory<T, INDEXES...>::mk (defaultItem)))
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
            inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (const _SharedPtrIRep& src) noexcept
                : inherited ((RequireNotNull (src), src))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename... INDEXES>
            inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (_SharedPtrIRep&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_ */

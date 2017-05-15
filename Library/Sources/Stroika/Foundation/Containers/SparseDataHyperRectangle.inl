/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_
#define _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_

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
            //NYI   : inherited (inherited::template MakeSharedPtr<Rep_> (std::forward<INDEXES> (dimensions)...))
            {
            }
            template <typename T, typename... INDEXES>
            inline SparseDataHyperRectangle<T, INDEXES...>::SparseDataHyperRectangle (const SparseDataHyperRectangle<T, INDEXES...>& src)
                : inherited (static_cast<const inherited&> (src))
            {
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SparseDataHyperRectangle_inl_ */

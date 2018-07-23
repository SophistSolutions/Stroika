/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_inl_

#include "../Concrete/DenseDataHyperRectangle_Vector.h"

namespace Stroika::Foundation::Containers {

    template <typename T, typename... INDEXES>
    class DenseDataHyperRectangle_Vector;
}

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ******************** DenseDataHyperRectangle_Factory<T> ************************
     ********************************************************************************
     */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename T, typename... INDEXES>
    atomic<DenseDataHyperRectangle<T, INDEXES...> (*) (INDEXES...)> DenseDataHyperRectangle_Factory<T, INDEXES...>::sFactory_ (nullptr);
#endif
    template <typename T, typename... INDEXES>
    inline DenseDataHyperRectangle<T, INDEXES...> DenseDataHyperRectangle_Factory<T, INDEXES...>::operator() (INDEXES... dimensions)
    {
        /*
         *  Would have been more performant to just and assure always properly set, but to initialize
         *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
         *  with containers constructed before main.
         *
         *  This works more generally (and with hopefully modest enough performance impact).
         */
        if (auto f = sFactory_.load ()) {
            return f (forward<INDEXES> (dimensions)...);
        }
        else {
            return Default_ (forward<INDEXES> (dimensions)...);
        }
    }
    template <typename T, typename... INDEXES>
    void DenseDataHyperRectangle_Factory<T, INDEXES...>::Register (DenseDataHyperRectangle<T, INDEXES...> (*factory) (INDEXES...))
    {
        sFactory_ = factory;
    }
    template <typename T, typename... INDEXES>
    inline DenseDataHyperRectangle<T, INDEXES...> DenseDataHyperRectangle_Factory<T, INDEXES...>::Default_ (INDEXES... dimensions)
    {
        return Concrete::DenseDataHyperRectangle_Vector<T, INDEXES...>{forward<INDEXES> (dimensions)...};
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Factory_inl_ */

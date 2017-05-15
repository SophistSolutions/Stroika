/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_inl_

#include "../Concrete/DataHyperRectangle_Sparse_stdmap.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /*
                 ********************************************************************************
                 ************************ SparseDataHyperRectangle_Factory<T> *******************
                 ********************************************************************************
                 */
                template <typename T, typename... INDEXES>
                atomic<SparseDataHyperRectangle<T, INDEXES...> (*) ()> SparseDataHyperRectangle_Factory<T, INDEXES...>::sFactory_ (nullptr);
                template <typename T, typename... INDEXES>
                inline SparseDataHyperRectangle<T, INDEXES...> SparseDataHyperRectangle_Factory<T, INDEXES...>::mk ()
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f ();
                    }
                    else {
                        return Default_ ();
                    }
                }
                template <typename T, typename... INDEXES>
                void SparseDataHyperRectangle_Factory<T, INDEXES...>::Register (SparseDataHyperRectangle<T, INDEXES...> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template <typename T, typename... INDEXES>
                inline SparseDataHyperRectangle<T, INDEXES...> SparseDataHyperRectangle_Factory<T, INDEXES...>::Default_ ()
                {
                    return DataHyperRectangle_Sparse_stdmap<T, INDEXES...>{};
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_inl_ */

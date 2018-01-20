/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_inl_

#include "../Concrete/SparseDataHyperRectangle_stdmap.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename T, typename... INDEXES>
            class SparseDataHyperRectangle_stdmap;

            namespace Factory {

                /*
                 ********************************************************************************
                 ************************ SparseDataHyperRectangle_Factory<T> *******************
                 ********************************************************************************
                 */
                template <typename T, typename... INDEXES>
                atomic<SparseDataHyperRectangle<T, INDEXES...> (*) (Configuration::ArgByValueType<T>)> SparseDataHyperRectangle_Factory<T, INDEXES...>::sFactory_ (nullptr);
                template <typename T, typename... INDEXES>
                inline SparseDataHyperRectangle<T, INDEXES...> SparseDataHyperRectangle_Factory<T, INDEXES...>::New (Configuration::ArgByValueType<T> defaultItem)
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f (defaultItem);
                    }
                    else {
                        return Default_ (defaultItem);
                    }
                }
                template <typename T, typename... INDEXES>
                void SparseDataHyperRectangle_Factory<T, INDEXES...>::Register (SparseDataHyperRectangle<T, INDEXES...> (*factory) (Configuration::ArgByValueType<T>))
                {
                    sFactory_ = factory;
                }
                template <typename T, typename... INDEXES>
                inline SparseDataHyperRectangle<T, INDEXES...> SparseDataHyperRectangle_Factory<T, INDEXES...>::Default_ (Configuration::ArgByValueType<T> defaultItem)
                {
                    return Concrete::SparseDataHyperRectangle_stdmap<T, INDEXES...>{defaultItem};
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SparseDataHyperRectangle_Factory_inl_ */

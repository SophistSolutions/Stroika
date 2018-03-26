/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_inl_

#include "../Concrete/MultiSet_Array.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 ************ MultiSet_Factory<T, TRAITS, EQUALS_COMPARER> **********************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS, typename EQUALS_COMPARER>
                atomic<MultiSet<T, TRAITS> (*) (const EQUALS_COMPARER&)> MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::sFactory_ (nullptr);
                template <typename T, typename TRAITS, typename EQUALS_COMPARER>
                inline MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::MultiSet_Factory (const EQUALS_COMPARER& equalsComparer)
                    : fEqualsComparer_ (equalsComparer)
                {
                    static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "Equals comparer required with Set_Factory");
                }
                template <typename T, typename TRAITS, typename EQUALS_COMPARER>
                inline MultiSet<T, TRAITS> MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::operator() () const
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f (fEqualsComparer_);
                    }
                    else {
                        return Default_ (fEqualsComparer_);
                    }
                }
                template <typename T, typename TRAITS, typename EQUALS_COMPARER>
                void MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::Register (MultiSet<T, TRAITS> (*factory) (const EQUALS_COMPARER&))
                {
                    sFactory_ = factory;
                }
                template <typename T, typename TRAITS, typename EQUALS_COMPARER>
                inline MultiSet<T, TRAITS> MultiSet_Factory<T, TRAITS, EQUALS_COMPARER>::Default_ (const EQUALS_COMPARER& equalsComparer)
                {
                    return Concrete::MultiSet_Array<T, TRAITS> (equalsComparer);
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_inl_ */

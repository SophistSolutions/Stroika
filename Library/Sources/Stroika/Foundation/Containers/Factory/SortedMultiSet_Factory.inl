/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_

#include "../Concrete/SortedMultiSet_stdmap.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 ********************* SortedMultiSet_Factory<T, TRAITS> ************************
                 ********************************************************************************
                 */
                template <typename T, typename TRAITS, typename INORDER_COMPARER>
                atomic<SortedMultiSet<T, TRAITS> (*) (const INORDER_COMPARER&)> SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::sFactory_ (nullptr);
                template <typename T, typename TRAITS, typename INORDER_COMPARER>
                inline SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::SortedMultiSet_Factory (const INORDER_COMPARER& inOrderComparer)
                    : fInOrderComparer_ (inOrderComparer)
                {
                    static_assert (Common::IsInOrderComparer<INORDER_COMPARER> (), "InOrder comparer required with SortedMultiSet_Factory");
                }
                template <typename T, typename TRAITS, typename INORDER_COMPARER>
                inline SortedMultiSet<T, TRAITS> SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::operator() () const
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f (fInOrderComparer_);
                    }
                    else {
                        return Default_ (fInOrderComparer_);
                    }
                }
                template <typename T, typename TRAITS, typename INORDER_COMPARER>
                void SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::Register (SortedMultiSet<T, TRAITS> (*factory) (const INORDER_COMPARER&))
                {
                    sFactory_ = factory;
                }
                template <typename T, typename TRAITS, typename INORDER_COMPARER>
                inline SortedMultiSet<T, TRAITS> SortedMultiSet_Factory<T, TRAITS, INORDER_COMPARER>::Default_ (const INORDER_COMPARER& inOrderComparer)
                {
                    return Concrete::SortedMultiSet_stdmap<T, TRAITS> (inOrderComparer);
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_inl_ */

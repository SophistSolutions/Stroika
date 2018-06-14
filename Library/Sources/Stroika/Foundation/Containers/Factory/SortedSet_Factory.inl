/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_

#include "../Concrete/SortedSet_stdset.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 ************************ SortedSet_Factory<T, INORDER_COMPARER> ****************
                 ********************************************************************************
                 */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
                template <typename T, typename INORDER_COMPARER>
                atomic<SortedSet<T> (*) (const INORDER_COMPARER&)> SortedSet_Factory<T, INORDER_COMPARER>::sFactory_ (nullptr);
#endif
                template <typename T, typename INORDER_COMPARER>
                inline SortedSet_Factory<T, INORDER_COMPARER>::SortedSet_Factory (const INORDER_COMPARER& inorderComparer)
                    : fInorderComparer_ (inorderComparer)
                {
                }
                template <typename T, typename INORDER_COMPARER>
                inline SortedSet<T> SortedSet_Factory<T, INORDER_COMPARER>::operator() () const
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f (fInorderComparer_);
                    }
                    else {
                        return Default_ (fInorderComparer_);
                    }
                }
                template <typename T, typename INORDER_COMPARER>
                inline void SortedSet_Factory<T, INORDER_COMPARER>::Register (SortedSet<T> (*factory) (const INORDER_COMPARER&))
                {
                    sFactory_ = factory;
                }
                template <typename T, typename INORDER_COMPARER>
                inline SortedSet<T> SortedSet_Factory<T, INORDER_COMPARER>::Default_ (const INORDER_COMPARER& inorderComparer)
                {
                    return Concrete::SortedSet_stdset<T> (inorderComparer);
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */

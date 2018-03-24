/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_inl_

#include "../Concrete/SortedCollection_LinkedList.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Factory {

                /*
                 ********************************************************************************
                 **************** SortedCollection_Factory<T, INORDER_COMPARER> *****************
                 ********************************************************************************
                 */
                template <typename T, typename INORDER_COMPARER>
                atomic<SortedCollection<T> (*) (const INORDER_COMPARER&)> SortedCollection_Factory<T, INORDER_COMPARER>::sFactory_ (nullptr);
                template <typename T, typename INORDER_COMPARER>
                inline SortedCollection_Factory<T, INORDER_COMPARER>::SortedCollection_Factory (const INORDER_COMPARER& inorderComparer)
                    : fLessComparer_ (inorderComparer)
                {
                }
                template <typename T, typename INORDER_COMPARER>
                inline SortedCollection<T> SortedCollection_Factory<T, INORDER_COMPARER>::operator() () const
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f (fLessComparer_);
                    }
                    else {
                        return Default_ (fLessComparer_);
                    }
                }
                template <typename T, typename INORDER_COMPARER>
                void SortedCollection_Factory<T, INORDER_COMPARER>::Register (SortedCollection<T> (*factory) (const INORDER_COMPARER&))
                {
                    sFactory_ = factory;
                }
                template <typename T, typename INORDER_COMPARER>
                inline SortedCollection<T> SortedCollection_Factory<T, INORDER_COMPARER>::Default_ (const INORDER_COMPARER& inorderComparer)
                {
                    return Concrete::SortedCollection_LinkedList<T> (inorderComparer);
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */

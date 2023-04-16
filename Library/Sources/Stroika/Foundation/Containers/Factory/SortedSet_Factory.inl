/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_

#include "../Concrete/SortedSet_stdset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ************************ SortedSet_Factory<T, INORDER_COMPARER> ****************
     ********************************************************************************
     */
    template <typename T, typename INORDER_COMPARER>
    constexpr SortedSet_Factory<T, INORDER_COMPARER>::SortedSet_Factory (const INORDER_COMPARER& inorderComparer, const Hints& hints)
        : fInorderComparer_{inorderComparer}
        , fHints_{hints}
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
        return Concrete::SortedSet_stdset<T>{inorderComparer};
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedSet_Factory_inl_ */

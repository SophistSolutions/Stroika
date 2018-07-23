/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_

#include "../Concrete/SortedMapping_stdmap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
        ********************************************************************************
        ************ SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> ***************
        ********************************************************************************
        */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_INORDER_COMPARER>
    atomic<SortedMapping<KEY_TYPE, VALUE_TYPE> (*) (const KEY_INORDER_COMPARER&)> SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::sFactory_ (nullptr);
#endif
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_INORDER_COMPARER>
    inline SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::SortedMapping_Factory (const KEY_INORDER_COMPARER& keyInOrderComparer)
        : fInOrderComparer_ (keyInOrderComparer)
    {
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_INORDER_COMPARER>
    inline SortedMapping<KEY_TYPE, VALUE_TYPE> SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::operator() () const
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
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_INORDER_COMPARER>
    void SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::Register (SortedMapping<KEY_TYPE, VALUE_TYPE> (*factory) (const KEY_INORDER_COMPARER&))
    {
        sFactory_ = factory;
    }
    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_INORDER_COMPARER>
    inline SortedMapping<KEY_TYPE, VALUE_TYPE> SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::Default_ (const KEY_INORDER_COMPARER& inOrderComparer)
    {
        return Concrete::SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE> (inOrderComparer);
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_inl_ */

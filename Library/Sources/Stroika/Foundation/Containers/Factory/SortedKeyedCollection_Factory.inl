/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_Factory_inl_

//#include "../Concrete/SortedKeyedCollection_stdmap.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ********** SortedKeyedCollection_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> *********
     ********************************************************************************
     */
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_INORDER_COMPARER>
    atomic<SortedKeyedCollection<KEY_TYPE, VALUE_TYPE> (*) (const KEY_INORDER_COMPARER&)> SortedKeyedCollection_Factory<KEY_TYPE, VALUE_TYPE, KEY_INORDER_COMPARER>::sFactory_ (nullptr);
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_INORDER_COMPARER>
    inline SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_INORDER_COMPARER>::SortedKeyedCollection_Factory (const KEY_INORDER_COMPARER& keyInOrderComparer)
        : fInOrderComparer_{keyInOrderComparer}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS> SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_INORDER_COMPARER>::operator() () const
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
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_INORDER_COMPARER>
    void SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_INORDER_COMPARER>::Register (SortedKeyedCollection<T, KEY_TYPE, TRAITS> (*factory) (const KEY_INORDER_COMPARER&))
    {
        sFactory_ = factory;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS> SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_INORDER_COMPARER>::Default_ (const KEY_INORDER_COMPARER& inOrderComparer)
    {
        // return Concrete::SortedKeyedCollection_stdmap<KEY_TYPE, VALUE_TYPE> {inOrderComparer};
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_Factory_inl_ */

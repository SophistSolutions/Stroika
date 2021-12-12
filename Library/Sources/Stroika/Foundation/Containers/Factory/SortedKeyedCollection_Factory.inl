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

#include "../Concrete/SortedKeyedCollection_stdset.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ********** SortedKeyedCollection_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> *********
     ********************************************************************************
     */
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
    atomic<SortedKeyedCollection<T, KEY_TYPE, TRAITS> (*) (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& keyComparer)> SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_INORDER_COMPARER>::sFactory_{nullptr};
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
    inline SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_INORDER_COMPARER>::SortedKeyedCollection_Factory (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& keyComparer)
        : fKeyExtractor_{keyExtractor}
        , fInOrderComparer_{keyComparer}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS> SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_INORDER_COMPARER>::operator() () const
    {
        /*
         *  Would have been more performant to just and assure always properly set, but to initialize
         *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
         *  with containers constructed before main.
         *
         *  This works more generally (and with hopefully modest enough performance impact).
         */
        if (auto f = sFactory_.load ()) {
            return f (fKeyExtractor_, fInOrderComparer_);
        }
        else {
            return Default_ (fKeyExtractor_, fInOrderComparer_);
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
    void SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_INORDER_COMPARER>::Register (SortedKeyedCollection<T, KEY_TYPE, TRAITS> (*factory) (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& keyComparer))
    {
        sFactory_ = factory;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EXTRACTOR, typename KEY_INORDER_COMPARER>
    inline SortedKeyedCollection<T, KEY_TYPE, TRAITS> SortedKeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_INORDER_COMPARER>::Default_ (const KEY_EXTRACTOR& keyExtractor, const KEY_INORDER_COMPARER& keyComparer)
    {
        return Concrete::SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>{keyExtractor, keyComparer};
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_Factory_inl_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_inl_

#include "../Concrete/KeyedCollection_LinkedList.h"

namespace Stroika::Foundation::Containers::Factory {

    /*
     ********************************************************************************
     ************** KeyedCollection_Factory<T, KEY_TYPE, TRAITS> ********************
     ********************************************************************************
     */
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS>
    atomic<KeyedCollection<T, KEY_TYPE, TRAITS> (*) (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)> KeyedCollection_Factory<T, KEY_TYPE, TRAITS>::sFactory_ (nullptr);
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection_Factory<T, KEY_TYPE, TRAITS>::KeyedCollection_Factory (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)
        : fKeyExtractorType_{keyExtractor}
        , fKeyEqualsComparer_{keyComparer}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS> KeyedCollection_Factory<T, KEY_TYPE, TRAITS>::operator() () const
    {
        /*
         *  Would have been more performant to just and assure always properly set, but to initialize
         *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
         *  with containers constructed before main.
         *
         *  This works more generally (and with hopefully modest enough performance impact).
         */
        if (auto f = sFactory_.load ()) {
            return f (fKeyExtractorType_, fKeyEqualsComparer_);
        }
        else {
            return Default_ (fKeyExtractorType_, fKeyEqualsComparer_);
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    void KeyedCollection_Factory<T, KEY_TYPE, TRAITS>::Register (KeyedCollection<T, KEY_TYPE, TRAITS> (*factory) (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer))
    {
        sFactory_ = factory;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS> KeyedCollection_Factory<T, KEY_TYPE, TRAITS>::Default_ (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)
    {
        /*
         *  Note - though this is not an efficient implementation of KeyedCollection<> for large sizes, its probably the most
         *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
         *  a hash function. And its quite reasonable for small KeyedCollection's - which are often the case.
         *
         *  Calls may use an explicit initializer of KeyedCollection_xxx<> to get better performance for large sized
         *  maps.
         */
        return Concrete::KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS>{keyExtractor, keyComparer};
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_inl_ */

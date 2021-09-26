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
     ****** KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EQUALS_COMPARER> *******
     ********************************************************************************
     */
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EQUALS_COMPARER>
    atomic<KeyedCollection<T, KEY_TYPE, TRAITS> (*) (KeyExtractorType keyExtractor, KEY_EQUALS_COMPARER keyComparer)> KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EQUALS_COMPARER>::sFactory_ (nullptr);
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EQUALS_COMPARER>
    inline KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EQUALS_COMPARER>::KeyedCollection_Factory (KeyExtractorType keyExtractor, KEY_EQUALS_COMPARER keyComparer)
        : fKeyExtractorType_{keyExtractor}
        , fKeyEqualsComparer_{keyComparer}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EQUALS_COMPARER>
    inline KeyedCollection<T, KEY_TYPE, TRAITS> KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EQUALS_COMPARER>::operator() () const
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
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EQUALS_COMPARER>
    void KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EQUALS_COMPARER>::Register (KeyedCollection<T, KEY_TYPE, TRAITS> (*factory) (KeyExtractorType keyExtractor, KEY_EQUALS_COMPARER keyComparer))
    {
        sFactory_ = factory;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_EQUALS_COMPARER>
    inline KeyedCollection<T, KEY_TYPE, TRAITS> KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EQUALS_COMPARER>::Default_ (KeyExtractorType keyExtractor, KEY_EQUALS_COMPARER keyComparer)
    {
        /*
         *  Note - though this is not an efficient implementation of KeyedCollection<> for large sizes, its probably the most
         *  efficeint representation which adds no requirements to KEY_TYPE, such as operator< (or a traits less) or
         *  a hash function. And its quite reasonable for small KeyedCollection's - which are often the case.
         *
         *  Calls may use an explicit initializer of KeyedCollection_xxx<> to get better performance for large sized
         *  maps.
         */
        return Concrete::KeyedCollection_LinkedList<T, KEY_TYPE, TRAITS, KEY_EQUALS_COMPARER>{keyExtractor, keyComparer};
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_inl_ */

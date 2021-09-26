/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSortedKeyedCollection_Fastest,
 *              mkSortedKeyedCollection_Smallest, mkSortedKeyedCollectionWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 */

namespace Stroika::Foundation::Containers {
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class SortedKeyedCollection;
}

namespace Stroika::Foundation::Containers::Factory {

    template <typename T, typename KEY_TYPE, typename TRAITS, typename KEY_INORDER_COMPARER = less<KEY_TYPE>>
    class SortedKeyedCollection_Factory {
    private:
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
        static atomic<SortedKeyedCollection<KEY_TYPE, VALUE_TYPE> (*) (const KEY_INORDER_COMPARER&)> sFactory_;
#else
        static inline atomic<SortedKeyedCollection<T, KEY_TYPE, TRAITS> (*) (const KEY_INORDER_COMPARER&)> sFactory_{nullptr};
#endif

    public:
        static_assert (Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER> (), "StrictInOrder comparer required with SortedKeyedCollection");

    public:
        SortedKeyedCollection_Factory (const KEY_INORDER_COMPARER& keyInOrderComparer = {});

    public:
        /**
         *  You can call this directly, but there is no need, as the Mapping<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual SortedKeyedCollection<T, KEY_TYPE, TRAITS> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given Mapping<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
         */
        static void Register (SortedKeyedCollection<T, KEY_TYPE, TRAITS> (*factory) (const KEY_INORDER_COMPARER&) = nullptr);

    private:
        KEY_INORDER_COMPARER fInOrderComparer_;

    private:
        static SortedKeyedCollection<T, KEY_TYPE, TRAITS> Default_ (const KEY_INORDER_COMPARER&);
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedKeyedCollection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_Factory_h_ */

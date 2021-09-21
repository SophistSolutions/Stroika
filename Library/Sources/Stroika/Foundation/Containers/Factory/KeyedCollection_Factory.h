/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <type_traits>

/**
 *  \file
 *
 *  TODO:
 */

namespace Stroika::Foundation::Containers {

    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection;

}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a KeyedCollection<> container
     *
     *  Note - you can override the underlying factory dynamically by calling KeyedCollection_Factory<T,KEY_TYPE,TRAITS>::Register (), or
     *  replace it statically by template-specializing KeyedCollection_Factory<T,TRAITS>::New () - though the later is trickier.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection_Factory {
    public:
        using KeyExtractorType        = typename KeyedCollection<T, KEY_TYPE, TRAITS>::KeyExtractorType;
        using KeyEqualityComparerType = typename KeyedCollection<T, KEY_TYPE, TRAITS>::KeyEqualityComparerType;

    private:
#if qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
        static atomic<KeyedCollection<T, KEY_TYPE, TRAITS> (*) (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)> sFactory_;
#else
        static inline atomic<KeyedCollection<T, KEY_TYPE, TRAITS> (*) (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)> sFactory_{nullptr};
#endif

    public:
        KeyedCollection_Factory (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer);

    public:
        /**
         *  You can call this directly, but there is no need, as the KeyedCollection<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual KeyedCollection<T, KEY_TYPE, TRAITS> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given KeyedCollection<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
         */
        static void Register (KeyedCollection<T, KEY_TYPE, TRAITS> (*factory) (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer) = nullptr);

    private:
        KeyExtractorType        fKeyExtractorType_;
        KeyEqualityComparerType fKeyEqualsComparer_;

    private:
        static KeyedCollection<T, KEY_TYPE, TRAITS> Default_ (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer);
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "KeyedCollection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_KeyedCollection_Factory_h_ */

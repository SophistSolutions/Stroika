/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSortedMapping_Fastest,
 *              mkSortedMapping_Smallest, mkSortedMappingWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 */

namespace Stroika::Foundation::Containers {
    template <typename KEY_TYPE, typename VALUE_TYPE>
    class SortedMapping;
}

namespace Stroika::Foundation::Containers::Factory {

    template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_INORDER_COMPARER = less<KEY_TYPE>>
    class SortedMapping_Factory {
    private:
        static inline atomic<SortedMapping<KEY_TYPE, VALUE_TYPE> (*) (const KEY_INORDER_COMPARER&)> sFactory_{nullptr};

    public:
        static_assert (not is_reference_v<KEY_TYPE> and not is_reference_v<VALUE_TYPE> and not is_reference_v<KEY_INORDER_COMPARER>, "typically if this fails its because a (possibly indirect) caller forgot to use forward<>(), or remove_cvref_t");
        static_assert (Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER> (), "StrictInOrder comparer required with SortedMapping");

    public:
        SortedMapping_Factory (const KEY_INORDER_COMPARER& keyInOrderComparer = {});

    public:
        /**
         *  You can call this directly, but there is no need, as the Mapping<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual SortedMapping<KEY_TYPE, VALUE_TYPE> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given Mapping<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
         */
        static void Register (SortedMapping<KEY_TYPE, VALUE_TYPE> (*factory) (const KEY_INORDER_COMPARER&) = nullptr);

    private:
        [[no_unique_address]] const KEY_INORDER_COMPARER fInOrderComparer_;

    private:
        static SortedMapping<KEY_TYPE, VALUE_TYPE> Default_ (const KEY_INORDER_COMPARER&);
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedMapping_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMapping_Factory_h_ */

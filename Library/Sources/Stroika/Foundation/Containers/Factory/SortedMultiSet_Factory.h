/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 */

namespace Stroika::Foundation::Containers {
    template <typename T, typename TRAITS>
    class SortedMultiSet;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedMultiSet<> container
     *
     *  Note - you can override the underlying factory dynamically by calling SortedMultiSet_Factory<T,TRAITS>::Register ().
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename TRAITS, typename INORDER_COMPARER>
    class SortedMultiSet_Factory {
    private:
        static inline atomic<SortedMultiSet<T, TRAITS> (*) (const INORDER_COMPARER&)> sFactory_{nullptr};

    public:
        static_assert (not is_reference_v<T> and not is_reference_v<INORDER_COMPARER>,
                       "typically if this fails its because a (possibly indirect) caller forgot to use forward<>(), or remove_cvref_t");
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedMultiSet");

    public:
        /**
         *  Hints can be used in factory constructor to guide the choice of the best container implementation/backend.
         */
        struct Hints {};

    public:
        constexpr SortedMultiSet_Factory (const INORDER_COMPARER& inorderComparer, const Hints& hints = {});

    public:
        /**
         *  You can call this directly, but there is no need, as the SortedMultiSet<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual SortedMultiSet<T, TRAITS> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given SortedMultiSet<T,TRAITS>. Note this is a global change.
         */
        static void Register (SortedMultiSet<T, TRAITS> (*factory) (const INORDER_COMPARER&) = nullptr);

    private:
        [[no_unique_address]] const INORDER_COMPARER fInOrderComparer_;
        [[no_unique_address]] const Hints            fHints_;

    private:
        static SortedMultiSet<T, TRAITS> Default_ (const INORDER_COMPARER&);
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedMultiSet_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedMultiSet_Factory_h_ */

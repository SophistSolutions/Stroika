/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *
 */

namespace Stroika::Foundation::Containers {

    template <typename T>
    class SortedCollection;
}
namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedCollection<> container
     *
     *  Note - you can override the underlying factory dynamically by calling SortedCollection_Factory<T,INORDER_COMPARER>::Register (), or
     *  replace it statically by template-specializing SortedCollection_Factory<T,INORDER_COMPARER>::New () - though the later is trickier.
     *
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename INORDER_COMPARER = less<T>>
    class SortedCollection_Factory {
    private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
        static atomic<SortedCollection<T> (*) (const INORDER_COMPARER&)> sFactory_;
#else
        static inline atomic<SortedCollection<T> (*) (const INORDER_COMPARER&)> sFactory_{nullptr};
#endif

    public:
        static_assert (Common::IsStrictInOrderComparer<INORDER_COMPARER> (), "StrictInOrder comparer required with SortedCollection");

    public:
        SortedCollection_Factory (const INORDER_COMPARER& inorderComparer);

    public:
        /**
         *  You can call this directly, but there is no need, as the SortedCollection<T> CTOR does so automatically.
         */
        nonvirtual SortedCollection<T> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given Collection<T>. Note this is a global change.
         */
        static void Register (SortedCollection<T> (*factory) (const INORDER_COMPARER&) = nullptr);

    private:
        [[NO_UNIQUE_ADDRESS_ATTR]] const INORDER_COMPARER fInorderComparer_;

    private:
        static SortedCollection<T> Default_ (const INORDER_COMPARER& inorderComparer);
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedCollection_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedCollection_Factory_h_ */

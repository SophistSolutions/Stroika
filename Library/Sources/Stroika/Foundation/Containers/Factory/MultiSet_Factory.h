/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkMultiSet_Fastest,
 *              mkMultiSet_Smallest, mkMultiSetWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 */

namespace Stroika::Foundation::Containers {

    template <typename T, typename TRAITS>
    class MultiSet;
}

namespace Stroika::Foundation::Containers::Factory {

    /**
    *  \brief   Singleton factory object - Used to create the default backend implementation of a MultiSet<> container
    *
    *  Note - you can override the underlying factory dynamically by calling MultiSet_Factory<T,TRAITS>::Register (), or
    *  replace it statically by template-specializing MultiSet_Factory<T,TRAITS>::New () - though the later is trickier.
    *
    *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
    */
    template <typename T, typename TRAITS, typename EQUALS_COMPARER>
    class MultiSet_Factory {
    private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
        static atomic<MultiSet<T, TRAITS> (*) (const EQUALS_COMPARER&)> sFactory_;
#else
        static inline atomic<MultiSet<T, TRAITS> (*) (const EQUALS_COMPARER&)> sFactory_{nullptr};
#endif

    public:
        static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "Equals comparer required with MultiSet_Factory");

    public:
        MultiSet_Factory (const EQUALS_COMPARER& equalsComparer);

    public:
        /**
         *  You can call this directly, but there is no need, as the MultiSet<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual MultiSet<T, TRAITS> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given MultiSet<T,TRAITS>. Note this is a global change.
         */
        static void Register (MultiSet<T, TRAITS> (*factory) (const EQUALS_COMPARER&) = nullptr);

    private:
        const EQUALS_COMPARER fEqualsComparer_;

    private:
        static MultiSet<T, TRAITS> Default_ (const EQUALS_COMPARER&);
    };
}

/*
 ********************************************************************************
 **************************** Implementation Details ****************************
 ********************************************************************************
 */
#include "MultiSet_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_Factory_h_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>

/**
 *  \file
 *
 **  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 ***VERY ROUGH UNUSABLE DRAFT*
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkSortedAssociation_Fastest,
 *              mkSortedAssociation_Smallest, mkSortedAssociationWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 */

namespace Stroika::Foundation::Containers {

    template <typename KEY_TYPE, typename VALUE_TYPE>
    class SortedAssociation;
}
namespace Stroika::Foundation::Containers::Factory {

    /**
     *  \brief   Singleton factory object - Used to create the default backend implementation of a SortedAssociation<> container
     *
     *  Note - you can override the underlying factory dynamically by calling SortedAssociation_Factory<T,TRAITS>::Register (), or
     *  replace it statically by template-specializing SortedAssociation_Factory<T,TRAITS>::New () - though the later is trickier.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = false_type>
    class SortedAssociation_Factory {
    private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
        static atomic<SortedAssociation<KEY_TYPE, VALUE_TYPE> (*) ()> sFactory_;
#else
        static inline atomic<SortedAssociation<KEY_TYPE, VALUE_TYPE> (*) ()> sFactory_{nullptr};
#endif

    public:
        /**
         *  You can call this directly, but there is no need, as the SortedAssociation<T,TRAITS> CTOR does so automatically.
         */
        nonvirtual SortedAssociation<KEY_TYPE, VALUE_TYPE> operator() () const;

    public:
        /**
         *  Register a replacement creator/factory for the given SortedAssociation<T,TRAITS>. Note this is a global change.
         */
        static void Register (SortedAssociation<KEY_TYPE, VALUE_TYPE> (*factory) () = nullptr);

    private:
        static SortedAssociation<KEY_TYPE, VALUE_TYPE> Default_ ();
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedAssociation_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedAssociation_Factory_h_ */

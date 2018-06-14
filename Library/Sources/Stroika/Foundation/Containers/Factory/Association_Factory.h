/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
#ifndef _Stroika_Foundation_Containers_Concrete_Association_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Association_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <type_traits>

/**
 *  \file
 *
 **  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 ***VERY ROUGH UNUSABLE DRAFT*
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkAssociation_Fastest,
 *              mkAssociation_Smallest, mkAssociationWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Rethink use of TRAITS as argument to factory. Probably can only do the SFINAE stuff
 *              when TRAITS EQUALS the default.
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename KEY_TYPE, typename VALUE_TYPE>
            class Association;

            namespace Factory {

                /**
                *  \brief   Singleton factory object - Used to create the default backend implementation of a Association<> container
                *
                *  Note - you can override the underlying factory dynamically by calling Association_Factory<T,TRAITS>::Register (), or
                *  replace it statically by template-specailizing Association_Factory<T,TRAITS>::New () - though the later is trickier.
                *
                *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                */
                template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER>
                class Association_Factory {
                private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
                    static atomic<Association<KEY_TYPE, VALUE_TYPE> (*) ()> sFactory_;
#else
                    static inline atomic<Association<KEY_TYPE, VALUE_TYPE> (*) ()> sFactory_{nullptr};
#endif

                public:
                    /**
                    *  You can call this directly, but there is no need, as the Association<T,TRAITS> CTOR does so automatically.
                    */
                    nonvirtual Association<KEY_TYPE, VALUE_TYPE> operator() () const;

                public:
                    /**
                    *  Register a replacement creator/factory for the given Association<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
                    */
                    static void Register (Association<KEY_TYPE, VALUE_TYPE> (*factory) () = nullptr);

                private:
                    static Association<KEY_TYPE, VALUE_TYPE> Default_ ();
                };

                template <typename KEY_TYPE, typename VALUE_TYPE>
                class Association_Factory<KEY_TYPE, VALUE_TYPE, false_type> {
                private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
                    static atomic<Association<KEY_TYPE, VALUE_TYPE> (*) ()> sFactory_;
#else
                    static inline atomic<Association<KEY_TYPE, VALUE_TYPE> (*) ()> sFactory_{nullptr};
#endif

                public:
                    /**
                    *  You can call this directly, but there is no need, as the Association<T,TRAITS> CTOR does so automatically.
                    */
                    nonvirtual Association<KEY_TYPE, VALUE_TYPE> operator() () const;

                public:
                    /**
                    *  Register a replacement creator/factory for the given Association<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
                    */
                    static void Register (Association<KEY_TYPE, VALUE_TYPE> (*factory) () = nullptr);

                private:
                    static Association<KEY_TYPE, VALUE_TYPE> Default_ ();

                private:
                    template <typename CHECK_KEY>
                    static Association<KEY_TYPE, VALUE_TYPE> Default_SFINAE_ (CHECK_KEY*, typename enable_if<Configuration::has_lt<CHECK_KEY>::value>::type* = 0);
                    static Association<KEY_TYPE, VALUE_TYPE> Default_SFINAE_ (...);
                };
            }
        }
    }
}

/*
********************************************************************************
******************************* Implementation Details *************************
********************************************************************************
*/
#include "Association_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_Factory_h_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <type_traits>

/**
 *  \file
 *
 *  TODO:
 *      @todo   Extend this metaphor to have different kinds of factories, like mkMapping_Fastest,
 *              mkMapping_Smallest, mkMappingWithHash_Fastest etc...
 *              Possibly extend to policy objects, and have properties for this stuff?
 *
 *      @todo   Rethink use of TRAITS as argument to factory. Probably can only do the SFINAE stuff
 *              when TRAITS EQUALS the default.
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            template <typename KEY_TYPE, typename VALUE_TYPE>
            class Mapping;

            namespace Factory {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Mapping<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Mapping_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing Mapping_Factory<T,TRAITS>::New () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 *  \note   Mapping_Factory<K,P> makes up its own default comparer, and so can use order mappings, like Mapping_stdmap, whereas
                 *          Mapping_Factory<K,P,E> - since it takes an equals comparer - is restricted to backends that work with an equals comparere.
                 */
                template <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = void>
                class Mapping_Factory {
                private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
                    static atomic<Mapping<KEY_TYPE, VALUE_TYPE> (*) (const KEY_EQUALS_COMPARER&)> sFactory_;
#else
                    static inline atomic<Mapping<KEY_TYPE, VALUE_TYPE> (*) (const KEY_EQUALS_COMPARER&)> sFactory_{nullptr};
#endif

                public:
                    static_assert (Common::IsEqualsComparer<KEY_EQUALS_COMPARER> (), "Equals comparer required with Mapping_Factory");

                public:
                    Mapping_Factory (const KEY_EQUALS_COMPARER& equalsComparer);

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Mapping<T,TRAITS> CTOR does so automatically.
                     */
                    nonvirtual Mapping<KEY_TYPE, VALUE_TYPE> operator() () const;

                public:
                    /**
                     *  Register a replacement creator/factory for the given Mapping<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
                     */
                    static void Register (Mapping<KEY_TYPE, VALUE_TYPE> (*factory) (const KEY_EQUALS_COMPARER&) = nullptr);

                private:
                    KEY_EQUALS_COMPARER fKeyEqualsComparer_;

                private:
                    static Mapping<KEY_TYPE, VALUE_TYPE> Default_ (const KEY_EQUALS_COMPARER&);
                };
                template <typename KEY_TYPE, typename VALUE_TYPE>
                class Mapping_Factory<KEY_TYPE, VALUE_TYPE, void> {
                private:
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
                    static atomic<Mapping<KEY_TYPE, VALUE_TYPE> (*) ()> sFactory_;
#else
                    static inline atomic<Mapping<KEY_TYPE, VALUE_TYPE> (*) ()>                    sFactory_{nullptr};
#endif

                public:
                    /**
                    *  You can call this directly, but there is no need, as the Mapping<T,TRAITS> CTOR does so automatically.
                    */
                    nonvirtual Mapping<KEY_TYPE, VALUE_TYPE> operator() () const;

                public:
                    /**
                    *  Register a replacement creator/factory for the given Mapping<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
                    */
                    static void Register (Mapping<KEY_TYPE, VALUE_TYPE> (*factory) () = nullptr);

                private:
                    static Mapping<KEY_TYPE, VALUE_TYPE> Default_ ();

                private:
                    template <typename CHECK_KEY>
                    static Mapping<KEY_TYPE, VALUE_TYPE> Default_SFINAE_ (CHECK_KEY*, typename enable_if<Configuration::has_lt<CHECK_KEY>::value>::type* = 0);
                    static Mapping<KEY_TYPE, VALUE_TYPE> Default_SFINAE_ (...);
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
#include "Mapping_Factory.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_ */

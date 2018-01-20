/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_
#define _Stroika_Foundation_Containers_Concrete_Mapping_Factory_h_

#include "../../StroikaPreComp.h"

#include <atomic>
#include <type_traits>

#include "../DefaultTraits/Mapping.h"

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

            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class Mapping;

            namespace Factory {

                /**
                 *  \brief   Singleton factory object - Used to create the default backend implementation of a Mapping<> container
                 *
                 *  Note - you can override the underlying factory dynamically by calling Mapping_Factory<T,TRAITS>::Register (), or
                 *  replace it statically by template-specailizing Mapping_Factory<T,TRAITS>::New () - though the later is trickier.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class Mapping_Factory {
                private:
                    static atomic<Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> (*) ()> sFactory_;

                public:
                    /**
                     *  You can call this directly, but there is no need, as the Mapping<T,TRAITS> CTOR does so automatically.
                     */
                    static Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> New ();

                public:
                    /**
                     *  Register a replacement creator/factory for the given Mapping<KEY_TYPE, VALUE_TYPE,TRAITS>. Note this is a global change.
                     */
                    static void Register (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> (*factory) () = nullptr);

                private:
                    static Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> Default_ ();

                private:
                    template <typename CHECK_KEY>
                    static Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> Default_SFINAE_ (CHECK_KEY*, typename enable_if<Configuration::has_lt<CHECK_KEY>::value and is_same<TRAITS, DefaultTraits::Mapping<CHECK_KEY, VALUE_TYPE>>::value>::type* = 0);
                    static Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> Default_SFINAE_ (...);
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

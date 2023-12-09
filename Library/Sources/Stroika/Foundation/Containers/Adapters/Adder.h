/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Adapters_Adder_h_
#define _Stroika_Foundation_Containers_Adapters_Adder_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::Adapters {

    using Configuration::ArgByValueType;

    /**
     *  Concept to say if Adder is supported.
     * 
     *  Really just any container that supports (with a single argument of the value_type):
     *      o   push_back
     *      o   push_front
     *      o   Add ()
     *      o   insert
     * 
     *  \note Compatable with (not exhaustive list)
     *      o   std::set
     *      o   std::vector
     *      o   std::map
     *      o   Containers::Sequence
     *      o   Containers::Collection
     *      o   Containers::KeyedCollection
     *      o   Containers::Mapping
     *      o   Containers::Multiset
     */
    template <typename CONTAINER>
    concept IAddableTo = requires (CONTAINER p) { p.push_back (declval<typename CONTAINER::value_type> ()); } or
                         requires (CONTAINER p) { p.push_front (declval<typename CONTAINER::value_type> ()); } or
                         requires (CONTAINER p) { p.Add (declval<typename CONTAINER::value_type> ()); } or
                         requires (CONTAINER p) { p.insert (declval<typename CONTAINER::value_type> ()); };

    /**
     *  Concept that returns true if the given type of element ELT2ADD can be added to the argument container.
     * 
     *  This corresponds to if you can construct an Adder adapter on the template, and invoke .Add on it with the argument type.
     */
    template <typename ELT2ADD, typename CONTAINER>
    concept IAddableCompatible = IAddableTo<CONTAINER> and convertible_to<ELT2ADD, typename CONTAINER::value_type>;

    /**
     *  \brief utility for generic code that wishes to add something to a somewhat arbitrary container, where the ordering
     *         of the addition is undefined/doesn't matter (whatever makes sense by default for that container).
     */
    template <IAddableTo CONTAINER_TYPE>
    struct Adder {
    public:
        /**
         */
        using value_type = typename CONTAINER_TYPE::value_type;

    public:
        /**
         */
        static void Add (CONTAINER_TYPE* container, Configuration::ArgByValueType<value_type> value);
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Adder.inl"

#endif /*_Stroika_Foundation_Containers_Adapters_Adder_h_ */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Adapters_Adder_h_
#define _Stroika_Foundation_Containers_Adapters_Adder_h_ 1

#include "../../StroikaPreComp.h"

#include <map>
#include <set>
#include <vector>

#include "../Collection.h"
#include "../KeyedCollection.h"
#include "../Mapping.h"
#include "../MultiSet.h"
#include "../Sequence.h"
#include "../Set.h"
/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::Adapters {

    using Configuration::ArgByValueType;

    namespace Private_ {
        struct IAddableTo_ {
            template <typename T>
            static constexpr bool IsSupported (...)
            {
                return false;
            }
            template <typename T>
            static constexpr bool IsSupported (const Collection<T>*)
            {
                return true;
            }
            template <typename T, typename KEY_TYPE, typename TRAITS>
            static constexpr bool IsSupported (const KeyedCollection<T, KEY_TYPE, TRAITS>*)
            {
                return true;
            };
            template <typename KEY_TYPE, typename VALUE_TYPE>
            static constexpr bool IsSupported (const Mapping<KEY_TYPE, VALUE_TYPE>*)
            {
                return true;
            };
            template <typename T>
            static constexpr bool IsSupported (const set<T>*)
            {
                return true;
            };
            template <typename T>
            static constexpr bool IsSupported (const Sequence<T>*)
            {
                return true;
            };
            template <typename T>
            static constexpr bool IsSupported (const Set<T>*)
            {
                return true;
            };
            template <typename T>
            static constexpr bool IsSupported (const MultiSet<T>*)
            {
                return true;
            };
            template <typename T>
            static constexpr bool IsSupported (const vector<T>*)
            {
                return true;
            };
        };

    }

    /**
     *  Concept to say if Adder is supported.
     */
    template <typename CONTAINER>
    concept IAddableTo = Private_::IAddableTo_::IsSupported (static_cast<const CONTAINER*> (nullptr));

    /**
     *  Concept that returns true if the given type of element ELT2ADD can be added to the argument container.
     * 
     *  This corresponds to if you can construct an Adder adapter on the template, and invoke .Add on it with the argument type.
     */
    template <typename ELT2ADD, typename CONTAINER>
    concept IAddableCompatible = IAddableTo<CONTAINER> and convertible_to<ELT2ADD, typename CONTAINER::value_type>;

    /**
     */
    template <typename CONTAINER_TYPE>
    struct Adder {
    public:
        /**
         */
        using value_type = typename CONTAINER_TYPE::value_type;

    public:
        /**
         */
        static void Add (CONTAINER_TYPE* container, Configuration::ArgByValueType<value_type> value);

    private:
        // Cannot do this with if constexpr locally inside Add, cuz hard todo the template matching/overloading
        // MAYBE possible, but not easy/obvious like this
        static void Add_ (Collection<value_type>* container, Configuration::ArgByValueType<value_type> value);
        template <typename T, typename KEY_TYPE, typename TRAITS>
        static void Add_ (KeyedCollection<T, KEY_TYPE, TRAITS>* container, Configuration::ArgByValueType<T> value);
        template <typename KEY_TYPE, typename VALUE_TYPE>
        static void Add_ (Mapping<KEY_TYPE, VALUE_TYPE>* container, Configuration::ArgByValueType<value_type> value);
        template <typename KEY_TYPE, typename VALUE_TYPE>
        static void Add_ (Mapping<KEY_TYPE, VALUE_TYPE>* container, Configuration::ArgByValueType<pair<KEY_TYPE, VALUE_TYPE>> value);
        static void Add_ (set<value_type>* container, Configuration::ArgByValueType<value_type> value);
        static void Add_ (Sequence<value_type>* container, Configuration::ArgByValueType<value_type> value);
        static void Add_ (Set<value_type>* container, Configuration::ArgByValueType<value_type> value);
        template <typename T>
        static void Add_ (MultiSet<T>* container, Configuration::ArgByValueType<value_type> value);
        static void Add_ (vector<value_type>* container, Configuration::ArgByValueType<value_type> value);
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Adder.inl"

#endif /*_Stroika_Foundation_Containers_Adapters_Adder_h_ */

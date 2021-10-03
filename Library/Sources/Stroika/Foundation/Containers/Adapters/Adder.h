/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
#include "../Sequence.h"
#include "../Set.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Adapters {

    using Configuration::ArgByValueType;

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

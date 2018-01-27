/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Adapters_Adder_h_
#define _Stroika_Foundation_Containers_Adapters_Adder_h_ 1

#include "../../StroikaPreComp.h"

#include <map>
#include <set>
#include <vector>

#include "../Collection.h"
#include "../Mapping.h"
#include "../Sequence.h"
#include "../Set.h"

/*
 *  \file
 *              ****VERY ROUGH DRAFT
 *
 *  DONT like how its done - with dependencies - but very helpful.
 *  Dont want to add to Iterable (though that would make some sense) - cuz:
 *       *STL stuff doesn't work with that
 *       *Up til now - Iterable has been all about readonly operations. Could have UpdatableIterable intermediate class...
 *
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Adapters {

                using Configuration::ArgByValueType;

                /*
                 *  utility we might want to move someplace else
                 *** EXPERIMENTAL - LGP 2016-07-22
                 */
                template <typename CONTAINER_TYPE>
                struct Adder {
                public:
                    /**
                     */
                    using value_type = typename CONTAINER_TYPE::value_type;

                public:
                    static void Add (CONTAINER_TYPE* container, Configuration::ArgByValueType<value_type> value);

                private:
                    template <typename TRAITS>
                    static void Add_ (Set<value_type, TRAITS>* container, Configuration::ArgByValueType<value_type> value);
                    template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                    static void Add_ (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>* container, Configuration::ArgByValueType<value_type> value);
                    template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                    static void Add_ (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>* container, Configuration::ArgByValueType<std::pair<KEY_TYPE, VALUE_TYPE>> value);
                    static void Add_ (set<value_type>* container, Configuration::ArgByValueType<value_type> value);
                    static void Add_ (vector<value_type>* container, Configuration::ArgByValueType<value_type> value);
                    static void Add_ (Sequence<value_type>* container, Configuration::ArgByValueType<value_type> value);
                    static void Add_ (Collection<value_type>* container, Configuration::ArgByValueType<value_type> value);
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
#include "Adder.inl"

#endif /*_Stroika_Foundation_Containers_Adapters_Adder_h_ */

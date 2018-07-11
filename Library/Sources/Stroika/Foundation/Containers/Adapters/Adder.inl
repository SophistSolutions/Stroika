/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Adapters_Adder_inl_
#define _Stroika_Foundation_Containers_Adapters_Adder_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Adapters {

                /*
                 ********************************************************************************
                 ******************************** Adapters::Adder *******************************
                 ********************************************************************************
                 */
                template <typename CONTAINER_TYPE>
                inline void Adder<CONTAINER_TYPE>::Add (CONTAINER_TYPE* container, Configuration::ArgByValueType<value_type> value)
                {
                    RequireNotNull (container);
                    Add_ (container, value);
                }
                template <typename CONTAINER_TYPE>
                inline void Adder<CONTAINER_TYPE>::Add_ (Set<value_type>* container, Configuration::ArgByValueType<value_type> value)
                {
                    RequireNotNull (container);
                    container->Add (value);
                }
                template <typename CONTAINER_TYPE>
                template <typename KEY_TYPE, typename VALUE_TYPE>
                inline void Adder<CONTAINER_TYPE>::Add_ (Mapping<KEY_TYPE, VALUE_TYPE>* container, Configuration::ArgByValueType<value_type> value)
                {
                    RequireNotNull (container);
                    container->Add (value);
                }
                template <typename CONTAINER_TYPE>
                template <typename KEY_TYPE, typename VALUE_TYPE>
                inline void Adder<CONTAINER_TYPE>::Add_ (Mapping<KEY_TYPE, VALUE_TYPE>* container, Configuration::ArgByValueType<pair<KEY_TYPE, VALUE_TYPE>> value)
                {
                    RequireNotNull (container);
                    container->Add (value); // SB OK because Common::KeyValuePair has non-explicit CTOR taking pair<>
                }
                template <typename CONTAINER_TYPE>
                inline void Adder<CONTAINER_TYPE>::Add_ (set<value_type>* container, Configuration::ArgByValueType<value_type> value)
                {
                    RequireNotNull (container);
                    container->insert (value);
                }
                template <typename CONTAINER_TYPE>
                inline void Adder<CONTAINER_TYPE>::Add_ (vector<value_type>* container, Configuration::ArgByValueType<value_type> value)
                {
                    RequireNotNull (container);
                    container->push_back (value);
                }
                template <typename CONTAINER_TYPE>
                inline void Adder<CONTAINER_TYPE>::Add_ (Sequence<value_type>* container, Configuration::ArgByValueType<value_type> value)
                {
                    RequireNotNull (container);
                    container->push_back (value);
                }
                template <typename CONTAINER_TYPE>
                inline void Adder<CONTAINER_TYPE>::Add_ (Collection<value_type>* container, Configuration::ArgByValueType<value_type> value)
                {
                    RequireNotNull (container);
                    container->Add (value);
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Adapters_Adder_inl_ */

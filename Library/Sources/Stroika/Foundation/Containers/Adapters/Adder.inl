/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Adapters_Adder_inl_
#define _Stroika_Foundation_Containers_Adapters_Adder_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers::Adapters {

    /*
     ********************************************************************************
     ******************************** Adapters::Adder *******************************
     ********************************************************************************
     */
    template <IAddableTo CONTAINER_TYPE>
    inline void Adder<CONTAINER_TYPE>::Add (CONTAINER_TYPE* container, Configuration::ArgByValueType<value_type> value)
    {
        RequireNotNull (container);
        using CONTAINER_ELT_TYPE = typename CONTAINER_TYPE::value_type;
        if constexpr (requires (CONTAINER_TYPE p) { p.push_back (declval<CONTAINER_ELT_TYPE> ()); }) {
            container->push_back (value);
        }
        else if constexpr (requires (CONTAINER_TYPE p) { p.push_front (declval<CONTAINER_ELT_TYPE> ()); }) {
            container->push_front (value);
        }
        else if constexpr (requires (CONTAINER_TYPE p) { p.Add (declval<CONTAINER_ELT_TYPE> ()); }) {
            container->Add (value);
        }
        else if constexpr (requires (CONTAINER_TYPE p) { p.insert (declval<CONTAINER_ELT_TYPE> ()); }) {
            container->insert (value);
        }
        else {
            AssertNotImplemented ();
        }
    }

}
#endif /* _Stroika_Foundation_Containers_Adapters_Adder_inl_ */

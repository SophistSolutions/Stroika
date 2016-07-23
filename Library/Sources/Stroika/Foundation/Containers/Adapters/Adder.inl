/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Adapters_Adder_inl_
#define _Stroika_Foundation_Containers_Adapters_Adder_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Adapters {




                /*
                 ********************************************************************************
                 **************************** Association operators *****************************
                 ********************************************************************************
                 */
                template        <typename CONTAINER_TYPE>
                template    <typename TRAITS>
                static  void    Adder < CCONTAINER_TYPE::Add_ (Set<ElementType, TRAITS>* container, Configuration::ArgByValueType<ElementType> value)
                {
                    RequireNotNull (container);
                    container->Add (value);
                }
                template        <typename CONTAINER_TYPE>
                template    <typename VALUE_TYPE, typename TRAITS>
                static  void    Adder < CCONTAINER_TYPE::Add_ (Mapping<ElementType, VALUE_TYPE, TRAITS>* container, Configuration::ArgByValueType<Common::KeyValuePair<ElementType, VALUE_TYPE>> value)
                {
                    RequireNotNull (container);
                    container->Add (value);
                }
                template        <typename CONTAINER_TYPE>
                static  void    Adder < CCONTAINER_TYPE::Add_ (vector<ElementType>* container, Configuration::ArgByValueType<ElementType> value)
                {
                    RequireNotNull (container);
                    container->push_back (value);
                }
                template        <typename CONTAINER_TYPE>
                static  void    Adder < CCONTAINER_TYPE::Add_ (Sequence<ElementType>* container, Configuration::ArgByValueType<ElementType> value)
                {
                    RequireNotNull (container);
                    container->push_back (value);
                }
                template        <typename CONTAINER_TYPE>
                static  void    Adder < CCONTAINER_TYPE::Add (CONTAINER_TYPE* container, Configuration::ArgByValueType<ElementType> value)
                {
                    RequireNotNull (container);
                    Add_ (container, value);
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Adapters_Adder_inl_ */

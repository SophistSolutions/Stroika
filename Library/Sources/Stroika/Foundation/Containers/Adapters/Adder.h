/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Adapters_Adder_h_
#define _Stroika_Foundation_Containers_Adapters_Adder_h_  1

#include    "../../StroikaPreComp.h"

#include    <map>
#include    <set>
#include    <vector>

#include    "../Mapping.h"
#include    "../Set.h"
#include    "../Sequence.h"



/*
 *  \file
 *              ****VERY ROUGH DRAFT

 *  DONT like how its done - with dependencies - but very helpful.
 *  Dont want to add to Iterable (though that would make some sense) - cuz:
        *STL stuff doesnt work with that
        *Up til now - Iterable has been all about readonly operations. Could have UpdatableIterable intermediate class...


 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Adapters {


                using   Configuration::ArgByValueType;


                /*
                 *  utility we might want to move someplace else
                 *** EXPERIMENTAL - LGP 2016-07-22
                 */
                template        <typename CONTAINER_TYPE>
                struct  Adder {
                    using   ElementType     = typename CONTAINER_TYPE::value_type;

                public:
                    static  void    Add (CONTAINER_TYPE* container, Configuration::ArgByValueType<ElementType> value);

                private:
                    template    <typename TRAITS>
                    static  void    Add_ (Set<ElementType, TRAITS>* container, Configuration::ArgByValueType<ElementType> value);
                    template    <typename VALUE_TYPE, typename TRAITS>
                    static  void    Add_ (Mapping<ElementType, VALUE_TYPE, TRAITS>* container, Configuration::ArgByValueType<Common::KeyValuePair<ElementType, VALUE_TYPE>> value);
                    static  void    Add_ (vector<ElementType>* container, Configuration::ArgByValueType<ElementType> value);
                    static  void    Add_ (Sequence<ElementType>* container, Configuration::ArgByValueType<ElementType> value);
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
#include    "Adder.inl"

#endif  /*_Stroika_Foundation_Containers_Adapters_Adder_h_ */

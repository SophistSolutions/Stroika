/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_MapReduce_inl_
#define _Stroika_Foundation_Traversal_MapReduce_inl_

#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             ********************************************************************************
             ******************************* Traversal::Map *********************************
             ********************************************************************************
             */
            template    <typename CONTAINER_OF_T>
            CONTAINER_OF_T  Map (const CONTAINER_OF_T& containerOfT, std::function<typename CONTAINER_OF_T::ElementType (typename CONTAINER_OF_T::ElementType)>& do2Each)
            {
                CONTAINER_OF_T  result;
                for (typename CONTAINER_OF_T::ElementType i : containerOfT) {
                    // unsure if we pdate in place, or create a new container? May need traits param to define how todo this!
                    result.Append (do2Each (i));
                }
                return result;
            }


            /*
             ********************************************************************************
             **************************** Traversal::Reduce *********************************
             ********************************************************************************
             */
            template    <typename CONTAINER_OF_T>
            typename CONTAINER_OF_T::ElementType    Reduce (const CONTAINER_OF_T& containerOfT, std::function<typename CONTAINER_OF_T::ElementType (typename CONTAINER_OF_T::ElementType memo, typename CONTAINER_OF_T::ElementType i)>& do2Each, typename CONTAINER_OF_T::ElementType memo)
            {
                typename CONTAINER_OF_T::ElementType    result  =   memo;
                for (typename CONTAINER_OF_T::ElementType i : containerOfT) {
                    result = do2Each (result, i);
                }
                return result;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_MapReduce_inl_ */

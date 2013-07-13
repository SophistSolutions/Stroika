/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_MapReduce_h_
#define _Stroika_Foundation_Traversal_MapReduce_h_  1

#include    "../StroikaPreComp.h"

#include    <limits>

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  NOTES:
 *
 *  TODO:
 *      @todo   Placeholder - so I know its work todo... but not really even started
 *
 *      @todo   Consider renaming this module to
 *                  >   FunctionalUtilties
 *                  >   FunctionalSupport
 *                  >   FunctionalAlgorithms
 *                  >   Or - Migrate this code directly into Iterable<T>!!!
 *
 *      @todo   See http://underscorejs.org/#filter - and add filter, and probably others - but not sure what
 *              to call these (functional helpers?)
 *              Same for http://underscorejs.org/#pluck
 *
 *      @todo   Consider replacing ElementType with value_type - after I verify taht alwways works in STL - and then
 *              this code will work with Stroika containers and STL containers
 *
 *      @todo   MAYBE use
 *              Default_MapReduce_Traits<CONTAINER_OF_T> {
 *                  typedef typename CONTAINER_OF_T::ElementType    ElementType;
 *
 *                  UPDATE_TARGET_CONTAINER (CONTAINER_OF_T* result, ElementType newElt);// UNCLEAR IF THIS APPENDS, or takes iteraotr???
 *              ...
 *              }
 *
 *      @todo   Consider having Map/Reduce (ETC) methods take Iterable<T> as the parameter, and then a templated
 *              param for resulting container (and method to add to it - maybe stdfunction?) - Maybe something like
 *              an STL output-iterator (but safe - using stroika containers)?
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  @see http://en.wikipedia.org/wiki/MapReduce
             *  @see http://www.ruby-doc.org/core-2.0/Array.html#method-i-map
             */
            template    <typename CONTAINER_OF_T>
            CONTAINER_OF_T  Map (const CONTAINER_OF_T& containerOfT, const std::function<typename CONTAINER_OF_T::ElementType (typename CONTAINER_OF_T::ElementType)>& do2Each);

            /**
             *  @see http://en.wikipedia.org/wiki/MapReduce
             *  @see http://underscorejs.org/#reduce
             */
            template    <typename CONTAINER_OF_T>
            typename CONTAINER_OF_T::ElementType    Reduce (const CONTAINER_OF_T& containerOfT, const std::function<typename CONTAINER_OF_T::ElementType (typename CONTAINER_OF_T::ElementType memo, typename CONTAINER_OF_T::ElementType i)>& do2Each, typename CONTAINER_OF_T::ElementType memo = typename CONTAINER_OF_T::ElementType ());


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "MapReduce.inl"

#endif  /*_Stroika_Foundation_Traversal_MapReduce_h_ */

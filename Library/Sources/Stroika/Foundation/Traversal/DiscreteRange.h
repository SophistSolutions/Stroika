/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_h_
#define _Stroika_Foundation_Traversal_DiscreteRange_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterable.h"
#include    "Range.h"



/**
 *  \file
 *
 *  TODO:
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             */
            template    <typename T>
            struct  DefaultDiscreteRangeTraits : DefaultRangeTraits<T> {
                // needed for iterator - return false if no more
                static bool GetNext (T* n);
            };


            /**
             */
            template    <typename T, typename TRAITS = DefaultDiscreteRangeTraits<T>>
            class  DiscreteRange : public Range<T, TRAITS>, public Iterable<T> {
                // must shadow methods that return Range in base class, and provide CTOR

                //
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DiscreteRange.inl"

#endif  /*_Stroika_Foundation_Traversal_DiscreteRange_h_ */

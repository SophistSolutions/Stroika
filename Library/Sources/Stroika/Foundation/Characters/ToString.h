/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_h_
#define _Stroika_Foundation_Characters_ToString_h_ 1

#include    "../StroikaPreComp.h"


#include    "String.h"



/**
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *  Convert an instance of the given object to a printable string representation. This representation
             *  is not guarnateed, pretty, or parsable. This feature is generally for debugging purposes, but can be used
             *  to render/emit objects in any informal setting where you just need a rough sense of the object (again,
             *  the only case I can think of here would be for debugging).
             *
             *  \note *Implementation Note*
             *      This implementation defaults to calling T().ToString ().
             */
            template    <typename T>
            String  ToString (const T& t);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "ToString.inl"

#endif  /*_Stroika_Foundation_Characters_ToString_h_*/

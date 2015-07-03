/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryTiedStreams_h_
#define _Stroika_Foundation_Streams_BinaryTiedStreams_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"


#include    "BinaryInputOutputStream.h"


#pragma message ("Warning: FILE DEPRECATED")


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


//@todo - REPALCE THIS WITH RELAXED DEFINITION OF BinaryInputOutputStream - not necesarily so closely related

            using BinaryTiedStreams = BinaryInputOutputStream;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_BinaryTiedStreams_h_*/


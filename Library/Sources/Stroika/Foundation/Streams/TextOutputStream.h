/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextOutputStream_h_
#define _Stroika_Foundation_Streams_TextOutputStream_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Characters/String.h"
#include    "../Configuration/Common.h"

#include    "OutputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 *      @todo   Add Close() as with - binaryoutputstream!
 *
 *      @todo   Add Flush() like we have for binaryoutputstream!
 *
 */

#pragma message ("Warning: TextOutputStream.h FILE DEPRECATED - use OutputStream<Character>")


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Characters::Character;
            using   Characters::String;


            using TextOutputStream = OutputStream<Character>;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_TextOutputStream_h_*/

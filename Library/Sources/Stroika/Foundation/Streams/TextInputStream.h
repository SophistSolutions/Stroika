/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStream_h_
#define _Stroika_Foundation_Streams_TextInputStream_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Characters/String.h"
#include    "../Configuration/Common.h"

#include    "InputStream.h"






namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Characters::Character;
            using   Characters::String;

#pragma message ("Warning: TextInputStream.h FILE DEPRECATED - use InputStream<Character>")


            using TextInputStream = InputStream<Character>;



        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "TextInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_TextInputStream_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicTextOutputStream_h_
#define _Stroika_Foundation_Streams_BasicTextOutputStream_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Characters/Character.h"
#include    "../Characters/String.h"
#include    "../Configuration/Common.h"

//#include    "TextOutputStream.h"
#include    "MemoryStream.h"



#pragma message ("Warning: BasicTextOutputStream FILE DEPRECATED - use MemoryStream<Characters::Character>")



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Characters::Character;
            using   Characters::String;

            using BasicTextOutputStream = MemoryStream<Characters::Character>;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "BasicTextOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BasicTextOutputStream_h_*/

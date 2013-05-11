/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Tokenize_h_
#define _Stroika_Foundation_Characters_Tokenize_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "String.h"



/**
 * TODO:
 *
 *      @todo   Consider adding new Split() function. It seems that is quite similar - maybe identical to
 *              the Tokenize() funciton. Consider overloads for String - returing Sequnce<String>
 *              and taking Set<Character>.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             */
            template    <typename STRING>
            vector<STRING> Tokenize (const STRING& str, const STRING& delimiters);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Tokenize.inl"

#endif  /*_Stroika_Foundation_Characters_Tokenize_h_*/

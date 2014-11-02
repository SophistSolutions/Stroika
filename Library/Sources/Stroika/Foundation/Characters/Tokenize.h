/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Tokenize_h_
#define _Stroika_Foundation_Characters_Tokenize_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "String.h"



/**
 * TODO:
 *
 *      @todo   Redo so at least WORKS with Sequence<String> - but maybe even get rid of other versions?
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
            _DeprecatedFunction_  (vector<STRING> Tokenize (const STRING& str, const STRING& delimiters), "Deprecated in version 2.0a51 of Stoika: use String::Tokenize() instead");


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

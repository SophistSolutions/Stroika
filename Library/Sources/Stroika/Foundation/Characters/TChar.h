/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_TChar_h_
#define _Stroika_Foundation_Characters_TChar_h_ 1

#include    "../StroikaPreComp.h"



#define qUSE_BACKWARD_COMPAT_TSTRING_CODE   1



#if     qUSE_BACKWARD_COMPAT_TSTRING_CODE
#include    <cstdlib>

#include    "SDKChar.h"
#include    "SDKString.h"
#endif


/*
 *  MODULE OBSOLETE - TO BE RETIRED SOON
 *      -- LGP 2013-09-28
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {




#if     qUSE_BACKWARD_COMPAT_TSTRING_CODE
            // SOON TO BE OBSOLETE - USE SDKChar
            //  -- LGP 2013-09-28
            typedef Characters::SDKChar TChar;


            // SOON TO BE OBSOLETE - USE SDKChar
            //  -- LGP 2013-09-28
#define TSTR(x)    SDKSTR(x)
#endif


        }
    }
}






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Characters_TChar_h_*/

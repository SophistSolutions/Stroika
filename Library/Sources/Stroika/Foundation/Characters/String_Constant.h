/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Constant_h_
#define _Stroika_Foundation_Characters_String_Constant_h_ 1

#include    "../StroikaPreComp.h"

#include    "Concrete/String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly.h"



/**
 * TODO:
 *
 *      @todo   Consider if there is some way - maybe in String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly - to
 *              assure the type of the string arg is constexpr wchar_t[]
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             *      String_Constant can safely be used to initilaize constant C-strings as Stroika strings,
             * with a minimum of cost.
             *
             *  \em WARNING - BE VERY CAREFUL - this is just an alias for String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly - so be
             *      sure arguments have application lifetime.
             *
             */
            using       String_Constant     =    Concrete::String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Characters_String_Constant_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Valgrind_h_
#define _Stroika_Foundation_Debug_Valgrind_h_  1

#include    "../StroikaPreComp.h"



/**
 *  Note - this is NOT required for use of valgrind memcheck() - at least as of v2.0a145. But it does
 *  help in getting clean results with valgrind tool helgrind.
 */
#ifndef qStroika_FeatureSupported_Valgrind
#define qStroika_FeatureSupported_Valgrind  0
#endif

#if     qStroika_FeatureSupported_Valgrind
#include    "helgrind.h"
#endif


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Early</a>
 *
 *      @todo
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Valgrind.inl"

#endif  /*_Stroika_Foundation_Debug_Valgrind_h_*/

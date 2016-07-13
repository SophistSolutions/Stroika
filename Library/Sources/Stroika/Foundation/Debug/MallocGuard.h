/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_MallocGuard_h_
#define _Stroika_Foundation_Debug_MallocGuard_h_  1

#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <windows.h>
#include    <tchar.h>
#endif
#include    <array>

#include    "../Characters/SDKChar.h"
#include    "../Characters/SDKString.h"
#include    "../Configuration/Common.h"
#include    "CompileTimeFlagChecker.h"
#include    "../Execution/ModuleInit.h"
#include    "../Time/Realtime.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /**
             *  If qStroika_Foundation_Debug_MallogGuard defined to 1,

             TBD

             but basically - replace malloc/free etc


             &&& for now - requires GCC
             &"
             *  \note   On detected errors, this will first call AssertNotReached (), and then std::terminate ();
             *          In Debug or Release versions (if you call Debug::RegisterDefaultFatalErrorHandlers ()) - you will get a stack trace dumped
             *          and typically a core file - when errors are detected.
             */
#if     !defined (qStroika_Foundation_Debug_MallogGuard)
#define qStroika_Foundation_Debug_MallogGuard   0
#endif





        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "MallocGuard.inl"

#endif  /*_Stroika_Foundation_Debug_MallocGuard_h_*/

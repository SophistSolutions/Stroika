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
 *  TODO:
 *      @todo   Support on Windows/MSVC
 *
 *      @todo   Maybe support generically (not sure if/how possible)
 *
 *      @todo   Finish support for stuff like posix_memalign () and other functions that maybe needed
 *
 *      @todo   Consider doing an implementation with the 'wrap' logic'
 *              http://stackoverflow.com/questions/262439/create-a-wrapper-function-for-malloc-and-free-in-c
 *              --wrap=symbol
 *               void *__wrap_malloc (size_t c)
 *              {
 *                  printf ("malloc called with %zu\n", c);
 *                  return __real_malloc (c);
 *              }
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /**
             *  If qStroika_Foundation_Debug_MallocGuard defined to 1, wrap malloc(), free () etc, to do extra checking for corruption, double free
             *  write off the ends, etc.
             *
             *  This works in DEBUG or RELEASE builds.
             *
             *  This - so far - only works for GCC.
             *
             *  This may not work perfectly if you call some wierd malloc variants, or mix __libc_malloc with free (), etc.
             *
             *  \note   You can enable this feature with
             *          ./configure ... --malloc-guard true
             *
             *  \note   On detected errors, this will call std::terminate ();
             *          In Debug or Release versions (if you call Debug::RegisterDefaultFatalErrorHandlers ()) - you will get a stack trace dumped
             *          and typically a core file - when errors are detected. Though maybe not, since that stuff all allocates memory, and clearly thats
             *          not working well when we fail...
             */
#if     !defined (qStroika_Foundation_Debug_MallocGuard)
#define qStroika_Foundation_Debug_MallocGuard   0
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

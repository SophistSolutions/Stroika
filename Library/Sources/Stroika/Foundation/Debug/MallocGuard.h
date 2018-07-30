/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_MallocGuard_h_
#define _Stroika_Foundation_Debug_MallocGuard_h_ 1

#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <windows.h>

#include <tchar.h>
#endif
#include <array>

#include "../Characters/SDKChar.h"
#include "../Characters/SDKString.h"
#include "../Configuration/Common.h"
#include "../Execution/ModuleInit.h"
#include "../Time/Realtime.h"
#include "CompileTimeFlagChecker.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-621 stopped testing regularly in Stroika
 *              2.0a217 (october 2017).
 *
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
 *
 *      @todo   Fix so valgrind/helgrind friendly. Not super important, since if those were working we wouldn't need
 *              this (its mostly for situations - such as the one I face at block - where valgrind is coming up lemons.
 */

namespace Stroika::Foundation::Debug {

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
     *  \note   If you can use ./configure --sanitize=address, that probably works better. Or - possibly - using valgrind.
     *
     *  \note   On detected errors, this will call std::terminate ();
     *          In Debug or Release versions (if you call Debug::RegisterDefaultFatalErrorHandlers ()) - you will get a stack trace dumped
     *          and typically a core file - when errors are detected. Though maybe not, since that stuff all allocates memory, and clearly thats
     *          not working well when we fail...
     */
#if !defined(qStroika_Foundation_Debug_MallocGuard)
#define qStroika_Foundation_Debug_MallocGuard 0
#endif

    /**
     *      qStroika_Foundation_Debug_MallocGuard_GuardSize can be 0, or any integer number greater;
     *
     *  \req qStroika_Foundation_Debug_MallocGuard
     */
#if !defined(qStroika_Foundation_Debug_MallocGuard_GuardSize)
#define qStroika_Foundation_Debug_MallocGuard_GuardSize 16
#endif

}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "MallocGuard.inl"

#endif /*_Stroika_Foundation_Debug_MallocGuard_h_*/

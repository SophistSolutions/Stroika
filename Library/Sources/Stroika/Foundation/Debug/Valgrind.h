/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Valgrind_h_
#define _Stroika_Foundation_Debug_Valgrind_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  \brief  Support for running under valgrind memcheck - the only valgrind tool Stroika supports (see Documentation/Debugging.md)
 *
 *  Memcheck needs nothing from here to work. This provides IsRunningUnderValgrind (), and - when configured with
 *  --valgrind enable, which defines qStroika_FeatureSupported_Valgrind - memcheck's client requests (<valgrind/memcheck.h>).
 *  It includes nothing else, so it is cheap to include anywhere.
 *
 *  @see https://github.com/svn2github/valgrind
 *  @see https://sourceforge.net/u/lluct/me722-cm/ci/master/tree/external/valgrind/main/memcheck/memcheck.h
 *  @see https://lists.sourceforge.net/lists/listinfo/valgrind-developers
 *
 *  TODO:
 *      @todo
 *
 */

#ifndef qStroika_FeatureSupported_Valgrind
#define qStroika_FeatureSupported_Valgrind 0
#endif

#if qStroika_FeatureSupported_Valgrind
#include <valgrind/memcheck.h>
#endif

/**
 *  Use IsRunningUnderValgrind () to test if we are running under valgrind. This often
 *  uses MUCH more memory and runs much slower (details depend on if running under memcheck or which we blurr,
 *  so this isn't perfect).
 *
 *  \note This can be called whether or not qStroika_FeatureSupported_Valgrind is defined, but it always returns false if qStroika_FeatureSupported_Valgrind is not defined.
 *
 */
bool IsRunningUnderValgrind ();

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Valgrind.inl"

#endif /*_Stroika_Foundation_Debug_Valgrind_h_*/

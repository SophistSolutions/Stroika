/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Valgrind_h_
#define _Stroika_Foundation_Debug_Valgrind_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  \brief  Include this file VERY EARLY ON - before including stuff like <cstdio> -
 *          to allow use of Valgrind (some features)
 *
 *  Note - this is NOT required for use of valgrind memcheck() - at least as of v2.0a145.
 *
 *  This module is VERY careful not to include unneded files - and includes just the minimal required to make
 *  the valgrind family of tools work IFF you preconfigure (./configure) qStroika_FeatureSupported_Valgrind to
 *  be a predefined C++ macro define.
 *
 *  @see http://valgrind.org/docs/manual/drd-manual.html
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
#include "valgrind/memcheck.h"
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

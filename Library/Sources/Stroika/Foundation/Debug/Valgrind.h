/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
 *  Note - this is NOT required for use of valgrind memcheck() - at least as of v2.0a145. But it does
 *  help in getting clean results with valgrind tool helgrind.
 *
 *  This module is VERY careful not to include unneded files - and includes just the minimal required to make
 *  the valgrind family of tools work IFF you preconfigure (./configure) qStroika_FeatureSupported_Valgrind to
 *  be a predefined C++ macro define.
 *
 *  @see http://valgrind.org/docs/manual/drd-manual.html
 *  @see https://github.com/svn2github/valgrind
 *  @see https://sourceforge.net/u/lluct/me722-cm/ci/master/tree/external/valgrind/main/helgrind/helgrind.h
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
#include "valgrind/helgrind.h"
#include "valgrind/memcheck.h"
#endif

#if qStroika_FeatureSupported_Valgrind
/*
 *  See https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug.html
 *
 *  undef because some systems pre-include and define this (by default empty), and we want to force to map to 
 *  helgrind etc versions - as in above docs -- LGP 2018-10-24
 */
#undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE
#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(A) ANNOTATE_HAPPENS_BEFORE (A)
#undef _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER
#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(A) ANNOTATE_HAPPENS_AFTER (A)
#endif

/**
 *  Use IsRunningUnderValgrind () to test if we are running under valgrind. This often
 *  uses MUCH more memory and runs much slower (details depend on if running under memcheck or helgrind, which we blurr,
 *  so this isn't perfect).
 *
 *  \note This can be called whether or not qStroika_FeatureSupported_Valgrind is defined, but it always returns false if qStroika_FeatureSupported_Valgrind is not defined.
 *
 */
bool IsRunningUnderValgrind ();

/**
 *  Use Macro Stroika_Foundation_Debug_ValgrindDisableHelgrind () on variables Helgrind should
 *  ignore.
 *
 *  There are many classes of 'bugs' with helgrind that we can most easily avoid by just ignoring those variables.
 *      >   std::atomic
 *      >   magic statics
 *
 *
 */
#if qStroika_FeatureSupported_Valgrind
#define Stroika_Foundation_Debug_ValgrindDisableHelgrind(X) \
    VALGRIND_HG_DISABLE_CHECKING (&(X), sizeof (X))
#else
#define Stroika_Foundation_Debug_ValgrindDisableHelgrind(X)
#endif

/**
 *  See https://bugs.kde.org/show_bug.cgi?id=379630
 *  for info on            
 *       VALGRIND_HG_MUTEX_INIT_POST (&fLock_, true);
 *
 *  It appears valgrind doesn't notice when a lock goes out of scope (destroyed). Failing with the wrappers for gnu/stl for valgrind.
 *  Still broken as of Ubuntu 1804
 */

/**
 *  Use Macro Stroika_Foundation_Debug_ValgrindDisableHelgrind_START/Stroika_Foundation_Debug_ValgrindDisableHelgrind_END
 *  to ignore a particular variable in a range of code.
 *
 *  EXAMPLE:
 *      int var = 1;
 *      .... thread 1 writes;
 *      .... thread 2 writes; // all with locks
 *      wait til thread 1/2 done;
 *      Stroika_Foundation_Debug_ValgrindDisableHelgrind_START(var);
 *      read var;   // helgrind doesn't know the thread completion is essentially a barrier
 *      Stroika_Foundation_Debug_ValgrindDisableHelgrind_END(var);
 */
#define Stroika_Foundation_Debug_ValgrindDisableHelgrind_START(X) \
    Stroika_Foundation_Debug_ValgrindDisableHelgrind (X)

#if qStroika_FeatureSupported_Valgrind
#define Stroika_Foundation_Debug_ValgrindDisableHelgrind_END(X) \
    VALGRIND_HG_ENABLE_CHECKING (&(X), sizeof (X))
#else
#define Stroika_Foundation_Debug_ValgrindDisableHelgrind_END(X)
#endif

/**
 *  If the current implementation of helgrind doesn't recognize std::atomic being atomic, disable warnings about those variables
 *  
 *  \note this still appears broken in valigrind on Ubuntu 1804/gcc
 */
#define Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic(X) \
    Stroika_Foundation_Debug_ValgrindDisableHelgrind (X)

/**
 *  \brief  Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE is ANNOTATE_HAPPENS_BEFORE except it can be used
 *          if no valgrind includes, and ifdefed out, and it can be used in an expression
 */
#if qStroika_FeatureSupported_Valgrind
#define Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE(X) \
    Stroika::Foundation::Debug::Do_Valgrind_ANNOTATE_HAPPENS_BEFORE_ (X)
#else
#define Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE(X) ((void)0)
#endif

/**
 *  \brief  Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER is ANNOTATE_HAPPENS_AFTER except it can be used
 *          if no valgrind includes, and ifdefed out, and it can be used in an expression
 */
#if qStroika_FeatureSupported_Valgrind
#define Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER(X) \
    Stroika::Foundation::Debug::Do_Valgrind_ANNOTATE_HAPPENS_AFTER_ (X)
#else
#define Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER(X) ((void)0)
#endif

#if defined(__cplusplus)
namespace Stroika::Foundation::Debug {
#if qStroika_FeatureSupported_Valgrind
    inline void Do_Valgrind_ANNOTATE_HAPPENS_BEFORE_ (const void* p)
    {
        // use this inline function def in Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE because then Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE and be used in comma operator expression
        ANNOTATE_HAPPENS_BEFORE (p);
    }
    inline void Do_Valgrind_ANNOTATE_HAPPENS_AFTER_ (const void* p)
    {
        // use this inline function def in Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_BEFORE because then Stroika_Foundation_Debug_Valgrind_ANNOTATE_HAPPENS_AFTER and be used in comma operator expression
        ANNOTATE_HAPPENS_AFTER (p);
    }
#endif
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Valgrind.inl"

#endif /*_Stroika_Foundation_Debug_Valgrind_h_*/

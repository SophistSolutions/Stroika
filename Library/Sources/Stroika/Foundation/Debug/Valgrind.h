/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Valgrind_h_
#define _Stroika_Foundation_Debug_Valgrind_h_  1

#include    "../StroikaPreComp.h"




/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Early</a>
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
 *
 *  TODO:
 *      @todo
 *
 */



#ifndef qStroika_FeatureSupported_Valgrind
#define qStroika_FeatureSupported_Valgrind  0
#endif

#if     qStroika_FeatureSupported_Valgrind
#include    "valgrind/helgrind.h"
#include    "valgrind/memcheck.h"
#endif

#if     qStroika_FeatureSupported_Valgrind
/*
 *  See https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug.html
 */
#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_BEFORE(A)  ANNOTATE_HAPPENS_BEFORE(A)
#define _GLIBCXX_SYNCHRONIZATION_HAPPENS_AFTER(A)   ANNOTATE_HAPPENS_AFTER(A)
#endif



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
#if     qStroika_FeatureSupported_Valgrind
#define Stroika_Foundation_Debug_ValgrindDisableHelgrind(X)     \
    VALGRIND_HG_DISABLE_CHECKING (&(X), sizeof (X))
#else
#define Stroika_Foundation_Debug_ValgrindDisableHelgrind(X)
#endif




/**
 *  If the current implementation of helgrind doesnt recognize std::atomic being atomic, disable warnings about those variables
 */
#define Stroika_Foundation_Debug_ValgrindDisableCheck_stdatomic(X)     \
    Stroika_Foundation_Debug_ValgrindDisableHelgrind(X)


#if     defined (__cplusplus)
namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


        }
    }
}
#endif



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Valgrind.inl"

#endif  /*_Stroika_Foundation_Debug_Valgrind_h_*/

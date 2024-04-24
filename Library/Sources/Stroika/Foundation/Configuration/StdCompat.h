/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_StdCompat_h_
#define _Stroika_Foundation_Configuration_StdCompat_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdarg>
#include <ranges>

// Various kooky constraints
//      (1) clang++15/16 don't set __cpp_lib_format, so cannot check __cpp_lib_format >= 201907 instead check __has_include(<format>)
//      (2) has_include <format> false positives on some versions of XCode, and no reason to even build qHasFeature_fmtlib unless
//          its needed, so check it first

#if qHasFeature_fmtlib
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/xchar.h>
#elif __has_include(<format>)
#include <format>
#endif

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Configuration::StdCompat {

    /**
     *  By default include all of std, but throw in selected missing things from some implementations.
     */
    using namespace std;

#if qHasFeature_fmtlib
#define qStroika_Foundation_Characters_FMT_PREFIX_ fmt
#elif __has_include(<format>)
#define qStroika_Foundation_Characters_FMT_PREFIX_ std
#else
    static_assert (false, "Stroika v3 requires some std::format compatible library - if building with one lacking builtin std::format, "
                          "configure --fmtlib use");
#endif

    /**
     *  To allow interop between std::format and fmt(fmtlib)::format, publish the names into the namespace 'Stroika::Foundation::Configuration::StdCompat' and use those.
     *  Lose this once I can fully depend upon std::format... --LGP 2024-03-12
     */
    using qStroika_Foundation_Characters_FMT_PREFIX_::format;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format_args;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format_error;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format_string;
    using qStroika_Foundation_Characters_FMT_PREFIX_::format_to;
    using qStroika_Foundation_Characters_FMT_PREFIX_::make_format_args;
    using qStroika_Foundation_Characters_FMT_PREFIX_::make_wformat_args;
    using qStroika_Foundation_Characters_FMT_PREFIX_::vformat;
    using qStroika_Foundation_Characters_FMT_PREFIX_::wformat_args;
    using qStroika_Foundation_Characters_FMT_PREFIX_::wformat_string;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StdCompat.inl"

#endif /*_Stroika_Foundation_Configuration_StdCompat_h_*/

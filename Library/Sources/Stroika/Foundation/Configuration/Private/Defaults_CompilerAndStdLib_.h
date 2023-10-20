﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

/**
 *  Often helpful in printout out warning etc messages
 *
 *      \par Example Usage:
 *           \code
 *              #define THREE 3
 *              #pragma message "The name of THREE: " _Stroika_Foundation_STRINGIFY_(THREE)
 *              #pragma message "The value of THREE: " _Stroika_Foundation_STRINGIFY2_(THREE)
 *          \endcode
 *          The name of THREE: THREE
 *          The value of THREE: 3
 */
#ifndef _Stroika_Foundation_STRINGIFY_
#define _Stroika_Foundation_STRINGIFY_(x) #x
#endif
#ifndef _Stroika_Foundation_STRINGIFY2_
#define _Stroika_Foundation_STRINGIFY2_(x) _Stroika_Foundation_STRINGIFY_ (x)
#endif

#if defined(__cplusplus)

/**
 *  https://github.com/cplusplus/draft/releases/download/n4868/n4868.pdf (search for __cplusplus)
 */
#define kStrokia_Foundation_Configuration_cplusplus_20 202002L

/*
 *******************************************************************
 *******************************************************************
 ******** CHECK / WARN about supported compilers/versions **********
 *******************************************************************
 *******************************************************************
 */
#include <climits> // include as little as possible but enuf to get LIBRARY defines like _LIBCPP_VERSION

/*
 *  @eee StroikaConfig.cpp
 *
 * We want to issue a warning about the compiler/build system compatability flags, but we don't want to issue the warning for every file
 * compiled.
 *
 * That way -for the COMMON case of using Stroika with a NEWER compiler than has been tested, you get ONE message, not one per file in stroika. This isnt perfect, because
 * you only get the warning when building Stroika, not YOUR application. But it was really a PITA building stroika based apps using an older
 * stroika and getting so many warnings.
 *
 *     -- LGP 2017-08-24
 */
#if defined(__clang__)

#if defined(__APPLE__)
// Must check CLANG first, since CLANG also defines GCC
// see
//      clang++-3.8 -dM -E - < /dev/null
#if (__clang_major__ < 14) || (__clang_major__ == 14 && (__clang_minor__ < 0))
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: Stroika v3 (older clang versions supported by Stroika v2.1) does not support versions prior to APPLE clang++ 14 (XCode 14)"
#endif
#if (__clang_major__ > 15)
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Info: Stroika untested with this version of clang++ (APPLE) - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#else
// Must check CLANG first, since CLANG also defines GCC
// see
//      clang++-3.8 -dM -E - < /dev/null
#if (__clang_major__ < 14)
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: Stroika v3 does not support versions prior to clang++ 14 (non-apple); note that Stroika v2.1 supports earlier clang "        \
    "versions"
#endif
#if (__clang_major__ > 16)
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Info: Stroika untested with this version of clang++ - (>16.0) USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#endif

#elif defined(__GNUC__)

#if __GNUC__ < 11
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: Stroika v3 does not support versions prior to GCC 11 (v2.1 supports g++7 and later, v2.0 supports g++5 and g++6 and g++-7)"
#endif
#if __GNUC__ > 13
#define _STROIKA_CONFIGURATION_WARNING_ "Info: Stroika untested with this version of GCC - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif

#elif defined(_MSC_VER)

#define _MSC_VER_2k22_17Pt0_ 1930
#define _MSC_VER_2k22_17Pt1_ 1931
#define _MSC_VER_2k22_17Pt2_ 1932
#define _MSC_VER_2k22_17Pt3_ 1933
#define _MSC_VER_2k22_17Pt4_ 1934
#define _MSC_VER_2k22_17Pt5_ 1935
#define _MSC_VER_2k22_17Pt6_ 1936
#define _MSC_VER_2k22_17Pt7_ 1937

// We COULD look at _MSC_FULL_VER but changes too often and too rarely makes a difference: just assume all bug defines the same for a given _MSC_VER
#if _MSC_VER < _MSC_VER_2k22_17Pt0_
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2022 (use Stroika v2.1 or earlier)"
#elif _MSC_VER <= _MSC_VER_2k22_17Pt7_
// We COULD look at _MSC_FULL_VER but changes too often and too rarely makes a difference: just assume all bug defines the same for a given _MSC_VER
#else
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: This version of Stroika is untested with this release (> 17.7) of Microsoft Visual Studio.net / Visual C++ - USING "         \
    "PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif

#else

#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: Stroika does recognize the compiler being used. It may work, but you may need to update some of the other defines for what " \
    "features are supported by your compiler."

#endif

/*
 *  If using libc++, require version 11 or later, since 
 *  version 10 missing new chrono code (and more?)
 * 
 *      NOTE ON _LIBCPP_VERSION  went from 15000 (5 digits) to 160000 (6 digits)
 */
#if defined(_LIBCPP_VERSION)
#if _LIBCPP_VERSION < 14000
#error "Stroika v3 requires a more c++-20 compliant version of std-c++ library than libc++14 (missing new chrono/span code for example); try newer libc++, older version of Stroika (e.g. 2.1), or libstdc++"
#endif
#endif

/* 
 * to find glibc version

        g++-11 foo.cpp

foo.cpp:
    #include <iostream>

    int main ()
    {
    using namespace std;
    cerr << "__GLIBCXX__=" << __GLIBCXX__ << "\n";
    cerr << "_GLIBCXX_RELEASE=" << _GLIBCXX_RELEASE << "\n";
    return 0;
    }

 prints __GLIBCXX__=20180728
 */
//#define GLIBCXX_10x_ 20200930
//#define GLIBCXX_11x_ 20210427
//#define GLIBCXX_11x_ 20210923
// DONT define GLIBCXX_11x_ cuz WAY too much of a moving target...

// NOTE SURE WHY THIS ISN'T (always) WORKING??? WORKS FOR 'THREE' case above - maybe these aren't really defined
// #pragma message "The value of __GLIBCXX__: " _Stroika_Foundation_STRINGIFY2_(__GLIBCXX__)
// #pragma message "The value of _GLIBCXX_RELEASE: " _Stroika_Foundation_STRINGIFY2_(_GLIBCXX_RELEASE)
// #pragma message "The value of _LIBCPP_VERSION: " _Stroika_Foundation_STRINGIFY2_(_LIBCPP_VERSION)
// #pragma message "The value of __cpp_lib_three_way_comparison: " _Stroika_Foundation_STRINGIFY2_(__cpp_lib_three_way_comparison)
// #pragma message "The value of __cpp_impl_three_way_comparison: " _Stroika_Foundation_STRINGIFY2_(__cpp_impl_three_way_comparison)

/*
 *
 *  Stroika supports a wide variety of compilers with a wide variety of bugs and degrees of C++ standards conformance.
 *
 *  If you see the compile message "USING PREVIOUS COMPILER VERSION BUG DEFINES" - Stroika will use the set of bug defines
 *  from the most recent version of the compiler available. That may nor may not work.
 *
 *  To FIX Stroika - force this true, and then add // in front of #ifndef       CompilerAndStdLib_AssumeBuggyIfNewerCheck_ and its endif,
 *  and go through and test and adjust each bug define for the new version of the compiler.
 *
 */
#ifndef CompilerAndStdLib_AssumeBuggyIfNewerCheck_
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) (X)
#endif

#if __has_include(<version>)
#include <version>
#elif defined(__clang__)
// include __config which defines _LIBCPP_VERSION by including ciso646 (see http://stackoverflow.com/questions/31657499/how-to-detect-stdlib-libc-in-the-preprocessor)
// For libc++ it is recommended to #include <ciso646> which serves no purpose in C++ and declares nothing, but for libc++ does define the _LIBCPP_VERSION macro
// note if this file is not found, check sudo apt-get install libc++-dev
#if __cplusplus < 202002L
#include <ciso646>
#endif
#endif

// For pre C++20, to get _GLIBCXX_RELEASE define
#if !__has_include(<version>)
#if __has_include(<bits/c++config.h>)
#include <bits/c++config.h>
#endif
#endif

/*
 *******************************************************************
 *******************************************************************
 **************** Define Features (and Bug Workarounds) ************
 *******************************************************************
 *******************************************************************
 */
//static_assert (__cpp_lib_atomic_shared_ptr >= 201711, "Stroika v3 requires __cpp_lib_atomic_shared_ptr >= 201711");   -- sadly fails on xcode 14.3, so must live with no having this...

/*
 *https://developercommunity.visualstudio.com/t/__sanitizer_annotate_contiguous_containe/10119696?entry=problem&ref=native&refTime=1660499588239&refUserId=b9c6175e-9d87-6b50-bc33-61424496814f
>C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.34.31933\include\sanitizer\common_interface_defs.h(154): error C2382: 'std::__sanitizer_annotate_contiguous_container': redefinition; different exception specifications
1>C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.34.31933\include\vector(451): note: see declaration of 'std::__sanitizer_annotate_contiguous_container'
1>C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.34.31933\include\sanitizer\common_interface_defs.h(154): error C2733: '__sanitizer_annotate_contiguous_container': you cannot overload a function with 'extern "C"' linkage
1>C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.34.31933\include\vector(451): note: see declaration of 'std::__sanitizer_annotate_contiguous_container'
1
*/
#ifndef qCompilerAndStdLib_sanitizer_annotate_contiguous_container_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k22_17Pt3_
// Verified still broken in _MSC_VER_2k22_17Pt4_
// Verified still broken in _MSC_VER_2k22_17Pt5_
// Appears fixed in _MSC_VER_2k22_17Pt6_
#define qCompilerAndStdLib_sanitizer_annotate_contiguous_container_Buggy                                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k22_17Pt3_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k22_17Pt5_)
#else
#define qCompilerAndStdLib_sanitizer_annotate_contiguous_container_Buggy 0
#endif

#endif

/*
* **** TEST IN RELEASE BUILD
* 
C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Frameworks\Led\Marker.inl(218): error C2027: use of undefined type 'Stroika::Frameworks::Led::TextStore'
C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Frameworks\Led\Marker.h(96): note: see declaration of 'Stroika::Frameworks::Led::TextStore'

AND I THINK diff manifestation of same bug

\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Execution\Throw.inl(57): warning C4353: nonstandard extension used: constant 0 as function expression.  Use '__noop' function intrinsic instead
C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Execution\Throw.inl(146): note: see reference to function template instantiation 'void Stroika::Foundation::Execution::Throw<std::bad_alloc>(const T &,const char *)' being compiled
*/
#ifndef qCompilerAndStdLib_ArgumentDependentLookupInTemplateExpansionTooAggressiveNowBroken_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k22_17Pt4_
// still broken in _MSC_VER_2k22_17Pt5_
// still broken in _MSC_VER_2k22_17Pt6_
// still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_ArgumentDependentLookupInTemplateExpansionTooAggressiveNowBroken_Buggy                                          \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k22_17Pt4_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_ArgumentDependentLookupInTemplateExpansionTooAggressiveNowBroken_Buggy 0
#endif

#endif

//
#ifndef qCompilerAndStdLib_ArgumentDependentLookupInTemplateExpansionTooAggressiveNowBroken_Buggy
#if defined(_MSC_VER)
// first broken in _MSC_VER_2k22_17Pt4_
#define qCompilerAndStdLib_ArgumentDependentLookupInTemplateExpansionTooAggressiveNowBroken_Buggy                                          \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k22_17Pt4_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k22_17Pt4_)
#else
#define qCompilerAndStdLib_ArgumentDependentLookupInTemplateExpansionTooAggressiveNowBroken_Buggy 0
#endif

#endif

/// error C2760: syntax error: ';' was unexpected here; expected ')
///C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_MultiSet.h(246): error C2760: syntax error: unexpected token ';', expected ')'
// You can use [[maybe_unused]] after the identifier, but not before the auto
/*
C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_MultiSet.h(250): error C2760: syntax error: ';' was unexpected here; expected ')'
C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_MultiSet.h(250): error C2065: 'it': undeclared identifier
C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_MultiSet.h(250): error C2760: syntax error: ')' was unexpected here; expected ';'
C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_MultiSet.h(250): error C3878: syntax error: unexpected token ')' following 'expression_statement'
C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_MultiSet.h(250): note: error recovery skipped: ')'
*/
#ifndef qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop_Buggy

#if defined(_MSC_VER)
// verified broken in _MSC_VER_2k22_17Pt0_
// verified broken in _MSC_VER_2k22_17Pt1_
// verified broken in _MSC_VER_2k22_17Pt2_
// verified broken in _MSC_VER_2k22_17Pt3_
// verified broken in _MSC_VER_2k22_17Pt4_
// verified broken in _MSC_VER_2k22_17Pt5_
// verified broken in _MSC_VER_2k22_17Pt6_
// verified broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop_Buggy                                                                          \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop_Buggy 0
#endif

#endif

#if __cpp_lib_chrono < 201611
#error "Stroika v3 requires c++20 chrono library support"
#endif
// #if __cpp_lib_chrono < 201907
// #error "Stroika v3 requires c++20 chrono library support"
// #endif

/**
 *
=================================================================
==13840==ERROR: AddressSanitizer: stack-use-after-scope on address 0x00dedb3fef90 at pc 0x7ff71b65a40f bp 0x00dedb3fdbd0 sp 0x00dedb3fdbd8
READ of size 8 at 0x00dedb3fef90 thread T0
    #0 0x7ff71b65a40e in std::initializer_list<Stroika::Foundation::IO::Network::URI>::begin C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\include\initializer_list:38
    #1 0x7ff71b6266f6 in `anonymous namespace'::Test1_URI_::Private_::Test_RegressionDueToBugInCompareURIsC20Spaceship_ C:\Sandbox\Stroika\DevRoot\Tests\43\Test.cpp:381
    #2 0x7ff71b626e41 in `anonymous namespace'::Test1_URI_::DoTests_ C:\Sandbox\Stroika\DevRoot\Tests\43\Test.cpp:407
    #3 0x7ff71b62d888 in `anonymous namespace'::DoRegressionTests_ C:\Sandbox\Stroika\DevRoot\Tests\43\Test.cpp:595
    #4 0x7ff71b65fb19 in Stroika::TestHarness::PrintPassOrFail C:\Sandbox\Stroika\DevRoot\Tests\TestHarness\TestHarness.cpp:75
    #5 0x7ff71b62d8cd in main C:\Sandbox\Stroika\DevRoot\Tests\43\Test.cpp:607
    #6 0x7ff71bc3d008 in invoke_main d:\a01\_work\43\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:78
    #7 0x7ff71bc3cf5d in __scrt_common_main_seh d:\a01\_work\43\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:288
    #8 0x7ff71bc3ce1d in __scrt_common_main d:\a01\_work\43\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:330
    #9 0x7ff71bc3d07d in mainCRTStartup d:\a01\_work\43\s\src\vctools\crt\vcstartup\src\startup\exe_main.cpp:16
    #10 0x7ffa00ff54df in BaseThreadInitThunk+0xf (C:\WINDOWS\System32\KERNEL32.DLL+0x1800154df)
    #11 0x7ffa023a485a in RtlUserThreadStart+0x2a (C:\WINDOWS\SYSTEM32\ntdll.dll+0x18000485a)

Address 0x00dedb3fef90 is located in stack of thread T0
SUMMARY: AddressSanitizer: stack-use-after-scope C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.31.31103\include\initializer_list:38 in std::initializer_list<Stroika::Foundation::IO::Network::URI>::begin
 * 
 * 
 * 
 Sent email to MSFT - can reproduce with ASAN and -- https://developercommunity.visualstudio.com/t/initializer-list-lifetime-buggy-maybe-just-asan-is/1439352

                for (string i : initializer_list<string>{"a", "b", "c"}) {
                    assert (i.length () == 1);
                }

 * UPDATED BUG REPORT WITH 
 *      https://developercommunity.visualstudio.com/t/initializer-list-lifetime-buggy-maybe-just-asan-is/1439352#T-N1439942-N1454940
 *      reproduction example
 * 
 * .
 */
#ifndef qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy

#if defined(_MSC_VER)
// verified broken in _MSC_VER_2k22_17Pt0_
// verified broken in _MSC_VER_2k22_17Pt1_
// verified broken in _MSC_VER_2k22_17Pt2_
// verified broken in _MSC_VER_2k22_17Pt3_
// verified broken in _MSC_VER_2k22_17Pt4_
// verified broken in _MSC_VER_2k22_17Pt5_
// verified broken in _MSC_VER_2k22_17Pt6_
// verified broken in _MSC_VER_2k22_17Pt7_   - MSFT claims fixed, but I tried to warn them - it is not - https://developercommunity.visualstudio.com/t/initializer-list-lifetime-buggy-maybe-just-asan-is/1439352#T-N1439942-N1454940
#define qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy 0
#endif

#endif

/*
 *https://developercommunity.visualstudio.com/t/__sanitizer_annotate_contiguous_containe/10119696?entry=problem&ref=native&refTime=1660499588239&refUserId=b9c6175e-9d87-6b50-bc33-61424496814f
 */

#ifndef qCompilerAndStdLib_sanitizer_annotate_contiguous_container_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k22_17Pt3_
#define qCompilerAndStdLib_sanitizer_annotate_contiguous_container_Buggy                                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k22_17Pt3_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k22_17Pt3_)
#else
#define qCompilerAndStdLib_sanitizer_annotate_contiguous_container_Buggy 0
#endif

#endif

/*
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Cryptography\Encoding\Algorithm\Base64.cpp(60): error C2440: 'initializing': cannot convert from 'int' to 'std::byte'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Cryptography\Encoding\Algorithm\Base64.cpp(60): note: Conversion to enumeration type requires an explicit cast (static_cast, C-style cast or function-style cast)
*/
// see https://en.cppreference.com/w/cpp/types/byte "due to C++17 relaxed enum class initialization rules."
#ifndef qCompilerAndStdLib_relaxedEnumClassInitializationRules_Buggy

#if defined(_MSC_VER)
// verified still broken in _MSC_VER_2k22_17Pt0_
// verified still broken in _MSC_VER_2k22_17Pt1_
// verified still broken in _MSC_VER_2k22_17Pt2_
// verified still broken in _MSC_VER_2k22_17Pt3_
// verified still broken in _MSC_VER_2k22_17Pt4_
// verified still broken in _MSC_VER_2k22_17Pt5_
// verified still broken in _MSC_VER_2k22_17Pt6_
// verified still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_relaxedEnumClassInitializationRules_Buggy                                                                       \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_relaxedEnumClassInitializationRules_Buggy 0
#endif

#endif

/*
* 
* ONLY in Release-x86 builds broken
* 
// Get runtime failure in Test_12_DateRange_ in 
FAILED : RegressionTestFailure;
dr.Contains (dr.GetMidpoint ());
C :\Sandbox\Stroika\DevRoot\Tests\50\Test.cpp : 750
        [](3 seconds)[50] Foundation::Time (../Builds/Release-U-32/Tests/Test50.exe)
 */
#ifndef qCompilerAndStdLib_ReleaseBld32Codegen_DateRangeInitializerDateOperator_Buggy

#if defined(_MSC_VER)
// APPEARS FIXED in _MSC_VER_2k22_17Pt0_
// And then RE-BROKEN in _MSC_VER_2k22_17Pt2_
// APPEARS still BROKEN in _MSC_VER_2k22_17Pt3_
// APPEARS still BROKEN in _MSC_VER_2k22_17Pt4_
// APPEARS still BROKEN in _MSC_VER_2k22_17Pt5_
// Fixed in _MSC_VER_2k22_17Pt6_
#define qCompilerAndStdLib_ReleaseBld32Codegen_DateRangeInitializerDateOperator_Buggy                                                      \
    (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((_MSC_VER_2k22_17Pt2_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k22_17Pt5_)) && !qDebug && defined (_M_IX86))
#else
#define qCompilerAndStdLib_ReleaseBld32Codegen_DateRangeInitializerDateOperator_Buggy 0
#endif

#endif

/*
*  on Windows DEBUG x86 builds only...
==2736==ERROR: AddressSanitizer: container-overflow on address 0x0110ed9d at pc 0x0020f13a bp 0x0110df2c sp 0x0110db0c
READ of size 6 at 0x0110ed9d thread T0
    #0 0x20f155 in __asan_wrap_strnlen D:\a\_work\1\s\src\vctools\asan\llvm\compiler-rt\lib\sanitizer_common\sanitizer_common_interceptors.inc:389
    #1 0x5c5548 in __crt_stdio_output::output_processor<char,__crt_stdio_output::string_output_adapter<char>,__crt_stdio_output::standard_base<char,__crt_stdio_output::string_output_adapter<char> > >::type_case_s_compute_narrow_string_length minkernel\crts\ucrt\inc\corecrt_internal_stdio_output.h:2323
    #2 0x5c4e8e in __crt_stdio_output::output_processor<char,__crt_stdio_output::string_output_adapter<char>,__crt_stdio_output::standard_base<char,__crt_stdio_output::string_output_adapter<char> > >::type_case_s minkernel\crts\ucrt\inc\corecrt_internal_stdio_output.h:2310
    #3 0x5ba7f1 in __crt_stdio_output::output_processor<char,__crt_stdio_output::string_output_adapter<char>,__crt_stdio_output::standard_base<char,__crt_stdio_output::string_output_adapter<char> > >::state_case_type minkernel\crts\ucrt\inc\corecrt_internal_stdio_output.h:2054
    #4 0x5b2ba8 in __crt_stdio_output::output_processor<char,__crt_stdio_output::string_output_adapter<char>,__crt_stdio_output::standard_base<char,__crt_stdio_output::string_output_adapter<char> > >::process minkernel\crts\ucrt\inc\corecrt_internal_stdio_output.h:1699
    #5 0x5a6104 in common_vsprintf<__crt_stdio_output::standard_base,char> minkernel\crts\ucrt\src\appcrt\stdio\output.cpp:167
    #6 0x5c9d75 in __stdio_common_vsprintf minkernel\crts\ucrt\src\appcrt\stdio\output.cpp:239
    #7 0x23b80e in vsnprintf C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt\stdio.h:1439
    #8 0x23b770 in snprintf C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt\stdio.h:1931
    #9 0x2278cc in Stroika::Foundation::Debug::Emitter::DoEmitMessage_<wchar_t> C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:354
    #10 0x221f6e in Stroika::Foundation::Debug::Emitter::EmitTraceMessage C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:259
    #11 0x224555 in Stroika::Foundation::Debug::Private_::EmitFirstTime C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:159
    #12 0x1bf670 in `Stroika::Foundation::Debug::Emitter::Get'::`2'::<lambda_1>::operator() C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.inl:34
    #13 0x1b607a in std::invoke<`Stroika::Foundation::Debug::Emitter::Get'::`2'::<lambda_1> > C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.33.31629\include\type_traits:1548
    #14 0x1b4f65 in std::call_once<`Stroika::Foundation::Debug::Emitter::Get'::`2'::<lambda_1> > C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.33.31629\include\mutex:558
    #15 0x1c1012 in Stroika::Foundation::Debug::Emitter::Get C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.inl:33
    #16 0x223534 in Stroika::Foundation::Debug::TraceContextBumper::TraceContextBumper C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:526
    #17 0x223328 in Stroika::Foundation::Debug::TraceContextBumper::TraceContextBumper C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:559
    #18 0x372092 in Stroika::Foundation::Execution::SignalHandlerRegistry::SignalHandlerRegistry C:\Stroika\Library\Sources\Stroika\Foundation\Execution\SignalHandlers.cpp:317
    #19 0x371f21 in Stroika::Foundation::Execution::SignalHandlerRegistry::Get C:\Stroika\Library\Sources\Stroika\Foundation\Execution\SignalHandlers.cpp:306
    #20 0x1cba70 in Stroika::TestHarness::Setup C:\Stroika\Tests\TestHarness\TestHarness.cpp:69
    #21 0x1ad737 in main C:\Stroika\Tests\04\Test.cpp:251
    #22 0x587b22 in invoke_main D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:78
    #23 0x587a26 in __scrt_common_main_seh D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:288
    #24 0x5878cc in __scrt_common_main D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:330
    #25 0x587b87 in mainCRTStartup D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_main.cpp:16
    #26 0x76f06708 in BaseThreadInitThunk+0x18 (C:\Windows\System32\KERNEL32.DLL+0x6b816708)
    #27 0x77567cfc in RtlGetFullPathName_UEx+0xac (C:\Windows\SYSTEM32\ntdll.dll+0x4b2e7cfc)
    #28 0x77567cca in RtlGetFullPathName_UEx+0x7a (C:\Windows\SYSTEM32\ntdll.dll+0x4b2e7cca)
*/
#ifndef qCompilerAndStdLib_Debug32Codegen_make_pair_string_Buggy

#if defined(_MSC_VER)
// first/only found broken in _MSC_VER_2k22_17Pt3_
// Appears FIXED in _MSC_VER_2k22_17Pt4_
#define qCompilerAndStdLib_Debug32Codegen_make_pair_string_Buggy                                                                           \
    (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((_MSC_VER == _MSC_VER_2k22_17Pt3_)) && qDebug && defined (_M_IX86))
#else
#define qCompilerAndStdLib_Debug32Codegen_make_pair_string_Buggy 0
#endif

#endif

/*
 * https://developercommunity.visualstudio.com/t/Fatal-Corruption-in-X86-ASAN-regression/10130063?port=1025&fsid=7a8d8e50-f549-4b33-a16d-c10fbf32b8fc&entry=problem
 */

#ifndef qCompilerAndStdLib_Debug32_asan_Poison_Buggy

#if defined(_MSC_VER)
// first/only found broken in _MSC_VER_2k22_17Pt3_
// Appears FIXED in _MSC_VER_2k22_17Pt4_
#define qCompilerAndStdLib_Debug32_asan_Poison_Buggy                                                                                       \
    (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((_MSC_VER == _MSC_VER_2k22_17Pt3_)) && qDebug && defined (_M_IX86))
#else
#define qCompilerAndStdLib_Debug32_asan_Poison_Buggy 0
#endif

#endif

/*
In file included from Test.cpp:16:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/Mapping_stdhashmap.h:157:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Factory/../Concrete/Mapping_stdhashmap.inl:267:61: error: out-of-line definition of 'Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>' does not match any declaration in 'Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>'
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (ITERABLE_OF_ADDABLE&& src)
                                                            ^~~~~~~~~~~~~~~~~~
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Factory/../Concrete/Mapping_stdhashmap.inl:283:61: error: out-of-line definition of 'Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>' does not match any declaration in 'Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>'
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
                                                            ^~~~~~~~~~~~~~~~~~
2 errors generated.
make[4]: *** [/Sandbox/Stroika-Dev//ScriptsLib/SharedBuildRules-Default.mk:30: /Sandbox/Stroika-Dev/IntermediateFiles/clang++-10-debug/
*/

#ifndef qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy

#if defined(__clang__) && defined(__APPLE__)
// first noticed broken in apply clang 14
// reproduced on clang 15
#define qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy                                               \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// first noticed broken in apply clang 14
// still broken in clang++ 15
// still broken in clang++ 16
#define qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy                                               \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 16))
#else
#define qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy 0
#endif

#endif

/*

   https://bugs.llvm.org/show_bug.cgi?id=42111


Linking  $StroikaRoot/Builds/Debug/HTMLViewCompiler...
duplicate symbol 'thread-local initialization routine for Stroika::Foundation::Execution::Thread::Ptr::sCurrentThreadRep_' in:
    /Users/runner/work/Stroika/Stroika/Builds/Debug/Stroika-Foundation.a(Thread.o)
    /Users/runner/work/Stroika/Stroika/Builds/Debug/Stroika-Foundation.a(WaitableEvent.o)
ld: 1 duplicate symbol for architecture x86_64
clang: error: linker command failed with exit code 1 (use -v to see invocation)

troika Tools {Debug}:
      Linking  $StroikaRoot/Builds/Debug/HTMLViewCompiler...
duplicate symbol '__ZTHN7Stroika10Foundation9Execution6Thread3Ptr18sCurrentThreadRep_E' in:
    /Users/lewis/Sandbox/StroikaDev/Builds/Debug/Stroika-Foundation.a[105](Thread.o)
    /Users/lewis/Sandbox/StroikaDev/Builds/Debug/Stroika-Foundation.a[110](WaitableEvent.o)
ld: 1 duplicate symbols
clang: error: linker command failed with exit code 1 (use -v to see invocation)
make[4]: ***



SIMILAR BUT SLIGHTYL DIFF ISSUE ON GCC
.. 
/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/valgrind-release-SSLPurify-NoBlockAlloc/Stroika-Foundation.a(WaitableEvent.o): in function `bool Stroika::Foundation::Execution::ConditionVariable<std::mutex, std::_V2::condition_variable_any>::wait_until<Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly(double)::{lambda()#1}>(std::unique_lock<std::mutex>&, double, Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly(double)::{lambda()#1}&&) [clone .constprop.0]':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ConditionVariable.inl:111: undefined reference to `Stroika::Foundation::Execution::Thread::GetCurrentThreadStopToken()'
/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/valgrind-release-SSLPurify-NoBlockAlloc/Stroika-Foundation.a(WaitableEvent.o): in function `Stroika::Foundation::Execution::ConditionVariable<std::mutex, std::_V2::condition_variable_any>::wait_until(std::unique_lock<std::mutex>&, double)':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ConditionVaria

*/
#ifndef qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy

#if defined(__clang__) && defined(__APPLE__)
// first noticed broken in apply clang 14
// replicated in xcode 15.0
#define qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__)
// first noticed broken in apply clang 14
// replicated in clang 15.0
#define qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__GNUC__) && !defined(__clang__)
// FIRST SEEN BROKEN IN GCC 11
#define qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 11)
#else
#define qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy 0
#endif

#endif

/*
    Compiling Tests/11/Test.cpp ...
PLEASE submit a bug report to https://github.com/llvm/llvm-project/issues/ and include the crash backtrace, preprocessed source, and associated run script.
Stack dump:
0.      Program arguments: clang++-15 --std=c++20 -I/mnt/c/Sandbox/Stroika/DevRoot/Builds/clang++-15-release-libstdc++/ThirdPartyComponents/include/ -I/mnt/c/Sandbox/Stroika/DevRoot/Library/Sources/ -I/mnt/c/Sandbox/Stroika/DevRoot/IntermediateFiles/clang++-15-release-libstdc++/ -Wall -Wno-switch -Wno-sign-compare -Wno-unused-function -Wno-unused-local-typedef -Wno-future-compat -Wno-unqualified-std-cast-call -O3 -fvisibility=hidden -g -DqDebug=0 -DqHasFeature_LibCurl=1 -DqHasFeature_OpenSSL=1 -DqHasFeature_WinHTTP=0 -DqHasFeature_Xerces=1 -DqHasFeature_ZLib=1 -DqHasFeature_sqlite=1 -DqHasFeature_LZMA=1 -DqHasFeature_boost=1 -DqTraceToFile=1 -DqDefaultTracingOn=1 -flto -stdlib=libstdc++ -c Test.cpp -o /mnt/c/Sandbox/Stroika/DevRoot/IntermediateFiles/clang++-15-release-libstdc++/Tests/11/Test.o
1.      <eof> parser at end of file
2.      /mnt/c/Sandbox/Stroika/DevRoot/Library/Sources/Stroika/Foundation/Containers/Factory/../Concrete/../Association.h:531:25: instantiating function definition 'Stroika::Foundation::Containers::Association<int, int>::operator=='
3.      /mnt/c/Sandbox/Stroika/DevRoot/Library/Sources/Stroika/Foundation/Containers/Factory/../Concrete/../Association.h:646:19: instantiating function definition 'Stroika::Foundation::Containers::Association<int, int>::EqualsComparer<>::EqualsComparer'
Stack dump without symbol names (ensure you have llvm-symbolizer in your PATH or set the environment var `LLVM_SYMBOLIZER_PATH` to point to it):
/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1(_ZN4llvm3sys15PrintStackTraceERNS_11raw_ostreamEi+0x31)[0x7f3d1ed9c3b1]
/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1(_ZN4llvm3sys17RunSignalHandlersEv+0xee)[0x7f3d1ed9a0fe]
/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1(_ZN4llvm3sys15CleanupOnSignalEm+0x101)[0x7f3d1ed9b771]
/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1(+0xe2767f)[0x7f3d1ecbf67f]
/lib/x86_64-linux-gnu/libc.so.6(+0x42520)[0x7f3d1d978520]
/usr/lib/llvm-15/bin/../lib/libclang-cpp.so.15(+0xda0b
*/

#ifndef qCompilerAndStdLib_RequiresIEqualsCrashesAssociation_Buggy

#if defined(__clang__) && defined(__APPLE__)
// wag untesed
// seems fixed in XCode 15
#define qCompilerAndStdLib_RequiresIEqualsCrashesAssociation_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 14))
#elif defined(__clang__) && !defined(__APPLE__)
// first noticed broken in apply clang 14
// broken in clang 15
// appears fixed in clang++16
#define qCompilerAndStdLib_RequiresIEqualsCrashesAssociation_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompilerAndStdLib_RequiresIEqualsCrashesAssociation_Buggy 0
#endif

#endif

/**
 *
On Vs2k19:
1>   Compiling Library/Sources/Stroika/Foundation/Cryptography/Digest/Algorithm/Jenkins.cpp ...
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): error C2244: 'Stroika::Foundation::Containers::Collection<T>::Collection': unable to match function definition to an existing declaration
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(67): note: see declaration of 'Stroika::Foundation::Containers::Collection<T>::Collection'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: definition
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(CONTAINER_OF_ADDABLE &&)'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: existing declarations
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(const Collection<T>::Iterable<T>::PtrImplementationTemplate<Stroika::Foundation::Containers::Collection<T>::_IRep> &) noexcept'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(Collection<T>::Iterable<T>::PtrImplementationTemplate<Stroika::Foundation::Containers::Collection<T>::_IRep> &&) noexcept'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(COPY_FROM_ITERATOR_OF_ADDABLE,COPY_FROM_ITERATOR_OF_ADDABLE)'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(CONTAINER_OF_ADDABLE &&)'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(const std::initializer_list<_Elem> &)'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(const Stroika::Foundation::Containers::Collection<T> &) noexcept'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(Stroika::Foundation::Containers::Collection<T> &&) noexcept'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(void)'
1>   Compiling Library/Sources/Stroika/Foundation/Characters/Platform/Windows/CodePage.cpp ...
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): error C2244: 'Stroika::Foundation::Containers::Collection<T>::Collection': unable to match function definition to an existing declaration
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(67): note: see declaration of 'Stroika::Foundation::Containers::Collection<T>::Collection'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: definition
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(CONTAINER_OF_ADDABLE &&)'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: existing declarations
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(const Collection<T>::Iterable<T>::PtrImplementationTemplate<Stroika::Foundation::Containers::Collection<T>::_IRep> &) noexcept'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(Collection<T>::Iterable<T>::PtrImplementationTemplate<Stroika::Foundation::Containers::Collection<T>::_IRep> &&) noexcept'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Containers\Collection.inl(68): note: 'Stroika::Foundation::Containers::Collection<T>::Collection(COPY_FROM_ITERATOR_OF_ADDABLE,COPY_FROM_ITERATOR_OF_ADDABLE)'


CLANG:
         Compiling Library/Sources/Stroika/Foundation/Characters/String.cpp ... 
In file included from CipherAlgorithm.cpp:14:
In file included from ./../../Memory/Optional.h:16:
In file included from ./../../Debug/../Characters/../Memory/../Containers/Adapters/Adder.h:13:
In file included from ./../../Containers/Factory/../Concrete/../Collection.h:411:
./../../Containers/Collection.inl:68:27: error: out-of-line definition of 'Collection<T>' does not match any declaration in 'Collection<T>'
    inline Collection<T>::Collection (CONTAINER_OF_ADDABLE&& src)
                          ^~~~~~~~~~
         Compiling Library/Sources/Stroika/Foundation/Characters/String2Int.cpp ... 


WORKAROUND IF NEEDED
in enable_if_t's, but may not need this anymore
#if qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy
                                                     and is_convertible_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>, T>
#else
                                                     and Collection<T>::template IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>
#endif

 */
#ifndef qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy

#if defined(_MSC_VER)
// BROKEN in _MSC_VER_2k22_17Pt0_
// Appears to work fine now _MSC_VER_2k22_17Pt1_
#define qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy                                                                       \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy 0
#endif

#endif

/*
      SEE 
            https://stackoverflow.com/questions/29483120/program-with-noexcept-constructor-accepted-by-gcc-rejected-by-clang

    ===
Test1.cpp:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Cache/Memoizer.h:72:9: error: exception specification of explicitly defaulted move constructor does not match the calculated
      one
        Memoizer (Memoizer&& from) noexcept = default;
        ^
Test.cpp:241:78: note: in instantiation of template class 'Stroika::Foundation::Cache::Memoizer<int, MemoizerSupport::DEFAULT_CACHE_BWA_, int, int>' requested here
                Memoizer<int, MemoizerSupport::DEFAULT_CACHE_BWA_, int, int> memoizer{[&totalCallsCount] (int a, int b) { totalCallsCount++;  return a + b; }};
                                                                             ^
1 error generated.
/Sandbox/Stroika-Dev//ScriptsLib/SharedBuildRules-Default.mk:19: recipe for target '/Sandbox/Stroika-Dev/IntermediateFiles/clang++-6-release-libstdc++/Tests/01/Test.o' failed
make[3]: *** [/Sandbox/Str
*/
#ifndef qCompilerAndStdLib_MoveCTORDelete_N4285_Buggy

#if defined(_MSC_VER)
// Broken in _MSC_VER_2k22_17Pt0_
// Verified fixed in _MSC_VER_2k22_17Pt1_
#define qCompilerAndStdLib_MoveCTORDelete_N4285_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_MoveCTORDelete_N4285_Buggy 0
#endif

#endif

/*
    http://stackoverflow.com/questions/15438968/vswprintf-fails-for-certain-unicode-codepoints-under-mac-os-x
    http://stackoverflow.com/questions/11713745/why-does-the-printf-family-of-functions-care-about-locale

    NOTE = http://www.unix.com/man-page/POSIX/3posix/vswprintf/ says there can be dependencies on the current locale, but
    only mentions for creating intergers - not for presense of unicode characters - which makes no apparent sense.

    NOTE - this comes up with processing with Format (e.g. DbgTrace) - of things that have been run through LimitLength () - which inserts a unicode elipsis character.
 */
#ifndef qCompilerAndStdLib_vswprintf_errantDependencyOnLocale_Buggy

#if defined(__APPLE__) && defined(__MACH__) /*qPlatform_MacOS - not not including Defaults_Configuration_Common.h before here*/
#define qCompilerAndStdLib_vswprintf_errantDependencyOnLocale_Buggy 1
#else
#define qCompilerAndStdLib_vswprintf_errantDependencyOnLocale_Buggy 0
#endif

#endif

/*
 *     Assert (::sem_init (&fSem_, kpshared, defaultValue) == 0) failed in 'Stroika::Foundation::Execution::Platform::POSIX::SemWaitableEvent::SemWaitableEvent()'; SemWaitableEvent.cpp:26
ABORTING...
 *
 *     https://stackoverflow.com/questions/1413785/sem-init-on-os-x
 */
#ifndef qCompilerAndStdLib_unnamed_semaphores_Buggy

#if defined(__APPLE__) && defined(__MACH__) /*qPlatform_MacOS - not not including Defaults_Configuration_Common.h before here*/
#define qCompilerAndStdLib_unnamed_semaphores_Buggy 1
#else
#define qCompilerAndStdLib_unnamed_semaphores_Buggy 0
#endif

#endif

/*
 *  NOTE - when this fails - it compiles but crashes in MSFT implementaiton
 *
 *                []  (19 seconds)  [50]  Foundation::Time  (../Builds/Debug-U-32/Test50/Test50.exe) crash/assert failure
 *                  NOTE - assert error so only fails on DEBUG builds

 As of 
    _STL_VERIFY(_Strbuf != nullptr, "istreambuf_iterator is not dereferencable"); asserts out

    https://developercommunity.visualstudio.com/t/qcompilerandstdlib-std-get-time-pctx-buggy/1359575

    REPORTED 2021-03-05 APX

 */
#ifndef qCompilerAndStdLib_std_get_time_pctx_Buggy

#if defined(_MSC_VER)
// VERIFIED STILL BROKEN in _MSC_VER_2k22_17Pt0_
// VERIFIED STILL BROKEN in _MSC_VER_2k22_17Pt1_
// VERIFIED STILL BROKEN in _MSC_VER_2k22_17Pt2_
// VERIFIED STILL BROKEN in _MSC_VER_2k22_17Pt3_
// APPARENTLY FIXED IN _MSC_VER_2k22_17Pt4_
#define qCompilerAndStdLib_std_get_time_pctx_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt3_)
#else
#define qCompilerAndStdLib_std_get_time_pctx_Buggy 0
#endif

#endif

/*
C:\Sandbox\Stroika\DevRoot\Samples\ActiveLedIt\Sources\Toolbar.cpp(660): error C2440: 'initializing': cannot convert from '_Ty' to 'ATL::CComQIPtr<IALCommand,& _GUID_e331d383_5bba_401a_91eb_5d3538b16053>'
        with
        [
            _Ty=ATL::CComPtr<IALCommand>
        ]
C:\Sandbox\Stroika\DevRoot\Samples\ActiveLedIt\Sources\Toolbar.cpp(660): note: No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called
C:\Sandbox\Stroika\DevRoot\Samples\ActiveLedIt\Sources\Toolbar.cpp(812): error C2440: 'initializing': cannot convert from 'ATL::CComPtr<IALCommand>' to 'ATL::CComVariant'
C:\Sandbox\Stroika\DevRoot\Samples\ActiveLedIt\Sources\Toolbar.cpp(812): note: No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called
C:\Sandbox\Stroika\DevRoot\Samples\ActiveLedIt\Sources\Toolbar.cpp(825): error C2440: 'initializing': cannot convert from 'ATL::CComPtr<IALCommand>' to 'ATL::CComVariant'
C:\Sandbox\Stroika\DevRoot\Samples\ActiveLedIt\Sources\Toolbar.cpp(825): note: No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called
C:\Sandbox\Stroika\DevRoot\Samples\ActiveLedIt\Sources\Toolbar.cpp(885): error C2440: 'initializing': cannot convert from 'ATL::CComPtr<IDispatch>' to 'ATL::CComQIPtr<IALCommandList,& _GUID_ae36308e_681b_402b_afb5_ec4737844073>'
C:\Sandbox\Stroika\DevRoot\Samples\ActiveLedIt\Sources\Toolbar.cpp(885): note: No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called
*/
#ifndef qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy
#if defined(_MSC_VER)
// still broken in _MSC_VER_2k22_17Pt0_
// still broken in _MSC_VER_2k22_17Pt1_
// still broken in _MSC_VER_2k22_17Pt2_
// still broken in _MSC_VER_2k22_17Pt3_
// still broken in _MSC_VER_2k22_17Pt4_
// still broken in _MSC_VER_2k22_17Pt5_
// still broken in _MSC_VER_2k22_17Pt6_
// still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy                                                                    \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy 0
#endif
#endif

/*
*  on Windows DEBUG x86 builds only...
==2736==ERROR: AddressSanitizer: container-overflow on address 0x0110ed9d at pc 0x0020f13a bp 0x0110df2c sp 0x0110db0c
READ of size 6 at 0x0110ed9d thread T0
    #0 0x20f155 in __asan_wrap_strnlen D:\a\_work\1\s\src\vctools\asan\llvm\compiler-rt\lib\sanitizer_common\sanitizer_common_interceptors.inc:389
    #1 0x5c5548 in __crt_stdio_output::output_processor<char,__crt_stdio_output::string_output_adapter<char>,__crt_stdio_output::standard_base<char,__crt_stdio_output::string_output_adapter<char> > >::type_case_s_compute_narrow_string_length minkernel\crts\ucrt\inc\corecrt_internal_stdio_output.h:2323
    #2 0x5c4e8e in __crt_stdio_output::output_processor<char,__crt_stdio_output::string_output_adapter<char>,__crt_stdio_output::standard_base<char,__crt_stdio_output::string_output_adapter<char> > >::type_case_s minkernel\crts\ucrt\inc\corecrt_internal_stdio_output.h:2310
    #3 0x5ba7f1 in __crt_stdio_output::output_processor<char,__crt_stdio_output::string_output_adapter<char>,__crt_stdio_output::standard_base<char,__crt_stdio_output::string_output_adapter<char> > >::state_case_type minkernel\crts\ucrt\inc\corecrt_internal_stdio_output.h:2054
    #4 0x5b2ba8 in __crt_stdio_output::output_processor<char,__crt_stdio_output::string_output_adapter<char>,__crt_stdio_output::standard_base<char,__crt_stdio_output::string_output_adapter<char> > >::process minkernel\crts\ucrt\inc\corecrt_internal_stdio_output.h:1699
    #5 0x5a6104 in common_vsprintf<__crt_stdio_output::standard_base,char> minkernel\crts\ucrt\src\appcrt\stdio\output.cpp:167
    #6 0x5c9d75 in __stdio_common_vsprintf minkernel\crts\ucrt\src\appcrt\stdio\output.cpp:239
    #7 0x23b80e in vsnprintf C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt\stdio.h:1439
    #8 0x23b770 in snprintf C:\Program Files (x86)\Windows Kits\10\include\10.0.19041.0\ucrt\stdio.h:1931
    #9 0x2278cc in Stroika::Foundation::Debug::Emitter::DoEmitMessage_<wchar_t> C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:354
    #10 0x221f6e in Stroika::Foundation::Debug::Emitter::EmitTraceMessage C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:259
    #11 0x224555 in Stroika::Foundation::Debug::Private_::EmitFirstTime C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:159
    #12 0x1bf670 in `Stroika::Foundation::Debug::Emitter::Get'::`2'::<lambda_1>::operator() C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.inl:34
    #13 0x1b607a in std::invoke<`Stroika::Foundation::Debug::Emitter::Get'::`2'::<lambda_1> > C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.33.31629\include\type_traits:1548
    #14 0x1b4f65 in std::call_once<`Stroika::Foundation::Debug::Emitter::Get'::`2'::<lambda_1> > C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.33.31629\include\mutex:558
    #15 0x1c1012 in Stroika::Foundation::Debug::Emitter::Get C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.inl:33
    #16 0x223534 in Stroika::Foundation::Debug::TraceContextBumper::TraceContextBumper C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:526
    #17 0x223328 in Stroika::Foundation::Debug::TraceContextBumper::TraceContextBumper C:\Stroika\Library\Sources\Stroika\Foundation\Debug\Trace.cpp:559
    #18 0x372092 in Stroika::Foundation::Execution::SignalHandlerRegistry::SignalHandlerRegistry C:\Stroika\Library\Sources\Stroika\Foundation\Execution\SignalHandlers.cpp:317
    #19 0x371f21 in Stroika::Foundation::Execution::SignalHandlerRegistry::Get C:\Stroika\Library\Sources\Stroika\Foundation\Execution\SignalHandlers.cpp:306
    #20 0x1cba70 in Stroika::TestHarness::Setup C:\Stroika\Tests\TestHarness\TestHarness.cpp:69
    #21 0x1ad737 in main C:\Stroika\Tests\04\Test.cpp:251
    #22 0x587b22 in invoke_main D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:78
    #23 0x587a26 in __scrt_common_main_seh D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:288
    #24 0x5878cc in __scrt_common_main D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:330
    #25 0x587b87 in mainCRTStartup D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_main.cpp:16
    #26 0x76f06708 in BaseThreadInitThunk+0x18 (C:\Windows\System32\KERNEL32.DLL+0x6b816708)
    #27 0x77567cfc in RtlGetFullPathName_UEx+0xac (C:\Windows\SYSTEM32\ntdll.dll+0x4b2e7cfc)
    #28 0x77567cca in RtlGetFullPathName_UEx+0x7a (C:\Windows\SYSTEM32\ntdll.dll+0x4b2e7cca)
*/
#ifndef qCompilerAndStdLib_Debug32Codegen_make_pair_string_Buggy

#if defined(_MSC_VER)
// first/only found broken in _MSC_VER_2k22_17Pt3_
#define qCompilerAndStdLib_Debug32Codegen_make_pair_string_Buggy                                                                           \
    (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((_MSC_VER == _MSC_VER_2k22_17Pt3_)) && qDebug && defined (_M_IX86))
#else
#define qCompilerAndStdLib_Debug32Codegen_make_pair_string_Buggy 0
#endif

#endif

/*
 * https://developercommunity.visualstudio.com/t/Fatal-Corruption-in-X86-ASAN-regression/10130063?port=1025&fsid=7a8d8e50-f549-4b33-a16d-c10fbf32b8fc&entry=problem
 */
#ifndef qCompilerAndStdLib_Debug32_asan_Poison_Buggy
#if defined(_MSC_VER)
// first/only found broken in _MSC_VER_2k22_17Pt3_
#define qCompilerAndStdLib_Debug32_asan_Poison_Buggy                                                                                       \
    (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((_MSC_VER == _MSC_VER_2k22_17Pt3_)) && qDebug && defined (_M_IX86))
#else
#define qCompilerAndStdLib_Debug32_asan_Poison_Buggy 0
#endif
#endif

/*
InternetMediaType.cpp:180:68: error: class template argument deduction failed:
  180 |         return Mapping<String, String>::SequentialThreeWayComparer{}(sortedMapping (lhs.fParameters_), sortedMapping (rhs.fParameters_));
      |                                                                    ^
InternetMediaType.cpp:180:68: error: no matching function for call to 'SequentialThreeWayComparer()'
In file included from ../Characters/../Containers/../Traversal/Iterable.h:1283,
                 from ../Characters/../Containers/Sequence.h:15,
                 from ../Characters/RegularExpression.h:11,
                 from InternetMediaType.cpp:6:
../Characters/../Containers/../Traversal/Iterable.inl:921:15: note: candidate: 'template<class T_THREEWAY_COMPARER> SequentialThreeWayComparer(const T_THREEWAY_COMPARER&)-> Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::Characters::String> >::SequentialThreeWayComparer<T_THREEWAY_COMPARER>'
  921 |     constexpr Iterable<T>::SequentialThreeWayComparer<T_THREEWAY_COMPARER>::SequentialThreeWayComparer (const T_THREEWAY_COMPARER& elementComparer)
      |               ^~~~~~~~~~~
../Characters/../Containers/../Traversal/Iterable.inl:921:15: note:   template argument deduction/substitution failed:
InternetMediaType.cpp:180:68: note:   couldn't deduce template parameter 'T_THREEWAY_COMPARER'
  180 |         return Mapping<String, String>::SequentialThreeWayComparer{}(sortedMapping (lhs.fParameters_), sortedMapping (rhs.fParameters_));
 */
#ifndef qCompilerAndStdLib_template_DefaultArgIgnoredWhenFailedDeduction_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// VERIFIED BROKEN IN GCC 10.0
// VERIFIED BROKEN IN GCC 10.2
// VERIFIED BROKEN IN GCC 11
// VERIFIED BROKEN IN GCC 12
// VERIFIED BROKEN IN GCC 13
#define qCompilerAndStdLib_template_DefaultArgIgnoredWhenFailedDeduction_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 13)
#else
#define qCompilerAndStdLib_template_DefaultArgIgnoredWhenFailedDeduction_Buggy 0
#endif

#endif

/*
../Characters/StringBuilder.inl:114:27: error: no declaration matches ‘Stroika::Foundation::Characters::StringBuilder& Stroika::Foundation::Characters::StringBuilder::operator+=(APPEND_ARG_T&&) requires requires(Stroika::Foundation::Characters::StringBuilder& s, APPEND_ARG_T&& a) {s->Stroika::Foundation::Characters::StringBuilder::Append()((forward<APPEND_ARG_T>)(a));}’
  114 |     inline StringBuilder& StringBuilder::operator+= (APPEND_ARG_T&& a)
      |                           ^~~~~~~~~~~~~
In file included from CodePage.cpp:13:
../Characters/StringBuilder.h:99:35: note: candidate is: ‘template<class APPEND_ARG_T> Stroika::Foundation::Characters::StringBuilder& Stroika::Foundation::Characters::StringBuilder::operator+=(APPEND_ARG_T&&) requires requires(Stroika::Foundation::Characters::StringBuilder& s, APPEND_ARG_T&& a) {s->Stroika::Foundation::Characters::StringBuilder::Append()((forward<APPEND_ARG_T>)(a));}’
   99 |         nonvirtual StringBuilder& operator+= (APPEND_ARG_T&& a)
      |                                   ^~~~~~~~
../Characters/StringBuilder.h:47:11: note: ‘class Stroika::Foundation::Characters::StringBuilder’ defined here
   47 |     class StringBuilder {
      |           ^~~~~~~~~~~~~
In file included from ../Characters/StringBuilder.h:273,
                 from CodePage.cpp:13:
../Characters/StringBuilder.inl:122:27: error: no declaration matches ‘Stroika::Foundation::Characters::StringBuilder& Stroika::Foundation::Characters::StringBuilder::operator<<(APPEND_ARG_T&&) requires requires(Stroika::Foundation::Characters::StringBuilder& s, APPEND_ARG_T&& a) {s->Stroika::Foundation::Characters::StringBuilder::Append()((forward<APPEND_ARG_T>)(a));}’
  122 |     inline StringBuilder& StringBuilder::operator<< (APPEND_ARG_T&& a)
   
   */
#ifndef qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy

#if defined(_MSC_VER)
// NOTE - not always buggy on MSFT - depends on chosen syntax, but the one I think most likely right doesnt work on vis studio so blame them
// still buggy in _MSC_VER_2k22_17Pt4_
// Fixed in _MSC_VER_2k22_17Pt6_
#define qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy                                                     \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt5_)
#elif defined(__GNUC__) && !defined(__clang__)
// VERIFIED BROKEN IN GCC 12
// VERIFIED BROKEN IN GCC 13
#define qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy                                                     \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 13)
#elif defined(__clang__) && defined(__APPLE__)
// Noticed broken in crapple-clang++14
// appears fixed in xcode 15
#define qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy                                                     \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 14))
#else
#define qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy 0
#endif

#endif

/**
 */
#ifndef qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine2_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Noticed broken in crapple-clang++14
// appears fixed in xcode 15
#define qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine2_Buggy                                                    \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 14))
#elif defined(__clang__) && !defined(__APPLE__)
// Noticed broken in -clang++14
//  broken in -clang++15
// Fixed in -clang++16
#define qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine2_Buggy                                                    \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine2_Buggy 0
#endif

#endif

/**
 *      Compiling Library/Sources/Stroika/Foundation/Execution/SpinLock.cpp ... 
In file included from SignalHandlers.cpp:27:
In file included from ./SignalHandlers.h:15:
./Function.h:99:23: error: substitution into constraint expression resulted in a non-constant expression
            requires (is_convertible_v<remove_cvref_t<CTOR_FUNC_SIG>, function<FUNCTION_SIGNATURE>> and
                      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/__type_traits/is_convertible.h:30:66: note: while checking constraint satisfaction for template 'Function<void (int)>' required here
    : public integral_constant<bool, __is_convertible_to(_T1, _T2)> {};
                                                                 ^
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/__type_traits/is_convertible.h:30:66: note: while substituting deduced template arguments into function template 'Function' [with CTOR_FUNC_SIG = Stroika::Foundation::Execution::SignalHandler]
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/__type_traits/is_convertible.h:103:42: note: in instantiation of template class 'std::is_convertible<Stroika::Foundation::Execution::SignalHandler, std::function<void (int)>>' re

 */
#ifndef qCompilerAndStdLib_template_Requires_constraint_not_treated_constexpr_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Noticed broken in crapple-clang++14
// reproduced in xcode 15
#define qCompilerAndStdLib_template_Requires_constraint_not_treated_constexpr_Buggy                                                        \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// Noticed broken in -clang++14
// noticed broken in clang++15 with LIBC++

#define qCompilerAndStdLib_template_Requires_constraint_not_treated_constexpr_Buggy                                                        \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15 || (_LIBCPP_VERSION < 170000)))
#else
#define qCompilerAndStdLib_template_Requires_constraint_not_treated_constexpr_Buggy 0
#endif

#endif

/**
* 
* https://github.com/llvm/llvm-project/issues/62785
* 
* 
*  Compiling Library/Sources/Stroika/Foundation/DataExchange/VariantValue.cpp ... 
PLEASE submit a bug report to https://github.com/llvm/llvm-project/issues/ and include the crash backtrace, preprocessed source, and associated run script.
Stack dump:
0.      Program arguments: clang++-14 --std=c++2b -I/Sandbox/Stroika-Dev/Builds/clang++-14-debug-libc++-c++2b/ThirdPartyComponents/include/ -I/Sandbox/Stroika-Dev/Library/Sources/ -I/Sandbox/Stroika-Dev/IntermediateFiles/clang++-14-debug-libc++-c++2b/ -Wall -Wno-switch -Wno-sign-compare -Wno-unused-function -Wno-unused-local-typedef -Wno-future-compat -Wno-unknown-attributes -fvisibility=hidden -g -D_GLIBCXX_DEBUG -DqDebug=1 -DqHasFeature_LibCurl=1 -DqHasFeature_OpenSSL=1 -DqHasFeature_WinHTTP=0 -DqHasFeature_Xerces=1 -DqHasFeature_ZLib=1 -DqHasFeature_sqlite=1 -DqHasFeature_LZMA=1 -DqHasFeature_boost=1 -DqTraceToFile=1 -DqDefaultTracingOn=1 -fsanitize=address,undefined -stdlib=libc++ -c VariantValue.cpp -o /Sandbox/Stroika-Dev/IntermediateFiles/clang++-14-debug-libc++-c++2b/Library/Foundation/DataExchange/VariantValue.o
1.      <eof> parser at end of file
2.      ./../Characters/../Containers/../Traversal/../Containers/Sequence.h:345:25: instantiating function definition 'Stroika::Foundation::Containers::Sequence<Stroika::Foundation::DataExchange::VariantValue>::operator=='
3.      ./../Characters/../Containers/../Memory/../Execution/../Time/../Traversal/Iterable.h:1471:19: instantiating function definition 'Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::DataExchange::VariantValue>::SequentialEqualsComparer<>::SequentialEqualsComparer'
 #0 0x00007ff57742ad01 llvm::sys::PrintStackTrace(llvm::raw_ostream&, int) (/lib/x86_64-linux-gnu/libLLVM-14.so.1+0xe3fd01)
 #1 0x00007ff577428a3e llvm::sys::RunSignalHandlers() (/lib/x86_64-linux-gnu/libLLVM-14.so.1+0xe3da3e)
 #2 0x00007ff57742a0ab llvm::sys::CleanupOnSignal(unsigned long) (/lib/x86_64-linux-gnu/libLLVM-14.so.1+0xe3f0ab)
 #3 0x00007ff577356dff (/lib/x86_64-linux-gnu/libLLVM-14.so.1+0xd6bdff)
 #4 0x00007ff5760d2520 (/lib/x86_64-linux-gnu/libc.so.6+0x42520)
 #5 0x00007ff57dbcfba1 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0xd12ba1)
 #6 0x00007ff57dbcb9c8 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0xd0e9c8)
 #7 0x00007ff57dba4bed (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0xce7bed)
 #8 0x00007ff57dba0af4 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0xce3af4)
 #9 0x00007ff57dba07a4 clang::Expr::EvaluateAsConstantExpr(clang::Expr::EvalResult&, clang::ASTContext const&, clang::Expr::ConstantExprKind) const (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0xce37a4)
#10 0x00007ff57e108a8a clang::Sema::CheckConstraintSatisfaction(clang::NamedDecl const*, llvm::ArrayRef<clang::Expr const*>, llvm::ArrayRef<clang::TemplateArgument>, clang::SourceRange, clang::ConstraintSatisfaction&) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x124ba8a)
#11 0x00007ff57e1096b5 clang::Sema::EnsureTemplateArgumentListConstraints(clang::TemplateDecl*, llvm::ArrayRef<clang::TemplateArgument>, clang::SourceRange) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x124c6b5)
#12 0x00007ff57e585b77 clang::Sema::CheckTemplateArgumentList(clang::TemplateDecl*, clang::SourceLocation, clang::TemplateArgumentListInfo&, bool, llvm::SmallVectorImpl<clang::TemplateArgument>&, bool, bool*) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x16c8b77)
#13 0x00007ff57e584174 clang::Sema::CheckTemplateIdType(clang::TemplateName, clang::SourceLocation, clang::TemplateArgumentListInfo&) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x16c7174)
#14 0x00007ff57e67b5c9 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17be5c9)
#15 0x00007ff57e67a793 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17bd793)
#16 0x00007ff57e66ca63 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17afa63)
#17 0x00007ff57e66c7de clang::Sema::SubstNestedNameSpecifierLoc(clang::NestedNameSpecifierLoc, clang::MultiLevelTemplateArgumentList const&) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17af7de)
#18 0x00007ff57e6ac3eb (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17ef3eb)
#19 0x00007ff57e6abac0 clang::Sema::InstantiateFunctionDefinition(clang::SourceLocation, clang::FunctionDecl*, bool, bool, bool) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17eeac0)
#20 0x00007ff57e31cb38 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x145fb38)
#21 0x00007ff57dfd8bbc clang::Sema::runWithSufficientStackSpace(clang::SourceLocation, llvm::function_ref<void ()>) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x111bbbc)
#22 0x00007ff57e2a0cbb clang::Sema::MarkFunctionReferenced(clang::SourceLocation, clang::FunctionDecl*, bool) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x13e3cbb)
#23 0x00007ff57e3f7a04 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x153aa04)
#24 0x00007ff57e3f3b24 clang::InitializationSequence::Perform(clang::Sema&, clang::InitializedEntity const&, clang::InitializationKind const&, llvm::MutableArrayRef<clang::Expr*>, clang::QualType*) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x1536b24)
#25 0x00007ff57e34708c clang::Sema::BuildCXXTypeConstructExpr(clang::TypeSourceInfo*, clang::SourceLocation, llvm::MutableArrayRef<clang::Expr*>, clang::SourceLocation, bool) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x148a08c)
#26 0x00007ff57e6721d5 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17b51d5)
#27 0x00007ff57e6723ef (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17b53ef)
#28 0x00007ff57e66bd57 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17aed57)
#29 0x00007ff57e68e4b5 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17d14b5)
#30 0x00007ff57e68253f (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17c553f)
#31 0x00007ff57e66a92c clang::Sema::SubstStmt(clang::Stmt*, clang::MultiLevelTemplateArgumentList const&) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17ad92c)
#32 0x00007ff57e6abca8 clang::Sema::InstantiateFunctionDefinition(clang::SourceLocation, clang::FunctionDecl*, bool, bool, bool) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17eeca8)
#33 0x00007ff57e6ae0d9 clang::Sema::PerformPendingInstantiations(bool) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x17f10d9)
#34 0x00007ff57dfda60b clang::Sema::ActOnEndOfTranslationUnitFragment(clang::Sema::TUFragmentKind) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x111d60b)
#35 0x00007ff57dfdac71 clang::Sema::ActOnEndOfTranslationUnit() (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x111dc71)
#36 0x00007ff57d97ec96 clang::Parser::ParseTopLevelDecl(clang::OpaquePtr<clang::DeclGroupRef>&, bool) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0xac1c96)
#37 0x00007ff57d8c18ed clang::ParseAST(clang::Sema&, bool, bool) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0xa048ed)
#38 0x00007ff57ea35b71 clang::CodeGenAction::ExecuteAction() (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x1b78b71)
#39 0x00007ff57f3d1b57 clang::FrontendAction::Execute() (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x2514b57)
#40 0x00007ff57f3293a6 clang::CompilerInstance::ExecuteAction(clang::FrontendAction&) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x246c3a6)
#41 0x00007ff57f44b45b clang::ExecuteCompilerInvocation(clang::CompilerInstance*) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x258e45b)
#42 0x000000000041328b cc1_main(llvm::ArrayRef<char const*>, char const*, void*) (/usr/lib/llvm-14/bin/clang+0x41328b)
#43 0x00000000004114bc (/usr/lib/llvm-14/bin/clang+0x4114bc)
#44 0x00007ff57efa7ed2 (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x20eaed2)
#45 0x00007ff577356b6d llvm::CrashRecoveryContext::RunSafely(llvm::function_ref<void ()>) (/lib/x86_64-linux-gnu/libLLVM-14.so.1+0xd6bb6d)
#46 0x00007ff57efa79c0 clang::driver::CC1Command::Execute(llvm::ArrayRef<llvm::Optional<llvm::StringRef> >, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >*, bool*) const (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x20ea9c0)
#47 0x00007ff57ef72183 clang::driver::Compilation::ExecuteCommand(clang::driver::Command const&, clang::driver::Command const*&) const (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x20b5183)
#48 0x00007ff57ef7240a clang::driver::Compilation::ExecuteJobs(clang::driver::JobList const&, llvm::SmallVectorImpl<std::pair<int, clang::driver::Command const*> >&) const (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x20b540a)
#49 0x00007ff57ef8c507 clang::driver::Driver::ExecuteCompilation(clang::driver::Compilation&, llvm::SmallVectorImpl<std::pair<int, clang::driver::Command const*> >&) (/lib/x86_64-linux-gnu/libclang-cpp.so.14+0x20cf507)
#50 0x0000000000410f26 main (/usr/lib/llvm-14/bin/clang+0x410f26)
#51 0x00007ff5760b9d90 __libc_start_call_main ./csu/../sysdeps/nptl/libc_start_call_main.h:58:16
#52 0x00007ff5760b9e40 call_init ./csu/../csu/libc-start.c:128:20
#53 0x00007ff5760b9e40 __libc_start_main ./csu/../csu/libc-start.c:379:5
#54 0x000000000040e3b5 _start (/usr/lib/llvm-14/bin/clang+0x40e3b5)
clang: error: clang frontend command failed with exit code 139 (use -v to see invocation)
Ubuntu clang version 14.0.0-1ubuntu1
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin
clang: note: diagnostic msg: 
********************

PLEASE ATTACH THE FOLLOWING FILES TO THE BUG REPORT:
Preprocessed source(s) and associated run script(s) are located at:
clang: note: diagnostic msg: /tmp/VariantValue-587e15.cpp
clang: note: diagnostic msg: /tmp/VariantValue-587e15.sh
clang: note: diagnostic msg: 

********************
make[4]: *** [/Sandbox/Stroika-Dev/ScriptsLib/SharedBuildRules-Default.mk:30: /Sandbox/Stroika-Dev/IntermediateFiles/clang++-14-debug-libc++-c++2b/Library/Foundation/DataExchange/VariantValue.o] Error 139
make[4]: *** Waiting for unfinished jobs....
      Compiling Library/Sources/Stroika/Foundation/Execution/SignalHandlers.cpp ... 
^Cmake[4]: *** [/Sandbox/Stroika-Dev/ScriptsLib/SharedBuildRules-Default.mk:30: /Sandbox/Stroika-Dev/IntermediateFiles/clang++-14-debug-libc++-c++2b/Library/Foundation/Execution/SignalHandlers.o] Interrupt
make[3]: *** [Makefile:100: Execution] Interrupt
make[4]: *** [/Sandbox/Stroika-Dev/ScriptsLib/SharedBuildRules-Default.mk:30: /Sandbox/Stroika-Dev/IntermediateFiles/clang++-14-debug-libc++-c++2b/Library/Foundation/DataExchange/ObjectVariantMapper.o] Interrupt
make[3]: *** [Makefile:100: DataExchange] Interrupt
make[2]: *** [Makefile:107: all_objs_] Interrupt
make[1]: *** [Makefile:20: 
 */
#ifndef qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Noticed broken in crapple-clang++14
// appears fixed in xcode 15
#define qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy                                                      \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 14))
#elif defined(__clang__) && !defined(__APPLE__)
// Noticed broken in -clang++14
// Noticed broken in -clang++15
// fixed in clang++16
#define qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy                                                      \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy 0
#endif

#endif

/*
* 
* https://stackoverflow.com/questions/53408962/try-to-understand-compiler-error-message-default-member-initializer-required-be
* 
* 
*  file included from UTFConvert.cpp:8:
./UTFConvert.h:86:66: error: default member initializer for 'fStrictMode' needed within definition of enclosing class 'UTFConvert' outside of member functions
        constexpr UTFConvert (const Options& options = Options{});
                                                                 ^
./UTFConvert.h:51:18: note: default member initializer declared here
            bool fStrictMode{false};
      OR
* 
In file included from Writer.cpp:8:
Writer.h:61:50: error: default member initializer for ‘Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines::Writer::Options::fSeparator’ required before the end of its enclosing class
   61 |         Writer (const Options& options = Options{});
      |                                                  ^
Writer.h:54:45: note: defined here
   54 |             Characters::Character fSeparator{','};
      |                                             ^~~~~~
Writer.h:61:50: error: default member initializer for ‘Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines::Writer::Options::fSpaceSeparate’ required before the end of its enclosing class
   61 |         Writer (const Options& options = Options{});
      |                                                  ^
Writer.h:55:49: note: defined here
   55 |             bool                  fSpaceSeparate{false}; // if true, emit space after all separators on a line but the last



   Compiling Library/Sources/Stroika/Foundation/Execution/WaitForIOReady.cpp ... 
In file included from Writer.cpp:8:
./Writer.h:68:43: error: default member initializer for 'fSeparator' needed within definition of enclosing class 'Writer' outside of member functions
        Writer (const Options& options = {});
                                          ^
./Writer.h:54:35: note: default member initializer declared here
            Characters::Character fSeparator{','};
                                  ^
1 error generated.
make[6]: *** [/Sandbox/Stroika-Dev/ScriptsLib/SharedB
      |                                                 ^~~~~~~~
         Co*/
#ifndef qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// First noticed in g++-11
// still broken in g++-12
// still broken in g++-13
#define qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 13)
#elif defined(__clang__) && defined(__APPLE__)
// First noticed in clang++-14
// Reproduced in clang++-15
#define qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// First noticed in clang++-14
// broken in clang++-15
// broken in clang++-16
#define qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 16))
#else
#define qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy 0
#endif

#endif

/*
esponse.h: In member function ‘auto Stroika::Frameworks::WebServer::Response::UpdateHeader(FUNCTION&&)’:
Response.h:373:30: error: no match for ‘operator==’ (operand types are ‘unsigned char:3’ and ‘Stroika::Frameworks::WebServer::Response::State’)
  373 |             Require (fState_ == State::ePreparingHeaders);
      |                      ~~~~~~~ ^~ ~~~~~~~~~~~~~~~~~~~~~~~~
      |                      |                 |
      |                      unsigned char:3   Stroika::Frameworks::WebServer::Response::State
../../Foundation/Characters/../Configuration/../Debug/Assertions.h:215:24: note: in definition of macro ‘
 */
#ifndef qCompilerAndStdLib_enum_with_bitLength_opequals_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// FIRST SEEEN BROKEN IN GCC 11
#define qCompilerAndStdLib_enum_with_bitLength_opequals_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ == 11)
#else
#define qCompilerAndStdLib_enum_with_bitLength_opequals_Buggy 0
#endif
#endif

/*
      Compiling Library/Sources/Stroika/Foundation/Characters/Character.cpp ...
In file included from ../Characters/Format.h:15,
                 from Statistics.cpp:6:
../Characters/String.h:218:20: error: explicit specialization in non-namespace scope ‘class Stroika::Foundation::Characters::String’
  218 |          template <>
      |                    ^
.
*/
#ifndef qCompilerAndStdLib_templateConstructorSpecialization_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// FIRST SEEEN BROKEN IN GCC 12
// BROKEN IN GCC 13
#define qCompilerAndStdLib_templateConstructorSpecialization_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 13)
#else
#define qCompilerAndStdLib_templateConstructorSpecialization_Buggy 0
#endif
#endif

/*
5>C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_Iterable.h(60): error C2059: syntax error: 'template'
25>C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_Iterable.h(76): note: see reference to function template instantiation 'void CommonTests::IterableTests::Test5_SequenceEquals_<USING_ITERABLE_CONTAINER,EQUALS_COMPARER>(const Stroika::Foundation::Traversal::Iterable<T> &,EQUALS_COMPARER)' being compiled
25>        with
25>        [
25*/
#ifndef qCompilerAndStdLib_template_template_call_SequentialEquals_Buggy

#if defined(_MSC_VER)
// still broken in _MSC_VER_2k22_17Pt0_
// still broken in _MSC_VER_2k22_17Pt1_
// still broken in _MSC_VER_2k22_17Pt2_
// still broken in _MSC_VER_2k22_17Pt3_
// still broken in _MSC_VER_2k22_17Pt4_
// still broken in _MSC_VER_2k22_17Pt5_
// still broken in _MSC_VER_2k22_17Pt6_
// still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_template_template_call_SequentialEquals_Buggy                                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_template_template_call_SequentialEquals_Buggy 0
#endif

#endif

/*
 *      ACCORDING To https://en.cppreference.com/w/cpp/locale/time_get/get
 * 
 *              m   parses the month as a decimal number (range [01,12]), leading zeroes permitted but not required
 * 
 *      CODE to reprodcue bug online:
 *              https://onlinegdb.com/Sy1IZ0-md
 * 
 *      GCC bug report:
 *              https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99439
 *              (known bug but they said DUP of other report - still not fixed -LGP 2021-05-12)
 *              https://gcc.gnu.org/bugzilla/show_bug.cgi?id=45896 (about 10 years old so not sure gonna be fixed anytime soon)
 * 
 *      but libstdc++ doesn't appear to support the leading zero.
 */
#ifndef qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy
#if defined(_GLIBCXX_RELEASE)
#define qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy (_GLIBCXX_RELEASE <= 11)
#else
#define qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy 0
#endif
#endif

/*
 * This could be a docker or ubuntu issue, but appears to be an issue with glibc or libstdc++
 * 
From:    https://en.cppreference.com/w/cpp/locale/time_get/date_order
    mdy month, day, year (American locales)

    std::locale                  l{"en_US.utf8"}; // originally tested with locale {} - which defaulted to C-locale
    const time_get<wchar_t>&     tmget = use_facet<time_get<wchar_t>> (l);
    VerifyTestResultWarning (tmget.date_order () == time_base::mdy);    // fails – returns no_order

    REPORTED TO GCC folks as:
         https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99556
         They responded this IS dup of https://gcc.gnu.org/bugzilla/show_bug.cgi?id=9635
         which is almost 20 years old, so dont hold breath on fix ;-) --LGP 2021-05-12

    NOTE:
        Since it appears the libg++ people have no intention of fixing this (anytime soon), instead of defaulting
        to wrong answer (with if defined(__GLIBCXX__) && __GLIBCXX__ <= 20220527) I'm just changing to 
        if defined(__GLIBCXX__)
        There is a check in the regtests (test_locale_time_get_date_order_no_order_Buggy) for when this is fixed, and it will warn if it ever is.
 */
#ifndef qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy
#if defined(_GLIBCXX_RELEASE)
#define qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy 1
#else
#define qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy 0
#endif
#endif

#ifndef qCompiler_vswprintf_on_elispisStr_Buggy

#if defined(__clang__) && defined(__APPLE__)
// first noticed in XCODE 13
// HANGS on XCode 14 (at least debug builds on my m1 machine- may have todo with codepage/installed locales?)
// Broken on XCode 15 too
#define qCompiler_vswprintf_on_elispisStr_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompiler_vswprintf_on_elispisStr_Buggy 0
#endif

#endif

// libstd c++ clang versions (around 14) have badly fucked this up.
// they leave __cpp_lib_three_way_comparison undefined, but still provide (in some versions - like V14) a partly broken
// version available to introduce compiler ambiguiity errors when used
//
//  NOTE: Generally the issue is for any STL types, like shared_ptr or optional - you must do something like this
//
//#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
//        return Common::compare_three_way_BWA{}(fRep_, rhs.fRep_);
//#else
//        return fRep_ <=> rhs.fRep_;
//#endif
//
#ifndef qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
#if defined(_LIBCPP_VERSION)
#if _LIBCPP_VERSION <= 14000
#if defined(__APPLE__)
#define qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy 1
#else
// for clang++-14 stdlib=libc+++, on ununtu 22.04, we have __cpp_lib_three_way_comparison undefined and yet the class DOES exist - just in
// a buggy form - so cannot test __cpp_lib_three_way_comparison to decide if we define it
#define qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy (_LIBCPP_VERSION < 13000)
#endif
#else

#define qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy 0
#endif
#else
#define qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy 0
#endif
#endif

/**
 *      This is going to limit how much I can support ranges in Stroika v3.
 * 
./../Characters/Character.inl:168:24: error: no matching function for call to 'all_of'
        return ranges::all_of (fromS, charComparer);
                       ^~~~~~
./../Characters/Character.inl:250:20: note: in instantiation of function template specialization 'Stroika::Foundation::Characters::Character::IsASCII<Stroika::Foundation::Characters::Latin1>' requested here
            return IsASCII (s) ? eASCII : eLatin1;
                   ^
./../Characters/String.inl:167:28: note: in instantiation of function template specialization 'Stroika::Foundation::Characters::Character::IsASCIIOrLatin1<Stroika::Foundation::Characters::Latin1>' requested here
        switch (Character::IsASCIIOrLatin1 (s)) {
                           ^
./../Characters/String.inl:345:20: note: in instantiation of function template specialization 'Stroika::Foundation::Characters::String::mk_<Stroika::Foundation::Characters::Latin1>' requested here
            return mk_ (span<const Latin1>{reinterpret_cast<const Latin1*> (s.data ()), s.size ()});
                   ^
 */
#ifndef qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy
#if defined(_LIBCPP_VERSION)
// unclear yet if broken in _LIBCPP_VERSION but only docs suggest broken til 15
#if defined(__APPLE__)
#define qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy 1
#else
// seems still broken in _LIBCPP_VERSION 15007
#define qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy (_LIBCPP_VERSION < 160000)
#endif
#elif defined(__clang_major__)
#define qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy 0
#endif
#endif

#ifndef qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy
#if defined(_LIBCPP_VERSION)
#if _LIBCPP_VERSION < 170000
#define qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy !qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
#else
#if defined(__APPLE__)
#define qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy 1
#else
#define qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy 0
#endif
#endif
#else
#define qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy 0
#endif
#endif

#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy and qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
#error "These cannot be both defined"
#endif

/*
 * Crazy man!  - https://connect.microsoft.com/VisualStudio/feedback/details/763051/a-value-of-predefined-macro-cplusplus-is-still-199711l
 *
 *       Stroika requires at least C++ ISO/IEC 14882:2011 supported by the compiler (informally known as C++ 11)
 *
 *      \note - they DO define _MSVC_LANG  to be the same value as cplusplus should be defined to! 
 *
 *      IF FAILS:       
 *              Stroika requires at least C++ ISO/IEC 14882:2011 supported by the compiler (informally known as C++ 11)
 * 
 * 
 * MESSAGE:  Stroika v3 requires at least C++ ISO/IEC 14882:2020(E) supported by the compiler (informally known as C++ 20)
 */
#ifndef qCompilerAndStdLib_cplusplus_macro_value_Buggy

#if defined(_MSC_VER)

// verified still broken in _MSC_VER_2k22_17Pt0_
// verified still broken in _MSC_VER_2k22_17Pt1_
// verified still broken in _MSC_VER_2k22_17Pt2_
// verified still broken in _MSC_VER_2k22_17Pt3_
// verified still broken in _MSC_VER_2k22_17Pt4_
// verified still broken in _MSC_VER_2k22_17Pt5_
// verified still broken in _MSC_VER_2k22_17Pt6_
// verified still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy 0
#endif

#endif

/*
In file included from Namespace.cpp:10:
./Namespace.h:36:14: warning: explicitly defaulted three-way comparison operator is implicitly deleted [-Wdefaulted-function-deleted]
        auto operator<=> (const NamespaceDefinition& rhs) const = default;
             ^
./Namespace.h:29:17: note: defaulted 'operator<=>' is implicitly deleted because there is no viable three-way comparison function for member 'fURI'
        wstring fURI;    // required non-null
                ^
*/
#ifndef qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy

#if defined(_LIBCPP_VERSION)
// Reproduced using clang++15, libcpp 15007, and Ubunutu 22.04
#define qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy (_LIBCPP_VERSION <= 15007)
#else
#define qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy

#if defined(__clang__) && defined(__APPLE__)
// appears fixed in xcode 15
#define qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy                                                            \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 14))
#elif defined(__clang__) && !defined(__APPLE__)
// seems fixed in clang++16
#define qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy                                                            \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__GNUC__) && !defined(__clang__)
// VERIFIED BROKEN IN GCC 12.3 - so wondering if this is not a compiler bug but a me bug --LGP 2023-08-08
// FIXED in GCC 13
#define qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 12)
#elif defined(_MSC_VER)
// verified broken in _MSC_VER_2k22_17Pt5_ (BUT ONLY WHEN BUILDING INSIDE DOCKER????)
// Fixed in _MSC_VER_2k22_17Pt6_
#define qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy                                                            \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt5_)
#else
#define qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy 0
#endif

#endif

/*
 ERROR ON GCC VERY WEIRD g++-11-debug-c++2a

 ./Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::Character>’:
../Characters/String.h:302:38:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::Character>’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::Character>::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
  261 |     class Iterable : public IterableBase, protected Debug::AssertExternallySynchronizedMutex {
      |           ^~~~~~~~
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Activity<Stroika::Foundation::Characters::String> >’:
../Execution/../Containers/Stack.h:80:11:   required from ‘class Stroika::Foundation::Containers::Stack<Stroika::Foundation::Execution::Activity<Stroika::Foundation::Characters::String> >’
../Execution/Exceptions.h:113:39:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Activity<Stroika::Foundation::Characters::String> >’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Activity<Stroika::Foundation::Characters::String> >::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::String>’:
../IO/FileSystem/../../Time/Date.h:537:52:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::String>’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::String>::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<std::thread::id>’:
../IO/FileSystem/../../Execution/Thread.h:1029:41:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<std::thread::id>’ has a field ‘Stroika::Foundation::Traversal::Iterable<std::thread::id>::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Thread::Ptr>’:
../IO/FileSystem/../../Execution/Thread.inl:337:75:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Thread::Ptr>’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Thread::Ptr>::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Configuration::SystemConfiguration::CPU::CoreDetails>’:
../Characters/../Containers/Sequence.h:216:11:   required from ‘class Stroika::Foundation::Containers::Sequence<Stroika::Foundation::Configuration::SystemConfiguration::CPU::CoreDetails>’
SystemConfiguration.h:115:47:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Configuration::SystemConfiguration::CPU::CoreDetails>’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Configuration::SystemConfiguration::CPU::CoreDetails>::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::VariantValue> >’:
../Characters/../Memory/../Containers/Adapters/../Mapping.h:108:11:   required from ‘class Stroika::Foundation::Containers::Mapping<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::VariantValue>’
/usr/include/c++/11/type_traits:1199:4:   required from ‘struct std::__is_trivially_copy_constructible_impl<Stroika::Foundation::DataExchange::VariantValue, true>’
/usr/include/c++/11/type_traits:1204:12:   required from ‘struct std::is_trivially_copy_constructible<Stroika::Foundation::DataExchange::VariantValue>’
/usr/include/c++/11/type_traits:3127:43:   required from ‘constexpr const bool std::is_trivially_copy_constructible_v<Stroika::Foundation::DataExchange::VariantValue>’
/usr/include/c++/11/optional:663:11:   required from ‘class std::optional<Stroika::Foundation::DataExchange::VariantValue>’
/usr/include/c++/11/type_traits:906:30:   [ skipping 5 instantiation contexts, use -ftemplate-backtrace-limit=0 to disable ]
/usr/include/c++/11/type_traits:935:12:   required from ‘struct std::__is_copy_constructible_impl<Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::DataExchange::VariantValue, Stroika::Foundation::Traversal::DefaultIteratorTraits<std::forward_iterator_tag, Stroika::Foundation::DataExchange::VariantValue, long int, Stroika::Foundation::DataExchange::VariantValue*, Stroika::Foundation::DataExchange::VariantValue&> >, true>’
/usr/include/c++/11/type_traits:941:12:   required from ‘struct std::is_copy_constructible<Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::DataExchange::VariantValue, Stroika::Foundation::Traversal::DefaultIteratorTraits<std::forward_iterator_tag, Stroika::Foundation::DataExchange::VariantValue, long int, Stroika::Foundation::DataExchange::VariantValue*, Stroika::Foundation::DataExchange::VariantValue&> > >’
/usr/include/c++/11/type_traits:3107:33:   required from ‘constexpr const bool std::is_copy_constructible_v<Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::DataExchange::VariantValue, Stroika::Foundation::Traversal::DefaultIteratorTraits<std::forward_iterator_tag, Stroika::Foundation::DataExchange::VariantValue, long int, Stroika::Foundation::DataExchange::VariantValue*, Stroika::Foundation::DataExchange::VariantValue&> > >’
../Characters/../Containers/../Traversal/Iterable.h:263:24:   required from ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::DataExchange::VariantValue>’
../Characters/../Containers/Sequence.h:216:11:   required from ‘class Stroika::Foundation::Containers::Sequence<Stroika::Foundation::DataExchange::VariantValue>’
../DataExchange/Variant/INI/../../VariantValue.inl:44:51:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::VariantValue> >’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::VariantValue> >::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::DataExchange::VariantValue>’:
../Characters/../Containers/Sequence.h:216:11:   required from ‘class Stroika::Foundation::Containers::Sequence<Stroika::Foundation::DataExchange::VariantValue>’
../DataExchange/Variant/INI/../../VariantValue.inl:44:51:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::DataExchange::VariantValue>’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::DataExchange::VariantValue>::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::Characters::String> >’:
../Characters/../Memory/../Containers/Adapters/../Mapping.h:108:11:   required from ‘class Stroika::Foundation::Containers::Mapping<Stroika::Foundation::Characters::String, Stroika::Foundation::Characters::String>’
../DataExchange/Variant/INI/Profile.h:30:33:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::Characters::String> >’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::Characters::String> >::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::Variant::INI::Section> >’:
../Characters/../Memory/../Containers/Adapters/../Mapping.h:108:11:   required from ‘class Stroika::Foundation::Containers::Mapping<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::Variant::INI::Section>’
../DataExchange/Variant/INI/Profile.h:38:34:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::Variant::INI::Section> >’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Common::KeyValuePair<Stroika::Foundation::Characters::String, Stroika::Foundation::DataExchange::Variant::INI::Section> >::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Function<void(const Stroika::Foundation::Execution::ProgressMonitor&)> >’:
../Characters/../Containers/Sequence.h:216:11:   required from ‘class Stroika::Foundation::Containers::Sequence<Stroika::Foundation::Execution::Function<void(const Stroika::Foundation::Execution::ProgressMonitor&)> >’
../Execution/ProgressMonitor.inl:34:51:   required from here
../Characters/../Containers/../Traversal/Iterable.h:261:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Function<void(const Stroika::Foundation::Execution::ProgressMonitor&)> >’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::Function<void(const Stroika::Foundation::Execution::ProgressMonitor&)> >::_fRep’ whose type uses the anonymous namespace [-Wsubobject-linkage]
In file included from ../Characters/FloatConversion.h:11,
                 from SystemConfiguration.cpp:24:
/usr/include/c++/11/optional: In instantiation of ‘union std::_Optional_payload_base<Stroika::Foundation::Containers::Set<unsigned int>::_IRep>::_Storage<Stroika::Foundation::Containers::Set<unsigned int>::_IRep, false>’:
/usr/include/c++/11/optional:241:30:   required from ‘struct std::_Optional_payload_base<Stroika::Foundation::Containers::Set<unsigned int>::_IRep>’
/usr/include/c++/11/optional:355:12:   required from ‘struct std::_Optional_payload<Stroika::Foundation::Containers::Set<unsigned int>::_IRep, true, false, false>’
/usr/include/c++/11/optional:388:12:   required from ‘struct std::_Optional_payload<Stroika::Foundation::Containers::Set<unsigned int>::_IRep, false, false, false>’
/usr/include/c++/11/optional:515:30:   required from ‘struct std::_Optional_base<Stroika::Foundation::Containers::Set<unsigned int>::_IRep, false, false>’
/usr/include/c++/11/optional:663:11:   required from ‘class std::optional<Stroika::Foundation::Containers::Set<unsigned int>::_IRep>’
/usr/include/c++/11/type_traits:906:30:   [ skipping 6 instantiation contexts, use -ftemplate-backtrace-limit=0 to disable ]
/usr/include/c++/11/type_traits:941:12:   required from ‘struct std::is_copy_constructible<Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::Containers::Set<unsigned int>::_IRep, Stroika::Foundation::Traversal::DefaultIteratorTraits<std::forward_iterator_tag, Stroika::Foundation::Containers::Set<unsigned int>::_IRep, long int, Stroika::Foundation::Containers::Set<unsigned int>::_IRep*, Stroika::Foundation::Containers::Set<unsigned int>::_IRep&> > >’
/usr/include/c++/11/type_traits:3107:33:   required from ‘constexpr const bool std::is_copy_constructible_v<Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::Containers::Set<unsigned int>::_IRep, Stroika::Foundation::Traversal::DefaultIteratorTraits<std::forward_iterator_tag, Stroika::Foundation::Containers::Set<unsigned int>::_IRep, long int, Stroika::Foundation::Containers::Set<unsigned int>::_IRep*, Stroika::Foundation::Containers::Set<unsigned int>::_IRep&> > >’
../Characters/../Containers/../Traversal/Iterable.h:263:24:   required from ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Containers::Set<unsigned int>::_IRep>’
../Characters/../Containers/../Traversal/Iterable.h:1191:30:   required from ‘class Stroika::Foundation::Traversal::Iterable<unsigned int>::_SafeReadWriteRepAccessor<Stroika::Foundation::Containers::Set<unsigned int>::_IRep>’
../Characters/../Containers/Set.inl:160:48:   required from ‘void Stroika::Foundation::Containers::Set<T>::Add(Stroika::Foundation::Configuration::ArgByValueType<T>) [with T = unsigned int; Stroika::Foundation::Configuration::ArgByValueType<T> = unsigned int; typename std::decay<_Tp>::type = unsigned int]’
SystemConfiguration.cpp:192:23:   required from here
/usr/include/c++/11/optional:238:15: error: cannot declare field ‘std::_Optional_payload_base<Stroika::Foundation::Containers::Set<unsigned int>::_IRep>::_Storage<Stroika::Foundation::Containers::Set<unsigned int>::_IRep, false>::_M_value’ to be of abstract type ‘Stroika::Foundation::Containers::Set<unsigned int>::_IRep’
  238 |           _Up _M_value;
      |               ^~~~~~~~
In file included from ../Characters/String.h:18,
                 from ../Characters/FloatConversion.h:15,
  
  */
#ifndef qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// VERIFIED BROKEN IN GCC 11
// VERIFIED BROKEN IN GCC 12
// FIXED in GCC 13
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 12)
#else
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy 0
#endif

#endif


/**
 /../../Memory/../Streams/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::String>’:
../../../Memory/../Streams/InputStream.inl:269:65:   required from here
../../../Memory/../Streams/../Traversal/Iterable.h:205:11: warning: ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::String>’ has a field ‘Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::String>::_SharedByValueRepType Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::String>::_fRep’ whose type has internal linkage [-Wsubobject-linkage]
      Compiling Library/Sources/Stroika/Foundation/Characters/CodePage.cpp ... 
      Compiling Library/Sources/Stroika/Foundation/Cryptography/OpenSSL/CipherAlgorithm.cpp ... 
      Compiling Library/Sources/Stroika/Foundation/Cryptography/Encoding/Algorithm/RC4.cpp ... 
In file included from ../Execution/../Characters/../Containers/Sequence.h:16,
                 from ../Execution/../Characters/String.h:15,
                 from ../Execution/Exceptions.h:15,
                 from CodePage.cpp:13:
../Execution/../Characters/../Containers/../Traversal/Iterable.h: In instantiation of ‘class Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Characters::Character>’:
../Execution/../Characters/String.h:163:52:   required from here
..
*/
#ifndef qCompilerAndStdLib_lambdas_in_unevaluatedContext_warning_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// Issue (just warning) in GCC 13
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_warning_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 13)
#else
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_warning_Buggy 0
#endif

#endif


/*
      Compiling Library/Sources/Stroika/Foundation/Characters/FloatConversion.cpp ...
FloatConversion.cpp: In function ‘Stroika::Foundation::Characters::String {anonymous}::Float2String_OptimizedForCLocaleAndNoStreamFlags_(FLOAT_TYPE, int, bool)’:
FloatConversion.cpp:117:73: error: ‘chars_format’ has not been declared
  117 |         ptrdiff_t resultStrLen = to_chars (buf.begin (), buf.end (), f, chars_format::general, precision).ptr - buf.begin ();
      |                                                                         ^~~~~~~~~~~~
make[4]: *** [/mnt/c/Sandbox/Stroika/DevRoot/ScriptsLib/SharedBuildRules-Default.mk:22: /mnt/c/Sandbox/Stroika/DevRoot/IntermediateFiles/Debug-unix/Library/Foundation/Characters/FloatConversion.o] Error 1
*/
#ifndef qCompilerAndStdLib_to_chars_FP_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// according to https://en.cppreference.com/w/cpp/compiler_support fixed in gcc11
#define qCompilerAndStdLib_to_chars_FP_Buggy 0
#elif defined(__clang__) && defined(__APPLE__)
// according to https://en.cppreference.com/w/cpp/compiler_support not yet supported so WAG
// still broken in XCode 14
// still broken in XCode 15
#define qCompilerAndStdLib_to_chars_FP_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__) && defined(_LIBCPP_VERSION)
// according to https://en.cppreference.com/w/cpp/compiler_support not yet supported so WAG
// appears still broken in clang++13 (maybe should depend on stdlib version not compiler version)
// appears fixed in clang++14 (or maybe SB depending on libversion)
// _LIBCPP_VERSION <= 14000
// _LIBCPP_VERSION <= 15007
// _LIBCPP_VERSION ==160000 (so say < 170000)
#define qCompilerAndStdLib_to_chars_FP_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION < 170000))
#else
#define qCompilerAndStdLib_to_chars_FP_Buggy 0
#endif

#endif

/*
#include <cassert>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>

using namespace std;

int main ()
{
    using FLOAT_TYPE = float;
    auto f = numeric_limits<FLOAT_TYPE>::min ();
    auto sbPrecision = numeric_limits<FLOAT_TYPE>::digits10 + 1;
    assert (sbPrecision == 7);
    auto test = [] (FLOAT_TYPE f, int precision) {
        char fAsString[1024];
        auto r1 = to_chars (fAsString, fAsString + sizeof(fAsString), f, chars_format::general, precision);
        *r1.ptr = 0;
        if (precision == 7) {
            assert (r1.ptr-fAsString == 12);
            assert (strcmp (fAsString, "1.175494e-38") == 0);
        }
        // now complete roundtrip
        FLOAT_TYPE r2 = 0;
        auto [ptr, ec] = from_chars (fAsString, fAsString + sizeof(fAsString), r2);
        auto f3 = strtof (fAsString, nullptr);
        cerr << "precision " << precision << ", strtof =" << f3 << " and r2=" << r2 << "\n";
        assert (r2 == f);
    };
    test (f, sbPrecision+1);
    test (f, sbPrecision);
    return 0;
}*/
#ifndef qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy

#if defined(_LIBCPP_VERSION)
// Appears still buggy in 14.0 clang libc++ on ubuntu 22.04 (doesnt compile)
// Broken with 150007 on clang15 ubunto 22.04
#define qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION <= 15007))
#elif defined(_GLIBCXX_RELEASE)
// according to https://en.cppreference.com/w/cpp/compiler_support fixed in gcc11 (library so affects clang too if built with glibc)
// AT LEAST with clang++14, this is broken in _GLIBCXX_RELEASE==12 (Ubuntu 22.04)
#define qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_GLIBCXX_RELEASE <= 12)
#else
#define qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy 0
#endif
#endif

/*
 ion_Factory.h:123:
In file included from ./../Characters/../Containers/Factory/KeyedCollection_Factory.inl:16:
In file included from ./../Characters/../Containers/Factory/../Concrete/KeyedCollection_stdset.h:136:
./../Characters/../Containers/Concrete/KeyedCollection_stdset.inl:60:22: error: no viable constructor or deduction guide for deduction of template arguments of 'SetInOrderComparer'
            , fData_{SetInOrderComparer{keyExtractor, inorderComparer}}
                     ^
                     */
#ifndef qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Appears broken on XCode 13
// Still broken on XCode 14
// Still broken on XCode 15
#define qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// appears still broken in clang++-13
// appears still broken in clang++-14
// appears still broken in clang++-15
// appears still broken in clang++-16
#define qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 16))
#elif defined(_MSC_VER)
// Newly broken in _MSC_VER_2k22_17Pt2_ - wonder if that means this is my bug not vs2k22/clang?
// broken in _MSC_VER_2k22_17Pt3_
// Appears FIXED in _MSC_VER_2k22_17Pt4_
#define qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy                                                                            \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((_MSC_VER_2k22_17Pt2_ <= _MSC_VER and _MSC_VER <= _MSC_VER_2k22_17Pt3_))
#else
#define qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy 0
#endif

#endif

/*

/usr/bin/ld: /usr/bin/ld: DWARF error: invalid or unhandled FORM value: 0x23
/tmp/lto-llvm-f3876d.o: in function `Stroika::Foundation::Memory::InlineBuffer<wchar_t, 2048ul>::reserve(unsigned long, bool)':
ld-temp.o:(.text._ZN7Stroika10Foundation6Memory12InlineBufferIwLm2048EE7reserveEmb[_ZN7Stroika10Foundation6Memory12InlineBufferIwLm2048EE7reserveEmb]+0xb6): undefined reference to `Stroika::Foundation::Execution::ThrowIfNull(void const*)'
/usr/bin/ld: /tmp/lto-llvm-f3876d.o: in function `Stroika::Foundation::Memory::InlineBuffer<std::byte, 8192ul>::reserve(unsigned long, bool)':
ld-temp.o:(.text._ZN7Stroika10Foundation6Memory12InlineBufferISt4byteLm8192EE7reserveEmb[_ZN7Stroika10Foundation6Memory12InlineBufferISt4byteLm8192EE7reserveEmb]+0xb1): undefined reference to `Stroika::Foundation::Execution::ThrowIfNull(void const*)'
/usr/bin/ld: /tmp/lto-llvm-f3876d.o: in function `Stroika::Foundation::Memory::InlineBuffer<char, 10240ul>::reserve(unsigned long, bool)':
ld-temp.o:(.text._ZN7Stroika10Foundation6Memory12InlineBufferIcLm10240EE7reserveEmb[_ZN7Stroika10Foundation6Memory12InlineBufferIcLm10240EE7reserveEmb]+0xae): undefined reference to `Stroika::Foundation::Execution::ThrowIfNull(void const*)'

*/
#ifndef qCompilerAndStdLib_release_bld_error_bad_obj_offset_Buggy

#if defined(__clang__) && !defined(__APPLE__)
// appears still broken in clang++-13 and -clang++-14-release-libstdc++
// and broekn uuntu22.04 clang++-15-release-libstdc++
// appears fixed in clang++16
#define qCompilerAndStdLib_release_bld_error_bad_obj_offset_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompilerAndStdLib_release_bld_error_bad_obj_offset_Buggy 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy

WinSock.cpp
Fault.cpp
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\io\network\internetprotocol\IP.h(75): error C2975: '_Test': invalid template argument for 'std::conditional', expected compile-time constant expression (compiling source file ..\..\Sources\Stroika\Foundation\IO\Network\InternetProtocol\IP.cpp)
C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.10.25017\include\xtr1common(69): note: see declaration of '_Test' (compiling source file ..\..\Sources\Stroika\Foundation\IO\Network\InternetProtocol\IP.cpp)
Socket.cpp
SocketAddress.cpp
SocketStream.cpp




1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\IO\Network\InternetProtocol\IP.h(108): error C2975: '_Test': invalid template argument for 'std::conditional_t', expected compile-time constant expression





./../Configuration/Endian.inl:34:37: error: constexpr function never produces a constant expression [-Winvalid-constexpr]
            inline constexpr Endian GetEndianness ()
                                    ^
./../Configuration/Endian.inl:37:25: note: read of member 'cdat' of union with active member 'sdat' is not allowed in a constant expression



error C2975: '_Test': invalid template argument for 'std::conditional', expected compile-time constant expression (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\SSL\SSLSocket.cpp)
1>C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.11.25503\include\xtr1common(66): note: see declaration of '_Test' (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\SSL\SSLSocket.cpp)

    ************NOTE - this may NOT be a bug. I seem to recall soemthing about this being a restruction in C++17 and earlier fixed  in C++20
    (AND I THINK GETENDIAN API IN C++20)


./../Configuration/Endian.inl:33:29: error: constexpr function never produces a constant expression [-Winvalid-constexpr]
    inline constexpr Endian GetEndianness ()
                            ^



                            : error C2975: '_Test': invalid template argument for 'std::conditional_t', expected compile-time constant expressio




            **** NOTE - I THINK THIS MAYBE FIXED IN C++20 - a C++20 feature??? ?>??
*/
#ifndef qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy

#if defined(__clang__) && defined(__APPLE__)
// VERIFIED BROKEN on XCode 11.0
// VERIFIED BROKEN on XCode 12.0
// VERIFIED BROKEN on XCode 13.0
// VERIFIED BROKEN on XCode 14
// VERIFIED BROKEN on XCode 15
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// still broken in clang++-10
// still broken in clang++-11
// still broken in clang++-12
// still broken in clang++-13
// still broken in clang++-14
// still broken in clang++-15
// still broken in clang++-16
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 16))
#elif defined(_MSC_VER)
// verified still broken in _MSC_VER_2k22_17Pt0_
// verified still broken in _MSC_VER_2k22_17Pt1_
// verified still broken in _MSC_VER_2k22_17Pt2_
// verified still broken in _MSC_VER_2k22_17Pt3_
// verified still broken in _MSC_VER_2k22_17Pt4_
// verified still broken in _MSC_VER_2k22_17Pt5_
// verified still broken in _MSC_VER_2k22_17Pt6_
// verified still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy                                                                       \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy 0
#endif

#endif

/**
 * 
 * WORKAROUND THANKS TO 
 *      https://github.com/llvm/llvm-project/issues/55560
 * 
 Sandbox/Stroika-Dev/Builds/clang++-11-release-libstdc++/Stroika-Foundation.a(UniformResourceIdentification.o): in function `basic_string':
/usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/bits/basic_string.h:620: undefined reference to `void std::__cxx11::basic_string<char8_t, std::char_traits<char8_t>, std::allocator<char8_t> >::_M_construct<char8_t const*>(char8_t const*, char8_t const*, std::forward_iterator_tag)'
clang: error: linker command failed with exit code 1 (use -v to see invocation)
make
 * 
 */
// only seen on Ubuntu 22.04
#ifndef qCompilerAndStdLib_clangWithLibStdCPPStringConstexpr_Buggy

#if defined(__clang__) && !defined(__APPLE__) && defined(_GLIBCXX_RELEASE)
// still broken in clang++-14
#define qCompilerAndStdLib_clangWithLibStdCPPStringConstexpr_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 14))
#else
#define qCompilerAndStdLib_clangWithLibStdCPPStringConstexpr_Buggy 0
#endif

#endif

/*
 * NOW ALLOWED IN C++17
Test.cpp:173:31: error: template template argument has different template parameters than its corresponding template template parameter
                Memoizer<int, LRUCache, int, int> memoizer{[&totalCallsCount](int a, int b) { totalCallsCount++;  return a + b; }};
                              ^
/Users/lewis/Sandbox/Stroika-Remote-Build-Dir/Library/Sources/Stroika/Foundation/Cache/LRUCache.h:86:5: note: too many template parameters in template template argument
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER = equal_to<KEY>, typename KEY_HASH_FUNCTION = nullptr_t, typename STATS_TYPE = Statistics::StatsType_DEFAULT>

**/
#ifndef qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy

#if defined(__clang__) && defined(__APPLE__)
// VERIFIED BROKEN on XCode 14
// verified still broken xcode 15
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy                                                \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// verified still broken in clang++-14
// verified still broken in clang++-15
// verified still broken in clang++-16
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy                                                \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 16))
#else
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy 0
#endif

#endif

/*
         Compiling Library/Sources/Stroika/Foundation/Cryptography/Digest/Algorithm/MD5.cpp ... 
PLEASE submit a bug report to https://github.com/llvm/llvm-project/issues/ and include the crash backtrace, preprocessed source, and associated run script.
Stack dump:
0.  Program arguments: clang++-15 --std=c++2b -I/__w/Stroika/Stroika/Builds/Debug/ThirdPartyComponents/include/ -I/__w/Stroika/Stroika/Library/Sources/ -I/__w/Stroika/Stroika/IntermediateFiles/Debug/ -Wall -Wno-switch -Wno-sign-compare -Wno-unused-function -Wno-unused-local-typedef -Wno-future-compat -Wno-unqualified-std-cast-call -fvisibility=hidden -g -D_GLIBCXX_DEBUG -DqDebug=1 -DqHasFeature_LibCurl=1 -DqHasFeature_OpenSSL=1 -DqHasFeature_WinHTTP=0 -DqHasFeature_Xerces=1 -DqHasFeature_ZLib=1 -DqHasFeature_sqlite=1 -DqHasFeature_LZMA=1 -DqHasFeature_boost=1 -DqTraceToFile=1 -DqDefaultTracingOn=1 -fsanitize=address,undefined -stdlib=libc++ -c CodeCvt.cpp -o /__w/Stroika/Stroika/IntermediateFiles/Debug/Library/Foundation/Characters/CodeCvt.o
1.  ./CodeCvt.inl:400:29: at annotation token
2.  ./CodeCvt.inl:19:1: parsing namespace 'Stroika'
 #0 0x00007fd3fd34d3b1 llvm::sys::PrintStackTrace(llvm::raw_ostream&, int) (/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1+0xf043b1)
 #1 0x00007fd3fd34b0fe llvm::sys::RunSignalHandlers() (/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1+0xf020fe)
 #2 0x00007fd3fd34c771 llvm::sys::CleanupOnSignal(unsigned long) (/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1+0xf03771)
 #3 0x00007fd3fd27067f (/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1+0xe2767f)
 #4 0x00007fd3fbf26520 (/lib/x86_64-linux-gnu/libc.so.6+0x42520)
 #5 0x00007fd4041dbbe4 (/usr/lib/llvm-15/bin/../lib/libclang-cpp.so.15+0xda0be4)
 #6 0x00007fd4041b4a56 (/usr/lib/llvm-15/bin/../lib/libclang-cpp.so.15+0xd79a56)
 #7 0x00007fd4041b09f4 (/usr/lib/llvm-15/bin/../lib/libclang-cpp.so.15+0xd759f4)
 #8 0x00007fd4041b06f1 clang::Expr::EvaluateAsConstantExpr(clang::Expr::EvalResult&, clang::ASTContext const&, clang::Expr::ConstantExprKind) const (/usr/lib/llvm-15/bin/../lib/libclang-cpp.so.15+0xd756f1)
 #9 0x00007fd40472c5dc (/usr/lib/llvm-15/bin/../lib/libclang-cpp.so.15+0x12f15dc)
#10 0x00007fd40472c082 clang::Sema::CheckConstraintSatisfaction(clang::NamedDecl const*, llvm::ArrayRef<clang::Expr const*>, llvm::ArrayRef<clang::TemplateArgument>, clang::SourceRange, clang::ConstraintSatisfaction&) (/usr/lib/llvm-15/bin/../lib/libclang-cpp.so.15+0x12f1082)
*/

#ifndef qCompilerAndStdLib_template_second_concept_Buggy

#if defined(__clang__) && defined(__APPLE__)
// VERIFIED BROKEN on XCode 14
// appears fixed in XCODE 15
#define qCompilerAndStdLib_template_second_concept_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 14))
#elif defined(__clang__) && !defined(__APPLE__)
// verified still broken in clang++-15
// appears fixed in clang++16
#define qCompilerAndStdLib_template_second_concept_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompilerAndStdLib_template_second_concept_Buggy 0
#endif

#endif

/*
 *
 * 
FAILED: RegressionTestFailure; replaced == L"abcdef";;Test.cpp: 753
   []  (1  seconds)  [02]  Foundation::Characters::Strings  (../Builds/clang++-11-debug-libc++/Tests/Test02)


@CONFIGVAR:     qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy

    String  replaced        =   stringWithCRLFs.ReplaceAll (RegularExpression (L"[\r\n]*", RegularExpression::eECMAScript), L"");
    infinite loops in RegTest #2 - HANG - Test17_RegExp_Search_

    ### REVIEW - COULD BE BUG IN MY CODE - BUT WORKS OK WITH gnu libstdc++ and msft C++ lib, and just fails with clang's libc++
*/
#ifndef qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy

#if defined(_LIBCPP_VERSION)
// Broken in _LIBCPP_VERSION  14000
// Broken in _LIBCPP_VERSION  15007
// Broken in _LIBCPP_VERSION  160000
#if defined(__APPLE__)
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION < 170000))
#else
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION < 170000))
#endif
#else
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy 0
#endif

#endif

/*
 * SEE https://developercommunity.visualstudio.com/content/problem/484206/const-int-posv-winerror-map-errval-should-probably.html
 *
 *  to test if fixed, run
 *      Test37 (execution::Exceptions) on windows and look in tracelog for 
 *          FIXED - qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy
 * 
 * FAILED: RegressionTestFailure; (error_code{WAIT_TIMEOUT, system_category ()} == errc::timed_out);;C:\Sandbox\Stroika\DevRoot\Tests\36\Test.cpp: 218
 */
#ifndef qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy

#if defined(_MSC_VER)
// Verified still broken in _MSC_VER_2k22_17Pt0_
// Verified still broken in _MSC_VER_2k22_17Pt1_
// Verified still broken in _MSC_VER_2k22_17Pt2_
// Verified still broken in _MSC_VER_2k22_17Pt3_
// Verified still broken in _MSC_VER_2k22_17Pt4_
// Verified still broken in _MSC_VER_2k22_17Pt5_
// Verified still broken in _MSC_VER_2k22_17Pt6_
// Verified still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy                                                                           \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy 0
#endif

#endif

/**
 // _mkgmtime64 not portable, but seems to be defined everywhere I've tried - add a qSupported if/when needed
 https://msdn.microsoft.com/en-us/library/2093ets1.aspx
 */
#if !defined(qCompilerAndStdLib_Supported_mkgmtime64)
#if defined(_MSC_VER)
#define qCompilerAndStdLib_Supported_mkgmtime64 1
#else
#define qCompilerAndStdLib_Supported_mkgmtime64 0
#endif
#endif

/**
 *  qCompilerAndStdLib_Supports_errno_t controls whether or not the compiler provides an implementation
 *  of errno_t (gcc for example just assumes its int).
 *
 *  NB: POSIX and C99 just say to assume its an int and doesn't define errno_t.
 */
#if !defined(qCompilerAndStdLib_Supports_errno_t)
#if defined(__clang__)
#define qCompilerAndStdLib_Supports_errno_t 0
#elif defined(__GNUC__)
#define qCompilerAndStdLib_Supports_errno_t 0
#elif defined(_MSC_VER)
#define qCompilerAndStdLib_Supports_errno_t 1
#else
#define qCompilerAndStdLib_Supports_errno_t 1
#endif
#endif

#if !defined(qCompiler_ValgrindDirectSignalHandler_Buggy)

// Now set in the configure script, because this depends on the OS
#define qCompiler_ValgrindDirectSignalHandler_Buggy 0

#endif

/**
* 
* OR sometimes wtih VS2k22 17.4, when running from command line, just appears to hang from command line 
*  (but see crash in debugger)
* 
* 


// ASAN crash on windows, with openssl maybe just with default provider


    ntdll.dll!00007ffa024a6cde()    Unknown
    ntdll.dll!00007ffa0246dc84()    Unknown
    ntdll.dll!00007ffa0241b5f1()    Unknown
    KernelBase.dll!00007ff9ffabcd2b()   Unknown
>   legacy.dll!_CrtIsValidHeapPointer(const void * block) Line 1407 C++
    legacy.dll!free_dbg_nolock(void * const block, const int block_use) Line 904    C++
    legacy.dll!_free_dbg(void * block, int block_use) Line 1030 C++
    legacy.dll!_freea_crt(void * memory) Line 317   C++
    legacy.dll!__crt_scoped_stack_ptr<wchar_t>::~__crt_scoped_stack_ptr<wchar_t>() Line 457 C++
    legacy.dll!__acrt_GetStringTypeA(__crt_locale_pointers * locale, unsigned long info_type, const char * string, int string_size_in_bytes, unsigned short * char_type, int code_page, int error) Line 99  C++
    legacy.dll!setSBUpLow(__crt_multibyte_data * ptmbci) Line 633   C++
    legacy.dll!_setmbcp_nolock(int codepage, __crt_multibyte_data * ptmbci) Line 838    C++
    legacy.dll!setmbcp_internal(const int requested_codepage, const bool is_for_crt_initialization, __acrt_ptd * const ptd, __crt_multibyte_data * * const current_multibyte_data) Line 424 C++
    legacy.dll!__acrt_initialize_multibyte() Line 920   C++
    legacy.dll!__acrt_execute_initializers(const __acrt_initializer * first, const __acrt_initializer * last) Line 25   C++
    legacy.dll!__acrt_initialize() Line 291 C++
    [External Code] 
    Test31.exe!win32_load(dso_st * dso) Line 106    C
    Test31.exe!DSO_load(dso_st * dso, const char * filename, dso_meth_st * meth, int flags) Line 151    C
    Test31.exe!provider_init(ossl_provider_st * prov) Line 887  C
    Test31.exe!provider_activate(ossl_provider_st * prov, int lock, int upcalls) Line 1103  C
    Test31.exe!ossl_provider_activate(ossl_provider_st * prov, int upcalls, int aschild) Line 1178  C
    Test31.exe!OSSL_PROVIDER_try_load(ossl_lib_ctx_st * libctx, const char * name, int retain_fallbacks) Line 31    C
    Test31.exe!OSSL_PROVIDER_load(ossl_lib_ctx_st * libctx, const char * name) Line 56  C
    Test31.exe!Stroika::Foundation::Cryptography::OpenSSL::LibraryContext::LoadProvider(const Stroika::Foundation::Characters::String & providerName) Line 228  C++
    Test31.exe!Stroika::Foundation::Cryptography::OpenSSL::LibraryContext::TemporarilyAddProvider::TemporarilyAddProvider(Stroika::Foundation::Cryptography::OpenSSL::LibraryContext * context, const Stroika::Foundation::Characters::String & providerName) Line 26   C++
    [External Code] 
    Test31.exe!`anonymous namespace'::AllSSLEncrytionRoundtrip::DoRegressionTests_() Line 550   C++


OR
=================================================================
==32172==ERROR: AddressSanitizer: access-violation on unknown address 0x7ffc091e2a54 (pc 0x7ffc091e2a54 bp 0x000000000003 sp 0x009d070ff6b8 T0)
    #0 0x7ffc091e2a53  (<unknown module>)
    #1 0x7ffd22e34acb in RtlProcessFlsData+0xfb (C:\WINDOWS\SYSTEM32\ntdll.dll+0x180054acb)
    #2 0x7ffd22e34781 in LdrShutdownProcess+0x101 (C:\WINDOWS\SYSTEM32\ntdll.dll+0x180054781)
    #3 0x7ffd22e3465c in RtlExitUserProcess+0xac (C:\WINDOWS\SYSTEM32\ntdll.dll+0x18005465c)
    #4 0x7ffd224ac66a in ExitProcess+0xa (C:\WINDOWS\System32\KERNEL32.DLL+0x18001c66a)
    #5 0x7ff67d0a1e79 in exit_or_terminate_process minkernel\crts\ucrt\src\appcrt\startup\exit.cpp:143
    #6 0x7ff67d0a1e35 in common_exit minkernel\crts\ucrt\src\appcrt\startup\exit.cpp:280
    #7 0x7ff67d0a2185 in exit minkernel\crts\ucrt\src\appcrt\startup\exit.cpp:293
    #8 0x7ff67cd8cc36 in __scrt_common_main_seh D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:295
    #9 0x7ff67cd8cadd in __scrt_common_main D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl:330
    #10 0x7ff67cd8cd3d in mainCRTStartup D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_main.cpp:16
    #11 0x7ffd224a54df in BaseThreadInitThunk+0xf (C:\WINDOWS\System32\KERNEL32.DLL+0x1800154df)
    #12 0x7ffd22de485a in RtlUserThreadStart+0x2a (C:\WINDOWS\SYSTEM32\ntdll.dll+0x18000485a)

AddressSanitizer can not provide additional info.
SUMMARY: AddressSanitizer: access-violation (<unknown module>)
==32172==ABORTING

*/
#ifndef qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy

#if defined(_MSC_VER)
// still buggy in _MSC_VER_2k22_17Pt0_
// still buggy in _MSC_VER_2k22_17Pt1_
// still buggy in _MSC_VER_2k22_17Pt2_
// still buggy in _MSC_VER_2k22_17Pt3_
// still buggy in _MSC_VER_2k22_17Pt4_
// Appears fixed in _MSC_VER_2k22_17Pt5_
#define qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy                                                                    \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt4_)
#elif defined(__GNUC__) && !defined(__clang__)
// VERIFIED BROKEN IN GCC 11
// appears fixed in GCC 12
#define qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 11)
#else
#define qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy 0
#endif

#endif

/*
ON DEBUG builds on macos only

       Builds/Debug/Tests/Test43 now

   [Succeeded]  (2  seconds)  [41]  Foundation::Execution::Other  (../Builds/Debug/Tests/Test41)
Interface.cpp:458:60: runtime error: member access within misaligned address 0x7ffee9ca61b4 for type 'const ifreq', which requires 8 byte alignment
0x7ffee9ca61b4: note: pointer points here
  00 00 00 00 6c 6f 30 00  00 00 00 00 00 00 00 00  00 00 00 00 10 02 00 00  7f 00 00 01 00 00 00 00
              ^
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior Interface.cpp:458:60 in
Interface.cpp:195:71: runtime error: member access within misaligned address 0x7ffee9ca61b4 for type 'const ifreq', which requires 8 byte alignment
0x7ffee9ca61b4: note: pointer points here
  00 00 00 00 6c 6f 30 00  00 00 00 00 00 00 00 00  00 00 00 00 10 02 00 00  7f 00 00 01 00 00 00 00
              ^
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior Interface.cpp:195:71 in
Interface.cpp:204:38: runtime error: member access within misaligned address 0x7ffee9ca61b4 for type 'const ifreq', which requires 8 byte alignment
0x7ffee9ca61b4: note: pointer points here
  00 00 00 00 6c 6f 30 00  00 00 00 00 00 00 00 00  00 00 00 00 10 02 00 00  7f 00 00 01 00 00 00 00
              ^
SUMMARY: UndefinedBehaviorSanitizer: undefined-behavior Interface.cpp:204:38 in
Interface.cpp:347:45: runtime error: member access within misaligned address 0x7ffee9ca61b4 for type 'const ifreq', which requires 8 byte alignment
0x7ffee9ca61b4: note: pointer points here
  00 00 00 00 6c 6f 30 00  00 00 00 00 00 00 00 00  00 00 00 00 10 02 00 00  7f 00 00 01 00 00 00 00


PROBABLY not a sanitizer bug - but not totally clear what this is caused by
TRIED alignas to fix on the array but no luck
*/
#if !defined(qMacUBSanitizerifreqAlignmentIssue_Buggy)

#if defined(__clang__) && defined(__APPLE__)
// reproduced on XCode 14
// reproduced on XCode 15
#define qMacUBSanitizerifreqAlignmentIssue_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ <= 15)
#else
#define qMacUBSanitizerifreqAlignmentIssue_Buggy 0
#endif

#endif

/*
 */
#ifndef qCompilerAndStdLib_locale_pctX_print_time_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Still BROKEN on XCode 14
// Still BROKEN on XCode 15
#define qCompilerAndStdLib_locale_pctX_print_time_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompilerAndStdLib_locale_pctX_print_time_Buggy 0
#endif

#endif

/*
 *  DateTime::Format (const locale& l, const String& formatPattern) const
 *     const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
 *     tmput.put (oss, oss, ' ', &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
 *  with formatPattern = "%c" - produces a numeric format date with C locale, compared to UNIX which produces 
 *  %a %b %e %T %Y - as is suggestged by code docs and https://en.cppreference.com/w/cpp/locale/time_put/put (example - not clear cuz not for all locales)
   ...
FAILED: RegressionTestFailure; tmp == L"Sun 05 Apr 1903 12:01:41 AM";;C:\Sandbox\Stroika\DevRoot\Tests\50\Test.cpp: 505
   []  (0  seconds)  [48]  Foundation::Time  (../Builds/Release-U-64/Tests/Test48.exe)
 */
#ifndef qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy

#if defined(_MSC_VER)
// verified broken in _MSC_VER_2k22_17Pt0_
// verified broken in _MSC_VER_2k22_17Pt1_
// verified broken in _MSC_VER_2k22_17Pt2_
// verified broken in _MSC_VER_2k22_17Pt3_
// verified broken in _MSC_VER_2k22_17Pt4_
// verified broken in _MSC_VER_2k22_17Pt5_
// verified broken in _MSC_VER_2k22_17Pt6_
// verified broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy                                                                \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy 0
#endif

#endif

/*
* Only broken on vs2k22, Release-x86 builds
FAILED: RegressionTestFailure; f1 < f2 or f2 < f1;;C:\Sandbox\Stroika\DevRoot\Tests\42\Test.cpp: 66
   []  (0  seconds)  [42]  Foundation::Execution::Other  (../Builds/Release-x86/Tests/Test42.exe)
*/
#ifndef qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy

#if defined(_MSC_VER)
// first noticed broken in _MSC_VER_2k22_17Pt0_
// still broken in _MSC_VER_2k22_17Pt1_
// still broken in _MSC_VER_2k22_17Pt2_
// still broken in _MSC_VER_2k22_17Pt3_
// still broken in _MSC_VER_2k22_17Pt4_
// still broken in _MSC_VER_2k22_17Pt5_
// still broken in _MSC_VER_2k22_17Pt6_
// still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy                                                                 \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k22_17Pt0_ <= _MSC_VER and _MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy 0
#endif

#endif

/*
 *  In DateTime DateTime::Parse (const String& rep, const locale& l, const String& formatPattern)
 *
 *   (void)tmget.get (itbegin, itend, iss, errState, &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
 *
 *#if qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy
 *    if (formatPattern == "%x %X") {
 *        if ((errState & ios::badbit) or (errState & ios::failbit)) {
 *            Execution::Throw (Date::FormatException::kThe);
 *        }
 *        wistringstream               iss2 (rep.As<wstring> ());
 *        istreambuf_iterator<wchar_t> itbegin2 (iss2);
 *        istreambuf_iterator<wchar_t> itend2;
 *        errState = ios::goodbit;
 *        tmget.get_date (itbegin2, itend2, iss, errState, &when);
 *    }
 *#endif
 * 
 *  Upon more careful analysis - 2021-03-07 - the bug appears to be that the tmget HAS THE CORRECT mdy dateorder, but in parsing
 *  it appears to IGNORE this and prase DMY
 * 
 * 
 *      REPORTED TO MICROSOFT:
 *              https://rextester.com/IOLX73233
 *              2021-03-07
 * 
 * 
 *  BUG WAS CALLED qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy until 2021-03-10, then renamed to be clearer
 * 
 *  NOTE - though not directly related to this bug, this is AGGRAVATED and of extra importance betcause of
 *          StdCPctxTraits::kLocaleClassic_Write4DigitYear being false
 */
#ifndef qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy

#if defined(_MSC_VER)
// verified still broken in _MSC_VER_2k22_17Pt0_
// verified still broken in _MSC_VER_2k22_17Pt1_
// verified still broken in _MSC_VER_2k22_17Pt2_
// verified still broken in _MSC_VER_2k22_17Pt3_
// verified still broken in _MSC_VER_2k22_17Pt4_
// verified still broken in _MSC_VER_2k22_17Pt5_
// verified still broken in _MSC_VER_2k22_17Pt6_
// verified still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy                                                       \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy 0
#endif

#endif

/*
 *
 *      FAILED: REGRESSION TEST DUE TO EXCEPTION: 'Invalid Time Format'
 *      [Failed]  (0  seconds)  [31]  Foundation::DataExchange::ObjectVariantMapper  (../Builds/clang++-7-release-libstdc++/Tests/Test31)
 *      FAILED: REGRESSION TEST DUE TO EXCEPTION: 'Invalid Time Format'
 *      [Failed]  (0  seconds)  [31]  Foundation::DataExchange::ObjectVariantMapper  (../Builds/clang++-7-release-libstdc++/Tests/Test31)
 *
 */
#ifndef qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy

#if defined(_GLIBCXX_RELEASE)
// VERIFIED BROKEN IN GCC10.0
// VERIFIED BROKEN IN GCC11
#define qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_GLIBCXX_RELEASE <= 11)
#else
#define qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy 0
#endif

#endif

// Must run regtest 2, and see if pass/fail (or look into why not handling these unicode strings)
// FAILED: RegressionTestFailure; not initializedLocale;;C:\Sandbox\Stroika\DevRoot\Tests\02\Test.cpp: 1203
#ifndef qCompilerAndStdLib_locale_utf8_string_convert_Buggy

#if defined(_MSC_VER)
// verified still broken in _MSC_VER_2k22_17Pt0_
// verified still broken in _MSC_VER_2k22_17Pt1_
// verified still broken in _MSC_VER_2k22_17Pt2_
// verified still broken in _MSC_VER_2k22_17Pt3_
// verified still broken in _MSC_VER_2k22_17Pt4_
// verified still broken in _MSC_VER_2k22_17Pt5_
// verified still broken in _MSC_VER_2k22_17Pt6_
// verified still broken in _MSC_VER_2k22_17Pt7_
#define qCompilerAndStdLib_locale_utf8_string_convert_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt7_)
#else
#define qCompilerAndStdLib_locale_utf8_string_convert_Buggy 0
#endif

#endif

/**
 *  We use some of these definitions in .natvis files, so we need them to stay around
 *  so they can be called by the debugger.\
 * 
 *  See https://developercommunity.visualstudio.com/t/please-re-open-httpsdevelopercommunityvisualstudio-1/1473717 
 *  for where I complained about this.
 * 
 *  TO TEST:
 *      > Run in debugger the TIME (roughly Test50) - and break in Test_2_TestTimeOfDay_, and Test_5_DateTimeTimeT_ etc 
 *        and see if the ToString() stuff gets called in debugger when viewing TimeOfday, DateTime etc objects.
 * 
 * AND WHEN I do next stroika release, send note to MSFT that this is still broken and provide sample/reference to how to reproduce
 * using 'Test50'.
*/
#ifndef qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy

#if defined(_MSC_VER)
// still broken in _MSC_VER_2k22_17Pt0_
// Microsoft appears to be ignorning this, so just assume broken in _MSC_VER_2k22_17Pt1_
// still broken in _MSC_VER_2k22_17Pt2_
#define qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy                                                                \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt2_)
#else
#define qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy 0
#endif

#endif

/*
 * Assertions.cpp:178:5: error: use of undeclared identifier 'quick_exit'
 */
#ifndef qCompilerAndStdLib_quick_exit_Buggy

#if defined(__clang__) && defined(__APPLE__)
// VERIFIED STILL BROKEN on XCode 14
// VERIFIED STILL BROKEN on XCode 15
#define qCompilerAndStdLib_quick_exit_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#else
#define qCompilerAndStdLib_quick_exit_Buggy 0
#endif

#endif

/*
 *     Running Stroka Tests {g++-valgrind-debug-SSLPurify-NoBlockAlloc}:
 *      [Succeeded]  (3  seconds)  [01]  Foundation::Caching  (valgrind -q --track-origins=yes --tool=memcheck --leak-check=full --suppressions=Valgrind-MemCheck-Common.supp  ../Builds/g++-valgrind-debug-SSLPurify-NoBlockAlloc/Test01)
 *              FAILED: Assert; !isinf (f);Stroika::Foundation::Characters::String {anonymous}::Float2String_(FLOAT_TYPE, const Stroika::Foundation::Characters::Float2StringOptions&) [with FLOAT_TYPE = long double];FloatConversion.cpp: 200
 *      []  (28 seconds)  [02]  Foundation::Characters::Strings  (valgrind -q --track-origins=yes --tool=memcheck --leak-check=full --suppressions=Valgrind-MemCheck-Common.supp  ../Builds/g++-valgrind-debug-SSLPurify-NoBlockAlloc/Test02)
 *
 *      Under VALGRIND, 
 *                                  DbgTrace ("fpclassify (%f) = %d", (double)f, fpclassify (f));
 *          prints:
 *                [-------MAIN-------][0022.587]  fpclassify (inf) = 4
 *                [-------MAIN-------][0022.595]  fpclassify (-inf) = 4
 * and from math.h:
 *              # define FP_NORMAL 4
 *
 *  \note - This maybe just a known valgrind bug/feature:
 *              https://stackoverflow.com/questions/44316523/wrong-result-of-stdfpclassify-for-long-double-using-valgrind
 */
#ifndef qCompilerAndStdLib_valgrind_fpclassify_check_Buggy

#if defined(__GNUC__)
// tested and fails gcc8 on Ubuntu 1804 (could be OS config/valgrind version)
#define qCompilerAndStdLib_valgrind_fpclassify_check_Buggy 1
#else
#define qCompilerAndStdLib_valgrind_fpclassify_check_Buggy 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_Support__PRETTY_FUNCTION__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__PRETTY_FUNCTION__

#if defined(__clang__) || defined(__GNUC__)
#define qCompilerAndStdLib_Support__PRETTY_FUNCTION__ 1
#else
#define qCompilerAndStdLib_Support__PRETTY_FUNCTION__ 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_Support__func__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__func__

#if defined(__clang__) || defined(__GNUC__)
#define qCompilerAndStdLib_Support__func__ 1
#else
#define qCompilerAndStdLib_Support__func__ 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_Support__FUNCTION__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__FUNCTION__

#if defined(_MSC_VER)
#define qCompilerAndStdLib_Support__FUNCTION__ 1
#else
#define qCompilerAndStdLib_Support__FUNCTION__ 0
#endif

#endif

/*
 *******************************************************************
 *******************************************************************
 ********************* REACT TO SOME DEFINES ***********************
 *******************************************************************
 *******************************************************************
 */

// When MSFT fixes qCompilerAndStdLib_cplusplus_macro_value_Buggy move back to the top of the file
#if qCompilerAndStdLib_cplusplus_macro_value_Buggy
#if _MSVC_LANG < kStrokia_Foundation_Configuration_cplusplus_20
#pragma message("Stroika v3 requires at least C++ ISO/IEC 14882:2020(E) supported by the compiler (informally known as C++ 20)")
#endif
#else
#if __cplusplus < kStrokia_Foundation_Configuration_cplusplus_20
#pragma message("Stroika v3 requires at least C++ ISO/IEC 14882:2020(E) supported by the compiler (informally known as C++ 20)")
#endif
#endif

#if qSilenceAnnoyingCompilerWarnings && defined(__GNUC__) && !defined(__clang__)
// Note - I tried tricks with token pasting, but only seems to work if I do all token pasting
// and that fails with 'astyle' which breaks up a-b tokens. Need quotes to work with astyle
// and no way I can find to concatenate strings that works with _Pragma
//  --LGP 2014-01-05
#define DISABLE_COMPILER_GCC_WARNING_START(WARNING_TO_DISABLE) _Pragma ("GCC diagnostic push") _Pragma (WARNING_TO_DISABLE)
#define DISABLE_COMPILER_GCC_WARNING_END(WARNING_TO_DISABLE) _Pragma ("GCC diagnostic pop")
#else
#define DISABLE_COMPILER_GCC_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_GCC_WARNING_END(WARNING_TO_DISABLE)
#endif

#if qSilenceAnnoyingCompilerWarnings && defined(_MSC_VER)
#define DISABLE_COMPILER_MSC_WARNING_START(WARNING_TO_DISABLE) __pragma (warning (push)) __pragma (warning (disable : WARNING_TO_DISABLE))
#define DISABLE_COMPILER_MSC_WARNING_END(WARNING_TO_DISABLE) __pragma (warning (pop))
#else
#define DISABLE_COMPILER_MSC_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_MSC_WARNING_END(WARNING_TO_DISABLE)
#endif

#if qSilenceAnnoyingCompilerWarnings && defined(__clang__)
// Note - I tried tricks with token pasting, but only seems to work if I do all token pasting
// and that fails with 'astyle' which breaks up a-b tokens. Need quotes to work with astyle
// and no way I can find to concatenate strings that works with _Pragma
//  --LGP 2014-01-05
#define DISABLE_COMPILER_CLANG_WARNING_START(WARNING_TO_DISABLE) _Pragma ("clang diagnostic push") _Pragma (WARNING_TO_DISABLE)
#define DISABLE_COMPILER_CLANG_WARNING_END(WARNING_TO_DISABLE) _Pragma ("clang diagnostic pop")
#else
#define DISABLE_COMPILER_CLANG_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_CLANG_WARNING_END(WARNING_TO_DISABLE)
#endif

#if qSilenceAnnoyingCompilerWarnings && defined(_MSC_VER)
// Our pattern of
//      AssertNotReached();
//      return x;
//  generates this warning:
//       warning C4702: unreachable code
// and we do this all over the place. The issue is that the assert if debug is true expands to [[noreturn]]
//  and if debug is false, expands to nothing. So the compiler sees different expectations of whether you ever get the the line
//  return x;
#pragma warning(disable : 4702)
#endif

// doesn't seem any portable way todo this, and not defined in C++ language
// Note - this doesn't appear in http://en.cppreference.com/w/cpp/language/attributes - as of 2016-06-22
#if defined(__clang__) || defined(__GNUC__)
#define dont_inline __attribute__ ((noinline))
#else
#define dont_inline __declspec (noinline)
#endif

/*
 *   Sometimes its handy to mark a line of code as a no-op - so its arguments are not executed (as with
 * trace macros).
 */
#if !defined(_NoOp_)
#if defined(_MSC_VER)
#if qCompilerAndStdLib_ArgumentDependentLookupInTemplateExpansionTooAggressiveNowBroken_Buggy
#define _NoOp_(...)
#else
#define _NoOp_ __noop
#endif
#else
#define _NoOp_(...)
#endif
#endif

#ifndef _Stroika_Foundation_Configuration_Private_DO_PRAGMA_
#define _Stroika_Foundation_Configuration_Private_DO_PRAGMA_(x) _Pragma (#x)
#endif

/*
 *  Wrap this macro around entire declaration, as in:
 *       _DeprecatedFile_ ("DEPRECATED in v2.0a32 - use IO::FileSystem::DirectoryIterator");
 */
#if !defined(_DeprecatedFile_)
#define _DeprecatedFile_(MESSAGE) _Stroika_Foundation_Configuration_Private_DO_PRAGMA_ (message ("WARNING: Deprecated File: " MESSAGE))
#endif

/*
 * https://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros
 * https://en.cppreference.com/w/cpp/language/attributes/no_unique_address
 *      *** DEPRECATED IN STROIKA 3.0d1 *** (can directly use attributes since assuming C++20)
 */
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(no_unique_address)
#define NO_UNIQUE_ADDRESS_ATTR no_unique_address
#else
#define NO_UNIQUE_ADDRESS_ATTR
#endif
#else
#define NO_UNIQUE_ADDRESS_ATTR
#endif

/*
 * https://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros
 * https://en.cppreference.com/w/cpp/language/attributes/likely
 *      *** DEPRECATED IN STROIKA 3.0d1 *** (can directly use attributes since assuming C++20)
 */
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(likely)
#define LIKELY_ATTR likely
#else
#define LIKELY_ATTR
#endif
#else
#define LIKELY_ATTR
#endif

/*
 * https://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros
 * https://en.cppreference.com/w/cpp/language/attributes/likely
 *      *** DEPRECATED IN STROIKA 3.0d1 *** (can directly use attributes since assuming C++20)
 */
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(unlikely)
#define UNLIKELY_ATTR unlikely
#else
#define UNLIKELY_ATTR
#endif
#else
#define UNLIKELY_ATTR
#endif

/**
 *  \def Stroika_Foundation_Configuration_STRUCT_PACKED
 *
 *  Make the argument strucuture packed - with as little empty space between elements as possible. This is not portable,
 *  but generally works.
 *
 *  \par Example Usage
 *      \code
 *          Stroika_Foundation_Configuration_STRUCT_PACKED (struct ICMPHeader {
 *              byte     type; // ICMP packet type
 *              byte     code; // Type sub code
 *              uint16_t checksum;
 *              uint16_t id;
 *              uint16_t seq;
 *              uint32_t timestamp; // not part of ICMP, but we need it
 *          });
 *      \endcode
 *
 */
#if defined(_MSC_VER)
#define Stroika_Foundation_Configuration_STRUCT_PACKED(...) __pragma (pack (push, 1)) __VA_ARGS__ __pragma (pack (pop))
#elif defined(__GNUC__) || defined(__clang__)
#define Stroika_Foundation_Configuration_STRUCT_PACKED(...) __VA_ARGS__ __attribute__ ((__packed__))
#endif

#endif /*defined(__cplusplus)*/

#if qCompilerAndStdLib_need_ciso646_Buggy
#include <ciso646>
#endif

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#if (__clang_major__ < 15) || (__clang_major__ == 15 && (__clang_minor__ < 0))
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: Stroika v3 (older clang versions supported by Stroika v2.1) does not support versions prior to APPLE clang++ 15 (XCode 14)"
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
#if (__clang_major__ > 18)
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Info: Stroika untested with this version of clang++ - (>18.x) USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#endif

#elif defined(__GNUC__)

#if __GNUC__ < 11
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: Stroika v3 does not support versions prior to GCC 11 (v2.1 supports g++7 and later, v2.0 supports g++5 and g++6 and g++-7)"
#endif
#if __GNUC__ > 14
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
#define _MSC_VER_2k22_17Pt8_ 1938
#define _MSC_VER_2k22_17Pt9_ 1939

// We COULD look at _MSC_FULL_VER but changes too often and too rarely makes a difference: just assume all bug defines the same for a given _MSC_VER
#if _MSC_VER < _MSC_VER_2k22_17Pt0_
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2022 (use Stroika v2.1 or earlier)"
#elif _MSC_VER <= _MSC_VER_2k22_17Pt9_
// We COULD look at _MSC_FULL_VER but changes too often and too rarely makes a difference: just assume all bug defines the same for a given _MSC_VER
#else
#define _STROIKA_CONFIGURATION_WARNING_                                                                                                    \
    "Warning: This version of Stroika is untested with this release (> 17.9) of Microsoft Visual Studio.net / Visual C++ - USING "         \
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
Test.cpp:1109:93: error: no viable constructor or deduction guide for deduction of template arguments of 'span'
            ScanDetails_ sd2 = doRead_ (Streams::ExternallyOwnedSpanInputStream::New<byte> (span{b}));
*/

#ifndef qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy

#if defined(_LIBCPP_VERSION)
#define qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy (_LIBCPP_VERSION < 16000)
#else
#define qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy 0
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
// verified fixed in _MSC_VER_2k22_17Pt8_
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
// verified broken in _MSC_VER_2k22_17Pt8_
// verified broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop_Buggy                                                                          \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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
Foundation::IO::Network::Transfer - ../Builds/Debug/Tests/Test45.exe --gtest_brief


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
// verified broken in _MSC_VER_2k22_17Pt8_
// verified broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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
// fixed in _MSC_VER_2k22_17Pt8_
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
     Compiling Library/Sources/Stroika/Frameworks/Led/BiDiLayoutEngine.cpp ... 
In file included from /usr/include/x86_64-linux-gnu/c++/14/bits/c++config.h:887,
                 from /usr/include/c++/14/climits:41,
                 from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Configuration/Private/Defaults_CompilerAndStdLib_.h:46,
                 from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Configuration/StroikaConfig.h:18,
                 from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/StroikaPreComp.h:7,
                 from ../StroikaPreComp.h:7,
                 from BiDiLayoutEngine.cpp:4:
/usr/include/c++/14/pstl/algorithm_impl.h: In function ‘_RandomAccessIterator __pstl::__internal::__brick_unique(_RandomAccessIterator, _RandomAccessIterator, _BinaryPredicate, std::true_type)’:
/usr/include/c++/14/pstl/algorithm_impl.h:1219:5: note: ‘#pragma message:  [Parallel STL message]: "Vectorized algorithm unimplemented, redirected to serial"’
 1219 |     _PSTL_PRAGMA_MESSAGE("Vectorized algorithm unimplemented, redirected to serial");
      |     ^~~~~~~~~~~~~~~~~~~~
/
*/
#ifndef qCompilerAndStdLib_PSTLWARNINGSPAM_Buggy

#if defined(__GNUC__) && !defined(__clang__)
#define qCompilerAndStdLib_PSTLWARNINGSPAM_Buggy ((__GNUC__ == 14))
#else
#define qCompilerAndStdLib_PSTLWARNINGSPAM_Buggy 0
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


./../DataExchange/XML/Schema.inl:40:34: error: out-of-line declaration of 'As' does not match any declaration in 'Stroika::Foundation::DataExchange::XML::Schema::Ptr'
   40 |     XML::DOM::Document::Ptr Ptr::As ();
      |                                  ^~
./../DataExchange/XML/Schema.inl:68:34: error: out-of-line declaration of 'As' does not match any declaration in 'Stroika::Foundation::DataExchange::XML::Schema::Ptr'
   68 |     XML::DOM::Document::Ptr Ptr::As (const Providers::IDOMProvider& p);



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

 file included from ./CodeCvt.h:469:
./CodeCvt.inl:591:53: error: out-of-line definition of 'New' does not match any declaration in 'Stroika::Foundation::Characters::CodeCvt::Options'
  591 |     constexpr inline auto CodeCvt<CHAR_T>::Options::New (typename CodeCvt<FROM_CHAR_T_OPTIONS>::Options o) -> Options
      |                                                     ^~~
1 e
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
// still broken in clang++ 17
// still broken in clang++ 18
#define qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy                                               \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
#else
#define qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy 0
#endif

#endif

/*
 ile included from Statistics.cpp:6:
In file included from ./../Characters/Format.h:30:
In file included from ./../Characters/String.h:17:
In file included from ./../Characters/../Containers/Set.h:629:
In file included from ./../Characters/../Containers/Set.inl:10:
In file included from ./../Characters/../Containers/Factory/Set_Factory.h:114:
In file included from ./../Characters/../Containers/Factory/Set_Factory.inl:14:
In file included from ./../Characters/../Containers/Factory/../Concrete/Set_Array.h:124:
./../Characters/../Containers/Factory/../Concrete/Set_Array.inl:36:15: error: type constraint differs in template redeclaration
   36 |     template <IEqualsComparer<T> EQUALS_COMPARER>
   
   */

#ifndef qCompilerAndStdLib_ContraintInMemberClassSeparateDeclare_Buggy

#if defined(__clang__) && !defined(__APPLE__)
// first noticed broken in apply clang 18
#define qCompilerAndStdLib_ContraintInMemberClassSeparateDeclare_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ == 18))
#else
#define qCompilerAndStdLib_ContraintInMemberClassSeparateDeclare_Buggy 0
#endif

#endif

#if qCompilerAndStdLib_ContraintInMemberClassSeparateDeclare_Buggy
#define BWA_Helper_ContraintInMemberClassSeparateDeclare_(ConstrainedTypeName) typename
#else
#define BWA_Helper_ContraintInMemberClassSeparateDeclare_(ConstrainedTypeName) ConstrainedTypeName
#endif

/**
 *  Library/Sources/Stroika/Foundation/Characters/String.cpp ... 
In file included from String.cpp:17:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Cryptography/Digest/Algorithm/SuperFastHash.h:11:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Cryptography/Digest/Algorithm/../Digester.h:12:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/BLOB.h:454:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/BLOB.inl:22:59: error: out-of-line declaration of 'AsBase64' does not match any declaration in 'Stroika::Foundation::Memory::BLOB'
   22 |     Characters::String Stroika::Foundation::Memory::BLOB::AsBase64 () const;
      |                                                           ^~~~~~~~
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/BLOB.inl:24:59: error: out-of-line declaration of 'AsBase64' does not match any declaration in 'Stroika::Foundation::Memory::BLOB'
   24 |     Characters::String Stroika::Foundation::Memory::BLOB::AsBase64 (const Cryptography::Encoding::Algorithm::Base64::Options& o) const;
      |                                                           ^~~~~~~~
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/BLOB.inl:201:43: error: out-of-line declaration of 'As' does not match any declaration in 'Stroika::Foundation::Memory::BLOB'
  201 |     Streams::InputStream::Ptr<byte> BLOB::As () const;
      |                                           ^~

*/
#ifndef qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy

#if defined(__clang__) && defined(__APPLE__)
// first noticed broken in apply clang 15
#define qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy                                                         \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// first noticed broken in apply clang 16
//  broken in  clang 17
// broken in  clang 18
#define qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy                                                         \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
#else
#define qCompilerAndStdLib_template_requires_doesnt_work_with_specialization_Buggy 0
#endif

#endif

/**
*  NO IDEA if this is LGP bug or compiler bug.

I don't understand why we need the explicit String version and requires not same_as<> string template versions (want just the template version)
*          But when I try that, clang and visaul studio barf with ...
* 
            1>C:\Sandbox\Stroika\TMPV3\Library\Sources\Stroika\Foundation\DataExchange\InternetMediaTypeRegistry.cpp(670): error C2666: 'Stroika::Foundation::Characters::String::operator ==': overloaded functions have similar conversions
            1>C:\Sandbox\Stroika\TMPV3\Library\Sources\Stroika\Foundation\Characters\String.inl(1124): note: could be 'bool Stroika::Foundation::Characters::String::operator ==<Stroika::Foundation::Characters::String>(T &&) const'
            1>        with
            1>        [
            1>            T=Stroika::Foundation::Characters::String
            1>        ]
            1>C:\Sandbox\Stroika\TMPV3\Library\Sources\Stroika\Foundation\Characters\String.inl(1124): note: or 'bool Stroika::Foundation::Characters::String::operator ==<Stroika::Foundation::Characters::String>(T &&) const' [synthesized expression 'y == x']
* 



                 from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ProcessRunner.h:21,
                 from SystemConfiguration.cpp:48:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ProgressMonitor.inl: In member function ‘void Stroika::Foundation::Execution::ProgressMonitor::Updater::SetCurrentTaskInfo(const Stroika::Foundation::Execution::ProgressMonitor::CurrentTaskInfo&)’:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ProgressMonitor.inl:149:68: error: use of deleted function ‘constexpr bool Stroika::Foundation::Execution::ProgressMonitor::CurrentTaskInfo::operator==(const Stroika::Foundation::Execution::ProgressMonitor::CurrentTaskInfo&) const’
  149 |             bool changed             = fRep_->fCurrentTaskInfo_ != taskInfo;
      |                                                                    ^~~~~~~~
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ProgressMonitor.h:245:25: note: ‘constexpr bool Stroika::Foundation::Execution::ProgressMonitor::CurrentTaskInfo::operator==(const Stroika::Foundation::Execution::ProgressMonitor::CurrentTaskInfo&) const’ is implicitly deleted because the default definition would be ill-formed:
  245 |         nonvirtual bool operator== (const CurrentTaskInfo& rhs) const = default;
      |                         ^~~~~~~~
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ProgressMonitor.h:245:25: warning: C++20 says that these are ambiguous, even though the second is reversed:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/String.h:1885,
                 from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/FloatConversion.h:15,
                 from SystemConfiguration.cpp:24:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/String.inl:1131:17: note: candidate 1: ‘bool Stroika::Foundation::Characters::String::operator==(T&&) const [with T = Stroika::Foundation::Characters::String&]’
 1131 |     inline bool String::operator== (T&& rhs) const
      |                 ^~~~~~


ing.cpp:1073:23: warning: ISO C++20 considers use of overloaded operator '==' (with operand types 'String' and 'String') to be ambiguous despite there being a unique best viable function [-Wambiguous-reversed-operator]
 1073 |     Ensure (sb.str () == SubString (0, from) + replacement + SubString (to));
      |             ~~~~~~~~~ ^  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/
*/
#ifndef qCompilerAndStdLib_CompareOpReverse_Buggy

#if defined(__clang__) && defined(__APPLE__)
// replicated in xcode 15
#define qCompilerAndStdLib_CompareOpReverse_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__)
// reproduced in clang 16
// reproduced in clang 17
// reproduced in clang 18
#define qCompilerAndStdLib_CompareOpReverse_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
#elif defined(__GNUC__) && !defined(__clang__)
// FIRST SEEN BROKEN IN GCC 13 (so manybe really MY BUG and not compiler bug, but I still don't get it...)
// Still broken in GCC 14
#define qCompilerAndStdLib_CompareOpReverse_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ == 13 || __GNUC__ == 14)
#elif defined(_MSC_VER)
// Still Broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_CompareOpReverse_Buggy                                                                                          \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k22_17Pt4_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k22_17Pt9_)
#else
#define qCompilerAndStdLib_CompareOpReverse_Buggy 0
#endif

#endif

/*
   Linking Test Builds/Release/Tests/Test02 (Foundation::Characters) ... 
/usr/bin/ld: /tmp/ccp5kDeT.ltrans5.ltrans.o: in function `Stroika::Foundation::Characters::CharacterEncodingException::CharacterEncodingException(Stroika::Foundation::Characters::CharacterEncodingException::EncodingOrDecoding, std::optional<unsigned long>, std::optional<Stroika::Foundation::Characters::String> const&)':
/__w/Stroika/Stroika/Tests/03/../../Library/Sources/Stroika/Foundation/Characters/StringBuilder.h:170:(.text+0x1f66): undefined reference to `Stroika::Foundation::Characters::String Stroika::Foundation::Characters::UnoverloadedToString<unsigned long>(unsigned long const&)'
collect2: error: ld returned 1 exit status
make[4]: *** [/__w/Stroika/Stroika//Tests/Makefile-Test-Template.mk:49: /__w/Stroika/Stroika/Builds/Release/Tests/Test03] Error 1
make[3]: *** [Makefile:179: 03] Error 2
make[3]: *** Waiting for unfinished jobs....
/usr/bin/ld: /tmp/cca4u1A2.ltrans20.ltrans.o: in function `Stroika::Foundation::Characters::CharacterEncodingException::CharacterEncodingException(Stroika::Foundation::Characters::CharacterEncodingException::EncodingOrDecoding, std::optional<unsigned long>, std::optional<Stroika::Foundation::Characters::String> const&)':
/__w/Stroika/Stroika/Tests/02/../../Library/Sources/Stroika/Foundation/Characters/StringBuilder.h:170:(.text+0x5086): undefined reference to `Stroika::Foundation::Characters::String Stroika::Foundation::Characters::UnoverloadedToString<unsigned long>(unsigned long const&)'
collect2: error: ld returned 1 exit status
make[4]: *** [/__w/Stroika/Stroika//Tests/Makefile-Test-Template.mk:49

        NOTE - maybe same issue as qCompilerAndStdLib_release_bld_error_bad_obj_offset_Buggy
*/
#ifndef qCompilerAndStdLib_LTOForgetsAnInlineSometimes_Buggy

#if defined(__clang__)
// seen on apply clang++ 15
#define qCompilerAndStdLib_LTOForgetsAnInlineSometimes_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__GNUC__)
// FIRST SEEN BROKEN IN GCC 13 and 12 (so manybe really MY BUG and not compiler bug, but I still don't get it...)
#define qCompilerAndStdLib_LTOForgetsAnInlineSometimes_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 13)
#else
#define qCompilerAndStdLib_LTOForgetsAnInlineSometimes_Buggy 0
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

/usr/bin/ld: /tmp/lto-llvm-d35bdb.o: in function `Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly(double)':
ld-temp.o:(.text._ZN7Stroika10Foundation9Execution13WaitableEvent3WE_16WaitUntilQuietlyEd+0xe3): undefined reference to `Stroika::Foundation::Execution::Thread::IsCurrentThreadInterruptible()'
clang: error: linker command failed with exit code 1 (use -v to see invocation)
make[5]: *** [Makefile:47: /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2310_x86_64/Builds/clang++-16-release-libc++23/Samples-SSDPClient/SSDPClient] Error 1
make[4]: *** [Makefile:29: all] Error 2


SIMILAR BUT SLIGHTYL DIFF ISSUE ON GCC
.. 
/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/valgrind-release-SSLPurify-NoBlockAlloc/Stroika-Foundation.a(WaitableEvent.o): in function `bool Stroika::Foundation::Execution::ConditionVariable<std::mutex, std::_V2::condition_variable_any>::wait_until<Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly(double)::{lambda()#1}>(std::unique_lock<std::mutex>&, double, Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly(double)::{lambda()#1}&&) [clone .constprop.0]':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ConditionVariable.inl:111: undefined reference to `Stroika::Foundation::Execution::Thread::GetCurrentThreadStopToken()'
/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/valgrind-release-SSLPurify-NoBlockAlloc/Stroika-Foundation.a(WaitableEvent.o): in function `Stroika::Foundation::Execution::ConditionVariable<std::mutex, std::_V2::condition_variable_any>::wait_until(std::unique_lock<std::mutex>&, double)':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/ConditionVaria



   Linking Test Builds/c/Tests/Test04 (Foundation::Configuration) ... 
/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/c/Stroika-Foundation.a(WaitableEvent.o): in function `Stroika::Foundation::Execution::ConditionVariable<std::__1::mutex, std::__1::condition_variable_any>::wait_until(std::__1::unique_lock<std::__1::mutex>&, std::__1::chrono::time_point<std::__1::chrono::steady_clock, std::__1::chrono::duration<double, std::__1::ratio<1l, 1l> > >)':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Execution/./ConditionVariable.inl:84:(.text._ZN7Stroika10Foundation9Execution17ConditionVariableINSt3__15mutexENS3_22condition_variable_anyEE10wait_untilERNS3_11unique_lockIS4_EENS3_6chrono10time_pointINSA_12steady_clockENSA_8durationIdNS3_5ratioILl1ELl1EEEEEEE[_ZN7Stroika10Foundation9Execution17ConditionVariableINSt3__15mutexENS3_22condition_variable_anyEE10wait_untilERNS3_11unique_lockIS4_EENS3_6chrono10time_pointINSA_12steady_clockENSA_8durationIdNS3_5ratioILl1ELl1EEEEEEE]+0x3e): undefined reference to `Stroika::Foundation::Execution::Thread::IsCurrentThreadInterruptible()'

*/
#ifndef qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy

#if defined(__clang__) && defined(__APPLE__)
// first noticed broken in apply clang 14
// replicated in xcode 15.0
#define qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__)
// first noticed broken in apply clang 14
// replicated in clang 15.
// reproduced in clang 16
// reproduced in clang 17
// release only bug - (lto?)- clang++18
#define qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
#elif defined(__GNUC__) && !defined(__clang__)
// FIRST SEEN BROKEN IN GCC 11
#define qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 11)
#else
#define qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy 0
#endif

#endif

/*
/usr/bin/ld: /tmp/cchyvHxb.ltrans6.ltrans.o: in function `Stroika::Foundation::Debug::BackTrace::Capture[abi:cxx11](Stroika::Foundation::Debug::BackTrace::Options const&) [clone .constprop.0]':
/usr/include/c++/13/stacktrace:196:(.text+0x844e): undefined reference to `__glibcxx_backtrace_pcinfo'
/usr/bin/ld: /usr/include/c++/13/stacktrace:196:(.text+0x84cd): undefined reference to `__glibcxx_backtrace_pcinfo'
/usr/bin/ld: /tmp/cchyvHxb.ltrans6.ltrans.o:/usr/include/c++/13/stacktrace:206:(.text+0x8914): undefined reference to `__glibcxx_backtrace_syminfo'
/usr/bin/ld: /tmp/cchyvHxb.ltrans6.ltrans.o: in function `Stroika::Foundation::Debug::BackTrace::Capture[abi:cxx11](Stroika::Foundation::Debug::BackTrace::Options const&) [clone .constprop.0]':
/usr/include/c++/13/stacktrace:196:(.text+0x89b0): undefined reference to `__glibcxx_backtrace_pcinfo'
/usr/bin/ld: /usr/include/c++/13/stacktrace:164:(.text+0x8a83): undefined reference to `__glibcxx_backtrace_create_state'
/usr/bin/ld: /usr/include/c++/13/stacktrace:164:(.text+0x8c8e): undefined reference to `__glibcxx_backtrace_create_state'
/usr/bin/ld: /usr/include/c++/13/stacktrace:164:(.text+0x8d41): undefined reference to `__glibcxx_backtrace_create_state'
/usr/bin/ld: /tmp/cchyvHxb.ltrans7.ltrans.o: in function `std::basic_stacktrace<std::allocator<std::stacktrace_entry> >::current(std::allocator<std::stacktrace_entry> const&) [clone .isra.0]':
/usr/include/c++/13/stacktrace:259:(.text+0x2f9): undefined reference to `__glibcxx_backtrace_simple'
/usr/bin/ld: /usr/include/c++/13/stacktrace:164:(.text+0x344): undefined reference to `__glibcxx_backtrace_create_state'
collect2: error: ld returned 1 exit status
                #   https://gcc.gnu.org/pipermail/gcc-bugs/2022-May/787733.html 
                #       says use '-lstdc++_libbacktrace' but doesn't exist on ubuntu 24.04 -LGP 24.04
*/
#ifndef qCompilerAndStdLib_stacktraceLinkError_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// Only SEEN BROKEN IN GCC 13, Ubuntu 24.04, but cannot test for that right easily... (ALSO - PRE-RELEASE UBUTNU - SO TRY AGAIN WHEN RELEASED)
// Same issue on GCC-14 and Ubuntu 24.04
#define qCompilerAndStdLib_stacktraceLinkError_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ == 13 || __GNUC__ == 14)
#else
#define qCompilerAndStdLib_stacktraceLinkError_Buggy 0
#endif

#endif

/*
 Symptom if broken - is that for sanitizer=undefined builds, we still get kBuiltWithUndefinedBehaviorSanitizer == false
 especially relevant in tests for qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy workarounds!
 */
#ifndef qCompilerAndStdLib_undefined_behavior_macro_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// FIRST SEEN BROKEN IN GCC 11
// Still broken in GCC 12
#define qCompilerAndStdLib_undefined_behavior_macro_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 12)
#else
#define qCompilerAndStdLib_undefined_behavior_macro_Buggy 0
#endif

#endif

/***
 GCC compiler gcrash on Ubuntu gcc11 on WSL
 */
#ifndef qCompilerAndStdLib_crash_compiling_break_in_forLoop_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// FIRST SEEN BROKEN IN GCC 11
#define qCompilerAndStdLib_crash_compiling_break_in_forLoop_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 11)
#else
#define qCompilerAndStdLib_crash_compiling_break_in_forLoop_Buggy 0
#endif

#endif

/*

raspberrypi-g++-11-debug-sanitize_undefined - running on raspi

 UTFConvert.cpp:769:119: runtime error: reference binding to misaligned address 0x014a84e5 for type '<unknown>', which requires 2 byte alignment
0x014a84e5: note: pointer points here
 37 5c 02 b0 b5 8c b0  02 af f8 60 b9 60 7a 60  3b 60 00 23 bb 61 fb 68  fa 68 00 2a 03 d0 03 f0  03
             ^ 
*/
#ifndef qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy

#if defined(__GNUC__) && !defined(__clang__) && defined(__arm__)
// FIRST SEEN BROKEN IN GCC 11
// appears broken in GCC 12 (ubuntu 22.04 cross-compile)
#define qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 12)
#else
#define qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy 0
#endif

#endif

/*

raspberrypi-g++-11-debug-sanitize_undefined - running on raspi

==16331==ERROR: AddressSanitizer: stack-use-after-scope on address 0xbeb33b80 at pc 0x0049c10f bp 0xbeb33688 sp 0xbeb33694
WRITE of size 100 at 0xbeb33b80 thread T0
    #0 0x49c10c in __interceptor_memcpy (/tmp/Test43+0x6c10c)
    #1 0x6289ee in memcpy /usr/arm-linux-gnueabihf/include/bits/string_fortified.h:29
    #2 0x6289ee in Stroika::Foundation::Debug::Private_::Emitter::DoEmit_(wchar_t const*, wchar_t const*) /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:549
    #3 0x6336c8 in int Stroika::Foundation::Debug::Private_::Emitter::DoEmitMessage_<wchar_t>(unsigned int, wchar_t const*, wchar_t const*) /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:431
    #4 0x62928e in Stroika::Foundation::Debug::Private_::Emitter::EmitTraceMessage(wchar_t const*, ...) /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:299
    #5 0x6298ba in Stroika::Foundation::Debug::TraceContextBumper::~TraceContextBumper() /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:641
    #6 0x6298ba in Stroika::Foundation::Debug::TraceContextBumper::~TraceContextBumper() /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:630
    #7 0x5523cc in DoTests_ /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Tests/43/Test.cpp:376
    #8 0x55cb52 in DoRegressionTests_ /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Tests/43/Test.cpp:568
    #9 0x57ac3c in Stroika::TestHarness::PrintPassOrFail(void (*)()) ../TestHarness/TestHarness.cpp:89
    #10 0xb6d5a3bc in __libc_start_call_main ../sysdeps/nptl/libc_start_call_main.h:58
    #11 0xb6d5a4c4 in __libc_start_main_impl csu/libc-start.c:360

Address 0xbeb33b80 is located in stack of thread T0 at offset 128 in frame
    #0 0x6288e8 in Stroika::Foundation::Debug::Private_::Emitter::DoEmit_(wchar_t const*, wchar_t const*) /home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2204-Cross-Compile2RaspberryPi/Library/Sources/Stroika/Foundation/Debug/Trace.cpp:545

  This frame has 1 object(s):
    [48, 4152) 'buf' (line 548) <== Memory access at offset 128 is inside this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism, swapcontext or vfork



ALSO SPERAETE BUG NOT REGULARLY TESTED COFNIG:
            <ConfigureCommandLine>configure raspberrypi-g++-12-debug-sanitize_address --config-tag Unix --config-tag raspberrypi --apply-default-debug-flags --only-if-has-compiler --trace2file enable --compiler-driver arm-linux-gnueabihf-g++-12 --sanitize none,address --cross-compiling true</ConfigureCommandLine>

lewis@raspberrypi:/tmp $ ./Test34
[==========] Running 19 tests from 1 test suite.
[----------] ...
[ RUN      ] Foundation_DataExchange_XML.SAX_PARSER_SchemaValidate
=================================================================
==7910==ERROR: AddressSanitizer: stack-use-after-scope on address 0xbeba8280 at pc 0x017ab839 bp 0xbeba8118 sp 0xbeba8118
WRITE of size 4 at 0xbeba8280 thread T0
    #0 0x17ab836 in Stroika::Foundation::Memory::InlineBuffer<wchar_t, 1024u>::InlineBuffer() ../Memory/InlineBuffer.inl:33
    #1 0x19f0ea4 in Stroika::Foundation::Characters::CodeCvt<wchar_t>::CodeCvt_WrapStdCodeCvt_<Stroika::Foundation::Characters::Private_::deletable_facet_<std::codecvt_byname<wchar_t, char, __mbstate_t> > >::CodeCvt_WrapStdCodeCvt_(Stroika::Foundation::Characters::CodeCvt<wchar_t>::Options const&, std::unique_ptr<Stroika::Foundation:
   ....
*/

#ifndef qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy

#if defined(__GNUC__) && !defined(__clang__) && defined(__arm__)
// FIRST SEEN BROKEN IN GCC 11
// appears broken in GCC 12 (ubuntu 22.04 cross-compile)
#define qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 12)
#else
#define qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy 0
#endif

#endif

/**
In file included from ObjectVariantMapper.h:1067,
                 from ObjectVariantMapper.cpp:22:
ObjectVariantMapper.inl: In instantiation of ‘static Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer_(const std::optional<_Tp>*) [with T = bool]’:
ObjectVariantMapper.inl:500:54:   required from ‘static Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer(ARGS&& ...) [with T = std::optional<bool>; ARGS = {}]’
ObjectVariantMapper.cpp:430:71:   required from here
ObjectVariantMapper.inl:584:47: internal compiler error: in tsubst_copy, at cp/pt.c:16891
  584 |             if (d.GetType () == VariantValue::eNull) {
      |                                 ~~~~~~~~~~~~~~^~~~~
Please submit a full bug report,
with preprocessed source if appropriate.
See <file:///usr/share/doc/gcc-11/README.Bugs> for instructions.
*/
#ifndef qCompilerAndStdLib_InternalCompilerErrorTSubCopy_Buggy

#if defined(__GNUC__) && !defined(__clang__)
#define qCompilerAndStdLib_InternalCompilerErrorTSubCopy_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 11)
#else
#define qCompilerAndStdLib_InternalCompilerErrorTSubCopy_Buggy 0
#endif

#endif

/**
/usr/bin/ld: /mnt/c/Sandbox/Stroika/DevRoot/IntermediateFiles/Debug-unix/Tests/48/Test.o: in function `(anonymous namespace)::Test_7_BLOB_()':
/mnt/c/Sandbox/Stroika/DevRoot/Tests/48/Test.cpp:262: undefined reference to `Stroika::Foundation::Characters::String Stroika::Foundation::Memory::BLOB::AsBase64<Stroika::Foundation::Characters::String>() const'
   Test 51: Foundation::Traversal:
      Compiling Tests/51/Test.cpp ...
      */
#ifndef qCompilerAndStdLib_specializeDeclarationRequiredSometimesToGenCode_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// FIRST SEEN BROKEN IN GCC 11
#define qCompilerAndStdLib_specializeDeclarationRequiredSometimesToGenCode_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 11)
#else
#define qCompilerAndStdLib_specializeDeclarationRequiredSometimesToGenCode_Buggy 0
#endif

#endif

/*

./../../Containers/Association.h:647:9: error: type constraint differs in template redeclaration



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
// broken in clang++18
#define qCompilerAndStdLib_RequiresIEqualsCrashesAssociation_Buggy                                                                         \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15) || (__clang_major__ == 18))
#else
#define qCompilerAndStdLib_RequiresIEqualsCrashesAssociation_Buggy 0
#endif

#endif

/***
 *     Compiling Library/Sources/Stroika/Foundation/Debug/BackTrace.cpp ... 
In file included from BackTrace.cpp:24:
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:595:3: error: no matching function for call to 'operator delete'
  595 |                 _GLIBCXX_OPERATOR_DELETE (static_cast<void*>(_M_frames),
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  596 |                                           _M_capacity * sizeof(value_type));
      |                                           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:550:35: note: expanded from macro '_GLIBCXX_OPERATOR_DELETE'
  550 | # define _GLIBCXX_OPERATOR_DELETE __builtin_operator_delete
      |                                   ^

*/
#ifndef qCompilerAndStdLib_StdBacktraceCompile_Buggy

#if defined(__clang__) && !defined(__APPLE__) && defined(__GLIBCXX__)
// broken in clang++-18 with
#define qCompilerAndStdLib_StdBacktraceCompile_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
#else
#define qCompilerAndStdLib_StdBacktraceCompile_Buggy 0
#endif

#endif

/*
 file included from AssertExternallySynchronizedMutex.cpp:6:
./../Characters/ToString.h:258:16: error: static assertion failed
  258 | static_assert (std::formattable<std::thread::id, wchar_t>);
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
./../Characters/ToString.h:258:16: note: because 'std::formattable<std::thread::id, wchar_t>' evaluated to false
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/format:2525:9: note: because '__format::__formattable_impl<remove_reference_t<id>, wchar_t>' evaluated to false
 2525 |       = __format::__formattable_impl<remove_reference_t<_Tp>, _CharT>;
      |         ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/format:2516:9: note: because '__parsable_with<std::thread::id, std::basic_format_context<std::back_insert_iterator<std::basic_string<wchar_t> >, wchar_t> >' evaluated to false
 2516 |       = __parsable_with<_Tp, _Context> && __formattable_with<_Tp, _Context>;
      |         ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/format:2491:9: note: because 'std::formatter<std::thread::id, wchar_t>' does not satisfy 'semiregular'
 2491 |       = semiregular<_Formatter>
      |         ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/concepts:280:27: note: because 'std::formatter<std::thread::id, wchar_t>' does not satisfy 'copyable'
  280 |     concept semiregular = copyable<_Tp> && default_initializable<_Tp>;
      |                           ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/concepts:275:24: note: because 'std::formatter<std::thread::id, wchar_t>' does not satisfy 'copy_constructible'
  275 |     concept copyable = copy_constructible<_Tp> && movable<_Tp>
      |                        ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/concepts:179:9: note: because 'std::formatter<std::thread::id, wchar_t>' does not satisfy 'move_constructible'
  179 |       = move_constructible<_Tp>
      |         ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/concepts:174:7: note: because 'constructible_from<std::formatter<std::thread::id, wchar_t>, std::formatter<std::thread::id, wchar_t> >' evaluated to false
  174 |     = constructible_from<_Tp, _Tp> && convertible_to<_Tp, _Tp>;
      |       ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/concepts:160:9: note: because 'std::formatter<std::thread::id, wchar_t>' does not satisfy 'destructible'
  160 |       = destructible<_Tp> && is_constructible_v<_Tp, _Args...>;
      |         ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/concepts:155:28: note: because '__detail::__destructible<std::formatter<std::thread::id, wchar_t> >' evaluated to false
  155 |     concept destructible = __detail::__destructible<_Tp>;
      |                            ^
In file included from Thread.cpp:18:
./../Characters/ToString.h:258:16: error: static assertion failed
  258 | static_assert (std::formattable<std::thread::id, wchar_t>);
      |                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
./../Characters/ToString.h:258:16: note: because 'std::formattable<std::thread::id, wchar_t>' evaluated to false
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/format:2525:9: note: because '__format::__formattable_impl<remove_reference_t<id>, wchar_t>' evaluated to false
 2525 |       = __format::__formattable_impl<remove_reference_t<_Tp>, _CharT>;
      |         ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/format:2516:9: note: because '__parsable_with<std::thread::id, std::basic_format_context<std::back_insert_iterator<std::basic_string<wchar_t> >, wchar_t> >' evaluated to false
 2516 |       = __parsable_with<_Tp, _Context> && __formattable_with<_Tp, _Context>;
      |         ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/format:2491:9: note: because 'std::formatter<std::thread::id, wchar_t>' does not satisfy 'semiregular'
 2491 |       = semiregular<_Formatter>
      |         ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/concepts:280:27: note: because 'std::formatter<std::thread::id, wchar_t>' does not satisfy 'copyable'
  280 |     concept semiregular = copyable<_Tp> && default_initializable<_Tp>;
  
  */
#ifndef qCompilerAndStdLib_FormatThreadId_Buggy

#if defined(__clang__) && defined(__GLIBCXX__)
// broken in clang++-18 with
#define qCompilerAndStdLib_FormatThreadId_Buggy 1
//#define qCompilerAndStdLib_FormatThreadId_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__GLIBCXX__ <= 20240315))
#else
#define qCompilerAndStdLib_FormatThreadId_Buggy 0
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
// still broken in _MSC_VER_2k22_17Pt8_
// still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy                                                                    \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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
// VERIFIED BROKEN IN GCC 11
// VERIFIED BROKEN IN GCC 12
// VERIFIED BROKEN IN GCC 13
// VERIFIED BROKEN IN GCC 14
#define qCompilerAndStdLib_template_DefaultArgIgnoredWhenFailedDeduction_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 14)
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
// VERIFIED BROKEN IN GCC 14
#define qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy                                                     \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 14)
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
In file included from Test.cpp:12:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/SparseDataHyperRectangle_stdmap.h:6:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/../SparseDataHyperRectangle.h:78:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/../SparseDataHyperRectangle.inl:7:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/../Factory/SparseDataHyperRectangle_Factory.h:109:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/../Factory/SparseDataHyperRectangle_Factory.inl:14:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/SparseDataHyperRectangle_stdmap.h:74:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/SparseDataHyperRectangle_stdmap.inl:116:21: warning: macro expansion producing 'defined' has undefined behavior [-Wexpansion-to-defined]
  116 |                 #if qCompilerAndStdLib_template_map_tuple_insert_Buggy && 0
      |                     ^
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Configuration/Private/Defaults_CompilerAndStdLib_.h:1278:132: note: expanded from macro 'qCompilerAndStdLib_template_map_tuple_insert_Buggy'
 1278 | #define qCompilerAndStdLib_template_map_tuple_insert_Buggy  CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ == 17) and defined (__GLIBCXX__)
      |                                                                                                                                    ^
In file included from Test.cpp:8:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/iostream:41:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/ostream:40:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/ios:44:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/ios_base.h:41:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/locale_classes.h:40:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/string:58:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/memory_resource.h:41:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/uses_allocator_args.h:38:
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:691:2: error: pack expansion contains parameter pack '_UTypes' that has a different length (1 vs. 2) from outer parameter packs
  691 |         using __convertible = __and_<is_convertible<_UTypes, _Types>...>;
      |         ^~~~~
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:853:27: note: in instantiation of template type alias '__convertible' requested here
  853 |           = _TCC<true>::template __convertible<_Args...>::value;
      |                                  ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:948:12: note: in instantiation of static data member 'std::tuple<std::tuple<unsigned long, unsigned long> &&>::__convertible<unsigned long &, unsigned long &>' requested here
  948 |         explicit(!__convertible<_UElements&...>)
      |                   ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:2000:36: note: while substituting deduced template arguments into function template 'tuple' [with _UElements = <unsigned long, unsigned long>]
 2000 |     { return tuple<_Elements&&...>(std::forward<_Elements>(__args)...); }
      |                                    ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/stl_map.h:992:11: note: in instantiation of function template specialization 'std::forward_as_tuple<std::tuple<unsigned long, unsigned long>>' requested here
  992 |                                  std::forward_as_tuple(std::move(__k)),
      |                                       ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/debug/map.h:406:24: note: in instantiation of function template specialization 'std::__cxx1998::map<std::tuple<unsigned long, unsigned long>, int>::insert_or_assign<int &>' requested here
  406 |           auto __res = _Base::insert_or_assign(std::move(__k),
      |                               ^
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/SparseDataHyperRectangle_stdmap.inl:124:28: note: (skipping 5 contexts in backtrace; use -ftemplate-backtrace-limit=0 to see all)
  124 |                     fData_.insert_or_assign (tuple<INDEXES...>{indexes...}, v);
      |                            ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/shared_ptr.h:464:4: note: in instantiation of function template specialization 'std::__shared_ptr<Stroika::Foundation::Containers::Concrete::SparseDataHyperRectangle_stdmap<int, unsigned long, unsigned long>::Rep_>::__shared_ptr<Stroika::Foundation::Memory::BlockAllocator<Stroika::Foundation::Containers::Concrete::SparseDataHyperRectangle_stdmap<int, unsigned long, unsigned long>::Rep_>, int &>' requested here
  464 |         : __shared_ptr<_Tp>(__tag, std::forward<_Args>(__args)...)
      |           ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/shared_ptr.h:992:14: note: in instantiation of function template specialization 'std::shared_ptr<Stroika::Foundation::Containers::Concrete::SparseDataHyperRectangle_stdmap<int, unsigned long, unsigned long>::Rep_>::shared_ptr<Stroika::Foundation::Memory::BlockAllocator<Stroika::Foundation::Containers::Concrete::SparseDataHyperRectangle_stdmap<int, unsigned long, unsigned long>::Rep_>, int &>' requested here
  992 |       return shared_ptr<_Tp>(_Sp_alloc_shared_tag<_Alloc>{__a},
      |              ^
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/BlockAllocated.inl:63:20: note: in instantiation of function template specialization 'std::allocate_shared<Stroika::Foundation::Containers::Concrete::SparseDataHyperRectangle_stdmap<int, unsigned long, unsigned long>::Rep_, Stroika::Foundation::Memory::BlockAllocator<Stroika::Foundation::Containers::Concrete::SparseDataHyperRectangle_stdmap<int, unsigned long, unsigned long>::Rep_>, int &>' requested here
   63 |             return allocate_shared<T> (BlockAllocator<T>{}, forward<ARGS_TYPE> (args)...);
      |                    ^
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Concrete/SparseDataHyperRectangle_stdmap.inl:207:29: note: in instantiation of function template specialization 'Stroika::Foundation::Memory::MakeSharedPtr<Stroika::Foundation::Containers::Concrete::SparseDataHyperRectangle_stdmap<int, unsigned long, unsigned long>::Rep_, int &>' requested here
  207 |         : inherited{Memory::MakeSharedPtr<Rep_> (defaultItem)}
      |                             ^
Test.cpp:51:49: note: in instantiation of member function 'Stroika::Foundation::Containers::Concrete::SparseDataHyperRectangle_stdmap<int, unsigned long, unsigned long>::SparseDataHyperRectangle_stdmap' requested here
   51 |                 DataHyperRectangleN<int, 2> x = Concrete::SparseDataHyperRectangle_stdmapN<int, 2>{};
      |                                                 ^
In file included from Test.cpp:8:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/iostream:41:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/ostream:40:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/ios:44:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/ios_base.h:41:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/locale_classes.h:40:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/string:58:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/memory_resource.h:41:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/bits/uses_allocator_args.h:38:
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:691:2: error: pack expansion contains parameter pack '_UTypes' that has a different length (1 vs. 2) from outer parameter packs
  691 |         using __convertible = __and_<is_convertible<_UTypes, _Types>...>;
      |         ^~~~~
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:853:27: note: in instantiation of template type alias '__convertible' requested here
  853 |           = _TCC<true>::template __convertible<_Args...>::value;
      |                                  ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:959:12: note: in instantiation of static data member 'std::tuple<std::tuple<unsigned long, unsigned long> &&>::__convertible<const unsigned long, const unsigned long>' requested here
  959 |         explicit(!__convertible<const _UElements...>)
      |                   ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/tuple:2000:36: note: while substituting deduced template arguments into function template 'tuple' [with _UElements = <unsigned long, unsigned long>]
 2000 |     { return tuple<_Elements&&...>(std::forward<_Elements>(__args)...); }
 
*/
#ifndef qCompilerAndStdLib_template_map_tuple_insert_Buggy

#if defined(__clang__) && defined(__GLIBCXX__)
#define qCompilerAndStdLib_template_map_tuple_insert_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ == 17)
#else
#define qCompilerAndStdLib_template_map_tuple_insert_Buggy 0
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

#ifndef qCompilerAndStdLib_ITimepointConfusesFormatWithFloats_Buggy
#if defined(__clang__)
// Seen on clang++-18 on ubuntu
#define qCompilerAndStdLib_ITimepointConfusesFormatWithFloats_Buggy (__clang_major__ <= 18)
#else
#define qCompilerAndStdLib_ITimepointConfusesFormatWithFloats_Buggy 0
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
#elif defined(__clang__) && !defined(__APPLE__) && defined(_LIBCPP_VERSION)
// Noticed broken in -clang++14
// noticed broken in clang++15 with LIBC++
// noticed broken in clang++17 with LIBC++
//  clang++18 with LIBC++
#define qCompilerAndStdLib_template_Requires_constraint_not_treated_constexpr_Buggy                                                        \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ <= 18)
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



  Compiling Library/Sources/Stroika/Foundation/Cache/Statistics.cpp ... 
In file included from Statistics.cpp:6:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.h:18:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/String.h:16:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Sequence.h:16:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Traversal/Iterable.h:1603:9: error: type constraint differs in template redeclaration
 1603 |         Common::IEqualsComparer<T>
      |         ^
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Traversal/Iterable.h:442:13: note: previous template declaration is here
  442 |             Common::IEqualsComparer<T>
      |             ^
In file included from Statistics.cpp:6:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.h:18:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/String.h:16:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Sequence.h:16:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Traversal/Iterable.h:1634:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Traversal/Iterable.inl:1198:28: error: incomplete type 'SequentialEqualsComparer' named in nested name specifier
 1198 |     constexpr Iterable<T>::SequentialEqualsComparer<T_EQUALS_COMPARER>::SequentialEqualsComparer (const T_EQUALS_COMPARER& elementEqualsComparer,
      |               ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Traversal/Iterable.inl:1212:75: error: out-of-line definition of 'operator()' from class 'SequentialEqualsComparer<T_EQUALS_COMPARER>' without definition
 1212 |     inline bool Iterable<T>::SequentialEqualsComparer<T_EQUALS_COMPARER>::operator() (const Iterable& lhs, const Iterable& rhs) const
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Traversal/Iterable.inl:1214:34: error: use of undeclared identifier 'lhs'
 1214 |         return SequentialEquals (lhs, rhs, fElementComparer, fUseIterableSize);
      |                                  ^
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Traversal/Iterable.inl:1214:39: error: use of undeclared identifier 'rhs'
 1214 |         return SequentialEquals (lhs, rhs, fElementComparer, fUseIterableSiz


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
// broken in clang++16
#define qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy                                                      \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15) or (__clang_major__ == 18))
#else
#define qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy 0
#endif

#endif

/*
 file included from /usr/lib/llvm-18/bin/../include/c++/v1/__type_traits/is_nothrow_destructible.h:14:
/usr/lib/llvm-18/bin/../include/c++/v1/__type_traits/is_destructible.h:28:61: error: ambiguous partial specializations of 'formatter<int, wchar_t>'
   28 | struct _LIBCPP_TEMPLATE_VIS is_destructible : _BoolConstant<__is_destructible(_Tp)> {};
      |                                                             ^
/usr/lib/llvm-18/bin/../include/c++/v1/__type_traits/is_nothrow_destructible.h:41:47: note: in instantiation of template class 'std::is_destructible<std::formatter<int, wchar_t>>' requested here
   41 |     : public __libcpp_is_nothrow_destructible<is_destructible<_Tp>::value, _Tp> {};
      |                                               ^
/usr/lib/llvm-18/bin/../include/c++/v1/__type_traits/is_nothrow_destructible.h:69:51: note: in instantiation of template class 'std::is_nothrow_destructible<std::formatter<int, wchar_t>>' requested here
   69 | inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<_Tp>::value;
      |                                                   ^
/usr/lib/llvm-18/bin/../include/c++/v1/__concepts/destructible.h:26:24: note: in instantiation of variable template specialization 'std::is_nothrow_destructible_v<std::formatter<int, wchar_t>>' requested here
   26 | concept destructible = is_nothrow_destructible_v<_Tp>;
      |                        ^
/usr/lib/llvm-18/bin/../include/c++/v1/__concepts/destructible.h:26:24: note: while substituting template arguments into constraint expression here
   26 | concept destructible = is_nothrow_destructible_v<_Tp>;
      |                        ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/lib/llvm-18/bin/../include/c++/v1/__concepts/constructible.h:27:30: note: while checking the s
*/
#ifndef qCompilerAndStdLib_template_concept_matcher_requires_Buggy
#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_template_concept_matcher_requires_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// Noticed broken in -clang++17
// Noticed broken in -clang++18
#define qCompilerAndStdLib_template_concept_matcher_requires_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
#else
#define qCompilerAndStdLib_template_concept_matcher_requires_Buggy 0
#endif

#endif

/**
 * In file included from /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/type_traits:510:
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/__type_traits/is_trivially_destructible.h:25:38: error: incomplete type 'Stroika::Foundation::Characters::String' used in type trait expression
    : public integral_constant<bool, __is_trivially_destructible(_Tp)> {};
                                     ^
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/optional:579:5: note: in instantiation of template class 'std::is_trivially_destructible<Stroika::Foundation::Characters::String>' requested here
    is_trivially_destructible<_Tp>::value &&
    ^
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/optional:632:15: note: in instantiation of default argument for '__optional_move_assign_base<Stroika::Foundation::Characters::String>' required here
    : private __optional_move_assign_base<_Tp>
              ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
./../Characters/../Containers/../Traversal/Iterable.h:882:135: note: in instantiation of template class 'std::optional<Stroika::Foundation::Characters::String>' requested here
        nonvirtual Characters::String Join (const Characters::String& separator, const optional<Characters::String>& finalSeparator = {}) const;
                                                                                                                                      ^
./../Characters/../Containers/../Traversal/Iterable.h:61:11: note: forward declaration of 'Stroika::Foundation::Characters::String'



OR

 file included from ./../Characters/Format.h:10:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/ios:39:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/exception:168:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/bits/exception_ptr.h:43:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/bits/move.h:57:
/usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/type_traits:1274:4: error: incomplete type 'Stroika::Foundation::Characters::String' used in type trait expression
                        __is_trivially_constructible(_Tp, const _Tp&)>>
                        ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/type_traits:1280:14: note: in instantiation of template class 'std::__is_trivially_copy_constructible_impl<Stroika::Foundation::Characters::String, true>' requested here
    : public __is_trivially_copy_constructible_impl<_Tp>
             ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/type_traits:3226:5: note: in instantiation of template class 'std::is_trivially_copy_constructible<Stroika::Foundation::Characters::String>' requested here
    is_trivially_copy_constructible<_Tp>::value;
    ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/optional:508:12: note: in instantiation of variable template specialization 'std::is_trivially_copy_constructible_v<Stroika::Foundation::Characters::String>' requested here
           bool = is_trivially_copy_constructible_v<_Tp>,
                  ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/optional:706:15: note: in instantiation of default argument for '_Optional_base<Stroika::Foundation::Characters::String>' required here
    : private _Optional_base<_Tp>,
              ^~~~~~~~~~~~~~~~~~~
./../Characters/../Containers/../Traversal/Iterable.h:882:135: note: in instantiation of template class 'std::optional<Stroika::Foundation::Characters::String>' requested here
        nonvirtual Characters::String Join (const Characters::String& separator, const optional<Characters::String>& finalSeparator = {}) const;
        
*/
#ifndef qCompilerAndStdLib_template_optionalDeclareIncompleteType_Buggy

#if defined(__clang__)
// also broken in clang++-17
// also broken in clang++-18
#define qCompilerAndStdLib_template_optionalDeclareIncompleteType_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ <= 18)
#else
#define qCompilerAndStdLib_template_optionalDeclareIncompleteType_Buggy 0
#endif

#endif

/**
 * PLEASE submit a bug report to https://github.com/llvm/llvm-project/issues/ and include the crash backtrace, preprocessed source, and associated run script.
Stack dump:
0.      Program arguments: clang++-15 -I/Sandbox/Stroika-Dev/Builds/clang++-15-debug-libstdc++-c++23/ThirdPartyComponents/include/ -I/Sandbox/Stroika-Dev/Library/Sources/ -I/Sandbox/Stroika-Dev/IntermediateFiles/clang++-15-debug-libstdc++-c++23/ -I/Sandbox/Stroika-Dev/Builds/clang++-15-debug-libstdc++-c++23/ThirdPartyComponents/include/libxml2/ -D_GLIBCXX_DEBUG -DqDebug=1 -DqHasFeature_LibCurl=1 -DqHasFeature_OpenSSL=1 -DqHasFeature_WinHTTP=0 -DqHasFeature_ATLMFC=0 -DqHasFeature_Xerces=0 -DqHasFeature_libxml2=1 -DqHasFeature_ZLib=1 -DqHasFeature_GoogleTest=1 -DqHasFeature_sqlite=1 -DqHasFeature_LZMA=1 -DqHasFeature_boost=1 -DqStroika_Foundation_Debug_Trace_TraceToFile=1 -DqStroika_Foundation_Debug_Trace_DefaultTracingOn=1 -DLIBXML_STATIC --std=c++2b -Wall -Wno-switch -Wno-sign-compare -Wno-unused-function -Wno-psabi -Wno-unused-local-typedef -Wno-future-compat -Wno-unqualified-std-cast-call -g -fsanitize=address,undefined -stdlib=libstdc++ -c FloatConversion.cpp -o /Sandbox/Stroika-Dev/IntermediateFiles/clang++-15-debug-libstdc++-c++23/Library/Foundation/Characters/FloatConversion.o
1.      ./ToString.inl:449:73: current parser token ','
2.      ./ToString.inl:438:1: parsing namespace 'Stroika'
3.      ./ToString.inl:448:5: parsing function body 'Stroika::Foundation::Traversal::Iterable::Join'
4.      ./ToString.inl:448:5: in compound statement ('{}')
Stack dump without symbol names (ensure you have llvm-symbolizer in your PATH or set the environment var `LLVM_SYMBOLIZER_PATH` to point to it):
/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1(_ZN4llvm3sys15PrintStackTraceERNS_11raw_ostreamEi+0x31)[0x7f6fe74a43b1]
/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1(_ZN4llvm3sys17RunSignalHandlersEv+0xee)[0x7f6fe74a20fe]
/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1(_ZN4llvm3sys15CleanupOnSignalEm+0x101)[0x7f6fe74a3771]
/usr/lib/llvm-15/bin/../lib/libLLVM-15.so.1(+0xe2767f)[0x7f6fe73c767f]
/lib/x86_64-linux-gnu/l
*/
#ifndef qCompilerAndStdLib_template_SubstDefaultTemplateParamVariableTemplate_Buggy

#if defined(__clang__)
#define qCompilerAndStdLib_template_SubstDefaultTemplateParamVariableTemplate_Buggy                                                        \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ <= 16)
#else
#define qCompilerAndStdLib_template_SubstDefaultTemplateParamVariableTemplate_Buggy 0
#endif

#endif

/*
      Compiling Library/Sources/Stroika/Foundation/Cryptography/Encoding/Algorithm/AES.cpp ... 
PLEASE submit a bug report to https://github.com/llvm/llvm-project/issues/ and include the crash backtrace, preprocessed source, and associated run script.
Stack dump:
0.      Program arguments: clang++-15 -I/Sandbox/Stroika-Dev/Builds/clang++-15-debug-libstdc++-c++23/ThirdPartyComponents/include/ -I/Sandbox/Stroika-Dev/Library/Sources/ -I/Sandbox/Stroika-Dev/IntermediateFiles/clang++-15-debug-libstdc++-c++23/ -I/Sandbox/Stroika-Dev/Builds/clang++-15-debug-libstdc++-c++23/ThirdPartyComponents/include/libxml2/ -D_GLIBCXX_DEBUG -DqDebug=1 -DqHasFeature_LibCurl=1 -DqHasFeature_OpenSSL=1 -DqHasFeature_WinHTTP=0 -DqHasFeature_ATLMFC=0 -DqHasFeature_Xerces=0 -DqHasFeature_libxml2=1 -DqHasFeature_ZLib=1 -DqHasFeature_GoogleTest=1 -DqHasFeature_sqlite=1 -DqHasFeature_LZMA=1 -DqHasFeature_boost=1 -DqStroika_Foundation_Debug_Trace_TraceToFile=1 -DqStroika_Foundation_Debug_Trace_DefaultTracingOn=1 -DLIBXML_STATIC --std=c++2b -Wall -Wno-switch -Wno-sign-compare -Wno-unused-function -Wno-psabi -Wno-unused-local-typedef -Wno-future-compat -Wno-unqualified-std-cast-call -g -fsanitize=address,undefined -stdlib=libstdc++ -c AES.cpp -o /Sandbox/Stroika-Dev/IntermediateFiles/clang++-15-debug-libstdc++-c++23/Library/Foundation/Cryptography/Encoding/Algorithm/AES.o
1.      ./../../../Execution/../Characters/ToString.inl:454:73: current parser token ','
2.      ./../../../Execution/../Characters/ToString.inl:438:1: parsing namespace 'Stroika'
3.      ./../../../Execution/../Characters/ToString.inl:448:5: parsing function body 'Stroika::Foundation::Traversal::Iterable::Join'
4.      ./../../../Execution/../Characters/ToString.inl:448:5: in compound statement ('{}')
Stack dump without symbol names (ensure you have llvm-symbolizer in your PATH or set the environment var `LLVM_SYMBOLIZER_PATH` to point to it):
*/
#ifndef qCompilerAndStdLib_kDefaultToStringConverter_Buggy

#if defined(__clang__)
#define qCompilerAndStdLib_kDefaultToStringConverter_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ <= 16)
#else
#define qCompilerAndStdLib_kDefaultToStringConverter_Buggy 0
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
// still broken in g++-14
#define qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 14)
#elif defined(__clang__) && defined(__APPLE__)
// First noticed in clang++-14
// Reproduced in clang++-15
#define qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// First noticed in clang++-14
// broken in clang++-15
// broken in clang++-16
// broken in clang++-17
// broken in clang++-18
#define qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
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
// BROKEN IN GCC 14
#define qCompilerAndStdLib_templateConstructorSpecialization_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 14)
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
// still broken in _MSC_VER_2k22_17Pt8_
// still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_template_template_call_SequentialEquals_Buggy                                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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
#if defined(__clang_major__)
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
// verified still broken in _MSC_VER_2k22_17Pt8_
// verified still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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
// Reproduced using xcode 15 with _LIBCPP_VERSION == 160006 (yes they appear to have added an extra digit recently)
#define qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy (_LIBCPP_VERSION < 170000)
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
// and GCC 14
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_warning_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 14)
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

#if defined(__clang__) && defined(__APPLE__)
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
// 170002 also seems broken, (at least with clang++-14)
// 180100 also seems broken with clang++-15 on ununtu 24.04
#define qCompilerAndStdLib_to_chars_FP_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION < 190000))
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

 ...  Stroika/ThirdPartyComponents/libxml2 -    [Succeeded]
Building Stroika Library {g++-11-debug}:
   Building Stroika Foundation Objs {g++-11-debug}:
      Compiling Library/Sources/Stroika/Foundation/Cache/Statistics.cpp ... 
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.h:241,
                 from Statistics.cpp:6:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.inl: In copy constructor ‘constexpr Stroika::Foundation::Characters::FormatString<char>::FormatString(const Stroika::Foundation::Characters::FormatString<char>&)’:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.inl:52:11: error: call to non-‘constexpr’ function ‘std::__debug::vector<_Tp, _Allocator>::vector(const std::__debug::vector<_Tp, _Allocator>&) [with _Tp = wchar_t; _Allocator = std::allocator<wchar_t>]’
   52 |         : fStringData_{src.fStringData_}
      |           ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /usr/include/c++/11/vector:76,
                 from /usr/include/c++/11/functional:62,
                 
*/
#ifndef qCompilerAndStdLib_vector_constexpr_Buggy
#if defined(_GLIBCXX_RELEASE)
#define qCompilerAndStdLib_vector_constexpr_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_GLIBCXX_RELEASE <= 12)
#else
#define qCompilerAndStdLib_vector_constexpr_Buggy 0
#endif
#endif

/*
 - Checking ...  Stroika/ThirdPartyComponents/sqlite -  [Succeeded]
   Stroika/ThirdPartyComponents/libxml2 Build 2.12.5 {g++-13-debug}:
      libxml2 2.12.5 - Checking ...  Stroika/ThirdPartyComponents/libxml2 -    [Succeeded]
Building Stroika Library {g++-13-debug}:
   Computing /Sandbox/Stroika-Dev/IntermediateFiles/g++-13-debug/Library/Foundation/cached-list-objs
   Building Stroika Foundation Objs {g++-13-debug}:
      Compiling Library/Sources/Stroika/Foundation/Cache/Statistics.cpp ... 
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.h:267,
                 from Statistics.cpp:6:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.inl: In copy constructor ‘constexpr Stroika::Foundation::Characters::FormatString<char>::FormatString(const Stroika::Foundation::Characters::FormatString<char>&)’:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.inl:57:11: warning: call to non-‘constexpr’ function ‘std::__debug::vector<_Tp, _Allocator>::vector(const std::__debug::vector<_Tp, _Allocator>&) [with _Tp = wchar_t; _Allocator = std::allocator<wchar_t>]’ [-Winvalid-constexpr]
   57 |         : fStringData_{src.fStringData_}
      |           ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /usr/include/c++/13/vector:76,
                 from /usr/include/c++/13/chrono:47,
                 from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Configuration/Concepts.h:9,
                 from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/Format.h:16:
/usr/include/c++/13/debug/vector:212:7: note: ‘std::__debug::vector<_Tp, _Allocator>::vector(const std::__debug::vector<_Tp, _Allocator>&) [with _Tp = wchar_t; _Allocator = std::allocator<wchar_t>]’ is not usable as a ‘constexpr’ function because:

*/
#ifndef qCompilerAndStdLib_vector_constexpr_warning_Buggy
#if defined(_GLIBCXX_RELEASE)
#define qCompilerAndStdLib_vector_constexpr_warning_Buggy (_GLIBCXX_RELEASE == 13)
#else
#define qCompilerAndStdLib_vector_constexpr_warning_Buggy 0
#endif
#endif

/*
In file included from ./../Characters/../Containers/Factory/KeyedCollection_Factory.inl:16:
In file included from ./../Characters/../Containers/Factory/../Concrete/KeyedCollection_stdset.h:136:
./../Characters/../Containers/Concrete/KeyedCollection_stdset.inl:60:22: error: no viable constructor or deduction guide for deduction of template arguments of 'SetInOrderComparer'
            , fData_{SetInOrderComparer{keyExtractor, inorderComparer}}
                     ^



  Compiling Library/Sources/Stroika/Foundation/Execution/IntervalTimer.cpp ... 
In file included from IntervalTimer.cpp:18:
In file included from ./IntervalTimer.h:10:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/KeyedCollection.h:594:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/KeyedCollection.inl:14:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Factory/KeyedCollection_Factory.h:123:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Factory/KeyedCollection_Factory.inl:16:
In file included from /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Factory/../Concrete/KeyedCollection_stdset.h:148:
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Containers/Factory/../Concrete/KeyedCollection_stdset.inl:59:22: error: no viable constructor or deduction guide for deduction of template arguments of 'SetInOrderComparer'
   59 |             , fData_{SetInOrderComparer{keyExtractor, inorderComparer}}
      |                      ^
/usr/lib/llvm-18/bin/../include/c++/v1/__memory/construct_at.h:41:46: note: in instantiation of member function 'Stroika::Foundation::Containers::Concrete::KeyedCollection_stdset<Stroika::Foundation::Execution::IntervalTimer::RegisteredTask, Stroika::Foundation::Execution::Function<void ()>, Stroika::Foundation::Containers::KeyedCollection_DefaultTraits<Stroika::Foundation::Execution::IntervalTimer::RegisteredTask, Stroika::Foundation::Execution::Function<void ()>, 
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
// appears still broken in clang++-17
// appears still broken in clang++-18
#define qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
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


/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/x/Stroika-Foundation.a(SDKString.o): in function `Stroika::Foundation::Memory::InlineBuffer<wchar_t, 1024ul>::Allocate_(unsigned long)':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/InlineBuffer.inl:532:(.text._ZN7Stroika10Foundation6Memory12InlineBufferIwLm1024EE7reserveEmb[_ZN7Stroika10Foundation6Memory12InlineBufferIwLm1024EE7reserveEmb]+0x93): undefined reference to `void Stroika::Foundation::Execution::ThrowIfNull<void*>(void*)'
/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/x/Stroika-Foundation.a(SDKString.o): in function `Stroika::Foundation::Memory::InlineBuffer<std::byte, 4096ul>::Allocate_(unsigned long)':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/InlineBuffer.inl:532:(.text._ZN7Stroika10Foundation6Memory12InlineBufferISt4byteLm4096EE7reserveEmb[_ZN7Stroika10Foundation6Memory12InlineBufferISt4byteLm4096EE7reserveEmb]+0x8e): undefined reference to `void Stroika::Foundation::Execution::ThrowIfNull<void*>(void*)'
/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/x/Stroika-Foundation.a(Utilities.o): in function `Stroika::Foundation::Memory::InlineBuffer<char, 10240ul>::Allocate_(unsigned long)':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/InlineBuffer.inl:532:(.text._ZN7Stroika10Foundation6Memory12InlineBufferIcLm10240EE7reserveEmb[_ZN7Stroika10Foundation6Memory12InlineBufferIcLm10240EE7reserveEmb]+0x8e): undefined reference to `void Stroika::Foundation::Execution::ThrowIfNull<void*>(void*)'
/usr/bin/ld: /Sandbox/Stroika-Dev/Builds/x/Stroika-Foundation.a(Utilities.o): in function `Stroika::Foundation::Memory::InlineBuffer<char8_t, 10240ul>::Allocate_(unsigned long)':
/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Memory/InlineBuffer.inl:532:(.text._ZN7Stroika10Foundation6Memory12InlineBufferIDuLm


      Linking  $StroikaRoot/Builds/clang++-17-release-libc++23/HTMLViewCompiler...
/usr/bin/ld: /tmp/lto-llvm-91d516.o: in function `Stroika::Foundation::Memory::InlineBuffer<wchar_t, 1024ul>::reserve(unsigned long, bool)':
ld-temp.o:(.text._ZN7Stroika10Foundation6Memory12InlineBufferIwLm1024EE7reserveEmb[_ZN7Stroika10Foundation6Memory12InlineBufferIwLm1024EE7reserveEmb]+0x83): undefined reference to `void Stroika::Foundation::Execution::ThrowIfNull<void*>(void*)'
/usr/bin/ld: /tmp/lto-llvm-91d516.o: in function `Stroika::Foundation::Memory::InlineBuffer<std::byte, 4096ul>::Allocate_(unsigned long)':
/home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2404_x86_64/Library/Sources/Stroika/Foundation/Memory/InlineBuffer.inl:532:(.text._ZN7Stroika10Foundation6Memory12InlineBufferISt4byteLm4096EE7reserveEmb[_ZN7Stroika10Foundation6Memory12InlineBufferISt4byteLm4096EE7reserveEmb]+0x91): undefined reference to `void Stroika::Foundation::Execution::ThrowIfNull<void*>(void*)'
/usr/bin/ld: /tmp/lto-llvm-91d516.o: in function `Stroika::Foundation::Memory::InlineBuffer<char, 10240ul>::Allocate_(unsigned long)':
/home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu2404_x86_64/Library/Sources/Stroika/Foundation/Memory/InlineBuffer.inl:532:(.text._ZN7Stroika10Foundation10Characters7CString6FormatEPKcz+0x1f6): undefined reference to `void Stroika::Foundation::Execution::ThrowIfNull<void*>(void*)'

*/
#ifndef qCompilerAndStdLib_release_bld_error_bad_obj_offset_Buggy

#if defined(__clang__) && !defined(__APPLE__)
// appears still broken in clang++-13 and -clang++-14-release-libstdc++
// and broekn uuntu22.04 clang++-15-release-libstdc++
// broken in clang++16 - release builds - seemed OK for a while but then broke again - not just says missing symbol... so possibly different bug???
// same issue with clang++-17-release-libc++23
// same issue with clang++-18-release-libc++23
#define qCompilerAndStdLib_release_bld_error_bad_obj_offset_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
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
// VERIFIED BROKEN on XCode 14
// VERIFIED BROKEN on XCode 15
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15))
#elif defined(__clang__) && !defined(__APPLE__)
// still broken in clang++-14
// still broken in clang++-15
// still broken in clang++-16
// still broken in clang++-17
// still broken in clang++-18
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
#elif defined(_MSC_VER)
// verified still broken in _MSC_VER_2k22_17Pt0_
// verified still broken in _MSC_VER_2k22_17Pt1_
// verified still broken in _MSC_VER_2k22_17Pt2_
// verified still broken in _MSC_VER_2k22_17Pt3_
// verified still broken in _MSC_VER_2k22_17Pt4_
// verified still broken in _MSC_VER_2k22_17Pt5_
// verified still broken in _MSC_VER_2k22_17Pt6_
// verified still broken in _MSC_VER_2k22_17Pt7_
// verified still broken in _MSC_VER_2k22_17Pt8_
// verified still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy                                                                       \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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
// verified still broken in clang++-17
// verified still broken in clang++-18
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy                                                \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 18))
#else
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy 0
#endif

#endif

/*
 * qCompilerAndStdLib_template_template_auto_deduced_Buggy fails on SystemPerofrmanceClient sample
 */
#ifndef qCompilerAndStdLib_template_template_auto_deduced_Buggy
#if defined(_MSC_VER)
// ifrst noted broken in _MSC_VER_2k22_17Pt8_
// still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_template_template_auto_deduced_Buggy                                                                            \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
#else
#define qCompilerAndStdLib_template_template_auto_deduced_Buggy 0
#endif

#endif

/*


ng Stroika Library {clang++-18-debug-libc++}:
   Building Stroika Foundation Objs {clang++-18-debug-libc++}:
      Compiling Library/Sources/Stroika/Foundation/Characters/CodeCvt.cpp ... 
In file included from CodeCvt.cpp:11:
In file included from ./CodeCvt.h:469:
./CodeCvt.inl:69:15: error: type constraint differs in template redeclaration
   69 |     template <IUNICODECanAlwaysConvertTo SERIALIZED_CHAR_T>
      |               ^



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
// appears broken in clang++-18
#define qCompilerAndStdLib_template_second_concept_Buggy                                                                                   \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 15) or (__clang_major__ == 18))
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
// Broken broken in 170002
// 180100 also seems broken with clang++-15 on ununtu 24.04
#if defined(__APPLE__)
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION < 190000))
#else
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION < 190000))
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
// Verified still broken in _MSC_VER_2k22_17Pt8_
// Verified still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy                                                                           \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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
// verified broken in _MSC_VER_2k22_17Pt8_
// verified broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy                                                                \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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
// still broken in _MSC_VER_2k22_17Pt8_
// appears fixed in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy                                                                 \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k22_17Pt0_ <= _MSC_VER and _MSC_VER <= _MSC_VER_2k22_17Pt8_)
#else
#define qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy 0
#endif

#endif

/*
* FAILED: RegressionTestFailure; resultTM.tm_mday == kTargetTM_MDY_.tm_mday;;C:\Sandbox\Stroika\DevRoot\Tests\50\Test.cpp: 213
* 
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
// verified still broken in _MSC_VER_2k22_17Pt8_
// verified still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy                                                       \
    CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
#else
#define qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy 0
#endif

#endif

/* attempts to format a date outside the range -1900 to 8099 trigger an assertion erorr in vis studio runtime lib
*/
#ifndef qCompilerAndStdLib_FormatRangeRestriction_Buggy

#if defined(_MSC_VER)
// verified still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_FormatRangeRestriction_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
#else
#define qCompilerAndStdLib_FormatRangeRestriction_Buggy 0
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
// verified still broken in _MSC_VER_2k22_17Pt8_
// verified still broken in _MSC_VER_2k22_17Pt9_
#define qCompilerAndStdLib_locale_utf8_string_convert_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt9_)
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

// Not quite the same thing as original bug define but seems a second/related issue that only apperars with clang
#if qCompilerAndStdLib_CompareOpReverse_Buggy && defined(__clang__)
#pragma clang diagnostic ignored "-Wambiguous-reversed-operator"
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

/*
 * DEFINING #define PSTL_USAGE_WARNINGS 0
#define _PSTL_USAGE_WARNINGS 0
*   Doesn't appear to work - not sure why...
*/
#if qCompilerAndStdLib_PSTLWARNINGSPAM_Buggy
#include <bits/c++config.h>
#undef _PSTL_PRAGMA_MESSAGE
#define _PSTL_PRAGMA_MESSAGE(x)
#endif

#if qCompilerAndStdLib_need_ciso646_Buggy
#include <ciso646>
#endif

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

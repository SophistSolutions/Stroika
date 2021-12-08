﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

#if defined(__cplusplus)

/**
 *
 */
#define kStrokia_Foundation_Configuration_cplusplus_11 201103

/**
 *
 */
#define kStrokia_Foundation_Configuration_cplusplus_14 201402

/**
 * According to https://en.wikipedia.org/wiki/C%2B%2B17 - 201703
 */
#define kStrokia_Foundation_Configuration_cplusplus_17 201703

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
#if (__clang_major__ < 11) || (__clang_major__ == 11 && (__clang_minor__ < 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika v2.1 (older clang versions supported by Stroika v2.0) does not support versions prior to APPLE clang++ 11 (XCode 11)"
#endif
#if (__clang_major__ > 13) || (__clang_major__ == 13 && (__clang_minor__ > 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Info: Stroika untested with this version of clang++ (APPLE) - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#else
// Must check CLANG first, since CLANG also defines GCC
// see
//      clang++-3.8 -dM -E - < /dev/null
#if (__clang_major__ < 6) || (__clang_major__ == 6 && (__clang_minor__ < 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika v2.1 does not support versions prior to clang++ 6 (non-apple); note that Stroika v2.0 supports clang3.9, clang4, and clang5"
#endif
#if (__clang_major__ > 13) || (__clang_major__ == 13 && (__clang_minor__ > 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Info: Stroika untested with this version of clang++ - (>13.0) USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#endif

#elif defined(__GNUC__)

#if __GNUC__ < 8
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika v2.1 does not support versions prior to GCC 8 (v2.0 supports g++5 and g++6 and g++-7)"
#endif
#if __GNUC__ > 11 || (__GNUC__ == 11 && (__GNUC_MINOR__ > 2))
#define _STROIKA_CONFIGURATION_WARNING_ "Info: Stroika untested with this version of GCC - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif

#elif defined(_MSC_VER)

// _MSC_VER=1914
#define _MS_VS_2k17_15Pt7Pt6_ 191426433
#define _MSC_VER_2k17_15Pt7_ 1914

// _MSC_VER=1915
#define _MSC_VER_2k17_15Pt8_ 1915

// This is the only one tested for the last couple years - from the VS2k17 vintage - 15.9.x or later... -- LGP 2021-06-14
// _MSC_VER=1916
#define _MSC_VER_2k17_15Pt9_ 1916

// _MSC_VER=1920
#define _MSC_VER_2k19_16Pt0_ 1920
#define _MS_VS_2k19_16Pt0Pt0pre2_ 192027305
#define _MS_VS_2k19_16Pt0Pt0pre4_ 192027404

// _MSC_VER=1921
#define _MSC_VER_2k19_16Pt1_ 1921
#define _MS_VS_2k19_16Pt1Pt0_ 192127702

// _MSC_VER=1922
#define _MSC_VER_2k19_16Pt2_ 1922
#define _MS_VS_2k19_16Pt2Pt0_ 192227905

// _MSC_VER=1923
#define _MSC_VER_2k19_16Pt3_ 1923

// _MSC_VER=1924
#define _MSC_VER_2k19_16Pt4_ 1924

// _MSC_VER=1925
#define _MSC_VER_2k19_16Pt5_ 1925

// _MSC_VER=1926
#define _MSC_VER_2k19_16Pt6_ 1926

// _MSC_VER=1927
#define _MSC_VER_2k19_16Pt7_ 1927

// _MSC_VER=1928
#define _MSC_VER_2k19_16Pt8_ 1928

// _MSC_VER=1928 (SADLY - due to https://developercommunity2.visualstudio.com/t/The-169-cc-compiler-still-uses-the-s/1335194)
#define _MSC_VER_2k19_16Pt9_ 1928

// _MSC_VER=1929 (back to incrementing for new 'second dot' versions it appears)...
#define _MSC_VER_2k19_16Pt10_ 1929

// _MSC_VER=1930
#define _MSC_VER_2k22_17Pt0_ 1930

#if _MSC_VER < 1910
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2017 (use Stroika v2.0 or earlier)"
#elif _MSC_VER <= _MSC_VER_2k17_15Pt7_
// check which pointer-version of MSVC2k17 (15.7.x)
#if _MSC_FULL_VER > _MS_VS_2k17_15Pt7Pt6_
#define _STROIKA_CONFIGURATION_WARNING_ "Info: This version ( #_MSC_FULL_VER ) - 15.7.x - of Stroika is untested with this Update of of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#elif _MSC_VER <= _MSC_VER_2k17_15Pt9_
// check which pointer-version of MSVC2k17 just assume anythign 15.9 is OK - checked up to 15.9.19
#elif _MSC_VER <= _MSC_VER_2k19_16Pt4_
// We COULD look at _MSC_FULL_VER and compare to _MS_VS_2k19_16Pt3Pt5_ etc, but changes too often and too rarely makes a difference
// Just assume all bug defines the same for a given _MSC_VER
#elif _MSC_VER <= _MSC_VER_2k19_16Pt10_
// We COULD look at _MSC_FULL_VER but changes too often and too rarely makes a difference: just assume all bug defines the same for a given _MSC_VER
#elif _MSC_VER <= _MSC_VER_2k22_17Pt0_
// We COULD look at _MSC_FULL_VER but changes too often and too rarely makes a difference: just assume all bug defines the same for a given _MSC_VER
#else
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: This version (> 17.0) of Stroika is untested with this release of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif

#else

#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika does recognize the compiler being used. It may work, but you may need to update some of the other defines for what features are supported by your compiler."

#endif

/* 
 * to find glibc version

        g++-8 foo.cpp

foo.cpp:
    #include <iostream>

    int main ()
    {
    using namespace std;
    cerr << "__GLIBCXX__=" << __GLIBCXX__ << "\n";
    return 0;
    }

 prints __GLIBCXX__=20180728
 */
#define GLIBCXX_7x_ 20180720
#define GLIBCXX_8x_ 20180728
#define GLIBCXX_9x_ 20191008
#define GLIBCXX_93x_ 20200408
//#define GLIBCXX_10x_ 20200930
#define GLIBCXX_10x_ 20210408
//#define GLIBCXX_11x_ 20210427
#define GLIBCXX_11x_ 20210923

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
#include <ciso646>
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

/*
@CONFIGVAR:     qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy

Stroika-Frameworks-WebServer.vcxproj


1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\dataexchange\badformatexception.h(40): warning C4359: 'Stroika::Foundation::DataExchange::BadFormatException::kThe': Alignment specifier is less than actual alignment (8), and will be ignored.
...
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(373): error C2719: 'defaultValue': formal parameter with requested alignment of 8 won't be aligned
1>  c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\iterator.h(566): note: see reference to class template instantiation 'Stroika::Foundation::Memory::Optional<Stroika::Foundation::IO::Network::Interface,Stroika::Foundation::Memory::Optional_Traits_Inplace_Storage<T>>' being compiled
....1>  c:\sandbox\stroika\devroot\library\sources\stroika\foundation\io\network\interface.cpp(137): note: see reference to class template instantiation 'Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::IO::Network::Interface>' being compiled
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(440): error C2719: 'unnamed-parameter': formal parameter with requested alignment of 8 won't be aligned
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(544): error C2719: 'rhs': formal parameter with requested alignment of 8 won't be aligned
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(552): error C2719: 'rhs': formal parameter with requested alignment of 8 won't be aligned
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\iterable.h(691): error C2719: 'unnamed-parameter': formal parameter with requested alignment of 8 won't be aligned
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\iterable.h(767): error C2719: 'unnamed-parameter': formal parameter with requested alignment of 8 won't be aligned

error C2719: 'end': formal parameter with requested alignment of 8 won't be aligned (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\ObjectVariantMapper.cpp)

...

2>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\common\../Configuration/Concepts.h(107): error C2718: 'Stroika::Foundation::Traversal::Iterator<T,std::iterator<std::forward_iterator_tag,T,ptrdiff_t,_Ty *,_Ty &>>': actual parameter with requested alignment of 8 won't be aligned
2>        with
2>        [
2>            T=Stroika::Foundation::IO::Network::Interface,
*/
#ifndef qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy

#if defined(_MSC_VER)
// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_ - BUT MUCH MORE SUBTLY - WEBSERVER APP CRASHES (USES OPTIONAL) - AT RUNTIME - (at least debug build) - SO TEST WEBSERVER SAMPLE
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
// still broken in _MS_VS_2k17_15Pt6Pt0_
// Still broken in _MS_VS_2k17_15Pt7Pt1_ - but now causes more crashes in more places (perhaps just calling code changed) - more mysterious crashes with use of Optional  in regtests crash now
// assume broken in _MS_VS_2k17_15Pt7Pt2_
// assume broken in _MS_VS_2k17_15Pt7Pt3_
// assume broken in _MS_VS_2k17_15Pt7Pt4_
// assume broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// Verified FIXED(BROKEN) in _MSC_VER_2k17_15Pt8_ (webserver no longer crashes - misleading - still broken
// but we no longer use Memory::Optional, so we will have to lose this BWA, but bug NOT FIXED)
// Can really only test this bug in stroika v2.0 branch since only place really still using Optional (that has this bug workaround)
// until we test v2.0 branch, ASSUME fixed in _MSC_VER_2k17_15Pt9_
#define qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy 0
#endif

#endif

///C:\Sandbox\Stroika\DevRoot\Tests\TestCommon\CommonTests_MultiSet.h(246): error C2760: syntax error: unexpected token ';', expected ')'
// You can use [[maybe_unused]] after the identifier, but not before the auto
#ifndef qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop_Buggy

#if defined(_MSC_VER)
// verified broken in _MSC_VER_2k19_16Pt10_
// verified broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k19_16Pt8_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop_Buggy 0
#endif

#endif

/*
  Building Stroika Foundation Objs:
      Compiling Library/Sources/Stroika/Foundation/Configuration/Version.cpp ... 
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\debug\AssertExternallySynchronizedMutex.h(158): fatal error C1001: An internal error has occurred in the compiler.
(compiler file 'd:\agent\_work\4\s\src\vctools\compiler\cxxfe\sl\p1\cxx\grammar.y', line 12721)
 To work around this problem, try simplifying or changing the program near the locations listed above.
Please choose the Technical Support command on the Visual C++
 Help menu, or open the Technical Support help file for more information
      Compiling Library/Sources/Stroika/Foundation/Configuration/Platform/Windows/Registry.cpp ... 
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\debug\AssertExternallySynchronizedMutex.h(158): fatal error C1001: An internal error has occurred in the compiler.
(compiler file 'd:\agent\_work\4\s\src\vctools\compiler\cxxfe\sl\p1\cxx\grammar.y', line 12721)
 To work around this problem, try simplifying or changing the program near the locations listed above.
Please choose the Technical Support command on the Visual C++
 Help menu, or open the Technical Support help file for more information
       */
#ifndef qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop2_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop2_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_maybe_unused_b4_auto_in_for_loop2_Buggy 0
#endif

#endif

/**
 *  not sure what is wrong here but some crasher in InternetMediaTypeRegistry::FrontendRep_ code...
 *  ONLY VS2k17 problem and not going to support much longer
 */
#ifndef qCompilerAndStdLib_initializer_list_sometimes_very_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_initializer_list_sometimes_very_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_initializer_list_sometimes_very_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_maybe_unused_in_lambda_ignored_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_maybe_unused_in_lambda_ignored_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_maybe_unused_in_lambda_ignored_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_MemInitializerWithBitfield_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_MemInitializerWithBitfield_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_MemInitializerWithBitfield_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_usingOfEnumFailsToBringIntoScope_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_usingOfEnumFailsToBringIntoScope_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_usingOfEnumFailsToBringIntoScope_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_uniformInitializationsFailsOnIntSize_t_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_uniformInitializationsFailsOnIntSize_t_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_uniformInitializationsFailsOnIntSize_t_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_startupAppMagicStaticsNotWorkingFully_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_startupAppMagicStaticsNotWorkingFully_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_startupAppMagicStaticsNotWorkingFully_Buggy 0
#endif

#endif

/*
>   Compiling Library/Sources/Stroika/Frameworks/WebServer/Request.cpp ...
2>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\concepts.h(467): error C2385: ambiguous access of 'value_type'
2>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\concepts.h(467): note: could be the 'value_type' in base 'Stroika::Foundation::Common::ReadOnlyProperty<Stroika::Foundation::Characters::String>'
2>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\concepts.h(467): note: or could be the 'value_type' in base 'Stroika::Foundation::Common::WriteOnlyProperty<Stroika::Foundation::Characters::String>'
2>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\concepts.h(493): note: see reference to class template instantiation 'Stroika::Foundation::Configuration::ExtractValueType<Stroika::Foundation::Common::Property<Stroika::Foundation::Characters::String>,void>' being compiled
2>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\concepts.h(493): note: see reference to alias template instantiation 'Stroika::Foundation::Configuration::ExtractValueType_t<Stroika::Foundation::Common::Property<Stroika::Foundation::Characters::String>>' being compiled
2>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\concepts.h(467): note: see reference to variable template 'const bool IsIterable_v<Stroika::Foundation::Common::Property<Stroika::Foundation::Characters::String> >' being compiled
2>c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\type_traits(16707566): note: see reference to function template instantiation 'auto Stroika::Frameworks::WebServer::Request::<lambda_50f8cdc935c03d80f9b5052785b593ec>::operator ()<Stroika::Foundation::Common::ReadOnlyProperty<bool>>(const Stroika::Foundation::Common::ReadOnlyProperty<bool> *) const' being compiled
2>c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\type_traits(1871): note: see reference to alias template instantiation 'std::_Is_invocable_r_<bool,Stroika::Frameworks::WebServer::Request::<lambda_50f8cdc935c03d80f9b5052785b593ec>&,const Stroika::Foundation::Common::ReadOnlyProperty<bool>*>' being compiled
2>c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\functional(1279): note: see reference to class template instantiation 'std::_Is_invocable_r<_Ret,_Fx,const Stroika::Foundation::Common::ReadOnlyProperty<bool> *>' being compiled
2>        with
2>*/
#ifndef qCompilerAndStdLib_template_value_type_ambiguous_confusion_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_template_value_type_ambiguous_confusion_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_template_value_type_ambiguous_confusion_Buggy 0
#endif

#endif


/*
Building Stroika Foundation Objs:
      Compiling Library/Sources/Stroika/Foundation/IO/Network/HTTP/CacheControl.cpp ... 
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\enumeration.inl(210): fatal error C1001: An internal error has occurred in the compiler.
(compiler file 'd:\agent\_work\4\s\src\vctools\compiler\cxxfe\sl\p1\c\yyaction.cpp', line 1142)
 To work around this problem, try simplifying or changing the program near the locations listed above.
Please choose the Technical Support command on the Visual C++
 Help menu, or open the Technical Support help file for more information
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\enumeration.inl(207): note: see reference to function template instantiation 'void Stroika::Foundation::Configuration::EnumNames<Stroika::Foundation::IO::Network::HTTP::CacheControl::Cacheability>::RequireItemsOrderedByEnumValue_(void) const' being compiled
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\enumeration.inl(114): note: see reference to function template instantiation 'void Stroika::Foundation::Configuration::EnumNames<Stroika::Foundation::IO::Network::HTTP::CacheControl::Cacheability>::RequireItemsOrderedByEnumValue_(void) const' being compiled
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\characters\string.inl(370): note: see reference to class template instantiation 'std::optional<size_t>' being compiled
c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\filesystem(2392): note: see reference to class template instantiation 'std::chrono::time_point<std::filesystem::_File_time_clock,std::filesystem::_File_time_clock::duration>' being compiled   
c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\type_traits(616): note: see reference to class template instantiation 'std::basic_string_view<wchar_t,std::char_traits<wchar_t>>' being compiled
c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\xstring(2124): note: see reference to class template instantiation 'std::is_convertible<const _StringViewIsh &,std::basic_string_view<wchar_t,std::char_traits<wchar_t>>>' being compiled       
        with
        [
*/
#ifndef qCompilerAndStdLib_constexpr_call_constexpr_sometimes_internalError_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_constexpr_call_constexpr_sometimes_internalError_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_constexpr_call_constexpr_sometimes_internalError_Buggy 0
#endif

#endif

/*
c:\sandbox\stroika\devroot\tests\52\test.cpp(766): error C2039: 'Contains': is not a member of 'std::set<int,std::less<int>,std::allocator<int>>'
c:\sandbox\stroika\devroot\tests\52\test.cpp(1494): note: see declaration of 'std::set<int,std::less<int>,std::allocator<int>>'
c:\sandbox\stroika\devroot\tests\52\test.cpp(796): note: see reference to function template instantiation 'void `anonymous-namespace'::Private_::CopyContainerByValue<CONTAINER>(CONTAINER,int)' being compiled
*/
#ifndef qCompilerAndStdLib_if_constexpr_annoyingly_evaluates_untaken_path_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_if_constexpr_annoyingly_evaluates_untaken_path_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_if_constexpr_annoyingly_evaluates_untaken_path_Buggy 0
#endif

#endif

/**
 *
 Sent email to MSFT - can reproduce with ASAN and -- https://developercommunity.visualstudio.com/t/initializer-list-lifetime-buggy-maybe-just-asan-is/1439352

                 for (string i : initializer_list<string>{"a", "b", "c"}) {
                    assert (i.length () == 1);
                }

 * UPDATED BUG REPORT WITH 
 *      https://developercommunity.visualstudio.com/t/initializer-list-lifetime-buggy-maybe-just-asan-is/1439352#T-N1439942-N1454940
 *      reproduction example
 */
#ifndef qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy

#if defined(_MSC_VER)
// verified broken in _MSC_VER_2k19_16Pt10_
// verified broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_ASAN_initializerlist_scope_Buggy 0
#endif

#endif

// First broken vs2k19 16.11.0, but seems to have same MSC_VER, so lump in with previous compiler
#ifndef qCompilerAndStdLib_ASAN_windows_http_badheader_Buggy

#if defined(_MSC_VER)
// verified broken in _MSC_VER_2k19_16Pt10_
// Appears FIXED in _MSC_VER_2k22_17Pt0_ (or maybe windows 11 - tested both at same time)
#define qCompilerAndStdLib_ASAN_windows_http_badheader_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k19_16Pt10_)
#else
#define qCompilerAndStdLib_ASAN_windows_http_badheader_Buggy 0
#endif

#endif

/*
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Cryptography\Encoding\Algorithm\Base64.cpp(60): error C2440: 'initializing': cannot convert from 'int' to 'std::byte'
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Cryptography\Encoding\Algorithm\Base64.cpp(60): note: Conversion to enumeration type requires an explicit cast (static_cast, C-style cast or function-style cast)
*/
// see https://en.cppreference.com/w/cpp/types/byte "due to C++17 relaxed enum class initialization rules."
#ifndef qCompilerAndStdLib_relaxedEnumClassInitializationRules_Buggy

#if defined(_MSC_VER)
// verified still broken in _MSC_VER_2k19_16Pt10_
// verified still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_relaxedEnumClassInitializationRules_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_relaxedEnumClassInitializationRules_Buggy 0
#endif

#endif

//
// Issue APPEARS to be operator bool called, and that is what gets compared on!!!
// Also breaks other operators
// REPORTED TO MSFT  - https://developercommunity.visualstudio.com/content/problem/997235/operator-bool-buggy-interaction-with-auto-generate.html
//
// Simple workaround appears to be to use explicit operator==
// Cannot TEST this now (expect using code from https://developercommunity.visualstudio.com/content/problem/997235/operator-bool-buggy-interaction-with-auto-generate)
// so probably remove this bug before long - leave define around for POINTER to discussion/reproduction details in case I run into this again anytime soon
//  (maybe untrue - appears I have explicit test case in Test code)--LGP 2020-05-21
//
#ifndef qCompilerAndStdLib_operatorCompareWithOperatorBoolConvertAutoGen_Buggy

#if defined(_MSC_VER)
// first found broken in _MSC_VER_2k19_16Pt5_
// APPEARS FIXED? _MSC_VER_2k19_16Pt6_
#define qCompilerAndStdLib_operatorCompareWithOperatorBoolConvertAutoGen_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k19_16Pt5_)
#else
#define qCompilerAndStdLib_operatorCompareWithOperatorBoolConvertAutoGen_Buggy 0
#endif

#endif

/*
// Get runtime failure in Test_12_DateRange_ in 
FAILED : RegressionTestFailure;
dr.Contains (dr.GetMidpoint ());
C :\Sandbox\Stroika\DevRoot\Tests\48\Test.cpp : 750
        [](3 seconds)[49] Foundation::Time (../Builds/Release-U-32/Tests/Test49.exe)
 */
#ifndef qCompilerAndStdLib_ReleaseBld32Codegen_DateRangeInitializerDateOperator_Buggy

#if defined(_MSC_VER)
// first/only found broken in _MSC_VER_2k19_16Pt6_ - and only in RELEASE and 32-bit builds
// still broken in _MSC_VER_2k19_16Pt7_
// still broken in _MSC_VER_2k19_16Pt8_
// still broken in in _MSC_VER_2k19_16Pt10_
// APPEARS FIXED in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_ReleaseBld32Codegen_DateRangeInitializerDateOperator_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k19_16Pt6_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k19_16Pt10_) && !qDebug && defined (_M_IX86))
#else
#define qCompilerAndStdLib_ReleaseBld32Codegen_DateRangeInitializerDateOperator_Buggy 0
#endif

#endif

/*
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(95): error C2061: syntax error: identifier 'CountedValue<unsigned int,unsigned int>'
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(190): note: see reference to function template instantiation 'void CommonTests::MultiSetTests::PRIVATE_::Test1_MiscStarterTests_::MultiSetIteratorTests_<DEFAULT_TESTING_SCHEMA>(const DEFAULT_TESTING_SCHEMA &,Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<T>> &)' being compiled
1>        with
1>        [
1>            DEFAULT_TESTING_SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>,std::equal_to<unsigned int>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>>>,
1>            T=size_t
1>        ]
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(363): note: see reference to function template instantiation 'void CommonTests::MultiSetTests::PRIVATE_::Test1_MiscStarterTests_::SimpleMultiSetTests_<DEFAULT_TESTING_SCHEMA>(const DEFAULT_TESTING_SCHEMA &,Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<T>> &)' being compiled
1>        with
1>        [
1>            DEFAULT_TESTING_SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>,std::equal_to<unsigned int>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>>>,
1>            T=size_t
1>        ]
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(481): note: see reference to function template instantiation 'void CommonTests::MultiSetTests::PRIVATE_::Test1_MiscStarterTests_::DoAllTests_<DEFAULT_TESTING_SCHEMA>(const DEFAULT_TESTING_SCHEMA &)' being compiled
1>        with
1>        [
1>            DEFAULT_TESTING_SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>,std::equal_to<unsigned int>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>>>
1>        ]
1>c:\sandbox\stroika\devroot\tests\17\test.cpp(38): note: see reference to function template instantiation 'void CommonTests::MultiSetTests::All_For_Type<SCHEMA>(const DEFAULT_TESTING_SCHEMA &)' being compiled
1>        with
1>        [
1>            SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>,std::equal_to<unsigned int>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>>>,
1>            DEFAULT_TESTING_SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>,std::equal_to<unsigned int>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>>>
1>        ]
1>c:\sandbox\stroika\devroot\tests\17\test.cpp(81): note: see reference to function template instantiation 'void `anonymous-namespace'::DoTestForConcreteContainer_<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<T>>,CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER,std::equal_to<unsigned int>,CommonTests::MultiSetTests::DefaultFactory<CONCRETE_CONTAINER>>>(const SCHEMA &)' being compiled
1>        with
1>        [
1>            T=size_t,
1>            CONCRETE_CONTAINER=Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>,
1>            SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>,std::equal_to<unsigned int>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<size_t,Stroika::Foundation::Containers::DefaultTraits::MultiSet<size_t>>>>
1>        ]
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(95): error C2143: syntax error: missing ';' before '{'
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(95): error C2061: syntax error: identifier 'CountedValue<Stroika::SimpleClass,unsigned int>'
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(190): note: see reference to function template instantiation 'void CommonTests::MultiSetTests::PRIVATE_::Test1_MiscStarterTests_::MultiSetIteratorTests_<DEFAULT_TESTING_SCHEMA>(const DEFAULT_TESTING_SCHEMA &,Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<T>> &)' being compiled
1>        with
1>        [
1>            DEFAULT_TESTING_SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>,std::equal_to<Stroika::SimpleClass>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>>>,
1>            T=Stroika::SimpleClass
1>        ]
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(363): note: see reference to function template instantiation 'void CommonTests::MultiSetTests::PRIVATE_::Test1_MiscStarterTests_::SimpleMultiSetTests_<DEFAULT_TESTING_SCHEMA>(const DEFAULT_TESTING_SCHEMA &,Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<T>> &)' being compiled
1>        with
1>        [
1>            DEFAULT_TESTING_SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>,std::equal_to<Stroika::SimpleClass>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>>>,
1>            T=Stroika::SimpleClass
1>        ]
1>c:\sandbox\stroika\devroot\tests\testcommon\commontests_multiset.h(481): note: see reference to function template instantiation 'void CommonTests::MultiSetTests::PRIVATE_::Test1_MiscStarterTests_::DoAllTests_<DEFAULT_TESTING_SCHEMA>(const DEFAULT_TESTING_SCHEMA &)' being compiled
1>        with
1>        [
1>            DEFAULT_TESTING_SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>,std::equal_to<Stroika::SimpleClass>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>>>
1>        ]
1>c:\sandbox\stroika\devroot\tests\17\test.cpp(38): note: see reference to function template instantiation 'void CommonTests::MultiSetTests::All_For_Type<SCHEMA>(const DEFAULT_TESTING_SCHEMA &)' being compiled
1>        with
1>        [
1>            SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>,std::equal_to<Stroika::SimpleClass>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>>>,
1>            DEFAULT_TESTING_SCHEMA=CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>,std::equal_to<Stroika::SimpleClass>,CommonTests::MultiSetTests::DefaultFactory<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<Stroika::SimpleClass>>>>
1>        ]
1>c:\sandbox\stroika\devroot\tests\17\test.cpp(82): note: see reference to function template instantiation 'void `anonymous-namespace'::DoTestForConcreteContainer_<Stroika::Foundation::Containers::MultiSet<Stroika::SimpleClass,Stroika::Foundation::Containers::DefaultTraits::MultiSet<T>>,CommonTests::MultiSetTests::DEFAULT_TESTING_SCHEMA<CONCRETE_CONTAINER,std::equal_to<Stroika::SimpleClass>,CommonTests::MultiSetTests::DefaultFactory<CONCRETE_CONTAINER>>>(const SCHEMA &)' being compiled
1>        with
1>        [
1>            T=Stroika::SimpleClass,
*/
#ifndef qCompilerAndStdLib_attributes_before_template_in_Template_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_attributes_before_template_in_Template_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_attributes_before_template_in_Template_Buggy 0
#endif

#endif

/**
 *
 On VS2k17:
         Compiling Library/Sources/Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.cpp ... 
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\collection.h(192): fatal error C1001: An internal error has occurred in the compiler.
(compiler file 'msc1.cpp', line 1518)
 To work around this problem, try simplifying or changing the program near the locations listed above.
Please choose the Technical Support command on the Visual C++ 
 Help menu, or open the Technical Support help file for more information
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\collection.h(369): note: see reference to class template instantiation 'Stroika::Foundation::Containers::Collection<T>' being compiled
c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\filesystem(2392): note: see reference to class template instantiation 'std::chrono::time_point<std::filesystem::_File_time_clock,std::filesystem::_File_time_clock::duration>' being compiled
c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\type_traits(616): note: see reference to class template instantiation 'std::basic_string_view<wchar_t,std::char_traits<wchar_t>>' being compiled
c:\program files (x86)\microsoft visual studio\2017\community\vc\tools\msvc\14.16.27023\include\xstring(2124): note: see reference to class template instantiation 'std::is_convertible<const _StringViewIsh &,std::basic_string_view<wchar_t,std::char_traits<wchar_t>>>' being compiled



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
#define qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#elif defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#elif defined(__clang__) && !defined(__APPLE__)
// broken in clang++-13
#define qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#else
#define qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy 0
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
 *                []  (19 seconds)  [48]  Foundation::Time  (../Builds/Debug-U-32/Test48/Test48.exe) crash/assert failure
 *                  NOTE - assert error so only fails on DEBUG builds

 As of 
    _STL_VERIFY(_Strbuf != nullptr, "istreambuf_iterator is not dereferencable"); asserts out

    https://developercommunity.visualstudio.com/t/qcompilerandstdlib-std-get-time-pctx-buggy/1359575

    REPORTED 2021-03-05 APX

 */
#ifndef qCompilerAndStdLib_std_get_time_pctx_Buggy

#if defined(_MSC_VER)
// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
// still broken in _MS_VS_2k17_15Pt6Pt0_
// still broken in _MS_VS_2k17_15Pt7Pt1_
// assume broken in _MS_VS_2k17_15Pt7Pt2_
// assume broken in _MS_VS_2k17_15Pt7Pt3_
// assume broken in _MS_VS_2k17_15Pt7Pt4_
// assume broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// still broken in _MSC_VER_2k17_15Pt8_
// VERIFIED STILL BROKEN in _MSC_VER_2k17_15Pt9_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt0_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt1_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt2_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt3_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt4_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt5_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt6_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt7_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt8_
// VERIFIED STILL BROKEN in _MSC_VER_2k19_16Pt10_
// VERIFIED STILL BROKEN in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_std_get_time_pctx_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_std_get_time_pctx_Buggy 0
#endif

#endif

/*
 *          error C2131: expression did not evaluate to a constant
>c:\sandbox\stroikadev\tests\37\test.cpp(203): error C2127: 'kOrigValueInit_': illegal initialization of 'constexpr' entity with a non-constant expression
*/
#ifndef qCompilerAndStdLib_constexpr_stdinitializer_Buggy

#if defined(_MSC_VER)
// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
// still broken in _MS_VS_2k17_15Pt6Pt0_
// still broken in _MS_VS_2k17_15Pt7Pt1_
// assume broken in _MS_VS_2k17_15Pt7Pt2_
// still broken in _MS_VS_2k17_15Pt7Pt3_
// assume broken in _MS_VS_2k17_15Pt7Pt4_
// assume broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// still broken in _MSC_VER_2k17_15Pt8_
// VERIFIED FIXED in _MSC_VER_2k17_15Pt9_
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy 0
#endif

#endif

/*
* 
* REPORTED IN https://developercommunity.visualstudio.com/t/static_initializer_lambda_funct_init_Bug/1287411
* 
*   SUPPOSEDLY FIXED in 16.10 accoridng to above
* 
* 
 Compiling Samples/WebService/Sources/Model.cpp ...
C:\Sandbox\Stroika\DevRoot\Samples\WebService\Sources\Model.cpp(98): fatal error C1001: Internal compiler error.
(compiler file 'msc1.cpp', line 1584)
 To work around this problem, try simplifying or changing the program near the locations listed above.
If possible please provide a repro here: https://developercommunity.visualstudio.com
Please choose the Technical Support command on the Visual C++
 Help menu, or open the Technical Support help file for more information
INTERNAL COMPILER ERROR in 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29333\bin\HostX64\x64\cl.exe'
    Please choose the Technical Support command on the Visual C++
*/
#ifndef qCompilerAndStdLib_static_initializer_lambda_funct_init_Buggy
#if defined(_MSC_VER)
// First broken in _MSC_VER_2k19_16Pt8_
// APPEARS FIXED IN _MSC_VER_2k19_16Pt10_
#define qCompilerAndStdLib_static_initializer_lambda_funct_init_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER == _MSC_VER_2k19_16Pt8_)
#else
#define qCompilerAndStdLib_static_initializer_lambda_funct_init_Buggy 0
#endif
#endif

/**
1>   Compiling Library/Sources/Stroika/Foundation/Cryptography/OpenSSL/CipherAlgorithm.cpp ...
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Cryptography\OpenSSL\CipherAlgorithm.cpp(89): fatal error C1001: Internal compiler error.
1>(compiler file 'msc1.cpp', line 1588)
1> To work around this problem, try simplifying or changing the program near the locations listed above.
1>If possible please provide a repro here: https://developercommunity.visualstudio.com
1>Please choose the Technical Support command on the Visual C++
1> Help menu, or open the Technical Support help file for more information
1>INTERNAL COMPILER ERROR in 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29910\bin\HostX64\x86\cl.exe'
1>    Please choose the Technical Support command on the Visual C++
1>    Help menu, or open the Technical Support help file for more information
 */
#ifndef qCompilerAndStdLib_const_extern_declare_then_const_define_namespace_Buggy
#if defined(_MSC_VER)
// First broken in _MSC_VER_2k19_16Pt8_
// APPEARS FIXED IN _MSC_VER_2k19_16Pt10_
#define qCompilerAndStdLib_const_extern_declare_then_const_define_namespace_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER == _MSC_VER_2k19_16Pt8_)
#else
#define qCompilerAndStdLib_const_extern_declare_then_const_define_namespace_Buggy 0
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
// First broken in _MSC_VER_2k19_16Pt8_
// still broken in _MSC_VER_2k19_16Pt10_
// still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k19_16Pt8_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_altComPtrCvt2ComQIPtrRequiresExtraCast_Buggy 0
#endif
#endif

/*
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(317,33): error C2672:  'Stroika::Foundation::Memory::NEltsOf_REAL_': no matching overloaded function found
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(326,13): error C2893:  Failed to specialize function template 'size_t Stroika::Foundation::Memory::NEltsOf_REAL_(const ARRAY_TYPE (&)[SIZE_OF_ARRAY])'
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(326,13): message :  With the following template arguments:
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(326,13): message :  'ARRAY_TYPE=Stroika::Foundation::Common::KeyValuePair<double,unsigned int>'
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(326,13): message :  'SIZE_OF_ARRAY=0'
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(326,13): error C3316:  'const Stroika::Foundation::Common::KeyValuePair<double,unsigned int> []': an array of unknown size cannot be used in a range-based for statement
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(319,78): error C2065:  'i': undeclared identifier
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(319,49): error C2064:  term does not evaluate to a function taking 1 arguments
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(326,13): message :  class does not define an 'operator()' or a user defined conversion operator to a pointer-to-function or reference-to-function that takes appropriate number of arguments
    1>C:\Sandbox\Stroika\DevRoot\Tests\45\Test.cpp(321,75): error C2065:  'i': undeclared identifier
*/
#ifndef qCompilerAndStdLib_constexpr_KeyValuePair_array_stdinitializer_Buggy

#if defined(_MSC_VER)
// WORKS anytime before _MS_VS_2k19_16Pt1Pt0_
// First broken in _MS_VS_2k19_16Pt1Pt0_
// broken in _MSC_VER_2k19_16Pt2_
// verified still broken in _MSC_VER_2k19_16Pt3_
// Verified FIXED in _MSC_VER_2k19_16Pt4_
#define qCompilerAndStdLib_constexpr_KeyValuePair_array_stdinitializer_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt3_)
#else
#define qCompilerAndStdLib_constexpr_KeyValuePair_array_stdinitializer_Buggy 0
#endif

#endif

/*
>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Configuration\Enumeration.inl(111): message :  see reference to function template instantiation 'void Stroika::Foundation::Configuration::EnumNames<Stroika::Foundation::Cryptography::OpenSSL::CipherAlgorithm>::RequireItemsOrderedByEnumValue_(void) const' being compiled (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\OpenSSL\CipherAlgorithm.cpp)
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Configuration\Enumeration.inl(208,1): error C1001:  An internal error has occurred in the compiler.
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Configuration\Enumeration.inl(208,1): error C1001: (compiler file 'd:\agent\_work\3\s\src\vctools\Compiler\CxxFE\sl\p1\c\yyaction.cpp', line 1187)
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Configuration\Enumeration.inl(208,1): error C1001:  To work around this problem, try simplifying or changing the program near the locations listed above.
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Configuration\Enumeration.inl(208,1): error C1001: Please choose the Technical Support command on the Visual C++
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Configuration\Enumeration.inl(208,1): error C1001:  Help menu, or open the Technical Support help file for more information (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\OpenSSL\DigestAlgorithm.cpp)
1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Configuration\Enumeration.inl(205): message :  see reference to function template instantiation 'void Stroika::Foundation::Configuration::EnumNames<Stroika::Foundation::Cryptography::OpenSSL::DigestAlgorithm>::RequireItemsOrderedByEnumValue_(void) const' being compiled (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\OpenSSL\DigestAlgorithm.cpp)
*/
#ifndef qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy

#if defined(_MSC_VER)
// First noticed broken in _MSC_VER_2k19_16Pt1_
// VERIFIED BROKEN _MSC_VER_2k19_16Pt2_
// Fixed in _MSC_VER_2k19_16Pt3_
#define qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt2_)
#else
#define qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy 0
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
// VERIFIED BROKEN IN GCC8
// VERIFIED BROKEN IN GCC9
// VERIFIED BROKEN IN GCC 9.2
// VERIFIED BROKEN IN GCC 9.3
// VERIFIED BROKEN IN GCC 10.0
// VERIFIED BROKEN IN GCC 10.2
// VERIFIED BROKEN IN GCC 11
#define qCompilerAndStdLib_template_DefaultArgIgnoredWhenFailedDeduction_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 11)
#else
#define qCompilerAndStdLib_template_DefaultArgIgnoredWhenFailedDeduction_Buggy 0
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

#ifndef qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// First found BROKEN IN GCC 10.0
// VERIFIED BROKEN IN GCC 10.2
#define qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ == 10)
#else
#define qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_GenericLambdaInsideGenericLambdaAssertCall_Buggy

// Generates internal compiler error
#if defined(_MSC_VER)
// First noticed broken _MSC_VER_2k19_16Pt2_
// Fixed in _MSC_VER_2k19_16Pt3_
#define qCompilerAndStdLib_GenericLambdaInsideGenericLambdaAssertCall_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt2_)
#else
#define qCompilerAndStdLib_GenericLambdaInsideGenericLambdaAssertCall_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_template_GenericLambdaInsideGenericLambdaDeductionInternalError_Buggy

// Generates internal compiler error
#if defined(_MSC_VER)
// First noticed broken _MSC_VER_2k19_16Pt2_
// Fixed in _MSC_VER_2k19_16Pt3_
#define qCompilerAndStdLib_template_GenericLambdaInsideGenericLambdaDeductionInternalError_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt2_)
#else
#define qCompilerAndStdLib_template_GenericLambdaInsideGenericLambdaDeductionInternalError_Buggy 0
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
// First noticed broken _MSC_VER_2k19_16Pt5_
// still broken in _MSC_VER_2k19_16Pt6_
// still broken in _MSC_VER_2k19_16Pt7_
// still broken in _MSC_VER_2k19_16Pt8_
// still broken in _MSC_VER_2k19_16Pt10_
// still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_template_template_call_SequentialEquals_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_template_template_call_SequentialEquals_Buggy 0
#endif

#endif

/**
 * According to https://en.cppreference.com/w/cpp/error/error_category/error_category ctor is constexpr since c++14
 */
#ifndef qCompilerAndStdLib_constexpr_error_category_ctor_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// VERIFIED BROKEN IN GCC8
// VERIFIED FIXED in GCC9
#define qCompilerAndStdLib_constexpr_error_category_ctor_Buggy (__GNUC__ <= 8)
#elif defined(_MSC_VER)
// verified still BROKEN in _MSC_VER_2k19_16Pt0_ (preview2)
// SEEMS to work or maybe I dont have well recorded the problem. But no obvious problems with: _MS_VS_2k19_16Pt0Pt0pre43_
#define qCompilerAndStdLib_constexpr_error_category_ctor_Buggy (_MSC_FULL_VER <= _MS_VS_2k19_16Pt0Pt0pre4_)
#else
#define qCompilerAndStdLib_constexpr_error_category_ctor_Buggy 0
#endif

#endif

// Run regtest 35 (Foundation::Execution::Exceptions) to see if fails
#ifndef qCompilerAndStdLib_error_code_compare_condition_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// https://stackoverflow.com/questions/44405394/how-to-portably-compare-stdsystem-error-exceptions-to-stderrc-values
#if __GNUC__ <= 8
#define qCompilerAndStdLib_error_code_compare_condition_Buggy (__GNUC_MINOR__ <= 3)
// VERIFIED FIXED in GCC9
#else
#define qCompilerAndStdLib_error_code_compare_condition_Buggy 0
#endif
#elif defined(__GLIBCXX__) && __GLIBCXX__ <= GLIBCXX_8x_
#define qCompilerAndStdLib_error_code_compare_condition_Buggy 1
#else
#define qCompilerAndStdLib_error_code_compare_condition_Buggy 0
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
#if defined(__GLIBCXX__) && __GLIBCXX__ <= GLIBCXX_11x_
#define qCompilerAndStdLib_locale_time_get_PCTM_RequiresLeadingZero_Buggy 1
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
 */
#ifndef qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy
#if defined(__GLIBCXX__) && __GLIBCXX__ <= GLIBCXX_11x_
#define qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy 1
#else
#define qCompilerAndStdLib_locale_time_get_date_order_no_order_Buggy 0
#endif
#endif

/*
Builds/g++-8-debug-c++17/Tests/Test41
=================================================================
==11117==ERROR: AddressSanitizer: stack-use-after-scope on address 0x7ffeaef13750 at pc 0x55fa63931042 bp 0x7ffeaef13680 sp 0x7ffeaef13670
WRITE of size 4 at 0x7ffeaef13750 thread T0
    #0 0x55fa63931041 in Stroika::Foundation::Characters::String::FromASCII(char const*, char const*) /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/String.cpp:352
    #1 0x55fa63d84a02 in Stroika::Foundation::Characters::String::FromASCII(char const*) ../../Characters/Concrete/../String.inl:188
    #2 0x55fa63f0263f in Stroika::Foundation::Characters::String Stroika::Foundation::IO::Network::InternetAddress::As<Stroika::Foundation::Characters::String>() const /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/IO/Network/InternetAddress.cpp:188
    #3 0x55fa63f4b745 in Stroika::Foundation::IO::Network::URL::Authority::Host::EncodeAsRawURL_(Stroika::Foundation::IO::Network::InternetAddress const&) /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/IO/Network/URL.cpp:171
    #4 0x55fa638f83d8 in Stroika::Foundation::IO::Network::URL::Authority::Host::Host(Stroika::Foundation::IO::Network::InternetAddress const&) (/Sandbox/Stroika-Dev/Builds/Debug/Tests/Test41+0x19e43d8)
    #5 0x55fa638e619c in TestHostParsing_ /Sandbox/Stroika-Dev/Tests/41/Test.cpp:267
    #6 0x55fa638e7dae in DoTests_ /Sandbox/Stroika-Dev/Tests/41/Test.cpp:299
    #7 0x55fa638f0411 in DoRegressionTests_ /Sandbox/Stroika-Dev/Tests/41/Test.cpp:440
    #8 0x55fa63925288 in Stroika::TestHarness::PrintPassOrFail(void (*)()) /Sandbox/Stroika-Dev/Tests/41/../TestHarness/TestHarness.cpp:75
    #9 0x55fa638f0443 in main /Sandbox/Stroika-Dev/Tests/41/Test.cpp:450
    #10 0x7fc2d310909a in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x2409a)
    #11 0x55fa637d0f19 in _start (/Sandbox/Stroika-Dev/Builds/Debug/Tests/Test41+0x18bcf19)

Address 0x7ffeaef13750 is located in stack of thread T0 at offset 144 in frame
    #0 0x55fa63930d87 in Stroika::Foundation::Characters::String::FromASCII(char const*, char const*) /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/String.cpp:345

  This frame has 1 object(s):
    [32, 4160) 'buf' <== Memory access at offset 144 is inside this variable
HINT: this may be a false positive if your program uses some custom stack unwind mechanism or swapcontext
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-use-after-scope /Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Characters/String.cpp:352 in Stroika::Foundation::Characters::String::FromASCII(char const*, char const*)
Shadow bytes around the buggy address:
  0x100055dda690: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100055dda6a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100055dda6b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100055dda6c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100055dda6d0: 00 00 00 00 00 00 00 00 f1 f1 f1 f1 00 00 00 00
=>0x100055dda6e0: 00 00 00 00 00 00 00 00 00 00[f8]f8 f8 f8 f8 f8
  0x100055dda6f0: f8 f8 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100055dda700: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100055dda710: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100055dda720: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100055dda730: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==11117==ABORTING
*/
#ifndef qCompiler_Sanitizer_stack_use_after_scope_asan_premature_poison_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// Appears fixed in GCC 9.0 (ubuntu 1904)
#define qCompiler_Sanitizer_stack_use_after_scope_asan_premature_poison_Buggy ((__GNUC__ <= 8))
#else
#define qCompiler_Sanitizer_stack_use_after_scope_asan_premature_poison_Buggy 0
#endif

#endif

// Debug builds - only fails running samples - not tests - crashes
// ./Builds/Debug/Samples-SystemPerformanceClient/SystemPerformanceClient - MacOS Only
#ifndef qCompiler_LimitLengthBeforeMainCrash_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Appears still BROKEN in XCODE 12
// Appears still BROKEN in XCODE 13
#define qCompiler_LimitLengthBeforeMainCrash_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((11 <= __clang_major__) && (__clang_major__ <= 13))
#else
#define qCompiler_LimitLengthBeforeMainCrash_Buggy 0
#endif

#endif

/**
 * (compiler file 'd:\agent\_work\3\s\src\vctools\Compiler\CxxFE\sl\p1\c\ParseTreeActions.cpp', line 5799)
 To work around this problem, try simplifying or changing the program near the locations listed above.
Please choose the Technical Support command on the Visual C++
 Help menu, or open the Technical Support help file for more information
C:\Sandbox\Stroika\DevRoot\Tests\32\Test.cpp(621): note: while compiling class template member function 'std::shared_ptr<Stroika::Foundation::DataExchange::StructuredStreamEvents::ObjectReader::IElementConsumer> `anonymous-namespace'::T7_SAXObjectReader_BLKQCL_ReadSensors_::TunerMappingReader_<`anonymous-namespace'::T7_SAXObjectReader_BLKQCL_ReadSensors_::TemperatureType_>::HandleChildStart(const Stroika::Foundation::DataExchange::StructuredStreamEvents::Name &)'
C:\Sandbox\Stroika\DevRoot\Tests\32\Test.cpp(656): note: see reference to class template insta

C:\Sandbox\Stroika\DevRoot\Tests\32\Test.cpp(851): fatal error C1001: An internal error has occurred in the compiler.
(compiler file 'd:\agent\_work\5\s\src\vctools\Compiler\CxxFE\sl\p1\c\ParseTreeActions.cpp', line 6142)
 To work around this problem, try simplifying or changing the program near the locations listed above.
Please choose the Technical Support command on the Visual C++
 Help menu, or open the Technical Support help file for more information

STILL:
28>Linking Builds/Debug-U-32/Tests/Test27.exe ...
33>C:\Sandbox\Stroika\DevRoot\Tests\32\Test.cpp(851): fatal error C1001: Internal compiler error.
33>(compiler file 'd:\agent\_work\4\s\src\vctools\Compiler\CxxFE\sl\p1\c\ParseTreeActions.cpp', line 6160)
33> To work around this problem, try simplifying or changing the program near the locations listed above.
33>If possible please provide a repro here: https://developercommunity.visualstudio.com
33>Please choose the Technical Support command on the Visual C++
33> Help menu, or open the Technical Support help file for more information
 */
#ifndef qCompilerAndStdLib_using_in_template_invoke_other_template_Buggy

#if defined(_MSC_VER)

// FIRST broken in _MSC_VER_2k19_16Pt3_
// verified broken in _MSC_VER_2k19_16Pt4_
// verified broken in _MSC_VER_2k19_16Pt5_
// FIXED in _MSC_VER_2k19_16Pt6_
#define qCompilerAndStdLib_using_in_template_invoke_other_template_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt5_)
#else
#define qCompilerAndStdLib_using_in_template_invoke_other_template_Buggy 0
#endif

#endif

/**
 * Internal compiler error building ActiveLedIt
 */
#ifndef qCompilerAndStdLib_ATL_Assign_wstring_COMOBJ_Buggy

#if defined(_MSC_VER)

// FIRST broken in _MSC_VER_2k19_16Pt3_
#define qCompilerAndStdLib_ATL_Assign_wstring_COMOBJ_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt3_)
#else
#define qCompilerAndStdLib_ATL_Assign_wstring_COMOBJ_Buggy 0
#endif

#endif

/*
 *  https://timsong-cpp.github.io/cppwp/draft.pdf documents
            if (n > 5) [[unlikely]] { // n > 5 is considered to be arbitrarily unlikely
                g(0);
                return n * 2 + 1;
            }
 *  But with gcc 9, this generates:
        ../Containers/../Configuration/Enumeration.inl: In member function 'ENUM_TYPE Stroika::Foundation::Configuration::EnumNames<ENUM_TYPE>::GetValue(const wchar_t*, const NOT_FOUND_EXCEPTION&) const':
        ../Containers/../Configuration/Enumeration.inl:190:13: warning: attributes at the beginning of statement are ignored [-Wattributes]
          190 |             [[UNLIKELY_ATTR]]

 */
#ifndef qCompiler_MisinterpretAttributeOnCompoundStatementAsWarningIgnored_Buggy

#if defined(__GNUC__) && !defined(__clang__)
// Broken in GCC 9.0
// Verified still broken in 9.2
// Verified still broken in 9.3
// Verfiried fixed in GCC 10.0
#define qCompiler_MisinterpretAttributeOnCompoundStatementAsWarningIgnored_Buggy ((__GNUC__ == 9))
#else
#define qCompiler_MisinterpretAttributeOnCompoundStatementAsWarningIgnored_Buggy 0
#endif

#endif

// You get double delete/shared_ptr failure on Test43 - IO:Transfer::Cache regression test
// Issue is that static inline not implemented properly in vs2k17 and generates multiple definitions.
// So you must add INGOREDUPDEFS linker flag, and then you get multiple construction (mostly harmless)
// followed by multiple destructions (very harmful) - and this error. So must avoid static inline for anything
// with a DTOR.
// NOTE - HAD BEEN USING qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy - for a while, but got rid of that 2021-06-16 and just use this define
//
#ifndef qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy
#if defined(_MSC_VER)
// first broken in in _MS_VS_2k17_15Pt9Pt7_
#define qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompiler_cpp17InlineStaticMemberOfClassDoubleDeleteAtExit_Buggy 0
#endif
#endif

/*
  Range.cpp
  ****NOTE - WARNING NOT ERRR - BUT USELESS AND ANNOYING ****

2>c:\program files (x86)\microsoft visual studio 14.0\vc\include\xmemory(169): warning C4996: 'std::uninitialized_copy_n::_Unchecked_iterators::_Deprecate': Call to 'std::uninitialized_copy_n' with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
2>  c:\program files (x86)\microsoft visual studio 14.0\vc\include\xmemory(169): note: see declaration of 'std::uninitialized_copy_n::_Unchecked_iterators::_Deprecate'
2>  c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\externallysynchronizeddatastructures\array.inl(190): note: see reference to function template instantiation '_FwdIt std::uninitialized_copy_n<T*,std::size_t,T*>(_InIt,std::_Diff,_FwdIt)' being compiled
2>          with
2>          [
*/
#ifndef qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy

#if defined(_MSC_VER)
// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
// still broken in _MS_VS_2k17_15Pt6Pt0_
// still broken in _MS_VS_2k17_15Pt7Pt1_
// assume broken in _MS_VS_2k17_15Pt7Pt2_
// assume broken in _MS_VS_2k17_15Pt7Pt3_
// assume broken in _MS_VS_2k17_15Pt7Pt4_
// assume broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// Verified FIXED in _MSC_VER_2k17_15Pt8_
#define qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy (_MSC_VER <= _MSC_VER_2k17_15Pt7_)
#else
#define qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_stdOptionalThreeWayCompare_Buggy

#if defined(_MSC_VER)

// first found broken in _MSC_VER_2k19_16Pt8_
// appears fixed in _MSC_VER_2k19_16Pt10_
#define qCompilerAndStdLib_stdOptionalThreeWayCompare_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k19_16Pt8_)
#elif defined(_LIBCPP_VERSION)
// only seen on _LIBCPP_VERSION=11, but thats cuz requires c++20
#define qCompilerAndStdLib_stdOptionalThreeWayCompare_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION <= 11000)
#else
#define qCompilerAndStdLib_stdOptionalThreeWayCompare_Buggy 0
#endif

#endif

/*
 MUST RUN TEST    Foundation::Execution::Other
 and see if it passes/fails - maybe review log text
 */
#ifndef qCompilerAndStdLib_atomic_bool_initialize_before_main_Buggy

#if defined(_MSC_VER)

// verified broken in _MSC_VER_2k19_16Pt0_ (debug builds only)
// verified broken in _MSC_VER_2k19_16Pt1_ (debug builds only)
// verified broken in _MSC_VER_2k19_16Pt2_
// verified broken in _MSC_VER_2k19_16Pt3_
// verified broken in _MSC_VER_2k19_16Pt4_
// Appears FIXED in _MSC_VER_2k19_16Pt5_
#define qCompilerAndStdLib_atomic_bool_initialize_before_main_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt4_)
#else
#define qCompilerAndStdLib_atomic_bool_initialize_before_main_Buggy 0
#endif

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
 */
#ifndef qCompilerAndStdLib_cplusplus_macro_value_Buggy

#if defined(_MSC_VER)

// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
// still broken in _MS_VS_2k17_15Pt6Pt0_
// still broken in _MS_VS_2k17_15Pt7Pt1_
// assume broken in _MS_VS_2k17_15Pt7Pt2_
// release notes say fixed, but after testing, still broken in _MS_VS_2k17_15Pt7Pt3_
// still broken in _MS_VS_2k17_15Pt7Pt4_
// assume broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// still broken in _MSC_VER_2k17_15Pt8_
// still broken in _MSC_VER_2k17_15Pt9_
// still broken in _MSC_VER_2k19_16Pt0_
// CONFUSED ABOUT --- seems fixed when I run msbuild from make cmdline but fail from IDE?_MS_VS_2k19_16Pt0Pt0pre2_
// CONFUSED ABOUT --- seems fixed when I run msbuild from make cmdline but fail from IDE?_MS_VS_2k19_16Pt0Pt0pre3_
// still broken in _MS_VS_2k19_16Pt0Pt0pre43_ (aka _MS_VS_2k19_16Pt0Pt0_) (running from visual studio gui)
// still broken in _MSC_VER_2k19_16Pt1_
// still broken in _MSC_VER_2k19_16Pt2_
// still broken in _MSC_VER_2k19_16Pt3_
// verified still broken in _MSC_VER_2k19_16Pt4_
// verified still broken in _MSC_VER_2k19_16Pt5_
// verified still broken in _MSC_VER_2k19_16Pt6_
// verified still broken in _MSC_VER_2k19_16Pt7_
// verified still broken in _MSC_VER_2k19_16Pt8_
// verified still broken in _MSC_VER_2k19_16Pt10_
// verified still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy 0
#endif

#endif

/*
 *
 *  c:\sandbox\stroika\devroot-v2.0\library\sources\stroika\foundation\containers\sortedmapping.h(100): fatal error C1001
: An internal error has occurred in the compiler. [C:\Sandbox\Stroika\DevRoot-v2.0\Library\Projects\VisualStudio.Net-
2017\Stroika-Foundation.vcxproj]

BREAKS
        using _SetRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

WORKAROUND:
        using _SetRepSharedPtr = Memory::SharedPtr<_IRep>;

    Error reported is elsewhere where the typedef is used. Happens about 15 times in Stroika.
*/
#ifndef qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k17_15Pt8_  (_MS_VS_2k17_15Pt8Pt0_)
// still broken in _MS_VS_2k17_15Pt8Pt4_
// assume still broken in _MS_VS_2k17_15Pt8Pt5_
// assume still broken in _MS_VS_2k17_15Pt8Pt8_
// verified still broken in _MSC_VER_2k17_15Pt9_
// verified still broken in _MSC_VER_2k19_16Pt0Pt0_ - preview1
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre2_
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre3_
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre43_ (aka _MS_VS_2k19_16Pt0Pt0_)
// verified still broken _MSC_VER_2k19_16Pt1_
// verified still broken _MSC_VER_2k19_16Pt2_
// verified still broken in _MSC_VER_2k19_16Pt3_
// verified fixed in _MSC_VER_2k19_16Pt4_
#define qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy (_MSC_VER_2k17_15Pt8_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k19_16Pt3_)
#else
#define qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy 0
#endif

#endif

/*
VS2k says:
     *1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Common\Compare.inl(35): error C2672: 'make_from_tuple': no matching overloaded function found (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\VariantValue.cpp)
    1>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\DataExchange\VariantValue.cpp(832): note: see reference to function template instantiation 'int Stroika::Foundation::Common::ThreeWayComparer<Stroika::Foundation::Containers::Sequence<Stroika::Foundation::DataExchange::VariantValue>>::operator ()<T,0x0>(const T &,const T &) const' being compiled

    
clang says:
    n file included from ./../Characters/FloatConversion.h:15:
    In file included from ./../Characters/../Containers/../Traversal/../Characters/String.h:13:
    In file included from ./../Characters/../Containers/../Memory/../Common/Compare.h:412:
    ./../Characters/../Common/Compare.inl:38:44: error: typename specifier refers to class template member in 'Stroika::Foundation::Containers::Sequence<Stroika::Foundation::DataExchange::VariantValue>';
          argument deduction not allowed here
            return make_from_tuple<typename Q::ThreeWayComparer> (fArgs_) (lhs, rhs);
                                               ^
    VariantValue.cpp:832:72: note: in instantiation of function template specialization
          'Stroika::Foundation::Common::ThreeWayComparer<Stroika::Foundation::Containers::Sequence<Stroika::Foundation::DataExchange::VariantValue>>::operator()<Stroika::Foundation::Containers::Sequence<Stroika
*/
#ifndef qCompilerAndStdLib_make_from_tuple_Buggy

#if defined(__clang__) && !defined(__APPLE__)
// Appears FIXED with clang++-9
#define qCompilerAndStdLib_make_from_tuple_Buggy ((__clang_major__ <= 8))
#elif defined(_MSC_VER)
// verified broken _MS_VS_2k19_16Pt0Pt0_
// verified FIXED in _MSC_VER_2k19_16Pt1_
#define qCompilerAndStdLib_make_from_tuple_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt0_)
#else
#define qCompilerAndStdLib_make_from_tuple_Buggy 0
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

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 12))
#elif defined(__clang__) && !defined(__APPLE__)
// First noted in C++20 mode on clang++-10
// broken in clang++-11
// broken in clang++-12
// broken in clang++-13
#define qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#else
#define qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy 0
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

#if defined(__clang__) && defined(__APPLE__)
// WAG?
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 12))
#elif defined(__clang__) && !defined(__APPLE__)
// First noted in C++20 mode on clang++-12
// https://en.cppreference.com/w/cpp/20 says maybe fixed in clang++13 or 14
// appears fixed in clang++-13
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 12))
#elif defined(__GNUC__) && !defined(__clang__)
// VERIFIED BROKEN IN GCC 11
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 12)
#else
#define qCompilerAndStdLib_lambdas_in_unevaluatedContext_Buggy 0
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

#if defined(_MSC_VER)
#define qCompilerAndStdLib_to_chars_FP_Buggy (_MSC_VER < _MSC_VER_2k19_16Pt0_)
#elif defined(__GNUC__) && !defined(__clang__)
// according to https://en.cppreference.com/w/cpp/compiler_support fixed in gcc11
#define qCompilerAndStdLib_to_chars_FP_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 10)
#elif defined(__clang__) && defined(__APPLE__)
// according to https://en.cppreference.com/w/cpp/compiler_support not yet supported so WAG
#define qCompilerAndStdLib_to_chars_FP_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#elif defined(__clang__) && !defined(__APPLE__)
// according to https://en.cppreference.com/w/cpp/compiler_support not yet supported so WAG
// appears still broken in clang++13 (maybe should depend on stdlib version not compiler version)
#define qCompilerAndStdLib_to_chars_FP_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
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
#if defined(__GLIBCXX__)
// according to https://en.cppreference.com/w/cpp/compiler_support fixed in gcc11 (library so affects clang too if built with glibc)
#define qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GLIBCXX__ <= GLIBCXX_11x_)
#else
#define qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy 0
#endif
#endif

/*
/usr/lib/gcc/x86_64-linux-gnu/8/../../../../include/c++/8/charconv:632: undefined reference to `__muloti4'
/home/lewis/Sandbox/Stroika-Build-Dir-Ubuntu1804_x86_64/Builds/my-clang++-7-release-libstdc++/Stroika-Foundation.a(VariantValue.o): In function `bool std::__detail::__raise_and_add<unsigned long>(unsigned long&, int, unsigned char)':
*/
#ifndef qCompilerAndStdLib_to_chars_INT_Buggy

#if defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_to_chars_INT_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 7))
#else
#define qCompilerAndStdLib_to_chars_INT_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Appears broken on XCode 13
#define qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#elif defined(__clang__) && !defined(__APPLE__)
// appears still broken in clang++-13
#define qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#else
#define qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy 0
#endif

#endif

/*
In file included from ./../Characters/../Containers/Concrete/Sequence_stdvector.h:112:
In file included from ./../Characters/../Containers/Concrete/Sequence_stdvector.inl:18:
./../Characters/../Containers/Concrete/../Private/IteratorImplHelper.h:78:184: error: missing default argument on parameter 'args'
        explicit IteratorImplHelper_ (const DATASTRUCTURE_CONTAINER* data, const ContainerDebugChangeCounts_* changeCounter = nullptr, ADDITIONAL_BACKEND_ITERATOR_CTOR_ARGUMENTS&&... args);
                                                                                                                                                                                       ^
/Applications/Xcode_12.4.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../include/c++/v1/memory:3033:32: note: in instantiation of function template 
*/
#ifndef qCompilerAndStdLib_template_default_arguments_then_paramPack_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Appears broken on XCode 12
// BUt fixed in XCode 13
#define qCompilerAndStdLib_template_default_arguments_then_paramPack_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_template_default_arguments_then_paramPack_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
#else
#define qCompilerAndStdLib_template_default_arguments_then_paramPack_Buggy 0
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
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#elif defined(__clang__) && !defined(__APPLE__)
// still broken in clang++-8
// still broken in clang++-9
// still broken in clang++-10
// still broken in clang++-11
// still broken in clang++-12
// still broken in clang++-13
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#elif defined(_MSC_VER)
// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
// still broken in _MS_VS_2k17_15Pt6Pt0_
// still broken in _MS_VS_2k17_15Pt7Pt1_
// assume still broken in _MS_VS_2k17_15Pt7Pt2_
// assume still broken in _MS_VS_2k17_15Pt7Pt3_
// still broken in _MS_VS_2k17_15Pt7Pt4_
// assume broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// verified still broken in _MSC_VER_2k17_15Pt8_
// verified still broken in _MSC_VER_2k17_15Pt9_
// verified still broken in _MSC_VER_2k19_16Pt0_
// verified still broken in _MSC_VER_2k19_16Pt1_
// verified still broken in _MSC_VER_2k19_16Pt2_
// verified still broken in _MSC_VER_2k19_16Pt3_
// verified still broken in _MSC_VER_2k19_16Pt4_
// verified still broken in _MSC_VER_2k19_16Pt5_
// verified still broken in _MSC_VER_2k19_16Pt6_
// verified still broken in _MSC_VER_2k19_16Pt7_
// verified still broken in _MSC_VER_2k19_16Pt8_
// verified still broken in _MSC_VER_2k19_16Pt10_
// veriried still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy 0
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
// VERIFIED BROKEN on XCode 11.0
// VERIFIED BROKEN on XCode 12.0
// VERIFIED BROKEN on XCode 13.0
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#elif defined(__clang__) && !defined(__APPLE__)
// verified still broken in clang++-10
// verified still broken in clang++-11
// verified still broken in clang++-12
// verified still broken in clang++-13
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#else
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy 0
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
// Broken in _LIBCPP_VERSION  3900
// Broken in _LIBCPP_VERSION  4000
// Broken in _LIBCPP_VERSION  5000
// Broken in _LIBCPP_VERSION  6000
// Broken in _LIBCPP_VERSION  7000
// Broken in _LIBCPP_VERSION  8000
// never tested 9000
// Broken in _LIBCPP_VERSION  10000
// Broken in _LIBCPP_VERSION  11000
// Broken in _LIBCPP_VERSION  12000
// Broken in _LIBCPP_VERSION  13000
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION <= 13000))
#else
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_strong_ordering_equals_Buggy

#if defined(_LIBCPP_VERSION)
// Broken in _LIBCPP_VERSION  10000
#define qCompilerAndStdLib_strong_ordering_equals_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION <= 10000))
#else
#define qCompilerAndStdLib_strong_ordering_equals_Buggy 0
#endif

#endif

/**
 *  @see https://bugs.llvm.org/show_bug.cgi?id=37818
 *
        condition_variable::wait_for(unique_lock<mutex>& __lk,
                                     const chrono::duration<_Rep, _Period>& __d)
        {
            using namespace chrono;
            if (__d <= __d.zero())
                return cv_status::timeout;
           ...
        }

    But http://en.cppreference.com/w/cpp/thread/condition_variable/wait_for says:
        Atomically releases lock, blocks the current executing thread, and adds it to the list of threads waiting...

    but this wont ever unlock if time <= 0. Now that happens if you set sThreadAbortCheckFrequency_Default = 0 (maybe crazy) - but
    still - this is buggy for the C++ lib I think...

NOTE:
    libstdc++ does (https://gcc.gnu.org/onlinedocs/gcc-4.6.2/libstdc++/api/a00818_source.html)
    template<typename _Lock, typename _Clock, typename _Duration>
        00217       cv_status
        00218       wait_until(_Lock& __lock,
        00219          const chrono::time_point<_Clock, _Duration>& __atime)
        00220       {
        00221         unique_lock<mutex> __my_lock(_M_mutex);
        00222         __lock.unlock();
        00223         cv_status __status = _M_cond.wait_until(__my_lock, __atime);
        00224         __lock.lock();
        00225         return __status;
        00226       }
 */
#ifndef qCompilerAndStdLib_conditionvariable_waitfor_nounlock_Buggy

#if defined(_LIBCPP_VERSION)
#define qCompilerAndStdLib_conditionvariable_waitfor_nounlock_Buggy (_LIBCPP_VERSION <= 6000)
#else
#define qCompilerAndStdLib_conditionvariable_waitfor_nounlock_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_TemplateEqualsCompareOverload_Buggy

#if defined(_MSC_VER)
// first noted broken in _MSC_VER_2k19_16Pt6_
// Appears fixed in _MSC_VER_2k19_16Pt7_
#define qCompilerAndStdLib_TemplateEqualsCompareOverload_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt6_)
#else
#define qCompilerAndStdLib_TemplateEqualsCompareOverload_Buggy 0
#endif

#endif

/*
\sandbox\stroika\devroot\tests\13\test.cpp(64): error C2794: 'CombinedType': is not a member of any direct or indirect base class of 'Stroika::Foundation::Containers::Private_DataHyperRectangle_::NTemplate<int,Stroika::Foundation::Containers::DataHyperRectangle>::Helper_<__make_integer_sequence_t<_IntSeq,_T,2>>'
5>        with
5>        [
5>            _IntSeq=std::integer_sequence,
5>            _T=size_t
5>        ]
5>c:\sandbox\stroika\devroot\tests\13\test.cpp(64): note: see reference to alias template instantiation 'Stroika::Foundation::Containers::DataHyperRectangleN<int,2>' being compiled
5>c:\program files (x86)\microsoft vis
 */
#ifndef qCompilerAndStdLib_TemplateUsingOfTemplateOfTemplateSpecializationVariadic_Buggy

#if defined(_MSC_VER)
#define qCompilerAndStdLib_TemplateUsingOfTemplateOfTemplateSpecializationVariadic_Buggy (_MSC_VER <= _MSC_VER_2k17_15Pt9_)
#else
#define qCompilerAndStdLib_TemplateUsingOfTemplateOfTemplateSpecializationVariadic_Buggy 0
#endif

#endif

/*
 >c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\concrete\mapping_linkedlist.inl(55): error C2146: syntax error: missing ';' before identifier '_APPLY_ARGTYPE' (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\Atom.cpp)
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\concrete\mapping_linkedlist.inl(211): note: see reference to class template instantiation 'Stroika::Foundation::Containers::Concrete::Mapping_LinkedList<KEY_TYPE,MAPPED_VALUE_TYPE>::Rep_<KEY_EQUALS_COMPARER>' being compiled (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\Atom.cpp)
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\concrete\mapping_linkedlist.inl(55): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\Atom.cpp)
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\concrete\mapping_linkedlist.inl(55): error C2868: 'Stroika::Foundation::Containers::Concrete::Mapping_LinkedList<KEY_TYPE,MAPPED_VALUE_TYPE>::Rep_<KEY_EQUALS_COMPARER>::_APPLY_ARGTYPE': illegal syntax for using-declaration; expected qualified-name (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\Atom.cpp)
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\concrete\mapping_linkedlist.inl(56): error C2146: syntax error: missing ';' before identifier '_APPLYUNTIL_ARGTYPE' (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\Atom.cpp)
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\concrete\mapping_linkedlist.inl(56): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int (compiling source file ..\..\Sources\Stroika\Foundation\DataExchange\Atom.cpp)
*/
#ifndef qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy

#if defined(_MSC_VER)
// first broken in _MS_VS_2k17_15Pt7Pt1_ (probably broken in .0 but I never tested that)
// still broken in _MS_VS_2k17_15Pt7Pt2_
// assume still broken in _MS_VS_2k17_15Pt7Pt3_
// still broken in _MS_VS_2k17_15Pt7Pt4_
// assume broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// verified broken in _MSC_VER_2k17_15Pt8_
// verified still broken in _MS_VS_2k17_15Pt8Pt4_
// assume still broken in _MS_VS_2k17_15Pt8Pt5_
// verified still broken in _MSC_VER_2k17_15Pt9_
// verified still broken in _MSC_VER_2k19_16Pt0_ (.0 preview 1)
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre2_
// verified FIXED in _MS_VS_2k19_16Pt0Pt0pre3_
#define qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy (_MSC_FULL_VER <= _MS_VS_2k19_16Pt0Pt0pre2_)
#else
#define qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy 0
#endif

#endif

/*
 *  Release-U-32 BUILDS:
 *
 *  LINK : warning C4744: 'static struct std::atomic<void *> Stroika::Foundation::Memory::Private_::BlockAllocationPool_<8>::sHeadLink_' has different type in 'c:\sandbox\stroika\devroot\library\sour
ces\stroika\foundation\debug\assertions.cpp' and 'c:\sandbox\stroika\devroot\samples\ssdpclient\sources\ssdpclient.cpp': 'struct (4 bytes)' and '__declspec(align(4)) struct (4 bytes)
*/
#ifndef qCompilerAndStdLib_inline_static_align_Buggy

#if defined(_MSC_VER)
// first seen broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// verified BROKEN _MSC_VER_2k17_15Pt8_ (OK in debug build, but broken in Release-U-32)
// verified BROKEN _MSC_VER_2k17_15Pt9_ (OK in debug build, but broken in Release-U-32)
// verified still broken in _MSC_VER_2k19_16Pt0_ (.0 preview 1) - not not sure anything more than warnings - no buggy behavior seen?
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre2_
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre3_
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre43_ (aka _MS_VS_2k19_16Pt0Pt0_)
// verified still broken in _MSC_VER_2k19_16Pt1_
// verified still broken in _MSC_VER_2k19_16Pt2_
// verified still broken in _MSC_VER_2k19_16Pt3_
// verified still broken in _MSC_VER_2k19_16Pt4_
// verified still broken in _MSC_VER_2k19_16Pt5_
// verified still broken in _MSC_VER_2k19_16Pt6_
// verified still broken in _MSC_VER_2k19_16Pt7_
// verfified FIXED in _MSC_VER_2k19_16Pt8_
#define qCompilerAndStdLib_inline_static_align_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt7_)
#else
#define qCompilerAndStdLib_inline_static_align_Buggy 0
#endif

#endif

/*
 * SEE https://developercommunity.visualstudio.com/content/problem/484206/const-int-posv-winerror-map-errval-should-probably.html
 *
 *  to test if fixed, run
 *      Test35 (execution::Exceptions) on windows and look in tracelog for 
 *          FIXED - qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy
 * 
 * FAILED: RegressionTestFailure; (error_code{WAIT_TIMEOUT, system_category ()} == errc::timed_out);;C:\Sandbox\Stroika\DevRoot\Tests\36\Test.cpp: 218
 */
#ifndef qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy

#if defined(_MSC_VER)
// Verified still broken in _MSC_VER_2k19_16Pt1_
// Verified still broken in _MSC_VER_2k19_16Pt2_
// Verified still broken in _MSC_VER_2k19_16Pt3_
// Verified still broken in _MSC_VER_2k19_16Pt4_
// Verified still broken in _MSC_VER_2k19_16Pt5_
// Verified still broken in _MSC_VER_2k19_16Pt6_
// Verified still broken in _MSC_VER_2k19_16Pt7_
// Verified still broken in _MSC_VER_2k19_16Pt8_
// Verified still broken in _MSC_VER_2k19_16Pt10_
// Verified still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
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

#if !defined(qCompilerAndStdLib_insert_or_assign_Buggy)
#define qCompilerAndStdLib_insert_or_assign_Buggy (__cplusplus < kStrokia_Foundation_Configuration_cplusplus_17)
#endif

// https://github.com/google/sanitizers/issues/1259
// https://github.com/google/sanitizers/issues/950 (sometimes also shows up as)
// THose were TSAN, but also shows up in valgrind/helgrind (so far only on Ubuntu 20.10) as:
/*
==3198448== Thread #2: Bug in libpthread: write lock granted on mutex/rwlock which is currently wr-held by a different thread
==3198448==    at 0x4840EDF: ??? (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_helgrind-amd64-linux.so)
==3198448==    by 0x2A9B48: __gthread_mutex_lock(pthread_mutex_t*) (gthr-default.h:749)
==3198448==    by 0x2AC76D: std::mutex::lock() (std_mutex.h:100)
==3198448==    by 0x2B050B: std::lock_guard<std::mutex>::lock_guard(std::mutex&) (std_mutex.h:159)
==3198448==    by 0x40D5E1: Stroika::Foundation::Execution::WaitableEvent::WE_::Set() (WaitableEvent.inl:65)
==3198448==    by 0x40D03E: Stroika::Foundation::Execution::WaitableEvent::Set() (WaitableEvent.cpp:96)
==3198448==    by 0x3C9C2B: Stroika::Foundation::Execution::Thread::Ptr::Rep_::ThreadMain_(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*) (Thread.cpp:589)
==3198448==    by 0x3C88E3: Stroika::Foundation::Execution::Thread::Ptr::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*)::{lambda()#1}::operator()() const (Thread.cpp:319)
==3198448==    by 0x3CEEE9: void std::__invoke_impl<void, Stroika::Foundation::Execution::Thread::Ptr::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*)::{lambda()#1}>(std::__invoke_other, Stroika::Foundation::Execution::Thread::Ptr::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*)::{lambda()#1}&&) (invoke.h:60)
==3198448==    by 0x3CEE9E: std::__invoke_result<Stroika::Foundation::Execution::Thread::Ptr::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*)::{lambda()#1}>::type std::__invoke<Stroika::Foundation::Execution::Thread::Ptr::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*)::{lambda()#1}>(std::__invoke_result&&, (Stroika::Foundation::Execution::Thread::Ptr::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*)::{lambda()#1}&&)...) (invoke.h:95)
==3198448==    by 0x3CEE4B: void std::thread::_Invoker<std::tuple<Stroika::Foundation::Execution::Thread::Ptr::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*)::{lambda()#1}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>) (thread:264)
==3198448==    by 0x3CEE1F: std::thread::_Invoker<std::tuple<Stroika::Foundation::Execution::Thread::Ptr::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Ptr::Rep_> const*)::{lambda()#1}> >::operator()() (thread:271)
==3198448== 
*/
// NOTE: I think underlying issue is probably with the tsan/helgrind (probably now common) instrumentation in
// lib std c++. which is why I use same bug define for tsan and valgrind
// https://stroika.atlassian.net/browse/STK-717
#if !defined(qCompiler_SanitizerDoubleLockWithConditionVariables_Buggy)

// Now set in the configure script, because this depends on the OS, in additional to the compiler version
#define qCompiler_SanitizerDoubleLockWithConditionVariables_Buggy 0

#endif

// ASAN crash on windows, with openssl maybe just with default provider
#ifndef qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy

#if defined(_MSC_VER)
// first noticed IN _MSC_VER_2k19_16Pt10_
// still buggy in _MSC_VER_2k22_17Pt0_
#define qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#elif defined(__GNUC__) && !defined(__clang__)
// VERIFIED BROKEN IN GCC 11
#define qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 11)
#else
#define qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy 0
#endif

#endif

/*
ON DEBUG builds on macos only

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
// see on XCOde 11
// reproduced on XCode 12
// reproduced on XCode 13
#define qMacUBSanitizerifreqAlignmentIssue_Buggy ((11 <= __clang_major__) && (__clang_major__ <= 13))
#else
#define qMacUBSanitizerifreqAlignmentIssue_Buggy 0
#endif

#endif

/*
 */
#ifndef qCompilerAndStdLib_locale_pctX_print_time_Buggy

#if defined(__clang__) && defined(__APPLE__)
// First Noted BROKEN on XCode 11.0
// First Noted BROKEN on XCode 12.0
// First Noted BROKEN on XCode 13.0
#define qCompilerAndStdLib_locale_pctX_print_time_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
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
FAILED: RegressionTestFailure; tmp == L"Sun 05 Apr 1903 12:01:41 AM";;C:\Sandbox\Stroika\DevRoot\Tests\48\Test.cpp: 356
   []  (0  seconds)  [48]  Foundation::Time  (../Builds/Release-U-64/Tests/Test48.exe)
 */
#ifndef qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy

#if defined(_MSC_VER)
// first noticed broken in _MSC_VER_2k17_15Pt8_
// verified still broken in _MSC_VER_2k17_15Pt9_
// verified still broken in _MSC_VER_2k19_16Pt0_
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre2_
// verified still broken in _MS_VS_2k19_16Pt0Pt0pre3_
// VERIFIED BROKEN IN _MS_VS_2k19_16Pt0Pt0pre43_ (aka _MS_VS_2k19_16Pt0Pt0_)
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt1_
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt2_
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt3_
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt4_
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt5_
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt6_
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt7_
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt8_
// VERIFIED BROKEN IN _MSC_VER_2k19_16Pt10_
// verified broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy 0
#endif

#endif

/*
* Only broken on vs2k22, RELEASE X86 builds
FAILED: RegressionTestFailure; f1 < f2 or f2 < f1;;C:\Sandbox\Stroika\DevRoot\Tests\42\Test.cpp: 66
   []  (0  seconds)  [42]  Foundation::Execution::Other  (../Builds/Release-x86/Tests/Test42.exe)
*/
#ifndef qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy

#if defined(_MSC_VER)
// first noticed broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k22_17Pt0_ <= _MSC_VER and _MSC_VER <= _MSC_VER_2k22_17Pt0_)
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
 *    if (formatPattern == L"%x %X") {
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
// first noticed broken in _MSC_VER_2k17_15Pt8_
// verified still broken in _MSC_VER_2k17_15Pt9_
// verified still broken in _MSC_VER_2k19_16Pt0_
// verified still broken in _MSC_VER_2k19_16Pt1_
// verified still broken in _MSC_VER_2k19_16Pt2_
// verified still broken in _MSC_VER_2k19_16Pt3_
// verified still broken in _MSC_VER_2k19_16Pt4_
// verified still broken in _MSC_VER_2k19_16Pt5_
// verified still broken in _MSC_VER_2k19_16Pt6_
// verified still broken in _MSC_VER_2k19_16Pt7_
// verified still broken in _MSC_VER_2k19_16Pt8_
// verified still broken in _MSC_VER_2k19_16Pt10_
// verified still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_locale_time_get_reverses_month_day_with_2digit_year_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
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
// VERIFIED BROKEN IN GCC8
// VERIFIED BROKEN IN GCC9.0
// VERIFIED BROKEN IN GCC10.0
// VERIFIED BROKEN IN GCC11
#define qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_GLIBCXX_RELEASE <= 11)
#else
#define qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy 0
#endif

#endif

/**
 WITHOUT the namespace aaa, this works fine.
namespace {
    namespace aaa {
        struct dev {
            static const ObjectVariantMapper kMapper_;
        };
    }
    using namespace aaa;
    const ObjectVariantMapper dev::kMapper_ = []() {
        ObjectVariantMapper mapper;

        using IO::Network::CIDR;
        mapper.Add<CIDR> ([](const ObjectVariantMapper& mapper, const CIDR* obj) -> VariantValue { return obj->ToString (); },
                          [](const ObjectVariantMapper& mapper, const VariantValue& d, CIDR* intoObj) -> void { *intoObj = CIDR{d.As<String> ()}; });
        return mapper;
    }();
}
*/
#ifndef qCompilerAndStdLib_lambda_expand_in_namespace_Buggy

#if defined(_MSC_VER)
// first noticed broken in _MSC_VER_2k17_15Pt8_
// VERIFIED FIXED in _MSC_VER_2k17_15Pt9_
#define qCompilerAndStdLib_lambda_expand_in_namespace_Buggy (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_lambda_expand_in_namespace_Buggy 0
#endif

#endif

/**
stHarness/SimpleClass.cpp ...
37>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Memory\Common.inl(73): error C2512: 'Stroika::Foundation::Traversal::Range<double,Stroika::Foundation::Traversal::RangeTraits::Default<T>>': no appropriate default constructor available
37>        with
37>        [
37>            T=double
37>        ]
37>C:\Sandbox\Stroika\DevRoot\Library\Sources\Stroika\Foundation\Memory\Common.inl(73): note: Constructor for class 'Stroika::Foundation::Traversal::Range<double,Stroika::Foundation::Traversal::RangeTraits::Default<T>>' is declared 'explicit'
37>        with
37>        [
 */
#ifndef qCompilerAndStdLib_default_constructor_initialization_issueWithExplicit_Buggy

#if defined(_MSC_VER)
// first noticed broken in _MSC_VER_2k19_16Pt8_
// APPEARS FIXED IN _MSC_VER_2k19_16Pt10_
#define qCompilerAndStdLib_default_constructor_initialization_issueWithExplicit_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k19_16Pt8_)
#else
#define qCompilerAndStdLib_default_constructor_initialization_issueWithExplicit_Buggy 0
#endif

#endif

/*
 * https://developercommunity.visualstudio.com/content/problem/330322/stdlocale-l2-en-usutf-8-broken-crashes-now.html
 * https://developercommunity.visualstudio.com/t/std::locale-l2-en_USUTF-8;--broke/330322
 *      SUPPOSEDLY FIXED IN Fixed In: Visual Studio 2019 version 16.9 - but without a differnt compiler ID # than 16.8, so
 *      wait to test in 16.10
 *
 *      Take the example code from 
            https://en.cppreference.com/w/cpp/locale/locale/locale

            and create a new visual stdio C++ windows console application from it.

            Run, and you will get
                Debug Assertion Failed!

                Program: C:\WINDOWS\SYSTEM32\MSVCP140D.dll
                File: f:\dd\vctools\crt\crtw32\stdcpp\xmbtowc.c
                Line: 89

                Expression: ploc->_Mbcurmax == 1 || ploc->_Mbcurmax == 2

            This is a very receent regression in your libraries. This worked in 15.8.0, I believe (or just before it).
            But its broken in 15.8.2 and 15.8.3.
            
            NOTE - Debug version asserts out, and release version doesn't. BUT - for at least some cases (stroika regtests)
            the release version builds a locale that doesnt work (with my test cases - unicode utf8 conversion).
           

    FAILED: RegressionTestFailure; false;;C:\Sandbox\Stroika\DevRoot\Tests\02\Test.cpp: 1194
 */
#ifndef qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k17_15Pt8_
// VERIFIED broken IN _MSC_VER_2k17_15Pt9_  (NOTE - this no longer gives the above assertion error but instead just fails in the UTF8 code conversion)
// VERIFIED still broken in _MSC_VER_2k19_16Pt0_
// VERIFIED still broken in _MS_VS_2k19_16Pt0Pt0pre2_
// VERIFIED still broken in _MS_VS_2k19_16Pt0Pt0pre3_
// VERIFIED still broken in _MS_VS_2k19_16Pt0Pt0pre43_ (aka _MS_VS_2k19_16Pt0Pt0_)
// VERIFIED still broken in _MSC_VER_2k19_16Pt1_
// VERIFIED FIXED _MSC_VER_2k19_16Pt2_ (FIXED BUT INSTEAD WE NOW HAVE qCompilerAndStdLib_locale_utf8_string_convert_Buggy)
#define qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy (_MSC_VER <= _MSC_VER_2k19_16Pt1_)
#else
#define qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy 0
#endif

#endif

// Must run regtest 2, and see if pass/fail (or look into why not handling these unicode strings)
// FAILED: RegressionTestFailure; not initializedLocale;;C:\Sandbox\Stroika\DevRoot\Tests\02\Test.cpp: 1203
#ifndef qCompilerAndStdLib_locale_utf8_string_convert_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k19_16Pt2_ (before was qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy or qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy)
// verified still broken in _MSC_VER_2k19_16Pt3_
// verified still broken in _MSC_VER_2k19_16Pt4_
// verified still broken in _MSC_VER_2k19_16Pt5_
// verified still broken in _MSC_VER_2k19_16Pt6_
// verified still broken in _MSC_VER_2k19_16Pt7_
// verified still broken in _MSC_VER_2k19_16Pt8_
// verified still broken in _MSC_VER_2k19_16Pt10_
// verified still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_locale_utf8_string_convert_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
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
// first noticed broken in _MSC_VER_2k19_16Pt10_
// still broken in _MSC_VER_2k22_17Pt0_
#define qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k22_17Pt0_)
#else
#define qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy 0
#endif

#endif

/*
 * Assertions.cpp:178:5: error: use of undeclared identifier 'quick_exit'
 */
#ifndef qCompilerAndStdLib_quick_exit_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Still broken XCode 10 - beta
// VERIFIED STILL BROKEN on XCode 11.0
// VERIFIED STILL BROKEN on XCode 12.0
// VERIFIED STILL BROKEN on XCode 13.0
#define qCompilerAndStdLib_quick_exit_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 13))
#else
#define qCompilerAndStdLib_quick_exit_Buggy 0
#endif

#endif

/*
 * OpenSSL calls under valgrind memcheck mysteriously cause failure/crash
 *
 *      On raspberry pi:
 *          lewis@raspberrypi:/tmp $ valgrind --gen-suppressions=yes -q --track-origins=yes --tool=memcheck --leak-check=full --suppressions=Valgrind-MemCheck-Common.supp  /tmp/Test43
 *          FAILED: SIGNAL= SIGILL
 *
 *          NOTE - we get the same failure with helgrind!
 *
 */
#ifndef qCompilerAndStdLib_arm_openssl_valgrind_Buggy

#if defined(__GNUC__) && defined(__arm__)
// tested still generates warning with gcc8 (actually crash on valgrind and raspberrypi - stretch - maybe fixed in buster? - maybe got worse cuz I upgraded libc on that machine but not valgrind to buster?)
// UNTESTED with GCC-9, because my only ARM machine right now doesn't have the right version of glibc -- LGP 2019-04-20
#define qCompilerAndStdLib_arm_openssl_valgrind_Buggy (__GNUC__ <= 8)
#else
#define qCompilerAndStdLib_arm_openssl_valgrind_Buggy 0
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
 *  This COULD have todo with using LTO? Or any other form of optimizaiton.
 *
 *      valgrind --track-origins=yes --tool=memcheck --leak-check=full --suppressions=Valgrind-MemCheck-Common.supp  ../Builds/g++-valgrind-release-SSLPurify-NoBlockAlloc/Tests/Test41
 
            ==25626== Conditional jump or move depends on uninitialised value(s)
            ==25626==    at 0x18791F: (anonymous namespace)::Test1_URL_Parsing_::Private_::BasicTests_AsOf21d22_() [clone .lto_priv.852] (in /Sandbox/Stroika-Dev/Builds/g++-valgrind-release-SSLPurify-NoBlockAlloc/Tests/Test41)
            ==25626==    by 0x1358AA: main (in /Sandbox/Stroika-Dev/Builds/g++-valgrind-release-SSLPurify-NoBlockAlloc/Tests/Test41)
            ==25626==  Uninitialised value was created by a stack allocation
            ==25626==    at 0x15181F: Stroika::Foundation::IO::Network::URL::Parse(Stroika::Foundation::Characters::String const&, Stroika::Foundation::IO::Network::URL::ParseOptions) (in /Sandbox/Stroika-Dev/Builds/g++-valgrind-release-SSLPurify-NoBlockAlloc/Tests/Test41)

 REALLY bad message. Triggered by:
                     VerifyTestResult (URL::Parse (kTestURL_, po) == URL (L"http", L"www.x.com", L"foo", L"bar=3"));

                    because it calls operator==, which calls
                    UniformResourceIdentification::operator!= (const Authority& lhs, const Authority& rhs)

                    and compares with missing port. NOT SURE if missing other stuff also causes trouble but it appears not.

            */
#ifndef qCompilerAndStdLib_valgrind_optional_compare_equals_Buggy

#if defined(__GNUC__)
// APPEARS fixed in GCC 9.0
#define qCompilerAndStdLib_valgrind_optional_compare_equals_Buggy (__GNUC__ <= 8)
#else
#define qCompilerAndStdLib_valgrind_optional_compare_equals_Buggy 0
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
#if _MSVC_LANG < kStrokia_Foundation_Configuration_cplusplus_17
#pragma message("Stroika v2.1 requires at least C++ ISO/IEC 14882:2017(E) supported by the compiler (informally known as C++ 17)")
#endif
#else
#if __cplusplus < kStrokia_Foundation_Configuration_cplusplus_17
#pragma message("Stroika v2.1 requires at least C++ ISO/IEC 14882:2017(E) supported by the compiler (informally known as C++ 17)")
#endif
#endif

#if qSilenceAnnoyingCompilerWarnings && defined(__GNUC__) && !defined(__clang__)
// Note - I tried tricks with token pasting, but only seems to work if I do all token pasting
// and that fails with 'astyle' which breaks up a-b tokens. Need quotes to work with astyle
// and no way I can find to concatenate strings that works with _Pragma
//  --LGP 2014-01-05
#define DISABLE_COMPILER_GCC_WARNING_START(WARNING_TO_DISABLE) \
    _Pragma ("GCC diagnostic push")                            \
        _Pragma (WARNING_TO_DISABLE)
#define DISABLE_COMPILER_GCC_WARNING_END(WARNING_TO_DISABLE) \
    _Pragma ("GCC diagnostic pop")
#else
#define DISABLE_COMPILER_GCC_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_GCC_WARNING_END(WARNING_TO_DISABLE)
#endif

#if qSilenceAnnoyingCompilerWarnings && defined(_MSC_VER)
#define DISABLE_COMPILER_MSC_WARNING_START(WARNING_TO_DISABLE) \
    __pragma (warning (push))                                  \
        __pragma (warning (disable                             \
                           : WARNING_TO_DISABLE))
#define DISABLE_COMPILER_MSC_WARNING_END(WARNING_TO_DISABLE) \
    __pragma (warning (pop))
#else
#define DISABLE_COMPILER_MSC_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_MSC_WARNING_END(WARNING_TO_DISABLE)
#endif

#if qSilenceAnnoyingCompilerWarnings && defined(__clang__)
// Note - I tried tricks with token pasting, but only seems to work if I do all token pasting
// and that fails with 'astyle' which breaks up a-b tokens. Need quotes to work with astyle
// and no way I can find to concatenate strings that works with _Pragma
//  --LGP 2014-01-05
#define DISABLE_COMPILER_CLANG_WARNING_START(WARNING_TO_DISABLE) \
    _Pragma ("clang diagnostic push")                            \
        _Pragma (WARNING_TO_DISABLE)
#define DISABLE_COMPILER_CLANG_WARNING_END(WARNING_TO_DISABLE) \
    _Pragma ("clang diagnostic pop")
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

#if qCompiler_MisinterpretAttributeOnCompoundStatementAsWarningIgnored_Buggy
#pragma GCC diagnostic ignored "-Wattributes"
#endif

// doesn't seem any portable way todo this, and not defined in C++ language
// Note - this doesn't appear in http://en.cppreference.com/w/cpp/language/attributes - as of 2016-06-22
#if defined(__clang__) || defined(__GNUC__)
#define dont_inline __attribute__ ((noinline))
#else
#define dont_inline __declspec(noinline)
#endif

/*
 *   Sometimes its handy to mark a line of code as a no-op - so its arguments are not executed (as with
 * trace macros).
 */
#if !defined(_NoOp_)
#if defined(_MSC_VER)
#define _NoOp_ __noop
#else
#define _NoOp_(...)
#endif
#endif

//#ifndef STRINGIFY
//#define STRINGIFY(a) #a
//#endif

#ifndef _Stroika_Foundation_Configuration_Private_DO_PRAGMA_
#define _Stroika_Foundation_Configuration_Private_DO_PRAGMA_(x) _Pragma (#x)
#endif

/*
 *  Wrap this macro around entire declaration, as in:
 *       _DeprecatedFile_ ("DEPRECATED in v2.0a32 - use IO::FileSystem::DirectoryIterator");
 */
#if !defined(_DeprecatedFile_)
#define _DeprecatedFile_(MESSAGE) \
    _Stroika_Foundation_Configuration_Private_DO_PRAGMA_ (message ("WARNING: Deprecated File: " MESSAGE))
#endif

/*
 * https://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros
 * https://en.cppreference.com/w/cpp/language/attributes/no_unique_address
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

#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
namespace Stroika::Foundation::Configuration {
    template <class InputIt, class Size, class ForwardIt>
    ForwardIt uninitialized_copy_n_MSFT_BWA (InputIt first, Size count, ForwardIt d_first)
    {
        // @see https://en.cppreference.com/w/cpp/memory/uninitialized_copy_n
        typedef typename std::iterator_traits<ForwardIt>::value_type Value;
        ForwardIt                                                    current = d_first;
        try {
            for (; count > 0; ++first, (void)++current, --count) {
                ::new (static_cast<void*> (std::addressof (*current))) Value (*first);
            }
        }
        catch (...) {
            for (; d_first != current; ++d_first) {
                d_first->~Value ();
            }
            throw;
        }
        return current;
    }
    template <class InputIt, class ForwardIt>
    ForwardIt uninitialized_copy_MSFT_BWA (InputIt first, InputIt last, ForwardIt d_first)
    {
        // @see https://en.cppreference.com/w/cpp/memory/uninitialized_copy
        typedef typename std::iterator_traits<ForwardIt>::value_type Value;
        ForwardIt                                                    current = d_first;
        try {
            for (; first != last; ++first, (void)++current) {
                ::new (static_cast<void*> (std::addressof (*current))) Value (*first);
            }
            return current;
        }
        catch (...) {
            for (; d_first != current; ++d_first) {
                d_first->~Value ();
            }
            throw;
        }
    }
} // namespace Stroika::Foundation::Configuration
#endif

#endif /*defined(__cplusplus)*/

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

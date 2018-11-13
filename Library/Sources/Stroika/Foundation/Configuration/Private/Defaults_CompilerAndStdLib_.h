/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
 *  WAG
 */
#define kStrokia_Foundation_Configuration_cplusplus_20 202000

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
#if (__clang_major__ < 10) || (__clang_major__ == 10 && (__clang_minor__ < 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika v2.1 (older clang versions supported by v2.0) does not support versions prior to APPLE clang++ 10 (XCode 10)"
#endif
#if (__clang_major__ > 10) || (__clang_major__ == 10 && (__clang_minor__ > 1))
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
#if (__clang_major__ > 7) || (__clang_major__ == 7 && (__clang_minor__ > 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Info: Stroika untested with this version of clang++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#endif

#elif defined(__GNUC__)

#if __GNUC__ < 7
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika v2.1 does not support versions prior to GCC 7 (v2.0 supports g++5 and g++6)"
#endif
#if __GNUC__ > 8 || (__GNUC__ == 8 && (__GNUC_MINOR__ > 2))
#define _STROIKA_CONFIGURATION_WARNING_ "Info: Stroika untested with this version of GCC - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif

#elif defined(_MSC_VER)

#define _MS_VS_2k17_VER_ 1910

#define _MS_VS_2k17_15Pt1_ 191025019
#define _MS_VS_2k17_15Pt3Pt2_ 191125507
#define _MS_VS_2k17_15Pt5Pt0_ 191225830
#define _MS_VS_2k17_15Pt5Pt2_ 191225831
#define _MS_VS_2k17_15Pt5Pt3_ 191225834
#define _MS_VS_2k17_15Pt5Pt5_ 191225835
#define _MS_VS_2k17_15Pt6Pt0_ 191326198
#define _MS_VS_2k17_15Pt7Pt1_ 191426428
#define _MS_VS_2k17_15Pt7Pt2_ 191426429
#define _MS_VS_2k17_15Pt7Pt3_ 191426430
#define _MS_VS_2k17_15Pt7Pt4_ 191426431
#define _MS_VS_2k17_15Pt7Pt5_ 191426433
// _MSC_VER=1914
#define _MS_VS_2k17_15Pt7Pt6_ 191426433
#define _MSC_VER_2k17_15Pt7_ 1914

// _MSC_VER=1915
#define _MSC_VER_2k17_15Pt8_ 1915
#define _MS_VS_2k17_15Pt8Pt0_ 191526726
#define _MS_VS_2k17_15Pt8Pt4_ 191526729
#define _MS_VS_2k17_15Pt8Pt5_ 191526730
#define _MS_VS_2k17_15Pt8Pt8_ 191526732

#if _MSC_VER < 1910
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2017"
#elif _MSC_VER <= _MSC_VER_2k17_15Pt7_
// check which pointer-version of MSVC2k17 (15.7.x)
#if _MSC_FULL_VER > _MS_VS_2k17_15Pt7Pt6_
#define _STROIKA_CONFIGURATION_WARNING_ "Info: This version ( #_MSC_FULL_VER ) - 15.7.x - of Stroika is untested with this Update of of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#elif _MSC_VER <= _MSC_VER_2k17_15Pt8_
// check which pointer-version of MSVC2k17 (15.8.x)
#if _MSC_FULL_VER > _MS_VS_2k17_15Pt8Pt8_
// @todo figure out how to add arg to message
#define _STROIKA_CONFIGURATION_WARNING_ "Info: This version (#_MSC_FULL_VER ) - 15.8.x - of Stroika is untested with this Update of of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#else
#define _STROIKA_CONFIGURATION_WARNING_ "Info: This version (> 15.8) of Stroika is untested with this release of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif

#else

#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika does recognize the compiler being used. It may work, but you may need to update some of the other defines for what features are supported by your compiler."

#endif

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
#define qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy 0
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
 *  in xcode 10 (beta) the headers for filesystem are there but I cannot find libraries, so disable for now... --LGP 2018-06-15
 */
#ifndef qCompilerAndStdLib_stdfilesystemAppearsPresentButDoesntWork_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_stdfilesystemAppearsPresentButDoesntWork_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
#else
#define qCompilerAndStdLib_stdfilesystemAppearsPresentButDoesntWork_Buggy 0
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
#define qCompilerAndStdLib_std_get_time_pctx_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
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
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy 0
#endif

#endif

/*
 *  This ONLY affects arm-linux-gnueabihf-g++-7
 *
 ... (7z/CharacterDelimitedLines/INI/JSON/XML/Zip)  (scp ../Builds/raspberrypi-gcc-7-sanitize/Test30
 ...  (scp ../Builds/raspberrypi-gcc-7-sanitize/Test40
 *
 * MUST MANUALLY FIX make default-regressions like to test if this is fixed (MAYBE WITH GCC8)
 *
 =================================================================
 ==13645==ERROR: AddressSanitizer: stack-use-after-scope on address 0x7e94da60 at pc 0x55d05d8d bp 0x7e94c5dc sp 0x7e94c5d4
 WRITE of size 4 at 0x7e94da60 thread T0
 #0 0x55d05d8b in Stroika::Foundation::Memory::SmallStackBuffer<unsigned char, 4096u>::SmallStackBuffer() ../../../DataExchange/../Execution/../Characters/../Containers/Factory/../Concrete/../Private/PatchingDataStructures/../../../Memory/SmallStackBuffer.inl:31
 #1 0x55d3e9fb in Stroika::Foundation::Memory::SmallStackBuffer<unsigned char, 4096u>::SmallStackBuffer(unsigned int) ../../../DataExchange/../Execution/../Characters/../Containers/Factory/../Concrete/../Private/PatchingDataStructures/../../../Memory/SmallStackBuffer.inl:41
 #2 0x55d30787 in Stroika::Foundation::Streams::TextReader::FromBinaryStreamBaseRep_::Read(Stroika::Foundation::Characters::Character*, Stroika::Foundation::Characters::Character*) /home/lewis/Sandbox/Stroika-Regression-Tests/Library/Sources/Stroika/Foundation/Streams/TextReader.cpp:79
 #3 0x55d35877 in Stroika::Foundation::Streams::TextReader::CachingSeekableBinaryStreamRep_::Read(Stroika::Foundation::Characters::Character*, Stroika::Foundation::Characters::Character*) (/tmp/Test30+0x11b3877)
 #4 0x55b2e5f9 in Stroika::Foundation::Streams::InputStream<Stroika::Foundation::Characters::Character>::Ptr::Read() const ../../../Streams/InputStream.inl:134
 #5 0x55b2c291 in Reader_value_ /home/lewis/Sandbox/Stroika-Regression-Tests/Library/Sources/Stroika/Foundation/DataExchange/Variant/JSON/Reader.cpp:337
 #6 0x55b2de41 in Stroika::Foundation::DataExchange::Variant::JSON::Reader::Rep_::Read(Stroika::Foundation::Streams::InputStream<Stroika::Foundation::Characters::Character>::Ptr const&) /home/lewis/Sandbox/Stroika-Regression-Tests/Library/Sources/Stroika/Foundation/DataExchange/Variant/JSON/Reader.cpp:407
 #7 0x55a54931 in Stroika::Foundation::DataExchange::Variant::Reader::Read(Stroika::Foundation::Streams::InputStream<Stroika::Foundation::Characters::Character>::Ptr const&) /home/lewis/Sandbox/Stroika-Regression-Tests/Library/Sources/Stroika/Foundation/DataExchange/Variant/Reader.inl:46
 #8 0x55a540a7 in Stroika::Foundation::DataExchange::Variant::Reader::Read(std::istream&) /home/lewis/Sandbox/Stroika-Regression-Tests/Library/Sources/Stroika/Foundation/DataExchange/Variant/Reader.cpp:34
 #9 0x5576427d in VerifyThisStringFailsToParse_ /home/lewis/Sandbox/Stroika-Regression-Tests/Tests/30/Test.cpp:315
 #10 0x557649cd in DoIt /home/lewis/Sandbox/Stroika-Regression-Tests/Tests/30/Test.cpp:330
 #11 0x5576cb09 in DoAll_ /home/lewis/Sandbox/Stroika-Regression-Tests/Tests/30/Test.cpp:598
 #12 0x5576efb5 in DoRegressionTests_ /home/lewis/Sandbox/Stroika-Regression-Tests/Tests/30/Test.cpp:803
 #13 0x558b7ee7 in Stroika::TestHarness::PrintPassOrFail(void (*)()) /home/lewis/Sandbox/Stroika-Regression-Tests/Tests/30/../TestHarness/TestHarness.cpp:81
 #14 0x5576f013 in main /home/lewis/Sandbox/Stroika-Regression-Tests/Tests/30/Test.cpp:820
 #15 0x76298677 in __libc_start_main (/lib/arm-linux-gnueabihf/libc.so.6+0x16677)

 Address 0x7e94da60 is located in stack of thread T0 at offset 4800 in frame
 #0 0x55d30247 in Stroika::Foundation::Streams::TextReader::FromBinaryStreamBaseRep_::Read(Stroika::Foundation::Characters::Character*, Stroika::Foundation::Characters::Character*) /home/lewis/Sandbox/Stroika-Regression-Tests/Library/Sources/Stroika/Foundation/Streams/TextReader.cpp:60

 This frame has 10 object(s):
 [32, 34) '<unknown>'
 [96, 100) 'outCursor'
 [160, 164) 'critSec'
 [224, 228) 'cursorB'
 [288, 292) '<unknown>'
 [352, 412) '<unknown>'
 [448, 456) 'mbState_'
 [512, 600) '<unknown>'
 [640, 4760) 'outBuf'
 [4800, 8920) 'inBuf' <== Memory access at offset 4800 is inside this variable
 HINT: this may be a false positive if your program uses some custom stack unwind mechanism or swapcontext
 (longjmp and C++ exceptions *are* supported)
 SUMMARY: AddressSanitizer: stack-use-after-scope ../../../DataExchange/../Execution/../Characters/../Containers/Factory/../Concrete/../Private/PatchingDataStructures/../../../Memory/SmallStackBuffer.inl:31 in Stroika::Foundation::Memory::SmallStackBuffer<unsigned char, 4096u>::SmallStackBuffer()
 Shadow bytes around the buggy address:
 0x2fd29af0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd29b00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd29b10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd29b20: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd29b30: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 =>0x2fd29b40: 00 00 00 00 00 00 00 f2 f2 f2 f2 f2[f8]f8 f8 f8
 0x2fd29b50: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd29b60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd29b70: 00 00 00 00 00 00 00 00 f8 f8 f8 f8 f8 f8 f8 f8
 0x2fd29b80: f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 00
 0x2fd29b90: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
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
 ==13645==ABORTING







 OR (had diff workaround) -
 LD_PRELOAD=/usr/lib/arm-linux-gnueabihf/libasan.so.4 ./Test44
 =================================================================
 ==16307==ERROR: AddressSanitizer: stack-use-after-scope on address 0x7ea13180 at pc 0x553b760d bp 0x7ea1315c sp 0x7ea13154
 READ of size 4 at 0x7ea13180 thread T0
 #0 0x553b760b in std::initializer_list<Stroika::Foundation::Characters::String>::begin() const /usr/arm-linux-gnueabihf/include/c++/7/initializer_list:75
 #1 0x553b1a0f in Stroika::Foundation::Characters::String const* std::begin<Stroika::Foundation::Characters::String>(std::initializer_list<Stroika::Foundation::Characters::String>) /usr/arm-linux-gnueabihf/include/c++/7/initializer_list:90
 #2 0x553b771b in void Stroika::Foundation::Containers::Set<Stroika::Foundation::Characters::String, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Characters::String, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Characters::String, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Characters::String> > > >::AddAll<std::initializer_list<Stroika::Foundation::Characters::String>, void>(std::initializer_list<Stroika::Foundation::Characters::String> const&) ../../Characters/../Containers/Set.inl:138
 #3 0x553b1aab in Stroika::Foundation::Containers::Set<Stroika::Foundation::Characters::String, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Characters::String, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Characters::String, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Characters::String> > > >::Set(std::initializer_list<Stroika::Foundation::Characters::String> const&) ../../Characters/../Containers/Set.inl:41
 #4 0x556b8b17 in ReadMountInfo_MTabLikeFile_ /home/lewis/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/IO/FileSystem/MountedFilesystem.cpp:135
 #5 0x556b94b7 in ReadMountInfo_FromProcFSMounts_ /home/lewis/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/IO/FileSystem/MountedFilesystem.cpp:153
 #6 0x556baa3f in Stroika::Foundation::IO::FileSystem::GetMountedFilesystems() /home/lewis/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/IO/FileSystem/MountedFilesystem.cpp:305
 #7 0x553a6ccb in DoTest /home/lewis/Sandbox/Stroika-Dev/Tests/44/Test.cpp:140
 #8 0x553a76a5 in DoRegressionTests_ /home/lewis/Sandbox/Stroika-Dev/Tests/44/Test.cpp:165
 #9 0x553ed22f in Stroika::TestHarness::PrintPassOrFail(void (*)()) /home/lewis/Sandbox/Stroika-Dev/Tests/44/../TestHarness/TestHarness.cpp:81
 #10 0x553a76eb in main /home/lewis/Sandbox/Stroika-Dev/Tests/44/Test.cpp:173
 #11 0x762b6677 in __libc_start_main (/lib/arm-linux-gnueabihf/libc.so.6+0x16677)

 Address 0x7ea13180 is located in stack of thread T0
 SUMMARY: AddressSanitizer: stack-use-after-scope /usr/arm-linux-gnueabihf/include/c++/7/initializer_list:75 in std::initializer_list<Stroika::Foundation::Characters::String>::begin() const
 Shadow bytes around the buggy address:
 0x2fd425e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd425f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd42600: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd42610: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd42620: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 =>0x2fd42630:[f8]f8 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd42640: 00 00 00 00 f8 00 00 00 00 00 00 00 00 00 00 00
 0x2fd42650: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd42660: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd42670: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 0x2fd42680: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
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


 but disabling asan also workaround for now and seems like likely asan bug since only on gcc and arm and when I investigate code looks fine.
 */
#ifndef qCompiler_Sanitizer_stack_use_after_scope_on_arm_Buggy

#if defined(__GNUC__) && !defined(__clang__)
#if defined(__arm__)
#define qCompiler_Sanitizer_stack_use_after_scope_on_arm_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 7))
#endif
#else
#define qCompiler_Sanitizer_stack_use_after_scope_on_arm_Buggy 0
#endif

#endif

/*
@DESCRIPTION:   

@see https://stroika.atlassian.net/browse/STK-635

http://open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3797.pdf
     A static data member of literal type can be declared in the
    class definition with the constexpr specifier; if so, its declaration shall specify a brace-or-equal-initializer
    in which every initializer-clause that is an assignment-expression is a constant expression. [ Note: In both
    these cases, the member may appear in constant expressions. — end note ] The member shall still be defined
    in a namespace scope if it is odr-used (3.2) in the program and the namespace scope definition shall not
    contain an initializer.


http://stackoverflow.com/questions/24342455/nested-static-constexpr-of-incomplete-type-valid-c-or-not
that doesn't work (duplicate definitions - works in a single file but not across multiple files)


NOTE: as of Stroika v2.1d11, we eliminated (except for deprecated) use of this so we should be able to lose this define when we
lose those deprecated interfaces.
*/
#ifndef qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 7))
#elif defined(__GNUC__)
// APPEARS still broken with gcc 6.2
// APPEARS still broken with gcc 6.3
// APPEARS still broken with gcc 7.1
// APPEARS still broken with gcc 7.2
// APPEARS still broken with gcc 8.1
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 8)
#elif defined(_MSC_VER)
// STILL WARNINGS in _MS_VS_2k17_15Pt1_
// now link error in _MS_VS_2k17_15Pt3Pt2_
// still broken (warnigns about second def ignored) - _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
// now compile error in _MS_VS_2k17_15Pt6Pt0_ --- now error C2027: use of undefined type 'Stroika::Foundation::Time::Date' (compiling source file ..\..\Sources\Stroika\Foundation\Configuration\SystemConfiguration.cpp)
// still broken in 15.7.1
// assume broken in _MS_VS_2k17_15Pt7Pt2_
// assume broken in _MS_VS_2k17_15Pt7Pt3_
// assume broken in _MS_VS_2k17_15Pt7Pt4_
// assume brokenin _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// verified broken in _MSC_VER_2k17_15Pt8_
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy 0
#endif

#endif

/*
 *
    Compiling {StroikaRoot}Library/Sources/Stroika/Frameworks/Modbus/Server.cpp ...
    Server.cpp:369:45: error: call to unavailable member function 'value': introduced in macOS 10.14
        options.fLogger.value ()->Log (Logger::Priority::eWarning, L"ModbusTCP unrecognized f...
           ~~~~~~~~~~~~~~~~^~~~~
    /Applications/Xcode-beta.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/optional:933:33: note:
      candidate function has been explicitly made unavailable
    constexpr value_type const& value() const&
*/
#ifndef qCompilerAndStdLib_optional_value_const_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_optional_value_const_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
#else
#define qCompilerAndStdLib_optional_value_const_Buggy 0
#endif

#endif

/**
 * get warnings like 
 *      warning: ‘no_sanitize’ attribute directive ignored [-Wattributes
 */
#ifndef qCompiler_noSanitizeAttribute_Buggy
#if defined(__clang__) && defined(__APPLE__)
// appears to work with XCode 10 on macos, if you use clang::no_sanitize for attribute name
#define qCompiler_noSanitizeAttribute_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 9))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompiler_noSanitizeAttribute_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 6))
#elif defined(__GNUC__)
// tested still generates warning with gcc8
//todo retest with gnu::no_sanitize
#define qCompiler_noSanitizeAttribute_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 7)
#else
#define qCompiler_noSanitizeAttribute_Buggy 1
#endif
#endif

#ifndef qCompiler_noSanitizeAttributeForLamdas_Buggy
#if defined(__clang__) && defined(__APPLE__)
// appears to work with XCode 10 on macos, if you use clang::no_sanitize for attribute name
#define qCompiler_noSanitizeAttributeForLamdas_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompiler_noSanitizeAttributeForLamdas_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 7))
#elif defined(__GNUC__)
// tested still generates warning with gcc8
#define qCompiler_noSanitizeAttributeForLamdas_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 8)
#else
#define qCompiler_noSanitizeAttributeForLamdas_Buggy 1
#endif
#endif

/**
 *  \note this also happens to clang++ 5 or lower (fixed in 6) on Linux (LLVM). But we aren't going to bother supporting
 *        those versions for Stroika 2.1. (ONLY HAS EFFECT with -std=c++17)
 *
 *  SAMPLE ERROR OUTPUT:
 *       Undefined symbols for architecture x86_64:
 *        "Stroika::Foundation::Configuration::DefaultNames<Stroika::Foundation::Execution::SignalHandler::Type>::k", referenced from:
 *            Stroika::Foundation::Characters::String Stroika::Foundation::Characters::Private_::ToString_<Stroika::Foundation::Execution::SignalHandler::Type>(Stroika::Foundation::Execution::SignalHandler::Type const&, std::__1::enable_if<is_enum<Stroika::Foundation::Execution::SignalHandler::Type>::value, void>::type*) in Stroika-Foundation.a(SignalHandlers.o)
 *        "Stroika::Foundation::Configuration::DefaultNames<Stroika::Foundation::Execution::Thread::Status>::k", referenced from:
 *            Stroika::Foundation::Characters::String Stroika::Foundation::Characters::Private_::ToString_<Stroika::Foundation::Execution::Thread::Status>(Stroika::Foundation::Execution::Thread::Status const&, std::__1::enable_if<is_enum<Stroika::Foundation::Execution::Thread::Status>::value, void>::type*) in Stroika-Foundation.a(Thread.o)
 *      ld: symbol(s) not found for architecture x86_64
 *      clang: error: linker command failed with exit code 1 (use -v to see invocation)
 */
#ifndef qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
#if defined(__clang__) && defined(__APPLE__)
#define qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 9))
#else
#define qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy 0
#endif
#endif

/**
 * 1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\execution\sharedstaticdata.h(113): fatal error C1001: An internal error has occurred in the compiler.
1>(compiler file 'f:\dd\vctools\compiler\cxxfe\sl\p1\c\symbols.c', line 24193)
1> To work around this problem, try simplifying or changing the program near the locations listed above.
1>Please choose the Technical Support command on the Visual C++

 */
#ifndef qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
#if defined(_MSC_VER)
// assume broken in _MS_VS_2k17_15Pt7Pt3_
// appears fixed in _MS_VS_2k17_15Pt7Pt4_
#define qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt7Pt3_)
#else
#define qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy 0
#endif
#endif

/**
Compiling  $StroikaRoot/Library/Sources/Stroika/Foundation/Configuration/VersionDefs.cpp...
Locale.cpp: In function ‘Stroika::Foundation::Characters::String Stroika::Foundation::Configuration::FindLocaleName(const Stroika::Foundation::Characters::String&, const Stroika::Foundation::Characters::String&)’:
Locale.cpp:102:5: internal compiler error: in process_init_constructor_array, at cp/typeck2.c:1308
};
^
0x6ccd5c process_init_constructor_array
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/typeck2.c:1307
0x6ccd5c process_init_constructor
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/typeck2.c:1623
0x6ccd5c digest_init_r
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/typeck2.c:1135
0x759ef6 finish_compound_literal(tree_node*, tree_node*, int)
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/semantics.c:2717
0x66c481 convert_like_real
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/call.c:6803
0x67355d build_over_call
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/call.c:7845
0x66c66c convert_like_real
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/call.c:6716
0x66c8f5 convert_like_real
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/call.c:6844
0x66d4a6 perform_implicit_conversion_flags(tree_node*, tree_node*, int, int)
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/call.c:10119
0x66bc5e build_special_member_call(tree_node*, tree_node*, vec<tree_node*, va_gc, vl_embed>**, tree_node*, int, int)
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/call.c:8371
0x746d59 expand_default_init
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/init.c:1850
0x746d59 expand_aggr_init_1
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/init.c:1965
0x74767c build_aggr_init(tree_node*, tree_node*, int, int)
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/init.c:1704
0x67eabd build_aggr_init_full_exprs
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/decl.c:6170
0x67eabd check_initializer
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/decl.c:6318
0x68e967 cp_finish_decl(tree_node*, tree_node*, bool, tree_node*, int)
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/decl.c:7037
0x71eded cp_parser_init_declarator
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/parser.c:19388
0x71f5e6 cp_parser_simple_declaration
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/parser.c:12777
0x72028a cp_parser_block_declaration
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/parser.c:12603
0x720c09 cp_parser_declaration_statement
/home/lewis/BUILD-GCC-7.1.0/objdir/../gcc-7.1.0/gcc/cp/parser.c:12212
Please submit a full bug report,
with preprocessed source if appropriate.
Please include the complete backtrace with any bug report.
See <https://gcc.gnu.org/bugs/> for instructions.

*/
#ifndef qCompilerAndStdLib_process_init_constructor_array_Buggy

#if !defined(__clang__) && defined(__GNUC__)
#define qCompilerAndStdLib_process_init_constructor_array_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ == 7)
#else
#define qCompilerAndStdLib_process_init_constructor_array_Buggy 0
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
#define qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt7_)
#else
#define qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy 0
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
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_MaybeUnusedIgnoredInLambdas_Buggy

#if defined(_MSC_VER)
// first noted in _MS_VS_2k17_15Pt7Pt3_
// still broken in _MS_VS_2k17_15Pt7Pt4_
// assume broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// still broken in _MSC_VER_2k17_15Pt8_
#define qCompilerAndStdLib_MaybeUnusedIgnoredInLambdas_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_MaybeUnusedIgnoredInLambdas_Buggy 0
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
#define qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER_2k17_15Pt8_ <= _MSC_VER && _MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy 0
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




./../Configuration/Endian.inl:34:37: error: constexpr function never produces a constant expression [-Winvalid-constexpr]
            inline constexpr Endian GetEndianness ()
                                    ^
./../Configuration/Endian.inl:37:25: note: read of member 'cdat' of union with active member 'sdat' is not allowed in a constant expression



error C2975: '_Test': invalid template argument for 'std::conditional', expected compile-time constant expression (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\SSL\SSLSocket.cpp)
1>C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.11.25503\include\xtr1common(66): note: see declaration of '_Test' (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\SSL\SSLSocket.cpp)

*/
#ifndef qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 7))
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
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
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
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 7))
#else
#define qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy 0
#endif

#endif

/*
/*
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
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy (CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_LIBCPP_VERSION <= 7000))
#else
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy 0
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

/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): error C2244: 'Stroika::Foundation::Traversal::DelegatedIterator<T,EXTRA_DATA>::Rep::Clone': unable to match function definition to an existing declaration
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(29): note: see declaration of 'Stroika::Foundation::Traversal::DelegatedIterator<T,EXTRA_DATA>::Rep::Clone'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): note: definition
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): note: 'Iterator<T,std::iterator<std::forward_iterator_tag,T,ptrdiff_t,_Ty*,_Ty&>>::SharedIRepPtr Stroika::Foundation::Traversal::DelegatedIterator<T,EXTRA_DATA>::Rep::Clone(void) const'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): note: existing declarations
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\delegatediterator.inl(32): note: 'std::shared_ptr<SHARED_T> Stroika::Foundation::Traversal::DelegatedIterator<T,EXTRA_DATA>::Rep::Clone(void) const'
*/
#ifndef qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy

#if defined(_MSC_VER)
// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
// still broken in _MS_VS_2k17_15Pt6Pt0_
// APPEARS fixed with _MS_VS_2k17_15Pt7Pt1_
#define qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt6Pt0_)
#else
#define qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy 0
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
#define qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MS_VS_2k17_15Pt7Pt1_ <= _MSC_FULL_VER && _MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy 0
#endif

#endif

/*
 *
 *  LINK : warning C4744: 'static struct std::atomic<void *> Stroika::Foundation::Memory::Private_::BlockAllocationPool_<8>::sHeadLink_' has different type in 'c:\sandbox\stroika\devroot\library\sour
ces\stroika\foundation\debug\assertions.cpp' and 'c:\sandbox\stroika\devroot\samples\ssdpclient\sources\ssdpclient.cpp': 'struct (4 bytes)' and '__declspec(align(4)) struct (4 bytes)
*/
#ifndef qCompilerAndStdLib_inline_static_align_Buggy

#if defined(_MSC_VER)
// first seen broken in _MS_VS_2k17_15Pt7Pt5_
// assume broken in _MS_VS_2k17_15Pt7Pt6_
// verified BROKEN _MSC_VER_2k17_15Pt8_ (OK in debug build, but broken in Release-U-32)
#define qCompilerAndStdLib_inline_static_align_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_inline_static_align_Buggy 0
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

/*
@CONFIGVAR:     qCompiler_SanitizerFunctionPtrConversionSuppressionBug
/*
 *  Running regression tests (built with clang (4 and 5) and sanitizer debug options)
 *      clang++-debug/Test31
 *      clang++-debug/Test33
 *      clang++-debug/Test40
 *
        /usr/bin/../lib/gcc/x86_64-linux-gnu/7.2.0/../../../../include/c++/7.2.0/bits/std_function.h:706:14: runtime error: call to function (unknown) through pointer to incorrect function type 'Stroika::Foundation::DataExchange::VariantValue (*)(const std::_Any_data &, const Stroika::Foundation::DataExchange::ObjectVariantMapper &, const void *&&)'
        (/home/lewis/Sandbox/Stroika-Dev/Builds/clang++-debug/Test31+0x176a5a0): note: (unknown) defined here
        SUMMARY: AddressSanitizer: undefined-behavior /usr/bin/../lib/gcc/x86_64-linux-gnu/7.2.0/../../../../include/c++/7.2.0/bits/std_function.h:706:14 in
        /usr/bin/../lib/gcc/x86_64-linux-gnu/7.2.0/../../../../include/c++/7.2.0/bits/std_function.h:706:14: runtime error: call to function (unknown) through pointer to incorrect function type 'void (*)(const std::_Any_data &, const Stroika::Foundation::DataExchange::ObjectVariantMapper &, const Stroika::Foundation::DataExchange::VariantValue &, void *&&)'
        (/home/lewis/Sandbox/Stroika-Dev/Builds/clang++-debug/Test31+0x176b760): note: (unknown) defined here
        SUMMARY: AddressSanitizer: undefined-behavior /usr/bin/../lib/gcc/x86_64-linux-gnu/7.2.0/../../../../include/c++/7.2.0/bits/std_function.h:706:14 in

 * @see https://stroika.atlassian.net/browse/STK-601 for details on why this is needed (ObjectVariantMapper)
 *
 *  NOTE - this is really NOT A BUG. I just cannot find a clean way to disable this (CORRECT BUT OK) runtime warning (well ubsan calls it an error but ...)
 *  THIS "BUG" really just tracks that fact that I cannot find a clean way to suppress this valid warning
 *
 *  \note @see GenerateConfiguration.pl for actual workaround to this bug - not in C++ code
*/
#if !defined(qCompiler_SanitizerFunctionPtrConversionSuppressionBug)

#if defined(__clang__) && !defined(__APPLE__)
/// CANNOT TEST YET - cuz don't have sanitzer stuff working with my private clang-7 build. For now assume broken so I have less trouble
// getting the clang-7 sanitzer stuff owrking (and when the rest is working, retry this maybe)
#define qCompiler_SanitizerFunctionPtrConversionSuppressionBug (__clang_major__ <= 7)
#else
#define qCompiler_SanitizerFunctionPtrConversionSuppressionBug 0
#endif

#endif

/*
 *  qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
 *
 *   Looking at returned string object from locale - its got a bogus length. And then the DTOR for that string causes crash. Just don't
 *   use this til debugged.
 */
#ifndef qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
#else
#define qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy 0
#endif

#endif

/*
 *  DateTime::Format (const locale& l, const String& formatPattern) const
 *     const time_put<wchar_t>& tmput = use_facet<time_put<wchar_t>> (l);
 *     tmput.put (oss, oss, ' ', &when, formatPattern.c_str (), formatPattern.c_str () + formatPattern.length ());
 *  with formatPattern = "%c" - produces a numeric format date with C locale, compared to UNIX which produces 
 *  %a %b %e %T %Y - as is suggestged by code docs and https://en.cppreference.com/w/cpp/locale/time_put/put (example - not clear cuz not for all locales)
   ...
 */
#ifndef qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k17_15Pt8_
#define qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_locale_pctC_returns_numbers_not_alphanames_Buggy 0
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
 */
#ifndef qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k17_15Pt8_
#define qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_locale_time_get_loses_part_of_date_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy

#if defined(_GLIBCXX_RELEASE)
#define qCompilerAndStdLib_locale_get_time_needsStrptime_sometimes_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_GLIBCXX_RELEASE <= 8)
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
    DISABLE_COMPILER_MSC_WARNING_START (4573);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
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
// first broken in _MSC_VER_2k17_15Pt8_
#define qCompilerAndStdLib_lambda_expand_in_namespace_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_lambda_expand_in_namespace_Buggy 0
#endif

#endif

/*
 * https://developercommunity.visualstudio.com/content/problem/330322/stdlocale-l2-en-usutf-8-broken-crashes-now.html
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
 */
#ifndef qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy

#if defined(_MSC_VER)
// first broken in _MSC_VER_2k17_15Pt8_
#define qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_VER <= _MSC_VER_2k17_15Pt8_)
#else
#define qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy 0
#endif

#endif

#ifndef qCompilerAndStdLib_quick_exit_Buggy

#if defined(__clang__) && defined(__APPLE__)
// Still broken XCode 10 - beta
#define qCompilerAndStdLib_quick_exit_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ <= 10))
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

#if defined(__GNUC__)
// tested still generates warning with gcc8
#define qCompilerAndStdLib_arm_openssl_valgrind_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ <= 8)
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
#pragma message("Stroika requires at least C++ ISO/IEC 14882:2017(E) supported by the compiler (informally known as C++ 17)")
#endif
#elif qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
#if __cplusplus < kStrokia_Foundation_Configuration_cplusplus_14
#pragma message("Stroika requires at least C++ ISO/IEC 14882:2014(E) supported by the compiler (informally known as C++ 14) - and really uses C++17 features if/when possible")
#endif
#else
#if __cplusplus < kStrokia_Foundation_Configuration_cplusplus_17
#pragma message("Stroika requires at least C++ ISO/IEC 14882:2017(E) supported by the compiler (informally known as C++ 17)")
#endif
#endif

// Stroika v2.1 requires C++17, but due to this bug, to enable that with clang, you must say c++14, and then a special warning define
// so you don't get warnings for using C++17 features. Not perfect, but will have to do
#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy && __cplusplus < kStrokia_Foundation_Configuration_cplusplus_17
#pragma clang diagnostic ignored "-Wc++17-extensions"
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
//  and if debug is false, expands to nothing. So the compiler sees differnt expectations of whether you ever get the the line
//  return x;
#pragma warning(disable : 4702)
#endif

// doesn't seem any portable way todo this, and not defined in C++ language
// Note - this doesn't appear in http://en.cppreference.com/w/cpp/language/attributes - as of 2016-06-22
#if defined(__clang__) || defined(__GNUC__)
#define dont_inline __attribute__ ((noinline))
#else
#define dont_inline __declspec(noinline)
#endif

#if !defined(Lambda_Arg_Unused_BWA)
#if qCompilerAndStdLib_MaybeUnusedIgnoredInLambdas_Buggy
#define Lambda_Arg_Unused_BWA(x) ((void)&x)
#else
#define Lambda_Arg_Unused_BWA(x)
#endif
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

/*
 *  Wrap this macro around entire declaration, as in:
 *       _DeprecatedFile_ ("DEPRECATED in v2.0a32 - use IO::FileSystem::DirectoryIterator");
 */
#define STRINGIFY(a) #a
#if !defined(_DeprecatedFile_)
#define _DeprecatedFile_(MESSAGE) \
    _Pragma (STRINGIFY (message##DEPRECATED##MESSAGE))
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
 *      Stroika_Foundation_Configuration_STRUCT_PACKED (struct ICMPHeader {
 *          byte     type; // ICMP packet type
 *          byte     code; // Type sub code
 *          uint16_t checksum;
 *          uint16_t id;
 *          uint16_t seq;
 *          uint32_t timestamp; // not part of ICMP, but we need it
 *      });
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
}
#endif

#endif /*defined(__cplusplus)*/

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

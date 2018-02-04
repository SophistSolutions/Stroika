﻿/*
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
 * We want to issue a warning about the compiler/build system compatability flags, but we dont want to issue the warning for every file
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
#if (__clang_major__ < 8) || (__clang_major__ == 8 && (__clang_minor__ < 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika does not support versions prior to clang++ 8.0 (APPLE)"
#endif
#if (__clang_major__ > 9) || (__clang_major__ == 9 && (__clang_minor__ > 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Info: Stroika untested with this version of clang++ (APPLE) - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#else
// Must check CLANG first, since CLANG also defines GCC
// see
//      clang++-3.8 -dM -E - < /dev/null
#if (__clang_major__ < 3) || (__clang_major__ == 3 && (__clang_minor__ < 9))
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika does not support versions prior to clang++ 3.9 (non-apple)"
#endif
#if (__clang_major__ > 5) || (__clang_major__ == 5 && (__clang_minor__ > 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Info: Stroika untested with this version of clang++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#endif

#elif defined(__GNUC__)

#if __GNUC__ < 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ < 0))
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika does not support versions prior to GCC 5.0"
#endif
#if __GNUC__ > 7 || (__GNUC__ == 7 && (__GNUC_MINOR__ > 2))
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

#if _MSC_VER < 1910
#define _STROIKA_CONFIGURATION_WARNING_ "Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2017"
#elif _MSC_VER <= 1912
// check which sub-version of MSVC2k17
#if _MSC_FULL_VER > _MS_VS_2k17_15Pt5Pt5_
// @todo figure out how to add arg to message
#define _STROIKA_CONFIGURATION_WARNING_ "Info: This version ( #_MSC_FULL_VER ) of Stroika is untested with this Update of of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
#define CompilerAndStdLib_AssumeBuggyIfNewerCheck_(X) 1
#endif
#else
#define _STROIKA_CONFIGURATION_WARNING_ "Info: This version of Stroika is untested with this release of Microsoft Visual Studio.net / Visual C++ - USING PREVIOUS COMPILER VERSION BUG DEFINES"
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

#if defined(__clang__)
// include __config which defines _LIBCPP_VERSION by including ciso646 (see http://stackoverflow.com/questions/31657499/how-to-detect-stdlib-libc-in-the-preprocessor)
// For libc++ it is recommended to #include <ciso646> which serves no purpose in C++ and declares nothing, but for libc++ does define the _LIBCPP_VERSION macro
#include <ciso646>
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
#define qCompilerAndStdLib_alignas_Sometimes_Mysteriously_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
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
 *                []  (19 seconds)  [46]  Foundation::Time  (../Builds/Debug-U-32/Test46/Test46.exe) crash/assert failure
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
#define qCompilerAndStdLib_std_get_time_pctx_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
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
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
#else
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy 0
#endif

#endif

/*
@ see https://stroika.atlassian.net/browse/STK-635

@DESCRIPTION:   http://stackoverflow.com/questions/24342455/nested-static-constexpr-of-incomplete-type-valid-c-or-not
*/
#ifndef qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 8) || ((__clang_major__ == 8) && (__clang_minor__ <= 1)))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 9)))
#elif defined(__GNUC__)
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ < 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 5)))
#else
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy 0
#endif

#endif

/*
 *  This ONLY affects arm-linux-gnueabihf-g++-7
 *
 *  https://stroika.atlassian.net/browse/STK-631
 *
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
 *
 */
#ifndef qCompilerAndStdLib_asan_on_arm_SetOfString_Buggy

#if defined(__GNUC__) && !defined(__clang__) && defined(__arm__)
#define qCompilerAndStdLib_asan_on_arm_SetOfString_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ < 7 || (__GNUC__ == 7 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompilerAndStdLib_asan_on_arm_SetOfString_Buggy 0
#endif

#endif

/*
 */
#ifndef qCompilerAndStdLib_template_extra_picky_templatetypenametemplate_Buggy

#if defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_template_extra_picky_templatetypenametemplate_Buggy (((__clang_major__ == 5) && (__clang_minor__ <= 0)))
#else
#define qCompilerAndStdLib_template_extra_picky_templatetypenametemplate_Buggy 0
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


@see also qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy - which provides another possible workaround 
http://stackoverflow.com/questions/24342455/nested-static-constexpr-of-incomplete-type-valid-c-or-not
that doesn't work (duplicate definitions - works in a single file but not across multiple files)

*/
#ifndef qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 9) || ((__clang_major__ == 9) && (__clang_minor__ <= 0)))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 5) || ((__clang_major__ == 5) && (__clang_minor__ <= 0)))
#elif defined(__GNUC__)
// APPEARS still broken with gcc 6.2
// APPEARS still broken with gcc 6.3
// APPEARS still broken with gcc 7.1
// APPEARS still broken with gcc 7.2
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ < 7 || (__GNUC__ == 7 && (__GNUC_MINOR__ <= 2)))
#elif defined(_MSC_VER)
// STILL WARNINGS in _MS_VS_2k17_15Pt1_
// now link error in _MS_VS_2k17_15Pt3Pt2_
// still broken (warnigns about second def ignored) - _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
#else
#define qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy 0
#endif

#endif

/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
1>  SocketAddress.cpp
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
1>  Socket.cpp
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
*
GCC:
error: redefinition of ‘constexpr const Stroika::Foundation::IO::Network::InternetAddress Stroika::Foundation::IO::Network::V4::kAddrAn


Vusual studio:

1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\io\network\InternetAddress.inl(255): error C2370: 'Stroika::Foundation::IO::Network::V4::kAddrAny': redefinition; different storage class (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\SSL\SSLSocket.cpp)
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\io\network\InternetAddress.h(328): note: see declaration of 'Stroika::Foundation::IO::Network::V4::kAddrAny' (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\SSL\SSLSocket.cpp)
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\io\network\InternetAddress.inl(256): error C2370: 'Stroika::Foundation::IO::Network::V4::kLocalhost': redefinition; different storage class (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\SSL\SSLSocket.cpp)
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\io\network\InternetAddress.h(329): note: see declaration of 'Stroika::Foundation::IO::Network::V4::kLocalhost' (compiling source file ..\..\Sources\Stroika\Foundation\Cryptography\SSL\SSLSocket.cpp)



clang 5.0
In file included from ./../../IO/Network/InternetAddress.h:392:
./../../IO/Network/InternetAddress.inl:280:47: error: redefinition of 'kAddrAny'
                    constexpr InternetAddress kAddrAny{in_addr{}};
                                              ^
./../../IO/Network/InternetAddress.h:352:43: note: previous definition is here
                    const InternetAddress kAddrAny;


*/
#ifndef qCompilerAndStdLib_constexpr_union_variants_Buggy

#if !defined(__clang__) && defined(__GNUC__)
// still broken with gcc 6.2
// still broken with gcc 7.1
#define qCompilerAndStdLib_constexpr_union_variants_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ < 7 || (__GNUC__ == 7 && (__GNUC_MINOR__ <= 2)))
#elif defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_constexpr_union_variants_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 9) || ((__clang_major__ == 9) && (__clang_minor__ <= 0)))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_constexpr_union_variants_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 5) || ((__clang_major__ == 5) && (__clang_minor__ <= 0)))
#elif defined(_MSC_VER)
// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
#define qCompilerAndStdLib_constexpr_union_variants_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
#else
#define qCompilerAndStdLib_constexpr_union_variants_Buggy 0
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
#define qCompilerAndStdLib_process_init_constructor_array_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ == 7 && (__GNUC_MINOR__ <= 2))
#else
#define qCompilerAndStdLib_process_init_constructor_array_Buggy 0
#endif

#endif

/**
 *
 *  In file included from /home/lewis/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Debug/TimingTrace.h:10:0,
                 from Test.cpp:10:
        /home/lewis/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Debug/../Execution/WhenTimeExceeded.h:34:68: error: field ‘fRunIfTakesTooLong’ has incomplete type ‘std::function<void(double) noexcept>’
                 function<void(Time::DurationSecondsType) noexcept> fRunIfTakesTooLong;
                                                                    ^~~~~~~~~~~~~~~~~~
    In file included from /home/lewis/gcc-7.1.0/include/c++/7.1.0/functional:58:0,
                 from /home/lewis/Sandbox/Stroika-Dev/Library/Sources/Stroika/Foundation/Cache/../Characters/../Containers/../Common/Compare.h:9,
 
 https://stackoverflow.com/questions/38760784/how-will-c17-exception-specifier-type-system-work

 *  This appears to be less of a compiler bug, and more of a language bug introduced in C++17. Wait til I better understand how
 *  it is, or will be resolved.
 *
 *  Right now - as of C++17 - it appears that std::function<void() noexcept> may not be well-defined.
 *
 */
#ifndef qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy

#if !defined(__clang__) && defined(__GNUC__)
#define qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ == 7 && (__GNUC_MINOR__ <= 2))
#elif defined(_MSC_VER)
// first broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
#define qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
#else
#define qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy 0
#endif

#endif

/**
 * Issue doesn't affect Ubuntu 16.04, but appears to affect any system using libc 2.26 or later with g++5
 */
#ifndef qCompilerAndStdLib_glibc_stdfunctionmapping_Buggy

#if !defined(__clang__) && defined(__GNUC__)
#define qCompilerAndStdLib_glibc_stdfunctionmapping_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ == 5 && (__GLIBC_MINOR__ > 23))
#else
#define qCompilerAndStdLib_glibc_stdfunctionmapping_Buggy 0
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
#define qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
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
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
#else
#define qCompilerAndStdLib_cplusplus_macro_value_Buggy 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy

    The issue is really
            warning: use of this statement in a constexpr function is a C++14
    for require lines at the start of constexp functions

*** FOR GCC 5.3 we get:
troika::Foundation::Configuration::EnumNames<Stroika::Foundation::Execution::Thread::Status>::BasicArrayInitializer{std::__cxx1998::__array_traits<std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>, 5ul>::_Type{std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)0u, ((const wchar_t*)"N\000\000\000u\000\000\000l\000\000\000l\000\000\000\000\000\000")}, std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)1u, ((const wchar_t*)"N\000\000\000o\000\000\000t\000\000\000-\000\000\000Y\000\000\000e\000\000\000t\000\000\000-\000\000\000R\000\000\000u\000\000\000n\000\000\000n\000\000\000i\000\000\000n\000\000\000g\000\000\000\000\000\000")}, std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)2u, ((const wchar_t*)"R\000\000\000u\000\000\000n\000\000\000n\000\000\000i\000\000\000n\000\000\000g\000\000\000\000\000\000")}, std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)3u, ((const wchar_t*)"A\000\000\000b\000\000\000o\000\000\000r\000\000\000t\000\000\000i\000\000\000n\000\000\000g\000\000\000\000\000\000")}, std::pair<Stroika::Foundation::Execution::Thread::Status, const wchar_t*>{(Stroika::Foundation::Execution::Thread::Status)4u, ((const wchar_t*)"C\000\000\000o\000\000\000m\000\000\000p\000\000\000l\000\000\000e\000\000\000t\000\000\000e\000\000\000d\000\000\000\000\000\000")}}})’
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/../Characters/../Configuration/Enumeration.inl:108:49: error: ‘constexpr void Stroika::Foundation::Configuration::EnumNames<ENUM_TYPE>::RequireItemsOrderedByEnumValue_() const [with ENUM_TYPE = Stroika::Foundation::Execution::Thread::Status]’ called in a constant expression
RequireItemsOrderedByEnumValue_ ();
^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/../Characters/../Configuration/Enumeration.inl:207:41: note: ‘constexpr void Stroika::Foundation::Configuration::EnumNames<ENUM_TYPE>::RequireItemsOrderedByEnumValue_() const [with ENUM_TYPE = Stroika::Foundation::Execution::Thread::Status]’ is not usable as a constexpr function because:
inline  constexpr   void    EnumNames<ENUM_TYPE>::RequireItemsOrderedByEnumValue_ () const
^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/../Characters/../Configuration/Enumeration.inl:207:41: sorry, unimplemented: unexpected AST of kind loop_expr
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/../Characters/../Configuration/Enumeration.inl:207: confused by earlier errors, bailing out

*/
#ifndef qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy

#if !defined(__clang__) && defined(__GNUC__)
// this is still broken even if you say -std=+14 in gcc49
// this is still broken even if you say -std=+14 in gcc51
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy (__GNUC__ <= 5)
#else
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_OptionalWithForwardDeclare_Buggy

./../../../Foundation/DataExchange/Variant/CharacterDelimitedLines/../../ObjectVariantMapper.inl:311:37: error: no viable overloaded '='



CLANG 5.0:
/home/lewis/clang-5.0.0/bin/../include/c++/v1/type_traits:3061:38: error: incomplete type 'Stroika::Foundation::IO::Network::Transfer::Connection::Options::Authentication' used in type trait expression
    : public integral_constant<bool, __is_constructible(_Tp, _Args...)>
                                     ^
./../../../Memory/Optional.h:399:30: note: in instantiation of template class 'std::__1::is_constructible<Stroika::Foundation::IO::Network::Transfer::Connection::Options::Authentication, const
      Stroika::Foundation::IO::Network::Transfer::Connection::Options::Authentication &>' requested here
                        std::is_constructible<T, const T2&>::value and
                             ^
./../../../Memory/Optional.h:401:17: note: in instantiation of default argument for 'Optional<Stroika::Foundation::IO::Network::Transfer::Connection::Options::Authentication,
      Stroika::Foundation::Memory::Optional_Traits_Blockallocated_Indirect_Storage<Stroika::Foundation::IO::Network::Transfer::Connection::Options::Authentication> >' required here
                Optional (const Optional<T2, TRAITS2>& from);
                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
./../../../Memory/Optional.h:351:19: note: while substituting deduced template arguments into function template 'Optional' [with T2 = Stroika::Foundation::IO::Network::Transfer::Connection::Options::Authentication, TRAITS2 =
      Stroika::Foundation::Memory::Optional_Traits_Blockallocated_Indirect_Storage<Stroika::Foundation::IO::Network::Transfer::Connection::Options::Authentication>, SFINAE_SAFE_CONVERTIBLE = (no value)]
            class Optional : private conditional<TRAITS::kIncludeDebugExternalSync, Debug::AssertExternallySynchronizedLock, Execution::NullMutex>::type {
                  ^
./Connection.h:205:40: note: while declaring the implicit copy constructor for 'Options'
                    struct Connection::Options {
                                       ^
./Connection.h:269:32: note: forward declaration of 'Stroika::Foundation::IO::Network::Transfer::Connection::Options::Authentication'
                        struct Authentication;


*/
#ifndef qCompilerAndStdLib_OptionalWithForwardDeclare_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_OptionalWithForwardDeclare_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 9) || ((__clang_major__ == 9) && (__clang_minor__ <= 0)))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_OptionalWithForwardDeclare_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 5) || ((__clang_major__ == 5) && (__clang_minor__ <= 0)))
#else
#define qCompilerAndStdLib_OptionalWithForwardDeclare_Buggy 0
#endif

#endif

/**
          Compiling  $StroikaRoot/Library/Sources/Stroika/Frameworks/WebServer/Request.cpp...
ConnectionManager.cpp:100:53: error: default initialization of an object of const type 'const ConnectionManager::Options' without a user-provided default constructor
const ConnectionManager::Options ConnectionManager::kDefaultOptions;
                                                    ^
 */
#ifndef qCompilerAndStdLib_DefaultCTORNotAutoGeneratedSometimes_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_DefaultCTORNotAutoGeneratedSometimes_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 8) || ((__clang_major__ == 8) && (__clang_minor__ <= 0)))
#else
#define qCompilerAndStdLib_DefaultCTORNotAutoGeneratedSometimes_Buggy 0
#endif

#endif

/**
 */
#ifndef qCompilerAndStdLib_deprecated_attribute_itselfProducesWarning_Buggy

#if !defined(__clang__) && defined(__GNUC__)
#define qCompilerAndStdLib_deprecated_attribute_itselfProducesWarning_Buggy (__GNUC__ <= 5)
#else
#define qCompilerAndStdLib_deprecated_attribute_itselfProducesWarning_Buggy 0
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
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 9) || ((__clang_major__ == 9) && (__clang_minor__ <= 0)))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 5) || ((__clang_major__ == 5) && (__clang_minor__ <= 0)))
#elif defined(__GNUC__)
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy (__GNUC__ == 5 && (__GNUC_MINOR__ <= 5))
#elif defined(_MSC_VER)
// still broken in _MS_VS_2k17_15Pt1_
// still broken in _MS_VS_2k17_15Pt3Pt2_
// still broken in _MS_VS_2k17_15Pt5Pt0_
// Assume broken in _MS_VS_2k17_15Pt5Pt2_
// Assume broken in _MS_VS_2k17_15Pt5Pt3_
// Assume broken in _MS_VS_2k17_15Pt5Pt5_
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
#else
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_SFINAE_SharedPtr_Buggy

In file included from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.h:180:0,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Generator.inl:8,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Generator.h:91,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Iterable.inl:17,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Iterable.h:1004,
                 from ../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/DirectoryIterable.cpp:6:
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl: In instantiation of ‘Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&)::MyIterable_::Rep::Rep(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]’:
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/../Memory/../Memory/SharedPtr.inl:519:73:   required from ‘Stroika::Foundation::Memory::SharedPtr<T> Stroika::Foundation::Memory::MakeSharedPtr(ARG_TYPES&& ...) [with T = Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]::MyIterable_::Rep; ARG_TYPES = {const Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::Characters::String, std::iterator<std::forward_iterator_tag, Stroika::Foundation::Characters::String, long int, Stroika::Foundation::Characters::String*, Stroika::Foundation::Characters::String&> >&}]’
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Iterable.h:143:89:   required from ‘static Stroika::Foundation::Memory::SharedPtr<T> Stroika::Foundation::Traversal::IterableBase::MakeSharedPtr(ARGS_TYPE&& ...) [with SHARED_T = Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]::MyIterable_::Rep; ARGS_TYPE = {const Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::Characters::String, std::iterator<std::forward_iterator_tag, Stroika::Foundation::Characters::String, long int, Stroika::Foundation::Characters::String*, Stroika::Foundation::Characters::String&> >&}]’
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:150:136:   required from ‘Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&)::MyIterable_::MyIterable_(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]’
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:153:17:   required from ‘Stroika::Foundation::Traversal::Iterable<T> Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]’
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/DirectoryIterable.cpp:29:84:   required from here
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:129:25: internal compiler error: Segmentation fault
                         Rep (const Iterator<T>& originalIterator)
                         ^
Please submit a full bug report,
with preprocessed source if appropriate.
See <file:///usr/share/doc/gcc-4.8/README.Bugs> for instructions.

*/
#ifndef qCompilerAndStdLib_SFINAE_SharedPtr_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_SFINAE_SharedPtr_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ == 8) && (__clang_minor__ <= 0))
#else
#define qCompilerAndStdLib_SFINAE_SharedPtr_Buggy 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy

Compiling  $StroikaRoot/Tests/43/../TestHarness/TestHarness.cpp...
/home/lewis/Sandbox/StroikaDev/Tests/43/Test.cpp:66:41: warning: moving a temporary object prevents copy elision [-Wpessimizing-move]
Optional<NotCopyable>   n2 (std::move (NotCopyable ()));    // use r-value reference to move
*/
#ifndef qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 9) || ((__clang_major__ == 9) && (__clang_minor__ <= 0)))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 4) || (__clang_minor__ <= 0))
#else
#define qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy 0
#endif

#endif

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
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy (_LIBCPP_VERSION <= 5000)
#else
#define qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy


In file included from ../../../Tests/29/Test.cpp:9:0:
../../..//Library/Sources/Stroika/Foundation/Containers/Bijection.h:493:49: error: 'bool Stroika::Foundation::Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::Lookup(Stroika::Foundation::Configuration::ArgByValueType<T>, Stroika::Foundation::Memory::Optional<RANGE_TYPE>*) const [with DOMAIN_TYPE = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred; RANGE_TYPE = Stroika::Foundation::Characters::String; TRAITS = Stroika::Foundation::Containers::Bijection_DefaultTraits<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred, Stroika::Foundation::Characters::String, Stroika::Foundation::Common::ComparerWithEquals<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred>, Stroika::Foundation::Common::ComparerWithEqualsOptionally<Stroika::Foundation::Characters::String> >; Stroika::Foundation::Configuration::ArgByValueType<T> = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred]', declared using local type 'Stroika::Foundation::Configuration::ArgByValueType<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred> {aka {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred}', is used but never defined [-fpermissive]
                 virtual  bool                   Lookup (ArgByValueType<DOMAIN_TYPE> key, Memory::Optional<RangeType>* item) const           =   0;
                                                 ^
../../..//Library/Sources/Stroika/Foundation/Containers/Bijection.h:495:49: error: 'void Stroika::Foundation::Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::Add(Stroika::Foundation::Configuration::ArgByValueType<T>, Stroika::Foundation::Configuration::ArgByValueType<RANGE_TYPE>) [with DOMAIN_TYPE = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred; RANGE_TYPE = Stroika::Foundation::Characters::String; TRAITS = Stroika::Foundation::Containers::Bijection_DefaultTraits<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred, Stroika::Foundation::Characters::String, Stroika::Foundation::Common::ComparerWithEquals<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred>, Stroika::Foundation::Common::ComparerWithEqualsOptionally<Stroika::Foundation::Characters::String> >; Stroika::Foundation::Configuration::ArgByValueType<T> = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred; Stroika::Foundation::Configuration::ArgByValueType<RANGE_TYPE> = const Stroika::Foundation::Characters::String&]', declared using local type 'Stroika::Foundation::Configuration::ArgByValueType<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred> {aka {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred}', is used but never defined [-fpermissive]
                 virtual  void                   Add (ArgByValueType<DOMAIN_TYPE> key, ArgByValueType<RANGE_TYPE> newElt)                    =   0;

../../..//Library/Sources/Stroika/Foundation/Containers/Bijection.h:493:49: error: 'bool Stroika::Foundation::Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::Lookup(Stroika::Foundation::Configuration::ArgByValueType<T>, Stroika::Foundation::Memory::Optional<RANGE_TYPE>*) const [with DOMAIN_TYPE = {anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred; RANGE_TYPE = Stroika::Foundation::Characters::String; TRAITS = Stroika::Foundation::Containers::Bijection_DefaultTraits<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred, Stroika::Foundation::Characters::String, Stroika::Foundation::Common::ComparerWithEquals<{anonymous}::DoRegressionTests_SimpleEnumTypes_5_()::Fred>, Stroika::Foundation::Common::ComparerWithEqualsOptionally<Stroika::Foundation::Characters::String> >; Stroika::Foundation::Configuration::ArgByValueType<T> =


*/
#ifndef qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy

#if !defined(__clang__) && defined(__GNUC__)
#define qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2))
#else
#define qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
@DESCRIPTION:   <p>Defined true if the compiler generates errors for static assertions in functions
            which should never be expanded. Such functions/static_assertions CAN be handy to make it
            more obvious of type mismatches with As<> etc templates.


    *** NOTE - this is so widely broken in clang/gcc - this maybe my misunderstanding. Review...

        "static assertion failed: Only specifically specialized variants are supported"


        @todo - REVIEW - THIS SEEMS MORE AND MORE LIKE MY BUG -- LGP 2017-05-04
</p>
*/
#ifndef qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 9) || ((__clang_major__ == 9) && (__clang_minor__ <= 0)))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 5) || ((__clang_major__ == 5) && (__clang_minor__ <= 0)))
#elif defined(__GNUC__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ < 7 || (__GNUC__ == 7 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy 0
#endif

#endif

/*
Compiling regtests for Median/OrderBy...
>; _Compare = std::function<bool(const int&, const int&)>]’
../../..//Library/Sources/Stroika/Foundation/Characters/../Containers/../Traversal/Iterable.inl:538:22:   required from ‘RESULT_TYPE Stroika::Foundation::Traversal::Iterable<T>::Median(const std::function<bool(T, T)>&) const [with RESULT_TYPE = int; T = int]’
../../../Tests/46/Test.cpp:781:13:   required from here
/usr/include/c++/5/bits/predefined_ops.h:123:31: error: invalid type argument of unary ‘*’ (have ‘int’)
*/
#ifndef qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Bugg

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ == 8) && (__clang_minor__ <= 0))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ == 3) && (7 <= __clang_minor__ and __clang_minor__ <= 9))
#elif defined(__GNUC__)
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy ((__GNUC__ == 5 && (__GNUC_MINOR__ == 1)) and __cplusplus == kStrokia_Foundation_Configuration_cplusplus_14)
#else
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy 0
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
#define qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_MSC_FULL_VER <= _MS_VS_2k17_15Pt5Pt5_)
#else
#define qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy 0
#endif

#endif

/**
 *  This works, and as near as I can tell, its just a compiler 'misfeature' with that make_unique_lock is slightly slower.
 *
 *  Tell that this is slow by running/comparing performance regtests with and without.
 *      Test:
 *          time Builds/Release-U-32/Test47/Test47 -x 1
 *
 *          Run test several time and average realtime results.
 *
 *          Run same test on UNIX and Windows.
 *
 *      \note   Tiny improvement found 2016-06-09 - v2.0a148x - with setting '1' on gcc 5.3-release, and even more tiny improvement
 *              for vs2k15 (Release-U-32). But for now - take the improvement
 */
#ifndef qCompilerAndStdLib_make_unique_lock_IsSlow

#define qCompilerAndStdLib_make_unique_lock_IsSlow 1

#endif /*qCompilerAndStdLib_make_unique_lock_IsSlow*/

/*
 *
 * clang uses the g++ stdlib, and since the bug is in both places, I suspect the issue is with the library, and
 * not the compiler, but its hard to unravel, and I need to get back to other things, so just disable. Now it only
 * affects Bijection<> and when you use it with types that have no operator< and operator== (type needs both to workaround
 * this bug) DECLARED - but  not DEFINED (somtimes need to define, but can be assert(false) definition).
 *
 EXAMPLE OUTPUT:
 In file included from /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Tests/11/Test.cpp:7:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/iostream:39:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/ostream:38:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/ios:40:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/char_traits.h:39:
In file included from /usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/stl_algobase.h:64:
/usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/stl_pair.h:215:24: error: invalid operands to binary expression ('const Stroika::SimpleClassWithoutComparisonOperators' and
      'const Stroika::SimpleClassWithoutComparisonOperators')
    { return __x.first == __y.first && __x.second == __y.second; }
             ~~~~~~~~~ ^  ~~~~~~~~~
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:103:64: note: in instantiation of function template specialization
      'std::operator==<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators>' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
                                                               ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:82:55: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
            static auto check(const X& x) -> decltype(XTEST);\
                                                      ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:103:13: note: while substituting deduced template arguments into function template 'check'
      [with X = std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators>]
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:84:35: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
            using type = decltype(check(declval<T>()));\
                                  ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:103:13: note: in instantiation of template class
      'Stroika::Foundation::Configuration::Private_::eq_result_impl<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:88:5: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
    using   NAME##_result = typename Private_::NAME##_result_impl<T>::type;\
    ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:103:13: note: in instantiation of template type alias 'eq_result' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/../Configuration/Concepts.h:90:63: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
    struct  has_##NAME : integral_constant <bool, not is_same<NAME##_result<T>, Stroika::Foundation::Configuration::substitution_failure>::value> {};
                                                              ^
<scratch space>:8:1: note: expanded from here
eq_result
^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Common/Compare.h:196:45: note: in instantiation of template class
      'Stroika::Foundation::Configuration::has_eq<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' requested here
                             Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value,
                                            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/../Traversal/Iterable.h:414:65: note: in instantiation of default argument for
      'DefaultEqualsComparer<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' required here
                template    <typename EQUALS_COMPARER = Common::DefaultEqualsComparer<T>>
                                                                ^~~~~~~~~~~~~~~~~~~~~~~~~
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/Bijection.h:120:40: note: in instantiation of template class
      'Stroika::Foundation::Traversal::Iterable<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' requested here
            class   Bijection : public Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>> {
                                       ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Tests/11/Test.cpp:32:44: note: in instantiation of template class 'Stroika::Foundation::Containers::Bijection<Stroika::SimpleClassWithoutComparisonOperators,
      Stroika::SimpleClassWithoutComparisonOperators, Stroika::Foundation::Containers::Bijection_DefaultTraits<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators,
      MySimpleClassWithoutComparisonOperators_ComparerWithEquals_, MySimpleClassWithoutComparisonOperators_ComparerWithEquals_> >' requested here
        auto testFunc = [] (const typename CONCRETE_CONTAINER::ArchetypeContainerType & s) {


 * OR WITH CLANG error messages look like:
/usr/bin/../lib/gcc/x86_64-linux-gnu/4.9/../../../../include/c++/4.9/bits/stl_pair.h:215:24: error: invalid operands to binary expression ('const Stroika::SimpleClassWithoutComparisonOperators' and
      'const Stroika::SimpleClassWithoutComparisonOperators')
    { return __x.first == __y.first && __x.second == __y.second; }
             ~~~~~~~~~ ^  ~~~~~~~~~
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:103:64: note: in instantiation of function template specialization
      'std::operator==<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators>' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
                                                               ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:82:55: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
            static auto check(const X& x) -> decltype(XTEST);\
                                                      ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:103:13: note: while substituting deduced template arguments into function template 'check'
      [with X = std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators>]
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
            ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:84:35: note: expanded from macro 'STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS'
            using type = decltype(check(declval<T>()));\
                                  ^
/media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Containers/DefaultTraits/../../Configuration/Concepts.h:103:13: note: in instantiation of template class
      'Stroika::Foundation::Configuration::Private_::eq_result_impl<std::pair<Stroika::SimpleClassWithoutComparisonOperators, Stroika::SimpleClassWithoutComparisonOperators> >' requested here
            STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS(eq, (x == x));
 *
 *
 */
#ifndef qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ == 8 && (__clang_minor__ <= 1))
#elif defined(__clang__) && !defined(__APPLE__)
#define qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__clang_major__ == 3 && (__clang_minor__ <= 9))
#else
#define qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy 0
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

/**
*
*   ENTER CODE:
#if __has_include (<optional>)
#endif

1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\configuration\private\defaults_compilerandstdlib_.h (1137) : warning C4067 : unexpected tokens following preprocessor directive - expected a newline

// STILL broken in _MS_VS_2k17_15Pt1_
// FIXED in _MS_VS_2k17_15Pt3Pt1_ (at least above iftest compiles) -- LGP 2017-08-20
*/
#if !defined(qCompilerAndStdLib_has_include_Buggy)
#if defined(_MSC_VER)
#define qCompilerAndStdLib_has_include_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (MSC_FULL_VER <= _MS_VS_2k17_15Pt1_)
#else
#define qCompilerAndStdLib_has_include_Buggy 0
#endif
#endif

/**
*/
#if !defined(qCompilerAndStdLib_Supports_stdoptional)
#if qCompilerAndStdLib_has_include_Buggy && defined(_MSC_VER)
#define qCompilerAndStdLib_Supports_stdoptional CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (_HAS_CXX17)
//#elif defined(__GNUC__) && __GNUC__ == 7
//#define qCompilerAndStdLib_Supports_stdoptional (__cplusplus >= kStrokia_Foundation_Configuration_cplusplus_17)
#else
// has_include works - because we have the include, but alas it somehow gets suppressed unless you compile with -std=c++1z, so test this macro and has_include
#define qCompilerAndStdLib_Supports_stdoptional ((__cplusplus >= kStrokia_Foundation_Configuration_cplusplus_17) && __has_include (<optional>))
#endif
#endif

/**
*/
#if !defined(qCompilerAndStdLib_Supports_stdexperimentaloptional)
#if qCompilerAndStdLib_Supports_stdoptional
#define qCompilerAndStdLib_Supports_stdexperimentaloptional 0
#elif qCompilerAndStdLib_has_include_Buggy
#define qCompilerAndStdLib_Supports_stdexperimentaloptional 0
#else
#define qCompilerAndStdLib_Supports_stdexperimentaloptional (__has_include (<experimental / optional>))
#endif
#endif

#if !defined(qCompilerAndStdLib_insert_or_assign_Buggy)
#if qCompilerAndStdLib_has_include_Buggy && defined(_MSC_VER)
#define qCompilerAndStdLib_insert_or_assign_Buggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (!_HAS_CXX17)
#else
#define qCompilerAndStdLib_insert_or_assign_Buggy (__cplusplus < kStrokia_Foundation_Configuration_cplusplus_17)
#endif
#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug
In file included from ../../..//Library/Sources/Stroika/Foundation/Characters/String.h:12:0,
                 from ../../../Tests/18/Test.cpp:13:
../../..//Library/Sources/Stroika/Foundation/Characters/../Containers/Sequence.h:548:45: error: 'void Stroika::Foundation::Containers::Sequence<T>::_IRep::Insert(size_t, const T*, const T*) [with T = {anonymous}::SimpleSequenceTest_14_Sequence_stdinitializer_complexType_()::StructureFieldInfo_; size_t = long unsigned int]', declared using local type 'const {anonymous}::SimpleSequenceTest_14_Sequence_stdinitializer_complexType_()::StructureFieldInfo_', is used but never defined [-fpermissive]
                 virtual void                Insert (size_t at, const T* from, const T* to)                          =   0;
                                             ^
*/
#if !defined(qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug)

#if !defined(__clang__) && defined(__GNUC__)
#define qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug ((__GNUC__ < 5) || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug 0
#endif

#endif

/*
@CONFIGVAR:     qCompiler_SanitizerVPtrTemplateTypeEraseureBug
/*
*  Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS because of https://stroika.atlassian.net/browse/STK-500
*          .. / .. / Streams / iostream / InputStreamFromStdIStream.inl:46 : 66 : runtime error : member call on address 0x60d000001300 which does not point to an object of type 'Rep_'
*              0x60d000001300 : note : object has invalid vptr
*              01 00 00 00  00 00 00 00 00 00 00 00  be be be be be be be be  be be be be be be be be  be be be be
*              ^~~~~~~~~~~~~~~~~~~~~~~
*              invalid vptr
*/
#if !defined(qCompiler_SanitizerVPtrTemplateTypeEraseureBug)

#if !defined(__clang__) && defined(__GNUC__)
#define qCompiler_SanitizerVPtrTemplateTypeEraseureBug CompilerAndStdLib_AssumeBuggyIfNewerCheck_ (__GNUC__ < 7 || (__GNUC__ == 7 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompiler_SanitizerVPtrTemplateTypeEraseureBug 0
#endif

#endif

/*
@CONFIGVAR:     qCompiler_SanitizerFunctionPtrConversionBug
/*
 *  Running regression tests (built with clang and sanitizer debug options)
 *
        /usr/bin/../lib/gcc/x86_64-linux-gnu/7.2.0/../../../../include/c++/7.2.0/bits/std_function.h:706:14: runtime error: call to function (unknown) through pointer to incorrect function type 'Stroika::Foundation::DataExchange::VariantValue (*)(const std::_Any_data &, const Stroika::Foundation::DataExchange::ObjectVariantMapper &, const void *&&)'
        (/home/lewis/Sandbox/Stroika-Dev/Builds/clang++-debug/Test31+0x176a5a0): note: (unknown) defined here
        SUMMARY: AddressSanitizer: undefined-behavior /usr/bin/../lib/gcc/x86_64-linux-gnu/7.2.0/../../../../include/c++/7.2.0/bits/std_function.h:706:14 in
        /usr/bin/../lib/gcc/x86_64-linux-gnu/7.2.0/../../../../include/c++/7.2.0/bits/std_function.h:706:14: runtime error: call to function (unknown) through pointer to incorrect function type 'void (*)(const std::_Any_data &, const Stroika::Foundation::DataExchange::ObjectVariantMapper &, const Stroika::Foundation::DataExchange::VariantValue &, void *&&)'
        (/home/lewis/Sandbox/Stroika-Dev/Builds/clang++-debug/Test31+0x176b760): note: (unknown) defined here
        SUMMARY: AddressSanitizer: undefined-behavior /usr/bin/../lib/gcc/x86_64-linux-gnu/7.2.0/../../../../include/c++/7.2.0/bits/std_function.h:706:14 in
*/
#if !defined(qCompiler_SanitizerFunctionPtrConversionBug)

#if defined(__clang__) && !defined(__APPLE__)
#define qCompiler_SanitizerFunctionPtrConversionBug (__clang_major__ < 5 or ((__clang_major__ == 5) && (__clang_minor__ <= 0)))
#else
#define qCompiler_SanitizerFunctionPtrConversionBug 0
#endif

#endif

/*
@CONFIGVAR:     qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
*
*   Looking at returned string object from locale - its got a bogus length. And then the DTOR for that string causes crash. Just dont
*   use this til debugged.
*/
#ifndef qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy

#if defined(__clang__) && defined(__APPLE__)
#define qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy CompilerAndStdLib_AssumeBuggyIfNewerCheck_ ((__clang_major__ < 9) || ((__clang_major__ == 9) && (__clang_minor__ <= 0)))
#else
#define qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy 0
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
@CONFIGVAR:     qDecoratedNameLengthExceeded_Buggy
@DESCRIPTION:
        C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\xrefwrap(173): warning C4503: 'Stroik...
        ...: decorated name length exceeded, name was truncated []
*/
#ifndef qDecoratedNameLengthExceeded_Buggy
#define qDecoratedNameLengthExceeded_Buggy 0
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
#if _MSVC_LANG < kStrokia_Foundation_Configuration_cplusplus_14
#pragma message("Stroika requires at least C++ ISO/IEC 14882:2014(E) supported by the compiler (informally known as C++ 14)")
#endif
#else
#if __cplusplus < kStrokia_Foundation_Configuration_cplusplus_14
#pragma message("Stroika requires at least C++ ISO/IEC 14882:2014(E) supported by the compiler (informally known as C++ 14)")
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

// doesn't seem any portable way todo this, and not defined in C++ language
// Note - this doesn't appear in http://en.cppreference.com/w/cpp/language/attributes - as of 2016-06-22
#if defined(__clang__) || defined(__GNUC__)
#define dont_inline __attribute__ ((noinline))
#else
#define dont_inline __declspec(noinline)
#endif

/**
 *   The Standard C++ mechanism of commenting out unused parameters isn't good enuf
 *   in the case where the parameters might be used conditionally. This hack is
 *   to shutup compiler warnings in those cases.
 *
 *      @todo eventually (when compilers support it) switch to using [[maybe_unused]] - http://en.cppreference.com/w/cpp/language/attributes
 */
#if !defined(Arg_Unused)
#define Arg_Unused(x) ((void)&x)
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

/**
 *  \def Stroika_Foundation_Configuration_STRUCT_PACKED
 *
 *  Make the argument strucuture packed - with as little empty space between elements as possible. This is not portable,
 *  but generally works.
 *
 *  \par Example Usage
 *      \code
 *      Stroika_Foundation_Configuration_STRUCT_PACKED (struct ICMPHeader {
 *          Byte     type; // ICMP packet type
 *          Byte     code; // Type sub code
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

#if qSilenceAnnoyingCompilerWarnings && defined(_MSC_VER) && qDecoratedNameLengthExceeded_Buggy
__pragma (warning (disable : 4503))
#endif

#endif /*defined(__cplusplus)*/

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

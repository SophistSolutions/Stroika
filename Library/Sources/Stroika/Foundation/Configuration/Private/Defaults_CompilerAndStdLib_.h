/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_ 1


/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */


#if     defined(__cplusplus)



/**
 *
 */
#define kStrokia_Foundation_Configuration_cplusplus_11  201103


/**
 *
 */
#define kStrokia_Foundation_Configuration_cplusplus_14  201402




/*
 *******************************************************************
 *******************************************************************
 ******** CHECK / WARN about supported compilers/versions **********
 *******************************************************************
 *******************************************************************
 */

#if     defined (__clang__)

// Must check CLANG first, since CLANG also defines GCC
#if     (__clang_major__ < 3) || (__clang_major__ == 3 && (__clang_minor__ < 5))
#pragma message ("Warning: Stroika does not support versions prior to clang++ 3.5")
#endif
#if     (__clang_major__ > 3) || (__clang_major__ == 3 && (__clang_minor__ > 6))
#pragma message ("Info: Stroika untested with this version of clang++")
#endif

#elif   defined (__GNUC__)

#if     __GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#pragma message ("Warning: Stroika does not support versions prior to GCC 4.8")
#endif
#if     (__GNUC__ == 4 && (__GNUC_MINOR__ > 9))
#pragma message ("Info: Stroika untested with this version of GCC")
#endif
#if     __GNUC__ > 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ > 2))
#pragma message ("Info: Stroika untested with this version of GCC")
#endif

#elif     defined (_MSC_VER)

#define _MS_VS_2k13_VER_                1800
// _MSC_FULL_VER value - see http://msdn.microsoft.com/en-us/library/b0084kay.aspx - so far unused
#define _MS_VS_2k13_FULLVER_            180021005
#define _MS_VS_2k13_Update1_FULLVER_    180021005
#define _MS_VS_2k13_Update2_FULLVER_    180030501
#define _MS_VS_2k13_Update3_FULLVER_    180030723
#define _MS_VS_2k13_Update4_FULLVER_    180031101
#define _MS_VS_2k13_Update5_FULLVER_    180040629

#define _MS_VS_2k15_VER_                1900
#define _MS_VS_2k15_RC_FULLVER_         190022816
#define _MS_VS_2k15_RTM_FULLVER_        190023026
#define _MS_VS_2k15_Update1_FULLVER_    190023506

#if      _MSC_VER < _MS_VS_2k13_VER_
#pragma message ("Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2013")
#endif
#if      _MSC_VER ==_MS_VS_2k13_VER_ && (_MSC_FULL_VER < _MS_VS_2k13_Update5_FULLVER_)
#pragma message ("Warning: Stroika requires update 5 if using Microsoft Visual Studio.net 2013")
#endif
#if      _MSC_VER > _MS_VS_2k15_VER_
#pragma message ("Info: This version of Stroika is untested with this version of Microsoft Visual Studio.net / Visual C++")
#elif    _MSC_FULL_VER > _MS_VS_2k15_Update1_FULLVER_
#pragma message ("Info: This version of Stroika is untested with this Update of of Microsoft Visual Studio.net / Visual C++")
#endif

#else

#pragma message ("Warning: Stroika does recognize the compiler being used. It may work, but you may need to update some of the other defines for what features are supported by your compiler.")

#endif














/*
 *******************************************************************
 *******************************************************************
 **************** Define Features (and Bug Workarounds) ************
 *******************************************************************
 *******************************************************************
 */




/*
@CONFIGVAR:     qCompilerAndStdLib_LocaleTM_put_Buggy
@DESCRIPTION:   <p></p>
*/
#ifndef qCompilerAndStdLib_LocaleTM_put_Buggy

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_LocaleTM_put_Buggy     (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8))
#else
#define qCompilerAndStdLib_LocaleTM_put_Buggy     0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy
@DESCRIPTION:   <p>Using tmput on Release, and 64bit builds core dumps. Just started. Not sure why.
            But using narrow string succeeds so workaround.

            Crashes in Test46 (performance regtest)
            </p>

    APPEARS FIXED IN MSVC2k15 RTM
*/
#ifndef qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy

#if     defined (_MSC_VER)
// Still broken in _MS_VS_2k13_Update4_FULLVER_
// Still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy     (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy     0
#endif

#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy
@DESCRIPTION:
    const time_get<wchar_t>& tmget = use_facet <time_get<wchar_t>> (l);
    istreambuf_iterator<wchar_t> i = tmget.get_date (itbegin, itend, iss, state, &when);
    ErroniousReportFailWhenDateBefore1900

    >>> TO TEST IF FIXED - RUN TIME REGRESSION TESTS (Foundation::Time - currently #44) <<<

    VERIFIED FIXED in VS 2k15 RTM
*/
#ifndef qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy

#if   defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy     (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy     0
#endif

#endif





/*
 *  In initial testing I've found no cases where this works...
 *      --LGP 2015-08-24
 */
#ifndef qCompilerAndStdLib_Locale_Buggy

#if   qPlatform_AIX
#define qCompilerAndStdLib_Locale_Buggy             1
#else
#define qCompilerAndStdLib_Locale_Buggy             0
#endif

#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy
@DESCRIPTION:

    >>> TO TEST IF FIXED - RUN TIME REGRESSION TESTS (Foundation::Time - currently #44) <<<
*/
#ifndef qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
// VERIFIED fixed in VS 2k15 RTM
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy    (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy    0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_VarDateFromStrOnFirstTry
    crazy bug - regression - with VC2k13
    Maybe same thing...
*/
#ifndef qCompilerAndStdLib_VarDateFromStrOnFirstTry_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_VarDateFromStrOnFirstTry_Buggy  (_MSC_FULL_VER < _MS_VS_2k13_Update2_FULLVER_)
#else
#define qCompilerAndStdLib_VarDateFromStrOnFirstTry_Buggy   0
#endif

#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_StdExitBuggy
*/
#ifndef qCompilerAndStdLib_StdExitBuggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_StdExitBuggy  (_MSC_FULL_VER < _MS_VS_2k15_RTM_FULLVER_)
#else
#define qCompilerAndStdLib_StdExitBuggy   0
#endif

#endif





/*
@DESCRIPTION:   Just annowing warning workaround
1>c:\program files (x86)\microsoft visual studio 14.0\vc\include\xmemory(210): warning C4996: 'std::_Uninitialized_copy_n': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'
1>  c:\program files (x86)\microsoft visual studio 14.0\vc\include\xmemory(190): note: see declaration of 'std::_Uninitialized_copy_n'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\containers\externallysynchronizeddatastructures\array.inl(215): note: see reference to function template instantiation '_FwdIt std::uninitialized_copy_n<T*,size_t,T*>(_InIt,_Diff,_FwdIt)' being compiled
*/
#ifndef qCompilerAndStdLib_uninitialized_copy_n_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
// still broken in _MS_VS_2k15_RC_FULLVER_
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_uninitialized_copy_n_Buggy  (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_uninitialized_copy_n_Buggy   0
#endif

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_make_unique_Buggy
seems missing on gcc 49 and untested otherwise, but works on msvc2k13. g++ may have in wrong folder/file (docs say memory)
*/
#ifndef qCompilerAndStdLib_make_unique_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_make_unique_Buggy    0
#else
#define qCompilerAndStdLib_make_unique_Buggy    1
#endif

#endif



/*
  Example output:
       1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\optional.h(80): error C2143: syntax error : missing ';' before 'Stroika::Foundation::Memory::Byte'
*/
#ifndef qCompilerAndStdLib_alignas_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_alignas_Buggy      (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_alignas_Buggy      0
#endif

#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_union_designators_Buggy
@DESCRIPTION:
                    union U4 {
                        short int si;
                        int        i;
                        long int  li;
                    } u4 = {.li=3};
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(79): error C2059: syntax error: '.'
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(79): error C2612: trailing '}' illegal in base/member initializer list
*/
#ifndef qCompilerAndStdLib_union_designators_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
// still broken in _MS_VS_2k15_RC_FULLVER_
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_union_designators_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_union_designators_Buggy      0
#endif

#endif



/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_Buggy
@DESCRIPTION:   NYI for VS2k13, but expected soon
*/
#ifndef qCompilerAndStdLib_constexpr_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RC_FULLVER_ (((MAYBE WORKS BUT TRY OFF FOR NOW - NEWER ISSUES)
// Fixed (ish) in _MS_VS_2k15_RTM_FULLVER_
#define qCompilerAndStdLib_constexpr_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_RC_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_Buggy      0
#endif

#endif






/*
2>c:\sandbox\stroika\devroot\library\sources\stroika\frameworks\systemperformance\instruments\../../../Foundation/DataExchange/ObjectVariantMapper.h(371): error C2589: '::' : illegal token on right side of '::'
2>c:\sandbox\stroika\devroot\library\sources\stroika\frameworks\systemperformance\instruments\../../../Foundation/DataExchange/ObjectVariantMapper.h(371): error C2059: syntax error : '::'
*/
#ifndef qCompilerAndStdLib_DefaultArgOfStaticTemplateMember_Buggy

#if   defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_DefaultArgOfStaticTemplateMember_Buggy     (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_DefaultArgOfStaticTemplateMember_Buggy     0
#endif

#endif






/*
1>e:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\interface.cpp(87): error C2799: 'k': an object of const-qualified class type without a user-provided default constructor must be initialized
1>  e:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\interface.cpp(87): note: see declaration of 'k'
1>  e:\sandbox\stroikadev\library\sources\stroika\foundation\configuration\enumeration.h(173): note: see declaration of 'Stroika::Foundation::Configuration::EnumNames<Stroika::Foundation::IO::Network::Interface::Status>::EnumNames'
2>------ Build started: Project: Test10, Configuration: Debug-U-32 Win32 ------
*/
#ifndef qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy

#if     defined (_MSC_VER)
// Still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy      0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_largeconstexprarray_Buggy
@DESCRIPTION:   causes internal compiler error
*/
#ifndef qCompilerAndStdLib_largeconstexprarray_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RC_FULLVER_ (((MAYBE WORKS BUT TRY OFF FOR NOW - NEWER ISSUES)
// Still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_largeconstexprarray_Buggy      (_MSC_FULL_VER == _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_largeconstexprarray_Buggy      0
#endif

#endif



/*
>c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(117): error C3256: 'kUpperBound': variable use does not produce a constant expression
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(117): note: while compiling class template member function 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(void)'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(582): note: see reference to function template instantiation 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(void)' being compiled
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(447): note: see reference to function template instantiation 'Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer_Range_<Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>>(void)' being compiled
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(180): note: see reference to function template instantiation 'Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer_<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>(const Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_> &)' being compiled
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.cpp(275): note: see reference to function template instantiation 'Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer<Stroika::Foundation::Time::DateRange>(void)' being compiled
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(118): error C3256: 'kLowerBound': variable use does not produce a constant expression
*/
#ifndef qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// Still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy      qCompilerAndStdLib_constexpr_Buggy
#endif

#endif


/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\memory\smallstackbuffer.h(128): error C4579: 'Stroika::Foundation::Memory::SmallStackBuffer<wchar_t,2048>::kGuard1_': in-class initialization for type 'const Stroika::Foundation::Memory::Byte [8]' is not yet implemented; static member will remain uninitialized at runtime but use in constant-expressions is supported
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\characters\stringbuilder.h(180): note: see reference to class template instantiation 'Stroika::Foundation::Memory::SmallStackBuffer<wchar_t,2048>' being compiled
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\memory\smallstackbuffer.h(128): error C2131: expression did not evaluate to a constant
                static  constexpr   Byte    kGuard1_[8]  =   { 0x45, 0x23, 0x12, 0x56, 0x99, 0x76, 0x12, 0x55, };
*/
#ifndef qCompilerAndStdLib_constexpr_arrays_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broekn in _MS_VS_2k15_Update1_FULLVER_
// Fixed in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_constexpr_arrays_Buggy      (_MSC_FULL_VER < _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_arrays_Buggy      qCompilerAndStdLib_constexpr_Buggy
#endif

#endif




/*
 *  http://stackoverflow.com/questions/8102125/is-local-static-variable-initialization-thread-safe-in-c11
*/
#ifndef qCompilerAndStdLib_static_initialization_threadsafety_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broekn in _MS_VS_2k15_Update1_FULLVER_
// Fixed in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_static_initialization_threadsafety_Buggy      (_MSC_FULL_VER < _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_static_initialization_threadsafety_Buggy      0
#endif

#endif






/*
>c:\sandbox\stroikadev\tests\35\test.cpp(203): error C2127: 'kOrigValueInit_': illegal initialization of 'constexpr' entity with a non-constant expression
*/
#ifndef qCompilerAndStdLib_constexpr_stdinitializer_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_stdinitializer_Buggy      0
#endif

#endif




/*
*/
#ifndef qCompilerAndStdLib_is_trivially_copyable_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// Fixed in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_is_trivially_copyable_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_RTM_FULLVER_)
#elif   defined (__GNUC__)
// broken in gcc49, and works in gcc52. Untested in gcc50/51
#define qCompilerAndStdLib_is_trivially_copyable_Buggy      (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9)))
#else
#define qCompilerAndStdLib_is_trivially_copyable_Buggy      0
#endif

#endif



/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(163): error C3256: 'kUpperBound': variable use does not produce a constant expression
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(163): note: while compiling class template member function 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(Stroika::Foundation::Traversal::Openness,Stroika::Foundation::Traversal::Openness)'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(124): note: see reference to function template instantiation 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(Stroika::Foundation::Traversal::Openness,Stroika::Foundation::Traversal::Openness)' being compiled
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\traversal\range.inl(122): note: while compiling class template member function 'Stroika::Foundation::Traversal::Range<Stroika::Foundation::Time::Date,Stroika::Foundation::Time::Private_::DateRangeTraitsType_>::Range(void)'
*/
#ifndef qCompilerAndStdLib_constexpr_with_delegated_construction_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// Still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_constexpr_with_delegated_construction_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_with_delegated_construction_Buggy      qCompilerAndStdLib_constexpr_Buggy
#endif

#endif




/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\characters\string.h(350): error C2131: expression did not evaluate to a constant
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\characters\string.h(350): note: failure was caused by non-constant arguments or reference to a non-constant symbol
*/
#ifndef qCompilerAndStdLib_constexpr_STL_string_npos_constexpr_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
// still broken in _MS_VS_2k15_RC_FULLVER_ (((MAYBE WORKS BUT TRY OFF FOR NOW - NEWER ISSUES)
// still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_constexpr_STL_string_npos_constexpr_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_STL_string_npos_constexpr_Buggy      qCompilerAndStdLib_constexpr_Buggy
#endif

#endif






/*
@DESCRIPTION:   http://stackoverflow.com/questions/24342455/nested-static-constexpr-of-incomplete-type-valid-c-or-not
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\time\date.h(289): error C2079: 'public: static Stroika::Foundation::Time::Date const Stroika::Foundation::Time::Date::kMin' uses undefined class 'Stroika::Foundation::Time::Date'
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\time\date.h(289): error C4579: 'Stroika::Foundation::Time::Date::kMin': in-class initialization for type 'const Stroika::Foundation::Time::Date' is not yet implemented; static member will remain uninitialized at runtime but use in constant-expressions is supported
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\time\date.h(289): error C2440: 'initializing': cannot convert from 'initializer list' to 'const Stroika::Foundation::Time::Date'
1>  c:\sandbox\stroikadev\library\sources\stroika\foundation\time\date.h(289): note: Source or target has incomplete type
*/
#ifndef qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy

#if     qCompilerAndStdLib_constexpr_Buggy
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      1
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      (__GNUC__ < 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2)))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      0
#endif

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_Iterator_template_MakeSharedPtr_gcc_crasher_Buggy
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_Iterator_template_MakeSharedPtr_gcc_crasher_Buggy

#if   defined (__GNUC__)
#define qCompilerAndStdLib_Iterator_template_MakeSharedPtr_gcc_crasher_Buggy      ((__GNUC__ == 4 && (__GNUC_MINOR__ <= 9)))
#else
#define qCompilerAndStdLib_Iterator_template_MakeSharedPtr_gcc_crasher_Buggy      0
#endif

#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy

#if     qCompilerAndStdLib_constexpr_Buggy
#define qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy      1
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy      ((__GNUC__ == 4 && (__GNUC_MINOR__ == 8)))
#else
#define qCompilerAndStdLib_constexpr_after_template_decl_constexpr_Buggy      0
#endif

#endif



/*
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
1>  SocketAddress.cpp
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
1>  Socket.cpp
1>c:\sandbox\stroikadev\library\sources\stroika\foundation\io\network\internetaddress.inl(237): error C2127: 'Stroika::Foundation::IO::Network::V4::kLocalhost': illegal initialization of 'constexpr' entity with a non-constant expression
*/
#ifndef qCompilerAndStdLib_constexpr_union_variants_Buggy

#if     qCompilerAndStdLib_constexpr_Buggy
#define qCompilerAndStdLib_constexpr_union_variants_Buggy       1
#elif   defined (_MSC_VER)
// still broken with _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_constexpr_union_variants_Buggy       (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_union_variants_Buggy       0
#endif

#endif








/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy

    The issue is really
            warning: use of this statement in a constexpr function is a C++14
    for require lines at the start of constexp functions
*/
#ifndef qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    (__cplusplus <= kStrokia_Foundation_Configuration_cplusplus_11)
#elif   defined (__GNUC__)
// this is still broken even if you say -std=+14 in gcc49
// this is still broken even if you say -std=+14 in gcc51
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    ((__GNUC__ < 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2))) || (__cplusplus <= kStrokia_Foundation_Configuration_cplusplus_11))
#elif   defined (_MSC_VER)
// Still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    qCompilerAndStdLib_constexpr_Buggy
#endif

#endif






/*
*/
#ifndef qCompilerAndStdLib_shared_mutex_module_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RTM_FULLVER_
// Fixed in _MS_VS_2k15_RTM_FULLVER_
// Fixed in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_shared_mutex_module_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_RTM_FULLVER_)
#else
#define qCompilerAndStdLib_shared_mutex_module_Buggy      (__cplusplus < kStrokia_Foundation_Configuration_cplusplus_14)
#endif

#endif









/*
 *  There are some (e.g. accept and recvfrom) APIs in the AIX headers
 *  which generate linker warning messages and then crash when I run them. These are inlines
 *  which somehow dont get generated properly.
 *      ld: 0711-768 WARNING: Object ../../../../../../Builds/DefaultConfiguration/Stroika-Foundation.a[Socket.o], section 1, function .recvfrom:
 *        The branch at address 0x2d70 is not followed by a recognized no-op
 *        or TOC-reload instruction. The unrecognized instruction is 0x7C691B78.
 *
 *  This CAN be worked around by turning on optimation so that they get inlined. But if no inlining,
 *  we must use some trick to get past this bug define to avoid the inline.
*/
#ifndef qCompilerAndStdLib_AIX_GCC_TOC_Inline_Buggy

#if     (defined (__GNUC__) && !defined (__clang__)) && qPlatform_AIX
#define qCompilerAndStdLib_AIX_GCC_TOC_Inline_Buggy      1
#else
#define qCompilerAndStdLib_AIX_GCC_TOC_Inline_Buggy      0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy
* NOTE sure if this is a bug or my misunderstanding.
*/
#ifndef qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy

#if     qCompilerAndStdLib_constexpr_Buggy
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy      1
#elif   defined (__clang__)
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy      ((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 6)))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy      (__GNUC__ < 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompilerAndStdLib_constexpr_union_enter_one_use_other_Buggy      0
#endif

#endif


#ifndef qCompilerAndStdLib_ParameterPack_Pass_Through_Lambda_Buggy

#if     defined (__GNUC__)
#define qCompilerAndStdLib_ParameterPack_Pass_Through_Lambda_Buggy      (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8)))
#else
#define qCompilerAndStdLib_ParameterPack_Pass_Through_Lambda_Buggy      0
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
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl: In instantiation of �Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&)::MyIterable_::Rep::Rep(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]�:
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/../Memory/../Memory/SharedPtr.inl:519:73:   required from �Stroika::Foundation::Memory::SharedPtr<T> Stroika::Foundation::Memory::MakeSharedPtr(ARG_TYPES&& ...) [with T = Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]::MyIterable_::Rep; ARG_TYPES = {const Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::Characters::String, std::iterator<std::forward_iterator_tag, Stroika::Foundation::Characters::String, long int, Stroika::Foundation::Characters::String*, Stroika::Foundation::Characters::String&> >&}]�
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/Iterable.h:143:89:   required from �static Stroika::Foundation::Memory::SharedPtr<T> Stroika::Foundation::Traversal::IterableBase::MakeSharedPtr(ARGS_TYPE&& ...) [with SHARED_T = Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]::MyIterable_::Rep; ARGS_TYPE = {const Stroika::Foundation::Traversal::Iterator<Stroika::Foundation::Characters::String, std::iterator<std::forward_iterator_tag, Stroika::Foundation::Characters::String, long int, Stroika::Foundation::Characters::String*, Stroika::Foundation::Characters::String&> >&}]�
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:150:136:   required from �Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&)::MyIterable_::MyIterable_(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]�
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:153:17:   required from �Stroika::Foundation::Traversal::Iterable<T> Stroika::Foundation::Traversal::MakeIterableFromIterator(const Stroika::Foundation::Traversal::Iterator<T>&) [with T = Stroika::Foundation::Characters::String]�
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/DirectoryIterable.cpp:29:84:   required from here
../../../../../../Library/Sources/Stroika/Foundation/IO/FileSystem/../../Traversal/IterableFromIterator.inl:129:25: internal compiler error: Segmentation fault
                         Rep (const Iterator<T>& originalIterator)
                         ^
Please submit a full bug report,
with preprocessed source if appropriate.
See <file:///usr/share/doc/gcc-4.8/README.Bugs> for instructions.

*/
#ifndef qCompilerAndStdLib_SFINAE_SharedPtr_Buggy

#if     defined (__GNUC__)
#define qCompilerAndStdLib_SFINAE_SharedPtr_Buggy      (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9)))
#else
#define qCompilerAndStdLib_SFINAE_SharedPtr_Buggy      0
#endif

#endif





#ifndef qCompilerAndStdLib_strtof_NAN_ETC_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_strtof_NAN_ETC_Buggy   (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_strtof_NAN_ETC_Buggy   0
#endif

#endif








#ifndef qCompilerAndStdLib_fdopendir_Buggy

#define qCompilerAndStdLib_fdopendir_Buggy   qPlatform_AIX

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy

#if     qDebug && !qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy
                : fLock_ (ATOMIC_FLAG_INIT)
#endif

1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\execution\spinlock.inl(29): error C2280: 'std::atomic_flag::atomic_flag(const std::atomic_flag &)' : attempting to reference a deleted function
1>          c:\program files (x86)\microsoft visual studio 12.0\vc\include\atomic(191) : see declaration of 'std::atomic_flag::atomic_flag'

*/
#ifndef qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RC_FULLVER_
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still broken in _MS_VS_2k15_Update1_FULLVER_
// Fixed in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy   (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy   0
#endif

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_shared_ptr_atomic_load_missing_Buggy
*/
#ifndef qCompilerAndStdLib_shared_ptr_atomic_load_missing_Buggy

#if     defined (__GNUC__)
#define qCompilerAndStdLib_shared_ptr_atomic_load_missing_Buggy     (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9)))
#else
#define qCompilerAndStdLib_shared_ptr_atomic_load_missing_Buggy     0
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

#if     defined (__GNUC__)
#define qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy     (__GNUC__ < 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy     0
#endif

#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
@DESCRIPTION:   <p>Defined true if the compiler generates errors for static assertions in functions
            which should never be expanded. Such functions/static_assertions CAN be handy to make it
            more obvious of type mismatches with As<> etc templates.</p>
*/
#ifndef qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       ((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 6)))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       (__GNUC__ < 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       0
#endif

#endif








/*
Compiling regtests for Median/OrderBy...
>; _Compare = std::function<bool(const int&, const int&)>]�
../../..//Library/Sources/Stroika/Foundation/Characters/../Containers/../Traversal/Iterable.inl:538:22:   required from �RESULT_TYPE Stroika::Foundation::Traversal::Iterable<T>::Median(const std::function<bool(T, T)>&) const [with RESULT_TYPE = int; T = int]�
../../../Tests/46/Test.cpp:781:13:   required from here
/usr/include/c++/5/bits/predefined_ops.h:123:31: error: invalid type argument of unary �*� (have �int�)
*/
#ifndef qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy

#if     defined (__GNUC__)
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy       ((__GNUC__ == 5 && (__GNUC_MINOR__ == 1)) and __cplusplus == kStrokia_Foundation_Configuration_cplusplus_14)
#else
#define qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy       0
#endif

#endif














/*
@CONFIGVAR:     qCompilerAndStdLib_regex_Buggy
@DESCRIPTION:   <p>Defined true if the compiler supports regex_replace</p>
*/
#ifndef qCompilerAndStdLib_regex_Buggy

#if     defined (__clang__)
// Dont know how to test for libstdc++ version, but that seems the problem. For example, though fixed with gcc 49, clang-34 not
// compatible with that fix...
#define qCompilerAndStdLib_regex_Buggy       ((__clang_major__ == 3) && (__clang_minor__ <= 6))
#elif   defined (__GNUC__)
// Note - Verified FIXED in gcc 4.9, so it was gcc / libstdc++ bug
// @todo - this seems broken with gcc 4.8 so I'm pretty sure its not a gcc bug. Debug more carefully!!!
// Empirically seems to not work with gcc47, and I saw lots of stuff on internet to suggest not.
#define qCompilerAndStdLib_regex_Buggy       (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8))
#else
#define qCompilerAndStdLib_regex_Buggy       0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_inet_ntop_const_Buggy
    MSFT API bug - second arg to inet_ntop SB const 0 http://pubs.opengroup.org/onlinepubs/009695399/functions/inet_ntop.html
*/
#ifndef qCompilerAndStdLib_inet_ntop_const_Buggy

#if     defined (_MSC_VER)
// Still broken in _MS_VS_2k13_Update4_FULLVER_
// Still broken in _MS_VS_2k15_RTM_FULLVER_
// Still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_inet_ntop_const_Buggy          (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_inet_ntop_const_Buggy          0
#endif

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy
@DESCRIPTION:
    BUILDING REGTESTS:
22>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\discreterange.h(77): error C2975: 'MIN' : invalid template argument for 'Stroika::Foundation::Traversal::RangeTraits::ExplicitDiscreteRangeTraits', expected compile-time constant expression
22>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\discreterange.h(53) : see declaration of 'MIN'
22>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\discreterange.h(123) : see reference to class template instantiation 'Stroika::Foundation::Traversal::RangeTraits::DefaultDiscreteRangeTraits_Integral<T>' being compiled
22>          with
22>          [
22>              T=int
22>          ]
22>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\discreterange.h(141) : see reference to class template instantiation 'Stroika::Foundation::Traversal::RangeTraits::DefaultDiscreteRangeTraits<T>' being compiled
*/
#ifndef qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy

#if     defined (_MSC_VER)
// Still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy          qCompilerAndStdLib_constexpr_Buggy
#else
#define qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy          0
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

#if   defined (_MSC_VER)
// Still broken in _MS_VS_2k13_Update4_FULLVER_
// Still broken in _MS_VS_2k15_RC_FULLVER_
// Still broken in _MS_VS_2k15_RTM_FULLVER_
// Still broken in _MS_VS_2k15_Update1_FULLVER_
#define qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy          (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy          0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_noexcept_Buggy
@DESCRIPTION:   Defined 0 if the compiler supports noexcept
*/
#ifndef qCompilerAndStdLib_noexcept_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_noexcept_Buggy       (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_noexcept_Buggy       0
#endif

#endif






/**
 *  This works, and as near as I can tell, its just a compiler 'misfeature' with msvc that this is slow.
 *
 *  Tell that this is slow by running/comparing performance regtests with and without.
 */
#ifndef qCompilerAndStdLib_make_unique_lock_IsSlow

#if     defined (_MSC_VER)
// ASSUME still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_make_unique_lock_IsSlow      (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_make_unique_lock_IsSlow      0
#endif

#endif /*qCompilerAndStdLib_make_unique_lock_IsSlow*/









//@CONFIGVAR:     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
// Not sure if this is a compiler bug/template lib bug, or just an adnvantage of the stroika SharedPtr
// verus std::shared_ptr<> stuff (now I'm getting rid of) over the std::shared_ptr<>
//
// At least two obvious (but not great) workarounds.
//      o   make the class public
//      o   use custom deleter
//
// Also - usually (always) assigning ptr to temporary variable of the right type is usually
// a workaround (for case when you have private REP of a an accessible base type, and its
// the temporary shared_ptr<> constructed for the exact private type (which will they get CTOR copied to
// other class)
//
// maybe using FRIENDS will work?
//
// Anyhow - based on experimentationw with gcc, and 'common sense' - this should be defined to work - I will
// assume its either a bug with the VS implementation of the compiler or the template itself.
//
// Example compiler output for error from vc++12 (so if you see this sort of thing - apply this ifdef/fix):
//      1>c:\program files (x86)\microsoft visual studio 11.0\vc\include\memory(713): error C2248: 'Stroika::Foundation::Streams::BasicBinaryOutputStream::IRep_' : cannot access private class declared in class 'Stroika::Foundation::Streams::BasicBinaryOutputStream'
//      1>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\streams\basicbinaryoutputstream.cpp(22) : see declaration of 'Stroika::Foundation::Streams::BasicBinaryOutputStream::IRep_'
//      1>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\streams\basicbinaryoutputstream.h(47) : see declaration of 'Stroika::Foundation::Streams::BasicBinaryOutputStream'
//      1>          c:\program files (x86)\microsoft visual studio 11.0\vc\include\memory(487) : see reference to function template instantiation 'void std::shared_ptr<_Ty>::_Resetp<_Ux>(_Ux *)' being compiled
//      1>          with
//      1>          [
//      1>              _Ty=Stroika::Foundation::Streams::BinaryOutputStream::_IRep,
//      1>              _Ux=Stroika::Foundation::Streams::BasicBinaryOutputStream::IRep_
//      1>          ]
//      1>          c:\program files (x86)\microsoft visual studio 11.0\vc\include\memory(487) : see reference to function template instantiation 'void std::shared_ptr<_Ty>::_Resetp<_Ux>(_Ux *)' being compiled
//      1>          with
//      1>          [
//      1>              _Ty=Stroika::Foundation::Streams::BinaryOutputStream::_IRep,
//      1>              _Ux=Stroika::Foundation::Streams::BasicBinaryOutputStream::IRep_
//      1>          ]
//      1>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\streams\basicbinaryoutputstream.cpp(139) : see reference to function template instantiation
//
//
#ifndef qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
// still broken in _MS_VS_2k15_RC_FULLVER_
// FIXED in _MS_VS_2k15_RTM_FULLVER_
#define qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy        (_MSC_FULL_VER <= _MS_VS_2k15_RC_FULLVER_)
#else
#define qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy        0
#endif

#endif






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
 *
 */
#ifndef qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy     (__clang_major__ == 3 && (__clang_minor__ <= 6))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy     (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9))
#else
#define qCompilerAndStdLib_SFINAEWithStdPairOpLess_Buggy     0
#endif

#endif





#ifndef qCompilerAndStdLib_StaticCastInvokesExplicitConversionOperator_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k15_RC_FULLVER_
// appears fixed in _MS_VS_2k15_RTM_FULLVER_
#define qCompilerAndStdLib_StaticCastInvokesExplicitConversionOperator_Buggy        (_MSC_FULL_VER <= _MS_VS_2k15_RC_FULLVER_)
#else
#define qCompilerAndStdLib_StaticCastInvokesExplicitConversionOperator_Buggy        0
#endif

#endif






/*
 *EXAMPLE:
48>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\concrete\bijection_linkedlist.inl(213): error C2039: 'Equals' : is not a member of 'Stroika::Foundation::Common::DefaultEqualsComparer<`anonymous-namespace'::Test_JSONReadWriteFile_::ScanKindType,_Ty2>'
48>          with
48>          [
48>              _Ty2=std::shared_ptr<int>
48>          ]
48>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\concrete\bijection_linkedlist.inl(210) : while compiling class template member function 'bool Stroika::Foundation::Containers::Concrete::Bijection_LinkedList<DOMAIN_TYPE,RANGE_TYPE,TRAITS>::Rep_::Lookup(`anonymous-namespace'::Test_JSONReadWriteFile_::ScanKindType,Stroika::Foundation::Memory::Optional<Stroika::Foundation::Characters::String,Stroika::Foundation::Memory::Optional_Traits_Inplace_Storage<T>> *) const'
48>          with
48>          [
48>              DOMAIN_TYPE=`anonymous-namespace'::Test_JSONReadWriteFile_::ScanKindType
48>  ,            RANGE_TYPE=Stroika::Foundation::Characters::String
*/
#ifndef qCompilerAndStdLib_hasEqualDoesntMatchStrongEnums_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_hasEqualDoesntMatchStrongEnums_Buggy        (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_hasEqualDoesntMatchStrongEnums_Buggy        0
#endif

#endif






//
// Without .template in one place gcc48 barfs, and with it vs 2k.net 2015 barfs. As near as I can tell
// gcc is right, but I'm uncertain.
//
//  -- LGP 2015-05-18
/*
47>c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(541): error C2143: syntax error: missing ';' before '&'
47>  c:\sandbox\stroikadev\tests\46\test.cpp(1312): note: see reference to function template instantiation 'Stroika::Foundation::DataExchange::ObjectVariantMapper::TypeMappingDetails Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer_ContainerWithStringishKey<`anonymous-namespace'::Test_JSONReadWriteFile_::SpectrumType,double,double>(void)' being compiled
47>c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(541): error C2461: 'Stroika::Foundation::DataExchange::ObjectVariantMapper::MakeCommonSerializer_ContainerWithStringishKey::<lambda_8ffbcf425fc80a60fcc9f2e3e4588588>': constructor syntax missing formal parameters
47>c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(541): error C2238: unexpected token(s) preceding ';'
47>c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(541): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int
47>c:\sandbox\stroikadev\library\sources\stroika\foundation\dataexchange\objectvariantmapper.inl(541): error C2143: syntax error: missing ',' before '&'
*/
#ifndef qCompilerAndStdLib_DotTemplateDisambiguator_Buggy

#if     defined (_MSC_VER)
// first broken in _MS_VS_2k15_RC_FULLVER_
#define qCompilerAndStdLib_DotTemplateDisambiguator_Buggy        (_MSC_FULL_VER == _MS_VS_2k15_RC_FULLVER_ || _MSC_FULL_VER == _MS_VS_2k15_RTM_FULLVER_)
#else
#define qCompilerAndStdLib_DotTemplateDisambiguator_Buggy        0
#endif

#endif







#ifndef qCompilerAndStdLib_TemplateDiffersOnReturnTypeOnly_Buggy

#if     defined (__GNUC__)
#define qCompilerAndStdLib_TemplateDiffersOnReturnTypeOnly_Buggy        (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8))
#else
#define qCompilerAndStdLib_TemplateDiffersOnReturnTypeOnly_Buggy        0
#endif

#endif





#ifndef qCompilerAndStdLib_stdContainerEraseConstArgSupport_Buggy

#if     defined (__GNUC__)
#define qCompilerAndStdLib_stdContainerEraseConstArgSupport_Buggy       (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8))
#else
#define qCompilerAndStdLib_stdContainerEraseConstArgSupport_Buggy       0
#endif

#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy
@DESCRIPTION:
    error: no matching constructor for initialization of 'std::codecvt_byname<wchar_t, char, mbstate_t>'
        deletable_facet_ (Args&& ...args) : FACET (std::forward<Args> (args)...) {}
*/
#ifndef qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy     (__clang_major__ == 3 && (__clang_minor__ <= 6))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy     (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9))
#else
#define qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy     0
#endif

#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_GCC_48_OptimizerBug
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_GCC_48_OptimizerBug

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_GCC_48_OptimizerBug     (__GNUC__ == 4 && (__GNUC_MINOR__ == 8))
#else
#define qCompilerAndStdLib_GCC_48_OptimizerBug     0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy     (__clang_major__ == 3 && (__clang_minor__ <= 6))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy     (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9))
#else
#define qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy     0
#endif

#endif






/*
     thread_local AbortFlagType_             s_Aborting_ (false);
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\execution\thread.cpp(88): error C2146: syntax error : missing ';' before identifier 'AbortFlagType_'
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\execution\thread.cpp(88): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\execution\thread.cpp(89): error C2146: syntax error : missing ';' before identifier 'AbortSuppressCountType_'
c:\sandbox\stroika\devroot\library\sources\stroika\foundation\execution\thread.cpp(89): error C4430: missing type specifier - int assumed. Note: C++ does not support default-int
*/
#ifndef qCompilerAndStdLib_thread_local_keyword_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_thread_local_keyword_Buggy       (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_thread_local_keyword_Buggy       0
#endif

#endif






#ifndef qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy   (__clang_major__ == 3 && (__clang_minor__ <= 5))
#else
#define qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy   qCompilerAndStdLib_thread_local_keyword_Buggy
#endif

#endif




/*
 *
 *      class   Seekable {
 *          protected:
 *              class   _IRep;
 *      ...
 *
 *      struct   InStr_IRep_ : BinaryInputStream::_IRep, Seekable::_IRep {
 */
#ifndef qCompilerAndStdLib_two_levels_nesting_Protected_Access_Buggy

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_two_levels_nesting_Protected_Access_Buggy        (__GNUC__ == 4 && (__GNUC_MINOR__ == 9))
#else
#define qCompilerAndStdLib_two_levels_nesting_Protected_Access_Buggy        0
#endif

#endif



// @todo - investigate this better
/*

// NO LONGER CRASHES as of _MS_VS_2k13_Update2_FULLVER_ - investigate error message. Could be my bug?
// -- LGP 2014-05-25

19>c:\sandbox\stroika\devroot\tests\testcommon\commontests_set.h(159): error C2248: 'Stroika::Foundation::Containers::SortedSet<Stroika::SimpleClassWithoutComparisonOperators,SimpleClassWithoutComparisonOperators_SETTRAITS>::SortedSet'
                    : cannot access protected member declared in class 'Stroika::Foundation::Containers::SortedSet<Stroika::SimpleClassWithoutComparisonOperators,SimpleClassWithoutComparisonOperators_SETTRAITS>'
19>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\sortedset.h(110) : see declaration of 'Stroika::Foundation::Containers::SortedSet<Stroika::SimpleClassWithoutComparisonOperators,SimpleClassWithoutComparisonOperators_SETTRAITS>::SortedSet'
19>          c:\sandbox\stroika\devroot\tests\testcommon\commontests_set.h(182) : see reference to function template instantiation 'void CommonTests::SetTests::Test5_UnionDifferenceIntersectionEtc_::DoAllTests_<USING_SET_CONTAINER,USING_BASESET_CONTAINER,TEST_FUNCTION>(TEST_FUNCTION)' being compiled
19>          with
19>          [
19>              USING_SET_CONTAINER=Stroika::Foundation::Containers::SortedSet<Stroika::SimpleClassWithoutComparisonOperators,SimpleClassWithoutComparisonOperators_SETTRAITS>
19>  ,            USING_BASESET_CONTAINER=Stroika::Foundation::Containers::SortedSet<Stroika::SimpleClassWithoutComparisonOperators,SimpleClassWithoutComparisonOperators_SETTRAITS>
19>  ,            TEST_FUNCTION=`anonymous-namespace'::RunTests_::<lambda_9202cc4e8657f58007ab10c1a185f58f>
19>          ]
*/
#ifndef qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug   (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug   0
#endif

#endif




// Doesnt crash but values appear as NANs - but not always ;-(
// See output in RegTest44
#ifndef qCompilerAndStdLib_stdinitializer_of_double_in_ranged_for_Bug

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_stdinitializer_of_double_in_ranged_for_Bug   (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_stdinitializer_of_double_in_ranged_for_Bug   0
#endif

#endif





/**
 *  qCompilerAndStdLib_Supports_errno_t controls whether or not the compiler provides an implementation
 *  of errno_t (gcc for example just assumes its int).
 *
 *  NB: POSIX and C99 just say to assume its an int and doesn't define errno_t.
 */
#if     !defined (qCompilerAndStdLib_Supports_errno_t)
#if     defined (__clang__)
#define qCompilerAndStdLib_Supports_errno_t 0
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_Supports_errno_t 0
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_errno_t 1
#else
#define qCompilerAndStdLib_Supports_errno_t 1
#endif
#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy
@DESCRIPTION:   Not sure if this is a gcc compiler bug or not - but seems wierd

                GCC 4.6 requires this above extra namesapce stuff. Not sure reasonable or bug? Investigate before creating bug workaround define
                 -- LGP 2012-05-26
*/
#if     !defined (qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy)

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy      (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8))
#else
#define qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy      0
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
#if     !defined (qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug)

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug      ((__GNUC__ < 5) || (__GNUC__ == 5 && (__GNUC_MINOR__ <= 2)))
#else
#define qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug      0
#endif

#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy
@DESCRIPTION:   See also qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy

#if     qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy
                static  SHARED_IMLP  DefaultElementCopier_ (const T& t);
                SharedByValue_CopyByFunction (SHARED_IMLP (*copier) (const T&) = DefaultElementCopier_) noexcept;
#else
                SharedByValue_CopyByFunction (SHARED_IMLP (*copier) (const T&) = [](const T& t) -> SHARED_IMLP  { return SHARED_IMLP (new T (t)); }) noexcept;
#endif

1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\sharedbyvalue.h(78): error C2226: syntax error : unexpected type 'SHARED_IMLP'
1>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\sharedbyvalue.h(82) : see reference to class template instantiation 'Stroika::Foundation::Memory::SharedByValue_CopyByFunction<T,SHARED_IMLP>' being compiled
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\memory\sharedbyvalue.h(78): error C2143: syntax error : missing ')' before '{'

*/
#ifndef qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy   (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy   0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
@DESCRIPTION:   See also qCompilerAndStdLib_Supports_lambda_default_argument_with_template_param_as_function_cast

        nonvirtual  String  LTrim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;

1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\characters\string.h(750): error C2062: type 'bool' unexpected
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\characters\string.h(750): error C2143: syntax error : missing ')' before '{'
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\characters\string.h(750): error C2059: syntax error : ')'
*/
#ifndef qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy    (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy    0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Support__PRETTY_FUNCTION__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__PRETTY_FUNCTION__

#if     defined (__GNUC__)
#define qCompilerAndStdLib_Support__PRETTY_FUNCTION__   1
#else
#define qCompilerAndStdLib_Support__PRETTY_FUNCTION__   0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Support__func__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__func__

#if     defined (__GNUC__)
#define qCompilerAndStdLib_Support__func__   1
#else
#define qCompilerAndStdLib_Support__func__   0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Support__FUNCTION__
@DESCRIPTION:   <p>FOR ASSERT</p>
*/
#ifndef qCompilerAndStdLib_Support__FUNCTION__

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Support__FUNCTION__   1
#else
#define qCompilerAndStdLib_Support__FUNCTION__   0
#endif

#endif










/*
@CONFIGVAR:     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
@DESCRIPTION:
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  String& operator= (String && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  String& operator= (String && newString) = default;
#endif
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\characters\string.h(396): error C2610: 'Stroika::Foundation::Characters::String &Stroika::Foundation::Characters::String::operator =(Stroika::Foundation::Characters::String &&)' : is not a special member function which can be defaulted
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\characters\string.inl(226): error C2264: 'Stroika::Foundation::Characters::String::operator =' : error in function definition or declaration; function not called
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\characters\string.inl(269): error C2264: 'Stroika::Foundation::Characters::String::operator =' : error in function definition or declaration; function not called
*/
#ifndef qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update5_FULLVER_
#define qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy        (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy        0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy
@DESCRIPTION:
            template    <typename   T>
            T   PinToSpecialPoint (T p, T special, T epsilon = (10000 * numeric_limits<T>::epsilon ()));
1>  ProgressMonitor.cpp
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\math\common.h(89): error C2064: term does not evaluate to a function taking 0 arguments


    ///// VERIFY WE CAN DELETE THIS DEFINE AS OF 2014-08-08
*/
#ifndef qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
#define qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy      qCompilerAndStdLib_constexpr_Buggy
#else
#define qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy      0
#endif

#endif




/*
@CONFIGVAR:     qDecoratedNameLengthExceeded_Buggy
@DESCRIPTION:
        C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include\xrefwrap(173): warning C4503: 'Stroik...
        ...: decorated name length exceeded, name was truncated []
*/
#ifndef qDecoratedNameLengthExceeded_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
// still broken in _MS_VS_2k15_RC_FULLVER_
// still broken in _MS_VS_2k15_RTM_FULLVER_
// still borken in _MS_VS_2k15_Update1_FULLVER_
#define qDecoratedNameLengthExceeded_Buggy      (_MSC_FULL_VER <= _MS_VS_2k15_Update1_FULLVER_)
#else
#define qDecoratedNameLengthExceeded_Buggy      0
#endif

#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_deprecatedFeatureMissing
@DESCRIPTION:
    To test if fixed, set to zero and stroika lib will fail to compile.
*/
#ifndef qCompilerAndStdLib_deprecatedFeatureMissing

#if     defined (__clang__)
#define qCompilerAndStdLib_deprecatedFeatureMissing             ((__clang_major__ == 3) && (__clang_minor__ <= 5))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_deprecatedFeatureMissing             (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9))
#elif   defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update4_FULLVER_
#define qCompilerAndStdLib_deprecatedFeatureMissing             (_MSC_FULL_VER <= _MS_VS_2k13_Update5_FULLVER_)
#else
#define qCompilerAndStdLib_deprecatedFeatureMissing              0
#endif

#endif



/*
@CONFIGVAR:     qCompilerAndStdLib_string_conversions_Buggy
@DESCRIPTION:   22.3.3.2.2  string conversions  N
*/
#ifndef qCompilerAndStdLib_string_conversions_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_string_conversions_Buggy              ((__clang_major__ == 3) && (__clang_minor__ <= 6))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_string_conversions_Buggy              (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9))
#else
#define qCompilerAndStdLib_string_conversions_Buggy              0
#endif

#endif












/*
 *******************************************************************
 *******************************************************************
 ********************* REACT TO SOME DEFINES ***********************
 *******************************************************************
 *******************************************************************
 */

#if     qSilenceAnnoyingCompilerWarnings && defined(__GNUC__) && !defined(__clang__)
// Note - I tried tricks with token pasting, but only seems to work if I do all token pasting
// and that fails with 'astyle' which breaks up a-b tokens. Need quotes to work with astyle
// and no way I can find to concatenate strings that works with _Pragma
//  --LGP 2014-01-05
#define DISABLE_COMPILER_GCC_WARNING_START(WARNING_TO_DISABLE)\
    _Pragma ( "GCC diagnostic push") \
    _Pragma ( WARNING_TO_DISABLE )
#define DISABLE_COMPILER_GCC_WARNING_END(WARNING_TO_DISABLE)\
    _Pragma ( "GCC diagnostic pop" )
#else
#define DISABLE_COMPILER_GCC_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_GCC_WARNING_END(WARNING_TO_DISABLE)
#endif





#if     qSilenceAnnoyingCompilerWarnings && defined(_MSC_VER)
#define DISABLE_COMPILER_MSC_WARNING_START(WARNING_TO_DISABLE)\
    __pragma ( warning (push) ) \
    __pragma ( warning (disable : WARNING_TO_DISABLE) )
#define DISABLE_COMPILER_MSC_WARNING_END(WARNING_TO_DISABLE)\
    __pragma ( warning (pop) )
#else
#define DISABLE_COMPILER_MSC_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_MSC_WARNING_END(WARNING_TO_DISABLE)
#endif



#if     qSilenceAnnoyingCompilerWarnings && defined(__clang__)
// Note - I tried tricks with token pasting, but only seems to work if I do all token pasting
// and that fails with 'astyle' which breaks up a-b tokens. Need quotes to work with astyle
// and no way I can find to concatenate strings that works with _Pragma
//  --LGP 2014-01-05
#define DISABLE_COMPILER_CLANG_WARNING_START(WARNING_TO_DISABLE)\
    _Pragma ( "clang diagnostic push") \
    _Pragma ( WARNING_TO_DISABLE )
#define DISABLE_COMPILER_CLANG_WARNING_END(WARNING_TO_DISABLE)\
    _Pragma ( "clang diagnostic pop" )
#else
#define DISABLE_COMPILER_CLANG_WARNING_START(WARNING_TO_DISABLE)
#define DISABLE_COMPILER_CLANG_WARNING_END(WARNING_TO_DISABLE)
#endif






// MSFT has a hack that prevents workarounds to other features they don't support, but luckily, they may that hack
// easy enough to disable ;-)
#if     defined (_MSC_VER) && (_MSC_VER == _MS_VS_2k13_VER_ || _MSC_VER == _MS_VS_2k15_VER_)
#define _ALLOW_KEYWORD_MACROS
#endif





#if     qCompilerAndStdLib_noexcept_Buggy
#define noexcept    throw ()
#endif





#if     qCompilerAndStdLib_constexpr_Buggy
// This works as an adequate workaround pretty often
#define constexpr   const
#endif





// doesnt seem any portable way todo this, and not defined in C++ language
#if     defined (__GNUC__)
#define dont_inline    __attribute__((noinline))
#else
#define dont_inline    __declspec(noinline)
#endif







#if     qCompilerAndStdLib_thread_local_keyword_Buggy
#if     defined (__GNUC__)
#define thread_local    __thread
#else
#define thread_local    __declspec(thread)
#endif
#endif





/**
 *   The Standard C++ mechanism of commenting out unused parameters isn't good enuf
 *   in the case where the parameters might be used conditionally. This hack is
 *   to shutup compiler warnings in those cases.
 */
#if     !defined (Arg_Unused)
#define Arg_Unused(x)   ((void) &x)
#endif





/*
 *   Sometimes its handy to mark a function as not actually returning (because of throws or other reasons)
 *   This can allow the compiler to occasionally better optimize, but mostly avoid spurious warnings.
 *
 *  From http://msdn.microsoft.com/en-us/library/k6ktzx3s(v=vs.80).aspx
 *      This __declspec attribute tells the compiler that a function does not return. As a consequence,
 *      the compiler knows that the code following a call to a __declspec(noreturn) function is unreachable.
 *
 *      If the compiler finds a function with a control path that does not return a value,
 *      it generates a warning (C4715) or error message (C2202). If the control path cannot
 *      be reached due to a function that never returns, you can use __declspec(noreturn) to prevent
 *      this warning or error.
 */
#if     !defined (_NoReturn_)
#if     defined(_MSC_VER)
#define _NoReturn_  __declspec(noreturn)
#elif   defined (__GNUG__ )
#define _NoReturn_  __attribute__((noreturn))
#else
#define _NoReturn_
#endif
#endif





/*
 *   Sometimes its handy to mark a line of code as a no-op - so its arguments are not executed (as with
 * trace macros).
 */
#if     !defined (_NoOp_)
#if defined(_MSC_VER)
#define _NoOp_  __noop
#else
#define _NoOp_(...)
#endif
#endif





/*
 *  USE _Deprecated_ INSTEAD
 */
#if     !defined (_DeprecatedFunction_)
#if     qCompilerAndStdLib_deprecatedFeatureMissing && defined (__GNUC__)
#define _DeprecatedFunction_(func,MESSAGE) func __attribute__ ((deprecated))
#elif   qCompilerAndStdLib_deprecatedFeatureMissing && defined(_MSC_VER)
#define _DeprecatedFunction_(func,MESSAGE) __declspec(deprecated) func
#else
#define _DeprecatedFunction_(func,MESSAGE) [[deprecated(MESSAGE)]] func
#endif
#endif
#if     !defined (_DeprecatedFunction2_)
#if     qCompilerAndStdLib_deprecatedFeatureMissing && defined(_MSC_VER)
#define _DeprecatedFunction2_(func,MESSAGE) __declspec(deprecated) func
#elif   __cplusplus >= kStrokia_Foundation_Configuration_cplusplus_14
#define _DeprecatedFunction2_(func,MESSAGE) [[deprecated(MESSAGE)]] func
#else
#define _DeprecatedFunction2_(func,MESSAGE) func
#endif
#endif




/*
 *  This doesnt always work, but works for c++14 or later, and VS
 *  EXAMPLE:
 *
 *      struct _Deprecated_("FRED NOW DEPRECATED - USE BARNY") Fred { int a; };
 *
 *      _Deprecated_ ("USE CheckAccess")
 *      void     CheckFileAccess(int);
 */
#if     !defined (_Deprecated_)
#if     __cplusplus >= kStrokia_Foundation_Configuration_cplusplus_14
#define _Deprecated_(MESSAGE) [[deprecated(MESSAGE)]]
#else
#define _Deprecated_(MESSAGE)
#endif
#endif



/*
 *  USE _Deprecated_ INSTEAD
 */
#if     !defined (_DeprecatedClass_)
#if     qCompilerAndStdLib_deprecatedFeatureMissing && defined (__GNUC__)
// this doesnt work for gcc pre gcc49 easily, so dont bother - 49 supports real [[deprecated]]
#define _DeprecatedClass_(func,MESSAGE) func
#elif   qCompilerAndStdLib_deprecatedFeatureMissing && defined(_MSC_VER)
#define _DeprecatedClass_(func,MESSAGE) __declspec(deprecated) func
#else
#define _DeprecatedClass_(func,MESSAGE) [[deprecated(MESSAGE)]] func
#endif
#endif



/*
 *  Wrap this macro around entire declaration, as in:
 *       _DeprecatedFile_ ("DEPRECATED in v2.0a32 - use IO::FileSystem::DirectoryIterator");
 */
#define STRINGIFY(a) #a
#if     !defined (_DeprecatedFile_)
#define _DeprecatedFile_(MESSAGE) \
    _Pragma (STRINGIFY (message ##  DEPRECATED  ## MESSAGE ))
#endif






/*
 *  NB: we can lose these macros once all our compilers support the new C++ syntax.
 *
 *  Also note - the bug workaround - enum  { NAME = static_cast<TYPE> (VALUE) } - doesn't
 *  work pefrectly - since it doesnt create a name of the desired type, but its usually
 *  pretty close.
 */
#if     qCompilerAndStdLib_constexpr_Buggy
#define DEFINE_CONSTEXPR_CONSTANT(TYPE,NAME,VALUE)\
    enum  { NAME = static_cast<TYPE> (VALUE) };
#else
#define DEFINE_CONSTEXPR_CONSTANT(TYPE,NAME,VALUE)\
    static  constexpr TYPE NAME = VALUE;
#endif






#if     qCompilerAndStdLib_shared_mutex_module_Buggy
namespace std {
    template    <typename MUTEX>
    class   shared_lock {
    public:
        using mutex_type = MUTEX;
    private:
        mutex_type& fMutex_;
    public:
        explicit shared_lock (mutex_type& m)
            : fMutex_ (m)
        {
            fMutex_.lock_shared ();
        }
        ~shared_lock ()
        {
            fMutex_.unlock_shared ();
        }
    };
}
#endif






#if     qSilenceAnnoyingCompilerWarnings && defined(_MSC_VER) && qDecoratedNameLengthExceeded_Buggy
__pragma ( warning (disable : 4503) )
#endif



#endif  /*defined(__cplusplus)*/



#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

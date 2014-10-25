/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_ 1


/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */


#if     defined(__cplusplus)





/*
 *******************************************************************
 *******************************************************************
 ******** CHECK / WARN about supported compilers/versions **********
 *******************************************************************
 *******************************************************************
 */

#if     defined (__clang__)

// Must check CLANG first, since CLANG also defines GCC
#if     (__clang_major__ < 3) || (__clang_major__ == 3 && (__clang_minor__ < 4))
#pragma message ("Warning: Stroika does not support versions prior to clang++ 3.4")
#endif
#if     (__clang_major__ > 3) || (__clang_major__ == 3 && (__clang_minor__ >= 6))
#pragma message ("Info: Stroika untested with this version of clang++")
#endif

#elif   defined (__GNUC__)

#if     __GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ < 7))
#pragma message ("Warning: Stroika does not support versions prior to GCC 4.7")
#endif
#if     __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 9))
#pragma message ("Info: Stroika untested with this version of GCC")
#endif

#elif     defined (_MSC_VER)

#define _MS_VS_2k13_VER_                1800
// _MSC_FULL_VER value - see http://msdn.microsoft.com/en-us/library/b0084kay.aspx - so far unused
#define _MS_VS_2k13_FULLVER_            180021005
#define _MS_VS_2k13_Update1_FULLVER_    180021005
#define _MS_VS_2k13_Update2_FULLVER_    180030501
#define _MS_VS_2k13_Update3_FULLVER_    180030723

#if      _MSC_VER < _MS_VS_2k13_VER_
#pragma message ("Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2013")
#endif
#if      _MSC_VER > _MS_VS_2k13_VER_
#pragma message ("Info: This version of Stroika is untested with this version of Microsoft Visual Studio.net / Visual C++")
#elif    _MSC_FULL_VER > _MS_VS_2k13_Update3_FULLVER_
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
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_LocaleTM_put_Buggy     (_MSC_FULL_VER < _MS_VS_2k13_Update2_FULLVER_)
#else
#define qCompilerAndStdLib_LocaleTM_put_Buggy     0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy
@DESCRIPTION:   <p>Using tmput on Release, and 64bit builds core dumps. Just started. Not sure why.
            But using narrow string succeeds so workaround.
            </p>
*/
#ifndef qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_LocaleTM_time_put_crash_sometimes_Buggy     (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
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
*/
#ifndef qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy

#if   defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy     (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy     0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy
@DESCRIPTION:

    >>> TO TEST IF FIXED - RUN TIME REGRESSION TESTS (Foundation::Time - currently #44) <<<
*/
#ifndef qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy    (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy    0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_VarDateFromStrOnFirstTry
@DESCRIPTION:   See also qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy
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
@CONFIGVAR:     qCompilerAndStdLib_constexpr_Buggy
@DESCRIPTION:   NYI for VS2k13, but expected soon
*/
#ifndef qCompilerAndStdLib_constexpr_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_constexpr_Buggy      (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qCompilerAndStdLib_constexpr_Buggy      0
#endif

#endif



/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy
@DESCRIPTION:   http://stackoverflow.com/questions/24342455/nested-static-constexpr-of-incomplete-type-valid-c-or-not
*/
#ifndef qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy

#if     qCompilerAndStdLib_constexpr_Buggy
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      1
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9)))
#else
#define qCompilerAndStdLib_constexpr_const_then_constexpr_Buggy      0
#endif

#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy

    The issue is really
            warning: use of this statement in a constexpr function is a C++1y
    for require lines at the start of constexp functions
*/
#ifndef qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    (__cplusplus <= 201103)
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    (__cplusplus <= 201103)
#else
#define qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy    qCompilerAndStdLib_constexpr_Buggy
#endif

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
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy   (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qCompilerAndStdLib_atomic_flag_atomic_flag_init_Buggy   0
#endif

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_threadYield_Buggy
*/
#ifndef qCompilerAndStdLib_threadYield_Buggy

#if     defined (__GNUC__)
#define qCompilerAndStdLib_threadYield_Buggy       (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 7)))
#else
#define qCompilerAndStdLib_threadYield_Buggy   0
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
*/
#ifndef qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy

#if     defined (__GNUC__)
#define qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy     (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8)))
#else
#define qCompilerAndStdLib_complex_templated_use_of_nested_enum_Buggy     0
#endif

#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy
@DESCRIPTION:   Bug with Microsoft VS 2k13.net C++ compiler where first use of floating point
*               numbers produces nan/#IND... Workraround is to just do bogus early use.
*
*               See also qCompilerAndStdLib_Supports_VarDateFromStrOnFirstTry
*
*               MSFT analyzed this. Its
*                   https://connect.microsoft.com/VisualStudio/feedback/details/808199/visual-c-2013-floating-point-broken-on-first-use
*
*                   There's a bug in a CRT function (dtoui3) which is used to convert from double to unsigned,
*                   it "leaks" a x87 stack register and ends up causing a stack overflow. That's how the fld1
*                   instruction used by foo ends up producing an IND.
*
*/
#ifndef qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy   (_MSC_FULL_VER < _MS_VS_2k13_Update2_FULLVER_)
#else
#define qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy   0
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
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       ((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 5)))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 9)))
#else
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       0
#endif

#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_UsingInheritedConstructor_Buggy
@DESCRIPTION:
EXAMPLE:
        struct UnAssignedableCharacter : Character {
#if     qCompilerAndStdLib_UsingInheritedConstructor_Buggy
            UnAssignedableCharacter (const Character&c) : Character (c) {}
#else
            using Character::Character;
#endif

            operator Character () const
            {
                return *this;
            }

            UnAssignedableCharacter& operator= (const UnAssignedableCharacter&) = delete;
        };
*/
#ifndef qCompilerAndStdLib_UsingInheritedConstructor_Buggy

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_UsingInheritedConstructor_Buggy      (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 7)))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_UsingInheritedConstructor_Buggy      (_MSC_FULL_VER < _MS_VS_2k13_Update2_FULLVER_)
#else
#define qCompilerAndStdLib_UsingInheritedConstructor_Buggy      0
#endif

#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_regex_Buggy
@DESCRIPTION:   <p>Defined true if the compiler supports regex_replace</p>
*/
#ifndef qCompilerAndStdLib_regex_Buggy

#if     defined (__clang__)
// @todo - this seems broken with clang 3.4 - so probably MY bug - not gcc/clang bug... debug soon!
#define qCompilerAndStdLib_regex_Buggy       ((__clang_major__ == 3) && (__clang_minor__ < 4))
#elif   defined (__GNUC__)
// @todo - this seems broken with gcc 4.8 so I'm pretty sure its not a gcc bug. Debug more carefully!!!
// Empirically seems to not work with gcc47, and I saw lots of stuff on internet to suggest not.
#define qCompilerAndStdLib_regex_Buggy       (__GNUC__ == 4 && (__GNUC_MINOR__ < 9))
#else
#define qCompilerAndStdLib_regex_Buggy       0
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

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy          (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#elif   defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// Still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy          qCompilerAndStdLib_constexpr_Buggy
#else
#define qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy          0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_noexcept_Buggy
@DESCRIPTION:   Defined 0 if the compiler supports noexcept
*/
#ifndef qCompilerAndStdLib_noexcept_Buggy

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_noexcept_Buggy       (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
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
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_make_unique_lock_IsSlow      (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qCompilerAndStdLib_make_unique_lock_IsSlow      0
#endif

#endif /*qCompilerAndStdLib_make_unique_lock_IsSlow*/












/*
*/
#ifndef qCompilerAndStdLib_Template_Baseclass_WierdIterableBaseBug

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_Template_Baseclass_WierdIterableBaseBug    (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qCompilerAndStdLib_Template_Baseclass_WierdIterableBaseBug    0
#endif

#endif





/**
@CONFIGVAR:     qCompilerAndStdLib_templated_constructionInTemplateConstructors_Buggy
@DESCRIPTION:   Crazy workaround of bug with llvc 3.2. Not well understood.
*/
#ifndef qCompilerAndStdLib_templated_constructionInTemplateConstructors_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_templated_constructionInTemplateConstructors_Buggy       ((__clang_major__ == 3) && (__clang_minor__ < 5))
#else
#define qCompilerAndStdLib_templated_constructionInTemplateConstructors_Buggy        0
#endif

#endif





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
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy        (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy        0
#endif

#endif






#ifndef qCompilerAndStdLib_stdContainerEraseConstArgSupport_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_stdContainerEraseConstArgSupport_Buggy      ((__clang_major__ == 3) && (__clang_minor__ < 5))
#elif     defined (__GNUC__)
#define qCompilerAndStdLib_stdContainerEraseConstArgSupport_Buggy       (__GNUC__ == 4 && (__GNUC_MINOR__ < 9))
#else
#define qCompilerAndStdLib_stdContainerEraseConstArgSupport_Buggy       0
#endif

#endif



/*
@CONFIGVAR:     qCompilerAndStdLib_CompareStronglyTypedEnums_Buggy
@DESCRIPTION:
                Note - this compiles with clang 3.4, but fails to generate the
                correct code.
*/
#ifndef qCompilerAndStdLib_CompareStronglyTypedEnums_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_CompareStronglyTypedEnums_Buggy     (__clang_major__ == 3 && (__clang_minor__ < 5))
#else
#define qCompilerAndStdLib_CompareStronglyTypedEnums_Buggy     0
#endif

#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy     (__clang_major__ == 3 && (__clang_minor__ < 5))
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
#define qCompilerAndStdLib_GCC_48_OptimizerBug     (__GNUC__ == 4 && (__GNUC_MINOR__ == 7 or __GNUC_MINOR__ == 8))
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
#define qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy     (__clang_major__ == 3 && (__clang_minor__ < 6))
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
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_thread_local_keyword_Buggy       (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#elif     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_thread_local_keyword_Buggy       (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#else
#define qCompilerAndStdLib_thread_local_keyword_Buggy       0
#endif

#endif






#ifndef qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy   (__clang_major__ == 3 && (__clang_minor__ < 5))
#else
#define qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy   qCompilerAndStdLib_thread_local_keyword_Buggy
#endif

#endif




//BinaryInputOutputStream::operator BinaryIn
#ifndef qCompilerAndStdLib_two_levels_nesting_Protected_Access_Buggy

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy            (__GNUC__ == 4 && (__GNUC_MINOR__ == 9))
#else
#define qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy            0
#endif

#endif



// @todo - investigate this better
/*
    virtual typename Iterable<T>::_SharedPtrIRep Clone (IteratorOwnerID forIterableEnvelope) const override
    {
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
        return IterableOfTSharedPtrIRep (new MyIterableRep_ (*this));
#else
        return typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (*this));
#endif
    }
1>c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\multiset.inl(152): error C2248: 'Stroika::Foundation::Traversal::Iterable<T>::_SharedPtrIRep' : cannot access protected typedef declared in class 'Stroika::Foundation::Traversal::Iterable<T>'
1>          with
1>          [
1>              T=size_t
1>          ]
1>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\traversal\iterable.h(226) : see declaration of 'Stroika::Foundation::Traversal::Iterable<T>::_SharedPtrIRep'
1>          with
1>          [
1>              T=size_t
1>          ]
1>          c:\sandbox\stroika\devroot\library\sources\stroika\foundation\containers\multiset.inl(148) : while compiling class template member function 'Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Traversal::Iterable<T>::_IRep> Stroika::Foundation::Containers::MultiSet<T,Stroika::Foundation::Containers::MultiSet_DefaultTraits<T,Stroika::Foundation::Common::ComparerWithEquals<T>>>::_IRep::_ElementsIterableHelper::MyIterableRep_::Clone(Stroika::Foundation::Traversal::IteratorOwnerID) const'
*/
#ifndef qTemplateAccessCheckConfusionProtectedNeststingBug

#if     defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qTemplateAccessCheckConfusionProtectedNeststingBug   (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qTemplateAccessCheckConfusionProtectedNeststingBug   0
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
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug   (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
#else
#define qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug   0
#endif

#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy            (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#else
#define qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy            0
#endif

#endif





#ifndef qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug        (_MSC_FULL_VER < _MS_VS_2k13_Update2_FULLVER_)
#else
#define qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug        0
#endif

#endif





// Dangerous with newer compilers - defaulting this to on.
// Hard to detect if it doesn't work...
#ifndef qCompilerAndStdLib_thread_local_initializers_Buggy

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_thread_local_initializers_Buggy      (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#else
#define qCompilerAndStdLib_thread_local_initializers_Buggy      0
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
#define qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy     (__GNUC__ == 4 && (__GNUC_MINOR__ < 9))
#else
#define qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy       0
#endif

#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug
@DESCRIPTION:
*/
#if     !defined (qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug)

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug     (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8))
#else
#define qCompilerAndStdLib_IllUnderstoodSequenceCTORinitializerListBug       0
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

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy   (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#elif   defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy   (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
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

#if     defined (__clang__)
#define qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy    (__clang_major__ == 3 && (__clang_minor__ < 5))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy    (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#elif   defined (_MSC_VER)
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy    (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
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
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy        (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
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
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy      qCompilerAndStdLib_constexpr_Buggy
#else
#define qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy      0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_constexpr_StaticDataMember_Buggy

#if     defined (__clang__)
// Seems to compile with clang 3.4, but then caused link errors - unclear if my bug or gcc bug?
#define qCompilerAndStdLib_constexpr_StaticDataMember_Buggy      ((__clang_major__ == 3) && (__clang_minor__ < 5))
#elif     defined (__GNUC__)
// Seems to compile with gcc 4.7.2, but then caused link errors - unclear if my bug or gcc bug?
#define qCompilerAndStdLib_constexpr_StaticDataMember_Buggy       (__GNUC__ == 4 && (__GNUC_MINOR__ < 9))
#else
#define qCompilerAndStdLib_constexpr_StaticDataMember_Buggy       qCompilerAndStdLib_constexpr_Buggy
#endif

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy

#if     defined (__clang__)
// Seems to compile with clang 3.4, but then caused link errors - unclear if my bug or gcc bug?
#define qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy      ((__clang_major__ == 3) && (__clang_minor__ < 6))
#else
#define qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy       0
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
// still broken in _MS_VS_2k13_Update2_FULLVER_
// still broken in _MS_VS_2k13_Update3_FULLVER_
#define qCompilerAndStdLib_deprecatedFeatureMissing             (_MSC_FULL_VER <= _MS_VS_2k13_Update3_FULLVER_)
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
#define qCompilerAndStdLib_string_conversions_Buggy              ((__clang_major__ == 3) && (__clang_minor__ <= 5))
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
#if     defined (_MSC_VER) && (_MSC_VER == _MS_VS_2k13_VER_)
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
 *   This can allow the compiler to occasionally better optimize, but mostly avoid spurrious warnings.
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
 *  Wrap this macro around entire declaration, as in:
 *      _DeprecatedFunction_ (inline bool    empty () const, "Instead use IsMissing() - to be removed after v2.0a11");
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




/*
 *  Wrap this macro around entire declaration, as in:
 *       class _DeprecatedClass_ (DirectoryContentsIterator, "DEPRECATED in v2.0a32 - use IO::FileSystem::DirectoryIterator")
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
 *  NB: we can lose these macros once all our compilers support the new C++ syntax.
 *
 *  Also note - the bug workaround - enum  { NAME = static_cast<TYPE> (VALUE) } - doesn't
 *  work pefrectly - since it doesnt create a name of the desired type, but its usually
 *  pretty close.
 */
#if     qCompilerAndStdLib_constexpr_StaticDataMember_Buggy
#define DEFINE_CONSTEXPR_CONSTANT(TYPE,NAME,VALUE)\
    enum  { NAME = static_cast<TYPE> (VALUE) };
#else
#define DEFINE_CONSTEXPR_CONSTANT(TYPE,NAME,VALUE)\
    static  constexpr TYPE NAME = VALUE;
#endif





#if     qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy
#define FILE_SCOPE_TOP_OF_TRANSLATION_UNIT_MSVC_FLOATING_POINT_BWA()\
    namespace {\
        struct _FILE_SCOPE_ATSTARTOFMODULE_MSVC_FLOATING_POINT_BWA_ {\
            double f ()\
            {\
                return 1.0;\
            }\
            double  MatchWord_ ()\
            {\
                double ttt = f ();\
                return ttt;\
            }\
            _FILE_SCOPE_ATSTARTOFMODULE_MSVC_FLOATING_POINT_BWA_ ()\
            {\
                double x = MatchWord_ ();\
            }\
        } _FILE_SCOPE_ATSTARTOFMODULE_MSVC_FLOATING_POINT_BWA_data_; \
    }
#endif





#endif  /*defined(__cplusplus)*/



#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

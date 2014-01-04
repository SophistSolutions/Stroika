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
#if     (__clang_major__ > 3) || (__clang_major__ == 3 && (__clang_minor__ >= 5))
#pragma message ("Info: Stroika untested with this version of clang++")
#endif

#elif   defined (__GNUC__)

#if     __GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ < 7))
#pragma message ("Warning: Stroika does not support versions prior to GCC 4.7")
#endif
#if     __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 8))
#pragma message ("Info: Stroika untested with this version of GCC")
#endif

#elif     defined (_MSC_VER)

#define _MS_VS_2k13_VER_    1800

#if      _MSC_VER < _MS_VS_2k13_VER_
#pragma message ("Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2013")
#endif
#if      _MSC_VER > _MS_VS_2k13_VER_
#pragma message ("Info: Stroika untested with this version of Microsoft Visual Studio.net")
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
// MSFT tries to support this but its buggy so disable usage
#define qCompilerAndStdLib_LocaleTM_put_Buggy     (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_LocaleTM_put_Buggy     0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy
@DESCRIPTION:
    const time_get<wchar_t>& tmget = use_facet <time_get<wchar_t>> (l);
    istreambuf_iterator<wchar_t> i = tmget.get_date (itbegin, itend, iss, state, &when);
    ErroniousReportFailWhenDateBefore1900
*/
#ifndef qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy

#if   defined (_MSC_VER)
#define qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy     (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_TMGetGetDateWhenDateBefore1900_Buggy     0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy
@DESCRIPTION:   <p></p>
*/
#ifndef qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear_Buggy    (_MSC_VER == _MS_VS_2k13_VER_)
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
#define qCompilerAndStdLib_VarDateFromStrOnFirstTry_Buggy  (_MSC_VER == _MS_VS_2k13_VER_)
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
#define qCompilerAndStdLib_constexpr_Buggy   (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_constexpr_Buggy   0
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
#define qCompilerAndStdLib_HasFirstTimeUsePerTranslationUnitFloatingPoint_Buggy   (_MSC_VER == _MS_VS_2k13_VER_)
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
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       ((__clang_major__ < 3) || ((__clang_major__ == 3) && (__clang_minor__ <= 4)))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8)))
#else
#define qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy       0
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
*/
#ifndef qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy           (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy           (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_TemplateParamterOfNumericLimitsMinMax_Buggy            0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_noexcept_Buggy
@DESCRIPTION:   Defined 0 if the compiler supports noexcept
*/
#ifndef qCompilerAndStdLib_noexcept_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_noexcept_Buggy    (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_noexcept_Buggy    0
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
#define qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy 1
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
#define qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy     (__clang_major__ == 3 && (__clang_minor__ < 5))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy     (__GNUC__ == 4 && (__GNUC_MINOR__ < 9))
#else
#define qCompilerAndStdLib_TypeTraitsNewNamesIsCopyableEtc_Buggy     0
#endif

#endif





//
// Workaround bug that the SIGNAL KILL doesn't appear to interupt the GCC implemntation of condition_variable::wait_for ()
// -- LGP 2012-05-27
//
//      MAYBE use http://www.mkssoftware.com/docs/man3/pthread_cancel.3.asp
//
//  @todo   At THIS point - I think qEVENT_GCCTHREADS_LINUX_WAITBUG is probbaly  NOT a GCC bug. Must debug better! But no
//          reason to think this is a compiler bug...
//
#ifndef qEVENT_GCCTHREADS_LINUX_WAITBUG
#define qEVENT_GCCTHREADS_LINUX_WAITBUG     (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8)))
#endif





#ifndef qCompilerAndStdLib_thread_local_keyword_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_thread_local_keyword_Buggy   (_MSC_VER == _MS_VS_2k13_VER_)
#elif     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_thread_local_keyword_Buggy   (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#else
#define qCompilerAndStdLib_thread_local_keyword_Buggy   0
#endif

#endif



// @todo - investigate this better
#ifndef qTemplateAccessCheckConfusionProtectedNeststingBug

#if     defined (_MSC_VER)
#define qTemplateAccessCheckConfusionProtectedNeststingBug   (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qTemplateAccessCheckConfusionProtectedNeststingBug   0
#endif

#endif




// @todo - investigate this better
#ifndef qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug   (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_stdinitializer_templateoftemplateCompilerCrasherBug   0
#endif

#endif





// @todo - investigate this better
#ifndef qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug   (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug   0
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
*/
#ifndef qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy   (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy   (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_lambda_default_argument_with_template_param_as_function_cast_Buggy   0
#endif

#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
@DESCRIPTION:   See also qCompilerAndStdLib_Supports_lambda_default_argument_with_template_param_as_function_cast
*/
#ifndef qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy

#if     defined (__clang__)
#define qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy    (__clang_major__ == 3 && (__clang_minor__ < 5))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy    (__GNUC__ == 4 && (__GNUC_MINOR__ < 8))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy    (_MSC_VER == _MS_VS_2k13_VER_)
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
@CONFIGVAR:     qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy
@DESCRIPTION:   <p>
            template    <typename   T>
            T   NearlyEquals (T l, T r, T epsilon = (1000 * numeric_limits<T>::epsilon()));
            </p>
*/
#ifndef qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy      (_MSC_VER == _MS_VS_2k13_VER_)
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
@CONFIGVAR:     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy
@DESCRIPTION:   <p></p>
*/
#ifndef qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy

#if     defined (_MSC_VER)
#define qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy       (_MSC_VER == _MS_VS_2k13_VER_)
#else
#define qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy       0
#endif

#endif



/*
@CONFIGVAR:     qCompilerAndStdLib_deprecatedFeatureMissing
@DESCRIPTION:
*/
#ifndef qCompilerAndStdLib_deprecatedFeatureMissing

#if     defined (__clang__)
#define qCompilerAndStdLib_deprecatedFeatureMissing             ((__clang_major__ == 3) && (__clang_minor__ <= 4))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_deprecatedFeatureMissing             (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_deprecatedFeatureMissing             (_MSC_VER == _MS_VS_2k13_VER_)
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
#define qCompilerAndStdLib_string_conversions_Buggy              ((__clang_major__ == 3) && (__clang_minor__ <= 4))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_string_conversions_Buggy              (__GNUC__ == 4 && (__GNUC_MINOR__ <= 8))
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
#define GCC_DIAG_STR_(s)		#s
#define GCC_DIAG_JOINSTR_(x,y)	GCC_DIAG_STR(x ## y)
#define GCC_DIAG_DO_PRAGMA_(x)	_Pragma (#x)
#define GCC_DIAG_PRAGMA_(x)		GCC_DIAG_DO_PRAGMA_(GCC diagnostic x)
#define DISABLE_COMPILER_GCC_WARNING_START(x)   GCC_DIAG_PRAGMA_(push) GCC_DIAG_PRAGMA(ignored GCC_DIAG_JOINSTR_(-W,x))
#define DISABLE_COMPILER_GCC_WARNING_END(x)     GCC_DIAG_PRAGMA_(pop)
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





#if     qCompilerAndStdLib_thread_local_keyword_Buggy
#if     defined (__GNUC__)
#define thread_local    __thread
#else
#define thread_local    __declspec(thread)
#endif
#endif





/*
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
 */
#if     !defined (_Deprecated_)
#if     qCompilerAndStdLib_deprecatedFeatureMissing && defined (__GNUC__)
#define _Deprecated_(func,MESSAGE) func __attribute__ ((deprecated))
#elif   qCompilerAndStdLib_deprecatedFeatureMissing && defined(_MSC_VER)
#define _Deprecated_(func,MESSAGE) __declspec(deprecated) func
#else
#define _Deprecated_(func,MESSAGE) [[deprecated(MESSAGE)]] func
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

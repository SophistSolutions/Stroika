/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
/////////////////////////////////////// DRAFT SUPPORT FOR CLANG++--- VERY MININAL TESTING AND NOT NOT CLOSE TO WORKING

// Must check CLANG first, since it also defines GCC
//#define __clang_major__ 3
//#define __clang_minor__ 0
//#define __clang_patchlevel__ 0
#if     __clang_major__ < 3 || (__clang_major__ == 3 && (__clang_minor__ < 0))
#pragma message ("Warning: Stroika does not support versions prior to clang++ 3.0")
#endif

#elif   defined (__GNUC__)

#if     __GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ < 6))
#pragma message ("Warning: Stroika does not support versions prior to GCC 4.6")
#endif
#if     __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 7))
#pragma message ("Info: Stroika untested with this version of GCC")
#endif

#elif     defined (_MSC_VER)

#define _MS_VS_2k10_VER_    1600
#define _MS_VS_2k12_VER_    1700
#if      _MSC_VER < _MS_VS_2k10_VER_
#pragma message ("Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2010")
#endif
#if      _MSC_VER > _MS_VS_2k12_VER_
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
@CONFIGVAR:     qHas_Syslog
@DESCRIPTION:   <p>tmphack - goes someplace else... -LGP 2011-09-27 </p>
*/
#ifndef qHas_Syslog
#define qHas_Syslog     qPlatform_POSIX
#endif












/*
@CONFIGVAR:     qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear
@DESCRIPTION:   <p></p>
*/
#ifndef qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear    (_MSC_VER <= _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear    0
#endif
#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_StdFindOverloadBug
@DESCRIPTION:   <p>Strange overloading bug - for reasons that elude me - gcc 4.6.3 doesnt find an obvious std::find() use, but if I say find() it works fine???</p>
*/
#if     !defined (qCompilerAndStdLib_StdFindOverloadBug)
#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_StdFindOverloadBug       ( __GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 6)))
#else
#define qCompilerAndStdLib_StdFindOverloadBug       0
#endif
#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_ConstructorDelegation
@DESCRIPTION:   <p></p>
*/
#ifndef qCompilerAndStdLib_Supports_ConstructorDelegation

#if     defined (__GNUC__)  && !defined (__clang__)
#define qCompilerAndStdLib_Supports_ConstructorDelegation   (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7)))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_ConstructorDelegation   (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_ConstructorDelegation   1
#endif

#endif








/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
@DESCRIPTION:   <p></p>
*/
#ifndef qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers  (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers  1
#endif
#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_constexpr
@DESCRIPTION:   <p>Defined true if the compiler supports constexpr</p>
*/
#ifndef qCompilerAndStdLib_Supports_constexpr
#if     defined (__clang__)
// Maybe not totally broken for clang - but broken enough to disable for now...
#define qCompilerAndStdLib_Supports_constexpr   ((__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ >= 1)))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_constexpr   (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_constexpr   1
#endif
#endif












/*
@CONFIGVAR:     qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded
@DESCRIPTION:   <p>Defined true if the compiler generates errors for static assertions in functions
            which should never be expanded. Such functions/static_assertions CAN be handy to make it
            more obvious of type mismatches with As<> etc templates.</p>
*/
#ifndef qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded
#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded           (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 7)))
#else
#define qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded            0
#endif
#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_regex_search
@DESCRIPTION:   <p>Defined true if the compiler supports regex_search</p>
*/
#ifndef qCompilerAndStdLib_Supports_regex_search

#if     defined (__GNUC__) && !defined (__clang__) &&  (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 6)))
#define qCompilerAndStdLib_Supports_regex_search            0
#else
#define qCompilerAndStdLib_Supports_regex_search            1
#endif

#endif








/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_regex_replace
@DESCRIPTION:   <p>Defined true if the compiler supports regex_replace</p>
*/
#ifndef qCompilerAndStdLib_Supports_regex_replace

#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_Supports_regex_replace       (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7)))
#else
#define qCompilerAndStdLib_Supports_regex_replace       1
#endif

#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_noexcept
@DESCRIPTION:   <p>Defined true if the compiler supports noexcept</p>
*/
#ifndef qCompilerAndStdLib_Supports_noexcept

#if     defined (_MSC_VER) && _MSC_VER <= _MS_VS_2k12_VER_
#define qCompilerAndStdLib_Supports_noexcept    0
#else
#define qCompilerAndStdLib_Supports_noexcept    1
#endif

#endif











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
// assume its either a bug with the VS implementaiton of the compiler or the template itself.
//
#ifndef qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes

#if     defined (_MSC_VER) && _MSC_VER <= _MS_VS_2k12_VER_
#define qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes 0
#else
#define qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes 1
#endif

#endif






#ifndef qCompilerAndStdLib_TemplateFriendFunctionsRequirePredeclaredTemplateFunction
#if     defined (__GNUC__) && !defined (__clang__)
// Not totally clear what compiler is wrong, but I'm not sure why I would need to pre-declare the
// template the way GCC 4.6.3 appears to require        -- LGP 2012-07-27
#define qCompilerAndStdLib_TemplateFriendFunctionsRequirePredeclaredTemplateFunction (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 6)))
#else
#define qCompilerAndStdLib_TemplateFriendFunctionsRequirePredeclaredTemplateFunction 0
#endif
#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_TemplateUsingEquals
@DESCRIPTION:   <p>Unclear if my usage is right, and what compiler purports to support it.
                Definetely not msvc 2010</p>
*/
#ifndef qCompilerAndStdLib_Supports_TemplateUsingEquals
#define qCompilerAndStdLib_Supports_TemplateUsingEquals 0
#endif








/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_DefaultParametersForTemplateFunctions
@DESCRIPTION:   <p>http://stackoverflow.com/questions/2447458/default-template-arguments-for-function-templates</p>
*/
#ifndef qCompilerAndStdLib_Supports_DefaultParametersForTemplateFunctions
#define qCompilerAndStdLib_Supports_DefaultParametersForTemplateFunctions 0
#endif









/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_threads
@DESCRIPTION:   <p>Defined true if the compiler supports the stdc++ threads library</p>
*/
#ifndef qCompilerAndStdLib_Supports_threads
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_threads (_MSC_VER >= _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_threads 1
#endif
#endif






#ifndef qCompilerAndStdLib_Supports_thread_local_keyword
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_thread_local_keyword (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_thread_local_keyword (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 8)))
#endif
#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_isnan
@DESCRIPTION:   <p>Defines if the compiler stdC++/c99 library supports the std::isnan() function</p>
*/
#ifndef qCompilerAndStdLib_Supports_isnan
#if     defined (_MSC_VER)
// msvc has _isnan
#define qCompilerAndStdLib_Supports_isnan   (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_isnan   1
#endif
#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_errno_t
@DESCRIPTION:   <p>Controls whether or not the compiler provides an implementation
                of errno_t (gcc for example just assumes its int)</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_errno_t)
#if     defined (__GNUC__) && !defined (__clang__)
#define qCompilerAndStdLib_Supports_errno_t (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 8)))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_errno_t 1
#else
#define qCompilerAndStdLib_Supports_errno_t 1
#endif
#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_stdchrono
@DESCRIPTION:   <p>Controls whether or not the compiler / standard library supports <chrono>.</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_stdchrono)
#if     defined (__clang__)
// Because of constexpr
#define qCompilerAndStdLib_Supports_stdchrono       ((__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ >= 1)))
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_stdchrono       (_MSC_VER > _MS_VS_2k10_VER_)
#else
#define qCompilerAndStdLib_Supports_stdchrono       1
#endif
#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_stdchrono_diff_double
@DESCRIPTION:   fConditionVariable_.wait_for (lock, Time::Duration (remaining).As<std::chrono::duration<double>> didnt
                compile without this hack...
*/
#if     !defined (qCompilerAndStdLib_Supports_stdchrono_diff_double)
#if   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_stdchrono_diff_double       (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_stdchrono_diff_double       qCompilerAndStdLib_Supports_stdchrono
#endif
#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_varadic_templates
@DESCRIPTION:   <p>Controls whether or not the compiler / standard library supports varadic templates.</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_varadic_templates)
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_varadic_templates  (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_varadic_templates  1
#endif
#endif











/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_stdatomic
@DESCRIPTION:   <p>Controls whether or not the compiler supports std::atomic.</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_stdatomic)
#if     defined (__clang__)
#define qCompilerAndStdLib_Supports_stdatomic   (__clang_major__ > 3 || (__clang_major__ == 3 && (__clang_minor__ >= 1)))
#elif   defined (_MSC_VER)
//#define qCompilerAndStdLib_Supports_stdatomic   (_MSC_VER > _MS_VS_2k10_VER_)
// really is supported, but fake off for now til we fix it --LGP 2012-10-20
#define qCompilerAndStdLib_Supports_stdatomic   (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_stdatomic   1
#endif
#endif








/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_override
@DESCRIPTION:   <p>Controls whether or not the compiler the override function annotion (added in C++11).</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_override)
#if     defined (__GNUC__) && !defined (__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ < 7)))
#define qCompilerAndStdLib_Supports_override    0
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_override    (_MSC_VER > _MS_VS_2k10_VER_)
#else
#define qCompilerAndStdLib_Supports_override    1
#endif
#endif











/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_lambda_default_argument
@DESCRIPTION:   <p>Defined true if the compiler supports lambda functions as default arguments to other functions</p>
*/
#ifndef qCompilerAndStdLib_Supports_lambda_default_argument
#if     defined (__clang__)
#define qCompilerAndStdLib_Supports_lambda_default_argument     (__clang_major__ > 3 || (__clang_major__ == 3 && (__clang_minor__ >= 1)))
#elif   defined (__GNUC__)
#define qCompilerAndStdLib_Supports_lambda_default_argument     (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 8)))
#else
#define qCompilerAndStdLib_Supports_lambda_default_argument     1
#endif
#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_lambda_default_argument_with_template_param_as_function_cast
@DESCRIPTION:   <p></p>
*/
#ifndef qCompilerAndStdLib_Supports_lambda_default_argument_with_template_param_as_function_cast
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_lambda_default_argument_with_template_param_as_function_cast    (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_lambda_default_argument_with_template_param_as_function_cast     qCompilerAndStdLib_Supports_lambda_default_argument
#endif
#endif








/*
@CONFIGVAR:     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
@DESCRIPTION:   <p>https://connect.microsoft.com/VisualStudio/feedback/details/572138
    From the C++0x FCD (N3092 5.1.2/6):
        "The closure type for a lambda-expression with no lambda-capture has a public non-virtual non-explicit const conversion
        function to pointer to function having the same parameter and return types as the closure type�s function call operator.
        The value returned by this conversion function shall be the address of a function that, when invoked, has the same effect
        as invoking the closure type�s function call operator."
    Not implemented in VS.net 2010
    Often results in compiler error "No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called"
</p>
*/
#ifndef qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported   (_MSC_VER >= _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported   1
#endif
#endif




#ifndef qCompilerAndStdLib_Supports_lambda_default_argument_WITH_closureCvtToFunctionPtrSupported
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_lambda_default_argument_WITH_closureCvtToFunctionPtrSupported   (_MSC_VER > _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_Supports_lambda_default_argument_WITH_closureCvtToFunctionPtrSupported   qCompilerAndStdLib_Supports_lambda_default_argument
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
@CONFIGVAR:     qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompilerBug
@DESCRIPTION:   <p>
            template    <typename   T>
            T   NearlyEquals (T l, T r, T epsilon = (1000 * numeric_limits<T>::epsilon()));
            </p>
*/
#ifndef qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompilerBug
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompilerBug      (_MSC_VER <= _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompilerBug      0
#endif
#endif










/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
@DESCRIPTION:   <p>Defined true if the compiler supports constexpr</p>
*/
#ifndef qCompilerAndStdLib_Supports_constexpr_StaticDataMember
#if     defined (__GNUC__)
// Seems to compile with gcc 4.7.2, but then caused link errors - unclear if my bug or gcc bug?
#define qCompilerAndStdLib_Supports_constexpr_StaticDataMember       ( __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 8)))
#else
#define qCompilerAndStdLib_Supports_constexpr_StaticDataMember       qCompilerAndStdLib_Supports_constexpr
#endif
#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
@DESCRIPTION:   <p></p>
*/
#ifndef qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug       (_MSC_VER <= _MS_VS_2k12_VER_)
#else
#define qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug       0
#endif
#endif





/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_string_conversions
@DESCRIPTION:   22.3.3.2.2  string conversions  N
*/
#ifndef qCompilerAndStdLib_Supports_string_conversions
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_string_conversions              ( __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 9)))
#else
#define qCompilerAndStdLib_Supports_string_conversions              1
#endif
#endif









/*
 *******************************************************************
 *******************************************************************
 ********************* REACT TO SOME DEFINES ***********************
 *******************************************************************
 *******************************************************************
 */


// MSFT has a hack that prevents workarounds to other features they don't support, but luckily, they may that hack
// easy enough to disable ;-)
#if     defined (_MSC_VER) && _MSC_VER == _MS_VS_2k12_VER_
#define _ALLOW_KEYWORD_MACROS
#endif





// SHOULD GO ELSEWHERE -- LGP 2011-10-27
#if     !qCompilerAndStdLib_Supports_noexcept
#define noexcept    throw ()
#endif




#if     !qCompilerAndStdLib_Supports_constexpr
// This works as an adequate workaround pretty often
#define constexpr   const
#endif



// If override is not supported, just stubbing it out but leaving it there for
// compilers that do support it, and/or documentation purposes
#if     !qCompilerAndStdLib_Supports_override
#define override
#endif





#if     !qCompilerAndStdLib_Supports_thread_local_keyword
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
 * NB: we can lose these macros once all our compilers support the new C++ syntax.
 */
#if  qCompilerAndStdLib_Supports_constexpr_StaticDataMember
#define DEFINE_CONSTEXPR_CONSTANT(TYPE,NAME,VALUE)\
    static  constexpr TYPE NAME = VALUE;
#else
#define DEFINE_CONSTEXPR_CONSTANT(TYPE,NAME,VALUE)\
    enum  { NAME = VALUE };
#endif






/*
* NB: we can lose these macros once all our compilers support the new C++ syntax.
*/
#if     qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
#define NO_DEFAULT_CONSTRUCTOR(NAME)\
    NAME () = delete;
#define NO_COPY_CONSTRUCTOR(NAME)\
    NAME (const NAME&) = delete;
#define NO_COPY_CONSTRUCTOR2(NAME,NAMEWITHT)\
    NAME (const NAMEWITHT&) = delete;
#define NO_ASSIGNMENT_OPERATOR(NAME)\
    const NAME& operator= (const NAME&) = delete;
#else
#define NO_DEFAULT_CONSTRUCTOR(NAME)\
    private:    NAME ();
#define NO_COPY_CONSTRUCTOR(NAME)\
    private:    NAME (const NAME&);
#define NO_COPY_CONSTRUCTOR2(NAME,NAMEWITHT)\
    private:    NAME (const NAMEWITHT&);
#define NO_ASSIGNMENT_OPERATOR(NAME)\
    private:    const NAME& operator= (const NAME&);
#endif




#endif  /*defined(__cplusplus)*/



#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

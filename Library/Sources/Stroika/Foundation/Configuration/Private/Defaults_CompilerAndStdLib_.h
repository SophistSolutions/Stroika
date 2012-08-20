/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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
 ***************************************************
 * CHECK / WARN about supported compilers/versions *
 ***************************************************
 */
#if     defined (__GNUC__)
	#if     __GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ < 6))
		#pragma message ("Warning: Stroika does not support versions prior to GCC 4.6")
	#endif
#elif     defined (_MSC_VER)
	#define	_MS_VS_2k10_VER_	1600
	#if      _MSC_VER < _MS_VS_2k10_VER_
		#pragma message ("Warning: Stroika does not support versions prior to Microsoft Visual Studio.net 2010")
	#endif
#else
	#pragma message ("Warning: Stroika does recognize the compiler being used. It may work, but you may need to update some of the other defines for what features are supported by your compiler.")
#endif












//tmphack - goes someplace else...
// LGP 2011-09-27
#ifndef qHas_Syslog
#define qHas_Syslog     qPlatform_POSIX
#endif












//NEED DOCS SOMPEPLACE
#ifndef qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear
#if     defined (__GNUC__)
// not sure what version will/does support this
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear    0
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear    1
#else
#define qCompilerAndStdLib_Supports_ConstructorDelegation   0
#endif
#endif








//NEED DOCS SOMPEPLACE
#ifndef qCompilerAndStdLib_Supports_ConstructorDelegation
#if     defined (__GNUC__)
// not sure what version will/does support this
#define qCompilerAndStdLib_Supports_ConstructorDelegation   0
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_Supports_ConstructorDelegation   0
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_ConstructorDelegation   1
#endif
#endif





//NEED DOCS SOMPEPLACE
#ifndef qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers  1
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers  0
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers  1
#endif
#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_constexpr
@DESCRIPTION:   <p>Defined true if the compiler supports constexpr</p>
*/
#ifndef qCompilerAndStdLib_Supports_constexpr
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_constexpr   1
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_Supports_constexpr   0
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_constexpr   1
#endif
#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_regex_search
@DESCRIPTION:   <p>Defined true if the compiler supports regex_search</p>
*/
#ifndef qCompilerAndStdLib_Supports_regex_search
#if     defined (__GNUC__)
#if     __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
#define qCompilerAndStdLib_Supports_regex_search    1
#else
#define qCompilerAndStdLib_Supports_regex_search    0
#endif
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_regex_search    1
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_regex_search    1
#endif
#endif






/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_regex_replace
@DESCRIPTION:   <p>Defined true if the compiler supports regex_replace</p>
*/
#ifndef qCompilerAndStdLib_Supports_regex_replace
#if     defined (__GNUC__)
#if     __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
#define qCompilerAndStdLib_Supports_regex_replace    1
#else
#define qCompilerAndStdLib_Supports_regex_replace    0
#endif
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_regex_replace    1
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_regex_replace    1
#endif
#endif







/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_noexcept
@DESCRIPTION:   <p>Defined true if the compiler supports noexcept</p>
*/
#ifndef qCompilerAndStdLib_Supports_noexcept
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_noexcept    1
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_Supports_noexcept    0
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_noexcept    1
#endif
#endif



// SHOULD GO ELSEWHERE -- LGP 2011-10-27
#if     !qCompilerAndStdLib_Supports_noexcept
#define noexcept    throw  ()
#endif












// Not sure if this is a compiler bug/template lib bug, or just an adnvantage of the stroika SharedPtr
// verus std::shared_ptr<> stuff (now I'm getting rid of) over the std::shared_ptr<>
//
// At least two obvious (but not great) workarounds.
//      o   make the class public
//      o   use custom deleter
//
// maybe using FRIENDS will work?
//
// Anyhow - based on experimentationw with gcc, and 'common sense' - this should be defined to work - I will
// assume its either a bug with the VS implementaiton of the compiler or the template itself.
//
#ifndef qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes 1
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes 0
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes 1
#endif
#endif






// Not totally clear what compiler is wrong, but I'm not sure why I would need to pre-declare the
// template the way GCC 4.6.3 appears to require        -- LGP 2012-07-27
#ifndef qCompilerAndStdLib_TemplateFriendFunctionsRequirePredeclaredTemplateFunction
#if     defined (__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 6)))
#define qCompilerAndStdLib_TemplateFriendFunctionsRequirePredeclaredTemplateFunction 1
#else
#define qCompilerAndStdLib_TemplateFriendFunctionsRequirePredeclaredTemplateFunction 0
#endif
#endif






#ifndef qCompilerAndStdLib_Supports_TemplateUsingEquals
// Unclear if my usage is right, and what compiler purports to support it. Definetely not msvc 2010
#define qCompilerAndStdLib_Supports_TemplateUsingEquals 0
#endif



// http://stackoverflow.com/questions/2447458/default-template-arguments-for-function-templates
#ifndef qCompilerAndStdLib_Supports_DefaultParametersForTemplateFunctions
#define qCompilerAndStdLib_Supports_DefaultParametersForTemplateFunctions 0
#endif


/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_threads
@DESCRIPTION:   <p>Defined true if the compiler supports the stdc++ threads library</p>
*/
#ifndef qCompilerAndStdLib_Supports_threads
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_threads 1
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_Supports_threads 0
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_threads 1
#endif
#endif


/*
@CONFIGVAR:     qCompilerAndStdLib_isnan
@DESCRIPTION:   <p>Defines if the compiler stdC++/c99 library supports the std::isnan() function</p>
*/
#ifndef qCompilerAndStdLib_isnan
#if     defined (__GNUC__)
#define qCompilerAndStdLib_isnan    1
#else
// default off cuz we have a good safe impl
#define qCompilerAndStdLib_isnan    0
#endif
#endif


/*
@CONFIGVAR:     qCompilerAndStdLib__isnan
@DESCRIPTION:   <p>Defines if the compiler stdC++/c99 library supports the std::isnan() function</p>
*/
#ifndef qCompilerAndStdLib__isnan
#if     defined (_MSC_VER)
#define qCompilerAndStdLib__isnan   1
#else
// default off cuz we have a good safe impl
#define qCompilerAndStdLib__isnan   0
#endif
#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_errno_t
@DESCRIPTION:   <p>Controls whether or not the compiler provides an implementation of errno_t (gcc for example just assumes its int)</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_errno_t)
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_errno_t 0
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_errno_t 1
#else
// GUESS
#define qCompilerAndStdLib_Supports_errno_t 0
#endif
#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_stdchrono
@DESCRIPTION:   <p>Controls whether or not the compiler / standard library supports <chrono>.</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_stdchrono)
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_stdchrono  1
#elif   defined (_MSC_VER)
#if      _MSC_VER >= 1700
#define qCompilerAndStdLib_Supports_stdchrono  1
#else
#define qCompilerAndStdLib_Supports_stdchrono  0
#endif
#else
// GUESS
#define qCompilerAndStdLib_Supports_stdchrono  1
#endif
#endif



/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_stdthreads
@DESCRIPTION:   <p>Controls whether or not the compiler / standard library supports threads.</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_stdthreads)
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_stdthreads  1
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_stdthreads  0
#else
// GUESS
#define qCompilerAndStdLib_Supports_stdthreads  1
#endif
#endif








/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_varadic_templates
@DESCRIPTION:   <p>Controls whether or not the compiler / standard library supports varadic templates.</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_varadic_templates)
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_varadic_templates  1
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_Supports_varadic_templates  0
#else
// GUESS
#define qCompilerAndStdLib_Supports_varadic_templates  1
#endif
#endif




/*
    *   The Standard C++ mechanism of commenting out unused parameters isn't good enuf
    *   in the case where the parameters might be used conditionally. This hack is
    *   to shutup compiler warnings in those cases.
    */
#ifndef Arg_Unused
#define Arg_Unused(x)   ((void) &x)
#endif




/*
    *   Sometimes its handy to mark a function as not actually returning (because of throws or other reasons)
    *   This can allow the compiler to occasionally better optimize, but mostly avoid spurrious warnings.
    */
#if defined(_MSC_VER)
#define _NoReturn_  __declspec(noreturn)
#elif defined (__GNUG__ )
#define _NoReturn_  __attribute__((noreturn))
#else
#define _NoReturn_
#endif




/*
    *   Sometimes its handy to mark a line of code as a no-op - so its arguments are not executed (as with
    * trace macros).
    */
#if defined(_MSC_VER)
#define _NoOp_  __noop
#else
#define _NoOp_(...)
#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_nullptr
@DESCRIPTION:   <p>Controls whether or not the compiler supports the nullptr value (added in C++11).</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_nullptr)
#if     defined (__GNUC__)
#if     __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 6))
#define qCompilerAndStdLib_Supports_nullptr 1
#else
#define qCompilerAndStdLib_Supports_nullptr 0
#endif
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_nullptr     1
#else
// GUESS
#define qCompilerAndStdLib_Supports_nullptr 1
#endif
#endif


#if     !qCompilerAndStdLib_Supports_nullptr
#define nullptr NULL
#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_stdatomic
@DESCRIPTION:   <p>Controls whether or not the compiler the override function annotion (added in C++11).</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_stdatomic)
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_stdatomic   1
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_Supports_stdatomic   0
#else
// GUESS
#define qCompilerAndStdLib_Supports_stdatomic   1
#endif
#endif



/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_override
@DESCRIPTION:   <p>Controls whether or not the compiler the override function annotion (added in C++11).</p>
*/
#if     !defined (qCompilerAndStdLib_Supports_override)
#if     defined (__GNUC__)
#if     __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
#define qCompilerAndStdLib_Supports_override    1
#else
#define qCompilerAndStdLib_Supports_override    0
#endif
#elif   defined (_MSC_VER)
#define qCompilerAndStdLib_Supports_override    0
#else
// GUESS
#define qCompilerAndStdLib_Supports_override    1
#endif
#endif


// If override is not supported, just stubbing it out but leaving it there for compilers that do support it, and/or documentation purposes
#if     !qCompilerAndStdLib_Supports_override
#define override
#endif




/*
@CONFIGVAR:     qCompilerAndStdLib_Supports_lambda_default_argument
@DESCRIPTION:   <p>Defined true if the compiler supports lamba functions as default arguments to other functions</p>
*/
#ifndef qCompilerAndStdLib_Supports_lambda_default_argument
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Supports_lambda_default_argument 0
#elif   defined (_MSC_VER) && _MSC_VER >= 1600
#define qCompilerAndStdLib_Supports_lambda_default_argument 1
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_Supports_lambda_default_argument 1
#endif
#endif



/*
@CONFIGVAR:     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
@DESCRIPTION:   <p>https://connect.microsoft.com/VisualStudio/feedback/details/572138
    From the C++0x FCD (N3092 5.1.2/6):
        "The closure type for a lambda-expression with no lambda-capture has a public non-virtual non-explicit const conversion
        function to pointer to function having the same parameter and return types as the closure type’s function call operator.
        The value returned by this conversion function shall be the address of a function that, when invoked, has the same effect
        as invoking the closure type’s function call operator."
    Not implemented in VS.net 2010
    Often results in compiler error "No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called"
</p>
*/
#ifndef qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
#if     defined (__GNUC__)
#define qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported   1
#elif   defined (_MSC_VER) && _MSC_VER <= 1600
#define qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported   0
#else
// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
#define qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported   1
#endif
#endif







//FOR ASSERT
#ifndef qCompilerAndStdLib_Support__PRETTY_FUNCTION__
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Support__PRETTY_FUNCTION__   1
#else
#define qCompilerAndStdLib_Support__PRETTY_FUNCTION__   0
#endif
#endif


//FOR ASSERT
#ifndef qCompilerAndStdLib_Support__func__
#if     defined (__GNUC__)
#define qCompilerAndStdLib_Support__func__   1
#else
#define qCompilerAndStdLib_Support__func__   0
#endif
#endif


//FOR ASSERT
#ifndef qCompilerAndStdLib_Support__FUNCTION__
#if     defined (_MSC_VER)
#define qCompilerAndStdLib_Support__FUNCTION__   1
#else
#define qCompilerAndStdLib_Support__FUNCTION__   0
#endif
#endif





#endif  /*defined(__cplusplus)*/


#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

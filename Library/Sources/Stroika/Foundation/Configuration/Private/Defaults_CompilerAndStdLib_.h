/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_
#define	_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_	1

	/*
	 * This file is a private Stroika implementation detail, and shouldn't be included directly. 
	 * Its used by StroikaConfig.h - optionally included there -
	 * to help implement the set of Stroika public configuration defines.
	 */



	/*
	@CONFIGVAR:		qCompilerAndStdLib_Supports_constexpr
	@DESCRIPTION:	<p>Defined true if the compiler supports constexpr</p>
	*/
	#ifndef	qCompilerAndStdLib_Supports_constexpr
		#if		defined (__GNUC__)
			#define	qCompilerAndStdLib_Supports_constexpr	1
		#elif	defined (_MSC_VER) && _MSC_VER <= 1600
			#define	qCompilerAndStdLib_Supports_constexpr	0
		#else
			// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
			#define	qCompilerAndStdLib_Supports_constexpr	1
		#endif
	#endif



	/*
	@CONFIGVAR:		qCompilerAndStdLib_Supports_threads
	@DESCRIPTION:	<p>Defined true if the compiler supports the stdc++ threads library</p>
	*/
	#ifndef	qCompilerAndStdLib_Supports_threads
		#if		defined (__GNUC__)
			#define	qCompilerAndStdLib_Supports_threads	1
		#elif	defined (_MSC_VER) && _MSC_VER <= 1600
			#define	qCompilerAndStdLib_Supports_threads	0
		#else
			// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
			#define	qCompilerAndStdLib_Supports_threads	1
		#endif
	#endif


	/*
	@CONFIGVAR:		qCompilerAndStdLib_isnan
	@DESCRIPTION:	<p>Defines if the compiler stdC++/c99 library supports the std::isnan() function</p>
	*/
	#ifndef	qCompilerAndStdLib_isnan
		#if		defined (__GNUC__)
			#define	qCompilerAndStdLib_isnan	1
		#else
			// default off cuz we have a good safe impl
			#define	qCompilerAndStdLib_isnan	0
		#endif
	#endif


	/*
	@CONFIGVAR:		qCompilerAndStdLib__isnan
	@DESCRIPTION:	<p>Defines if the compiler stdC++/c99 library supports the std::isnan() function</p>
	*/
	#ifndef	qCompilerAndStdLib__isnan
		#if		defined (_MSC_VER)
			#define	qCompilerAndStdLib__isnan	1
		#else
			// default off cuz we have a good safe impl
			#define	qCompilerAndStdLib__isnan	0
		#endif
	#endif




	/*
	@CONFIGVAR:		qCompilerAndStdLib_Supports_errno_t
	@DESCRIPTION:	<p>Controls whether or not the compiler provides an implementation of errno_t (gcc for example just assumes its int)</p>
	*/
	#if		!defined (qCompilerAndStdLib_Supports_errno_t)
		#if		defined (__GNUC__)
			#define	qCompilerAndStdLib_Supports_errno_t	0
		#elif	defined (_MSC_VER)
			#define	qCompilerAndStdLib_Supports_errno_t	1
		#else
			// GUESS
			#define	qCompilerAndStdLib_Supports_errno_t	0
		#endif
	#endif




	/*
	@CONFIGVAR:		qCompilerAndStdLib_Supports_stdthreads
	@DESCRIPTION:	<p>Controls whether or not the compiler / standard library supports threads.</p>
	*/
	#if		!defined (qCompilerAndStdLib_Supports_stdthreads)
		#if		defined (__GNUC__)
			#define	qCompilerAndStdLib_Supports_stdthreads	1
		#elif	defined (_MSC_VER)
			#define	qCompilerAndStdLib_Supports_stdthreads	0
		#else
			// GUESS
			#define	qCompilerAndStdLib_Supports_stdthreads	1
		#endif
	#endif




	/*
		*	The StandaredC++ mechanism of commenting out unused parameters isn't good enuf
		*	in the case where the parameters might be used conditionally. This hack is
		*	to shutup compiler warnings in those cases.
		*/
	#ifndef	Arg_Unused
	#define	Arg_Unused(x)	((void) &x)
	#endif




	/*
		*	Sometimes its handy to mark a function as not actually returning (because of throws or other reasons)
		*	This can allow the compiler to occasionally better optimize, but mostly avoid spurrious warnings.
		*/
	#if defined(_MSC_VER)
		#define	_NoReturn_	__declspec(noreturn)
	#elif defined (__GNUG__ )
		#define	_NoReturn_	__attribute__((noreturn))
	#else
		#define	_NoReturn_
	#endif




	/*
		*	Sometimes its handy to mark a line of code as a no-op - so its arguments are not executed (as with
		* trace macros).
		*/
	#if defined(_MSC_VER)
		#define	_NoOp_	__noop
	#else
		#define	_NoOp_
	#endif




	/*
	@CONFIGVAR:		qCompilerAndStdLib_Supports_nullptr
	@DESCRIPTION:	<p>Controls whether or not the compiler supports the nullptr value (added in C++11).</p>
	*/
	#if		!defined (qCompilerAndStdLib_Supports_nullptr)
		#if		defined (__GNUC__)
			#if		__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 6))
				#define	qCompilerAndStdLib_Supports_nullptr	1
			#else
				#define	qCompilerAndStdLib_Supports_nullptr	0
			#endif
		#elif	defined (_MSC_VER)
			#define	qCompilerAndStdLib_Supports_nullptr		1
		#else
			// GUESS
			#define	qCompilerAndStdLib_Supports_nullptr	1
		#endif
	#endif


	#if		!qCompilerAndStdLib_Supports_nullptr
		#define	nullptr	NULL
	#endif






	/*
	@CONFIGVAR:		qCompilerAndStdLib_Supports_override
	@DESCRIPTION:	<p>Controls whether or not the compiler the override function annotion (added in C++11).</p>
	*/
	#if		!defined (qCompilerAndStdLib_Supports_override)
		#if		defined (__GNUC__)
			#if		__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
				#define	qCompilerAndStdLib_Supports_override	1
			#else
				#define	qCompilerAndStdLib_Supports_override	0
			#endif
		#elif	defined (_MSC_VER)
			#define	qCompilerAndStdLib_Supports_override	0
		#else
			// GUESS
			#define	qCompilerAndStdLib_Supports_override	1
		#endif
	#endif


	// If override is not supported, just stubbing it out but leaving it there for compilers that do support it, and/or documentation purposes
	#if		!qCompilerAndStdLib_Supports_override
		#define	override
	#endif




	/*
	@CONFIGVAR:		qCompilerAndStdLib_Supports_lambda_default_argument
	@DESCRIPTION:	<p>Defined true if the compiler supports constexpr</p>
	*/
	#ifndef	qCompilerAndStdLib_Supports_lambda_default_argument
		#if		defined (__GNUC__)
			#define	qCompilerAndStdLib_Supports_lambda_default_argument	0
		#elif	defined (_MSC_VER) && _MSC_VER >= 1600
			#define	qCompilerAndStdLib_Supports_lambda_default_argument	1
		#else
			// Guess TRUE, but only so we get an appropriate error compiling if false, and we can easily correct it here
			#define	qCompilerAndStdLib_Supports_lambda_default_argument	1
		#endif
	#endif



#endif	/*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

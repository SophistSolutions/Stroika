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



#endif	/*_Stroika_Foundation_Configuration_Private_Defaults_CompilerAndStdLib_h_*/

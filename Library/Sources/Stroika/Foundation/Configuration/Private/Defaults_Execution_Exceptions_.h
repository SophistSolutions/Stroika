/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Private_Defaults_Execution_Exceptions_h_
#define	_Stroika_Foundation_Configuration_Private_Defaults_Execution_Exceptions_h_	1

	/*
	 * This file is a private Stroika implementation detail, and shouldn't be included directly. 
	 * Its used by StroikaConfig.h - optionally included there -
	 * to help implement the set of Stroika public configuration defines.
	 */


	/*
	 * See DOCS in Stroika/Foundation/Execution/Exceptions.h
	 */
	#if		!defined (qCompilerSupports_errno_t)
		#if		defined (__GNUC__)
			#define	qCompilerSupports_errno_t	0
		#elif	defined (_MSC_VER)
			#define	qCompilerSupports_errno_t	1
		#else
			// GUESS
			#define	qCompilerSupports_errno_t	0
		#endif
	#endif

#endif	/*_Stroika_Foundation_Configuration_Private_Defaults_Execution_Exceptions_h_*/

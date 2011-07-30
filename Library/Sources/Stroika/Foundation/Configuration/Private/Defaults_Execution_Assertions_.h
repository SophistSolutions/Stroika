/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_Private_Defaults_Execution_Assertions_h_
#define	_Stroika_Foundation_Configuration_Private_Defaults_Execution_Assertions_h_	1

	/*
	 * This file is a private Stroika implementation detail, and shouldn't be included directly. 
	 * Its used by StroikaConfig.h - optionally included there -
	 * to help implement the set of Stroika public configuration defines.
	 */


	/*
	 *	Assume the define _DEBUG is used throughout the code to indicate DEBUG mode (assertions on). Assure NDEBUG flag
	 *	is set consistently (even if its not explicitly checked).
	 */
	#if		!defined (qDebug)
		#if		defined(_DEBUG)
			#define	qDebug	1
		#elif	defined (NDEBUG)
			#define	qDebug	0
		#else
			// DEFAULT if we have no idea?
			#warning Neither qDebug nor _DEBUG no NDEBUG defined, defaulting to no debug (qDebug=0)
			#define	qDebug	0
		#endif
	#endif



	// Check for consistent defines
	#if		qDebug
		#if		!defined(_DEBUG)
			// NB #warning is a non-standard extension - maybe we shouldnt use?
			// Maybe get rid of this?
			#warning 	Perhaps _DEBUG should also have been defined?
		#endif
		#if		defined (NDEBUG)
			#error	INCONSISTENT DEFINES (NDEBUG and qDebug=1)
		#endif
	#else
		#if		defined(_DEBUG)
			#error	INCONSISTENT DEFINES (_DEBUG and qDebug=0)
		#endif
		#if		!defined (NDEBUG)
			// NB #warning is a non-standard extension - maybe we shouldnt use?
			// Maybe get rid of this?
			#warning	Perhaps NDEBUG should aslo have been defined
		#endif
	#endif



#endif	/*_Stroika_Foundation_Configuration_Private_Defaults_Execution_Assertions_h_*/

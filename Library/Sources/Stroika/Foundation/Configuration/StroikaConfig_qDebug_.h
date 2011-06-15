/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_qDebug_h_
#define	_Stroika_Foundation_Configuration_qDebug_h_	1


/*
 *	Assume the define _DEBUG is used throughout the code to indicate DEBUG mode (assertions on). Assure NDEBUG flag
 *	is set consistently (even if its not explicitly checked).
 */
#if		defined(_DEBUG)
	#define	qDebug	1
#else
	#define	qDebug	0
#endif

// Check for consistent defines
#if		qDebug
	#if		!defined(_DEBUG)
		#error	INCONSISTENT DEFINES
	#endif
	#if		defined (NDEBUG)
		#error	INCONSISTENT DEFINES
	#endif
#else
	#if		defined(_DEBUG)
		#error	INCONSISTENT DEFINES
	#endif
	#if		!defined (NDEBUG)
		#error	INCONSISTENT DEFINES
	#endif
#endif



#endif	/*_Stroika_Foundation_Configuration_qDebug_h_*/

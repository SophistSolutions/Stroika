/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_StroikaConfig_qEndian_h_
#define	_Stroika_Foundation_Configuration_StroikaConfig_qEndian_h_	1






	// We should automate detecing this, but I don't know any portable way todo so at compile time - just runtime.
	#if		!defined (qEndian_Little) && !defined (qEndian_Big)
		#if		defined (_LITTLE_ENDIAN_)
			#define	qEndian_Little		1
		#elif	defined (_BIG_ENDIAN_)
			#define	qEndian_Big		1
		#elif	defined(__hppa__)
			#define	qEndian_Big		1
		#elif	defined(__m68k__) || defined(mc68000) || defined(_M_M68K)
			#define	qEndian_Big		1
		#elif	defined(__MIPS__) && defined(__MISPEB__)
			#define	qEndian_Big		1
		#elif	defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC)
			#define	qEndian_Big		1
		#elif	defined(__sparc__)
			#define	qEndian_Big		1
		#else
			// DEFAULT
			#define	qEndian_Little		1
		#endif

		// Be sure other defined
		#if		qEndian_Little
			#define	qEndian_Big		0
		#elif	qEndian_Big
			#define	qEndian_Little		0
		#else
			#error	INCONSISTENT DEFINES
		#endif
	#endif





#endif	/*_Stroika_Foundation_Configuration_StroikaConfig_qEndian_h_*/

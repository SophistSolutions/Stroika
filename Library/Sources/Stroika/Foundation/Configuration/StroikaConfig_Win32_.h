/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_StroikaConfig_Win32_h_
#define	_Stroika_Foundation_Configuration_StroikaConfig_Win32_h_	1


	////PRIVATE IMPL DETAIL - DONT INCLUDE DIRECTLY

	#if		defined(_WINDOWS)
		#define	qPlatform_Windows	1
	#endif
	#if		defined(_WIN64)
		#define	qPlatform_Win64		1
		#define	qPlatform_Win32		0
		// in case only _WIN32 defined
		#define	qPlatform_Windows	1
	#elif		defined(_WIN32)
		#define	qPlatform_Win64		0
		#define	qPlatform_Win32		1
		// in case only _WIN32 defined
		#define	qPlatform_Windows	1
	#endif


	#if		qPlatform_Win32
		#if		!qPlatform_Windows
			#error	INCONSISTENT DEFINES
		#endif
	#endif



	#if		qPlatform_Windows

		// not important, but a good default
		#if		!defined (STRICT)
		#define	STRICT
		#endif

		/*
		 *	See http://msdn2.microsoft.com/en-us/library/aa383745.aspx
		 *
		 *	...
		 *		Windows Vista/Windows Server 2008 	_WIN32_WINNT>=0x0600
		 *					WINVER>=0x0600
		 *
		 *	...
		 *		Windows Server 2003 	_WIN32_WINNT>=0x0502
		 *					WINVER>=0x0502
		 *	...
		 *		Windows XP 	_WIN32_WINNT>=0x0501
		 *					WINVER>=0x0501
		 *	...
		 *		Windows Me 	_WIN32_WINDOWS=0x0500
		 *					WINVER>=0x0500
		 *
		 *		Internet Explorer 6.0 	
		 *					_WIN32_IE>=0x0600
		 */

		// We COULD easily use an earlier build (was using 0x501 until 2011-02-22) - but use 0x502 because 
		// MyGetThreadId () can be more efficient using builtin version if we define 0x502 - and no real reason not to...
		//		-- LGP 2011-02-22
		#if		!defined (WINVER)
		#define	WINVER			0x0501
		#endif
		#if		!defined (_WIN32_WINNT)
		#define	_WIN32_WINNT	0x0501
		#endif
		#if		!defined (_WIN32_WINDOWS)
		#define	_WIN32_WINDOWS	0x0501
		#endif
		#if		!defined (_WIN32_IE)
		#define	_WIN32_IE		0x0600
		#endif


		// Avoid MSFT Win32 macro which interferes with several 'standard c++' things - not just the min/max templates, but
		// the numeric_limits<>::max() stuff as well!
		#define	NOMINMAX

	#endif




#endif	/*_Stroika_Foundation_Configuration_StroikaConfig_Win32_h_*/

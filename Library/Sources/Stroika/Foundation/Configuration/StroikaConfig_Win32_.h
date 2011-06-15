/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Configuration_StroikaConfig_Win32_h_
#define	_Stroika_Foundation_Configuration_StroikaConfig_Win32_h_	1



	#if		defined(_WINDOWS)
		#define	qPlatform_Windows	1
	#endif
	#if		defined(_WIN32)
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











// BELOW DEFINES NEED TOGO IN ANOTHER FILE - LIKE STROIKACONFIG_qDebug


	/*
	 * SEE ALSO Stroika/Foundation/Debug/Trace.h
	 *
		*	So we can distribute near-release versions of HF to users, and get feedback - a trace file
		*	to help debug problems we cannot reproduce.
		*/
	#ifndef	qTraceToFile
		#define	qTraceToFile	0
	#endif



	/*
	 * SEE ALSO Stroika/Foundation/Debug/Trace.h
	 *
	@CONFIGVAR:		qDefaultTracingOn
	@DESCRIPTION:	<p>Led contains a limited amount of pre-built tracing code. This could be expanded someday,
				depending on how useful people find it. This defaults to being on only for Windows and if @'qDebug' is
				on (windows only cuz thats the only place I've implemented the trace message emitter so far).</p>
					<p>See also @'qDebug' and @'DebugTrace'</p>
		*/
	#ifndef	qDefaultTracingOn
		#if		defined(_DEBUG)
			#define	qDefaultTracingOn	1
		#else
			#define	qDefaultTracingOn	0
		#endif
	#endif



	/*
	 * SEE ALSO Stroika/Foundation/Memory/BlockAllocated.h
	 *
	@CONFIGVAR:		qAllowBlockAllocation
	@DESCRIPTION:	<p>Allow use of block-allocation. The main reason to
		disable it indescriminantly is for debugging purposes (looking for
		memory leaks). But others may have other reasons.</p>
			<p>Defaults to true.</p>
		*/
	//#define	qAllowBlockAllocation						0
	//#define	qAllowBlockAllocation						1
	#ifndef	qAllowBlockAllocation
		#define	qAllowBlockAllocation						1
	#endif



#endif	/*_Stroika_Foundation_Configuration_StroikaConfig_Win32_h_*/

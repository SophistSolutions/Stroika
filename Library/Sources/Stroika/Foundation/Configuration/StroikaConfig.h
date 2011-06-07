/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__StroiaFoundationConfig_h__
#define	__StroiaFoundationConfig_h__	1


/*
 *	Assume the define _DEBUG is used throughout the code to indicate DEBUG mode (assertions on). Assure NDEBUG flag
 *	is set consistently (even if its not explicitly checked).
 */
#if		defined(_DEBUG) && defined (NDEBUG)
#error	CONFLICT - ONE OR THE OTHER
#endif
#if		!defined(_DEBUG) && !defined (NDEBUG)
#error	DEFINE - ONE OR THE OTHER
#endif


// We should automate detecing this, but I don't know any portable way todo so at compile time - just runtime.
#define	qIsLittleEndian		1
#define	qIsBigEndian		0



// Not sure we will ever support this? Maybe?
//#define	qRealInlines	1




/*
 *	Since this include file CAN define #defines which are required before including system includes, its required
 *	that THIS file be included before all others (like a precomp.h precompiled header file).
 *
 *	This is the ONLY file this is true of. Otherwise, we generally include system files before RFLLib files etc.
 */

// Used to declare these in vcproj file automatically, but better to declare here to assure
// synchonized among all projects that include the R4LLib project
//		-- LGP 2007-09-05
#if		defined(_WIN32)


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

#endif	/*__StroiaFoundationConfig_h__*/

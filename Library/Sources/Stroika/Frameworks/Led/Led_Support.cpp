/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../Foundation/StroikaPreComp.h"

#include	<cctype>
#include	<cstdarg>

#include	"Led_Config.h"	// For qWindows etc defines...

#if		qMacOS
	#include	<AppleEvents.h>		// for URL support
	#include	<Scrap.h>
	#include	<TextUtils.h>
#elif	qWindows
	#include	<fcntl.h>
	#include	<io.h>
#elif	qXWindows
	#include	<unistd.h>
	#include	<time.h>
	#include	<sys/time.h>
	#include	<stdio.h>
	#include	<sys/stat.h>
	#include	<sys/types.h>
	#include	<fcntl.h>
#endif

#include	"Led_CodePage.h"

#include	"Led_Support.h"

#if		qUseInternetConfig
	#if		qSDKHasInternetConfig
		#include	<InternetConfig.h>
	#else
		#include	<ICTypes.h>
		#include	<ICAPI.h>
	#endif
#endif
#if		qUseActiveXToOpenURLs
	#include	<URLMon.h>
#endif



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {



/*
@METHOD:		ACP2WideString
@DESCRIPTION:	
*/
wstring	ACP2WideString (const string& s)
{
	size_t nChars	= s.length () + 1;	// convert null byte, too
	Led_SmallStackBuffer<wchar_t>	result (nChars);
	CodePageConverter	cpg (GetDefaultSDKCodePage ());
	cpg.MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return wstring (result);
}




/*
@METHOD:		Wide2ACPString
@DESCRIPTION:	
*/
string	Wide2ACPString (const wstring& s)
{
	size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
	Led_SmallStackBuffer<char>	result (nChars);
	CodePageConverter	cpg (GetDefaultSDKCodePage ());
	cpg.MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return string (result);
}




#if		qSDK_UNICODE
/*
@METHOD:		Led_ANSI2SDKString
@DESCRIPTION:	<p></p>
*/
Led_SDK_String	Led_ANSI2SDKString (const string& s)
{
	size_t nChars	= s.length () + 1;	// convert null byte, too
	Led_SmallStackBuffer<Led_SDK_Char>	result (nChars);
	CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return Led_SDK_String (result);
}
#else
Led_SDK_String	Led_Wide2SDKString (const wstring& s)
{
	size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
	Led_SmallStackBuffer<char>	result (nChars);
	CodePageConverter	cpg (GetDefaultSDKCodePage ());
	cpg.MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return Led_SDK_String (result);
}
wstring	Led_SDKString2Wide (const Led_SDK_String& s)
{
	return ACP2WideString (s);
}
#endif	

#if		qSDK_UNICODE
/*
@METHOD:		Led_SDKString2ANSI
@DESCRIPTION:	<p></p>
*/
string	Led_SDKString2ANSI (const Led_SDK_String& s)
{
	size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
	Led_SmallStackBuffer<char>	result (nChars);
	CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return string (result);
}
#endif

#if			!qWideCharacters
Led_tString	Led_WideString2tString (const wstring& s)
{
	return Wide2ACPString (s);
}

wstring	Led_tString2WideString (const Led_tString& s)
{
	return ACP2WideString (s);
}
#endif

#if		qWideCharacters != qSDK_UNICODE
/*
@METHOD:		Led_tString2SDKString
@DESCRIPTION:	<p></p>
*/
Led_SDK_String	Led_tString2SDKString (const Led_tString& s)
{
	#if		qWideCharacters && !qSDK_UNICODE
		size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
		Led_SmallStackBuffer<Led_SDK_Char>	result (nChars);
		CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
		return Led_SDK_String (result);
	#elif	!qWideCharacters && qSDK_UNICODE
		size_t nChars	= s.length () + 1;	// convert null byte, too
		Led_SmallStackBuffer<Led_SDK_Char>	result (nChars);
		CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
		return Led_SDK_String (result);
	#else
		#error	"Hmm"
	#endif
}
#endif

#if		qWideCharacters != qSDK_UNICODE
/*
@METHOD:		Led_SDKString2tString
@DESCRIPTION:	<p></p>
*/
Led_tString	Led_SDKString2tString (const Led_SDK_String& s)
{
	#if		qWideCharacters && !qSDK_UNICODE
		size_t nChars	= s.length () + 1;	// convert null byte, too
		Led_SmallStackBuffer<Led_tChar>	result (nChars);
		CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
		return Led_tString (result);
	#elif	!qWideCharacters && qSDK_UNICODE
		size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
		Led_SmallStackBuffer<Led_tChar>	result (nChars);
		CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
		return Led_tString (result);
	#else
		#error	"Hmm"
	#endif
}
#endif

#if		qWideCharacters
/*
@METHOD:		Led_ANSIString2tString
@DESCRIPTION:	<p></p>
*/
Led_tString	Led_ANSIString2tString (const string& s)
{
	size_t nChars	= s.length () + 1;	// convert null byte, too
	Led_SmallStackBuffer<Led_tChar>	result (nChars);
	CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return Led_tString (result);
}
#endif

#if		qWideCharacters
/*
@METHOD:		Led_tString2ANSIString
@DESCRIPTION:	<p></p>
*/
string	Led_tString2ANSIString (const Led_tString& s)
{
	size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
	Led_SmallStackBuffer<char>	result (nChars);
	CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return string (result);
}
#endif



/*
@METHOD:		Led_BeepNotify
@DESCRIPTION:	<p>Make an audible beep on the users terminal. Used as a simple warning
			mechanism (like for typing bad characters).</p>
				<p>NB: For X-Windows only, this function invokes the private gBeepNotifyCallBackProc
			callback procedure to handle the beeping.
			This is because with X-Windows, the beep callback requires data (XDisplay) we don't have at this level.</p>
*/
void	Led_BeepNotify ()
{
	#if		qMacOS
		::SysBeep (1);
	#elif	qWindows
		::MessageBeep (MB_OK);
	#elif	qXWindows
		if (gBeepNotifyCallBackProc != NULL) {
			(gBeepNotifyCallBackProc) ();
		}
	#endif
}


#if		qXWindows
	static	unsigned long	sLastXWindowsEventTime;
	static	double			sLastEventReferenceTime;
	static	double	GetThisMachineCurTime ()
		{
			struct	timeval		tv;
			memset (&tv, 0, sizeof (tv));
			Verify (::gettimeofday (&tv, NULL) == 0);
			Assert (tv.tv_usec < 1000000);
			double	t	=	static_cast<double> (tv.tv_sec) + (tv.tv_usec / 1000000.0);
			return t;
		}
#endif

/*
@METHOD:		Led_GetTickCount
@DESCRIPTION:	<p>Get the number of seconds since some constant, system-specified reference time. This is used
			to tell how much time has elapsed since a particular event.</p>
				<p>Note - though this is based on the same reference time as an time values packed into event records,
			it maybe differently normalized. These times are all in seconds, whereas event records are often in
			other units (ticks - 1/60 of a second, or milliseconds).</p>
				<p>In the case of X-Windows - this business is very complicated because there are two different times
			one might be intersted in. There is the time on the client (where Led is running) and the time on the X-Server
			(users computer screen). Alas - X11R4 appears to have quite weak support for time - and offers no way I've found
			to accurately get the time from the users computer. As a result - with X-Windows - you must arrange to call
			@'SyncronizeLedXTickCount' for each event that specifies a time value (as soon as that event arrives). This
			data - together with time values from the client (where Led is running) computer will be used to provide a
			better approximation of the true elapsed time.</p>
*/
float	Led_GetTickCount ()
{
	#if		qMacOS
		return (float (::TickCount ()) / 60.0f);
	#elif	qWindows
		return (float (::GetTickCount ()) / 1000.0f);
	#elif	qXWindows
		static	float		sLastTickCountReturned;				// hack so we don't time-warp (out of sync between X client and could
																// cause time to go backwards cuz of our computations without this hack
		float	timeOfLastEvent	=	float (sLastXWindowsEventTime) / 1000.0f;
		double	curTime			=	GetThisMachineCurTime ();
		if (sLastEventReferenceTime == 0) {
			sLastEventReferenceTime = curTime;
		}
		double	resultTime		=	(timeOfLastEvent + (curTime - sLastEventReferenceTime));

		// avoid timewarp
		resultTime = Led_Max (resultTime, sLastTickCountReturned);
		sLastTickCountReturned = resultTime;
		return resultTime;
	#endif
}


#if		qXWindows
/*
@METHOD:		gBeepNotifyCallBackProc
@DESCRIPTION:	<p>X-Windows specific magic. See @'Led_BeepNotify'.</p>
*/
void	(*gBeepNotifyCallBackProc) ()	=	NULL;

/*
@METHOD:		SyncronizeLedXTickCount
@DESCRIPTION:	<p>X-Windows specific magic. See @'Led_GetTickCount'.</p>
*/
void	SyncronizeLedXTickCount (unsigned long xTickCount)
{
	sLastEventReferenceTime = GetThisMachineCurTime ();
	sLastXWindowsEventTime = xTickCount;
}

/*
@METHOD:		LedTickCount2XTime
@DESCRIPTION:	<p>X-Windows specific. See also @'SyncronizeLedXTickCount' and @'Led_GetTickCount'. Maps Led_GetTickCount ()
			result to the sort of time value you can stick into an XEvent record.</p>
*/
unsigned long	LedTickCount2XTime (float ledTickCount)
{
	return static_cast<unsigned long> (ledTickCount * 1000.0f);
}
#endif




/*
@METHOD:		Led_GetDoubleClickTime
@DESCRIPTION:	<p>Returns the amount of time (in seconds) between clicks which the OS deems should be interpretted
			as a double click.</p>
*/
float	Led_GetDoubleClickTime ()
{
	#if		qMacOS
		return (float (::GetDblTime ()) / 60.0f);
	#elif	qWindows
		return (float (::GetDoubleClickTime ()) / 1000.0f);
	#elif	qXWindows
		return 0.25f;	// SAME AS DOUBLE_CLICK_TIME FROM gdkevents.c
	#endif
}








/*
 ********************************************************************************
 ************************************ SPrintF ***********************************
 ********************************************************************************
 */

/*
@METHOD:		SPrintF
@DESCRIPTION:	<p>An @'Led_SDK_String' version of @'std::sprintf'. A slightly more convenient form
			of this function is the @'Format' function.
			</p>
*/
void	SPrintF (Led_SDK_Char* buffer, const Led_SDK_Char* format, ...)
{
	va_list								argsList;
	va_start (argsList, format);

	#if		qSDK_UNICODE
		(void)::vswprintf (buffer, format, argsList);
	#else
		(void)::vsprintf (buffer, format, argsList);
	#endif
	va_end (argsList);
}




/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */

/*
@METHOD:		Format
@DESCRIPTION:	<p>A simple wrapper on @'SPrintF' - but this function has a fixed size (10K Led_SDK_Char)
			buffer. Use @'SPrintF' directly if a larger buffer is required.</p>
*/
Led_SDK_String	Format (const Led_SDK_Char* format, ...)
{
	Led_SDK_Char				msgBuf [10*1024];	// no idea how big to make it...
	va_list						argsList;
	va_start (argsList, format);

	#if		qSDK_UNICODE
		(void)::vswprintf (msgBuf, format, argsList);
	#else
		(void)::vsprintf (msgBuf, format, argsList);
	#endif
	va_end (argsList);
	return msgBuf;
}






/*
 ********************************************************************************
 ******************************** EmitTraceMessage ******************************
 ********************************************************************************
 */
/*
@METHOD:		EmitTraceMessage
@DESCRIPTION:	<p>This function takes a 'format' argument and then any number of additional arguments - exactly
			like std::printf (). It calls std::vsprintf () internally. This can be called directly - regardless of the 
			 @'qDefaultTracingOn' flag - but is typically just called indirectly by calling
			 @'LedDebugTrace'.</p>
*/
void	EmitTraceMessage (const Led_SDK_Char* format, ...)
{
	Led_SmallStackBuffer<Led_SDK_Char>	msgBuf (10*1024);	// no idea how big to make it...
	va_list								argsList;
	va_start (argsList, format);

	#if		qSDK_UNICODE
		(void)::vswprintf (msgBuf, format, argsList);
	#else
		(void)::vsprintf (msgBuf, format, argsList);
	#endif
	#if		qWindows
		size_t	len	=	::_tcslen (msgBuf);
		if (msgBuf[len-1] != '\r' and msgBuf[len-1] != '\n') {
			(void)::_tcscat (msgBuf, Led_SDK_TCHAROF ("\r\n"));
		}
		::OutputDebugString (msgBuf);
	#else
		// SB pretty easy - but no need right now... LGP 2002-12-19
		Assert (false);	// NYI
	#endif
	va_end (argsList);
}






/*
 ********************************************************************************
 *********************** Led_ThrowOutOfMemoryException **************************
 ********************************************************************************
 */
static	void	(*sLedOutOfMemoryExceptionCallback) ()		=	NULL;

void	Led_ThrowOutOfMemoryException ()
{
	if (sLedOutOfMemoryExceptionCallback == NULL) {
		throw std::bad_alloc ();
	}
	else {
		(sLedOutOfMemoryExceptionCallback) ();
	}
	Assert (false);	// should never get here!
}

void	(*Led_Get_OutOfMemoryException_Handler ()) ()
{
	return sLedOutOfMemoryExceptionCallback;
}

void	Led_Set_OutOfMemoryException_Handler (void (*outOfMemoryHandler) ())
{
	sLedOutOfMemoryExceptionCallback = outOfMemoryHandler;
}





#if		!qNoSTRNICMP
int	Led_tStrniCmp (const Led_tChar* l, const Led_tChar* r, size_t n)
{
	RequireNotNull (l);
	RequireNotNull (r);
	#if		qSingleByteCharacters
		return ::strnicmp (l, r, n);
	#elif	qMultiByteCharacters
		return ::_mbsnicmp (l, r, n);
	#elif	qWideCharacters
		return ::wcsnicmp (l, r, n);
	#endif	
}

int	Led_tStriCmp (const Led_tChar* l, const Led_tChar* r)
{
	RequireNotNull (l);
	RequireNotNull (r);
	#if		qSingleByteCharacters
		return ::stricmp (l, r);
	#elif	qMultiByteCharacters
		return ::_mbsicmp (l, r);
	#elif	qWideCharacters
		return ::wcsicmp (l, r);
	#endif	
}
#endif




/*
 ********************************************************************************
 *********************** Led_ThrowBadFormatDataException ************************
 ********************************************************************************
 */
static	void	(*sLedBadFormatDataExceptionCallback) ()		=	NULL;

void	Led_ThrowBadFormatDataException ()
{
	if (sLedBadFormatDataExceptionCallback == NULL) {
		// not sure what this should throw by default?
		Led_ThrowOutOfMemoryException ();
	}
	else {
		(sLedBadFormatDataExceptionCallback) ();
	}
	Assert (false);	// should never get here!
}

void	(*Led_Get_BadFormatDataException_Handler ()) ()
{
	return sLedBadFormatDataExceptionCallback;
}

void	Led_Set_BadFormatDataException_Handler (void (*badFormatDataExceptionCallback) ())
{
	sLedBadFormatDataExceptionCallback = badFormatDataExceptionCallback;
}




#if		qMacOS
/*
 ********************************************************************************
 ***************************** Led_ThrowOSErr ***********************************
 ********************************************************************************
 */
static	void	(*sLedThrowOSErrExceptionCallback) (OSErr)		=	NULL;

void	Led_ThrowOSErr (OSErr err)
{
	if (err != noErr) {
		if (sLedThrowOSErrExceptionCallback == NULL) {
			throw err;
		}
		else {
			(sLedThrowOSErrExceptionCallback) (err);
		}
		Assert (false);	// should never get here!
	}
}

void	(*Led_Get_ThrowOSErrException_Handler ()) (OSErr err)
{
	return sLedThrowOSErrExceptionCallback;
}

void	Led_Set_ThrowOSErrException_Handler (void (*throwOSErrExceptionCallback) (OSErr err))
{
	sLedThrowOSErrExceptionCallback = throwOSErrExceptionCallback;
}
#endif








#if		qAllowBlockAllocation
	void*	Led_Block_Alloced_sizeof_4_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_8_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_12_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_16_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_20_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_24_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_28_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_32_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_36_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_40_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_44_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_48_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_52_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_56_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_60_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_64_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_68_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_72_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_76_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_80_sNextLink	=	NULL;
#endif



#if		qNoSTRNICMP
int	Led_tStrniCmp (const Led_tChar* lhs, const Led_tChar* rhs, size_t n)
{
	RequireNotNull (lhs);
	RequireNotNull (rhs);
	#if		qSingleByteCharacters || qWideCharacters
		if (n) {
			int f;
			int l;
			do {
				#if		qSingleByteCharacters
					if ( ((f = (unsigned short)(*(lhs++))) >= 'A') && (f <= 'Z') ) {
						f -= 'A' - 'a';
					}
					if ( ((l = (unsigned short)(*(rhs++))) >= 'A') && (l <= 'Z') ) {
						l -= 'A' - 'a';
					}
				#elif	qWideCharacters
					if ( ((f = (unsigned char)(*(lhs++))) >= 'A') && (f <= 'Z') ) {
						f -= 'A' - 'a';
					}
					if ( ((l = (unsigned char)(*(rhs++))) >= 'A') && (l <= 'Z') ) {
						l -= 'A' - 'a';
					}
				#endif
			}
			while ( --n && f && (f == l) );
			return (f - l);
		}
		return (0);		
	#else
		Assert (false);		//NYI
	#endif	
}

int	Led_tStriCmp (const Led_tChar* lhs, const Led_tChar* rhs)
{
	RequireNotNull (lhs);
	RequireNotNull (rhs);
	#if		qSingleByteCharacters || qWideCharacters
		int f;
		int l;
		do {
			#if		qSingleByteCharacters
				if ( ((f = (unsigned short)(*(lhs++))) >= 'A') && (f <= 'Z') ) {
					f -= 'A' - 'a';
				}
				if ( ((l = (unsigned short)(*(rhs++))) >= 'A') && (l <= 'Z') ) {
					l -= 'A' - 'a';
				}
			#elif	qWideCharacters
				if ( ((f = (unsigned char)(*(lhs++))) >= 'A') && (f <= 'Z') ) {
					f -= 'A' - 'a';
				}
				if ( ((l = (unsigned char)(*(rhs++))) >= 'A') && (l <= 'Z') ) {
					l -= 'A' - 'a';
				}
			#endif
		}
		while (f && (f == l) );
		return (f - l);
	#else
		Assert (false);		//NYI
	#endif	
}
#endif



size_t	Led_NativeToNL (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize)
{
	Led_tChar*	outPtr	=	outBuf;
	for (size_t i = 1; i <= srcTextBytes; i++) {
		#if		qMacOS
			Led_tChar	c	 = (srcText[i-1] == '\r')? '\n': srcText[i-1];
		#elif	qWindows
			Led_tChar	c	 = srcText[i-1];
			if (c == '\r') {
				// peek at next character - and if we have a CRLF sequence - then advance pointer
				// (so we skip next NL) and pretend this was an NL..
				// NB: we DONT map plain CR to NL - to get that to happen - use Led_NormalizeTextToNL()
				if (i < srcTextBytes and srcText[i] == '\n') {
					c = '\n';
					i++;
				}
			}
		#elif	qXWindows
			Led_tChar	c	 = srcText[i-1];
		#endif
		*outPtr++ = c;
	}
	size_t	nBytes	=	outPtr - outBuf;
	Assert (nBytes <= outBufSize);
	Led_Arg_Unused (outBufSize);
	return (nBytes);
}

size_t	Led_NLToNative (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize)
{
	Require (srcText != outBuf);	// though we support this for the others - its too hard
										// in this case for the PC...
	Led_tChar*	outPtr	=	outBuf;
	for (size_t i = 1; i <= srcTextBytes; i++) {
		Assert (outPtr < outBuf+outBufSize);
		#if		qMacOS
			Led_tChar	c	 = (srcText[i-1] == '\n')? '\r': srcText[i-1];
		#elif	qWindows
			Led_tChar	c	 = srcText[i-1];
			if (c == '\n') {
				*outPtr++ = '\r';
			}
		#elif	qXWindows
			Led_tChar	c	 = srcText[i-1];
		#endif
		*outPtr++ = c;
	}
	size_t	nBytes	=	outPtr - outBuf;
	Assert (nBytes <= outBufSize);
	Led_Arg_Unused (outBufSize);
	return (nBytes);
}

// return #bytes in output buffer (NO NULL TERM) - assert buffer big enough - output buf as big is input buf
// always big enough!!!
size_t	Led_NormalizeTextToNL (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize)
{
	// NB: We DO Support the case where srcText == outBuf!!!!
	Led_tChar*	outPtr	=	outBuf;
	for (size_t i = 0; i < srcTextBytes; i++) {
		Assert (outPtr < outBuf+outBufSize);
		Led_tChar	c	=	srcText[i];
		if (c == '\r') {
			// peek at next character - and if we have a CRLF sequence - then advance pointer
			// (so we skip next NL) and pretend this was an NL..
			if (i+1 < srcTextBytes and srcText[i+1] == '\n') {
				i++;
			}
			c = '\n';
		}
		*outPtr++ = c;
	}
	size_t	nBytes	=	outPtr - outBuf;
	Assert (nBytes <= outBufSize);
	Led_Arg_Unused (outBufSize);
	return (nBytes);
}

/*
@METHOD:		Led_SkrunchOutSpecialChars
@DESCRIPTION:	<p>This function is useful to remove embedded NUL-characters from a string, or
	any other such undesirable characters. Returns NEW length of string (after removals).
	Removes in place.</p>
*/
size_t	Led_SkrunchOutSpecialChars (Led_tChar* text, size_t textLen, Led_tChar charToRemove)
{
	size_t		charsSkipped	=	0;
	Led_tChar*	end				=	text + textLen;
	for (Led_tChar* p = text; p+charsSkipped < end; ) {
		if (*(p + charsSkipped) == charToRemove) {
			charsSkipped++;
			continue;
		}
		*p = *(p + charsSkipped);
		++p;
	}
	return textLen-charsSkipped;
}



#if		qMultiByteCharacters
bool	Led_IsValidMultiByteString (const Led_tChar* start, size_t len)
{
	AssertNotNull (start);
	const	Led_tChar*	end	=	&start[len];
	const Led_tChar* cur = start;
	for (; cur < end; cur = Led_NextChar (cur)) {
		if (Led_IsLeadByte (*cur)) {
			if (cur+1 == end) {
				return false;				// string ends on LedByte!!!
			}
			if (not Led_IsValidSecondByte (*(cur + 1))) {
				return false;				// byte second byte after lead-byte
			}
		}
		else if (not Led_IsValidSingleByte (*cur)) {
			return false;
		}
	}
	return (cur == end);	// if it were less - we'd be in the loop, and if it were
							// more - we'd have ended on a lead-byte
}
#endif









/*
 ********************************************************************************
 ************************** Led_ClipboardObjectAcquire **************************
 ********************************************************************************
 */

Led_ClipboardObjectAcquire::Led_ClipboardObjectAcquire (Led_ClipFormat clipType):
	#if		qMacOS || qWindows
	fOSClipHandle (NULL),
	#endif
	fLockedData (NULL)
{
	#if		qMacOS
		#if		TARGET_CARBON
			ScrapRef			scrap	=	NULL;
			Led_ThrowIfOSStatus (::GetCurrentScrap (&scrap));
			SInt32		byteCount	=	0;
			OSStatus	status		=	::GetScrapFlavorSize (scrap, clipType, &byteCount);
			if (status != noTypeErr) {
				fOSClipHandle			=	Led_DoNewHandle (byteCount);
				Led_ThrowIfNull (fOSClipHandle);
				Assert (::GetHandleSize (fOSClipHandle) == byteCount);
				::HLock (fOSClipHandle);
				fLockedData = *fOSClipHandle;
				Led_ThrowIfOSStatus (::GetScrapFlavorData (scrap, clipType, &byteCount, fLockedData));	
			}
		#else
			long	scrapOffset	=	0;
			fOSClipHandle			=	Led_DoNewHandle (0);
			Led_ThrowIfNull (fOSClipHandle);
			long	result		=	::GetScrap (fOSClipHandle, clipType, &scrapOffset);
			if (result < 0) {
				::DisposeHandle (fOSClipHandle);
				fOSClipHandle = NULL;
				return;
			}
			Assert (::GetHandleSize (fOSClipHandle) == result);
			::HLock (fOSClipHandle);
			fLockedData = *fOSClipHandle;
		#endif
	#elif	qWindows
		// perhaps rewrite to use exceptions, but for now - when no cliptype avail - set flag so GoodClip() method can check -
		// just cuz thats what surounding code seems to expect - LGP 980617
		fOSClipHandle	=	::GetClipboardData (clipType);
		#if		0
			DWORD	errResult	=	::GetLastError ();	// could be helpful for debugging if above fails... - Maybe I should do ThrowIfErrorHRESULT().... - LGP 2000/04/26
		#endif
		if (fOSClipHandle != NULL) {
			fLockedData = ::GlobalLock (fOSClipHandle);
		}
	#endif
}






#if		qWindows
/*
 ********************************************************************************
 **************************** VariantArrayPacker ********************************
 ********************************************************************************
 */
VariantArrayPacker::VariantArrayPacker (VARIANT* v, VARTYPE vt, size_t nElts):
	fSafeArrayVariant (v),
	fPtr (NULL)
{
	RequireNotNull (v);
	// assumes passed in variant is CONSTRUCTED (initied) - but not necesarily having the right type
	::VariantClear (fSafeArrayVariant);
	fSafeArrayVariant->parray = ::SafeArrayCreateVector (vt, 0, static_cast<ULONG> (nElts));
	Led_ThrowIfNull (fSafeArrayVariant->parray);
	fSafeArrayVariant->vt = VT_ARRAY | vt;
	Led_ThrowIfErrorHRESULT (::SafeArrayAccessData (fSafeArrayVariant->parray, &fPtr));
}

VariantArrayPacker::~VariantArrayPacker ()
{
	AssertNotNull (fSafeArrayVariant);
	AssertNotNull (fSafeArrayVariant->parray);
	::SafeArrayUnaccessData (fSafeArrayVariant->parray);
}

void*	VariantArrayPacker::PokeAtData () const
{
	return fPtr;
}






/*
 ********************************************************************************
 ***************************** VariantArrayUnpacker *****************************
 ********************************************************************************
 */
VariantArrayUnpacker::VariantArrayUnpacker (const VARIANT& v):
	fSafeArray (v.parray)
{
	if (not (v.vt & VT_ARRAY) or fSafeArray == NULL) {
		throw E_INVALIDARG;
	}
	if ((v.vt & ~VT_ARRAY) != GetArrayElementType ()) {
		// I THINK this is right??? - LGP 2003-06-12 - but I'm not sure this is much reason to check/??
		throw E_INVALIDARG;
	}
	void*	useP	=	NULL;
	Led_ThrowIfErrorHRESULT (::SafeArrayAccessData (fSafeArray, &useP));
	fPtr = useP;
}

VariantArrayUnpacker::~VariantArrayUnpacker ()
{
	AssertNotNull (fSafeArray);
	::SafeArrayUnaccessData (fSafeArray);
}

const void*		VariantArrayUnpacker::PeekAtData () const
{
	return fPtr;
}

VARTYPE		VariantArrayUnpacker::GetArrayElementType () const
{
	AssertNotNull (fSafeArray);
	VARTYPE	vt	=	VT_EMPTY;
	Led_ThrowIfErrorHRESULT (::SafeArrayGetVartype (fSafeArray, &vt));
	return vt;
}

size_t		VariantArrayUnpacker::GetLength () const
{
	AssertNotNull (fSafeArray);
	long	lb	=	0;
	long	ub	=	0;
	Led_ThrowIfErrorHRESULT (::SafeArrayGetLBound (fSafeArray, 1, &lb));
	Led_ThrowIfErrorHRESULT (::SafeArrayGetUBound (fSafeArray, 1, &ub));
	return ub-lb+1;
}
#endif









#if		qWindows
/*
 ********************************************************************************
 **************************** CreateSafeArrayOfBSTR *****************************
 ********************************************************************************
 */

/*
@METHOD:		CreateSafeArrayOfBSTR
@DESCRIPTION:	<p>Overloaded method to create a VARIANT record containing a safe-array of BSTRs.</p>
*/
VARIANT	CreateSafeArrayOfBSTR (const wchar_t*const* strsStart, const wchar_t*const* strsEnd)
{
	size_t	nElts	=	strsEnd - strsStart;
	VARIANT	result;
	::VariantInit (&result);
	try {
		const	bool	kEncodeAsVariant	=	false;	// neither seems to work with javascript test code in MSIE, but both work
														// with my C++-based unpacking code...
		if (kEncodeAsVariant) {
			VariantArrayPacker	packer (&result, VT_VARIANT, nElts);
			VARIANT*	vptr	=	reinterpret_cast<VARIANT*> (packer.PokeAtData ());
			for (size_t i = 0; i < nElts; ++i) {
				::VariantInit (&vptr[i]);
				Led_ThrowIfNull (vptr[i].bstrVal = ::SysAllocString (strsStart[i]));
				vptr[i].vt = VT_VARIANT;
			}
		}
		else {
			VariantArrayPacker	packer (&result, VT_BSTR, nElts);
			BSTR*	bptr	=	reinterpret_cast<BSTR*> (packer.PokeAtData ());
			for (size_t i = 0; i < nElts; ++i) {
				Led_ThrowIfNull (bptr[i] = ::SysAllocString (strsStart[i]));
			}
		}
	}
	catch (...) {
		::VariantClear (&result);
		throw;
	}
	return result;
}

VARIANT	CreateSafeArrayOfBSTR (const vector<const wchar_t*>& v)
{
	return CreateSafeArrayOfBSTR (&*v.begin (), &*v.end ());
}

VARIANT	CreateSafeArrayOfBSTR (const vector<wstring>& v)
{
	vector<const wchar_t*>	tmp;
	for (vector<wstring>::const_iterator i = v.begin (); i != v.end (); ++i) {
		// I hope (assure?) this doesn't create temporaries and take .c_str () of temporaries
		tmp.push_back ((*i).c_str ());
	}
	return CreateSafeArrayOfBSTR (tmp);
}



/*
 ********************************************************************************
 ***************** UnpackVectorOfStringsFromVariantArray ************************
 ********************************************************************************
 */
vector<wstring>	UnpackVectorOfStringsFromVariantArray (const VARIANT& v)
{
	vector<wstring>			result;
	VariantArrayUnpacker	up (v);
	size_t					nElts	=	up.GetLength ();
	switch (up.GetArrayElementType ()) {
		case	VT_BSTR: {
			const	BSTR*	bArray	=	reinterpret_cast<const BSTR*> (up.PeekAtData ());
			for (size_t i = 0; i < nElts; ++i) {
				result.push_back (bArray[i]);
			}
		}
		break;
		case	VT_VARIANT: {
			const	VARIANT*	vArray	=	reinterpret_cast<const VARIANT*> (up.PeekAtData ());
			for (size_t i = 0; i < nElts; ++i) {
				result.push_back (vArray[i].bstrVal);
			}
		}
		break;
		default:	throw (DISP_E_BADVARTYPE);
	}
	return result;
}
#endif


#if		qWindows
/*
@METHOD:		DumpSupportedInterfaces
@DESCRIPTION:	<p>@'qWindows' only</p>
				<p>Helpful COM debugging utility which dumps to the debugger window all the interfaces
			supported by a given COM object. The arguments 'objectName' and 'levelPrefix' can be NULL (optional).
			</p>
*/
void	DumpSupportedInterfaces (IUnknown* obj, const char* objectName, const char* levelPrefix)
{
	{
		size_t						labelLen	=	((objectName == NULL)? 0: ::strlen (objectName)) + 100;
		Led_SmallStackBuffer<char>	labelBuf (labelLen);
		if (objectName == NULL) {
			labelBuf[0] = '\0';
		}
		else {
			(void)::sprintf (labelBuf, " (named '%s')", objectName);
		}
		Led_SmallStackBuffer<char>	msgBuf (labelLen + 1000);
		(void)::sprintf (msgBuf, "Dumping interfaces for object%s at 0x%x:\n", static_cast<char*> (labelBuf), reinterpret_cast<int> (obj));
		::OutputDebugStringA (msgBuf);

		if (levelPrefix == NULL) {
			levelPrefix = "\t";
		}
	}

	/*
	 *	All interfaces are stored in the registry. Lookup each one, and do a query-interface
	 *	with each on the given object to see what interfaces it contains.
	 */
	HKEY		interfaceKey	=	NULL;
	HKEY		iKey			=	NULL;
	BSTR		tmpStr			=	NULL;
	IUnknown*	test			=	NULL;
	try {
		if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, _T ("Interface"), 0, KEY_READ, &interfaceKey) != ERROR_SUCCESS) {
			throw GetLastError ();
		}
		for (DWORD i = 0; ; ++i) {
			char	subKey[MAX_PATH];
			DWORD	subKeySize	=	sizeof (subKey);
			if (::RegEnumKeyExA (interfaceKey, i, subKey, &subKeySize, 0, 0, 0, 0) == ERROR_SUCCESS) {
				try {
					CLSID	iid;
					(void)::memset (&iid, 0, sizeof (iid));
					tmpStr = ::SysAllocString (ACP2WideString (subKey).c_str ());
					Led_ThrowIfErrorHRESULT (::CLSIDFromString (tmpStr, &iid));
					if (tmpStr != NULL)			{	::SysFreeString (tmpStr);		tmpStr = NULL;	}
					if (SUCCEEDED (obj->QueryInterface (iid, reinterpret_cast<void**> (&test)))) {
						// dump that obj interface
						Assert (iKey == NULL);
						if (::RegOpenKeyExA (interfaceKey, subKey, 0, KEY_READ, &iKey) != ERROR_SUCCESS) {
							throw GetLastError ();
						}
						DWORD	dwType = NULL;
						TCHAR	interfaceNameBuf[1024];
						DWORD	interfaceNameBufSize	=	sizeof (interfaceNameBuf);
						if (::RegQueryValueEx (iKey, _T(""), NULL, &dwType, reinterpret_cast<LPBYTE> (interfaceNameBuf), &interfaceNameBufSize) == ERROR_SUCCESS) {
							OutputDebugStringA (levelPrefix);
							OutputDebugString (interfaceNameBuf);
							OutputDebugStringA ("\n");
						}
						if (iKey != NULL)			{	::CloseHandle (iKey);			iKey = NULL;			}
					}
					if (test != NULL)	{		test->Release ();		test = NULL; }
				}
				catch (...) {
					OutputDebugStringA (levelPrefix);
					OutputDebugStringA ("ERROR LOOKING UP INTERFACE - IGNORING");
					OutputDebugStringA ("\n");
				}
			}
			else {
				break;	// end of list of interfaces?
			}
		}
		if (test != NULL)			{	test->Release ();				test = NULL;			}
		if (tmpStr != NULL)			{	::SysFreeString (tmpStr);		tmpStr = NULL;			}
		if (interfaceKey != NULL)	{	::CloseHandle (interfaceKey);	interfaceKey = NULL;	}
	}
	catch (...) {
		if (iKey != NULL)			{	::CloseHandle (iKey);			iKey = NULL;			}
		if (interfaceKey != NULL)	{	::CloseHandle (interfaceKey);	interfaceKey = NULL;	}
		if (tmpStr != NULL)			{	::SysFreeString (tmpStr);		tmpStr = NULL;			}		
		if (test != NULL)			{	test->Release ();				test = NULL;			}
	}
}

/*
@METHOD:		DumpObjectsInIterator
@DESCRIPTION:	<p>@'qWindows' only</p>
				<p>Helpful COM debugging utility which dumps to the debugger window all the subobjects of a given COM object,
			along with the interfaces they supoport (see also @'DumpSupportedInterfaces'). The arguments
			'iteratorName' and 'levelPrefix' can be NULL (optional).
			</p>
*/
void	DumpObjectsInIterator (IEnumUnknown* iter, const char* iteratorName, const char* levelPrefix)
{
	{
		size_t						labelLen	=	((iteratorName == NULL)? 0: ::strlen (iteratorName)) + 100;
		Led_SmallStackBuffer<char>	labelBuf (labelLen);
		if (iteratorName == NULL) {
			labelBuf[0] = '\0';
		}
		else {
			(void)::sprintf (labelBuf, " (named '%s')", iteratorName);
		}
		Led_SmallStackBuffer<char>	msgBuf (labelLen + 1000);
		(void)::sprintf (msgBuf, "Dumping objects (and their interface names) for iterator%s at 0x%x\n", static_cast<char*> (labelBuf), reinterpret_cast<int> (iter));
		::OutputDebugStringA (msgBuf);

		if (levelPrefix == NULL) {
			levelPrefix = "\t";
		}
	}
	if (iter == NULL) {
		::OutputDebugStringA (levelPrefix);
		::OutputDebugStringA ("<<NULL ENUM POINTER>>\n");	// try DumpSupportedInterfaces to see what interfaces your enum interface supports!
		return;
	}
	IUnknown*	nextObj	=	NULL;
	for (size_t i = 0; SUCCEEDED (iter->Next (1, &nextObj, NULL)); ++i) {
		char	nameBuf[1024];
		(void)::sprintf (nameBuf, "obj#%d", i);
		char	levelPrefixBuf[1024];
		Assert (::strlen (levelPrefix) < sizeof (levelPrefixBuf)/2);	// assert MUCH less
		strcpy (levelPrefixBuf, levelPrefix);
		strcat (levelPrefixBuf, "\t");
		DumpSupportedInterfaces (nextObj, nameBuf, levelPrefixBuf);
		AssertNotNull (nextObj);
		nextObj->Release ();
		nextObj = NULL;
	}
	Assert (nextObj == NULL);
}
#endif





/*
 ********************************************************************************
 *************************************** Led_URLD *******************************
 ********************************************************************************
 */
Led_URLD::Led_URLD (const char* url, const char* title):
	fData ()
{
	RequireNotNull (url);
	RequireNotNull (title);
	size_t	urlLen		=	::strlen (url);
	size_t	titleLen	=	::strlen (title);
	
	size_t	resultURLDLen = (urlLen + 1 + titleLen + 1);
	fData.resize (resultURLDLen, '\0');

	char*	data	=	&fData.front ();
	strcpy (data, url);
	strcat (data, "\r");
	strcat (data, title);
}

Led_URLD::Led_URLD (const void* urlData, size_t nBytes):
	fData ()
{
	// assure a validly formatted Led_URLD - regardless of input
	for (size_t i = 0; i < nBytes; i++) {
		char	c	=	((char*)urlData)[i];
		fData.push_back (c);
		if (c == '\0') {
			break;
		}
	}
	// make sure NUL-terminated...
	if (fData.size () == 0 or fData.back () != '\0') {
		fData.push_back ('\0');
	}
}

size_t	Led_URLD::GetURLDLength () const
{
	// Note we DONT count the terminating NUL - cuz Netscape 2.0 doesn't appear to. And thats our reference.
	return fData.size ()-1;
}

size_t	Led_URLD::GetURLLength () const
{
	size_t	len	=	fData.size ();
	for (size_t i = 0; i < len; i++) {
		if (fData[i] == '\r') {
			return i;
		}
	}
	return len-1;
}

size_t	Led_URLD::GetTitleLength () const
{
	size_t	len	=	fData.size ();
	for (size_t i = 0; i < len; i++) {
		if (fData[i] == '\r') {
			Assert (len >= (i+1+1));	// cuz must be NUL-term & skip \r
			return len-(i+1+1);				// ditto
		}
	}
	return 0;
}

char*	Led_URLD::PeekAtURLD () const
{
	return (&const_cast<Led_URLD*>(this)->fData.front ());
}

char*	Led_URLD::PeekAtURL () const
{
	return (&const_cast<Led_URLD*>(this)->fData.front ());
}

char*	Led_URLD::PeekAtTitle () const
{
	return PeekAtURL () + GetURLLength () + 1;	// skip URL and '\r'
}

string	Led_URLD::GetTitle () const
{
	return string (PeekAtTitle (), GetTitleLength ());
}

string	Led_URLD::GetURL () const
{
	return string (PeekAtURL (), GetURLLength ());
}





/*
 ********************************************************************************
 ********************************* Led_URLManager *******************************
 ********************************************************************************
 */
#if		qUseSpyglassDDESDIToOpenURLs
DWORD	Led_URLManager::sDDEMLInstance	=	0;	//	The DDEML instance identifier.
#endif
Led_URLManager*	Led_URLManager::sThe	=	NULL;

Led_URLManager::Led_URLManager ()
{
}

Led_URLManager&	Led_URLManager::Get ()
{
	if (sThe == NULL) {
		sThe = new Led_URLManager ();
	}
	return *sThe;
}

void	Led_URLManager::Set (Led_URLManager* newURLMgr)
{
	if (sThe != newURLMgr) {
		delete sThe;
		sThe = newURLMgr;
	}
}

void	Led_URLManager::Open (const string& url)
{
	#if		qUseInternetConfig
		try {
			Open_IC (url);
		}
		catch (...) {
			Open_SpyglassAppleEvent (url);
		}
	#elif	qMacOS
		Open_SpyglassAppleEvent (url);
	#elif	qWindows
		#if		qUseActiveXToOpenURLs && qUseSpyglassDDESDIToOpenURLs
			try {
				Open_ActiveX (url);
			}
			catch (...) {
				Open_SpyglassDDE (url);
			}
		#else
			#if		qUseActiveXToOpenURLs
				Open_ActiveX (url);
			#elif	qUseSpyglassDDESDIToOpenURLs
				Open_SpyglassDDE (url);
			#endif
		#endif
	#endif
	#if		qUseSystemNetscapeOpenURLs
		Open_SystemNetscape (url);
	#endif
}

#if		qMacOS
string	Led_URLManager::FileSpecToURL (const FSSpec& fsp)
{
	short	len	=	0;
	Handle	h	=	NULL;
	Led_ThrowOSErr (FSpGetFullPath (&fsp, &len, &h));
	AssertNotNull (h);
	const	char	kFilePrefix[]	=	"file:///";
	size_t			kFilePrefixLen	=	::strlen (kFilePrefix);
	char*	result	=	new char [kFilePrefixLen + len + 1];
	(void)::memcpy (result, kFilePrefix, kFilePrefixLen);
	(void)::memcpy (&result[kFilePrefixLen], *h, len);
	::DisposeHandle (h);
	result[kFilePrefixLen + len] = '\0';
	for (size_t i = 0; i < len; i++) {
		if (result[kFilePrefixLen + i] == ':') {
			result[kFilePrefixLen + i] = '/';
		}
	}
	string	r	=	result;
	delete[] result;

	// Now go through and quote illegal URL characters
	string	rr;
	for (size_t i = 0; i < r.length (); i++) {
		char	c	=	r[i];
		if (isalnum (c) or c == '/' or c == '.' or c == '_' or c == ':' or c == '#') {
			rr.append (&c, 1);
		}
		else {
			rr.append ("%");
			unsigned char	x	=	(((unsigned char)c) >> 4);
			Assert (x <= 0xf);
			x = (x>9)? (x - 10 + 'A'): (x + '0');
			rr.append ((char*)&x, 1);
			x	=	((unsigned char)(c & 0xf));
			Assert (x <= 0xf);
			x = (x>9)? (x - 10 + 'A'): (x + '0');
			rr.append ((char*)&x, 1);
		}
	}
	return rr;
}
#elif	qWindows
string	Led_URLManager::FileSpecToURL (const string& path)
{
	Assert (false);	// nyi (not needed anywhere right now)
	return "";
}
#endif

#if		qUseInternetConfig
void	Led_URLManager::Open_IC (const string& url)
{
	// If we compile for internet config, first try that. If that fails, fall back on Netscape.
	Str255	hint		=	"\p";	// not sure what this is for. See how it works without it...
	OSType	signature	=	'\?\?\?\?';
	{
		ProcessSerialNumber myPSN;
		memset (&myPSN, 0, sizeof (myPSN));
		Led_ThrowOSErr (::GetCurrentProcess (&myPSN));
		ProcessInfoRec info;
		memset (&info, 0, sizeof (info));
		info.processInfoLength = sizeof(info);
		Led_ThrowOSErr (::GetProcessInformation (&myPSN, &info));
		signature = info.processSignature;
	}
	ICInstance	icInstance;
	Led_ThrowOSErr (::ICStart (&icInstance, signature));
	#if		qSDKHasInternetConfig
	//	OSErr	err	=	::ICGeneralFindConfigFile (icInstance, true, true, 0, NULL);
		OSErr	err	=	noErr;
	#else
		ICError	err = ::ICFindConfigFile (icInstance, 0, NULL);
	#endif
	if (err == noErr) {
		long	urlStart	=	0;
		long	urlEnd		=	0;
		// Unclear if/why url would be modified, but since they declare it as non-cost
		// better be sure...LGP 961028
		char	urlBuf[1024];
		strncpy (urlBuf, url.c_str (), sizeof (urlBuf));
		urlBuf[sizeof(urlBuf)-1] = '\0';
		err = ::ICLaunchURL (icInstance, hint, urlBuf, ::strlen (urlBuf), &urlStart, &urlEnd);
	}
	::ICStop (icInstance);
	Led_ThrowOSErr (err);
}
#endif

#if		qMacOS
void	Led_URLManager::Open_SpyglassAppleEvent (const string& url)
{
	const	OSType	AE_spy_receive_suite	=	'WWW!';
	const	OSType	AE_spy_openURL			=	'OURL';	// typeChar OpenURL
	ProcessSerialNumber	browserPSN	=	FindBrowser ();
	AppleEvent event;
	{
		AEAddressDesc progressApp;
		Led_ThrowOSErr (::AECreateDesc (typeProcessSerialNumber, &browserPSN, sizeof(browserPSN), &progressApp));
		try {
			Led_ThrowOSErr (::AECreateAppleEvent (AE_spy_receive_suite, AE_spy_openURL, &progressApp, kAutoGenerateReturnID, kAnyTransactionID, &event));
			::AEDisposeDesc (&progressApp);
		}
		catch (...) {
			::AEDisposeDesc (&progressApp);
			throw;
		}
	}
	try {
		Led_ThrowOSErr (::AEPutParamPtr (&event,keyDirectObject,typeChar, url.c_str (), url.length ()));
		AppleEvent reply;
		Led_ThrowOSErr (::AESend (&event, &reply, kAEWaitReply, kAENormalPriority, kAEDefaultTimeout, nil, nil));
		::AEDisposeDesc (&event);
		::AEDisposeDesc (&reply);
	}
	catch (...) {
		::AEDisposeDesc (&event);
		throw;
	}
}
#endif

#if		qUseActiveXToOpenURLs
void	Led_URLManager::Open_ActiveX (const string& url)
{
	Led_SmallStackBuffer<wchar_t>	wideURLBuf (url.length ()+1);
	{
		int nWideChars = ::MultiByteToWideChar (CP_ACP, 0, url.c_str (), url.length (), wideURLBuf, url.length ());
		wideURLBuf[nWideChars] = '\0';
	}
	IUnknown*	pUnk	=	NULL;		// implies we are an OLE-ignorant app. But I'm not sure what else I'm supposed to pass here!

	/*
	 *	Use LoadLibrary/GetProcAddress instead of direct call to avoid having to link with
	 *	urlmon.lib. This avoidance allows us to run on systems that don't have MSIE installed.
	 *
	 *	Personally, I think its pretty weak MS doesn't offer a simpler way to deal with this problem.
	 *	(or if they do, their docs are weak).
	 *		--LGP 961015
	 */
	#if		1
		static	HINSTANCE	urlmonLibrary	=	::LoadLibrary (_T ("urlmon"));
		if (urlmonLibrary == NULL) {
			Led_ThrowOutOfMemoryException ();	// not sure what else to throw?
		}
		static	 HRESULT	 (WINAPI *hlinkNavigateString) (IUnknown*,LPCWSTR)	=
			(HRESULT  (WINAPI *) (IUnknown*,LPCWSTR))::GetProcAddress (urlmonLibrary, "HlinkNavigateString");
		if (hlinkNavigateString == NULL) {
			Led_ThrowOutOfMemoryException ();	// not sure what else to throw?
		}
		HRESULT		result	=	(*hlinkNavigateString) (pUnk, wideURLBuf);
	#else
		HRESULT		result	=	::HlinkNavigateString (pUnk, wideURLBuf);
	#endif
	if (not SUCCEEDED (result)) {
		throw result;
	}
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
void	Led_URLManager::Open_SpyglassDDE (const string& url)
{
	/*
	 *	Use DDE to talk to any browser supporting the Spyglass DDE SDI Suite (right now I only know of
	 *	Netscape, ironicly) - LGP 960502
	 */
	
	Require (sDDEMLInstance != 0);	//	Must call Led_URLManager::InitDDEHandler ()
										// (maybe we should throw? put up alert? LGP 960502)


	// WWW_OPENURL
	const	kDDETIMEOUT	=	2*1000;		// timeout in milliseconds
	{
		// Open Client connection
		HSZ		hszBrowser	=	NULL;	// connect to any browser
		HSZ		hszTopic	=	::DdeCreateStringHandle (sDDEMLInstance, _T ("WWW_OpenURL"), CP_WINANSI);
		HCONV	hConv		=	::DdeConnect (sDDEMLInstance, hszBrowser, hszTopic, NULL);
		::DdeFreeStringHandle (sDDEMLInstance, hszTopic);
	
		long	dwWindowID	=	-1;
		long	dwFlags		=	0;
		// Format DDE message
		HSZ	hszItem	=	ClientArguments ("QCS,QCS,DW,DW,QCS,QCS,CS", &url, NULL, &dwWindowID, &dwFlags, NULL, NULL, NULL);

		// Send DDE message
		//	Send along the item to the server, as an XTYP_REQUEST.
		//	Return the data received from the server.
		//	Proactively delete the hszItem string.
		HDDEDATA hRetVal = ::DdeClientTransaction (NULL, 0, hConv, hszItem, CF_TEXT, XTYP_REQUEST, kDDETIMEOUT, NULL);
		if(hszItem != NULL)	{
			::DdeFreeStringHandle (sDDEMLInstance, hszItem);
		}

		::DdeDisconnect (hConv);	
	}

	// WWW_ACTIVATE(activate the window/browser application)
	{
		// Open Client connection
		HSZ		hszBrowser	=	NULL;	// connect to any browser
		HSZ		hszTopic	=	::DdeCreateStringHandle (sDDEMLInstance, _T ("WWW_Activate"), CP_WINANSI);
		HCONV	hConv		=	::DdeConnect (sDDEMLInstance, hszBrowser, hszTopic, NULL);
		::DdeFreeStringHandle (sDDEMLInstance, hszTopic);

		long	dwWindowID	=	-1;
		long	dwFlags		=	0;
		// Format DDE message
		HSZ hszItem = ClientArguments ("DW,DW", &dwWindowID, &dwFlags);

		//	Send along the item to the server, as an XTYP_REQUEST.
		//	Return the data received from the server.
		//	Proactively delete the hszItem string.
		HDDEDATA hRetVal = DdeClientTransaction (NULL, 0, hConv, hszItem, CF_TEXT, XTYP_REQUEST, kDDETIMEOUT, NULL);
		if (hszItem != NULL)	{
			::DdeFreeStringHandle (sDDEMLInstance, hszItem);
		}

		::DdeDisconnect (hConv);		// Close connection
	}
}
#endif

#if		qUseSystemNetscapeOpenURLs
void	Led_URLManager::Open_SystemNetscape (const string& url)
{
	string	execString;
	/*
	 *	Code lifted (based on) code from AbiWord - xap_UnixFrame.cpp
	 */
	struct stat statbuf;
	memset (&statbuf, 0, sizeof (statbuf));
	//
	// The gnome-help-browser sucks right now. Instead open with netscape
	// or kde.
	// When it gets better we should restore this code.
	/*
	//if (!stat("/opt/gnome/bin/gnome-help-browser", statbuf) || !stat("/usr/local/bin/gnome-help-browser", statbuf) || !stat("/usr/bin/gnome-help-browser", statbuf))
	{
		execString = g_strdup_printf("gnome-help-browser %s &", szURL);
	}
	*/
#if 0
// My system has kdehelp - and I can exec it by itself - but passing args produces an error from KDE:
//		ERROR: KFM is not running
//		KFM not ready
	if (!stat("/opt/kde/bin/kdehelp", &statbuf) || !stat("/usr/local/kde/bin/kdehelp", &statbuf) || !stat("/usr/local/bin/kdehelp", &statbuf) || !stat("/usr/bin/kdehelp", &statbuf))
	{
		execString = "kdehelp " + url + "&";
	}
	else
#endif
	{
		// Try to connect to a running Netscape, if not, start new one
		Led_SmallStackBuffer<char> buf (url.length () * 2 + 1000);
		sprintf (buf, "netscape -remote openURL\\(\"%s\"\\) || netscape \"%s\" &", url.c_str (), url.c_str ());
		execString = buf;
	}
	system (execString.c_str ());
}
#endif

#if		qMacOS
pascal	OSErr	Led_URLManager::FSpGetFullPath (const FSSpec* spec, short* fullPathLength, Handle* fullPath)
{
	// Based on code from Apple Macintosh Developer Technical Support
	// "MoreFiles 1.4.2" example code
	OSErr		result;
	FSSpec		tempSpec;
	CInfoPBRec	pb;
	
	/* Make a copy of the input FSSpec that can be modified */
	BlockMoveData(spec, &tempSpec, sizeof(FSSpec));
	
	if ( tempSpec.parID == fsRtParID ) {
		/* The object is a volume */
		
		/* Add a colon to make it a full pathname */
		++tempSpec.name[0];
		tempSpec.name[tempSpec.name[0]] = ':';
		
		/* We're done */
		result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
	}
	else {
		/* The object isn't a volume */
		
		/* Put the object name in first */
		result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
		if ( result == noErr ) {
			/* Get the ancestor directory names */
			pb.dirInfo.ioNamePtr = tempSpec.name;
			pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
			pb.dirInfo.ioDrParID = tempSpec.parID;
			do	/* loop until we have an error or find the root directory */
			{
				pb.dirInfo.ioFDirIndex = -1;
				pb.dirInfo.ioDrDirID = pb.dirInfo.ioDrParID;
				result = PBGetCatInfoSync(&pb);
				if ( result == noErr )
				{
					/* Append colon to directory name */
					++tempSpec.name[0];
					tempSpec.name[tempSpec.name[0]] = ':';
					
					/* Add directory name to beginning of fullPath */
					(void) Munger(*fullPath, 0, NULL, 0, &tempSpec.name[1], tempSpec.name[0]);
					result = MemError();
				}
			} while ( (result == noErr) && (pb.dirInfo.ioDrDirID != fsRtDirID) );
		}
	}
	if ( result == noErr ) {
		/* Return the length */
		*fullPathLength = GetHandleSize(*fullPath);
	}
	else {
		/* Dispose of the handle and return NULL and zero length */
		DisposeHandle(*fullPath);
		*fullPath = NULL;
		*fullPathLength = 0;
	}
	
	return ( result );
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
void	Led_URLManager::InitDDEHandler ()
{
	//	Initialize DDEML
	Require (sDDEMLInstance == 0);	// only call once

	// callback not normally called, but seems to sometimes be called under error conditions, and we get crashes 
	// if no callback present (ie if I pass NULL to DDEInitialize()) - not when I call DDEInitialize(), but much
	// later - apparently when DDEML tries to deliver a message.
	if (::DdeInitialize (&sDDEMLInstance, SimpleDdeCallBack, APPCMD_CLIENTONLY, 0)) {
		sDDEMLInstance = 0;
	}
}
#endif

#if		qMacOS
ProcessSerialNumber	Led_URLManager::FindBrowser ()
{
	OSType	sig				=	'MOSS';	// currently we hardwire Netscape - but we could extend this list to include all known browsers...
	OSType	processType		=	'APPL';
	ProcessSerialNumber	psn;
	psn.highLongOfPSN = 0;
	psn.lowLongOfPSN  = kNoProcess;
	OSErr 			err;
	ProcessInfoRec	info;
	do {
		err= ::GetNextProcess (&psn);
		if ( err == noErr ) {
			info.processInfoLength = sizeof(ProcessInfoRec);
			info.processName = NULL;
			info.processAppSpec = NULL;
			err= ::GetProcessInformation (&psn, &info);
		}
	}
	while( (err == noErr) &&  ((info.processSignature != sig) ||  (info.processType != processType)));
	Led_ThrowOSErr (err);
	return info.processNumber;
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
HDDEDATA CALLBACK	Led_URLManager::SimpleDdeCallBack (UINT /*type*/, UINT /*fmt*/, HCONV /*hconv*/, HSZ /*hsz1*/, HSZ /*hsz2*/, HDDEDATA /*hData*/, DWORD /*dwData1*/, DWORD /*dwData2*/)
{
	return 0;
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
const char*	Led_URLManager::SkipToNextArgument (const char* pFormat)
{
	RequireNotNull (pFormat);
	if (*pFormat == '\0')	{
		return (pFormat);
	}

	//	The next format is directly after a ','
	while (*pFormat != ',' && *pFormat != '\0')	{
		pFormat++;
	}
	if (*pFormat == ',')	{
		pFormat++;
	}
	return (pFormat);
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
HSZ	Led_URLManager::ClientArguments (const char* pFormat, ...)
{
	RequireNotNull (pFormat);

	//	Always pass in pointer values.
	//	This way, a NULL, can be interpreted as an optional, missing parameter.
	const char*	pTraverse =	pFormat;	

	//	Variable number of arguments.
	va_list VarList;
	va_start(VarList, pFormat);
	char caNumpad[64];
	string	csBuffer;
	string	csRetval;
	while (*pTraverse != '\0')	{
		//	Erase temp data from our last pass.
		caNumpad[0] = '\0';
		csBuffer = string ();
		//	Compare our current format to the known formats
		if (0 == strncmp (pTraverse, "DW", 2))	{
			//	A DWORD.
			DWORD*	pWord = va_arg (VarList, DWORD *);
			if(pWord != NULL)	{
			    //  See if we need to use hex.
				sprintf (caNumpad, "%lu", *pWord);
				csRetval += caNumpad;
			}
		}
		else if (0 == strncmp (pTraverse, "CS", 2))	{
			//	A CString, not quoted
			string*	pCS = va_arg (VarList, string*);
			if (pCS != NULL)	{
				csRetval += *pCS;
			}
		}
		else if(0 == strncmp (pTraverse, "QCS", 3))	{
			//	A quoted CString
			string*	pQCS = va_arg (VarList, string*);
			if (pQCS != NULL)	{
				csRetval += '\"';
				//	Need to escape any '"' to '\"', literally.
				const char *pConvert = pQCS->c_str ();
				while(*pConvert != '\0')	{
					if(*pConvert == '\"')	{
						csRetval += '\\';
					}
					csRetval += *pConvert;
					pConvert++;
				}
				csRetval += '\"';
			}
		}
		else if (0 == strncmp (pTraverse, "BL", 2))	{
			//	If compiling in 32 bit mode, BOOLs change size.
			typedef short TwoByteBool;
			//	A boolean
			TwoByteBool *pBool = va_arg(VarList, TwoByteBool *);
			if (pBool != NULL)	{
				csRetval += *pBool? "TRUE": "FALSE";
			}
		}
		else	{
			//	Unhandled format, just get out of loop.
			Assert (false);
			break;
		}

		//	Go on to next type
		pTraverse = SkipToNextArgument (pTraverse);

		//	See if we need a comma
		if (*pTraverse != '\0')	{
			csRetval += ',';
		}
	}

	va_end (VarList);

	//	Make sure we're atleast returning something.
	if (csRetval.empty ())	{
		return (NULL);
	}

	//	Return our resultant HSZ, created from our string.
	HSZ Final = ::DdeCreateStringHandleA (sDDEMLInstance, csRetval.c_str (), CP_WINANSI);
	return(Final);
}

char*	Led_URLManager::ExtractArgument (HSZ hszArgs, int iArg)
{
	//	Quoted strings are counted as only one argument, though
	//		the quotes are not copied into the return string.

	DWORD dwLength = ::DdeQueryString (sDDEMLInstance, hszArgs, NULL, 0L, CP_WINANSI) + 1;
	char *pTraverse = new char[dwLength];
	char *pRemove = pTraverse;
	::DdeQueryStringA (sDDEMLInstance, hszArgs, pTraverse, dwLength, CP_WINANSI);
	
	//	safety dance
	if(*pTraverse == '\0' || iArg < 1)	{
		delete(pRemove);
		return(NULL);
	}
	
	//	Need to decrement the argument we're looking for, as the very
	//		first argument has no ',' at the beginning.
	iArg--;
	
	//	Search through the arguments, seperated by ','.
	while(iArg)	{
		//	Special handling of quoted strings.
		if(*pTraverse == '\"')	{
			//	Find the ending quote.
			while(*pTraverse != '\0')	{
				pTraverse++;
				if(*pTraverse == '\"')	{
					pTraverse++;	//	One beyond, please
					break;
				}
				else if(*pTraverse == '\\')	{
					//	Attempting to embed a quoted, perhaps....
					if(*(pTraverse + 1) == '\"')	{
						pTraverse++;
					}
				}
			}
		}
		while(*pTraverse != '\0' && *pTraverse != ',')	{
			pTraverse++;
		}

		//	Go beyond a comma
		if(*pTraverse == ',')	{
			pTraverse++;
		}
		
		iArg--;
		
		if(*pTraverse == '\0')	{
			break;
		}
	}
	
	//	Handle empty arguments here.
	if(*pTraverse == ',' || *pTraverse == '\0')	{
		delete(pRemove);
		return(NULL);
	}
	
	int iLength = 1;
	char *pCounter = pTraverse;
	typedef short TwoByteBool;
	TwoByteBool bQuoted = FALSE;
	
	//	specially handle quoted strings
	if(*pCounter == '\"')	{
		pCounter++;
		bQuoted = TRUE;

		while(*pCounter != '\0')	{
			if(*pCounter == '\"')	{
				break;
			}
			else if(*pCounter == '\\')	{
				if(*(pCounter + 1) == '\"')	{
					pCounter++;
					iLength++;
				}
			}
			
			pCounter++;
			iLength++;
		}
	}
	while(*pCounter != '\0' && *pCounter != ',')	{
		iLength++;
		pCounter++;
	}
	
	//	Subtrace one to ignore ending quote if we were quoted....
	if(bQuoted == TRUE)	{
		iLength--;
	}
	
	//	Argument's of length 1 are of no interest.
	if(iLength == 1)	{
		delete(pRemove);
		return(NULL);
	}
	
	char *pRetVal = new char[iLength];
	
	if(*pTraverse == '\"')	{
		pTraverse++;
	}	
	strncpy(pRetVal, pTraverse, iLength - 1);
	pRetVal[iLength - 1] = '\0';
	
	delete(pRemove);
	return(pRetVal);
}

void	Led_URLManager::ServerReturned (HDDEDATA hArgs, const char *pFormat, ...)
{
	//	Of course, only pointers should be passed in as the variable number of
	//		arguments so assignment can take place.
	//	hArgs is free'd off by this function.
	char *pDataArgs = (char *)DdeAccessData(hArgs, NULL);
	
	//	Initialize variable number of argumetns.
	va_list VarList;
	va_start(VarList, pFormat);	
	
	//	It will be possible that with only one argument,
	//		that we are receiving raw data intead of string notation.
	if(strchr(pFormat, ',') == NULL)	{
		//	Only one argument.
		if(strcmp(pFormat, "DW") == 0)	{
			DWORD *pWord;
			pWord = va_arg(VarList, DWORD *);
			*pWord = *(DWORD *)pDataArgs;
			DdeUnaccessData(hArgs);
			DdeFreeDataHandle(hArgs);
			va_end(VarList);
			return;
		}
		else if(strcmp(pFormat, "BL") == 0)	{
			typedef short TwoByteBool;
			TwoByteBool *pBool;
			pBool = va_arg(VarList, TwoByteBool *);
			*pBool = *(TwoByteBool *)pDataArgs;
			DdeUnaccessData(hArgs);
			DdeFreeDataHandle(hArgs);
			va_end(VarList);
			return;
		}
	}

	//	We are assuming NULL terminated data, since there is more than one
	//		parameter expected.
	HSZ hszArgs = DdeCreateStringHandleA (sDDEMLInstance, pDataArgs, CP_WINANSI);
	DdeUnaccessData(hArgs);	
	DdeFreeDataHandle(hArgs);

	int i_ArgNum = 0;
	const char *pScan = pFormat;
	char *pExtract;
	
	//	Loop through the arguments we are going to parse.
	while(pScan && *pScan)	{
		i_ArgNum++;
		pExtract = ExtractArgument(hszArgs, i_ArgNum);
	
		if(0 == strncmp(pScan, "DW", 2))	{
			//	DWORD.
			DWORD *pWord;
			pWord = va_arg(VarList, DWORD *);
			
			//	If there is nothing to scan, use a default value.
			if(pExtract == NULL)	{
				*pWord = 0x0;
			}
			else	{
				sscanf(pExtract, "%lu", pWord);
			}
		}
		else if(0 == strncmp(pScan, "QCS", 3))	{
			//	A quoted CString
			string*	pCS = va_arg(VarList, string*);
			
			if(pExtract == NULL)	{
				//pCS->Empty();
				*pCS = string ();
			}
			else	{
				//	Fun thing about a qouted string, is that we need
				//		to compress and '\"' into '"'.
				//	Extractions took off the leading and ending quotes.
				char *pCopy = pExtract;
				while(*pCopy)	{
					if(*pCopy == '\\' && *(pCopy + 1) == '\"')	{
						pCopy++;
					}
					
					*pCS += *pCopy;
					pCopy++;
				}
			}
		}
		else if(0 == strncmp(pScan, "CS", 2))	{
			//	A CString
			string*	pCS = va_arg (VarList, string*);

			if(pExtract == NULL)	{
				*pCS = string ();
			}
			else	{
				*pCS = pExtract;
			}
		}
		else if(0 == strncmp(pScan, "BL", 2))	{
			typedef short TwoByteBool;
			//	A boolean
			TwoByteBool *pBool = va_arg(VarList, TwoByteBool *);
			
			if(pExtract == NULL)	{
				*pBool = FALSE;
			}
			else	{
				//	Compare for a TRUE or a FALSE
				#if		defined (__MWERKS__)
					#define	stricmp	strcmp
					// TMP HACK - we don't build with MWERKS for the PC anyhow - just test compiles...
					// LGP 960617
				#endif
				if (0 == stricmp(pExtract, "TRUE"))	{
					*pBool = TRUE;
				}
				else	{
					*pBool = FALSE;
				}
			}
		}
		
		//	Go on to the next argument in our format string.
		pScan = SkipToNextArgument (pScan);
		
		//	Free the memory that was used during extraction.
		if (pExtract != NULL)	{
			delete pExtract;
		}
	}
	
	//	Done with variable number of arguments
	va_end(VarList);
	
	//	Free off our created HSZ string.
	DdeFreeStringHandle (sDDEMLInstance, hszArgs);
}
#endif









/*
 ********************************************************************************
 *************************** MakeSophistsAppNameVersionURL **********************
 ********************************************************************************
 */
string	MakeSophistsAppNameVersionURL (const string& relURL, const string& appName, const string& extraArgs)
{
	Require (relURL.length () > 0 and relURL[0] == '/');
	char	fullVersionBuf[1024];
	(void)::sprintf (fullVersionBuf, "%d", qLed_FullVersion);
	string	fullURL	=	"http://www.sophists.com" + relURL +
							"?AppName=" + appName +
							#if		qWindows
								string ("&Platform=Windows") +
							#elif	qMacOS
								string ("&Platform=MacOS") +
							#elif	qXWindows
								string ("&Platform=XWindows") +
							#endif
							"&MajorMinorVersion=" + qLed_MajorMinorVersionString +
							"&LedFullVersion=" + fullVersionBuf +
							"&ShortVersionString=" + qLed_ShortVersionString +
							extraArgs
				;
	return fullURL;
}











/*
 ********************************************************************************
 ******************************* Led_CasedCharsEqual ****************************
 ********************************************************************************
 */
bool	Led_CasedCharsEqual (char lhs, char rhs, bool ignoreCase)
{
	if (lhs == rhs) {
		return true;
	}
	if (ignoreCase and
		isascii (lhs) and isalpha (lhs) and
		isascii (rhs) and isalpha (rhs) and
		toupper (lhs) == toupper (rhs)
		) {
		return true;
	}
	return false;
}



/*
 ********************************************************************************
 ******************************* Led_CasedStringsEqual **************************
 ********************************************************************************
 */
bool	Led_CasedStringsEqual (const string& lhs, const string& rhs, bool ignoreCase)
{
	if (lhs.length () != rhs.length ()) {
		return false;
	}
	for (size_t i = 0; i < lhs.length (); i++) {
		if (not Led_CasedCharsEqual (lhs[i], rhs[i], ignoreCase)) {
			return false;
		}
	}
	return true;
}








/*
 ********************************************************************************
 ************************************* FileReader *******************************
 ********************************************************************************
 */
FileReader::FileReader (
						#if		qMacOS
							const FSSpec* fileName
						#elif	qWindows || qXWindows
							const Led_SDK_Char* fileName
						#endif
					)
{
	// OPEN
	#if		qMacOS
		short	fd	=	0;
		Led_ThrowOSErr (::FSpOpenDF (fileName, fsRdPerm, &fd));
	#elif	qWindows
		#if		qMustUseNoUnderbarForOpenFlagsCompilerBug
			int	fd	=	::_topen (fileName, O_RDONLY | O_BINARY);
		#else
			int	fd	=	::_topen (fileName, _O_RDONLY | _O_BINARY);
		#endif
		Led_ThrowIfFalseGetLastError (fd != -1);
	#elif	qXWindows
		int	fd	=	::open (fileName, O_RDONLY);
		if (fd == -1) {
			Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
		}
	#endif

	// Get LENGTH
	#if		qMacOS
		size_t	fileLen	= 0;
		try {
			long	logEOF;
			Led_ThrowOSErr (::GetEOF (fd, &logEOF));
			fileLen = logEOF;
		}
		catch (...) {
			::FSClose (fd);
			throw;
		}
	#elif	qWindows
		size_t	fileLen	= ::_lseek (fd, 0, SEEK_END);
	#elif	qXWindows
		size_t	fileLen	= ::lseek (fd, 0, SEEK_END);
	#endif

	Byte*	fileData	=	NULL;
	try {
		fileData = new Byte [fileLen];

		// READ IN DATA
		#if		qMacOS
			long	count	=	fileLen;
			OSErr	err		=	::FSRead (fd, &count, (char*)fileData);
			Led_ThrowOSErr ((err == eofErr)? noErr: err);
			if (count != fileLen) {
				Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
			}
		#elif	qWindows
			::_lseek (fd, 0, SEEK_SET);
			Led_ThrowIfFalseGetLastError (::_read (fd, fileData, fileLen) == int (fileLen));
		#elif	qXWindows
			// MUST ADD ERRCHECKING!
			::lseek (fd, 0, SEEK_SET);
			::read (fd, fileData, fileLen);
		#endif
	}
	catch (...) {
		#if		qMacOS
			::FSClose (fd);
		#elif	qWindows || qXWindows
			::close (fd);
		#endif
		delete[] fileData;
		throw;
	}

	// Close file, and update data pointers to stored file data...
	#if		qMacOS
		::FSClose (fd);
	#elif	qWindows || qXWindows
		::close (fd);
	#endif
	fFileDataStart = fileData;
	fFileDataEnd = fileData + fileLen;
}

FileReader::~FileReader ()
{
	#if		qDeleteOfPointerToConstantDataFailsOverloadError
		delete[] (char*)fFileDataStart;
	#else
		delete[] fFileDataStart;
	#endif
}




/*
 ********************************************************************************
 *********************************** FileWriter *********************************
 ********************************************************************************
 */
FileWriter::FileWriter (
						#if		qMacOS
							const FSSpec* fileName
						#elif	qWindows || qXWindows
							const Led_SDK_Char* fileName
						#endif
				):
			fFD (0)
{
	#if		qMacOS
		// Do I need to do someting special on MacOS to get the file created??? FSCreate???
		Led_ThrowOSErr (::FSpOpenDF (fileName, fsRdWrPerm, &fFD));
	#elif	qWindows
		#if		qMustUseNoUnderbarForOpenFlagsCompilerBug
			fFD =	::_topen (fileName, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666 );
		#else
			fFD	=	::_topen (fileName, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, 0666 );
		#endif
		Led_ThrowIfFalseGetLastError (fFD != -1);
	#elif	qXWindows
		fFD	=	::open (fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fFD == -1) {
			Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
		}
	#endif
}

FileWriter::~FileWriter ()
{
	#if		qMacOS
		::FSClose (fFD);
	#elif	qWindows || qXWindows
		::close (fFD);
	#endif
}

void	FileWriter::Append (const Byte* data, size_t count)
{
	#if		qMacOS
// UNTESTED - MUST MAKE SURE ARGS RIGHT!!! - LGP 2003-12-09
		long	countWritten	=	0;
		OSErr	err		=	::FSWrite (fFD, &countWritten, data);
		Led_ThrowOSErr ((err == eofErr)? noErr: err);
		if (count != countWritten) {
			Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
		}
	#elif	qWindows
		Led_ThrowIfFalseGetLastError (::_write (fFD, data, count) == int (count));
	#elif	qXWindows
		// MUST ADD ERRCHECKING!
		::write (fFD, data, count);
	#endif
}





#if		qLedCheckCompilerFlagsConsistency
	namespace LedCheckCompilerFlags {
		int	LedCheckCompilerFlags_(qDebug)					=	qDebug;
		int	LedCheckCompilerFlags_(qSingleByteCharacters)	=	qSingleByteCharacters;
		int	LedCheckCompilerFlags_(qMultiByteCharacters)	=	qMultiByteCharacters;
		int	LedCheckCompilerFlags_(qWideCharacters)			=	qWideCharacters;
		int	LedCheckCompilerFlags_(qProvideIMESupport)		=	qProvideIMESupport;
	}
#endif





		}
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<cstdio>
#include	<fstream>
#include	<map>
#include	<cmath>

#include	"../Configuration/StroikaConfig.h"


#include	"../Execution/CriticalSection.h"
#include	"../Memory/Basics.h"
#include	"../Time/Realtime.h"

#if		qTraceToFile
#include	"../IO/FileUtils.h"
#include	"../Time/DateUtils.h"
#endif


#include	"Trace.h"



using	namespace	Stroika::Foundation;

using	namespace	Characters;
using	namespace	Debug;
using	namespace	Execution;






/*
 ********************************************************************************
 ********************************** Private::MODULE_INIT ************************
 ********************************************************************************
 */
namespace	{
	static	CriticalSection*	sEmitTraceCritSec	=	NULL;
	#if		qTraceToFile
		static	ofstream*	sTraceFile	=	NULL;
	#endif
	typedef	DWORD	ThreadID;
	#if		qDefaultTracingOn
		static	map<ThreadID,unsigned int>*	sCounts;
	#endif
}


Private::MODULE_INIT::MODULE_INIT ()
{
	Assert (sEmitTraceCritSec == NULL);
	sEmitTraceCritSec = DEBUG_NEW CriticalSection ();
	#if		qDefaultTracingOn
		Assert (sCounts == NULL);
		sCounts = DEBUG_NEW map<ThreadID,unsigned int> ();
	#endif
	#if		qTraceToFile
		Assert (sTraceFile == NULL);
		sTraceFile = DEBUG_NEW ofstream ();
		sTraceFile->open (Emitter::Get ().GetTraceFileName ().c_str (), ios::out | ios::binary);
	#endif
}

Private::MODULE_INIT::~MODULE_INIT ()
{
	delete sEmitTraceCritSec;
	sEmitTraceCritSec = NULL;
	#if		qTraceToFile
		AssertNotNil (sTraceFile);
		sTraceFile->close ();
		delete sTraceFile;
	#endif
	#if		qDefaultTracingOn
		delete sCounts;
	#endif
}








namespace	{
	inline	CriticalSection&	GetCritSection_ ()
		{
			// this is a 'false' or 'apparent' memory leak, but we allocate the object this way because in C++ things
			// can be destroyed in any order, (across OBJs), and though this gets destroyed late, its still possible
			// someone might do a trace message.
			//		-- LGP 2008-12-21
			EnsureNotNil (sEmitTraceCritSec);
			return *sEmitTraceCritSec;
		}
}



#if		qTraceToFile
tstring	Emitter::GetTraceFileName () const
{
	static	tstring	sTraceFileName;
	if (sTraceFileName.empty ()) {
		// Use TempDir instead of EXEDir because on vista, installation permissions prevent us from (easily) writing in EXEDir.
		// (could fix of course, but I'm not sure desirable - reasonable defaults)
		//
		// Don't want to use TempFileLibrarian cuz we dont want these deleted on app exit
		tstring	mfname;
		{
			TCHAR	mfbuf[MAX_PATH];
			memset (mfbuf, 0, sizeof (mfbuf));
			Verify (::GetModuleFileName (NULL, mfbuf, NEltsOf (mfbuf)));
			mfname = mfbuf;
			size_t i = mfname.rfind ('\\');
			if (i != tstring::npos) {
				mfname = mfname.substr (i + 1);
			}
			i = mfname.rfind ('.');
			if (i != tstring::npos) {
				mfname.erase (i);
			}
			for (tstring::iterator i = mfname.begin (); i != mfname.end (); ++i) {
				if (*i == ' ') {
					*i = '-';
				}
			}
		}
		tstring nowstr	=	totstring (Time::DateTime::Now ().Format4XML ());
		for (tstring::iterator i = nowstr.begin (); i != nowstr.end (); ++i) {
			if (*i == ':') {
				*i = '-';
			}
		}
		sTraceFileName = IO::GetSpecialDir_GetTempDir () + Format (_T ("TraceLog_%s_%s.txt"), mfname.c_str (), nowstr.c_str ());
	}
	return sTraceFileName;
}
#endif


#if		qTraceToFile
namespace	{
	void	Emit2File_ (const char* text)
		{
			RequireNotNil (text);
			RequireNotNil (sTraceFile);
			try {
				if (sTraceFile->is_open ()) {
					(*sTraceFile) << text;
					sTraceFile->flush ();
				}
			}
			catch (...) {
				Assert (false);
			}
		}
	void	Emit2File_ (const wchar_t* text)
		{
			RequireNotNil (text);
			Emit2File_ (WideStringToUTF8 (text).c_str ());
		}
}
#endif




/*
 ********************************************************************************
 ************************************ Emitter ***********************************
 ********************************************************************************
 */
Emitter::Emitter ()
	: fLastNCharBufCharCount (0)
//	, fLastNCharBuf_CHAR () 
//	, fLastNCharBuf_WCHAR ()
	, fLastNCharBuf_WCHARFlag (false)
	, fLastNCharBuf_Token (0)
	, fLastNCharBuf_WriteTickcount (0.0f)
{
}

/*
@DESCRIPTION:	<p>This function takes a 'format' argument and then any number of additional arguments - exactly
			like std::printf (). It calls std::vsprintf () internally. This can be called directly - regardless of the 
			 @'qDefaultTracingOn' flag - but is typically just called indirectly by calling
			 @'LedDebugTrace'.</p>
*/
void	Emitter::EmitTraceMessage (const char* format, ...)
{
	try {
		char		msgBuf [4*1024];		// since we use safe version of printf (truncates) - we no longer need large buf
		va_list		argsList;
		va_start (argsList, format);
		#if		__STDC_WANT_SECURE_LIB__
			(void)::vsnprintf_s (msgBuf, NEltsOf (msgBuf), _TRUNCATE, format, argsList);
		#else
			(void)::vsnprintf (msgBuf, NEltsOf (msgBuf), format, argsList);
		#endif
		size_t	len	=	Characters::Length (msgBuf);
		if (msgBuf[len-1] != '\r' and msgBuf[len-1] != '\n' and len < NEltsOf (msgBuf) - 2) {
			(void)::strcat_s (msgBuf, "\r\n");
		}
		va_end (argsList);
		DoEmitMessage_ (0, msgBuf);
	}
	catch (...) {
		Assert (false);	// Should NEVER happen anymore becuase of new vsnprintf() stuff
		DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
	}
}

void	Emitter::EmitTraceMessage (const wchar_t* format, ...)
{
	try {
		wchar_t		msgBuf [4*1024];		// since we use safe version of printf (truncates) - we no longer need large buf
		va_list		argsList;
		va_start (argsList, format);

		#if		__STDC_WANT_SECURE_LIB__
			(void)::_vsnwprintf_s (msgBuf, NEltsOf (msgBuf), _TRUNCATE, format, argsList);
		#else
			(void)::vsnwprintf (msgBuf, NEltsOf (msgBuf), format, argsList);
		#endif
		size_t	len	=	Characters::Length (msgBuf);
		if (msgBuf[len-1] != '\r' and msgBuf[len-1] != '\n' and len < NEltsOf (msgBuf) - 2) {
			(void)::wcscat_s (msgBuf, L"\r\n");
		}
		va_end (argsList);
		DoEmitMessage_ (0, msgBuf);
	}
	catch (...) {
		Assert (false);	// Should NEVER happen anymore becuase of new vsnprintf() stuff
		DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
	}
}

Emitter::TraceLastBufferedWriteTokenType	Emitter::EmitTraceMessage (size_t bufferLastNChars, const char* format, ...)
{
	try {
		char		msgBuf [4*1024];		// since we use safe version of printf (truncates) - we no longer need large buf
		va_list		argsList;
		va_start (argsList, format);
		#if		__STDC_WANT_SECURE_LIB__
			(void)::vsnprintf_s (msgBuf, NEltsOf (msgBuf), _TRUNCATE, format, argsList);
		#else
			(void)::vsnprintf (msgBuf, NEltsOf (msgBuf), format, argsList);
		#endif
		size_t	len	=	Characters::Length (msgBuf);
		if (msgBuf[len-1] != '\r' and msgBuf[len-1] != '\n' and len < NEltsOf (msgBuf) - 2) {
			(void)::strcat_s (msgBuf, "\r\n");
		}
		va_end (argsList);
		return DoEmitMessage_ (bufferLastNChars, msgBuf);
	}
	catch (...) {
		Assert (false);	// Should NEVER happen anymore becuase of new vsnprintf() stuff
		DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
		return 0;
	}
}

Emitter::TraceLastBufferedWriteTokenType	Emitter::EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...)
{
	try {
		wchar_t		msgBuf [4*1024];		// since we use safe version of printf (truncates) - we no longer need large buf
		va_list		argsList;
		va_start (argsList, format);

		#if		__STDC_WANT_SECURE_LIB__
			(void)::_vsnwprintf_s (msgBuf, NEltsOf (msgBuf), _TRUNCATE, format, argsList);
		#else
			(void)::vsnwprintf (msgBuf, NEltsOf (msgBuf), format, argsList);
		#endif
		size_t	len	=	Characters::Length (msgBuf);
		if (msgBuf[len-1] != '\r' and msgBuf[len-1] != '\n' and len < NEltsOf (msgBuf) - 2) {
			(void)::wcscat_s (msgBuf, L"\r\n");
		}
		va_end (argsList);
		return DoEmitMessage_ (bufferLastNChars, msgBuf);
	}
	catch (...) {
		Assert (false);	// Should NEVER happen anymore becuase of new vsnprintf() stuff
		DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
		return 0;
	}
}

namespace	{
	// Declared HERE instead of the template so they get shared across TYPE values for CHARTYPE
	static	bool		sFirstTime	=	true;
	static	ThreadID	sMainThread;
}

template	<typename	CHARTYPE>
	Emitter::TraceLastBufferedWriteTokenType	Emitter::DoEmitMessage_ (size_t bufferLastNChars, CHARTYPE* p)
		{
			AutoCriticalSection critSec (GetCritSection_ ());
			FlushBufferedCharacters_ ();
			static	Time::TickCountType	sStartOfTime	=	0.0;
			if (sStartOfTime == 0.0) {
				sStartOfTime = Time::GetTickCount ();
			}
			Time::TickCountType	curRelativeTime	=	Time::GetTickCount () - sStartOfTime;
			{
				char	buf[1024];
				ThreadID	threadID	=	::GetCurrentThreadId ();
				if (sFirstTime) {
					sMainThread = threadID;
				}
				if (sMainThread == threadID) {
					::sprintf_s  (buf, "[-MAIN-][%08.3f]\t", curRelativeTime);
					if (sFirstTime) {
						sFirstTime = false;
						char buf2[1024];
						if (threadID <= 0xffff) {
							::sprintf_s  (buf2, "(REAL THREADID=0x%04x)\t", threadID);
						}
						else {
							::sprintf_s  (buf2, "(REAL THREADID=0x%08x)\t", threadID);
						}
						strcat_s (buf, buf2);
					}
				}
				else if (threadID <= 0xffff) {
					// it appears these IDs are < 16bits, so making the printout format shorter makes it a bit more readable.
					::sprintf_s  (buf, "[0x%04x][%08.3f]\t", threadID, curRelativeTime);
				}
				else {
					::sprintf_s  (buf, "[0x%08x][%08.3f]\t", threadID, curRelativeTime);
				}
				DoEmit_ (buf);
			}
			#if		qDefaultTracingOn
				unsigned int	contextDepth	=	TraceContextBumper::GetCount ();
				for (unsigned int i = 0; i < contextDepth; ++i) {
					DoEmit_ (L"\t");
				}
			#endif
			if (bufferLastNChars == 0) {
				DoEmit_ (p);
				fLastNCharBuf_Token++;	// even if not buffering, increment, so other buffers known to be invalid
			}
			else {
				Assert (Characters::Length (p) > bufferLastNChars);
				BufferNChars_ (bufferLastNChars, p + Characters::Length (p) - bufferLastNChars);
				p[Characters::Length (p) - bufferLastNChars] = '\0';
				DoEmit_ (p);
				fLastNCharBuf_WriteTickcount = curRelativeTime + sStartOfTime;
				fLastNCharBuf_Token++;	// even if not buffering, increment, so other buffers known to be invalid
			}
			return fLastNCharBuf_Token;
		}

void	Emitter::BufferNChars_ (size_t bufferLastNChars, const char* p)
{
	Assert (bufferLastNChars < NEltsOf (fLastNCharBuf_CHAR));
	fLastNCharBufCharCount = bufferLastNChars;
	strcpy_s (fLastNCharBuf_CHAR, p);
	fLastNCharBuf_WCHARFlag = false;
}

void	Emitter::BufferNChars_ (size_t bufferLastNChars, const wchar_t* p)
{
	Assert (bufferLastNChars < NEltsOf (fLastNCharBuf_WCHAR));
	fLastNCharBufCharCount = bufferLastNChars;
	::wcscpy_s (fLastNCharBuf_WCHAR, p);
	fLastNCharBuf_WCHARFlag = true;
}

void	Emitter::FlushBufferedCharacters_ ()
{
	if (fLastNCharBufCharCount != 0) {
		if (fLastNCharBuf_WCHARFlag) {
			DoEmit_ (fLastNCharBuf_WCHAR);
		}
		else {
			DoEmit_ (fLastNCharBuf_CHAR);
		}
		fLastNCharBufCharCount = 0;
	}
}

bool	Emitter::UnputBufferedCharactersForMatchingToken (TraceLastBufferedWriteTokenType token)
{
	// If the fLastNCharBuf_Token matches (no new tokens written since the saved one) and the time
	// hasn't been too long (we currently write 1/100th second timestamp resolution).
	// then blank unput (ignore) buffered characters, and return true so caller knows to write
	// funky replacement for those characters.
	if (fLastNCharBuf_Token == token and (Time::GetTickCount () - fLastNCharBuf_WriteTickcount < 0.02f)) {
		fLastNCharBufCharCount = 0;
		return true;
	}
	return false;	// assume old behavior for now
}

void	Emitter::DoEmit_ (const char* p)
{
	::OutputDebugStringA (p);
	#if		qTraceToFile
		Emit2File_ (p);
	#endif
}

void	Emitter::DoEmit_ (const wchar_t* p)
{
	::OutputDebugStringW (p);
	#if		qTraceToFile
		Emit2File_ (p);
	#endif
}









/*
 ********************************************************************************
 ****************************** TraceContextBumper ******************************
 ********************************************************************************
 */
#if		qDefaultTracingOn
namespace	{
	// maintain a per-thread-id counter, so if we have multiple threads emitting stuff at the same time, we can more easily
	// see the nesting...
	//
	// However - we want to throw away any entries in the list with a ZERO count - since these are a
	// waste of memory (and appear to a leak detector as a memory leak)!!!
	//		-- LGP 2009-05-27
	inline	unsigned int	GetCount_ ()
		{
			ThreadID	threadID	=	::GetCurrentThreadId ();
			AutoCriticalSection critSec (GetCritSection_ ());
			map<ThreadID,unsigned int>::const_iterator	i	=	sCounts->find (threadID);
			if (i == sCounts->end ()) {
				return 0;
			}
			Assert (i != sCounts->end ());
			return i->second;
		}
	inline	void	IncCount_ ()
		{
			ThreadID	threadID	=	::GetCurrentThreadId ();
			AutoCriticalSection critSec (GetCritSection_ ());
			map<ThreadID,unsigned int>::iterator	i	=	sCounts->find (threadID);
			if (i == sCounts->end ()) {
				(void)sCounts->insert (map<ThreadID,unsigned int>::value_type (threadID, 1)).first;
			}
			else {
				Assert (i != sCounts->end ());
				i->second++;
			}
		}
	inline	void	DecrCount_ ()
		{
			ThreadID	threadID	=	::GetCurrentThreadId ();
			AutoCriticalSection critSec (GetCritSection_ ());
			map<ThreadID,unsigned int>::iterator	i	=	sCounts->find (threadID);
			Assert (i != sCounts->end ());
			i->second--;
			if (i->second == 0) {
				sCounts->erase (i);
			}
		}
}

unsigned int	TraceContextBumper::GetCount ()
{
	return GetCount_ ();
}
void	TraceContextBumper::IncCount ()
{
	IncCount_ ();
}
void	TraceContextBumper::DecrCount ()
{
	DecrCount_ ();
}
#endif





/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	__LedSupport_h__
#define	__LedSupport_h__	1

#include	"../../Foundation/StroikaPreComp.h"
#include	"../../Foundation/Configuration/Common.h"
#include	"../../Foundation/Debug/Assertions.h"

/*
@MODULE:	LedSupport
@DESCRIPTION:
		<p>Support defines, and utility functions that should usually be provided
	by some other class library (e.g. Stroika, TCL, MFC, OWL, etc. These
	defines should just thunk down to the appropriate class library defines -
	where possible.</p>
 */


#include	<climits>
#include	<cstdint>
#include	<new>
#include	<cstddef>
#include	<cstring>
#include	<functional>
#include	<string>
#include	<vector>

#include	"Led_Config.h"


#if		qMacOS
	#include	<LowMem.h>
	#include	<Scrap.h>
	#include	<Processes.h>		// for URL support
#elif	qWindows
	#include	<Windows.h>			//
	#include	<DDEML.h>			// really only needed if qUseSpyglassDDESDIToOpenURLs - but that define only set in LedConfig.h
	#include	<tchar.h>
	#include	<oaidl.h>			// for SAFEARRAY
	#if		CRTDBG_MAP_ALLOC
		#include	<stdlib.h>
		#include	<crtdbg.h>
		#define	CRTDBG_MAP_ALLOC_NEW	new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#endif
#elif	qXWindows
	#include	<X11/X.h>
	#include	<X11/Xatom.h>
#endif


using	namespace	std;

namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {


		using	Foundation::Byte;



/*
 **************** Compiler/Lib bug workarounds... **************
 */

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4786)		//	qQuiteAnnoyingDebugSymbolTruncationWarnings
	#pragma	warning (4 : 4800)		//	qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
	#pragma	warning (4 : 4250)		//	qQuiteAnnoyingDominanceWarnings
#endif



#if		qSTLTemplatesErroniouslyRequireOpLessCuzOfOverExpanding
	#define	STLOpLessDeclare_BWA(T)\
		public:\
			friend bool operator< (T,T)  {Assert (false); return false; /*notreached*/}
#else
	#define	STLOpLessDeclare_BWA(T)
#endif



#if		qSTLTemplatesErroniouslyRequireOpEqualsCuzOfOverExpanding
	#define	STLOpEqualDeclare_BWA(T)\
		public:\
			friend bool operator== (T,T)  {Assert (false); return false; /*notreached*/}
#else
	#define	STLOpEqualDeclare_BWA(T)
#endif





#if		qSTLTemplatesErroniouslyRequireDefCTORCuzOfOverExpanding
	#define	STLDefCTORDeclare_BWA(T)\
		public:\
			T() {Assert (false);/*notreached*/}
#else
	#define	STLDefCTORDeclare_BWA(T)
#endif



#if		qSTLwstringBugNeedsTypedef
	typedef	basic_string<wchar_t>	wstring;
#endif




/*
 *	The StandaredC++ mechanism of commenting out unused parameters isn't good enuf
 *	in the case where the parameters might be used conditionally. This hack is
 *	to shutup compiler warnings in those cases.
 */
#ifndef	Led_Arg_Unused
#define	Led_Arg_Unused(x)	((void) &x)
#endif



const	size_t	kBadIndex	=	size_t (-1);






/*
@CLASS:			Led_tChar
@DESCRIPTION:	<p>There are lots of different ways of encoding character sets. And it is
	a goal for Led to be able to support them all. The three major ways are wide characters,
	single-byte characters, and multibyte character strings.</p>
		<p><code>Led_tChar</code> is a basic type used throughout Led. In the
	case of single byte character systems it is a <code>char</code>, and this also corresponds
	to a character. In mutlibyte character systems, it is also of type <code>char</code>,
	but then it does NOT correspond to a character. Rather some consecutive sequence
	of <code>Led_tChar</code>s specify a character. And for wide character sets (e.g UNICODE),
	<code>Led_tChar</code> is of type <code>wchar_t</code>, and - like with single byte
	character sets - does contain a full character.</p>

		<p>Now simply supporting one of these would be easy. Support all three within the
	same code base requires a small amount of discipline. It requires that we never assume that
	<code> sizeof (char) == sizeof (Led_tChar)</code> - because that supposed equality <b>is not true</b>.
	And it requires that we not assume that every possible <code>Led_tChar</code> pointer 
	refers to a valid character beginning. Instead, we must use utility routines
	<code><a href=#Led_MutlibyteCharRoutines>Led_NextChar</a></code> and
	<code><a href=#Led_MutlibyteCharRoutines>Led_PreviousChar</a></code> to navigate through
	<code>Led_tChar*</code> strings.</p>

		<p>All of Led's APIs which refer to characters or bytes or any such thing <em>really</em> 
	refer to <code>Led_tChar</code>s (actually, typically <code>Led_tChar*</code>).
	All of Led's APIs gaurantee (unless otherwise noted) that they will never split a character 
	(return a character offset which points in the middle of a multibyte character).
	And all of Led's APIs similarly <em>require</em> that all offsets and strings passed to 
	it consist of valid character boundaries.
	You can always use (and Led does many asserts internally)
	<code>ValidateTextForCharsetConformance()</code> to check / validate a string to make
	sure it is valid with respect to the character set you are using (for example doesn't end
	spliting a multibyte character).</p>
*/
	#if		defined (__cplusplus)
		#if		qSingleByteCharacters
			typedef	char	Led_tChar;
		#elif	qMultiByteCharacters
			typedef	char	Led_tChar;
		#elif	qWideCharacters
			typedef	wchar_t	Led_tChar;
		#else
			#error	"One of these must be defined"
		#endif
	#endif



/*
@CLASS:			LED_TCHAR_OF
@DESCRIPTION:	<p>Like the Win32SDK macro _T(). Except is based on the Led type @'Led_tChar', and the Led
	macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
*/
	#define	LED_TCHAR_OF__(X)	L ## X
	#if		qSingleByteCharacters || qMultiByteCharacters
		#define	LED_TCHAR_OF(X)	X
	#elif	qWideCharacters
		#define	LED_TCHAR_OF(X)	LED_TCHAR_OF__(X)
	#endif	






#if		qWideCharacters
	const	wchar_t	kNonBreakingSpace	=	0x00a0;
	const	wchar_t	kZeroWidthSpace		=	0x200b;
	const	wchar_t	kPoundSign			=	0x00a3;
	const	wchar_t	kCentSign			=	0x00a2;
	const	wchar_t	kYenSign			=	0x00a5;
#endif



/*
@CLASS:			Led_tString
@DESCRIPTION:	<p>This expands to either the ANSI C++ string class library class <code>string</code>
	or <code>wstring</code> - depending on whether we are using UNICODE or not. It is a <code>basic_string</code>
	templated on @'Led_tChar'.</p>
*/
typedef		std::basic_string<Led_tChar>	Led_tString;



/*
@METHOD:		Led_tStrlen
@DESCRIPTION:	<p>Expands to any of the ANSI C++ functions, std::strlen ()/wcslen/_mbstrlen(not ansiC++), depending on 
	macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
*/
size_t	Led_tStrlen (const Led_tChar* s);




/*
@METHOD			Led_tStrCmp
@DESCRIPTION:	<p>Expands to any of the ANSI C++ functions, std::strcmp/etc, depending on 
	macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
*/
int	Led_tStrCmp (const Led_tChar* l, const Led_tChar* r);




/*
@METHOD:		Led_tStrnCmp
@DESCRIPTION:	<p>Expands to any of the ANSI C++ functions, std::strncmp/etc, depending on 
	macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
*/
int	Led_tStrnCmp (const Led_tChar* l, const Led_tChar* r, size_t n);



/*
@METHOD:		Led_tStrniCmp
@DESCRIPTION:	<p>Expands to any of the ANSI C++ functions, std::strnicmp/etc, depending on 
	macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'. See also @'Led_tStriCmp'.</p>
*/
int	Led_tStrniCmp (const Led_tChar* l, const Led_tChar* r, size_t n);



/*
@METHOD:		Led_tStriCmp
@DESCRIPTION:	<p>Expands to any of the ANSI C++ functions, std::stricmp/etc, depending on 
	macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'. See also @'Led_tStrniCmp'.</p>
*/
int	Led_tStriCmp (const Led_tChar* l, const Led_tChar* r);



/*
@METHOD:		Led_tStrChr
@DESCRIPTION:	<p>Expands to any of the ANSI C++ functions, std::strchr/etc, depending on 
	macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
*/
const Led_tChar*	Led_tStrChr (const Led_tChar* s, Led_tChar c);



/*
@CLASS:			Led_SDK_Char
@DESCRIPTION:	<p>See @'Led_SDK_String'</p>
*/
#if		qSDK_UNICODE
	typedef		wchar_t	Led_SDK_Char;
#else
	typedef		char	Led_SDK_Char;
#endif



/*
@CLASS:			Led_SDK_String
@DESCRIPTION:	<p>Expands to either the ANSI @'string' or the ANSI @'wstring' class, depending on the
	value of the configuration define @'qSDK_UNICODE'.</p>
		<p>This is rarely used - but is used for things like font names, and other system IO strings
	that may need to be in one format or another.</p>
*/
#if		qSDK_UNICODE
	typedef		wstring	Led_SDK_String;
#else
	typedef		string	Led_SDK_String;
#endif



/*
@CLASS:			Led_SDK_TCHAROF
@DESCRIPTION:	<p>Like the Win32SDK macro _T(). See @'Led_SDK_String'.</p>
*/
#if		qSDK_UNICODE
	#define	Led_SDK_TCHAROF(X)	LED_TCHAR_OF__(X)
#else
	#define	Led_SDK_TCHAROF(X)	X
#endif	




wstring	ACP2WideString (const string& s);
string	Wide2ACPString (const wstring& s);




/*
@METHOD:		Led_ANSI2SDKString
@DESCRIPTION:	
*/
Led_SDK_String	Led_ANSI2SDKString (const string& s);




/*
@METHOD:		Led_Wide2SDKString
@DESCRIPTION:	
*/
Led_SDK_String	Led_Wide2SDKString (const wstring& s);




/*
@METHOD:		Led_SDKString2ANSI
@DESCRIPTION:	
*/
string	Led_SDKString2ANSI (const Led_SDK_String& s);




/*
@METHOD:		Led_SDKString2Wide
@DESCRIPTION:	
*/
wstring	Led_SDKString2Wide (const Led_SDK_String& s);





/*
@METHOD:		Led_tString2SDKString
@DESCRIPTION:	
*/
Led_SDK_String	Led_tString2SDKString (const Led_tString& s);




/*
@METHOD:		Led_WideString2tString
@DESCRIPTION:	
*/
Led_tString	Led_WideString2tString (const wstring& s);




/*
@METHOD:		Led_SDKString2tString
@DESCRIPTION:	
*/
Led_tString	Led_SDKString2tString (const Led_SDK_String& s);



/*
@METHOD:		Led_ANSIString2tString
@DESCRIPTION:	
*/
Led_tString	Led_ANSIString2tString (const string& s);




/*
@METHOD:		Led_tString2ANSIString
@DESCRIPTION:	
*/
string	Led_tString2ANSIString (const Led_tString& s);


/*
@METHOD:		Led_tString2WideString
@DESCRIPTION:	
*/
wstring	Led_tString2WideString (const Led_tString& s);




void			SPrintF (Led_SDK_Char* buffer, const Led_SDK_Char* format, ...);

Led_SDK_String	Format (const Led_SDK_Char* format, ...);



void	EmitTraceMessage (const Led_SDK_Char* format, ...);


/*
@METHOD:		LedDebugTrace
@DESCRIPTION:	<p>This function either does NOTHING (trying to not even evaluate its arguments)
			or does a printf style PRINT function by delegating to @'EmitTraceMessage'. Which of
			these two behaviors you see is conditioned on @'qDefaultTracingOn'</p>
*/
#ifndef	LedDebugTrace
	#if		qDefaultTracingOn
		#define	LedDebugTrace	EmitTraceMessage
	#elif	qNoOpKeywordSupported
		#define	LedDebugTrace	__noop
	#else
		// not ideal - but the best portable solution I can think of???
		#define	LedDebugTrace
	#endif
#endif




/*
@METHOD:		Led_ThrowOutOfMemoryException
@DESCRIPTION:	<p>Even though Standard C++ defines how this SHOULD behave, many class libraries (e.g. MFC and
	PowerPlant) expect non-C++-Standard things to get thrown when you run out of memory.</p>
		<p>So - Led internally calls this routine for out of memory exceptions.</p>
		<p>The behavior of this routine detaults to the C++ standard, unless you are using some class library, like
	PowerPlant or MFC. Then those modules may call @'Led_Set_OutOfMemoryException_Handler'. You can call @'Led_Get_OutOfMemoryException_Handler' 
	or @'Led_Set_OutOfMemoryException_Handler' to get/set the handler.</p>
		<p>Note - this method never returns - it always throws.</p>
*/
void	Led_ThrowOutOfMemoryException ();

/*
@METHOD:		Led_Get_OutOfMemoryException_Handler
@DESCRIPTION:	<p>Get the handler used in @'Led_ThrowOutOfMemoryException'.</p>
*/
void	(*Led_Get_OutOfMemoryException_Handler ()) ();

/*
@METHOD:		Led_Set_OutOfMemoryException_Handler
@DESCRIPTION:	<p>Set the handler used in @'Led_ThrowOutOfMemoryException'.</p>
		<p>Note - any handler supplied must never return - it must always throw.</p>
*/
void	Led_Set_OutOfMemoryException_Handler (void (*outOfMemoryHandler) ());






/*
@METHOD:		Led_ThrowBadFormatDataException
@DESCRIPTION:	<p>This is called internally by Led (or your code) when some data is found to be in a bad format.</p>
		<p>Either your class library will call this to specify a class-lib specific exception type for this kind of
	error, or you can in your application. Or just leave it todo the default.</p>
		<p>Note - this method never returns - it always throws.</p>
		<p>See also @'Led_ThrowOutOfMemoryException'.</p>
*/
void	Led_ThrowBadFormatDataException ();

/*
@METHOD:		Led_Get_BadFormatDatException_Handler
@DESCRIPTION:	<p>Get the handler used in @'Led_ThrowBadFormatDataException'.</p>
*/
void	(*Led_Get_BadFormatDataException_Handler ()) ();

/*
@METHOD:		Led_Set_BadFormatDatException_Handler
@DESCRIPTION:	<p>Set the handler used in @'Led_ThrowBadFormatDataException'.</p>
		<p>Note - any handler supplied must never return - it must always throw.</p>
*/
void	Led_Set_BadFormatDataException_Handler (void (*badFormatDataExceptionCallback) ());












#if		qMacOS
/*
@METHOD:		Led_ThrowOSErr
@DESCRIPTION:	<p>This is called internally by Led (or your code) after MacOS system calls that return an OSErr.</p>
		<p>By default that error # is thrown - but you can override this behavior by calling @'Led_Set_ThrowOSErrException_Handler'.</p>
		<p>Note - this method never returns - it always throws, unless 'err' == 'noErr' - in which case it does nothing.</p>
*/
void	Led_ThrowOSErr (OSErr err);

/*
@METHOD:		Led_Get_ThrowOSErrException_Handler
@DESCRIPTION:	<p>Get the handler used in @'Led_ThrowBadFormatDataException'.</p>
*/
void	(*Led_Get_ThrowOSErrException_Handler ()) (OSErr err);

/*
@METHOD:		Led_Set_ThrowOSErrException_Handler
@DESCRIPTION:	<p>Set the handler used in @'Led_ThrowBadFormatDataException'.</p>
		<p>Note - any handler supplied must never return - it must always throw.</p>
*/
void	Led_Set_ThrowOSErrException_Handler (void (*throwOSErrExceptionCallback) (OSErr err));
#endif



#if		qWindows
class	Win32ErrorException {
	public:
		Win32ErrorException (DWORD error);

		operator DWORD () const;

	private:
		DWORD	fError;
};

class	HRESULTErrorException {
	public:
		HRESULTErrorException (HRESULT hresult);

		operator HRESULT () const;

	private:
		HRESULT	fHResult;
};
#endif


#if		qWindows
void	Led_ThrowIfFalseGetLastError (bool test);
void	Led_ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode);
#endif



#if		qWindows
/*
@METHOD:		Led_ThrowIfErrorHRESULT
@DESCRIPTION:	<p>If the HRESULT failed, then throw that HRESULT.</p>
*/
void	Led_ThrowIfErrorHRESULT (HRESULT hr);
#endif


#if		qMacOS
void	Led_ThrowIfOSErr (OSErr err);
void	Led_ThrowIfOSStatus (OSStatus err);
#endif


/*
@METHOD:		Led_ThrowIfNull
@DESCRIPTION:	<p>If p == NULL, then @'Led_ThrowOutOfMemoryException'.</p>
*/
void	Led_ThrowIfNull (void* p);





short			Led_ByteSwapFromMac (short src);
unsigned short	Led_ByteSwapFromMac (unsigned short src);

short			Led_ByteSwapFromWindows (short src);
unsigned short	Led_ByteSwapFromWindows (unsigned short src);
long			Led_ByteSwapFromWindows (long src);
unsigned long	Led_ByteSwapFromWindows (unsigned long src);

void			Led_USHORTToBuf (unsigned short u, unsigned short* realBuf);
unsigned short	Led_BufToUSHORT (const unsigned short* realBuf);
void			Led_ULONGToBuf (unsigned long ul, unsigned long* realBuf);
unsigned long	Led_BufToULONG (const unsigned long* buf);
void			Led_ULONGToBuf (unsigned int ul, unsigned int* realBuf);
unsigned int	Led_BufToULONG (const unsigned int* buf);





#define	Led_Min(lhs,rhs)	(((lhs)>(rhs))?(rhs):(lhs))
#define	Led_Max(lhs,rhs)	(((lhs)<(rhs))?(rhs):(lhs))
#define	Led_Abs(val)		(((val)<0)?(-(val)):(val))








// These functions do NOT null terminate.
// they both return the number of bytes in the target text based on conversion from
// the source text. They silently ignore and do the best they can filtering
// any bad characters out. They both ASSERT the dest buffer is big enough.
// 2* input buffer size is ALWAYS big enough (really only need bigger out
// on PC going from NL to native - same size good enuf in all other cases)
size_t	Led_NativeToNL (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize);
size_t	Led_NLToNative (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize);

// return #bytes in output buffer (NO NULL TERM) - assert buffer big enough - output buf as big is input buf
// always big enough!!!
// NB: This routine is written carefully so that srcText CAN EQUAL outBuf!!!
size_t	Led_NormalizeTextToNL (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize);


size_t	Led_SkrunchOutSpecialChars (Led_tChar* text, size_t textLen, Led_tChar charToRemove);





/*
@CLASS:			Led_BlockAllocated<T>
@DESCRIPTION:	<p><code>Led_BlockAllocated&ltT&gt</code> is a templated class designed to allow easy use
	of a block-allocated memory strategy. This means zero overhead malloc/memory allocation for fixed
	size blocks (with the only problem being the storage is never - or almost never - returned to the
	free store - it doesn't leak - but cannot be used for other things). This is often a useful
	tradeoff for things you allocate a great number of.</p>
		<p>You shouldn't disable it lightly. But you may wish to temporarily disable block-allocation
	while checking for memory leaks by shutting of the @'qAllowBlockAllocation' compile-time configuration variable.</p>
		<p>Note also - you can avoid some of the uglines of the overload declarations by using the 
	@'LED_DECLARE_USE_BLOCK_ALLOCATION' macro.</p>
*/
template	<typename	T>	class	Led_BlockAllocated {
	public:
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);

#if		qAllowBlockAllocation
	private:
	public:
		static	void	GetMem ();	// Break out into separate function so we can
									// make op new inline for MOST important case
									// were alloc is cheap linked list operation...

		static	void*	sNextLink;
		static	void*	GetNextLink ();
		static	void	SetNextLink (void* nextLink);
#endif
};


/*
@CLASS:			LED_DECLARE_USE_BLOCK_ALLOCATION
@DESCRIPTION:	<p>This helper macro can be used to avoid some of the C++ gorp required in declaring that you are
	using block-allocation with a given class.</p>
		<p>An example of use might be:
	<code>
	<pre>
		class	Foo {
			public:
				LED_DECLARE_USE_BLOCK_ALLOCATION(Foo);
		}
	</pre>
	</code>
		</p>
		<p>See also - @'LED_DECLARE_DONT_USE_BLOCK_ALLOCATION' and @'Led_BlockAllocated<T>'.</p>
*/
#if		qAllowBlockAllocation
	#define	LED_DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)\
			static	void*	operator new (size_t n)		{	return (Led_BlockAllocated<THIS_CLASS>::operator new (n));	}\
			static	void	operator delete (void* p)	{	Led_BlockAllocated<THIS_CLASS>::operator delete (p);		}
#else
	#define	LED_DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS)
#endif

/*
@CLASS:			LED_DECLARE_DONT_USE_BLOCK_ALLOCATION
@DESCRIPTION:	<p>If you subclass from a class which uses block-allication, you can use this to turn off block allocation
	in your particular subclass.</p>
		<p>See also @'LED_DECLARE_USE_BLOCK_ALLOCATION' and @'Led_BlockAllocated<T>'.</p>
*/
#if		qAllowBlockAllocation
	#define	LED_DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS)\
			static	void*	operator new (size_t n)		{	return ::operator new (n);	}\
			static	void	operator delete (void* p)	{	::operator delete (p);		}
#else
	#define	LED_DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS)
#endif




/*
@METHOD:		PUSH_BACK
@DESCRIPTION:	<p></p>
*/
template <class VECTOR>
	inline	void	PUSH_BACK (VECTOR& v, const typename VECTOR::value_type& e)
		{
			size_t	vSize	=	v.size ();
			size_t	vCap	=	v.capacity ();
			Assert (vSize <= vCap);
			if (vSize == vCap) {
				size_t	newCap	=	vSize * 2;
				newCap = max (newCap, size_t (4));
				v.reserve (newCap);	// grow by factor of 2 to assure logarithmic # of copies rather than quadradic
			}
			v.push_back (e);
		}





#if		qMacOS
	// throw if cannot do allocation. Use tmp memory if qUseMacTmpMemForAllocs.
	// fall back on application-heap-zone if no tmp memory
	Handle	Led_DoNewHandle (size_t n);
	void	Led_CheckSomeLocalHeapRAMAvailable (size_t n = 1024);	// only to avoid MacOS crashes on toolbox calls with little RAM left
#endif




#if		qMacOS || qWindows
/*
@CLASS:			Led_StackBasedHandleLocker
@DESCRIPTION:	<p>A utility class to lock (and on exit from the block unlock) a handle.</p>
*/
class	Led_StackBasedHandleLocker {
	public:
		#if		qMacOS
			typedef	Handle	GenericHandle;
		#elif	qWindows
			typedef	HANDLE	GenericHandle;
		#endif
		Led_StackBasedHandleLocker (GenericHandle h);
		~Led_StackBasedHandleLocker ();

		nonvirtual	void*	GetPointer () const;

	private:
		GenericHandle		fHandle;
		#if		qMacOS
			unsigned char	fOldState;
		#elif	qWindows
			void*			fPointer;
		#endif
};
#endif




/*
@CLASS:			DiscontiguousRunElement<DATA>
@DESCRIPTION:	<p>We frequently need to keep track of runs of data. Sometimes - those elements are discontiguous.
	This template is merely used to give us a uniform naming convention for how we deal with those runs.</p>
		<p>These elements are meant to be used with @'DiscontiguousRun<DATA>'.</p>
*/
template	<typename	DATA>
	struct	DiscontiguousRunElement {
		DiscontiguousRunElement (size_t offset, size_t length): fOffsetFromPrev (offset), fElementLength (length), fData () {}
		DiscontiguousRunElement (size_t offset, size_t length, const DATA& data): fOffsetFromPrev (offset), fElementLength (length), fData (data) {}

		size_t	fOffsetFromPrev;
		size_t	fElementLength;
		DATA	fData;
	};
template <>
	struct	DiscontiguousRunElement<void> {
		DiscontiguousRunElement (size_t offset, size_t length): fOffsetFromPrev (offset), fElementLength (length) {}
		size_t	fOffsetFromPrev;
		size_t	fElementLength;
	};


/*
@CLASS:			DiscontiguousRun<DATA>
@DESCRIPTION:	<p>An ordered list of @'DiscontiguousRunElement<DATA>'. The ordering is very important. The 'fOffsetFromPrev'
	is the offset in marker-positions the data is intended to be from the end of the previous element. For the first
	element - this if the offset from the 'beginning'.</p>
		<p>NB: I would PREFER to have declared this as a typedef - so I would get the vector<> ctors for free. But
	I couldn't figure out how to declare it in a way that made MSVC60 happy. So I settled for this.</p>
*/
template	<typename	DATA>
	struct	DiscontiguousRun : vector<DiscontiguousRunElement<DATA> > {
	};






/*
@CONFIGVAR:		qKeepLRUCacheStats
@DESCRIPTION:	<p>Defines whether or not we capture statistics (for debugging purposes) in @'LRUCache<ELEMENT>'.
			This should be ON by default - iff @'qDebug' is true.</p>
 */
#ifndef	qKeepLRUCacheStats
#define	qKeepLRUCacheStats		qDebug
#endif





/*
@CLASS:			LRUCache<ELEMENT>
@DESCRIPTION:	<p>A basic LRU (least recently used) cache mechanism. You provide a class type argument 'ELEMENT' defined roughly as follows:
				<br>
				<code>
		struct	ELEMENT {
			struct	COMPARE_ITEM;
			nonvirtual	void	Clear ();
			static		bool	Equal (const ELEMENT&, const COMPARE_ITEM&);
		};
				</code>
				</p>
				<p>The <code>COMPARE_ITEM</code> is an object which defines the attributes which make the given item UNIQUE (for Lookup purposes).
			Think of it as the KEY. The <code>Clear ()</code> method must be provided to invaliate the given item (usually by setting part of the COMPARE_ITEM
			to an invalid value) so it won't get found by a Lookup. The 'Equal' method compares an element and a COMPARE_ITEM in the Lookup method.
				</p>
				<p>Note that the type 'ELEMENT' must be copyable (though its rarely copied - just in response
			to a @'LRUCache<ELEMENT>::SetMaxCacheSize' call.</p>
				<p>To iterate over the elements of the cache - use an @'LRUCache<ELEMENT>::CacheIterator'.
			</p>
*/
template	<typename	ELEMENT>
	class	LRUCache {
		public:
			typedef	typename ELEMENT::COMPARE_ITEM	COMPARE_ITEM;

		public:
			LRUCache (size_t maxCacheSize);

		public:
			nonvirtual	size_t	GetMaxCacheSize () const;
			nonvirtual	void	SetMaxCacheSize (size_t maxCacheSize);

		public:
			struct	CacheElement {
				public:
					CacheElement ():
						fNext (NULL),
						fPrev (NULL),
						fElement ()
						{
						}

				public:
					CacheElement*	fNext;
					CacheElement*	fPrev;

				public:
					ELEMENT			fElement;
			};

		public:
			/*
			@CLASS:			LRUCache<ELEMENT>::CacheIterator
			@DESCRIPTION:	<p>Used to iterate over elements of an @'LRUCache<ELEMENT>'</p>
							<p>Please note that while an CacheIterator object exists for an LRUCache - it is not
						safe to do other operations on the LRUCache - like @'LRUCache<ELEMENT>::LookupElement' or @'LRUCache<ELEMENT>::AddNew'.
						</p>
			*/
			struct	CacheIterator {
				CacheIterator (CacheElement* c): fCur (c) {}
				CacheElement*	fCur;
				CacheIterator& operator++ ()
					{
						RequireNotNull (fCur);
						fCur = fCur->fNext;
						return *this;
					}
				ELEMENT& operator* ()
					{
						RequireNotNull (fCur);
						return fCur->fElement;
					}
				bool operator== (CacheIterator rhs)
					{
						return fCur == rhs.fCur;
					}
				bool operator!= (CacheIterator rhs)
					{
						return fCur != rhs.fCur;
					}
			};
			nonvirtual	CacheIterator	begin ()	{ return fCachedElts_First; }
			nonvirtual	CacheIterator	end ()		{ return NULL; }

		public:
			nonvirtual	void	ClearCache ();

		public:
			nonvirtual	ELEMENT*	LookupElement (const COMPARE_ITEM& item);
			nonvirtual	ELEMENT*	AddNew ();

		#if		qKeepLRUCacheStats
		public:
			size_t		fCachedCollected_Hits;
			size_t		fCachedCollected_Misses;
		#endif

		private:
			vector<CacheElement>	fCachedElts_BUF;
			CacheElement*			fCachedElts_First;
			CacheElement*			fCachedElts_fLast;

		private:
			nonvirtual	void	ShuffleToHead (CacheElement* b);
	};







/*
 *	Character set support.
 */
#if		qMultiByteCharacters
	bool	Led_IsLeadByte (unsigned char c);
	bool	Led_IsValidSingleByte (unsigned char c);	// non-leadbyte first byte...
	bool	Led_IsValidSecondByte (unsigned char c);
	bool	Led_IsValidMultiByteString (const Led_tChar* start, size_t len);	// check all chars valid - and 'len' doesn't split char!

	/*
	 *	Led_FindPrevOrEqualCharBoundary use useful for when we guess an index in the middle
	 *	of a string, and we want to use that index - or THAT mbyte character. So this
	 *	routine always returns guessedEnd or guessedEnd-1, depending on which is the beginning
	 *	of the mbtye character.
	 */
	const Led_tChar*	Led_FindPrevOrEqualCharBoundary (const Led_tChar* start, const Led_tChar* guessedEnd);
	Led_tChar*			Led_FindPrevOrEqualCharBoundary (Led_tChar* start, Led_tChar* guessedEnd);
#endif



/*
 *	These routines can be used either with single byte, multibyte, or wide
 *	characters. They are intended to insulate most of the code from having
 *	to worry about this.
 */
/*
@METHOD:		Led_NextChar
@DESCRIPTION:	<p>See @'Led_tChar'</p>
*/
Led_tChar*			Led_NextChar (Led_tChar* fromHere);
const Led_tChar*	Led_NextChar (const Led_tChar* fromHere);

/*
@METHOD:		Led_PreviousChar
@DESCRIPTION:	<p>See @'Led_tChar'</p>
*/
Led_tChar*			Led_PreviousChar (Led_tChar* startOfString, Led_tChar* fromHere);
const Led_tChar*	Led_PreviousChar (const Led_tChar* startOfString, const Led_tChar* fromHere);



/*
@METHOD:		ValidateTextForCharsetConformance
@DESCRIPTION:	<p>See @'Led_tChar'</p>
*/
bool	ValidateTextForCharsetConformance (const Led_tChar* text, size_t length);	// just return true or false - no other diagnostics




unsigned	Led_DigitCharToNumber (char digitChar);			// require input is valid decimal digit
char		Led_NumberToDigitChar (unsigned digitValue);	// require input is valid decimal digit value

bool	Led_CasedCharsEqual (char lhs, char rhs, bool ignoreCase = true);
bool	Led_CasedStringsEqual (const string& lhs, const string& rhs, bool ignoreCase = true);








/*
 *	Clipboard access support.
 *
 *		Note - when using this on the PC - be sure clipboard opened/closed
 *	surrounding access to this object - as is currently done in Led_MFC.
 *
 */
#if		qMacOS
	typedef	OSType	Led_ClipFormat;
#elif	qWindows
	typedef	UINT	Led_ClipFormat;
#elif	qXWindows
	typedef	long	Led_ClipFormat;	
#endif
#if		qMacOS
	const Led_ClipFormat	kTEXTClipFormat	=	'TEXT';
	const Led_ClipFormat	kPICTClipFormat	=	'PICT';
	const Led_ClipFormat	kFILEClipFormat	=	'hfs ';		//	flavorTypeHFS->from <Drag.h>
#elif	qWindows
	#if		qWideCharacters
	const Led_ClipFormat	kTEXTClipFormat	=	CF_UNICODETEXT;
	#else
	const Led_ClipFormat	kTEXTClipFormat	=	CF_TEXT;
	#endif
//	const Led_ClipFormat	kPICTClipFormat	=	CF_METAFILEPICT;
	const Led_ClipFormat	kPICTClipFormat	=	CF_DIB;
	const Led_ClipFormat	kFILEClipFormat	=	CF_HDROP;
#elif	qXWindows
	const Led_ClipFormat	kTEXTClipFormat	=	XA_STRING;
	const Led_ClipFormat	kFILEClipFormat	=	1;	// X-TMP-HACK-LGP991213 - not sure what this should be???
#endif
const Led_ClipFormat		kBadClipFormat	=	0;


class	Led_ClipboardObjectAcquire {
	public:
		Led_ClipboardObjectAcquire (Led_ClipFormat clipType);
		~Led_ClipboardObjectAcquire ();

		static	bool	FormatAvailable_TEXT ();
		static	bool	FormatAvailable (Led_ClipFormat clipType);

		nonvirtual	bool	GoodClip () const;		// avoid exceptions usage for now - and just use flag
		nonvirtual	void*	GetData () const;
		nonvirtual	size_t	GetDataLength () const;

	private:
		#if		qMacOS
			Handle	fOSClipHandle;
		#elif	qWindows
			HANDLE	fOSClipHandle;
		#endif
		void*		fLockedData;
};



void	Led_BeepNotify ();
float	Led_GetTickCount ();		// return tickount in floating point seconds
float	Led_GetDoubleClickTime ();	// time-interval which defines how quick we consider two consecutive clicks a dbl-click

#if		qXWindows
extern	void	(*gBeepNotifyCallBackProc) ();
unsigned long	LedTickCount2XTime (float ledTickCount);
void			SyncronizeLedXTickCount (unsigned long xTickCount);
#endif







#if		qWindows
class	VariantArrayPacker {
	public:
		VariantArrayPacker (VARIANT* v, VARTYPE vt, size_t nElts);
		~VariantArrayPacker ();

	public:
		nonvirtual	void*	PokeAtData () const;

	private:
		VARIANT*	fSafeArrayVariant;
		void*		fPtr;
};

class	VariantArrayUnpacker {
	public:
		VariantArrayUnpacker (const VARIANT& v);
		~VariantArrayUnpacker ();

	public:
		nonvirtual	const void*	PeekAtData () const;
		nonvirtual	VARTYPE		GetArrayElementType () const;
		nonvirtual	size_t		GetLength () const;

	private:
		SAFEARRAY*	fSafeArray;
		const void*	fPtr;
};
#endif



#if		qWindows
VARIANT	CreateSafeArrayOfBSTR (const wchar_t*const* strsStart, const wchar_t*const* strsEnd);
VARIANT	CreateSafeArrayOfBSTR (const vector<const wchar_t*>& v);
VARIANT	CreateSafeArrayOfBSTR (const vector<wstring>& v);

vector<wstring>	UnpackVectorOfStringsFromVariantArray (const VARIANT& v);
#endif




#if		qWindows
void	DumpSupportedInterfaces (IUnknown* obj, const char* objectName = NULL, const char* levelPrefix = NULL);
void	DumpObjectsInIterator (IEnumUnknown* iter, const char* iteratorName = NULL, const char* levelPrefix = NULL);
#endif






/*
@CLASS:			Led_URLD
@DESCRIPTION:	<p><code>Led_URLD</code> is an abstraction of the Netscape Web Browser URL object.
	At least thats where the name comes from. And some of its more quirky attributes.
	This object has two attributes, a URL and a title.
	It has accessors to convert it into and from the Netscape URLD object format (URLD on mac
	and "Netscape Bookmark" on windows; same thing).
	It also can easily be used for dealing with URLs with ActiveX/Microsoft URL representations.</p>
		<p>See also, @'Led_URLManager'</p>
*/
struct	Led_URLD {
	Led_URLD (const char* url, const char* title);
	Led_URLD (const void* urlData, size_t nBytes);
	
	nonvirtual	size_t	GetURLDLength () const;		// return byte count of URLD itself (includes NUL-byte at end)
	nonvirtual	size_t	GetURLLength () const;		// return byte count of URL itself
	nonvirtual	size_t	GetTitleLength () const;	// return byte count of title
	
	nonvirtual	char*	PeekAtURLD () const;
	nonvirtual	char*	PeekAtURL () const;			// NB: doesn't return NUL-term string! - check GetURLLength
	nonvirtual	char*	PeekAtTitle () const;		// NB: doesn't return NUL-term string! - check GetTitleLength

	nonvirtual	string	GetTitle () const;
	nonvirtual	string	GetURL () const;

	vector<char>	fData;
};


/*
@CLASS:			Led_URLManager
@DESCRIPTION:	<p><code>Led_URLManager</code> is a utility class to deal with opening URLs.
	Depending on the OS you are running Led with, it can be configured to use 
	<em>DDE</em>, <em>ActiveX</em>, <em>ShellExec</em>, <em>AppleEvents</em>, or
	<em>Internet Config (IC)</em>.
	By default, it uses all that make sense for the given operating system in some reasonable
	order (prefering more robust approaches, and those that work more frequently and those that
	report better errors).</p>
		<p>This class also provides a utility - <code>FileSpecToURL</code> - to make a URL object
	from a full path name.</p>
		<p>See also, @'Led_URLD'</p>
*/
class	Led_URLManager {
	public:
		Led_URLManager ();

		static	Led_URLManager&	Get ();	// by default builds one, but you can specify your
										// own. NB: deletes old value, so if you specify
										// your own, be sure to allocate it with new so it
										// can be deleted...
		static	void			Set (Led_URLManager* newURLMgr);
	private:
		static	Led_URLManager*	sThe;

	public:
		virtual	void	Open (const string& url);	// throws on detected errors
		#if		qMacOS
		virtual	string	FileSpecToURL (const FSSpec& fsp);
		#elif	qWindows
		virtual	string	FileSpecToURL (const string& path);
		#endif

	protected:
		#if		qUseInternetConfig
			nonvirtual	void	Open_IC (const string& url);
		#endif
		#if		qMacOS
			nonvirtual	void	Open_SpyglassAppleEvent (const string& url);
		#endif
		#if		qUseActiveXToOpenURLs
			nonvirtual	void	Open_ActiveX (const string& url);
		#endif
		#if		qUseSpyglassDDESDIToOpenURLs
			nonvirtual	void	Open_SpyglassDDE (const string& url);
		#endif
		#if		qUseSystemNetscapeOpenURLs
			nonvirtual	void	Open_SystemNetscape (const string& url);
		#endif


	#if		qMacOS
	private:
		static	pascal	OSErr	FSpGetFullPath (const FSSpec *spec, short *fullPathLength, Handle *fullPath);
	#endif


	#if		qMacOS
	public:
		static	ProcessSerialNumber	FindBrowser ();
	#endif

	#if		qUseSpyglassDDESDIToOpenURLs
	public:
		static	void	InitDDEHandler ();	// to be able to open URLs with DDE this must be called, but
											// it takes over all DDE processing, disabling your app from doing
											// any other DDE.

		// Use this for more low level extensions of the DDE support...
		static	DWORD	sDDEMLInstance;	//	The DDEML instance identifier.
		static	HDDEDATA CALLBACK	SimpleDdeCallBack (UINT /*type*/, UINT /*fmt*/, HCONV /*hconv*/, HSZ /*hsz1*/, HSZ /*hsz2*/, HDDEDATA /*hData*/, DWORD /*dwData1*/, DWORD /*dwData2*/);
		static	const char*			SkipToNextArgument (const char *pFormat);
		static	HSZ					ClientArguments (const char *pFormat, ...);
		static	char*				ExtractArgument (HSZ hszArgs, int iArg);
		static	void				ServerReturned (HDDEDATA hArgs, const char *pFormat, ...);
	#endif
};




string	MakeSophistsAppNameVersionURL (const string& relURL, const string& appName, const string& extraArgs = string ());







/*
@CONFIGVAR:		qLedCheckCompilerFlagsConsistency
@DESCRIPTION:	<p>Some development environments make it easy to accidentally provide an inconsistent set of compilation flags
			across compilation units (e.g. MSVC/MS Visual Studio.Net is like this). This is a <em>bad</em> thing.</p>
				<p>Led enables certain features - like debug checking - based on certain compilation flags (e.g. @'qDebug').
			Some virtual methods are added to certain classes (e.g. Invariant check methods). And some other features maybe
			conditionally defined based on other flags.</p>
				<p>This mechanism attempts to detect if these flags are set inconsistently. This is only set on
			by default for DEBUG builds - but - unfortunately - this can just as easily affect release builds (even
			when the DEBUG builds work fine. For this reason - if you are seeing strange behavior when you compile for
			a RELEASE build - you may want to externally specify this flag to test.
				</p>
 */
#ifndef	qLedCheckCompilerFlagsConsistency
#define	qLedCheckCompilerFlagsConsistency	qDebug
#endif

#if		qLedCheckCompilerFlagsConsistency
	namespace LedCheckCompilerFlags {
		#define	LedCheckCompilerFlags_(a)	_CHECK_##a
		extern	int	LedCheckCompilerFlags_(qDebug);
		extern	int	LedCheckCompilerFlags_(qSingleByteCharacters);
		extern	int	LedCheckCompilerFlags_(qMultiByteCharacters);
		extern	int	LedCheckCompilerFlags_(qWideCharacters);
		extern	int	LedCheckCompilerFlags_(qProvideIMESupport);

		struct	FlagsChecker {
			FlagsChecker ()
				{
					/*
					 *	See the docs on @'qLedCheckCompilerFlagsConsistency' if this ever fails.
					 */
					if (LedCheckCompilerFlags_(qDebug) != qDebug)									{			abort ();		}
					if (LedCheckCompilerFlags_(qSingleByteCharacters) != qSingleByteCharacters)		{			abort ();		}
					if (LedCheckCompilerFlags_(qMultiByteCharacters) != qMultiByteCharacters)		{			abort ();		}
					if (LedCheckCompilerFlags_(qWideCharacters) != qWideCharacters)					{			abort ();		}
					if (LedCheckCompilerFlags_(qProvideIMESupport) != qProvideIMESupport)			{			abort ();		}
				}
		};
		static	struct	FlagsChecker	sFlagsChecker;
	}
#endif











/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (4 : 4800)	//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif

#if		!qHasIsAscii && defined (isascii)
	//#warning	"You probably should define qHasIsAscii for your compiler."
#endif


#if		qBasicString_C_STR_BROKEN_forWCHAR_T
		}
	}
}
	// Sadly this is NEEDED for GCC 3.? on RH 7.3 but is NOT needed for GCC 3.2 or later on RH8. Not sure how to
	// tell from compile time flags which is which?
	template <>
		inline	const wchar_t*	basic_string<wchar_t>::c_str () const
			{
				terminate ();
				return data ();
			}
namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {
	#endif


#if		qMacOS
		}
	}
}
	#include	<Sound.h>
namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {
	#endif



	inline	size_t	Led_tStrlen (const Led_tChar* s)
		{
			RequireNotNull (s);
			#if		qSingleByteCharacters
				return ::strlen (s);
			#elif	qMultiByteCharacters
				return ::_mbstrlen (s);
			#elif	qWideCharacters
				return ::wcslen (s);
			#endif	
		}


	inline	int	Led_tStrCmp (const Led_tChar* l, const Led_tChar* r)
		{
			RequireNotNull (l);
			RequireNotNull (r);
			#if		qSingleByteCharacters
				return ::strcmp (l, r);
			#elif	qMultiByteCharacters
				return ::_mbscmp (l, r);
			#elif	qWideCharacters
				return ::wcscmp (l, r);
			#endif	
		}

	
	inline	int	Led_tStrnCmp (const Led_tChar* l, const Led_tChar* r, size_t n)
		{
			RequireNotNull (l);
			RequireNotNull (r);
			#if		qSingleByteCharacters
				return ::strncmp (l, r, n);
			#elif	qMultiByteCharacters
				return ::_mbsncmp (l, r, n);
			#elif	qWideCharacters
				return ::wcsncmp (l, r, n);
			#endif	
		}



	inline	const Led_tChar* Led_tStrChr (const Led_tChar* s, Led_tChar c)
		{
			RequireNotNull (s);
			#if		qWideCharacters
				return ::wcschr (s, c);
			#else
				return ::strchr (s, c);
			#endif
		}




// This bizare logic to test twice for the presence of isascii is because the first test is
// whether the development environment provides an isascii(). The second test is
// in case Led is used with some other library which may be included alog with Led
// and that other library works around the absence of isascii - just as we did...
#if		!qHasIsAscii && !defined (isascii)
	// I know this used to be part of the UNIX ctype - unsure why it does not appear to
	// be in the ANSI ctype??? - LGP 950211
	inline	bool	isascii (unsigned char c)
		{
			return (c <= 0x7f);
		}
#endif






	#if		qSDK_UNICODE
		inline	Led_SDK_String	Led_Wide2SDKString (const wstring& s)
			{
				return s;
			}
		inline	wstring	Led_SDKString2Wide (const Led_SDK_String& s)
			{
				return s;
			}
	#else
		inline	Led_SDK_String	Led_ANSI2SDKString (const string& s)
			{
				return s;
			}
		inline	string	Led_SDKString2ANSI (const Led_SDK_String& s)
			{
				return s;
			}
	#endif

	#if			qWideCharacters
		inline	Led_tString	Led_WideString2tString (const wstring& s)
			{
				return s;
			}
		inline	wstring	Led_tString2WideString (const Led_tString& s)
			{
				return s;
			}
	#endif

	#if		!qWideCharacters
		inline	Led_tString	Led_ANSIString2tString (const string& s)
			{
				return s;
			}
		inline	string	Led_tString2ANSIString (const Led_tString& s)
			{
				return s;
			}
	#endif


	#if		qWideCharacters == qSDK_UNICODE
		inline	Led_SDK_String	Led_tString2SDKString (const Led_tString& s)
			{
					return s;
			}
		inline	Led_tString	Led_SDKString2tString (const Led_SDK_String& s)
			{
					return s;
			}
	#endif


#if		qWindows
//	class	Win32ErrorException
	inline	Win32ErrorException::Win32ErrorException (DWORD error):
		fError (error)
		{
		}
	inline	Win32ErrorException::operator DWORD () const
		{
			return fError;
		}


//	class	HRESULTErrorException
	inline	HRESULTErrorException::HRESULTErrorException (HRESULT hresult):
		fHResult (hresult)
		{
		}
	inline	HRESULTErrorException::operator HRESULT () const
		{
			return fHResult;
		}
#endif

#if		qWindows
	/*
	@METHOD:		Led_ThrowIfFalseGetLastError
	@DESCRIPTION:	<p></p>
	*/
	inline	void	Led_ThrowIfFalseGetLastError (bool test)
		{
			if (not test) {
				throw (Win32ErrorException (::GetLastError ()));
			}
		}
	/*
	@METHOD:		Led_ThrowIfNotERROR_SUCCESS
	@DESCRIPTION:	<p></p>
	*/
	inline	void	Led_ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
		{
			if (win32ErrCode != ERROR_SUCCESS) {
				throw Win32ErrorException (win32ErrCode);
			}
		}
#endif




#if		qWindows
	inline	void	Led_ThrowIfErrorHRESULT (HRESULT hr)
		{
			if (not SUCCEEDED (hr)) {
				throw HRESULTErrorException (hr);
			}
		}
#endif


#if		qMacOS
	/*
	@METHOD:		Led_ThrowIfOSErr
	@DESCRIPTION:	<p>If the argument err is not noErr, then throw that error.</p>
	*/
	inline	void	Led_ThrowIfOSErr (OSErr err)
		{
			if (err != noErr) {
				throw err;
			}
		}
	/*
	@METHOD:		Led_ThrowIfOSStatus
	@DESCRIPTION:	<p>If the argument err is not noErr, then throw that error.</p>
	*/
	inline	void	Led_ThrowIfOSStatus (OSStatus err)
		{
			if (err != noErr) {
				throw err;
			}
		}
#endif
	inline	void	Led_ThrowIfNull (void* p)
		{
			if (p == NULL) {
				Led_ThrowOutOfMemoryException ();
			}
		}





	inline	unsigned short	Led_ByteSwapFromMac (unsigned short src)
		{
			#if		qMacOS
				return src;
			#elif	qWindows
				return ((src&0xff) << 8) + (src>>8);
			#endif
		}
	inline	short	Led_ByteSwapFromMac (short src)
		{
			return short (Led_ByteSwapFromMac ((unsigned short)src));
		}



	inline	unsigned short	Led_ByteSwapFromWindows (unsigned short src)
		{
			#if		qMacOS
				return ((src&0xff) << 8) + (src>>8);
			#elif	qWindows
				return src;
			#endif
		}
	inline	short	Led_ByteSwapFromWindows (short src)
		{
			return short (Led_ByteSwapFromWindows ((unsigned short)src));
		}
	inline	unsigned long	Led_ByteSwapFromWindows (unsigned long src)
		{
			#if		qMacOS
				return (Led_ByteSwapFromWindows ((unsigned short)(src&0xffff)) << 16) + Led_ByteSwapFromWindows ((unsigned short)(src>>16));
			#elif	qWindows
				return src;
			#endif
		}
	inline	long	Led_ByteSwapFromWindows (long src)
		{
			return long (Led_ByteSwapFromWindows ((unsigned long)src));
		}




	inline	void	Led_USHORTToBuf (unsigned short u, unsigned short* realBuf)
		{
			unsigned char* buf	=	(unsigned char*)realBuf;
			buf[0] = (unsigned char)(u >> 8);
			buf[1] = (unsigned char)u;
		}
	inline	unsigned short	Led_BufToUSHORT (const unsigned short* realBuf)
		{
			const unsigned char* buf	=	(const unsigned char*)realBuf;
			return (unsigned short)((((unsigned short)buf[0]) << 8) + (unsigned short)buf[1]);
		}
	inline	void	Led_ULONGToBuf (unsigned long ul, unsigned long* realBuf)
		{
			unsigned short* buf	=	(unsigned short*)realBuf;
			Led_USHORTToBuf((unsigned short)(ul >> 16), buf);
			Led_USHORTToBuf((unsigned short)(ul), buf + 1);
		}
	inline	unsigned long	Led_BufToULONG (const unsigned long* buf)
		{
			unsigned short*	bufAsShortArray	=	(unsigned short*)buf;
			return (((unsigned long)Led_BufToUSHORT(bufAsShortArray)) << 16) + Led_BufToUSHORT(bufAsShortArray + 1);
		}
	inline	void	Led_ULONGToBuf (unsigned int ul, unsigned int* realBuf)
		{
			unsigned short* buf	=	(unsigned short*)realBuf;
			Led_USHORTToBuf((unsigned short)(ul >> 16), buf);
			Led_USHORTToBuf((unsigned short)(ul), buf + 1);
		}
	inline	unsigned int	Led_BufToULONG (const uint32_t* buf)
		{
			unsigned short*	bufAsShortArray	=	(unsigned short*)buf;
			return (((unsigned long)Led_BufToUSHORT(bufAsShortArray)) << 16) + Led_BufToUSHORT(bufAsShortArray + 1);
		}







#if		qAllowBlockAllocation
	// This must be included here to keep genclass happy, since the .cc file will not be included
	// in the genclassed .cc file....
	inline	void**	Led_Block_Alloced_GetMem_Util (const size_t kSize)
		{
			Assert (kSize >= sizeof (void*));	//	cuz we overwrite first sizeof(void*) for link

			/*
			 * Picked particular kTargetMallocSize since with malloc overhead likely to turn out to be
			 * a chunk which memory allocator can do a good job on.
			 */
			const	size_t	kTargetMallocSize	=	16360;					// 16384 = 16K - leave a few bytes sluff...
			const	size_t	kChunks = Led_Max (kTargetMallocSize / kSize, (size_t)10);

			/*
			 * Please note that the following line is NOT a memory leak. Please look at the
			 * Led FAQ question#29 - "Does Led have any memory leaks?
			 * How does qAllowBlockAllocation affect memory leaks?"
			 */
			void**	newLinks	=	(void**)::new char [kChunks*kSize];
			void**	curLink		=	newLinks;
			for (size_t i = 1; i < kChunks; i++) {
				*curLink = &(((char*)newLinks)[i*kSize]);
				curLink = (void**)*curLink;
			}
			*curLink = NULL;		// NULL-terminate the link list
			return (newLinks);
		}



/*
 *		In order to reduce fragmentation, we allocate all chunks of common sizes out of
 *	the same pool. Assuming the compiler is able to inline the references to these
 *	below, there is really no cost in adding more. I picked the ones I thought most
 *	likely to come up, but adding more should be just fine - strictly a win.
 *
 *		Don't bother implementing Led_Block_Alloced_sizeof_N_GetMems() cuz flunging will
 *	genericly give us the same code-sharing effect.
 */
	extern	void*	Led_Block_Alloced_sizeof_4_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_8_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_12_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_16_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_20_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_24_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_28_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_32_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_36_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_40_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_44_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_48_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_52_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_56_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_60_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_64_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_68_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_72_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_76_sNextLink;
	extern	void*	Led_Block_Alloced_sizeof_80_sNextLink;



	template	<typename	T>	inline	void*	Led_BlockAllocated<T>::GetNextLink ()
		{
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma push
				#pragma warn -8008
				#pragma warn -8066
			#endif
			/*
			 * It is hoped that since all these comparisons can be evaluated at compile
			 * time, they will be. Then this code reduces to just a return of a single
			 * global variable. That should inline nicely.
			 *
			 *	NB: For Apples compiler (by far our worst at optimizing), this does successfully
			 *		get translated into just one variable reference (C 3.2.3, CFront 3.2, ETO #8, Thursday, November 5, 1992 1:51:42 PM)
			 */
			if (sizeof (T) <= 4) {
				return (Led_Block_Alloced_sizeof_4_sNextLink);
			}
			else if (sizeof (T) <= 8) {
				return (Led_Block_Alloced_sizeof_8_sNextLink);
			}
			else if (sizeof (T) <= 12) {
				return (Led_Block_Alloced_sizeof_12_sNextLink);
			}
			else if (sizeof (T) <= 16) {
				return (Led_Block_Alloced_sizeof_16_sNextLink);
			}
			else if (sizeof (T) <= 20) {
				return (Led_Block_Alloced_sizeof_20_sNextLink);
			}
			else if (sizeof (T) <= 24) {
				return (Led_Block_Alloced_sizeof_24_sNextLink);
			}
			else if (sizeof (T) <= 28) {
				return (Led_Block_Alloced_sizeof_28_sNextLink);
			}
			else if (sizeof (T) <= 32) {
				return (Led_Block_Alloced_sizeof_32_sNextLink);
			}
			else if (sizeof (T) <= 36) {
				return (Led_Block_Alloced_sizeof_36_sNextLink);
			}
			else if (sizeof (T) <= 40) {
				return (Led_Block_Alloced_sizeof_40_sNextLink);
			}
			else if (sizeof (T) <= 44) {
				return (Led_Block_Alloced_sizeof_44_sNextLink);
			}
			else if (sizeof (T) <= 48) {
				return (Led_Block_Alloced_sizeof_48_sNextLink);
			}
			else if (sizeof (T) <= 52) {
				return (Led_Block_Alloced_sizeof_52_sNextLink);
			}
			else if (sizeof (T) <= 56) {
				return (Led_Block_Alloced_sizeof_56_sNextLink);
			}
			else if (sizeof (T) <= 60) {
				return (Led_Block_Alloced_sizeof_60_sNextLink);
			}
			else if (sizeof (T) <= 64) {
				return (Led_Block_Alloced_sizeof_64_sNextLink);
			}
			else if (sizeof (T) <= 68) {
				return (Led_Block_Alloced_sizeof_68_sNextLink);
			}
			else if (sizeof (T) <= 72) {
				return (Led_Block_Alloced_sizeof_72_sNextLink);
			}
			else if (sizeof (T) <= 76) {
				return (Led_Block_Alloced_sizeof_76_sNextLink);
			}
			else if (sizeof (T) <= 80) {
				return (Led_Block_Alloced_sizeof_80_sNextLink);
			}
			else {
				return (sNextLink);
			}
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma pop
			#endif
		}
	template	<typename	T>	inline	void	Led_BlockAllocated<T>::SetNextLink (void* nextLink)
		{
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma push
				#pragma warn -8008
                                #pragma warn -8066
			#endif
			/*
			 * It is hoped that since all these comparisons can be evaluated at compile
			 * time, they will be. Then this code reduces to just an assignement to a single
			 * global variable. That should inline nicely.
			 */
			if (sizeof (T) <= 4) {
				Led_Block_Alloced_sizeof_4_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 8) {
				Led_Block_Alloced_sizeof_8_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 12) {
				Led_Block_Alloced_sizeof_12_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 16) {
				Led_Block_Alloced_sizeof_16_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 20) {
				Led_Block_Alloced_sizeof_20_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 24) {
				Led_Block_Alloced_sizeof_24_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 28) {
				Led_Block_Alloced_sizeof_28_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 32) {
				Led_Block_Alloced_sizeof_32_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 36) {
				Led_Block_Alloced_sizeof_36_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 40) {
				Led_Block_Alloced_sizeof_40_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 44) {
				Led_Block_Alloced_sizeof_44_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 48) {
				Led_Block_Alloced_sizeof_48_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 52) {
				Led_Block_Alloced_sizeof_52_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 56) {
				Led_Block_Alloced_sizeof_56_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 60) {
				Led_Block_Alloced_sizeof_60_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 64) {
				Led_Block_Alloced_sizeof_64_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 68) {
				Led_Block_Alloced_sizeof_68_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 72) {
				Led_Block_Alloced_sizeof_72_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 76) {
				Led_Block_Alloced_sizeof_76_sNextLink = nextLink;
			}
			else if (sizeof (T) <= 80) {
				Led_Block_Alloced_sizeof_80_sNextLink = nextLink;
			}
			else {
				sNextLink = nextLink;
			}
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma pop
			#endif
		}
#endif
	template	<typename	T>	inline	void*	Led_BlockAllocated<T>::operator new (size_t n)
		{
		#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
			#pragma push
			#pragma warn -8008
                        #pragma warn -8066
		#endif
			Assert (sizeof (T) >= sizeof (void*));	//	cuz we overwrite first sizeof(void*) for link
			Assert (n == sizeof (T));
			Led_Arg_Unused (n);							// n only used for debuggging, avoid compiler warning
		#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
			#pragma pop
		#endif

			#if		qAllowBlockAllocation
				/*
				 * To implement linked list of Led_BlockAllocated(T)'s before they are
				 * actually alloced, re-use the begining of this as a link pointer.
				 */
				if (GetNextLink () == NULL) {
					GetMem ();
				}
				void*	result = GetNextLink ();
				SetNextLink (*(void**)GetNextLink ());

				return (result);
			#else
				return ::operator new (n);
			#endif
		}
	template	<typename	T>	inline	void	Led_BlockAllocated<T>::operator delete (void* p)
		{
			#if		qAllowBlockAllocation
				if (p != NULL) {
					(*(void**)p) = GetNextLink ();
					SetNextLink (p);
				}
			#else
				::operator delete (p);
			#endif
		}
#if		qAllowBlockAllocation
	template	<typename	T>	void	Led_BlockAllocated<T>::GetMem ()
		{
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma push
				#pragma warn -8008
                                #pragma warn -8066
			#endif
		#if		qDebug
			/*
			 * Temporary hack to verify were getting the most out of our blockallocator.
			 * If this is ever gets called, add extra sizes. NO COST, just wins!!!
			 */
			if (sizeof (T) > 80) {
		//		DebugMessage ("BlockAllocator<T> has a size > 44: %d", sizeof (T));
			}
		#endif

			if (sizeof (T) <= 4) {
				Led_Block_Alloced_sizeof_4_sNextLink = Led_Block_Alloced_GetMem_Util (4);
			}
			else if (sizeof (T) <= 8) {
				Led_Block_Alloced_sizeof_8_sNextLink = Led_Block_Alloced_GetMem_Util (8);
			}
			else if (sizeof (T) <= 12) {
				Led_Block_Alloced_sizeof_12_sNextLink = Led_Block_Alloced_GetMem_Util (12);
			}
			else if (sizeof (T) <= 16) {
				Led_Block_Alloced_sizeof_16_sNextLink = Led_Block_Alloced_GetMem_Util (16);
			}
			else if (sizeof (T) <= 20) {
				Led_Block_Alloced_sizeof_20_sNextLink = Led_Block_Alloced_GetMem_Util (20);
			}
			else if (sizeof (T) <= 24) {
				Led_Block_Alloced_sizeof_24_sNextLink = Led_Block_Alloced_GetMem_Util (24);
			}
			else if (sizeof (T) <= 28) {
				Led_Block_Alloced_sizeof_28_sNextLink = Led_Block_Alloced_GetMem_Util (28);
			}
			else if (sizeof (T) <= 32) {
				Led_Block_Alloced_sizeof_32_sNextLink = Led_Block_Alloced_GetMem_Util (32);
			}
			else if (sizeof (T) <= 36) {
				Led_Block_Alloced_sizeof_36_sNextLink = Led_Block_Alloced_GetMem_Util (36);
			}
			else if (sizeof (T) <= 40) {
				Led_Block_Alloced_sizeof_40_sNextLink = Led_Block_Alloced_GetMem_Util (40);
			}
			else if (sizeof (T) <= 44) {
				Led_Block_Alloced_sizeof_44_sNextLink = Led_Block_Alloced_GetMem_Util (44);
			}
			else if (sizeof (T) <= 48) {
				Led_Block_Alloced_sizeof_48_sNextLink = Led_Block_Alloced_GetMem_Util (48);
			}
			else if (sizeof (T) <= 52) {
				Led_Block_Alloced_sizeof_52_sNextLink = Led_Block_Alloced_GetMem_Util (52);
			}
			else if (sizeof (T) <= 56) {
				Led_Block_Alloced_sizeof_56_sNextLink = Led_Block_Alloced_GetMem_Util (56);
			}
			else if (sizeof (T) <= 60) {
				Led_Block_Alloced_sizeof_60_sNextLink = Led_Block_Alloced_GetMem_Util (60);
			}
			else if (sizeof (T) <= 64) {
				Led_Block_Alloced_sizeof_64_sNextLink = Led_Block_Alloced_GetMem_Util (64);
			}
			else if (sizeof (T) <= 68) {
				Led_Block_Alloced_sizeof_68_sNextLink = Led_Block_Alloced_GetMem_Util (68);
			}
			else if (sizeof (T) <= 72) {
				Led_Block_Alloced_sizeof_72_sNextLink = Led_Block_Alloced_GetMem_Util (72);
			}
			else if (sizeof (T) <= 76) {
				Led_Block_Alloced_sizeof_76_sNextLink = Led_Block_Alloced_GetMem_Util (76);
			}
			else if (sizeof (T) <= 80) {
				Led_Block_Alloced_sizeof_80_sNextLink = Led_Block_Alloced_GetMem_Util (80);
			}
			else {
				/*
				 * NON-Shared freepool. This should rarely, if ever happen, but we fully support
				 * it anyhow.
				 */
				SetNextLink (Led_Block_Alloced_GetMem_Util (sizeof (T)));
			}
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma pop
			#endif
		}

	template	<typename	T>	void*	Led_BlockAllocated<T>::sNextLink = NULL;
#endif





#if		qMacOS
	// throw if cannot do allocation. Use tmp memory if qUseMacTmpMemForAllocs.
	// fall back on application-heap-zone if no tmp memory
	inline	Handle	Led_DoNewHandle (size_t n)
		{
			#if		qUseMacTmpMemForAllocs
				OSErr err;	// ingored...
				Handle	h	=	::TempNewHandle (n, &err);	// try temp mem, and use our local mem if no temp mem left
				if (h == NULL) {
					h = ::NewHandle (n);
				}
			#else
				Handle	h = ::NewHandle (n);
			#endif
			Led_ThrowIfNull (h);
			return h;
		}
	inline	void	Led_CheckSomeLocalHeapRAMAvailable (size_t n)
		{
			Handle	h	=	::NewHandle (n);
			Led_ThrowIfNull (h);
			::DisposeHandle (h);
		}
#endif







template	<typename ARRAY_CONTAINER, class	T>
	size_t	IndexOf (const ARRAY_CONTAINER& array, T item)
		{
			for (typename ARRAY_CONTAINER::const_iterator i = array.begin (); i != array.end (); ++i) {
				if (*i == item) {
					return (i - array.begin ());
				}
			}
			return kBadIndex;
		}








#if		qMacOS || qWindows
	inline	Led_StackBasedHandleLocker::Led_StackBasedHandleLocker (GenericHandle h):
		fHandle (h)
			{
				RequireNotNull (h);
				#if		qMacOS
					fOldState = ::HGetState (h);
					::HLock (h);
				#elif	qWindows
					fPointer = ::GlobalLock (h);
				#endif
			}
	inline	Led_StackBasedHandleLocker::~Led_StackBasedHandleLocker ()
			{
				#if		qMacOS
					::HSetState (fHandle, fOldState);
				#elif	qWindows
					::GlobalUnlock (fHandle);
				#endif
			}
	inline	void*	Led_StackBasedHandleLocker::GetPointer () const
			{
				#if		qMacOS
					return *fHandle;
				#elif	qWindows
					return fPointer;
				#endif
			}
#endif








//	class	LRUCache<ELEMENT>
	template	<typename	ELEMENT>
		LRUCache<ELEMENT>::LRUCache (size_t maxCacheSize):
				#if		qKeepLRUCacheStats
				fCachedCollected_Hits (0),
				fCachedCollected_Misses (0),
				#endif
				fCachedElts_BUF (),
				fCachedElts_First (NULL),
				fCachedElts_fLast (NULL)
			{
				SetMaxCacheSize (maxCacheSize);
			}
	template	<typename	ELEMENT>
		inline	size_t	LRUCache<ELEMENT>::GetMaxCacheSize () const
			{
				return fCachedElts_BUF.size ();
			}
	template	<typename	ELEMENT>
		void	LRUCache<ELEMENT>::SetMaxCacheSize (size_t maxCacheSize)
			{
				Require (maxCacheSize >= 1);
				if (maxCacheSize != fCachedElts_BUF.size ()) {
					fCachedElts_BUF.resize (maxCacheSize);
					// Initially link LRU together.
					fCachedElts_First = &*fCachedElts_BUF.begin ();
					fCachedElts_fLast = fCachedElts_First + maxCacheSize-1;
					fCachedElts_BUF[0].fPrev = NULL;
					for (size_t i = 0; i < maxCacheSize-1; ++i) {
						fCachedElts_BUF[i].fNext = fCachedElts_First + (i+1);
						fCachedElts_BUF[i+1].fPrev = fCachedElts_First + (i);
					}
					fCachedElts_BUF[maxCacheSize-1].fNext = NULL;
				}
			}
	template	<typename	ELEMENT>
		inline	void	LRUCache<ELEMENT>::ShuffleToHead (CacheElement* b)
			{
				AssertNotNull (b);
				if (b == fCachedElts_First) {
					Assert (b->fPrev == NULL);
					return;	// already at head
				}
				CacheElement*	prev	=	b->fPrev;
				AssertNotNull (prev);					// don't call this if already at head
				// patch following and preceeding blocks to point to each other
				prev->fNext = b->fNext;
				if (b->fNext == NULL) {
					Assert (b == fCachedElts_fLast);
					fCachedElts_fLast = b->fPrev;
				}
				else {
					b->fNext->fPrev = prev;
				}

				// Now patch us into the head of the list
				CacheElement*	oldFirst	=	fCachedElts_First;
				AssertNotNull (oldFirst);
				b->fNext = oldFirst;
				oldFirst->fPrev = b;
				b->fPrev = NULL;
				fCachedElts_First = b;

				Ensure (fCachedElts_fLast != NULL and fCachedElts_fLast->fNext == NULL);
				Ensure (fCachedElts_First != NULL and fCachedElts_First == b and fCachedElts_First->fPrev == NULL and fCachedElts_First->fNext != NULL);
			}
	template	<typename	ELEMENT>
		inline	void	LRUCache<ELEMENT>::ClearCache ()
			{
				for (CacheElement* cur = fCachedElts_First; cur != NULL; cur = cur->fNext) {
					cur->fElement.Clear ();
				}
			}
	template	<typename	ELEMENT>
		/*
		@METHOD:		LRUCache<ELEMENT>::LookupElement
		@DESCRIPTION:	<p>Check and see if the given element is in the cache. Return that element if its tehre, and NULL otherwise.
					Note that this routine re-orders the cache so that the most recently looked up element is first, and because
					of this re-ordering, its illegal to do a Lookup while a @'LRUCache<ELEMENT>::CacheIterator' exists
					for this LRUCache.</p>
		*/
		inline	ELEMENT*	LRUCache<ELEMENT>::LookupElement (const COMPARE_ITEM& item)
			{
				for (CacheElement* cur = fCachedElts_First; cur != NULL; cur = cur->fNext) {
					if (ELEMENT::Equal (cur->fElement, item)) {
						ShuffleToHead (cur);
						#if		qKeepLRUCacheStats
							fCachedCollected_Hits++;
						#endif
						return &fCachedElts_First->fElement;
					}
				}
				#if		qKeepLRUCacheStats
					fCachedCollected_Misses++;
				#endif
				return NULL;
			}
	template	<typename	ELEMENT>
		/*
		@METHOD:		LRUCache<ELEMENT>::AddNew
		@DESCRIPTION:	<p>Create a new LRUCache element (potentially bumping some old element out of the cache). This new element
					will be considered most recently used. Note that this routine re-orders the cache so that the most recently looked
					up element is first, and because of this re-ordering, its illegal to do a Lookup while 
					a @'LRUCache<ELEMENT>::CacheIterator' exists for this LRUCache.</p>
		*/
		inline	ELEMENT*	LRUCache<ELEMENT>::AddNew ()
			{
				ShuffleToHead (fCachedElts_fLast);
				return &fCachedElts_First->fElement;
			}





#if		qMultiByteCharacters
	inline	bool	Led_IsLeadByte (unsigned char c)
		{
			#error	"That Multibyte character set not supported"
		}
	inline	bool	Led_IsValidSingleByte (unsigned char /*c*/)
		{
			// This isn't really right, but close enough for now. Alec Wysocker seems to think
			// so anyhow... LGP 950306
			return true;
		}
	inline	bool	Led_IsValidSecondByte (unsigned char c)
		{
			#error	"That Multibyte character set not supported"
		}
#endif
	inline	Led_tChar*		Led_NextChar (Led_tChar* fromHere)	
		{
			AssertNotNull (fromHere);
			#if		qSingleByteCharacters || qWideCharacters
				return (fromHere + 1);		// address arithmatic does the magic for wide characters
			#elif	qMultiByteCharacters
				return (Led_IsLeadByte (*fromHere)? (fromHere+2): (fromHere+1));
			#endif
		}
	inline	const Led_tChar*	Led_NextChar (const Led_tChar* fromHere)
		{
			AssertNotNull (fromHere);
			#if		qSingleByteCharacters || qWideCharacters
				return (fromHere + 1);		// address arithmatic does the magic for wide characters
			#elif	qMultiByteCharacters
				return (Led_IsLeadByte (*fromHere)? (fromHere+2): (fromHere+1));
			#endif
		}
	/*

FIXUP COMMENT - FROM EMAIL - AND ABOUT PREV_CHAR IMPLEMENTATION...
        Bytes can be:

        (1)     Ascii (0-0x7f)
        (2)     FirstByte of DoubleByte Char
                (c >= 0x81 and c <= 0x9f) or (c >= 0xe0 and c <= 0xfc)
        (3)     SecondByte of DoubleByte Char
                (c >= 0x40 and c <= 0x7e) or (c >= 0x80 and c <= 0xfc)

(NB: This clasification counts on the fact that we filter out all bad SJIS)

        note that class of ASCII bytes and the class of FirstBytes does not overlap, but the class of SecondBytes DOES overlap with both others.

        Our task in scanning backward is to be able to UNIQUELY tell by looking at a byte - WITHOUT KNOWING ITS CONTEXT BEFOREHAND - if it is an ASCII byte, FirstByte, or a SecondByte.

        This analysis makes our job a little easier. Most bytes just by looking at them - can be immediately clasified.

        0x00-0x3f ==> ASCII
        0x40-0x7e ==> ASCII or SecondByte
        0x7f      ==> ASCII
        0x80      ==> SecondByte
        0x81-0x9f ==> FirstByte or SecondByte
        0xa0-0xdf ==> SecondByte
        0xe0-0xfc ==> FirstByte or SecondByte
        
        *	This Algoritm assumes that mbyte character sets have at most two bytes per character. This
        *	is true for SJIS - but I'm not posative it is always true - LGP 950216.

	 */
	inline	const Led_tChar*	Led_PreviousChar (const Led_tChar* startOfString, const Led_tChar* fromHere)
		{
			AssertNotNull (startOfString);
			AssertNotNull (fromHere);
			Assert (startOfString < fromHere);	// Must be room for previous character to exist!
			#if		qSingleByteCharacters || qWideCharacters
				Led_Arg_Unused (startOfString);
				return (fromHere - 1);		// address arithmatic does the magic for wide characters
			#elif	qMultiByteCharacters
				/*
				 *	If the previous byte is a lead-byte, then the real character boundsary
				 *	is really TWO back.
				 *
				 *	Proof by contradiction:
				 *		Assume prev character is back one byte. Then the chracter it is part of
				 *	contains the first byte of the character we started with. This obviously
				 *	cannot happen. QED.
				 *
				 *		This is actually a worth-while test since lots of second bytes look quite
				 *	a lot like lead-bytes - so this happens a lot.
				 */
				if (Led_IsLeadByte (*(fromHere-1))) {
					Assert (fromHere-startOfString >= 2);		// else split character...
					return (fromHere - 2);
				}
				if (fromHere == startOfString + 1) {
					return (startOfString);	// if there is only one byte to go back, it must be an ASCII byte
				}
				// we go back by BYTES til we find a syncronization point
				const Led_tChar* cur = fromHere-2;
				for (; cur > startOfString; cur--) {
					if (not Led_IsLeadByte (*cur)) {
						// Then we are in case 1, 2, 3, 4 or 6 (not 5 or 7). So ew know we are looking
						// at an ASCII byte, or a secondbyte. Therefore - the NEXT byte from here must be
						// a valid mbyte char boundary.
						cur++;
						break;
					} 
				}
				Assert (cur < fromHere);
				// Now we are pointing AT LEAST one mbyte char back from 'fromHere' so scan forward as we used
				// to to find the previous character...
				for (; cur < fromHere; ) {
					const Led_tChar*	next = Led_NextChar (cur);
					if (next == fromHere) {
						return (cur);
					}
					Assert (next < fromHere);	// if we've gone past - then fromHere must have split a mbyte char!
					cur = next;
				}
				Assert (false); return (0);		// previous character must exist!!!
			#endif
		}
	inline	Led_tChar*			Led_PreviousChar (Led_tChar* startOfString, Led_tChar* fromHere)
		{
			// We could duplicate all the code above - but its simpler to just cast and invoke
			// the above impemenation...
			return ((Led_tChar*)Led_PreviousChar ((const Led_tChar*)startOfString, (const Led_tChar*)fromHere));
		}



	inline	bool	ValidateTextForCharsetConformance (
							#if		qMultiByteCharacters
								const Led_tChar* text, size_t length
							#else
								const Led_tChar*, size_t
							#endif
						)
	{
		#if		qMultiByteCharacters
			return (Led_IsValidMultiByteString (text, length));
		#else
			return true;		// probably should do SOME validation here for other character sets - at least
								// for plain ascii!!! - LGP 950212
		#endif
	}

	inline	unsigned	Led_DigitCharToNumber (char digitChar)
		{
			// assume '0'..'9' are consecutive - true for ascii at least - LGP 961015

			// require input is valid decimal digit
			Require (digitChar >= '0');
			Require (digitChar <= '9');
			return (digitChar-'0');
		}
	inline	char		Led_NumberToDigitChar (unsigned digitValue)
		{
			// assume '0'..'9' are consecutive - true for ascii at least - LGP 961015

			// require input is valid decimal digit value
			Require (digitValue <= 9);
			return (digitValue+'0');
		}





	#if		qMultiByteCharacters
		inline	const Led_tChar*	Led_FindPrevOrEqualCharBoundary (const Led_tChar* start, const Led_tChar* guessedEnd)
			{
				if (guessedEnd == start) {
					return guessedEnd;
				}
	
		        // we go back by BYTES til we find a syncronization point
		        const Led_tChar* cur = guessedEnd-1;
	        	for (; cur > start; cur--) {
					if (not Led_IsLeadByte (*cur)) {
						// Then we are in case 1, 2, 3, 4 or 6 (not 5 or 7). So ew know we are looking
						// at an ASCII byte, or a secondbyte. Therefore - the NEXT byte from here must be
						// a valid mbyte char boundary.
						cur++;
						break;
					}
				}
				const Led_tChar* closestStart = cur;
				for (;;) {
					cur = Led_NextChar(cur);
					if (cur > guessedEnd) {
	                	break;
	                }
					closestStart = cur;
				}
				Assert ((closestStart == guessedEnd) or (closestStart == guessedEnd - 1));
				return closestStart;
			}
		inline	Led_tChar*			Led_FindPrevOrEqualCharBoundary (Led_tChar* start, Led_tChar* guessedEnd)
			{
				// We could duplicate all the code above - but its simpler to just cast and invoke
				// the above impemenation...
				return ((Led_tChar*)Led_FindPrevOrEqualCharBoundary ((const Led_tChar*)start, (const Led_tChar*)guessedEnd));
			}
	#endif









	inline	bool	Led_ClipboardObjectAcquire::FormatAvailable (Led_ClipFormat clipType)
		{
			#if		qMacOS
				#if		TARGET_CARBON
					ScrapRef			scrap	=	NULL;
					Led_ThrowIfOSStatus (::GetCurrentScrap (&scrap));
					ScrapFlavorFlags	flags	=	0;
					return (::GetScrapFlavorFlags (scrap, clipType, &flags) == noErr);
				#else
					long	scrapOffset	=	0;
					long	result		=	::GetScrap (NULL, clipType, &scrapOffset);
					return (result > 0);
				#endif
			#elif	qWindows
				return (::IsClipboardFormatAvailable (clipType));
			#elif	qXWindows
				// Wild guess - no good answer yet - LGP 2003-05-06
				return true;
			#endif
		}
	inline	bool	Led_ClipboardObjectAcquire::FormatAvailable_TEXT ()
		{
			if (FormatAvailable (kTEXTClipFormat)) {
				return true;
			}
			#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
			if (FormatAvailable (CF_TEXT)) {
				return true;
			}
			#endif
			return false;
		}
	inline	Led_ClipboardObjectAcquire::~Led_ClipboardObjectAcquire ()
		{
			// For windows me must unlock, but not delete
			#if		qWindows
				if (fLockedData != NULL) {
					::GlobalUnlock (fLockedData);
				}
			#endif
		
			// For mac me must delete - could unlock too - but no need
			#if		qMacOS
				if (fOSClipHandle != NULL) {
					::DisposeHandle (fOSClipHandle);
				}
			#endif
		}
	inline	bool	Led_ClipboardObjectAcquire::GoodClip () const
		{
			#if		qMacOS || qWindows
				return (fOSClipHandle != NULL and fLockedData != NULL);
			#else
				return false;	// X-TMP-HACK-LGP991213
			#endif
		}
	inline	void*	Led_ClipboardObjectAcquire::GetData () const
		{
			Assert (GoodClip ());
			return (fLockedData);
		}
	inline	size_t	Led_ClipboardObjectAcquire::GetDataLength () const
		{
			Assert (GoodClip ());
			#if		qMacOS
				return (::GetHandleSize (fOSClipHandle));
			#elif	qWindows
				return (::GlobalSize (fOSClipHandle));
			#endif
		}





		}
	}
}




	#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
		#pragma	warning (pop)
	#endif

#endif	/*__LedSupport_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

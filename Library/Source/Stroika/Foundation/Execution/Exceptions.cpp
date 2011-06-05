/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<errno.h>
#include	<windows.h>
#include	<winerror.h>
#include	<wininet.h>		// for error codes
#include	<tchar.h>

#include	"../IO/FileUtils.h"
#include	"../Debug/Trace.h"
#include	"../Characters/StringUtils.h"

#include	"Exceptions.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;


// for InternetGetConnectedState
#pragma comment(lib, "Wininet.lib")



using	namespace	StringUtils;
using	namespace	Exceptions;

using	Debug::Trace::TraceContextBumper;


namespace {
	inline	tstring	Win32Error2String_ (DWORD win32Err)
		{
			switch (win32Err) {
				case	ERROR_NOT_ENOUGH_MEMORY:	return _T ("Not enough memory to complete that operation (ERROR_NOT_ENOUGH_MEMORY)");
				case	ERROR_OUTOFMEMORY:			return _T ("Not enough memory to complete that operation (ERROR_OUTOFMEMORY)");
			}
			if (INTERNET_ERROR_BASE <= win32Err and win32Err < INTERNET_ERROR_BASE + INTERNET_ERROR_LAST) {
				switch (win32Err) {
					case	ERROR_INTERNET_INVALID_URL:					return _T ("ERROR_INTERNET_INVALID_URL");
					case	ERROR_INTERNET_CANNOT_CONNECT:				return _T ("Failed to connect to internet URL (ERROR_INTERNET_CANNOT_CONNECT)");
					case	ERROR_INTERNET_NAME_NOT_RESOLVED:			return _T ("ERROR_INTERNET_NAME_NOT_RESOLVED");
					case	ERROR_INTERNET_INCORRECT_HANDLE_STATE:		return _T ("ERROR_INTERNET_INCORRECT_HANDLE_STATE");
					case	ERROR_INTERNET_TIMEOUT:						return _T ("Operation timed out (ERROR_INTERNET_TIMEOUT)");
					case	ERROR_INTERNET_CONNECTION_ABORTED:			return _T ("ERROR_INTERNET_CONNECTION_ABORTED");
					case	ERROR_INTERNET_CONNECTION_RESET:			return _T ("ERROR_INTERNET_CONNECTION_RESET");
					case	ERROR_HTTP_INVALID_SERVER_RESPONSE:			return _T ("Invalid Server Response (ERROR_HTTP_INVALID_SERVER_RESPONSE)");
					case	ERROR_INTERNET_PROTOCOL_NOT_FOUND: {
						DWORD	r = 0;
						if (::InternetGetConnectedState (&r, 0) and (r & INTERNET_CONNECTION_OFFLINE)==0) {
							return _T ("ERROR_INTERNET_PROTOCOL_NOT_FOUND");
						}
						else {
							return _T ("ERROR_INTERNET_PROTOCOL_NOT_FOUND (offline mode)");
						}
					}
					default: {
						TCHAR	buf[1024];
						(void)::_stprintf_s (buf, _T ("INTERNET error code: %d"), win32Err);
						return buf;
					}
				}
			}
			TCHAR*	lpMsgBuf	=	NULL;
			if (not ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									NULL,
									win32Err,
									MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
									reinterpret_cast<TCHAR*> (&lpMsgBuf),
									0,
									NULL)
				) {
				return Format (_T ("Win32 error# %d"), static_cast<DWORD> (win32Err));
			}
			tstring	result	=	lpMsgBuf;
			::LocalFree (lpMsgBuf);
			return Trim (result);
		}
}




/*
 ********************************************************************************
 ******************************* Win32ErrorException ****************************
 ********************************************************************************
 */
namespace	{
	struct	AssertionChecker {
		AssertionChecker ()
		{
			Assert (Win32ErrorException::kERROR_INTERNET_TIMEOUT == ERROR_INTERNET_TIMEOUT);
			Assert (Win32ErrorException::kERROR_INTERNET_INVALID_URL == ERROR_INTERNET_INVALID_URL);
			Assert (Win32ErrorException::kERROR_INTERNET_UNRECOGNIZED_SCHEME == ERROR_INTERNET_UNRECOGNIZED_SCHEME);
			Assert (Win32ErrorException::kERROR_INTERNET_NAME_NOT_RESOLVED == ERROR_INTERNET_NAME_NOT_RESOLVED);
			Assert (Win32ErrorException::kERROR_INTERNET_PROTOCOL_NOT_FOUND == ERROR_INTERNET_PROTOCOL_NOT_FOUND);
			Assert (Win32ErrorException::kERROR_INTERNET_CANNOT_CONNECT == ERROR_INTERNET_CANNOT_CONNECT);
		}
	}	sAssertionChecker;
}
void	Win32ErrorException::DoThrow (DWORD error)
{
	switch (error) {
		case	ERROR_SUCCESS: {
			DbgTrace ("Win32ErrorException::DoThrow (ERROR_SUCCESS) - throwing Win32ErrorException (ERROR_NOT_SUPPORTED)");
			throw Win32ErrorException (ERROR_NOT_SUPPORTED);	// unsure WHAT to throw here - SOMETHING failed - but GetLastError () must have given
																// a bad reason code?
		}
		case	ERROR_NOT_ENOUGH_MEMORY:
		case	ERROR_OUTOFMEMORY: {
			DbgTrace ("Win32ErrorException::DoThrow (0x%x) - throwing bad_alloc", error);
			throw bad_alloc ();
		}
		case	ERROR_SHARING_VIOLATION: {
			DbgTrace ("Win32ErrorException::DoThrow (0x%x) - throwing FileBusyException", error);
			throw FileBusyException ();
		}
		case ERROR_FILE_NOT_FOUND: {
			DbgTrace ("Win32ErrorException::DoThrow (0x%x) - throwing FileAccessException", error);
			throw FileUtils::FileAccessException ();	// don't know if they were reading or writing at this level..., and don't know file name...
		}
		case ERROR_PATH_NOT_FOUND: {
			DbgTrace ("Win32ErrorException::DoThrow (0x%x) - throwing FileAccessException", error);
			throw FileUtils::FileAccessException ();	// don't know if they were reading or writing at this level..., and don't know file name...
		}
		default: {
			DbgTrace ("Win32ErrorException::DoThrow (0x%x) - throwing Win32ErrorException", error);
			throw Win32ErrorException (error);
		}
	}
}

tstring	Win32ErrorException::LookupMessage (DWORD dw)
{
	return Win32Error2String_ (dw);
}






/*
 ********************************************************************************
 ************************** Win32StructuredException ****************************
 ********************************************************************************
 */
void	Win32StructuredException::RegisterHandler ()
{
	_set_se_translator (trans_func);
}

void	Win32StructuredException::trans_func (unsigned int u, EXCEPTION_POINTERS* pExp)
{
	TraceContextBumper	ctx (_T ("Win32StructuredException::trans_func"));
	{
		// I wish I knew how to get a PROCNAME of where the caller was...
		DbgTrace (_T ("u = 0x%x (%s)"), u, LookupMessage (u).c_str ());
		if (pExp != NULL) {
			if (pExp->ContextRecord != NULL) {
				TraceContextBumper	ctx (_T ("ContextRecord"));
				DbgTrace ("ContextRecord->ContextFlags = 0x%x", pExp->ContextRecord->ContextFlags);
				DbgTrace ("ContextRecord->Dr0 = 0x%x", pExp->ContextRecord->Dr0);
				DbgTrace ("ContextRecord->Esp = 0x%x", pExp->ContextRecord->Esp);
			}
			if (pExp->ExceptionRecord != NULL) {
				TraceContextBumper	ctx (_T ("ExceptionRecord"));
				DbgTrace ("ExceptionRecord->ExceptionAddress = 0x%x", pExp->ExceptionRecord->ExceptionAddress);
				DbgTrace ("ExceptionRecord->ExceptionCode = 0x%x", pExp->ExceptionRecord->ExceptionCode);
				DbgTrace ("ExceptionRecord->ExceptionFlags = 0x%x", pExp->ExceptionRecord->ExceptionFlags);
				DbgTrace ("ExceptionRecord->NumberParameters = %d", pExp->ExceptionRecord->NumberParameters);
			}
		}
	}
	DbgTrace ("Translating it into a Win32StructuredException::THROW()");
	Assert (false);	// in case debug turned on, helpful to drop into the debugger here!
	throw Win32StructuredException (u);
}

tstring	Win32StructuredException::LookupMessage (unsigned int u)
{
	switch (u) {
		case	EXCEPTION_ACCESS_VIOLATION:			return _T ("EXCEPTION_ACCESS_VIOLATION");
		case	EXCEPTION_DATATYPE_MISALIGNMENT:	return _T ("EXCEPTION_DATATYPE_MISALIGNMENT");
		case	EXCEPTION_ARRAY_BOUNDS_EXCEEDED:	return _T ("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		case	EXCEPTION_FLT_DENORMAL_OPERAND:		return _T ("EXCEPTION_FLT_DENORMAL_OPERAND");
		case	EXCEPTION_FLT_DIVIDE_BY_ZERO:		return _T ("EXCEPTION_FLT_DENORMAL_OPERAND");
		case	EXCEPTION_FLT_INEXACT_RESULT:		return _T ("EXCEPTION_FLT_INEXACT_RESULT");
		case	EXCEPTION_FLT_INVALID_OPERATION:	return _T ("EXCEPTION_FLT_INVALID_OPERATION");
		case	EXCEPTION_FLT_OVERFLOW:				return _T ("EXCEPTION_FLT_OVERFLOW");
		case	EXCEPTION_FLT_STACK_CHECK:			return _T ("EXCEPTION_FLT_STACK_CHECK");
		case	EXCEPTION_FLT_UNDERFLOW:			return _T ("EXCEPTION_FLT_UNDERFLOW");
		case	EXCEPTION_INT_DIVIDE_BY_ZERO:		return _T ("EXCEPTION_INT_DIVIDE_BY_ZERO");
		case	EXCEPTION_INT_OVERFLOW:				return _T ("EXCEPTION_INT_OVERFLOW");
		case	EXCEPTION_PRIV_INSTRUCTION:			return _T ("EXCEPTION_PRIV_INSTRUCTION");
		case	EXCEPTION_IN_PAGE_ERROR:			return _T ("EXCEPTION_IN_PAGE_ERROR");
		case	EXCEPTION_ILLEGAL_INSTRUCTION:		return _T ("EXCEPTION_ILLEGAL_INSTRUCTION");
		case	EXCEPTION_NONCONTINUABLE_EXCEPTION:	return _T ("EXCEPTION_NONCONTINUABLE_EXCEPTION");
		case	EXCEPTION_STACK_OVERFLOW:			return _T ("EXCEPTION_STACK_OVERFLOW");
		case	EXCEPTION_INVALID_DISPOSITION:		return _T ("EXCEPTION_INVALID_DISPOSITION");
		case	EXCEPTION_GUARD_PAGE:				return _T ("EXCEPTION_GUARD_PAGE");
		case	EXCEPTION_INVALID_HANDLE:			return _T ("EXCEPTION_INVALID_HANDLE");
		default: {
			TCHAR	buf[1024];
			(void)::_stprintf_s (buf, _T ("Win32 Structured Exception Code - 0x%x"), u);
			return buf;	
		}
	}
}






/*
 ********************************************************************************
 ***************************** HRESULTErrorException ****************************
 ********************************************************************************
 */
tstring	HRESULTErrorException::LookupMessage (HRESULT hr)
{
	switch (hr) {
		case	E_FAIL:						return _T ("HRESULT failure (E_FAIL)");
		case	E_ACCESSDENIED:				return _T ("HRESULT failure (E_ACCESSDENIED)");
		case	E_INVALIDARG:				return _T ("HRESULT failure (E_INVALIDARG)");
		case	E_NOINTERFACE:				return _T ("HRESULT failure (E_NOINTERFACE)");
		case	E_POINTER:					return _T ("HRESULT failure (E_POINTER)");
		case	E_HANDLE:					return _T ("HRESULT failure (E_HANDLE)");
		case	E_ABORT:					return _T ("HRESULT failure (E_ABORT)");
		case	DISP_E_TYPEMISMATCH:		return _T ("HRESULT failure (DISP_E_TYPEMISMATCH)");
		case	DISP_E_EXCEPTION:			return _T ("HRESULT failure (DISP_E_EXCEPTION)");
		case	INET_E_RESOURCE_NOT_FOUND:	return _T ("HRESULT failure (INET_E_RESOURCE_NOT_FOUND)");
		case	REGDB_E_CLASSNOTREG:		return _T ("HRESULT failure (REGDB_E_CLASSNOTREG)");
		case	ERROR_NOT_ENOUGH_MEMORY:	return _T ("Not enough memory to complete that operation (ERROR_NOT_ENOUGH_MEMORY)");
		case	ERROR_OUTOFMEMORY:			return _T ("Not enough memory to complete that operation (ERROR_OUTOFMEMORY)");
	}
	if (HRESULT_FACILITY (hr) == FACILITY_WIN32) {
		return Win32Error2String_ (HRESULT_CODE (hr));
	}
	if (HRESULT_FACILITY (hr) == FACILITY_INTERNET) {
		unsigned int	wCode	=		HRESULT_CODE (hr);
		if (wCode < INTERNET_ERROR_BASE) {
			wCode += INTERNET_ERROR_BASE;		// because the HRESULT_CODE doesn't (at least sometimes) include the INTERNET_ERROR_BASE
												// included in the constants below...
		}
		return Win32Error2String_ (wCode);
#if 0
		switch (wCode) {
			case	ERROR_INTERNET_INVALID_URL:					return _T ("ERROR_INTERNET_INVALID_URL");
			case	ERROR_INTERNET_CANNOT_CONNECT:				return _T ("Failed to connect to internet URL (ERROR_INTERNET_CANNOT_CONNECT)");
			case	ERROR_INTERNET_NAME_NOT_RESOLVED:			return _T ("ERROR_INTERNET_NAME_NOT_RESOLVED");
			case	ERROR_INTERNET_INCORRECT_HANDLE_STATE:		return _T ("ERROR_INTERNET_INCORRECT_HANDLE_STATE");
			case	ERROR_INTERNET_TIMEOUT:						return _T ("Operation timed out (ERROR_INTERNET_TIMEOUT)");
			case	ERROR_INTERNET_CONNECTION_ABORTED:			return _T ("ERROR_INTERNET_CONNECTION_ABORTED");
			case	ERROR_INTERNET_CONNECTION_RESET:			return _T ("ERROR_INTERNET_CONNECTION_RESET");
			case	ERROR_INTERNET_PROTOCOL_NOT_FOUND: {
				DWORD	r = 0;
				if (::InternetGetConnectedState (&r, 0) and (r & INTERNET_CONNECTION_OFFLINE)==0) {
					return _T ("ERROR_INTERNET_PROTOCOL_NOT_FOUND");
				}
				else {
					return _T ("ERROR_INTERNET_PROTOCOL_NOT_FOUND (offline mode)");
				}
			}
			default: {
				TCHAR	buf[1024];
				(void)::_stprintf_s (buf, _T ("HRESULT FACILITY_INTERNET error code: %d"), HRESULT_CODE (hr));
				return buf;
			}
		}
#endif
	}
	TCHAR	buf[1024];
	(void)::_stprintf_s (buf, _T ("HRESULT error code: 0x%x"), hr);
	return buf;	
}









/*
 ********************************************************************************
 ***************************** errno_ErrorException *****************************
 ********************************************************************************
 */
errno_ErrorException::errno_ErrorException (errno_t e):
	StringException (tstring2Wide (LookupMessage (e))),
	fError (e)
{
}

tstring	errno_ErrorException::LookupMessage (errno_t e)
{
#if 0
	switch (hr) {
		case	E_FAIL:						return _T ("HRESULT failure (E_FAIL)");
	}
#endif
	TCHAR	buf[2048];
	buf[0] = '\0';
	if (_tcserror_s (buf, e) != 0) {
		return buf;
	}
	(void)::_stprintf_s (buf, _T ("errno_t error code: 0x%x"), e);
	return buf;	
}

void	errno_ErrorException::DoThrow (DWORD error)
{
	switch (error) {
		case	ENOMEM: {
			Exceptions::DoThrow (bad_alloc (), "errno_ErrorException::DoThrow (ENOMEM) - throwing bad_alloc");
		}
		// If I decide to pursue mapping, this maybe a good place to start
		//	http://aplawrence.com/Unixart/errors.html
		//		-- LGP 2009-01-02
#if 0
		case	EPERM: {
			// not sure any point in this unification. Maybe if I added my OWN private 'access denied' exception
			// the mapping/unification would make sense.
			//		-- LGP 2009-01-02
			DbgTrace ("errno_ErrorException::DoThrow (EPERM) - throwing ERROR_ACCESS_DENIED");
			throw Win32Exception (ERROR_ACCESS_DENIED);
		}
#endif
	}
	DbgTrace ("errno_ErrorException (0x%x) - throwing errno_ErrorException", error);
	throw errno_ErrorException (error);
}







/*
 ********************************************************************************
 ********************** RequiredComponentMissingException ***********************
 ********************************************************************************
 */
const	wchar_t	RequiredComponentMissingException::kJava[]			=	L"Java";
const	wchar_t	RequiredComponentMissingException::kPDFViewer[]		=	L"PDFViewer";
const	wchar_t	RequiredComponentMissingException::kPDFOCXViewer[]	=	L"PDFOCXViewer";
namespace	{
	wstring	mkMsg (const wstring& component)
		{
			wstring	cName	=	component;
			if (cName == RequiredComponentMissingException::kPDFViewer)			{ cName = L"PDF Viewer";		}
			else if (cName == RequiredComponentMissingException::kPDFOCXViewer)	{ cName = L"PDF Viewer (OCX)";	}
			wstring	x	=	Format (L"A required component - %s - is missing, or is out of date", component.c_str ());
			return x;
		}
}
RequiredComponentMissingException::RequiredComponentMissingException (const wstring& component):
	StringException (mkMsg (component)),
	fComponent (component)
{
}








/*
 ********************************************************************************
 **************** RequiredComponentVersionMismatchException *********************
 ********************************************************************************
 */
namespace	{
	wstring	mkMsg (const wstring& component, const wstring& requiredVersion)
		{
			wstring	x	=	Format (L"A required component - %s - is missing, or is out of date", component.c_str ());
			if (not requiredVersion.empty ()) {
				x += Format (L"; version '%s' is required", requiredVersion.c_str ());
			}
			return x;
		}
}
RequiredComponentVersionMismatchException::RequiredComponentVersionMismatchException (const wstring& component, const wstring& requiredVersion):
	StringException (mkMsg (component, requiredVersion))
{
}






/*
 ********************************************************************************
 **************** FeatureNotSupportedInThisVersionException *********************
 ********************************************************************************
 */
FeatureNotSupportedInThisVersionException::FeatureNotSupportedInThisVersionException (const wstring& feature):
	StringException (Format (L"%s is not supported in this version of HealthFrame: see the documentation (F1) on features in this version", feature.c_str ())),
	fFeature (feature)
{
}








/*
 ********************************************************************************
 ***************************** ThrowIfShellExecError ****************************
 ********************************************************************************
 */
void	Exceptions::ThrowIfShellExecError (HINSTANCE r)
{
	int	errCode	=	reinterpret_cast<int> (r);
	if (errCode <= 32) {
		DbgTrace ("ThrowIfShellExecError (0x%x) - throwing exception", errCode);
		switch (errCode) {
			case	0:						Win32ErrorException::DoThrow (ERROR_NOT_ENOUGH_MEMORY);	// The operating system is out of memory or resources. 
			case	ERROR_FILE_NOT_FOUND:	Win32ErrorException::DoThrow (ERROR_FILE_NOT_FOUND);	// The specified file was not found. 
			case	ERROR_PATH_NOT_FOUND:	Win32ErrorException::DoThrow (ERROR_PATH_NOT_FOUND);	//  The specified path was not found. 
			case	ERROR_BAD_FORMAT:		Win32ErrorException::DoThrow (ERROR_BAD_FORMAT);		//  The .exe file is invalid (non-Microsoft Win32® .exe or error in .exe image). 
			case	SE_ERR_ACCESSDENIED:	throw (HRESULTErrorException (E_ACCESSDENIED));			//  The operating system denied access to the specified file. 
			case	SE_ERR_ASSOCINCOMPLETE:	Win32ErrorException::DoThrow (ERROR_NO_ASSOCIATION);	//  The file name association is incomplete or invalid. 
			case	SE_ERR_DDEBUSY:			Win32ErrorException::DoThrow (ERROR_DDE_FAIL);			//  The Dynamic Data Exchange (DDE) transaction could not be completed because other DDE transactions were being processed. 
			case	SE_ERR_DDEFAIL:			Win32ErrorException::DoThrow (ERROR_DDE_FAIL);			//  The DDE transaction failed. 
			case	SE_ERR_DDETIMEOUT:		Win32ErrorException::DoThrow (ERROR_DDE_FAIL);			//  The DDE transaction could not be completed because the request timed out. 
			case	SE_ERR_DLLNOTFOUND:		Win32ErrorException::DoThrow (ERROR_DLL_NOT_FOUND);		//  The specified dynamic-link library (DLL) was not found. 
			//case	SE_ERR_FNF:				throw (Win32ErrorException (ERROR_FILE_NOT_FOUND));		//  The specified file was not found. 
			case	SE_ERR_NOASSOC:			Win32ErrorException::DoThrow (ERROR_NO_ASSOCIATION);	//  There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable. 
			case	SE_ERR_OOM:				Win32ErrorException::DoThrow (ERROR_NOT_ENOUGH_MEMORY);	//  There was not enough memory to complete the operation. 
			//case	SE_ERR_PNF:				throw (Win32ErrorException (ERROR_PATH_NOT_FOUND));		//  The specified path was not found. 
			case	SE_ERR_SHARE:			Win32ErrorException::DoThrow (ERROR_INVALID_SHARENAME);	//  
			default: {
				// Not sure what error to report here...
				Win32ErrorException::DoThrow (ERROR_NO_ASSOCIATION);
			}
		}
	}
}





/*
 ********************************************************************************
 ********** Exceptions::RegisterDefaultHandler_invalid_parameter ****************
 ********************************************************************************
 */
	namespace	{
		/*
		 *	Because of Microsoft's new secure-runtime-lib  - we must provide a handler to catch errors (shouldn't occur - but in case.
		 *	We treat these largely like ASSERTION errors, but then translate them into a THROW of an exception - since that is
		 *	probably more often the right thing todo.
		 */
		void	invalid_parameter_handler_ (const wchar_t * expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
			{
				TraceContextBumper	trcCtx (_T ("invalid_parameter_handler"));
				DbgTrace  (L"Func='%s', expr='%s', file='%s'.", function, expression, file);
				Assert (false);
				throw Win32ErrorException (ERROR_INVALID_PARAMETER);
			}
	}
void	Exceptions::RegisterDefaultHandler_invalid_parameter ()
{
	(void)_set_invalid_parameter_handler (invalid_parameter_handler_);
}



/*
 ********************************************************************************
 ********** Exceptions::RegisterDefaultHandler_pure_function_call ***************
 ********************************************************************************
 */
	namespace	{
		void	purecall_handler_ ()
			{
				TraceContextBumper	trcCtx (_T ("purecall_handler_"));
				Assert (false);
				throw Win32ErrorException (ERROR_INVALID_PARAMETER);	// not sure better # / exception to throw?
			}
	}
void	Exceptions::RegisterDefaultHandler_pure_function_call ()
{
	(void)_set_purecall_handler (purecall_handler_);
}





//	class	CatchAndCaptureExceptionHelper
CatchAndCaptureExceptionHelper::CatchAndCaptureExceptionHelper ()
	: fHRESULTErrorException ()
	, fWin32ErrorException ()
	, fStringException ()
	, fFileFormatException ()
	, fFileBusyException ()
	, fSilentException ()
	, fRequiredComponentMissingException ()
{
}

void	CatchAndCaptureExceptionHelper::DoRunWithCatchRePropagate (Callback* callback)
{
	// Subclassers add additional 'exceptions' as data members, and override this to catch additioanl fields, and
	// AnyExceptionCaught/RethrowIfAnyCaught to rethrow each
	RequireNotNil (callback);
	try {
		callback->DoItInsideCatcher ();
	}
	CATCH_AND_CAPTURE_CATCH_BLOCK(*this);
}

bool	CatchAndCaptureExceptionHelper::AnyExceptionCaught () const
{
	return 
		fHRESULTErrorException.get () != NULL or 
		fWin32ErrorException.get () != NULL or 
		fStringException.get () != NULL or 
		fFileFormatException.get () != NULL or
		fFileBusyException.get () != NULL or
		fSilentException.get () != NULL or
		fRequiredComponentMissingException.get () != NULL
		;
}

void	CatchAndCaptureExceptionHelper::RethrowIfAnyCaught () const
{
	if (fHRESULTErrorException.get () != NULL) {
		throw *fHRESULTErrorException.get ();
	}
	if (fWin32ErrorException.get () != NULL) {
		throw *fWin32ErrorException.get ();
	}
	if (fStringException.get () != NULL) {
		throw *fStringException.get ();
	}
	if (fFileFormatException.get () != NULL) {
		throw *fFileFormatException.get ();
	}
	if (fFileBusyException.get () != NULL) {
		throw *fFileBusyException.get ();
	}
	if (fSilentException.get () != NULL) {
		throw *fSilentException.get ();
	}
	if (fRequiredComponentMissingException.get () != NULL) {
		throw *fRequiredComponentMissingException.get ();
	}
}


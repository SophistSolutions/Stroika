/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<errno.h>

#include	"../Characters/StringUtils.h"
#include	"../Debug/Trace.h"
#include	"../IO/FileAccessException.h"
#include	"../IO/FileUtils.h"

#include	"Exceptions.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;




using	namespace	Characters;
using	namespace	Execution;

using	Debug::TraceContextBumper;










/*
 ********************************************************************************
 ***************************** errno_ErrorException *****************************
 ********************************************************************************
 */
errno_ErrorException::errno_ErrorException (errno_t e)
	: StringException (TString2Wide (LookupMessage (e)))
	, fError (e)
{
}

TString	errno_ErrorException::LookupMessage (errno_t e)
{
#if 0
	switch (hr) {
		case	E_FAIL:						return TSTR ("HRESULT failure (E_FAIL)");
	}
#endif
	TChar	buf[2048];
	buf[0] = '\0';
#if		qPlatform_Windows
	if (_tcserror_s (buf, e) != 0) {
		return buf;
	}
	(void)::_stprintf_s (buf, TSTR ("errno_t error code: 0x%x"), e);
#else
	AssertNotImplemented ();
#endif
	return buf;	
}

void	errno_ErrorException::DoThrow (errno_t error)
{
	switch (error) {
		case	ENOMEM: {
			Execution::DoThrow (bad_alloc (), "errno_ErrorException::DoThrow (ENOMEM) - throwing bad_alloc");
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
FeatureNotSupportedInThisVersionException::FeatureNotSupportedInThisVersionException (const wstring& feature)
	: StringException (Format (L"%s is not supported in this version of HealthFrame: see the documentation (F1) on features in this version", feature.c_str ()))
	, fFeature (feature)
{
}








#if		qPlatform_Windows
/*
 ********************************************************************************
 ***************************** ThrowIfShellExecError ****************************
 ********************************************************************************
 */
void	Execution::ThrowIfShellExecError (HINSTANCE r)
{
	int	errCode	=	reinterpret_cast<int> (r);
	if (errCode <= 32) {
		DbgTrace ("ThrowIfShellExecError (0x%x) - throwing exception", errCode);
		switch (errCode) {
			case	0:						Platform::Windows::Exception::DoThrow (ERROR_NOT_ENOUGH_MEMORY);	// The operating system is out of memory or resources. 
			case	ERROR_FILE_NOT_FOUND:	Platform::Windows::Exception::DoThrow (ERROR_FILE_NOT_FOUND);	// The specified file was not found. 
			case	ERROR_PATH_NOT_FOUND:	Platform::Windows::Exception::DoThrow (ERROR_PATH_NOT_FOUND);	//  The specified path was not found. 
			case	ERROR_BAD_FORMAT:		Platform::Windows::Exception::DoThrow (ERROR_BAD_FORMAT);		//  The .exe file is invalid (non-Microsoft Win32® .exe or error in .exe image). 
			case	SE_ERR_ACCESSDENIED:	throw (Platform::Windows::HRESULTErrorException (E_ACCESSDENIED));			//  The operating system denied access to the specified file. 
			case	SE_ERR_ASSOCINCOMPLETE:	Platform::Windows::Exception::DoThrow (ERROR_NO_ASSOCIATION);	//  The file name association is incomplete or invalid. 
			case	SE_ERR_DDEBUSY:			Platform::Windows::Exception::DoThrow (ERROR_DDE_FAIL);			//  The Dynamic Data Exchange (DDE) transaction could not be completed because other DDE transactions were being processed. 
			case	SE_ERR_DDEFAIL:			Platform::Windows::Exception::DoThrow (ERROR_DDE_FAIL);			//  The DDE transaction failed. 
			case	SE_ERR_DDETIMEOUT:		Platform::Windows::Exception::DoThrow (ERROR_DDE_FAIL);			//  The DDE transaction could not be completed because the request timed out. 
			case	SE_ERR_DLLNOTFOUND:		Platform::Windows::Exception::DoThrow (ERROR_DLL_NOT_FOUND);		//  The specified dynamic-link library (DLL) was not found. 
			//case	SE_ERR_FNF:				throw (Platform::Windows::Exception (ERROR_FILE_NOT_FOUND));		//  The specified file was not found. 
			case	SE_ERR_NOASSOC:			Platform::Windows::Exception::DoThrow (ERROR_NO_ASSOCIATION);	//  There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable. 
			case	SE_ERR_OOM:				Platform::Windows::Exception::DoThrow (ERROR_NOT_ENOUGH_MEMORY);	//  There was not enough memory to complete the operation. 
			//case	SE_ERR_PNF:				throw (Platform::Windows::Exception (ERROR_PATH_NOT_FOUND));		//  The specified path was not found. 
			case	SE_ERR_SHARE:			Platform::Windows::Exception::DoThrow (ERROR_INVALID_SHARENAME);	//  
			default: {
				// Not sure what error to report here...
				Platform::Windows::Exception::DoThrow (ERROR_NO_ASSOCIATION);
			}
		}
	}
}
#endif





#if		qPlatform_Windows
/*
 ********************************************************************************
 ********** Execution::RegisterDefaultHandler_invalid_parameter ****************
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
				TraceContextBumper	trcCtx (TSTR ("invalid_parameter_handler"));
				DbgTrace  (L"Func='%s', expr='%s', file='%s'.", function, expression, file);
				Assert (false);
				throw Platform::Windows::Exception (ERROR_INVALID_PARAMETER);
			}
	}
void	Execution::RegisterDefaultHandler_invalid_parameter ()
{
	(void)_set_invalid_parameter_handler (invalid_parameter_handler_);
}
#endif





#if		qPlatform_Windows
/*
 ********************************************************************************
 ********** Execution::RegisterDefaultHandler_pure_function_call ***************
 ********************************************************************************
 */
	namespace	{
		void	purecall_handler_ ()
			{
				TraceContextBumper	trcCtx (TSTR ("purecall_handler_"));
				Assert (false);
				throw Platform::Windows::Exception (ERROR_INVALID_PARAMETER);	// not sure better # / exception to throw?
			}
	}
void	Execution::RegisterDefaultHandler_pure_function_call ()
{
	(void)_set_purecall_handler (purecall_handler_);
}
#endif





//	class	CatchAndCaptureExceptionHelper
CatchAndCaptureExceptionHelper::CatchAndCaptureExceptionHelper ()
	: fStringException ()
	, fFileFormatException ()
	, fFileBusyException ()
	, fSilentException ()
	, fRequiredComponentMissingException ()
#if		qPlatform_Windows
	, fHRESULTErrorException ()
	, fWin32ErrorException ()
#endif
{
}

void	CatchAndCaptureExceptionHelper::DoRunWithCatchRePropagate (Callback* callback)
{
	// Subclassers add additional 'Execution' as data members, and override this to catch additioanl fields, and
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
		fStringException.get () != NULL
		or fFileFormatException.get () != NULL
		or fFileBusyException.get () != NULL
		or fSilentException.get () != NULL
		or fRequiredComponentMissingException.get () != NULL
#if		qPlatform_Windows
		or fHRESULTErrorException.get () != NULL
		or fWin32ErrorException.get () != NULL
#endif
		;
}

void	CatchAndCaptureExceptionHelper::RethrowIfAnyCaught () const
{
#if		qPlatform_Windows
	if (fHRESULTErrorException.get () != NULL) {
		throw *fHRESULTErrorException.get ();
	}
	if (fWin32ErrorException.get () != NULL) {
		throw *fWin32ErrorException.get ();
	}
#endif
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


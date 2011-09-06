/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
	#include	<winerror.h>
	#include	<wininet.h>		// for error codes
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../../Characters/StringUtils.h"
#include	"../../../Configuration/Common.h"
#include	"../../../Containers/Common.h"
#include	"../../../Debug/Trace.h"
#include	"../../../IO/FileAccessException.h"
#include	"../../../Time/Realtime.h"

#include	"Exception.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Debug;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::Execution::Platform;
using	namespace	Stroika::Foundation::Execution::Platform::Windows;






// for InternetGetConnectedState
#pragma comment(lib, "Wininet.lib")





#if		qPlatform_Windows
namespace {
	inline	TString	Win32Error2String_ (DWORD win32Err)
		{
			switch (win32Err) {
				case	ERROR_NOT_ENOUGH_MEMORY:	return TSTR ("Not enough memory to complete that operation (ERROR_NOT_ENOUGH_MEMORY)");
				case	ERROR_OUTOFMEMORY:			return TSTR ("Not enough memory to complete that operation (ERROR_OUTOFMEMORY)");
			}
			if (INTERNET_ERROR_BASE <= win32Err and win32Err < INTERNET_ERROR_BASE + INTERNET_ERROR_LAST) {
				switch (win32Err) {
					case	ERROR_INTERNET_INVALID_URL:					return TSTR ("ERROR_INTERNET_INVALID_URL");
					case	ERROR_INTERNET_CANNOT_CONNECT:				return TSTR ("Failed to connect to internet URL (ERROR_INTERNET_CANNOT_CONNECT)");
					case	ERROR_INTERNET_NAME_NOT_RESOLVED:			return TSTR ("ERROR_INTERNET_NAME_NOT_RESOLVED");
					case	ERROR_INTERNET_INCORRECT_HANDLE_STATE:		return TSTR ("ERROR_INTERNET_INCORRECT_HANDLE_STATE");
					case	ERROR_INTERNET_TIMEOUT:						return TSTR ("Operation timed out (ERROR_INTERNET_TIMEOUT)");
					case	ERROR_INTERNET_CONNECTION_ABORTED:			return TSTR ("ERROR_INTERNET_CONNECTION_ABORTED");
					case	ERROR_INTERNET_CONNECTION_RESET:			return TSTR ("ERROR_INTERNET_CONNECTION_RESET");
					case	ERROR_HTTP_INVALID_SERVER_RESPONSE:			return TSTR ("Invalid Server Response (ERROR_HTTP_INVALID_SERVER_RESPONSE)");
					case	ERROR_INTERNET_PROTOCOL_NOT_FOUND: {
						DWORD	r = 0;
						if (::InternetGetConnectedState (&r, 0) and (r & INTERNET_CONNECTION_OFFLINE)==0) {
							return TSTR ("ERROR_INTERNET_PROTOCOL_NOT_FOUND");
						}
						else {
							return TSTR ("ERROR_INTERNET_PROTOCOL_NOT_FOUND (offline mode)");
						}
					}
					default: {
						TCHAR	buf[1024];
						(void)::_stprintf_s (buf, TSTR ("INTERNET error code: %d"), win32Err);
						return buf;
					}
				}
			}
			TCHAR*	lpMsgBuf	=	nullptr;
			if (not ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									nullptr,
									win32Err,
									MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
									reinterpret_cast<TCHAR*> (&lpMsgBuf),
									0,
									nullptr)
				) {
				return Format (TSTR ("Win32 error# %d"), static_cast<DWORD> (win32Err));
			}
			TString	result	=	lpMsgBuf;
			::LocalFree (lpMsgBuf);
			return Trim (result);
		}
}
#endif










#if		qPlatform_Windows
/*
 ********************************************************************************
 *********************** Platform::Windows::Exception ***************************
 ********************************************************************************
 */
void	Execution::Platform::Windows::Exception::DoThrow (DWORD error)
{
	switch (error) {
		case	ERROR_SUCCESS: {
			DbgTrace ("Platform::Windows::Exception::DoThrow (ERROR_SUCCESS) - throwing Platform::Windows::Exception (ERROR_NOT_SUPPORTED)");
			throw Platform::Windows::Exception (ERROR_NOT_SUPPORTED);	// unsure WHAT to throw here - SOMETHING failed - but GetLastError () must have given
																// a bad reason code?
		}
		case	ERROR_NOT_ENOUGH_MEMORY:
		case	ERROR_OUTOFMEMORY: {
			DbgTrace ("Platform::Windows::Exception::DoThrow (0x%x) - throwing bad_alloc", error);
			throw bad_alloc ();
		}
		case	ERROR_SHARING_VIOLATION: {
			DbgTrace ("Platform::Windows::Exception::DoThrow (0x%x) - throwing FileBusyException", error);
			throw IO::FileBusyException ();
		}
		case ERROR_FILE_NOT_FOUND: {
			DbgTrace ("Platform::Windows::Exception::DoThrow (0x%x) - throwing FileAccessException", error);
			throw IO::FileAccessException ();	// don't know if they were reading or writing at this level..., and don't know file name...
		}
		case ERROR_PATH_NOT_FOUND: {
			DbgTrace ("Platform::Windows::Exception::DoThrow (0x%x) - throwing FileAccessException", error);
			throw IO::FileAccessException ();	// don't know if they were reading or writing at this level..., and don't know file name...
		}
		default: {
			DbgTrace ("Platform::Windows::Exception::DoThrow (0x%x) - throwing Platform::Windows::Exception", error);
			throw Platform::Windows::Exception (error);
		}
	}
}

TString	Execution::Platform::Windows::Exception::LookupMessage (DWORD dw)
{
	return Win32Error2String_ (dw);
}
#endif

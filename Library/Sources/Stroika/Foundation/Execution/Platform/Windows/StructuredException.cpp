/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../../Configuration/Common.h"
#include	"../../../Containers/Common.h"
#include	"../../../Debug/Trace.h"
#include	"../../../Time/Realtime.h"

#include	"StructuredException.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Debug;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::Execution::Platform;
using	namespace	Stroika::Foundation::Execution::Platform::Windows;






/*
 ********************************************************************************
 **************** Platform::Windows::StructuredException ************************
 ********************************************************************************
 */
void	Platform::Windows::StructuredException::RegisterHandler ()
{
	_set_se_translator (trans_func_);
}

void	Platform::Windows::StructuredException::trans_func_ (unsigned int u, EXCEPTION_POINTERS* pExp)
{
	TraceContextBumper	ctx (TSTR ("Platform::Windows::StructuredException::trans_func_"));
	{
		// I wish I knew how to get a PROCNAME of where the caller was...
		DbgTrace (TSTR ("u = 0x%x (%s)"), u, LookupMessage (u).c_str ());
		if (pExp != nullptr) {
			if (pExp->ContextRecord != nullptr) {
				TraceContextBumper	ctx (TSTR ("ContextRecord"));
				DbgTrace ("ContextRecord->ContextFlags = 0x%x", pExp->ContextRecord->ContextFlags);
				DbgTrace ("ContextRecord->Dr0 = 0x%x", pExp->ContextRecord->Dr0);
			#if		qPlatform_Win32
				DbgTrace ("ContextRecord->Esp = 0x%x", pExp->ContextRecord->Esp);
			#endif
			}
			if (pExp->ExceptionRecord != nullptr) {
				TraceContextBumper	ctx (TSTR ("ExceptionRecord"));
				DbgTrace ("ExceptionRecord->ExceptionAddress = 0x%x", pExp->ExceptionRecord->ExceptionAddress);
				DbgTrace ("ExceptionRecord->ExceptionCode = 0x%x", pExp->ExceptionRecord->ExceptionCode);
				DbgTrace ("ExceptionRecord->ExceptionFlags = 0x%x", pExp->ExceptionRecord->ExceptionFlags);
				DbgTrace ("ExceptionRecord->NumberParameters = %d", pExp->ExceptionRecord->NumberParameters);
			}
		}
	}
	DbgTrace ("Translating it into a Platform::Windows::StructuredException::THROW()");
	Assert (false);	// in case debug turned on, helpful to drop into the debugger here!
	throw Platform::Windows::StructuredException (u);
}

TString	Platform::Windows::StructuredException::LookupMessage (unsigned int u)
{
	switch (u) {
		case	EXCEPTION_ACCESS_VIOLATION:			return TSTR ("EXCEPTION_ACCESS_VIOLATION");
		case	EXCEPTION_DATATYPE_MISALIGNMENT:	return TSTR ("EXCEPTION_DATATYPE_MISALIGNMENT");
		case	EXCEPTION_ARRAY_BOUNDS_EXCEEDED:	return TSTR ("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		case	EXCEPTION_FLT_DENORMAL_OPERAND:		return TSTR ("EXCEPTION_FLT_DENORMAL_OPERAND");
		case	EXCEPTION_FLT_DIVIDE_BY_ZERO:		return TSTR ("EXCEPTION_FLT_DENORMAL_OPERAND");
		case	EXCEPTION_FLT_INEXACT_RESULT:		return TSTR ("EXCEPTION_FLT_INEXACT_RESULT");
		case	EXCEPTION_FLT_INVALID_OPERATION:	return TSTR ("EXCEPTION_FLT_INVALID_OPERATION");
		case	EXCEPTION_FLT_OVERFLOW:				return TSTR ("EXCEPTION_FLT_OVERFLOW");
		case	EXCEPTION_FLT_STACK_CHECK:			return TSTR ("EXCEPTION_FLT_STACK_CHECK");
		case	EXCEPTION_FLT_UNDERFLOW:			return TSTR ("EXCEPTION_FLT_UNDERFLOW");
		case	EXCEPTION_INT_DIVIDE_BY_ZERO:		return TSTR ("EXCEPTION_INT_DIVIDE_BY_ZERO");
		case	EXCEPTION_INT_OVERFLOW:				return TSTR ("EXCEPTION_INT_OVERFLOW");
		case	EXCEPTION_PRIV_INSTRUCTION:			return TSTR ("EXCEPTION_PRIV_INSTRUCTION");
		case	EXCEPTION_IN_PAGE_ERROR:			return TSTR ("EXCEPTION_IN_PAGE_ERROR");
		case	EXCEPTION_ILLEGAL_INSTRUCTION:		return TSTR ("EXCEPTION_ILLEGAL_INSTRUCTION");
		case	EXCEPTION_NONCONTINUABLE_EXCEPTION:	return TSTR ("EXCEPTION_NONCONTINUABLE_EXCEPTION");
		case	EXCEPTION_STACK_OVERFLOW:			return TSTR ("EXCEPTION_STACK_OVERFLOW");
		case	EXCEPTION_INVALID_DISPOSITION:		return TSTR ("EXCEPTION_INVALID_DISPOSITION");
		case	EXCEPTION_GUARD_PAGE:				return TSTR ("EXCEPTION_GUARD_PAGE");
		case	EXCEPTION_INVALID_HANDLE:			return TSTR ("EXCEPTION_INVALID_HANDLE");
		default: {
			TCHAR	buf[1024];
			(void)::_stprintf_s (buf, TSTR ("Windows Structured Exception Code - 0x%x"), u);
			return buf;	
		}
	}
}

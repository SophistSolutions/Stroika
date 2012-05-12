/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"ExceptionPropagate.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;




/*
 ********************************************************************************
 ************************* Execution::ExceptionPropagateHelper ******************
 ********************************************************************************
 */



//	class	ExceptionPropagateHelper
ExceptionPropagateHelper::ExceptionPropagateHelper ()
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

void	ExceptionPropagateHelper::DoRunWithCatchRePropagate (IRunnable* callback)
{
	// Subclassers add additional 'Execution' as data members, and override this to catch additioanl fields, and
	// AnyExceptionCaught/RethrowIfAnyCaught to rethrow each
	RequireNotNull (callback);
	try {
		callback->Run ();
	}
	CATCH_AND_CAPTURE_CATCH_BLOCK(*this);
}

bool	ExceptionPropagateHelper::AnyExceptionCaught () const
{
	return 
		fStringException.get () != nullptr
		or fFileFormatException.get () != nullptr
		or fFileBusyException.get () != nullptr
		or fSilentException.get () != nullptr
		or fRequiredComponentMissingException.get () != nullptr
#if		qPlatform_Windows
		or fHRESULTErrorException.get () != nullptr
		or fWin32ErrorException.get () != nullptr
#endif
		;
}

void	ExceptionPropagateHelper::RethrowIfAnyCaught () const
{
#if		qPlatform_Windows
	if (fHRESULTErrorException.get () != nullptr) {
		throw *fHRESULTErrorException.get ();
	}
	if (fWin32ErrorException.get () != nullptr) {
		throw *fWin32ErrorException.get ();
	}
#endif
	if (fStringException.get () != nullptr) {
		throw *fStringException.get ();
	}
	if (fFileFormatException.get () != nullptr) {
		throw *fFileFormatException.get ();
	}
	if (fFileBusyException.get () != nullptr) {
		throw *fFileBusyException.get ();
	}
	if (fSilentException.get () != nullptr) {
		throw *fSilentException.get ();
	}
	if (fRequiredComponentMissingException.get () != nullptr) {
		throw *fRequiredComponentMissingException.get ();
	}
}


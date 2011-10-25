/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"Debugger.h"
#include	"Trace.h"

#include	"Fatal.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Debug;


namespace	{
	void	_DefaultFatalErorrHandler_ (const TChar* msg)
		{
			DbgTrace (TSTR ("Fatal Error %s encountered"), msg);
			Debug::DropIntoDebuggerIfPresent ();
			abort ();
		}
	void (*sFatalErrorHandler_) (const TChar* msg)		=	nullptr;		// our handlers can never get called until  RegisterDefaultFatalErrorHandlers () is called

	void	_DefaultTerminateHandler_ ()
		{
			(sFatalErrorHandler_) (TSTR ("std::terminate () called"));
		}
	void	_DefaultUnexpectedHandler_ ()
		{
			(sFatalErrorHandler_) (TSTR ("std::unexpected () called"));
		}
}

void	Debug::RegisterDefaultFatalErrorHandlers (void (*fatalErrorHandler) (const TChar* msg))
{
	sFatalErrorHandler_ = (fatalErrorHandler == nullptr)? _DefaultFatalErorrHandler_ : fatalErrorHandler;
	set_terminate (_DefaultTerminateHandler_);
	set_unexpected (_DefaultUnexpectedHandler_);
}

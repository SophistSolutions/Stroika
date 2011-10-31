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

	void	TerminateHandler_ ()
		{
			(sFatalErrorHandler_) (TSTR ("std::terminate () called"));
		}
	void	UnexpectedHandler_ ()
		{
			(sFatalErrorHandler_) (TSTR ("std::unexpected () called"));
		}
	#if		qPlatform_Windows
	void	PurecallHandler_ ()
		{
			(sFatalErrorHandler_) (TSTR ("purecall_handler_ () called"));
		}
	#endif
}






void	Debug::RegisterDefaultFatalErrorHandlers (void (*fatalErrorHandler) (const TChar* msg))
{
	sFatalErrorHandler_ = (fatalErrorHandler == nullptr)? _DefaultFatalErorrHandler_ : fatalErrorHandler;
	set_terminate (TerminateHandler_);
	set_unexpected (UnexpectedHandler_);
	#if		qPlatform_Windows
		// Not C++ standard - just msvc error call
		(void)_set_purecall_handler (PurecallHandler_);
	#endif
}

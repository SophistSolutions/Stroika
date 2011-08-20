/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include <cassert>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"Assertions.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Debug;


#if		qDebug
	namespace {
		void	(*sLedAssertFailedCallback) (const char* fileName, int lineNum)		=	NULL;
	}

	void	(*Stroika::Foundation::Debug::GetAssertionHandler ()) (const char* fileName, int lineNum)
	{
		return sLedAssertFailedCallback;
	}

	void	Stroika::Foundation::Debug::SetAssertionHandler (void (*assertionHandler) (const char* fileName, int lineNum))
	{
		sLedAssertFailedCallback = assertionHandler;
	}

	void	Stroika::Foundation::Debug::_Debug_Trap_ (const char* fileName, int lineNum)
	{
		if (sLedAssertFailedCallback == NULL) {
			#if		qPlatform_Windows
				DebugBreak ();
			#else
				assert (false);
			#endif
		}
		else {
			(sLedAssertFailedCallback) (fileName, lineNum);
		}
	}
#endif



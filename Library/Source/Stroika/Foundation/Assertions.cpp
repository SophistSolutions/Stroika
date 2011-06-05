/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"StroikaPreComp.h"

#include	<Windows.h>

#include	"Assertions.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;


#if		defined (_DEBUG)
namespace {
	void	(*sLedAssertFailedCallback) (const char* fileName, int lineNum)		=	NULL;
}

void	(*GetAssertionHandler ()) (const char* fileName, int lineNum)
{
	return sLedAssertFailedCallback;
}

void	SetAssertionHandler (void (*assertionHandler) (const char* fileName, int lineNum))
{
	sLedAssertFailedCallback = assertionHandler;
}

void	_Debug_Trap_ (const char* fileName, int lineNum)
{
	if (sLedAssertFailedCallback == NULL) {
		DebugBreak ();
	}
	else {
		(sLedAssertFailedCallback) (fileName, lineNum);
	}
}
#endif



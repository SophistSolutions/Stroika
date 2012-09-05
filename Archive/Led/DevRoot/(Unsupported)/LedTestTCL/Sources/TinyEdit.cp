/******************************************************************************
	TinyEdit.c
	
	Main program for a tiny editor.
	
	Copyright © 1989 Symantec Corporation. All rights reserved.

 ******************************************************************************/

#include	"CEditApp.h"

#include	"stdio.h"

#if		__profile__
	#define	qProfile		1
#endif

#ifndef	qProfile
	#define	qProfile		0
#endif

#if		qProfile && defined (__SC__)
	#include	"profile.h"
#endif

#if		qProfile && defined (__MWERKS__)
	#include	"profiler.h"
#endif

void main()

{
#if		qProfile && defined (__SC__)
	const	size_t	kMaxFunctionsProfiled	=	2048;
	const	size_t	kMaxStackFramesProfiled	=	256;
	::InitProfile (kMaxFunctionsProfiled, kMaxStackFramesProfiled);
	_trace = 1;
	(void)freopen ("Prof.out", "w", stdout);
#endif
#if		qProfile && defined (__MWERKS__)
	OSErr	proErr	=	ProfilerInit (collectDetailed, bestTimeBase, 10000, 1000);
#endif
	CEditApp*	editApp = new CEditApp;
	editApp->IEditApp();
	editApp->Run();
	editApp->Exit();
#if		qProfile && defined (__MWERKS__)
	ProfilerDump ("\pLedProfile.prof");
	ProfilerTerm ();
#endif
}


#if		defined (__MWERKS__) && !defined (NDEBUG)
short MY_TCL_assertion_failed (char *condition, char *fileName, int lineNum)
{
	Debugger();
	return 0;
}
//_C_LIB_DECL
void _Assert(char * aaa)
{
	// cuz default version simply silently exits!!!
	(void)MY_TCL_assertion_failed (aaa, "CORE?", 1);
}
//_END_C_LIB_DECL
#endif

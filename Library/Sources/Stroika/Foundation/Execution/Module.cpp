/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<windows.h>
#endif

#include	"../IO/FileUtils.h"

#include	"Module.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;





/*
 ********************************************************************************
 ***************************** Execution::GetEXEDir *****************************
 ********************************************************************************
 */
TString	Execution::GetEXEDir ()
{
	return IO::GetFileDirectory (GetEXEPath ());
}





/*
 ********************************************************************************
 **************************** Execution::GetEXEPath *****************************
 ********************************************************************************
 */
TString	Execution::GetEXEPath ()
{
#if		qPlatform_Windows
	Characters::TChar	buf[MAX_PATH];
	memset (buf, 0, sizeof (buf));
	Verify (::GetModuleFileName (nullptr, buf, NEltsOf (buf)));
	return buf;
#else
	AssertNotImplemented ();
	return TString ();
#endif
}


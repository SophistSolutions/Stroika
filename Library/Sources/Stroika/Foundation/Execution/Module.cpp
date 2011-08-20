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
	Characters::TChar	buf[MAX_PATH];
	memset (buf, 0, sizeof (buf));
#if		qPlatform_Windows
	Verify (::GetModuleFileName (NULL, buf, NEltsOf (buf)));
#else
	AssertNotImplemented ();
#endif
	return buf;
}


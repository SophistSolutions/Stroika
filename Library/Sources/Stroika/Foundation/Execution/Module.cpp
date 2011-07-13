/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<windows.h>
#include	<tchar.h>

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
	TCHAR	buf[MAX_PATH];
	memset (buf, 0, sizeof (buf));
	Verify (::GetModuleFileName (NULL, buf, NEltsOf (buf)));
	return buf;
}


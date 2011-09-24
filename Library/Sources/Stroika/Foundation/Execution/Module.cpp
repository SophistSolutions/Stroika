/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<windows.h>
#endif

#include	"../Execution/Exceptions.h"
#include	"../Memory/SmallStackBuffer.h"
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
	// See also http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
	//		Mac OS X: _NSGetExecutablePath() (man 3 dyld)
	//		Linux: readlink /proc/self/exe
	//		Solaris: getexecname()
	//		FreeBSD: sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
	//		BSD with procfs: readlink /proc/curproc/file
	//		Windows: GetModuleFileName() with hModule = NULL
	//
#if		qPlatform_Windows
	Characters::TChar	buf[MAX_PATH];
	memset (buf, 0, sizeof (buf));
	Verify (::GetModuleFileName (nullptr, buf, NEltsOf (buf)));
	return buf;
	Memory::SmallStackBuffer<Characters::TChar>	bufx (1000);
#elif	qPlatform_POSIX && qSupport_Proc_Filesystem
	// readlink () isn't clear about finding the right size. THe only way to tell it wasn't enuf (maybe) is if all the
	// bytes passed in are used. That COULD mean it all fit, or there was more. If we get that - double buf size and try again
	Memory::SmallStackBuffer<Characters::TChar> buf (1000);
	ssize_t	n;
	while ( (n = readlink ("/proc/self/exe", buf, buf.GetSize () - 1)) == buf.GetSize ()) {
		buf.GrowToSize (buf.GetSize () * 2);
	}
	if (n < 0) {
		Execution::ThrowIfError_errno_t ();
		Assert (false);	// errno SB set!!!
	}
	(buf.begin () + n) = '\0';
	return buf.begin ();
#else
	AssertNotImplemented ();
	return TString ();
#endif
}


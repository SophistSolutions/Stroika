/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<shlobj.h>
	#include	<windows.h>
#elif	qPlatoform_POSIX
	#include	<cstdlib>
#endif

#include	"../../Execution/Exceptions.h"

#include	"FileUtils.h"
#include	"PathName.h"

#include	"WellKnownLocations.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::FileSystem;

using	Characters::TChar;







/*
 ********************************************************************************
 ************ FileSystem::WellKnownLocations::GetMyDocuments ********************
 ********************************************************************************
 */
TString	FileSystem::WellKnownLocations::GetMyDocuments (bool createIfNotPresent)
{
#if		qPlatform_Windows
	TChar	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	Execution::Platform::Windows::ThrowIfFalseGetLastError (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
	TString	result = fileBuf;
	// Assure non-empty result
	if (result.empty ()) {
		result = TSTR("c:");	// shouldn't happen
	}
	// assure ends in '\'
	if (result[result.size ()-1] != '\\') {
		result += TSTR("\\");
	}
	Ensure (result[result.size ()-1] == '\\');
	Ensure (not createIfNotPresent or DirectoryExists (result));
	return result;
#elif	qPlatform_POSIX
	const char *pPath = getenv ("HOME");
	if (pPath == nullptr) {
		return TString ();
	}
	return pPath;
#else
	AssertNotImplemented ();
	return TString ();
#endif
}






/*
 ********************************************************************************
 ******** FileSystem::WellKnownLocations::GetApplicationData ********************
 ********************************************************************************
 */
TString	FileSystem::WellKnownLocations::GetApplicationData (bool createIfNotPresent)
{
#if		qPlatform_Windows
	TChar	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, createIfNotPresent));
	TString	result = fileBuf;
	// Assure non-empty result
	if (result.empty ()) {
		result = TSTR ("c:");	// shouldn't happen
	}
	// assure ends in '\'
	if (result[result.size ()-1] != '\\') {
		result += TSTR ("\\");
	}
	Ensure (result[result.size ()-1] == '\\');
	Ensure (not createIfNotPresent or DirectoryExists (result));
	return result;
#elif	qPlatform_POSIX
	return TSTR ("/var/lib/");
#else
	AssertNotImplemented ();
	return TString ();
#endif
}






#if		qPlatform_Windows
/*
 ********************************************************************************
 ************** FileSystem::WellKnownLocations::GetWinSxS ***********************
 ********************************************************************************
 */
TString	FileSystem::WellKnownLocations::GetWinSxS ()
{
	TChar	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_WINDOWS, false));
	TString	result = fileBuf;
	// Assure non-empty result
	if (result.empty ()) {
		return result;
	}
	result = AssureDirectoryPathSlashTerminated (result) + TSTR ("WinSxS");
	result = AssureDirectoryPathSlashTerminated (result);
	if (not DirectoryExists (result)) {
		result.clear ();
	}
	Ensure (result.empty () or DirectoryExists (result));
	return result;
}
#endif






/*
 ********************************************************************************
 ************* FileSystem::WellKnownLocations::GetTemporary *********************
 ********************************************************************************
 */
TString	FileSystem::WellKnownLocations::GetTemporary ()
{
	TString	tempPath;
#if		qPlatform_Windows
	TChar	buf[1024];
	if (::GetTempPath (NEltsOf (buf), buf) == 0) {
		tempPath = TSTR ("c:\\Temp\\");
	}
	else {
		tempPath = buf;
	}
#elif	qPlatform_POSIX
	return TSTR ("/tmp/");
#else
	AssertNotImplemented ();
#endif
	return AssureDirectoryPathSlashTerminated (tempPath);
}







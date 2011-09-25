/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<aclapi.h>
	#include	<io.h>
	#include	<shlobj.h>
	#include	<windows.h>
#endif

#include	"../../Execution/Exceptions.h"

#include	"FileUtils.h"

#include	"WellKnownLocations.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::FileSystem;

using	Characters::TChar;





/*
 ********************************************************************************
 ******************** FileSystem::GetSpecialDir_MyDocuments *********************
 ********************************************************************************
 */
TString	FileSystem::GetSpecialDir_MyDocuments (bool createIfNotPresent)
{
#if		qPlatform_Windows
	TChar	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	ThrowIfFalseGetLastError (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
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
#else
	AssertNotImplemented ();
	return TString ();
#endif
}






/*
 ********************************************************************************
 ******************** FileSystem::GetSpecialDir_AppData *************************
 ********************************************************************************
 */
TString	FileSystem::GetSpecialDir_AppData (bool createIfNotPresent)
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
#else
	AssertNotImplemented ();
	return TString ();
#endif
}






#if		qPlatform_Windows
/*
 ********************************************************************************
 ********************* FileSystem::GetSpecialDir_WinSxS *************************
 ********************************************************************************
 */
TString	FileSystem::GetSpecialDir_WinSxS ()
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
 ********************* FileSystem::GetSpecialDir_GetTempDir *********************
 ********************************************************************************
 */
TString	FileSystem::GetSpecialDir_GetTempDir ()
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
#else
	AssertNotImplemented ();
#endif
	return AssureDirectoryPathSlashTerminated (tempPath);
}







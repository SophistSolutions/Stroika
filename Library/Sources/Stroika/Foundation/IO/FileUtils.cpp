/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<sys/stat.h>
#include	<ctime>
#include	<limits>
#include	<fstream>
#include	<fcntl.h>
#include	<io.h>
#include	<cstdio>

#if		qPlatform_Windows
	#include	<aclapi.h>
	#include	<shlobj.h>
	#include	<windows.h>
#endif

#include	"../Execution/Exceptions.h"
#include	"../Containers/Common.h"
#include	"../Containers/SetUtils.h"
#include	"../Debug/Trace.h"
#include	"../IO/FileAccessException.h"
#include	"../Memory/SmallStackBuffer.h"

#include	"FileUtils.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::Memory;













/*
 ********************************************************************************
 *************************** IO::Private::UsingModuleHelper *********************
 ********************************************************************************
 */
IO::Private::UsingModuleHelper::UsingModuleHelper ()
	:fAppTempFileManager ()
{
}

IO::Private::UsingModuleHelper::~UsingModuleHelper ()
{
}







/*
 ********************************************************************************
 **************************** IO::GetSpecialDir_MyDocuments *********************
 ********************************************************************************
 */
TString	IO::GetSpecialDir_MyDocuments (bool createIfNotPresent)
{
	TChar	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	ThrowIfFalseGetLastError (::SHGetSpecialFolderPath (NULL, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
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
}






/*
 ********************************************************************************
 **************************** IO::GetSpecialDir_AppData *************************
 ********************************************************************************
 */
TString	IO::GetSpecialDir_AppData (bool createIfNotPresent)
{
	TChar	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	Verify (::SHGetSpecialFolderPath (NULL, fileBuf, CSIDL_COMMON_APPDATA, createIfNotPresent));
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
}






/*
 ********************************************************************************
 ***************************** IO::GetSpecialDir_WinSxS *************************
 ********************************************************************************
 */
TString	IO::GetSpecialDir_WinSxS ()
{
	TChar	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	Verify (::SHGetSpecialFolderPath (NULL, fileBuf, CSIDL_WINDOWS, false));
	TString	result = fileBuf;
	// Assure non-empty result
	if (result.empty ()) {
		return result;
	}
	result = AssureDirectoryPathSlashTerminated (result) + _T ("WinSxS");
	result = AssureDirectoryPathSlashTerminated (result);
	if (not DirectoryExists (result)) {
		result.clear ();
	}
	Ensure (result.empty () or DirectoryExists (result));
	return result;
}






/*
 ********************************************************************************
 ***************************** IO::GetSpecialDir_GetTempDir *********************
 ********************************************************************************
 */
TString	IO::GetSpecialDir_GetTempDir ()
{
	TString	tempPath;
	TCHAR	buf[1024];
	if (::GetTempPath (NEltsOf (buf), buf) == 0) {
		tempPath = _T ("c:\\Temp\\");
	}
	else {
		tempPath = buf;
	}
	return AssureDirectoryPathSlashTerminated (tempPath);
}






/*
 ********************************************************************************
 ************************* IO::CheckFileAccess ***************************
 ********************************************************************************
 */
void	IO::CheckFileAccess (const TString& fileFullPath, bool checkCanRead, bool checkCanWrite)
{
	// quick hack - not fully implemented - but since advsiory only - not too important...

	if (not FileExists (fileFullPath)) {
// FOR NOW - MIMIC OLD CODE - BUT FIX TO CHECK READ AND WRITE (AND BOTH) ACCESS DEPENDING ON ARGS) -- LGP 2009-08-15
		Execution::DoThrow (FileAccessException (fileFullPath, IO::eRead_FAM));
	}
}








/*
 ********************************************************************************
 ********************** IO::AssureDirectoryPathSlashTerminated ******************
 ********************************************************************************
 */
TString	IO::AssureDirectoryPathSlashTerminated (const TString& dirPath)
{
	if (dirPath.empty ()) {
		Assert (false);	// not sure if this is an error or not. Not sure how code used.
						// put assert in there to find out... Probably should THROW!
						//		-- LGP 2009-05-12
		return _T ("\\");
	}
	else {
		TChar	lastChar = dirPath[dirPath.length ()-1];
		return (lastChar == '\\')?
				dirPath: 
				(dirPath + _T ("\\"))
			;
	}
}







/*
 ********************************************************************************
 ******************************* IO::SafeFilenameChars **************************
 ********************************************************************************
 */
TString	IO::SafeFilenameChars (const TString& s)
{
	wstring	tmp	=	TString2Wide (s);	// analyze as wide-char string so we don't mis-identify
										// characters (by looking at lead bytes etc)
Again:
	for (wstring::iterator i = tmp.begin (); i != tmp.end (); ++i) {
		switch (*i) {
			case	'\\':	*i = '_'; break;
			case	'/':	*i = '_'; break;
			case	':':	*i = ' '; break;
			case	'.':	tmp.erase (i); goto Again;
		}
	}
	return Wide2TString (tmp);
}







/*
 ********************************************************************************
 ********************************* IO::ResolveShortcut **************************
 ********************************************************************************
 */
TString	IO::ResolveShortcut (const TString& path2FileOrShortcut)
{
	// NB: this requires COM, and for now - I don't want the support module depending on the COM module,
	// so just allow this to fail if COM isn't initialized.
	//		-- LGP 2007-09-23
	//
	{
		SHFILEINFO   info;
		if (::SHGetFileInfo (path2FileOrShortcut.c_str (), 0, &info, sizeof (info), SHGFI_ATTRIBUTES) == 0) {
			return path2FileOrShortcut;
		}
		// not a shortcut?
		if (!(info.dwAttributes & SFGAO_LINK)) {
			return path2FileOrShortcut;
		}
	}

    // obtain the IShellLink interface
    IShellLink*		psl	=	NULL;
    IPersistFile*	ppf	=	NULL;
	try {
		if (FAILED (::CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl))) {
			return path2FileOrShortcut;
		}
		if (SUCCEEDED (psl->QueryInterface (IID_IPersistFile, (LPVOID*)&ppf))) {
			if (SUCCEEDED (ppf->Load (TString2Wide (path2FileOrShortcut).c_str (), STGM_READ))) {
				// Resolve the link, this may post UI to find the link
				if (SUCCEEDED (psl->Resolve(0, SLR_NO_UI))) {
					TChar	path[MAX_PATH+1];
					memset (path, 0, sizeof (path));
					if (SUCCEEDED (psl->GetPath (path, NEltsOf (path), NULL, 0))) {
						ppf->Release ();
						ppf = NULL;
						psl->Release ();
						psl = NULL;
						return path;
					}
				}
			}
		}
	}
	catch (...) {
		if (ppf != NULL) {
			ppf->Release ();
		}
		if (psl != NULL) {
			psl->Release ();
		}
		Execution::DoReThrow ();
	}
	if (ppf != NULL) {
		ppf->Release ();
	}
	if (psl != NULL) {
		psl->Release ();
	}
	return path2FileOrShortcut;
}










/*
 ********************************************************************************
 ************************* IO::FileSizeToDisplayString **************************
 ********************************************************************************
 */
wstring	IO::FileSizeToDisplayString (FileOffset_t bytes)
{
	if (bytes < 1000) {
		return Format (L"%d bytes", static_cast<int> (bytes));
	}
	else if (bytes < 1000 * 1024) {
		return Format (L"%.1f K", static_cast<double> (bytes)/1024.0f);
	}
	else {
		return Format (L"%.1f MB", static_cast<double> (bytes)/(1024*1024.0f));
	}
}





/*
 ********************************************************************************
 *********************************** IO::GetFileSize ****************************
 ********************************************************************************
 */
FileOffset_t	IO::GetFileSize (const TString& fileName)
{
	WIN32_FILE_ATTRIBUTE_DATA	fileAttrData;
	(void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
	ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
	return fileAttrData.nFileSizeLow + (static_cast<FileOffset_t> (fileAttrData.nFileSizeHigh) << 32);
}






/*
 ********************************************************************************
 ***************************** IO::GetFileLastModificationDate ******************
 ********************************************************************************
 */
DateTime		IO::GetFileLastModificationDate (const TString& fileName)
{
	WIN32_FILE_ATTRIBUTE_DATA	fileAttrData;
	(void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
	ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
	return DateTime (fileAttrData.ftLastWriteTime);
}






/*
 ********************************************************************************
 ******************************* IO::GetFileLastAccessDate **********************
 ********************************************************************************
 */
DateTime			IO::GetFileLastAccessDate (const TString& fileName)
{
	WIN32_FILE_ATTRIBUTE_DATA	fileAttrData;
	(void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
	ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
	return DateTime (fileAttrData.ftLastAccessTime);
}




/*
 ********************************************************************************
 ***************************** IO::SetFileAccessWideOpened **********************
 ********************************************************************************
 */
/*
 * Sets permissions for users on a given folder to full control
 *
 *		Add 'Everyone' to have FULL ACCESS to the given argument file
 *
 */
void	IO::SetFileAccessWideOpened (const TString& filePathName)
{
	static	PACL pACL = NULL;	// Don't bother with ::LocalFree (pACL); - since we cache keeping this guy around for speed
	if (pACL == NULL) {
		PSID pSIDEveryone = NULL;

		{
			// Specify the DACL to use.
			// Create a SID for the Everyone group.
			SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
			if (!::AllocateAndInitializeSid (&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSIDEveryone))  { 
				return;		// if this fails - perhaps old OS with no security - just fail silently...
			}
		}

		EXPLICIT_ACCESS ea[1];
		memset (&ea, 0, sizeof (ea));

		// Set FULL access for Everyone.
		ea[0].grfAccessPermissions = GENERIC_ALL;
		ea[0].grfAccessMode = SET_ACCESS;
		ea[0].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
		ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
		ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea[0].Trustee.ptstrName = (LPTSTR) pSIDEveryone;

		if (ERROR_SUCCESS != ::SetEntriesInAcl (NEltsOf (ea), ea, NULL, &pACL)) {
			::FreeSid (pSIDEveryone);
			return;	// silently ignore errors - probably just old OS etc....
		}
		::FreeSid (pSIDEveryone);
	}

	// Try to modify the object's DACL.
	DWORD dwRes  = SetNamedSecurityInfo(
			const_cast<TChar*> (filePathName.c_str ()),          // name of the object
			SE_FILE_OBJECT,              // type of object
			DACL_SECURITY_INFORMATION,   // change only the object's DACL
			NULL, NULL,                  // don't change owner or group
			pACL,                        // DACL specified
			NULL
		);                       // don't change SACL
	// ignore error from this routine for now  - probably means either we don't have permissions or OS too old to support...
}








/*
 ********************************************************************************
 ******************************** IO::CreateDirectory ***************************
 ********************************************************************************
 */
void	IO::CreateDirectory (const TString& directoryPath, bool createParentComponentsIfNeeded)
{
	if (createParentComponentsIfNeeded) {
		// walk path and break into parts, and from top down - try to create parent directory structure.
		// Ignore any failures - and just let the report of failure (if any must result) come from original basic
		// CreateDirectory call.
		size_t	index	=	directoryPath.find (_T ("\\"));
		while (index != -1 and index + 1 < directoryPath.length ()) {
			TString	parentPath = directoryPath.substr (0, index);
			IgnoreExceptionsForCall (CreateDirectory (parentPath, false));
			index = directoryPath.find ('\\', index+1);
		}
	}

	if (not ::CreateDirectory (directoryPath.c_str (), NULL)) {
		DWORD error = ::GetLastError ();
		if (error != ERROR_ALREADY_EXISTS) {
			Execution::DoThrow (Execution::Platform::Windows::Exception (error));
		}
	}
}







/*
 ********************************************************************************
 ************************* IO::CreateDirectoryForFile ***************************
 ********************************************************************************
 */
void	IO::CreateDirectoryForFile (const TString& filePath)
{
	if (filePath.empty ()) {
		Execution::DoThrow (Execution::Platform::Windows::Exception (ERROR_FILE_NOT_FOUND));
	}
	if (FileExists (filePath)) {
		// were done
		return;
	}
	CreateDirectory (GetFileDirectory (filePath), true);
}






/*
 ********************************************************************************
 ********************************** IO::GetVolumeName ***************************
 ********************************************************************************
 */
TString	IO::GetVolumeName (const TString& driveLetterAbsPath)
{
	// SEM_FAILCRITICALERRORS needed to avoid dialog in call to GetVolumeInformation
	AdjustSysErrorMode	errorModeAdjuster (AdjustSysErrorMode::GetErrorMode () | SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	DWORD	ignored	=	0;
	TChar	volNameBuf[1024];
	memset (volNameBuf, 0, sizeof (volNameBuf));
	TChar	igBuf[1024];
	memset (igBuf, 0, sizeof (igBuf));
	BOOL	result	=	::GetVolumeInformation (
							AssureDirectoryPathSlashTerminated (driveLetterAbsPath).c_str (),
							volNameBuf,
							NEltsOf (volNameBuf),
							NULL,
							&ignored,
							&ignored,
							igBuf,
							NEltsOf (igBuf)
							);
	if (result) {
		return volNameBuf;
	}
	return TString ();
}





/*
 ********************************************************************************
 ***************************** IO::AssureLongFileName ***************************
 ********************************************************************************
 */
TString	IO::AssureLongFileName (const TString& fileName)
{
	DWORD	r	=	::GetLongPathName (fileName.c_str (), NULL, 0);
	if (r != 0) {
		SmallStackBuffer<TChar>	buf (r);
		r = ::GetLongPathName (fileName.c_str (), buf, r);
		if (r != 0) {
			return TString (buf);
		}
	}
	return fileName;
}





/*
 ********************************************************************************
 ********************************** IO::GetFileSuffix ***************************
 ********************************************************************************
 */
TString	IO::GetFileSuffix (const TString& fileName)
{
	TString	useFName	=	fileName;

	{
		TChar	fNameBuf[4 * MAX_PATH ];
		fNameBuf[0] = '\0';
		DWORD	r	=	::GetLongPathName (fileName.c_str (), fNameBuf, NEltsOf (fNameBuf)-1);
		if (r != 0) {
			useFName = fNameBuf;
		}
	}
	TChar	fname[_MAX_FNAME];
	TChar	drive[_MAX_DRIVE];
	TChar	dir[_MAX_DIR];
	TChar	ext[_MAX_EXT];
	memset (drive, 0, sizeof (drive));
	memset (dir, 0, sizeof (dir));
	memset (fname, 0, sizeof (fname));
	memset (ext, 0, sizeof (ext));
	::_tsplitpath_s (useFName.c_str (), drive, dir, fname, ext);
	// returns leading '.' in name...
	return ext;
}




/*
 ********************************************************************************
 ******************************** IO::GetFileBaseName ***************************
 ********************************************************************************
 */
TString	IO::GetFileBaseName (const TString& pathName)
{
	TString	useFName	=	pathName;

	{
		TChar	fNameBuf[4 * MAX_PATH ];
		DWORD	r	=	GetLongPathName (pathName.c_str (), fNameBuf, NEltsOf (fNameBuf)-1);
		if (r != 0) {
			useFName = fNameBuf;
		}
	}
	TChar	fname[_MAX_FNAME];
	TChar	drive[_MAX_DRIVE];
	TChar	dir[_MAX_DIR];
	TChar	ext[_MAX_EXT];
	memset (drive, 0, sizeof (drive));
	memset (dir, 0, sizeof (dir));
	memset (fname, 0, sizeof (fname));
	memset (ext, 0, sizeof (ext));
	::_tsplitpath_s (useFName.c_str (), drive, dir, fname, ext);
	return fname;
}





/*
 ********************************************************************************
 ******************************** IO::StripFileSuffix ***************************
 ********************************************************************************
 */
TString	IO::StripFileSuffix (const TString& pathName)
{
	TString	useFName	=	pathName;
	TString	fileSuffix	=	GetFileSuffix (pathName);
	if (useFName.length () > fileSuffix.length ()) {
		return useFName.substr (0, useFName.length () - fileSuffix.length ());
	}
	else {
		return useFName;
	}
}






/*
 ********************************************************************************
 ******************************** IO::GetFileDirectory **************************
 ********************************************************************************
 */
TString	IO::GetFileDirectory (const TString& pathName)
{
	// could use splitpath, but this maybe better, since works with \\UNCNAMES
	TString	tmp		=	pathName;
	size_t	idx		=	tmp.rfind ('\\');
	if (idx != TString::npos) {
		tmp.erase (idx + 1);
	}
	return tmp;
}







/*
 ********************************************************************************
 ************************************* IO::FileExists ***************************
 ********************************************************************************
 */
bool	IO::FileExists (const TChar* filePath)
{
	RequireNotNil (filePath);
	DWORD attribs = ::GetFileAttributes (filePath);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}	
	return not (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

bool	IO::FileExists (const TString& filePath)
{
	return FileExists (filePath.c_str ());
}







/*
 ********************************************************************************
 ******************************** IO::DirectoryExists ***************************
 ********************************************************************************
 */
bool	IO::DirectoryExists (const TChar* filePath)
{
	RequireNotNil (filePath);
	DWORD attribs = ::GetFileAttributes (filePath);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}	
	return !! (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

bool	IO::DirectoryExists (const TString& filePath)
{
	return DirectoryExists (filePath.c_str ());
}







/*
 ********************************************************************************
 *************************************** IO::CopyFile ***************************
 ********************************************************************************
 */
void	IO::CopyFile (const TString& srcFile, const TString& destPath)
{
// see if can be/should be rewritten to use Win32 API of same name!!!
//
// If I DONT do that remapping to Win32 API, then redo this at least to copy / rename through tmpfile
	if (not FileExists (srcFile)) {
		Execution::DoThrow (FileAccessException (srcFile, IO::eRead_FAM));
	}
	CreateDirectoryForFile (destPath);
	ThrowIfFalseGetLastError (::CopyFile (destPath.c_str (), srcFile.c_str (), false));
}









/*
 ********************************************************************************
 ************************************* IO::FindFiles ****************************
 ********************************************************************************
 */
vector<TString>	IO::FindFiles (const TString& path, const TString& fileNameToMatch)
{
	vector<TString>	result;
	if (path.empty ()) {
		return result;
	}
	TString	usePath			=	AssureDirectoryPathSlashTerminated (path);
	TString	matchFullPath	=	usePath + (fileNameToMatch.empty ()? TSTR ("*"): fileNameToMatch);
	WIN32_FIND_DATA fd;
	memset (&fd, 0, sizeof (fd));
	HANDLE hFind = ::FindFirstFile (matchFullPath.c_str (), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		try {
			do {
				TString fileName = fd.cFileName;
				if (not (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					result.push_back (usePath + fileName);
				}
			} while (::FindNextFile (hFind, &fd));
		}
		catch (...) {
			::FindClose (hFind);
			Execution::DoReThrow ();
		}
		::FindClose (hFind);
	}
	return result;
}








/*
 ********************************************************************************
 ********************************* IO::FindFilesOneDirUnder *********************
 ********************************************************************************
 */
vector<TString>	IO::FindFilesOneDirUnder (const TString& path, const TString& fileNameToMatch)
{
	if (path.empty ()) {
		return vector<TString> ();
	}

	set<TString>	resultSet;
	TString	usePath	=	AssureDirectoryPathSlashTerminated (path);
	WIN32_FIND_DATA fd;
	memset (&fd, 0, sizeof (fd));
	HANDLE hFind = ::FindFirstFile ((usePath + _T ("*")).c_str (), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			TString fileName = (LPCTSTR) &fd.cFileName;
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				TString fileName = (LPCTSTR) &fd.cFileName;
				const	TString	kDOT	=	_T (".");
				const	TString	kDOTDOT	=	_T ("..");
				if ((fileName != kDOT) and (fileName != kDOTDOT)) {
					vector<TString>	subdirFiles	=	FindFiles (usePath + fileName, fileNameToMatch);
					resultSet = resultSet + set<TString> (subdirFiles.begin (), subdirFiles.end ());
				}
			}
		} while (::FindNextFile (hFind, &fd));
		::FindClose (hFind);
	}
	return vector<TString> (resultSet.begin (), resultSet.end ());
}








/*
 ********************************************************************************
 ************************* IO::DeleteAllFilesInDirectory ************************
 ********************************************************************************
 */
void	IO::DeleteAllFilesInDirectory (const TString& path, bool ignoreErrors)
{
	if (path.empty ()) {
		Execution::DoThrow (Execution::Platform::Windows::Exception (ERROR_INVALID_NAME));
	}
	TString	dir2Use	=	AssureDirectoryPathSlashTerminated (path);

	WIN32_FIND_DATA	fd;
	(void)::memset (&fd, 0, sizeof (fd));
	HANDLE			hFind = ::FindFirstFile ((LPCTSTR) (dir2Use + _T ("*")).c_str (), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		try {
			do {
				TString	fileName = fd.cFileName;
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if ((fileName != _T (".")) and (fileName != _T (".."))) {
						DeleteAllFilesInDirectory (dir2Use + fileName + _T ("\\"), ignoreErrors);
						try {
							ThrowIfFalseGetLastError (::RemoveDirectory ((dir2Use + fileName).c_str ()));
						}
						catch (...) {
							DbgTrace (_T ("Exception %s calling RemoveDirectory on file '%s'"), ignoreErrors? _T ("(ignored)"): _T (""), (dir2Use + fileName).c_str ());
							if (!ignoreErrors) {
								Execution::DoReThrow ();
							}
						}
					}
				}
				else {
					try {
						ThrowIfFalseGetLastError (::DeleteFile ((dir2Use + fileName).c_str ()));
					}
					catch (...) {
						DbgTrace (_T ("Exception %s calling ::DeleteFile on file '%s'"), ignoreErrors? _T ("(ignored)"): _T (""), (dir2Use + fileName).c_str ());
						if (!ignoreErrors) {
							Execution::DoReThrow ();
						}
					}
				}
			} while (::FindNextFile (hFind, &fd));
		}
		catch (...) {
			::FindClose (hFind);
			Execution::DoReThrow ();
		}
		::FindClose (hFind);
	}
}





/*
 ********************************************************************************
 ************************************ IO::WriteString ***************************
 ********************************************************************************
 */
void	IO::WriteString (ostream& out, const wstring& s)
{
	string	s1 = WideStringToNarrow (s, kCodePage_UTF8);
	out << s1.size ();
	out << ' ';
	out << s1 << '\t';
}






/*
 ********************************************************************************
 ************************************* IO::ReadString ***************************
 ********************************************************************************
 */
wstring	IO::ReadString (istream& in)
{
	int	strlen;
	in >> strlen;
	in.get ();	// throw away character between size and string
	
	string	s;
	s.reserve (strlen);
	for (int i = 0; i < strlen; ++i) {
		s += in.get ();
	}
	return NarrowStringToWide (s, kCodePage_UTF8);
}








/*
 ********************************************************************************
 *********************************** IO::ReadBytes ******************************
 ********************************************************************************
 */
vector<Byte>	IO::ReadBytes (istream& in)
{
	streamoff	start	=	in.tellg ();
	in.seekg (0, ios_base::end);
	streamoff	end		=	in.tellg ();
	Assert (start <= end);
	if (streamoff (end - start) > streamoff (numeric_limits<size_t>::max ())) {
		Execution::DoThrow (StringException (L"stream too large"));
	}
	size_t	len	=	static_cast<size_t> (end-start);
	SmallStackBuffer<Byte>	buf (len);
	in.seekg (start, ios_base::beg);
	in.read (reinterpret_cast<char*> (buf.begin ()), len);
	size_t xxx = static_cast<size_t> (in.gcount ());
	Assert (xxx <= len);
	return vector<Byte> (static_cast<const Byte*> (buf), static_cast<const Byte*> (buf) + xxx);
}




/*
 ********************************************************************************
 ************************************* IO::WriteBytes ***************************
 ********************************************************************************
 */
void	IO::WriteBytes (ostream& out, const vector<Byte>& s)
{
	out.write (reinterpret_cast<const char*> (Containers::Start (s)), s.size ());
}






/*
 ********************************************************************************
 ***************************** IO::DirectoryChangeWatcher ***********************
 ********************************************************************************
 */
IO::DirectoryChangeWatcher::DirectoryChangeWatcher (const TString& directoryName, bool watchSubTree, DWORD notifyFilter):
	fDirectory (directoryName),
	fWatchSubTree (watchSubTree),
	fThread (),
	fDoneEvent (::CreateEvent (NULL, false, false, NULL)),
	fWatchEvent (::FindFirstChangeNotification (fDirectory.c_str (), fWatchSubTree, notifyFilter)),
	fQuitting (false)
{
	fThread = Execution::SimpleThread (&ThreadProc, this);
	fThread.SetThreadName (L"DirectoryChangeWatcher");
	fThread.Start ();
}

IO::DirectoryChangeWatcher::~DirectoryChangeWatcher ()
{
	fQuitting = true;
	if (fDoneEvent != INVALID_HANDLE_VALUE) {
		Verify (::SetEvent (fDoneEvent));
	}
	IgnoreExceptionsForCall (fThread.StopAndWaitForDone ());
	if (fDoneEvent != INVALID_HANDLE_VALUE) {
		Verify (::CloseHandle (fDoneEvent));
	}
	if (fWatchEvent != INVALID_HANDLE_VALUE) {
		Verify (::FindCloseChangeNotification (fWatchEvent));
	}
}

void	IO::DirectoryChangeWatcher::ValueChanged ()
{
}

void	IO::DirectoryChangeWatcher::ThreadProc (void* lpParameter)
{
	DirectoryChangeWatcher*		_THS_	=	reinterpret_cast<DirectoryChangeWatcher*> (lpParameter);
	while (not _THS_->fQuitting and _THS_->fWatchEvent != INVALID_HANDLE_VALUE) {
		HANDLE	events[2];
		events[0] = _THS_->fDoneEvent;
		events[1] = _THS_->fWatchEvent;
		::WaitForMultipleObjects (NEltsOf (events), events, false, INFINITE);
		Verify (::FindNextChangeNotification (_THS_->fWatchEvent));
		if (not _THS_->fQuitting) {
			_THS_->ValueChanged ();
		}
	}
}





/*
 ********************************************************************************
 ******************************* IO::AppTempFileManager *************************
 ********************************************************************************
 */
AppTempFileManager::AppTempFileManager ():
	fTmpDir ()
{
	TString	tmpDir	=	GetSpecialDir_GetTempDir ();

	TChar	exePath[MAX_PATH];
	memset (exePath, 0, sizeof exePath);
	Verify (::GetModuleFileName (NULL, exePath, NEltsOf (exePath)));

	TString	exeFileName	=	exePath;
	{
		size_t	i	=	exeFileName.rfind (_T ("\\"));
		if (i != TString::npos) {
			exeFileName = exeFileName.substr (i + 1);
		}
		// strip trailing .EXE
		i = exeFileName.rfind ('.');
		if (i != TString::npos) {
			exeFileName = exeFileName.erase (i);
		}
		// no biggie, but avoid spaces in tmpfile path name (but dont try too hard, should be
		// harmless)
		//	-- LGP 2009-08-16
		for (TString::iterator i = exeFileName.begin (); i != exeFileName.end (); ++i) {
			if (*i == ' ') {
				*i = '-';
			}
		}
	}
	// Need to include more than just the process-id, since code linked against this lib could be
	// loaded as a DLL into this process, and it would
	// use the same dir, and then delete it when that DLL exits (such as the rebranding DLL).
	//
	// But whatever we do, the DLL may do also, so we must use what is in the filesystem
	// to disambiguiate.
	//
	tmpDir += _T ("HealthFrameWorks\\");
	CreateDirectory (tmpDir);
	for (int i = 0; i < INT_MAX; ++i) {
		TString	d	=	tmpDir + Format (_T ("%s-%d-%d\\"), exeFileName.c_str (), ::GetCurrentProcessId (), i + rand ());
		if (not ::CreateDirectory (d.c_str (), NULL)) {
			DWORD error = ::GetLastError ();
			if (error == ERROR_ALREADY_EXISTS) {
				continue;	// try again
			}
			else {
				DbgTrace ("bad news if we cannot create AppTempFileManager::fTmpDir: %d", error);
				Execution::DoThrow (Platform::Windows::Exception (error));
			}
		}
		// we succeeded - good! Done...
		tmpDir = d;
		break;
	}
	fTmpDir = tmpDir;
	DbgTrace (_T ("AppTempFileManager::CTOR: created '%s'"), fTmpDir.c_str ());
}

AppTempFileManager::~AppTempFileManager ()
{
	DbgTrace (_T ("AppTempFileManager::DTOR: clearing '%s'"), fTmpDir.c_str ());
	DeleteAllFilesInDirectory (fTmpDir, true);
	Verify (::RemoveDirectory (fTmpDir.c_str ()));
}

TString	AppTempFileManager::GetTempFile (const TString& fileNameBase)
{
	TString	fn	=	AppTempFileManager::Get ().GetMasterTempDir () + fileNameBase;
	IO::CreateDirectoryForFile (fn);

	TString::size_type	suffixStart = fn.rfind ('.');
	if (suffixStart == TString::npos) {
		fn += _T (".txt");
		suffixStart = fn.rfind ('.');
	}
	int	attempts = 0;
	while (attempts < 5) {
		TString	s = fn;
		char	buf[100];
		(void)::snprintf (buf, NEltsOf (buf), "%d", ::rand ());
		s.insert (suffixStart, ToTString (buf));
		if (not FileExists (s.c_str ())) {
			HANDLE	f = ::CreateFile (s.c_str (), FILE_ALL_ACCESS, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			if (f != NULL) {
				::CloseHandle (f);
				DbgTrace (_T ("AppTempFileManager::GetTempFile (): returning '%s'"), s.c_str ());
				return s;
			}
		}
	}
	Execution::DoThrow (StringException (L"Unknown error creating file"), "AppTempFileManager::GetTempFile (): failed to create tempfile");
}

TString	AppTempFileManager::GetTempDir (const TString& fileNameBase)
{
	TString	fn	=	AppTempFileManager::Get ().GetMasterTempDir () + fileNameBase;

	int	attempts = 0;
	while (attempts < 5) {
		TString	s = fn;
		char	buf[100];
		(void)::snprintf (buf, NEltsOf (buf), "%d\\", ::rand ());
		s.append (ToTString  (buf));
		if (not DirectoryExists (s)) {
			CreateDirectory (s, true);
			DbgTrace (_T ("AppTempFileManager::GetTempDir (): returning '%s'"), s.c_str ());
			return s;
		}
	}
	Execution::DoThrow (StringException (L"Unknown error creating temporary file"), "AppTempFileManager::GetTempDir (): failed to create tempdir");
}









/*
 ********************************************************************************
 **************************** TempFileLibrarian *********************************
 ********************************************************************************
 */
TempFileLibrarian::TempFileLibrarian (const TString& privateDirectory, bool purgeDirectory, bool makeTMPDIRRel, bool deleteFilesOnDescruction)
	: fFiles ()
	, fPrivateDirectory (privateDirectory)
	, fMakeTMPDIRRel (makeTMPDIRRel)
	, fDeleteFilesOnDescruction (deleteFilesOnDescruction)
	, fCriticalSection ()
{
	::srand (static_cast<unsigned int> (::time (0)));
	if (purgeDirectory and fPrivateDirectory.size () > 0) {
		DeleteAllFilesInDirectory (AppTempFileManager::Get ().GetMasterTempDir () + fPrivateDirectory + _T ("\\"));
	}
}

TempFileLibrarian::~TempFileLibrarian ()
{
	if (fDeleteFilesOnDescruction) {
		for (set<TString>::iterator it = fFiles.begin (); it != fFiles.end (); ++it) {
			// item could be a file or directory, so see if dir delete works, and only if that fails,
			// then try to delete the item as a directory ... all silently ignoring failures...
			if (::DeleteFile (it->c_str ()) == 0) {
				IO::DeleteAllFilesInDirectory (*it);
				(void)::RemoveDirectory (it->c_str ());
			}
		}
		if (fPrivateDirectory.size () > 0) {
			(void)::RemoveDirectory ((AppTempFileManager::Get ().GetMasterTempDir () + fPrivateDirectory + _T ("\\")).c_str ());
		}
	}
}

TString	TempFileLibrarian::GetTempFile (const TString& fileNameBase)
{
	TString	fn	=	fileNameBase;
	if (fn.find (':') == -1) {
		if (fPrivateDirectory.size () > 0) {
			fn = fPrivateDirectory + _T ("\\") + fn;
		}

		if (fMakeTMPDIRRel) {
			fn = AppTempFileManager::Get ().GetMasterTempDir () + fn;
		}
	}
	IO::CreateDirectoryForFile (fn);

	TString::size_type	suffixStart = fn.rfind ('.');
	if (suffixStart == TString::npos) {
		fn += _T (".txt");
		suffixStart = fn.rfind ('.');
	}
	
	int	attempts = 0;
	while (attempts < 5) {
		TString	s = fn;
		char	buf[100];
		(void)::snprintf (buf, NEltsOf (buf), "%d", ::rand ());
		s.insert (suffixStart, ToTString (buf));
		if (not IO::FileExists (s.c_str ())) {
			HANDLE	f = ::CreateFile (s.c_str (), FILE_ALL_ACCESS, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			if (f != NULL) {
				CloseHandle (f);
				AutoCriticalSection enterCriticalSection (fCriticalSection);
				fFiles.insert (s);
				return s;
			}
		}
	}
	Execution::DoThrow (StringException (L"Unknown error creating file"));
}

TString	TempFileLibrarian::GetTempDir (const TString& fileNameBase)
{
	TString	fn	=	fileNameBase;
	if (fn.find (':') == -1) {
		if (fPrivateDirectory.size () > 0) {
			fn = fPrivateDirectory + _T ("\\") + fn;
		}
		if (fMakeTMPDIRRel) {
			fn = AppTempFileManager::Get ().GetMasterTempDir () + fn;
		}
	}

	int	attempts = 0;
	while (attempts < 5) {
		TString	s = fn;
		char	buf[100];
		{
			// man page doesn't gaurantee thread-safety of rand ()
			AutoCriticalSection enterCriticalSection (fCriticalSection);
			(void)::snprintf (buf, NEltsOf (buf), "%d\\", ::rand ());
		}
		s.append (ToTString  (buf));
		if (not IO::DirectoryExists (s)) {
			IO::CreateDirectory (s, true);
			AutoCriticalSection enterCriticalSection (fCriticalSection);
			fFiles.insert (s);
			return s;
		}
	}
	Execution::DoThrow (StringException (L"Unknown error creating temporary file"));
}







/*
 ********************************************************************************
 ************************************ IO::ScopedTmpDir **************************
 ********************************************************************************
 */
ScopedTmpDir::ScopedTmpDir (const TString& fileNameBase)
	: fTmpDir (AppTempFileManager::Get ().GetTempDir (fileNameBase))
{
}

ScopedTmpDir::~ScopedTmpDir ()
{
	try {
		DbgTrace (_T ("ScopedTmpDir::~ScopedTmpDir - removing contents for '%s'"), fTmpDir.c_str ());
		DeleteAllFilesInDirectory (fTmpDir);
		Verify (::RemoveDirectory (fTmpDir.c_str ()));
	}
	catch (...) {
		DbgTrace ("ignoring exception in ~ScopedTmpDir - removing tmpfiles");
	}
}






/*
 ********************************************************************************
 ************************ IO::ScopedTmpFile ******************************
 ********************************************************************************
 */
ScopedTmpFile::ScopedTmpFile (const TString& fileNameBase):
	fTmpFile (AppTempFileManager::Get ().GetTempFile (fileNameBase))
{
}

ScopedTmpFile::~ScopedTmpFile ()
{
	try {
		DbgTrace (_T ("ScopedTmpFile::~ScopedTmpFile - removing '%s'"), fTmpFile.c_str ());
		ThrowIfFalseGetLastError (::DeleteFile (fTmpFile.c_str ()));
	}
	catch (...) {
		DbgTrace ("ignoring exception in ~ScopedTmpFile - removing tmpfile");
	}
}






/*
 ********************************************************************************
 ********************** IO::ThroughTmpFileWriter *************************
 ********************************************************************************
 */
ThroughTmpFileWriter::ThroughTmpFileWriter (const TString& realFileName, const TString& tmpSuffix):
	fRealFilePath (realFileName),
	fTmpFilePath (realFileName + tmpSuffix)
{
	Require (not realFileName.empty ());
	Require (not tmpSuffix.empty ());
}

ThroughTmpFileWriter::~ThroughTmpFileWriter ()
{
	if (not fRealFilePath.empty ()) {
		DbgTrace (_T ("ThroughTmpFileWriter::DTOR - tmpfile not successfully commited to '%s'"), fRealFilePath.c_str ());
		(void)::DeleteFile (fTmpFilePath.c_str ());
	}
}

void	ThroughTmpFileWriter::Commit ()
{
	Require (not fTmpFilePath.empty ());	// cannot Commit more than once
											// Also - NOTE - you MUST close fTmpFilePath (any file descriptors that have opened it) BEFORE the Commit!
	try {
		ThrowIfFalseGetLastError (::MoveFileEx (fTmpFilePath.c_str (), fRealFilePath.c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH));
	}
	catch (const Platform::Windows::Exception& we) {
		// On Win9x - this fails cuz OS not impl...
		if (static_cast<DWORD> (we) == ERROR_CALL_NOT_IMPLEMENTED) {
			::DeleteFile (fRealFilePath.c_str ());
			ThrowIfFalseGetLastError (::MoveFile (fTmpFilePath.c_str (), fRealFilePath.c_str ()));
		}
		else {
			Execution::DoReThrow ();
		}
	}
	fRealFilePath.clear ();
	fTmpFilePath.clear ();
}












/*
 ********************************************************************************
 ************************************* FileReader *******************************
 ********************************************************************************
 */
FileReader::FileReader (const TChar* fileName)
{
	const	bool	kUseWIN32FILEIOAPI	=	true;	// else on Win2k reading files throgh VMWare mounting mechanism -
													// get error on read... not real importnat - but if low-level Win32
													// works better for that - it may elsewhere too
	if (kUseWIN32FILEIOAPI) {
		// OPEN
		// FILE_READ_DATA fails on WinME - generates ERROR_INVALID_PARAMETER - so use GENERIC_READ
		HANDLE	fd = ::CreateFile (fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		ThrowIfFalseGetLastError (fd != INVALID_HANDLE_VALUE);

		Byte*	fileData	=	NULL;
		size_t	fileLen		=	0;
		try {
			// Get LENGTH
			fileLen	=	::SetFilePointer (fd, 0, NULL, FILE_END);
			ThrowIfFalseGetLastError (fileLen != INVALID_SET_FILE_POINTER);
			ThrowIfFalseGetLastError (::SetFilePointer (fd, 0, NULL, FILE_BEGIN) == 0);

			fileData = DEBUG_NEW Byte [fileLen];
//BAD CODE - WONT WORK FOR FILES > 4GB
			// READ IN DATA
			DWORD	osRead	=	0;
			ThrowIfFalseGetLastError (::ReadFile (fd, fileData, static_cast<DWORD> (fileLen), &osRead, NULL));
			ThrowIfFalseGetLastError (osRead == fileLen);
		}
		catch (...) {
			::CloseHandle  (fd);
			delete[] fileData;
			Execution::DoReThrow ();
		}

		// Close file, and update data pointers to stored file data...
		::CloseHandle  (fd);
		fFileDataStart = fileData;
		fFileDataEnd = fileData + fileLen;
	}
	else {
		// OPEN
		int	fd	=	-1;
		ThrowIfError_errno_t (::_tsopen_s (&fd, fileName, O_RDONLY | O_BINARY, _SH_DENYNO, 0));
		ThrowIfFalseGetLastError (fd != -1);

		// Get LENGTH
		size_t	fileLen	= ::_lseek (fd, 0, SEEK_END);

		Byte*	fileData	=	NULL;
		try {
			fileData = DEBUG_NEW Byte [fileLen];

			// READ IN DATA
			::_lseek (fd, 0, SEEK_SET);
			ThrowIfFalseGetLastError (::_read (fd, fileData, fileLen) == int (fileLen));
		}
		catch (...) {
			::_close (fd);
			delete[] fileData;
			Execution::DoReThrow ();
		}

		// Close file, and update data pointers to stored file data...
		::_close (fd);
		fFileDataStart = fileData;
		fFileDataEnd = fileData + fileLen;
	}
}

FileReader::~FileReader ()
{
	delete[] fFileDataStart;
}





/*
 ********************************************************************************
 *********************************** FileWriter *********************************
 ********************************************************************************
 */
FileWriter::FileWriter (const TChar* fileName)
	: fFD (-1)
{
	ThrowIfError_errno_t (::_tsopen_s (&fFD, fileName, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE));
	ThrowIfFalseGetLastError (fFD != -1);
}

FileWriter::~FileWriter ()
{
	if (fFD != -1) {
		::_close (fFD);
	}
}

void	FileWriter::Append (const Byte* data, size_t count)
{
	RequireNotNil (fFD != -1);
	ThrowIfFalseGetLastError (::_write (fFD, data, static_cast<unsigned int> (count)) == static_cast<int> (count));
}






/*
 ********************************************************************************
 ***************************** MemoryMappedFileReader ***************************
 ********************************************************************************
 */
MemoryMappedFileReader::MemoryMappedFileReader (const TChar* fileName):
	fFileDataStart (NULL),
	fFileDataEnd (NULL),
	fFileHandle (INVALID_HANDLE_VALUE),
	fFileMapping (INVALID_HANDLE_VALUE)
{
	try {
		// FILE_READ_DATA fails on WinME - generates ERROR_INVALID_PARAMETER - so use GENERIC_READ
		fFileHandle = ::CreateFile (fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		ThrowIfFalseGetLastError (fFileHandle != INVALID_HANDLE_VALUE);
		DWORD	fileSize	=	::GetFileSize (fFileHandle, NULL);
		if (fileSize != 0) {
			fFileMapping = ::CreateFileMapping (fFileHandle, NULL, PAGE_READONLY, 0, fileSize, 0);
			ThrowIfFalseGetLastError (fFileMapping != NULL);
			fFileDataStart = reinterpret_cast<const Byte*> (::MapViewOfFile (fFileMapping, FILE_MAP_READ, 0, 0, NULL));
			ThrowIfFalseGetLastError (fFileDataStart != NULL);
			fFileDataEnd = fFileDataStart + fileSize;
		}
	}
	catch (...) {
		if (fFileMapping != INVALID_HANDLE_VALUE) {
			::CloseHandle (fFileMapping);
		}
		if (fFileHandle != INVALID_HANDLE_VALUE) {
			::CloseHandle (fFileHandle);
		}
		Execution::DoReThrow ();
	}
}

MemoryMappedFileReader::~MemoryMappedFileReader ()
{
	if (fFileDataStart != NULL) {
		(void)::UnmapViewOfFile (fFileDataStart);
	}
	if (fFileMapping != INVALID_HANDLE_VALUE) {
		::CloseHandle (fFileMapping);
	}
	if (fFileHandle != INVALID_HANDLE_VALUE) {
		::CloseHandle (fFileHandle);
	}
}




/*
 ********************************************************************************
 *********************** IO::AdjustSysErrorMode **************************
 ********************************************************************************
 */

UINT	AdjustSysErrorMode::GetErrorMode ()
{
	UINT	good	=	::SetErrorMode (0);
	::SetErrorMode (good);
	return good;
}

AdjustSysErrorMode::AdjustSysErrorMode (UINT newErrorMode):
	fSavedErrorMode (::SetErrorMode (newErrorMode))
{
}

AdjustSysErrorMode::~AdjustSysErrorMode ()
{
	(void)::SetErrorMode (fSavedErrorMode);
}






/*
 ********************************************************************************
 ************************ DirectoryContentsIterator *****************************
 ********************************************************************************
 */
DirectoryContentsIterator::DirectoryContentsIterator (const TString& pathExpr):
	fHandle (INVALID_HANDLE_VALUE),
	fFindFileData (),
	fDirectory (pathExpr)
{
	memset (&fFindFileData, 0, sizeof (fFindFileData));
	size_t i = fDirectory.rfind ('\\');
	if (i == wstring::npos) {
		Execution::DoThrow (StringException (L"Cannot find final '\\' in directory path"));
	}
	fDirectory = fDirectory.substr (0, i+1);
	fHandle = ::FindFirstFile (pathExpr.c_str (), &fFindFileData);
}

DirectoryContentsIterator::~DirectoryContentsIterator ()
{
	if (fHandle != INVALID_HANDLE_VALUE) {
		::FindClose (fHandle);
	}
}

bool	DirectoryContentsIterator::NotAtEnd () const
{
	return fHandle != INVALID_HANDLE_VALUE;
}

TString	DirectoryContentsIterator::operator *() const
{
	Ensure (NotAtEnd ());
	return fDirectory + fFindFileData.cFileName;
}

void	DirectoryContentsIterator::operator++ ()
{
	memset (&fFindFileData, 0, sizeof (fFindFileData));
	if (::FindNextFile (fHandle, &fFindFileData) == 0) {
		::FindClose (fHandle);
		fHandle = INVALID_HANDLE_VALUE;
	}
}



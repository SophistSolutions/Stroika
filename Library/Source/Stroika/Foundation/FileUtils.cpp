/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"StroikaPreComp.h"

#include	<aclapi.h>
#include	<shlobj.h>

#include	<sys/stat.h>
#include	<ctime>
#include	<limits>
#include	<fstream>
#include	<fcntl.h>
#include	<io.h>
#include	<cstdio>
#include	<shlwapi.h>
#include	<windows.h>
#include	<tchar.h>

#include	"Exceptions.h"
#include	"SetUtils.h"
#include	"Trace.h"

#include	"FileUtils.h"

using	namespace	Stroika;
using	namespace	Stroika::Foundation;


using	namespace	StringUtils;
using	namespace	FileUtils;
using	namespace	Exceptions;

using	ThreadUtils::AutoCriticalSection;












/*
 ********************************************************************************
 ******************** FileUtils::Private::UsingModuleHelper *********************
 ********************************************************************************
 */
FileUtils::Private::UsingModuleHelper::UsingModuleHelper ()
	:fAppTempFileManager ()
{
}

FileUtils::Private::UsingModuleHelper::~UsingModuleHelper ()
{
}





/*
 ********************************************************************************
 ********************* FileUtils::GetSpecialDir_MyDocuments *********************
 ********************************************************************************
 */
tstring	FileUtils::GetSpecialDir_MyDocuments (bool createIfNotPresent)
{
	TCHAR	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	ThrowIfFalseGetLastError (::SHGetSpecialFolderPath (NULL, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
	tstring	result = fileBuf;
	// Assure non-empty result
	if (result.empty ()) {
		result = _T("c:");	// shouldn't happen
	}
	// assure ends in '\'
	if (result[result.size ()-1] != '\\') {
		result += _T("\\");
	}
	Ensure (result[result.size ()-1] == '\\');
	Ensure (not createIfNotPresent or DirectoryExists (result));
	return result;
}






/*
 ********************************************************************************
 ********************* FileUtils::GetSpecialDir_AppData *************************
 ********************************************************************************
 */
tstring	FileUtils::GetSpecialDir_AppData (bool createIfNotPresent)
{
	TCHAR	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	Verify (::SHGetSpecialFolderPath (NULL, fileBuf, CSIDL_COMMON_APPDATA, createIfNotPresent));
	tstring	result = fileBuf;
	// Assure non-empty result
	if (result.empty ()) {
		result = _T("c:");	// shouldn't happen
	}
	// assure ends in '\'
	if (result[result.size ()-1] != '\\') {
		result += _T("\\");
	}
	Ensure (result[result.size ()-1] == '\\');
	Ensure (not createIfNotPresent or DirectoryExists (result));
	return result;
}






/*
 ********************************************************************************
 ********************** FileUtils::GetSpecialDir_WinSxS *************************
 ********************************************************************************
 */
tstring	FileUtils::GetSpecialDir_WinSxS ()
{
	TCHAR	fileBuf[MAX_PATH];
	memset (fileBuf, 0, sizeof (fileBuf));
	Verify (::SHGetSpecialFolderPath (NULL, fileBuf, CSIDL_WINDOWS, false));
	tstring	result = fileBuf;
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
 ********************** FileUtils::GetSpecialDir_GetTempDir *********************
 ********************************************************************************
 */
tstring	FileUtils::GetSpecialDir_GetTempDir ()
{
	tstring	tempPath;
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
 ********************* FileUtils::FileAccessException ***************************
 ********************************************************************************
 */
namespace	{
	wstring	mkMessage_ (const tstring& fileName, FileAccessMode fileAccessMode)
		{
			wstring	message;
			if ((fileAccessMode & eRead_FAM) and (fileAccessMode & eWrite_FAM)) {
				message = L"Cannot read from or write to file";
			}
			else if (fileAccessMode & eRead_FAM) {
				message = L"Cannot read from file";
			}
			else if (fileAccessMode & eWrite_FAM) {
				message = L"Cannot write to file";
			}
			else {
				message = L"Access failure for file";
			}
			if (not fileName.empty ()) {
				message = Format (L"%s: '%.200s'", message.c_str (), StringUtils::LimitLength (tstring2Wide (fileName), 100, false).c_str ());
			}
			return message;
		}
}
FileAccessException::FileAccessException (const tstring& fileName, FileAccessMode fileAccessMode)
	:StringException (mkMessage_ (fileName, fileAccessMode))
	,fFileName (fileName)
	,fFileAccessMode (fileAccessMode)
{
}



/*
 ********************************************************************************
 ************************* FileUtils::CheckFileAccess ***************************
 ********************************************************************************
 */
void	FileUtils::CheckFileAccess (const tstring& fileFullPath, bool checkCanRead, bool checkCanWrite)
{
	// quick hack - not fully implemented - but since advsiory only - not too important...

	if (not FileExists (fileFullPath)) {
// FOR NOW - MIMIC OLD CODE - BUT FIX TO CHECK READ AND WRITE (AND BOTH) ACCESS DEPENDING ON ARGS) -- LGP 2009-08-15
		Exceptions::DoThrow (FileAccessException (fileFullPath, FileUtils::eRead_FAM));
	}
}








/*
 ********************************************************************************
 *************** FileUtils::AssureDirectoryPathSlashTerminated ******************
 ********************************************************************************
 */
tstring	FileUtils::AssureDirectoryPathSlashTerminated (const tstring& dirPath)
{
	if (dirPath.empty ()) {
		Assert (false);	// not sure if this is an error or not. Not sure how code used.
						// put assert in there to find out... Probably should THROW!
						//		-- LGP 2009-05-12
		return _T ("\\");
	}
	else {
		TCHAR	lastChar = dirPath[dirPath.length ()-1];
		return (lastChar == '\\')?
				dirPath: 
				(dirPath + _T ("\\"))
			;
	}
}







/*
 ********************************************************************************
 ************************ FileUtils::SafeFilenameChars **************************
 ********************************************************************************
 */
tstring	FileUtils::SafeFilenameChars (const tstring& s)
{
	wstring	tmp	=	tstring2Wide (s);	// analyze as wide-char string so we don't mis-identify
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
	return Wide2tstring (tmp);
}







/*
 ********************************************************************************
 ************************** FileUtils::ResolveShortcut **************************
 ********************************************************************************
 */
tstring	FileUtils::ResolveShortcut (const tstring& path2FileOrShortcut)
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
			if (SUCCEEDED (ppf->Load (tstring2Wide (path2FileOrShortcut).c_str (), STGM_READ))) {
				// Resolve the link, this may post UI to find the link
				if (SUCCEEDED (psl->Resolve(0, SLR_NO_UI))) {
					TCHAR	path[MAX_PATH+1];
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
		Exceptions::DoReThrow ();
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
 ****************** FileUtils::FileSizeToDisplayString **************************
 ********************************************************************************
 */
wstring	FileUtils::FileSizeToDisplayString (FileOffset_t bytes)
{
	if (bytes < 1000) {
		return Format (L"%d bytes", static_cast<int> (bytes));
	}
	else if (bytes < 1000 * 1024) {
		return Format (L"%.1f K", static_cast<float> (bytes)/1024.0f);
	}
	else {
		return Format (L"%.1f MB", static_cast<float> (bytes)/(1024*1024.0f));
	}
}





/*
 ********************************************************************************
 ****************************** FileUtils::GetFileSize **************************
 ********************************************************************************
 */
FileOffset_t	FileUtils::GetFileSize (const tstring& fileName)
{
	WIN32_FILE_ATTRIBUTE_DATA	fileAttrData;
	(void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
	ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
	return fileAttrData.nFileSizeLow + (static_cast<FileOffset_t> (fileAttrData.nFileSizeHigh) << 32);
}






/*
 ********************************************************************************
 ********************** FileUtils::GetFileLastModificationDate ******************
 ********************************************************************************
 */
DateTime		FileUtils::GetFileLastModificationDate (const tstring& fileName)
{
	WIN32_FILE_ATTRIBUTE_DATA	fileAttrData;
	(void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
	ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
	return DateTime (fileAttrData.ftLastWriteTime);
}






/*
 ********************************************************************************
 ************************ FileUtils::GetFileLastAccessDate **********************
 ********************************************************************************
 */
DateTime			FileUtils::GetFileLastAccessDate (const tstring& fileName)
{
	WIN32_FILE_ATTRIBUTE_DATA	fileAttrData;
	(void)::memset (&fileAttrData, 0, sizeof (fileAttrData));
	ThrowIfFalseGetLastError (::GetFileAttributesEx (fileName.c_str (), GetFileExInfoStandard, &fileAttrData));
	return DateTime (fileAttrData.ftLastAccessTime);
}




/*
 ********************************************************************************
 ********************** FileUtils::SetFileAccessWideOpened **********************
 ********************************************************************************
 */
/*
 * Sets permissions for users on a given folder to full control
 *
 *		Add 'Everyone' to have FULL ACCESS to the given argument file
 *
 */
void	FileUtils::SetFileAccessWideOpened (const tstring& filePathName)
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
			const_cast<TCHAR*> (filePathName.c_str ()),          // name of the object
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
 ************************* FileUtils::CreateDirectory ***************************
 ********************************************************************************
 */
void	FileUtils::CreateDirectory (const tstring& directoryPath, bool createParentComponentsIfNeeded)
{
	if (createParentComponentsIfNeeded) {
		// walk path and break into parts, and from top down - try to create parent directory structure.
		// Ignore any failures - and just let the report of failure (if any must result) come from original basic
		// CreateDirectory call.
		size_t	index	=	directoryPath.find (_T ("\\"));
		while (index != -1 and index + 1 < directoryPath.length ()) {
			tstring	parentPath = directoryPath.substr (0, index);
			IgnoreExceptionsForCall (CreateDirectory (parentPath, false));
			index = directoryPath.find ('\\', index+1);
		}
	}

	if (not ::CreateDirectory (directoryPath.c_str (), NULL)) {
		DWORD error = ::GetLastError ();
		if (error != ERROR_ALREADY_EXISTS) {
			Exceptions::DoThrow (Exceptions::Win32ErrorException (error));
		}
	}
}







/*
 ********************************************************************************
 ****************** FileUtils::CreateDirectoryForFile ***************************
 ********************************************************************************
 */
void	FileUtils::CreateDirectoryForFile (const tstring& filePath)
{
	if (filePath.empty ()) {
		Exceptions::DoThrow (Exceptions::Win32ErrorException (ERROR_FILE_NOT_FOUND));
	}
	if (FileExists (filePath)) {
		// were done
		return;
	}
	CreateDirectory (GetFileDirectory (filePath), true);
}






/*
 ********************************************************************************
 *************************** FileUtils::GetVolumeName ***************************
 ********************************************************************************
 */
tstring	FileUtils::GetVolumeName (const tstring& driveLetterAbsPath)
{
	// SEM_FAILCRITICALERRORS needed to avoid dialog in call to GetVolumeInformation
	AdjustSysErrorMode	errorModeAdjuster (AdjustSysErrorMode::GetErrorMode () | SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	DWORD	ignored	=	0;
	TCHAR	volNameBuf[1024];
	memset (volNameBuf, 0, sizeof (volNameBuf));
	TCHAR	igBuf[1024];
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
	return tstring ();
}





/*
 ********************************************************************************
 ********************** FileUtils::AssureLongFileName ***************************
 ********************************************************************************
 */
tstring	FileUtils::AssureLongFileName (const tstring& fileName)
{
	DWORD	r	=	::GetLongPathName (fileName.c_str (), NULL, 0);
	if (r != 0) {
		SmallStackBuffer<TCHAR>	buf (r);
		r = ::GetLongPathName (fileName.c_str (), buf, r);
		if (r != 0) {
			return tstring (buf);
		}
	}
	return fileName;
}





/*
 ********************************************************************************
 *************************** FileUtils::GetFileSuffix ***************************
 ********************************************************************************
 */
tstring	FileUtils::GetFileSuffix (const tstring& fileName)
{
	tstring	useFName	=	fileName;

	{
		TCHAR	fNameBuf[4 * MAX_PATH ];
		fNameBuf[0] = '\0';
		DWORD	r	=	::GetLongPathName (fileName.c_str (), fNameBuf, NEltsOf (fNameBuf)-1);
		if (r != 0) {
			useFName = fNameBuf;
		}
	}
	TCHAR	fname[_MAX_FNAME];
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	ext[_MAX_EXT];
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
 ************************* FileUtils::GetFileBaseName ***************************
 ********************************************************************************
 */
tstring	FileUtils::GetFileBaseName (const tstring& pathName)
{
	tstring	useFName	=	pathName;

	{
		TCHAR	fNameBuf[4 * MAX_PATH ];
		DWORD	r	=	GetLongPathName (pathName.c_str (), fNameBuf, NEltsOf (fNameBuf)-1);
		if (r != 0) {
			useFName = fNameBuf;
		}
	}
	TCHAR	fname[_MAX_FNAME];
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	ext[_MAX_EXT];
	memset (drive, 0, sizeof (drive));
	memset (dir, 0, sizeof (dir));
	memset (fname, 0, sizeof (fname));
	memset (ext, 0, sizeof (ext));
	::_tsplitpath_s (useFName.c_str (), drive, dir, fname, ext);
	return fname;
}





/*
 ********************************************************************************
 ************************* FileUtils::StripFileSuffix ***************************
 ********************************************************************************
 */
tstring	FileUtils::StripFileSuffix (const tstring& pathName)
{
	tstring	useFName	=	pathName;
	tstring	fileSuffix	=	GetFileSuffix (pathName);
	if (useFName.length () > fileSuffix.length ()) {
		return useFName.substr (0, useFName.length () - fileSuffix.length ());
	}
	else {
		return useFName;
	}
}






/*
 ********************************************************************************
 ************************* FileUtils::GetFileDirectory **************************
 ********************************************************************************
 */
tstring	FileUtils::GetFileDirectory (const tstring& pathName)
{
	// could use splitpath, but this maybe better, since works with \\UNCNAMES
	tstring	tmp		=	pathName;
	size_t	idx		=	tmp.rfind ('\\');
	if (idx != tstring::npos) {
		tmp.erase (idx + 1);
	}
	return tmp;
}







/*
 ********************************************************************************
 ****************************** FileUtils::FileExists ***************************
 ********************************************************************************
 */
bool	FileUtils::FileExists (const TCHAR* filePath)
{
	RequireNotNil (filePath);
	DWORD attribs = ::GetFileAttributes (filePath);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}	
	return not (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

bool	FileUtils::FileExists (const tstring& filePath)
{
	return FileExists (filePath.c_str ());
}





/*
 ********************************************************************************
 ************************* FileUtils::DirectoryExists ***************************
 ********************************************************************************
 */
bool	FileUtils::DirectoryExists (const TCHAR* filePath)
{
	RequireNotNil (filePath);
	DWORD attribs = ::GetFileAttributes (filePath);
	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return false;
	}	
	return !! (attribs & FILE_ATTRIBUTE_DIRECTORY);
}

bool	FileUtils::DirectoryExists (const tstring& filePath)
{
	return DirectoryExists (filePath.c_str ());
}







/*
 ********************************************************************************
 ******************************** FileUtils::CopyFile ***************************
 ********************************************************************************
 */
void	FileUtils::CopyFile (const tstring& srcFile, const tstring& destPath)
{
// see if can be/should be rewritten to use Win32 API of same name!!!
//
// If I DONT do that remapping to Win32 API, then redo this at least to copy / rename through tmpfile
	if (not FileExists (srcFile)) {
		Exceptions::DoThrow (FileAccessException (srcFile, FileUtils::eRead_FAM));
	}
	CreateDirectoryForFile (destPath);
	ThrowIfFalseGetLastError (::CopyFile (destPath.c_str (), srcFile.c_str (), false));
}









/*
 ********************************************************************************
 ****************************** FileUtils::FindFiles ****************************
 ********************************************************************************
 */
vector<tstring>	FileUtils::FindFiles (const tstring& path, const tstring& fileNameToMatch)
{
	vector<tstring>	result;
	if (path.empty ()) {
		return result;
	}
	tstring	usePath			=	AssureDirectoryPathSlashTerminated (path);
	tstring	matchFullPath	=	usePath + (fileNameToMatch.empty ()? _T("*"): fileNameToMatch);
	WIN32_FIND_DATA fd;
	memset (&fd, 0, sizeof (fd));
	HANDLE hFind = ::FindFirstFile (matchFullPath.c_str (), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		try {
			do {
				tstring fileName = fd.cFileName;
				if (not (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					result.push_back (usePath + fileName);
				}
			} while (::FindNextFile (hFind, &fd));
		}
		catch (...) {
			::FindClose (hFind);
			Exceptions::DoReThrow ();
		}
		::FindClose (hFind);
	}
	return result;
}








/*
 ********************************************************************************
 ************************** FileUtils::FindFilesOneDirUnder *********************
 ********************************************************************************
 */
vector<tstring>	FileUtils::FindFilesOneDirUnder (const tstring& path, const tstring& fileNameToMatch)
{
	if (path.empty ()) {
		return vector<tstring> ();
	}

	set<tstring>	resultSet;
	tstring	usePath	=	AssureDirectoryPathSlashTerminated (path);
	WIN32_FIND_DATA fd;
	memset (&fd, 0, sizeof (fd));
	HANDLE hFind = ::FindFirstFile ((usePath + _T ("*")).c_str (), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			tstring fileName = (LPCTSTR) &fd.cFileName;
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				tstring fileName = (LPCTSTR) &fd.cFileName;
				const	tstring	kDOT	=	_T (".");
				const	tstring	kDOTDOT	=	_T ("..");
				if ((fileName != kDOT) and (fileName != kDOTDOT)) {
					vector<tstring>	subdirFiles	=	FindFiles (usePath + fileName, fileNameToMatch);
					resultSet = resultSet + set<tstring> (subdirFiles.begin (), subdirFiles.end ());
				}
			}
		} while (::FindNextFile (hFind, &fd));
		::FindClose (hFind);
	}
	return vector<tstring> (resultSet.begin (), resultSet.end ());
}








/*
 ********************************************************************************
 ****************** FileUtils::DeleteAllFilesInDirectory ************************
 ********************************************************************************
 */
void	FileUtils::DeleteAllFilesInDirectory (const tstring& path, bool ignoreErrors)
{
	if (path.empty ()) {
		Exceptions::DoThrow (Exceptions::Win32ErrorException (ERROR_INVALID_NAME));
	}
	tstring	dir2Use	=	AssureDirectoryPathSlashTerminated (path);

	WIN32_FIND_DATA	fd;
	(void)::memset (&fd, 0, sizeof (fd));
	HANDLE			hFind = ::FindFirstFile ((LPCTSTR) (dir2Use + _T ("*")).c_str (), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		try {
			do {
				tstring	fileName = fd.cFileName;
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if ((fileName != _T (".")) and (fileName != _T (".."))) {
						DeleteAllFilesInDirectory (dir2Use + fileName + _T ("\\"), ignoreErrors);
						try {
							ThrowIfFalseGetLastError (::RemoveDirectory ((dir2Use + fileName).c_str ()));
						}
						catch (...) {
							DbgTrace (_T ("Exception %s calling RemoveDirectory on file '%s'"), ignoreErrors? _T ("(ignored)"): _T (""), (dir2Use + fileName).c_str ());
							if (!ignoreErrors) {
								Exceptions::DoReThrow ();
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
							Exceptions::DoReThrow ();
						}
					}
				}
			} while (::FindNextFile (hFind, &fd));
		}
		catch (...) {
			::FindClose (hFind);
			Exceptions::DoReThrow ();
		}
		::FindClose (hFind);
	}
}





/*
 ********************************************************************************
 ***************************** FileUtils::WriteString ***************************
 ********************************************************************************
 */
void	FileUtils::WriteString (ostream& out, const wstring& s)
{
	string	s1 = WideStringToNarrow (s, CP_UTF8);
	out << s1.size ();
	out << ' ';
	out << s1 << '\t';
}






/*
 ********************************************************************************
 ****************************** FileUtils::ReadString ***************************
 ********************************************************************************
 */
wstring	FileUtils::ReadString (istream& in)
{
	int	strlen;
	in >> strlen;
	in.get ();	// throw away character between size and string
	
	string	s;
	s.reserve (strlen);
	for (int i = 0; i < strlen; ++i) {
		s += in.get ();
	}
	return NarrowStringToWide (s, CP_UTF8);
}








/*
 ********************************************************************************
 ******************************* FileUtils::ReadBytes ***************************
 ********************************************************************************
 */
vector<Byte>	FileUtils::ReadBytes (istream& in)
{
	streamoff	start	=	in.tellg ();
	in.seekg (0, ios_base::end);
	streamoff	end		=	in.tellg ();
	Assert (start <= end);
	if (end - start > numeric_limits<size_t>::max ()) {
		Exceptions::DoThrow (StringException (L"stream too large"));
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
 ****************************** FileUtils::WriteBytes ***************************
 ********************************************************************************
 */
void	FileUtils::WriteBytes (ostream& out, const vector<Byte>& s)
{
	out.write (reinterpret_cast<const char*> (R4LLib::Start (s)), s.size ());
}






/*
 ********************************************************************************
 ********************** FileUtils::DirectoryChangeWatcher ***********************
 ********************************************************************************
 */
FileUtils::DirectoryChangeWatcher::DirectoryChangeWatcher (const tstring& directoryName, bool watchSubTree, DWORD notifyFilter):
	fDirectory (directoryName),
	fWatchSubTree (watchSubTree),
	fThread (),
	fDoneEvent (::CreateEvent (NULL, false, false, NULL)),
	fWatchEvent (::FindFirstChangeNotification (fDirectory.c_str (), fWatchSubTree, notifyFilter)),
	fQuitting (false)
{
	fThread = ThreadUtils::SimpleThread (&ThreadProc, this);
	fThread.SetThreadName (L"DirectoryChangeWatcher");
	fThread.Start ();
}

FileUtils::DirectoryChangeWatcher::~DirectoryChangeWatcher ()
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

void	FileUtils::DirectoryChangeWatcher::ValueChanged ()
{
}

void	FileUtils::DirectoryChangeWatcher::ThreadProc (void* lpParameter)
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
 ************************ FileUtils::AppTempFileManager *************************
 ********************************************************************************
 */
AppTempFileManager::AppTempFileManager ():
	fTmpDir ()
{
	tstring	tmpDir	=	GetSpecialDir_GetTempDir ();

	TCHAR	exePath[MAX_PATH];
	memset (exePath, 0, sizeof exePath);
	Verify (::GetModuleFileName (NULL, exePath, NEltsOf (exePath)));

	tstring	exeFileName	=	exePath;
	{
		size_t	i	=	exeFileName.rfind (_T ("\\"));
		if (i != tstring::npos) {
			exeFileName = exeFileName.substr (i + 1);
		}
		// strip trailing .EXE
		i = exeFileName.rfind ('.');
		if (i != tstring::npos) {
			exeFileName = exeFileName.erase (i);
		}
		// no biggie, but avoid spaces in tmpfile path name (but dont try too hard, should be
		// harmless)
		//	-- LGP 2009-08-16
		for (tstring::iterator i = exeFileName.begin (); i != exeFileName.end (); ++i) {
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
		tstring	d	=	tmpDir + Format (_T ("%s-%d-%d\\"), exeFileName.c_str (), ::GetCurrentProcessId (), i + rand ());
		if (not ::CreateDirectory (d.c_str (), NULL)) {
			DWORD error = ::GetLastError ();
			if (error == ERROR_ALREADY_EXISTS) {
				continue;	// try again
			}
			else {
				DbgTrace ("bad news if we cannot create AppTempFileManager::fTmpDir: %d", error);
				Exceptions::DoThrow (Win32ErrorException (error));
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

tstring	AppTempFileManager::GetTempFile (const tstring& fileNameBase)
{
	tstring	fn	=	AppTempFileManager::Get ().GetMasterTempDir () + fileNameBase;
	FileUtils::CreateDirectoryForFile (fn);

	int	suffixStart = fn.rfind ('.');
	if (suffixStart == -1) {
		fn += _T (".txt");
		suffixStart = fn.rfind ('.');
	}
	int	attempts = 0;
	while (attempts < 5) {
		tstring	s = fn;
		char	buf[100];
		sprintf_s (buf, "%d", ::rand ());
		s.insert (suffixStart, totstring (buf));
		if (not FileExists (s.c_str ())) {
			HANDLE	f = ::CreateFile (s.c_str (), FILE_ALL_ACCESS, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			if (f != NULL) {
				::CloseHandle (f);
				DbgTrace (_T ("AppTempFileManager::GetTempFile (): returning '%s'"), s.c_str ());
				return s;
			}
		}
	}
	Exceptions::DoThrow (StringException (L"Unknown error creating file"), "AppTempFileManager::GetTempFile (): failed to create tempfile");
}

tstring	AppTempFileManager::GetTempDir (const tstring& fileNameBase)
{
	tstring	fn	=	AppTempFileManager::Get ().GetMasterTempDir () + fileNameBase;

	int	attempts = 0;
	while (attempts < 5) {
		tstring	s = fn;
		char	buf[100];
		(void)::sprintf_s (buf, "%d\\", ::rand ());
		s.append (totstring  (buf));
		if (not DirectoryExists (s)) {
			CreateDirectory (s, true);
			DbgTrace (_T ("AppTempFileManager::GetTempDir (): returning '%s'"), s.c_str ());
			return s;
		}
	}
	Exceptions::DoThrow (StringException (L"Unknown error creating temporary file"), "AppTempFileManager::GetTempDir (): failed to create tempdir");
}









/*
 ********************************************************************************
 **************************** TempFileLibrarian *********************************
 ********************************************************************************
 */
TempFileLibrarian::TempFileLibrarian (const tstring& privateDirectory, bool purgeDirectory, bool makeTMPDIRRel, bool deleteFilesOnDescruction):
	fFiles (),
	fPrivateDirectory (privateDirectory),
	fMakeTMPDIRRel (makeTMPDIRRel),
	fDeleteFilesOnDescruction (deleteFilesOnDescruction),
	fCriticalSection ()
{
	::srand (static_cast<unsigned int> (::time (0)));
	if (purgeDirectory and fPrivateDirectory.size () > 0) {
		DeleteAllFilesInDirectory (AppTempFileManager::Get ().GetMasterTempDir () + fPrivateDirectory + _T ("\\"));
	}
}

TempFileLibrarian::~TempFileLibrarian ()
{
	if (fDeleteFilesOnDescruction) {
		for (set<tstring>::iterator it = fFiles.begin (); it != fFiles.end (); ++it) {
			// item could be a file or directory, so see if dir delete works, and only if that fails,
			// then try to delete the item as a directory ... all silently ignoring failures...
			if (::DeleteFile (it->c_str ()) == 0) {
				FileUtils::DeleteAllFilesInDirectory (*it);
				(void)::RemoveDirectory (it->c_str ());
			}
		}
		if (fPrivateDirectory.size () > 0) {
			(void)::RemoveDirectory ((AppTempFileManager::Get ().GetMasterTempDir () + fPrivateDirectory + _T ("\\")).c_str ());
		}
	}
}

tstring	TempFileLibrarian::GetTempFile (const tstring& fileNameBase)
{
	tstring	fn	=	fileNameBase;
	if (fn.find (':') == -1) {
		if (fPrivateDirectory.size () > 0) {
			fn = fPrivateDirectory + _T ("\\") + fn;
		}

		if (fMakeTMPDIRRel) {
			fn = AppTempFileManager::Get ().GetMasterTempDir () + fn;
		}
	}
	FileUtils::CreateDirectoryForFile (fn);

	int	suffixStart = fn.rfind ('.');
	if (suffixStart == -1) {
		fn += _T (".txt");
		suffixStart = fn.rfind ('.');
	}
	
	int	attempts = 0;
	while (attempts < 5) {
		tstring	s = fn;
		char	buf[100];
		sprintf_s (buf, "%d", ::rand ());
		s.insert (suffixStart, totstring (buf));
		if (not FileUtils::FileExists (s.c_str ())) {
			HANDLE	f = ::CreateFile (s.c_str (), FILE_ALL_ACCESS, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			if (f != NULL) {
				CloseHandle (f);
				AutoCriticalSection enterCriticalSection (fCriticalSection);
				fFiles.insert (s);
				return s;
			}
		}
	}
	Exceptions::DoThrow (StringException (L"Unknown error creating file"));
}

tstring	TempFileLibrarian::GetTempDir (const tstring& fileNameBase)
{
	tstring	fn	=	fileNameBase;
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
		tstring	s = fn;
		char	buf[100];
		{
			// man page doesn't gaurantee thread-safety of rand ()
			AutoCriticalSection enterCriticalSection (fCriticalSection);
			(void)::sprintf_s (buf, "%d\\", ::rand ());
		}
		s.append (totstring  (buf));
		if (not FileUtils::DirectoryExists (s)) {
			FileUtils::CreateDirectory (s, true);
			AutoCriticalSection enterCriticalSection (fCriticalSection);
			fFiles.insert (s);
			return s;
		}
	}
	Exceptions::DoThrow (StringException (L"Unknown error creating temporary file"));
}







/*
 ********************************************************************************
 ***************************** FileUtils::ScopedTmpDir **************************
 ********************************************************************************
 */
ScopedTmpDir::ScopedTmpDir (const tstring& fileNameBase):
	fTmpDir (AppTempFileManager::Get ().GetTempDir (fileNameBase))
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
 ************************ FileUtils::ScopedTmpFile ******************************
 ********************************************************************************
 */
ScopedTmpFile::ScopedTmpFile (const tstring& fileNameBase):
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
 ********************** FileUtils::ThroughTmpFileWriter *************************
 ********************************************************************************
 */
ThroughTmpFileWriter::ThroughTmpFileWriter (const tstring& realFileName, const tstring& tmpSuffix):
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
	catch (const Win32ErrorException& we) {
		// On Win9x - this fails cuz OS not impl...
		if (static_cast<DWORD> (we) == ERROR_CALL_NOT_IMPLEMENTED) {
			::DeleteFile (fRealFilePath.c_str ());
			ThrowIfFalseGetLastError (::MoveFile (fTmpFilePath.c_str (), fRealFilePath.c_str ()));
		}
		else {
			Exceptions::DoReThrow ();
		}
	}
	fRealFilePath.clear ();
	fTmpFilePath.clear ();
}







/*
 ********************************************************************************
 **************************** FileUtils::GetEXEDir ******************************
 ********************************************************************************
 */
tstring	FileUtils::GetEXEDir ()
{
	return GetFileDirectory (GetEXEPath ());
}







/*
 ********************************************************************************
 ************************** FileUtils::GetEXEPath *******************************
 ********************************************************************************
 */
tstring	FileUtils::GetEXEPath ()
{
	TCHAR	buf[MAX_PATH];
	memset (buf, 0, sizeof (buf));
	Verify (::GetModuleFileName (NULL, buf, NEltsOf (buf)));
	return buf;
}










/*
 ********************************************************************************
 ************************************* FileReader *******************************
 ********************************************************************************
 */
FileReader::FileReader (const TCHAR* fileName)
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

			// READ IN DATA
			DWORD	osRead	=	0;
			ThrowIfFalseGetLastError (::ReadFile (fd, fileData, fileLen, &osRead, NULL));
			ThrowIfFalseGetLastError (osRead == fileLen);
		}
		catch (...) {
			::CloseHandle  (fd);
			delete[] fileData;
			Exceptions::DoReThrow ();
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
			Exceptions::DoReThrow ();
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
FileWriter::FileWriter (const TCHAR* fileName):
	fFD (-1)
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
	ThrowIfFalseGetLastError (::_write (fFD, data, count) == int (count));
}






/*
 ********************************************************************************
 ***************************** MemoryMappedFileReader ***************************
 ********************************************************************************
 */
MemoryMappedFileReader::MemoryMappedFileReader (const TCHAR* fileName):
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
		Exceptions::DoReThrow ();
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
 *********************** FileUtils::AdjustSysErrorMode **************************
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
DirectoryContentsIterator::DirectoryContentsIterator (const tstring& pathExpr):
	fHandle (INVALID_HANDLE_VALUE),
	fFindFileData (),
	fDirectory (pathExpr)
{
	memset (&fFindFileData, 0, sizeof (fFindFileData));
	size_t i = fDirectory.rfind ('\\');
	if (i == wstring::npos) {
		Exceptions::DoThrow (StringException (L"Cannot find final '\\' in directory path"));
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

tstring	DirectoryContentsIterator::operator *() const
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



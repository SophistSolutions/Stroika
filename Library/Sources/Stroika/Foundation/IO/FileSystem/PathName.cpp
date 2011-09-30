/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#include	"../../Characters/TString.h"
#include	"../../Memory/SmallStackBuffer.h"

#include	"PathName.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::FileSystem;





#if		!qCompilerAndStdLib_Supports_constexpr
	#if		qPlatform_Windows
	const	TChar	FileSystem::kPathComponentSeperator	=	'\\';
	#elif	qPlatform_POSIX
	const	TChar	FileSystem::kPathComponentSeperator	=	'/';
	#endif
#endif












/*
 ********************************************************************************
 ********************** FileSystem::AssureDirectoryPathSlashTerminated ******************
 ********************************************************************************
 */
TString	FileSystem::AssureDirectoryPathSlashTerminated (const TString& dirPath)
{
#if		qPlatform_Windows
	if (dirPath.empty ()) {
		Assert (false);	// not sure if this is an error or not. Not sure how code used.
						// put assert in there to find out... Probably should THROW!
						//		-- LGP 2009-05-12
		return TSTR ("\\");
	}
	else {
		TChar	lastChar = dirPath[dirPath.length ()-1];
		return (lastChar == '\\')?
				dirPath: 
				(dirPath + TSTR ("\\"))
			;
	}
#else
	AssertNotImplemented (); return TString ();
#endif
}







/*
 ********************************************************************************
 *********************** FileSystem::SafeFilenameChars **************************
 ********************************************************************************
 */
TString	FileSystem::SafeFilenameChars (const TString& s)
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
 ***************************** FileSystem::AssureLongFileName ***************************
 ********************************************************************************
 */
TString	FileSystem::AssureLongFileName (const TString& fileName)
{
	#if		qPlatform_Windows
		DWORD	r	=	::GetLongPathName (fileName.c_str (), nullptr, 0);
		if (r != 0) {
			Memory::SmallStackBuffer<TChar>	buf (r);
			r = ::GetLongPathName (fileName.c_str (), buf, r);
			if (r != 0) {
				return TString (buf);
			}
		}
	#endif
	return fileName;
}





/*
 ********************************************************************************
 ********************************** FileSystem::GetFileSuffix ***************************
 ********************************************************************************
 */
TString	FileSystem::GetFileSuffix (const TString& fileName)
{
#if		qPlatform_Windows
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
#else
	AssertNotImplemented ();
	return TString ();
#endif
}




/*
 ********************************************************************************
 ******************************** FileSystem::GetFileBaseName ***************************
 ********************************************************************************
 */
TString	FileSystem::GetFileBaseName (const TString& pathName)
{
#if		qPlatform_Windows
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
#else
	AssertNotImplemented ();
#endif
}





/*
 ********************************************************************************
 ******************************** FileSystem::StripFileSuffix ***************************
 ********************************************************************************
 */
TString	FileSystem::StripFileSuffix (const TString& pathName)
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
 ************************ FileSystem::GetFileDirectory **************************
 ********************************************************************************
 */
TString	FileSystem::GetFileDirectory (const TString& pathName)
{
#if		qPlatform_Windows
	// could use splitpath, but this maybe better, since works with \\UNCNAMES
	TString	tmp		=	pathName;
	size_t	idx		=	tmp.rfind ('\\');
	if (idx != TString::npos) {
		tmp.erase (idx + 1);
	}
	return tmp;
#elif	qPlatform_POSIX
	// could use splitpath, but this maybe better, since works with \\UNCNAMES
	TString	tmp		=	pathName;
	size_t	idx		=	tmp.rfind ('/');
	if (idx != TString::npos) {
		tmp.erase (idx + 1);
	}
	return tmp;
#else
	AssertNotImplemented ();
	return TString ();	// GRACEFULLY DEAL IWTH DIRECTION OF SLAHSES!!!
#endif
}





/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "PathName.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;





#if     qCompilerAndStdLib_constexpr_Buggy
#if     qPlatform_Windows
const   wchar_t   FileSystem::kPathComponentSeperator =   '\\';
#elif   qPlatform_POSIX
const   wchar_t   FileSystem::kPathComponentSeperator =   '/';
#endif
#endif












/*
 ********************************************************************************
 ************** FileSystem::AssureDirectoryPathSlashTerminated ******************
 ********************************************************************************
 */
String FileSystem::AssureDirectoryPathSlashTerminated (const String& dirPath)
{
    if (dirPath.empty ()) {
        AssertNotReached ();    // not sure if this is an error or not. Not sure how code used.
        // put assert in there to find out... Probably should THROW!
        //      -- LGP 2009-05-12
        return String (&kPathComponentSeperator, &kPathComponentSeperator + 1);
    }
    else {
        Character   lastChar = dirPath[dirPath.size () - 1];
        if (lastChar == kPathComponentSeperator) {
            return dirPath;
        }
        String result  =   dirPath;
        result += kPathComponentSeperator;
        return result;
    }
}







/*
 ********************************************************************************
 *********************** FileSystem::SafeFilenameChars **************************
 ********************************************************************************
 */
String FileSystem::SafeFilenameChars (const String& s)
{
    wstring tmp =   s.As<wstring> ();   // analyze as wide-char string so we don't mis-identify
    // characters (by looking at lead bytes etc)
Again:
    for (auto i = tmp.begin (); i != tmp.end (); ++i) {
        switch (*i) {
            case    '\\':
                *i = '_';
                break;
            case    '/':
                *i = '_';
                break;
            case    ':':
                *i = ' ';
                break;
            case    '.':
                tmp.erase (i);
                goto Again;
        }
    }
    return tmp;
}








/*
 ********************************************************************************
 ********************* FileSystem::AssureLongFileName ***************************
 ********************************************************************************
 */
String FileSystem::AssureLongFileName (const String& fileName)
{
#if     qPlatform_Windows
    DWORD   r   =   ::GetLongPathNameW (fileName.c_str (), nullptr, 0);
    if (r != 0) {
        Memory::SmallStackBuffer<wchar_t> buf (r);
        r = ::GetLongPathNameW (fileName.c_str (), buf, r);
        if (r != 0) {
            return static_cast<const wchar_t*> (buf);
        }
    }
#endif
    return fileName;
}





/*
 ********************************************************************************
 ************************** FileSystem::GetFileSuffix ***************************
 ********************************************************************************
 */
String FileSystem::GetFileSuffix (const String& fileName)
{
#if     qPlatform_Windows
    String useFName    =   fileName;
    {
        SDKChar   fNameBuf[4 * MAX_PATH];
        fNameBuf[0] = '\0';
        DISABLE_COMPILER_MSC_WARNING_START (4267)
        DWORD   r   =   ::GetLongPathName (fileName.AsSDKString ().c_str (), fNameBuf, NEltsOf (fNameBuf) - 1);
        DISABLE_COMPILER_MSC_WARNING_END (4267)
        if (r != 0) {
            useFName = String::FromSDKString (fNameBuf);
        }
    }
    SDKChar   fname[_MAX_FNAME] {};
    SDKChar   drive[_MAX_DRIVE] {};
    SDKChar   dir[_MAX_DIR] {};
    SDKChar   ext[_MAX_EXT] {};
    // @todo - probably should check result (errno_t) and throw?
    ::_tsplitpath_s (useFName.AsSDKString ().c_str (), drive, dir, fname, ext);
    // returns leading '.' in name...
    return String::FromSDKString (ext);
#else
    AssertNotImplemented ();
    return String ();
#endif
}




/*
 ********************************************************************************
 ************************ FileSystem::GetFileBaseName ***************************
 ********************************************************************************
 */
String FileSystem::GetFileBaseName (const String& pathName)
{
#if     qPlatform_Windows
    String useFName    =   pathName;
    {
        SDKChar   fNameBuf[4 * MAX_PATH ];
        DWORD   r   =   ::GetLongPathName (pathName.AsSDKString ().c_str (), fNameBuf, static_cast<DWORD> (NEltsOf (fNameBuf) - 1));
        if (r != 0) {
            useFName = String::FromSDKString (fNameBuf);
        }
    }
    SDKChar   fname[_MAX_FNAME] {};
    SDKChar   drive[_MAX_DRIVE] {};
    SDKChar   dir[_MAX_DIR] {};
    SDKChar   ext[_MAX_EXT] {};
    ::_tsplitpath_s (useFName.AsSDKString ().c_str (), drive, dir, fname, ext);
    return String::FromSDKString (fname);
#elif   qPlatform_POSIX
    String tmp =   pathName;
    {
        size_t i = tmp.rfind ('/');
        if (i != String::kBadIndex) {
            tmp =  tmp.substr (i + 1);
        }
    }
    {
        size_t i = tmp.find ('.');
        if (i != String::kBadIndex) {
            tmp =  tmp.substr (0, i);
        }
    }
    return tmp;
#else
    AssertNotImplemented ();
#endif
}





/*
 ********************************************************************************
 ************************ FileSystem::StripFileSuffix ***************************
 ********************************************************************************
 */
String FileSystem::StripFileSuffix (const String& pathName)
{
    String useFName    =   pathName;
    String fileSuffix  =   GetFileSuffix (pathName);
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
String FileSystem::GetFileDirectory (const String& pathName)
{
    // could use splitpath, but this maybe better, since works with \\UNCNAMES
    String  tmp     =   pathName;
    size_t  idx     =   tmp.rfind (kPathComponentSeperator);
    if (idx != String::kBadIndex) {
        tmp.erase (idx + 1);
    }
    return tmp;
}





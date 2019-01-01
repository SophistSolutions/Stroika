/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Characters/String_Constant.h"
#include "../../Memory/SmallStackBuffer.h"

#include "PathName.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

/*
 ********************************************************************************
 ************** FileSystem::AssureDirectoryPathSlashTerminated ******************
 ********************************************************************************
 */
String FileSystem::AssureDirectoryPathSlashTerminated (const String& dirPath)
{
    if (dirPath.empty ()) {
        AssertNotReached (); // not sure if this is an error or not. Not sure how code used.
        // put assert in there to find out... Probably should THROW!
        //      -- LGP 2009-05-12
        return String (&kPathComponentSeperator, &kPathComponentSeperator + 1);
    }
    else {
        Character lastChar = dirPath[dirPath.size () - 1];
        if (lastChar == kPathComponentSeperator) {
            return dirPath;
        }
        String result = dirPath;
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
    // analyze as wide-char string so we don't mis-identify characters (by looking at lead bytes etc)
    wstring tmp = s.As<wstring> ();
Again:
    for (auto i = tmp.begin (); i != tmp.end (); ++i) {
        switch (*i) {
            case '\\':
                *i = '_';
                break;
            case '/':
                *i = '_';
                break;
            case ':':
                *i = ' ';
                break;
            case '.':
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
#if qPlatform_Windows
    DWORD r = ::GetLongPathNameW (fileName.c_str (), nullptr, 0);
    if (r != 0) {
        SmallStackBuffer<wchar_t> buf (SmallStackBufferCommon::eUninitialized, r);
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
#if qPlatform_Linux
    if (auto o = fileName.RFind ('.')) {
        if (auto olastSlash = fileName.RFind (kPathComponentSeperator)) {
            if (*o < *olastSlash) {
                return String{}; // if the . comes before the last slash, its not a file suffix
            }
        }
        return fileName.SubString (*o);
    }
    return String{};
#elif qPlatform_Windows
    String useFName = fileName;
    {
        SDKChar fNameBuf[4 * MAX_PATH];
        fNameBuf[0] = '\0';
        DISABLE_COMPILER_MSC_WARNING_START (4267)
        DWORD r = ::GetLongPathName (fileName.AsSDKString ().c_str (), fNameBuf, NEltsOf (fNameBuf) - 1);
        DISABLE_COMPILER_MSC_WARNING_END (4267)
        if (r != 0) {
            useFName = String::FromSDKString (fNameBuf);
        }
    }
    SDKChar fname[_MAX_FNAME]{};
    SDKChar drive[_MAX_DRIVE]{};
    SDKChar dir[_MAX_DIR]{};
    SDKChar ext[_MAX_EXT]{};
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
    if (pathName.empty ()) {
        return String ();
    }
    String baseName = ExtractDirAndBaseName (pathName).second;
    if (not baseName.empty () and baseName.GetCharAt (baseName.size () - 1) == kPathComponentSeperator) {
        baseName = baseName.RemoveAt (baseName.size () - 1);
    }
    {
        ;
        // Strip the trailing .XXX if that isn't the entire string
        if (auto i = baseName.RFind ('.')) {
            if (*i != 0) {
                baseName = baseName.SubString (0, *i);
            }
        }
    }
    return baseName;
}

/*
 ********************************************************************************
 ******************** FileSystem::ExtractDirAndBaseName *************************
 ********************************************************************************
 */
pair<String, String> FileSystem::ExtractDirAndBaseName (const String& pathName)
{
    Require (not pathName.empty ());
    bool   endedWithSlash = false;
    String dirname;
    String basename;
    {
        String tmp = pathName;
        while (not tmp.empty () and tmp.GetCharAt (tmp.size () - 1) == kPathComponentSeperator) {
            tmp            = tmp.RemoveAt (tmp.size () - 1);
            endedWithSlash = true;
        }
        size_t i = tmp.rfind (kPathComponentSeperator);
        if (i == String::npos) {
            basename = tmp;
        }
        else {
            dirname  = tmp.SubString (0, i + 1);
            basename = tmp.SubString (i + 1);
        }
    }
    if (dirname.empty ()) {
        if (basename.empty ()) {
            dirname = AssureDirectoryPathSlashTerminated (pathName); // e.g. pathName = "/" and so we stripped trailing slashes down to nothing!
        }
        else {
            dirname = AssureDirectoryPathSlashTerminated (L".");
        }
    }
    Assert (dirname == AssureDirectoryPathSlashTerminated (dirname));
    if (endedWithSlash and not basename.empty ()) {
        basename += kPathComponentSeperator;
    }
    return pair<String, String>{dirname, basename};
}

/*
 ********************************************************************************
 ************************ FileSystem::StripFileSuffix ***************************
 ********************************************************************************
 */
String FileSystem::StripFileSuffix (const String& pathName)
{
    String useFName   = pathName;
    String fileSuffix = GetFileSuffix (pathName);
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
    if (auto idx = pathName.RFind (kPathComponentSeperator)) {
        String tmp = pathName;
        tmp.erase (*idx + 1);
        return tmp;
    }
    return pathName;
}

/*
 ********************************************************************************
 ************************* FileSystem::IsDirectoryName **************************
 ********************************************************************************
 */
bool FileSystem::IsDirectoryName (const String& pathName)
{
    Require (not pathName.empty ());
    return pathName.EndsWith (kPathComponentSeperator);
}

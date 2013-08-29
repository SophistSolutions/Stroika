/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <shlobj.h>
#include    <windows.h>
#elif   qPlatoform_POSIX
#include    <cstdlib>
#endif

#include    "../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#include    "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif

#include    "FileUtils.h"
#include    "PathName.h"

#include    "WellKnownLocations.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;

using   Characters::TChar;







/*
 ********************************************************************************
 ************ FileSystem::WellKnownLocations::GetMyDocuments ********************
 ********************************************************************************
 */
String FileSystem::WellKnownLocations::GetMyDocuments (bool createIfNotPresent)
{
#if     qPlatform_Windows
    TChar   fileBuf[MAX_PATH];
    memset (fileBuf, 0, sizeof (fileBuf));
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
    TString result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = TSTR("c:");    // shouldn't happen
    }
    // assure ends in '\'
    if (result[result.size () - 1] != '\\') {
        result += TSTR("\\");
    }
    Ensure (result[result.size () - 1] == '\\');
    Ensure (not createIfNotPresent or DirectoryExists (String::FromTString (result)));
    return String::FromTString (result);
#elif   qPlatform_POSIX
    const char* pPath = getenv ("HOME");
    if (pPath == nullptr) {
        return String ();
    }
    return String::FromTString (pPath);
#else
    AssertNotImplemented ();
    return String ();
#endif
}






/*
 ********************************************************************************
 ******** FileSystem::WellKnownLocations::GetApplicationData ********************
 ********************************************************************************
 */
String FileSystem::WellKnownLocations::GetApplicationData (bool createIfNotPresent)
{
#if     qPlatform_Windows
    TChar   fileBuf[MAX_PATH];
    memset (fileBuf, 0, sizeof (fileBuf));
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, createIfNotPresent));
    TString result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = TSTR ("c:");   // shouldn't happen
    }
    // assure ends in '\'
    if (result[result.size () - 1] != '\\') {
        result += TSTR ("\\");
    }
    Ensure (result[result.size () - 1] == '\\');
    Ensure (not createIfNotPresent or DirectoryExists (String::FromTString (result)));
    return String::FromTString (result);
#elif   qPlatform_POSIX
    return L"/var/lib/";
#else
    AssertNotImplemented ();
    return String ();
#endif
}






#if     qPlatform_Windows
/*
 ********************************************************************************
 ************** FileSystem::WellKnownLocations::GetWinSxS ***********************
 ********************************************************************************
 */
String FileSystem::WellKnownLocations::GetWinSxS ()
{
    TChar   fileBuf[MAX_PATH];
    memset (fileBuf, 0, sizeof (fileBuf));
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_WINDOWS, false));
    String result = String::FromTString (fileBuf);
    // Assure non-empty result
    if (result.empty ()) {
        return result;
    }
    result = AssureDirectoryPathSlashTerminated (result) + L"WinSxS";
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
String FileSystem::WellKnownLocations::GetTemporary ()
{
    String tempPath;
#if     qPlatform_Windows
    wchar_t   buf[1024];
    if (::GetTempPathW (NEltsOf (buf), buf) == 0) {
        tempPath = L"c:\\Temp\\";
    }
    else {
        tempPath = buf;
    }
#elif   qPlatform_POSIX
    return String (L"/tmp/");
#else
    AssertNotImplemented ();
#endif
    return AssureDirectoryPathSlashTerminated (tempPath);
}




/*
 ********************************************************************************
 ************* FileSystem::WellKnownLocations::GetTemporaryT ********************
 ********************************************************************************
 */
TString FileSystem::WellKnownLocations::GetTemporaryT ()
{
    TString tempPath;
#if     qPlatform_Windows
    TChar   buf[1024];
    if (::GetTempPath (NEltsOf (buf), buf) == 0) {
        tempPath = TSTR ("c:\\Temp\\");
    }
    else {
        tempPath = AssureDirectoryPathSlashTerminated (String::FromTString (buf)).AsTString ();
    }
#elif   qPlatform_POSIX
    return TSTR ("/tmp/");
#else
    AssertNotImplemented ();
#endif
    return tempPath;
}







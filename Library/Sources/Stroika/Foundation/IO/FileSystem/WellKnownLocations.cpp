/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <shlobj.h>
#include    <windows.h>
#elif   qPlatoform_POSIX
#include    <cstdlib>
#endif

#include    "../../Characters/String_Constant.h"
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

using   Characters::SDKChar;
using   Characters::String_Constant;







/*
 ********************************************************************************
 ************ FileSystem::WellKnownLocations::GetMyDocuments ********************
 ********************************************************************************
 */
String FileSystem::WellKnownLocations::GetMyDocuments (bool createIfNotPresent)
{
#if     qPlatform_Windows
    wchar_t   fileBuf[MAX_PATH];
    memset (fileBuf, 0, sizeof (fileBuf));
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::SHGetSpecialFolderPathW (nullptr, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
    String result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = String_Constant (L"c:");    // shouldn't happen
    }
    // assure ends in '\'
    if (result[result.size () - 1] != '\\') {
        result += String_Constant (L"\\");
    }
    Ensure (result[result.size () - 1] == '\\');
    Ensure (not createIfNotPresent or Directory (result).Exists ());
    return result;
#elif   qPlatform_POSIX
    const char* pPath = getenv ("HOME");
    if (pPath == nullptr) {
        return String ();
    }
    return String::FromSDKString (pPath);
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
    SDKChar   fileBuf[MAX_PATH];
    memset (fileBuf, 0, sizeof (fileBuf));
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, createIfNotPresent));
    SDKString result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = SDKSTR ("c:");   // shouldn't happen
    }
    // assure ends in '\'
    if (result[result.size () - 1] != '\\') {
        result += SDKSTR ("\\");
    }
    Ensure (result[result.size () - 1] == '\\');
    Ensure (not createIfNotPresent or Directory (String::FromSDKString (result)).Exists ());
    return String::FromSDKString (result);
#elif   qPlatform_POSIX
    return String_Constant (L"/var/lib/");
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
    wchar_t   fileBuf[MAX_PATH];
    memset (fileBuf, 0, sizeof (fileBuf));
    Verify (::SHGetSpecialFolderPathW (nullptr, fileBuf, CSIDL_WINDOWS, false));
    String result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        return result;
    }
    result = AssureDirectoryPathSlashTerminated (result) + String_Constant (L"WinSxS");
    result = AssureDirectoryPathSlashTerminated (result);
    if (not Directory (result).Exists ()) {
        result.clear ();
    }
    Ensure (result.empty () or Directory (result).Exists ());
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
        tempPath = String_Constant (L"c:\\Temp\\");
    }
    else {
        tempPath = buf;
    }
#elif   qPlatform_POSIX
    return String_Constant (L"/tmp/");
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
SDKString FileSystem::WellKnownLocations::GetTemporaryT ()
{
    SDKString tempPath;
#if     qPlatform_Windows
    SDKChar   buf[1024];
    if (::GetTempPath (NEltsOf (buf), buf) == 0) {
        tempPath = SDKSTR ("c:\\Temp\\");
    }
    else {
        tempPath = AssureDirectoryPathSlashTerminated (String::FromSDKString (buf)).AsSDKString ();
    }
#elif   qPlatform_POSIX
    return SDKSTR ("/tmp/");
#else
    AssertNotImplemented ();
#endif
    return tempPath;
}







/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <shlobj.h>
#include <windows.h>
#elif qPlatoform_POSIX
#include <cstdlib>
#endif

#include "../../Characters/String_Constant.h"
#include "../../Execution/Exceptions.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#include "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif

#include "FileUtils.h"
#include "PathName.h"

#include "WellKnownLocations.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

using Characters::SDKChar;
using Characters::String_Constant;

/*
 ********************************************************************************
 ************ FileSystem::WellKnownLocations::GetMyDocuments ********************
 ********************************************************************************
 */
String FileSystem::WellKnownLocations::GetMyDocuments (bool createIfNotPresent)
{
#if qPlatform_POSIX
    // @todo NYI createIfNotPresent - not sure we want/should???

    // Cacheable because the environment variables should be set externally.
    // This has the defect that it misses setenv calls, but that SB so rare,
    // and not clearly a bug we ignore subsequent changes...
    static const String kCachedResult_ = []() -> String {
        // http://pubs.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap08.html
        const char* pPath = ::getenv ("HOME");
        if (pPath != nullptr) {
            return AssureDirectoryPathSlashTerminated (String::FromSDKString (pPath));
        }
        return String{};
    }();
    return kCachedResult_;
#elif qPlatform_Windows
    // @todo DO overlaod with no args, so we can CACHE - like we do for POSIX!

    wchar_t fileBuf[MAX_PATH]{};
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::SHGetSpecialFolderPathW (nullptr, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
    String result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = String_Constant (L"c:"); // shouldn't happen
    }
    // assure ends in '\'
    if (result[result.size () - 1] != '\\') {
        result += String_Constant (L"\\");
    }
    Ensure (result[result.size () - 1] == '\\');
    Ensure (not createIfNotPresent or Directory (result).Exists ());
    return result;
#else
    AssertNotImplemented ();
    return String ();
#endif
}

/*
 ********************************************************************************
 ********* FileSystem::WellKnownLocations::GetSpoolDirectory ********************
 ********************************************************************************
 */
String FileSystem::WellKnownLocations::GetSpoolDirectory ()
{
#if qPlatform_POSIX
    static const String_Constant kVarSpool_{L"/var/spool/"};
    return kVarSpool_;
#elif qPlatform_Windows
    /// Not sure what better than FOLDERID_ProgramData / "Spool"???
    SDKChar fileBuf[MAX_PATH]{};
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, false));
    SDKString result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = SDKSTR ("c:"); // shouldn't happen
    }
    // assure ends in '\'
    if (result[result.size () - 1] != '\\') {
        result += SDKSTR ("\\");
    }
    Ensure (result[result.size () - 1] == '\\');
    if (Directory (String::FromSDKString (result)).Exists ()) {
        return String::FromSDKString (result);
    }
    else {
        return String ();
    }
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
#if qPlatform_POSIX
    // USED UNTIL STROIKA v2.0a207 - so watch out for older apps - backward compat - static String kVarLib_ = String_Constant{ L"/var/lib/" };
    static const String_Constant kVarLib_{L"/var/opt/"};
    return kVarLib_;
#elif qPlatform_Windows
    SDKChar fileBuf[MAX_PATH]{};
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, createIfNotPresent));
    SDKString result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = SDKSTR ("c:"); // shouldn't happen
    }
    // assure ends in '\'
    if (result[result.size () - 1] != '\\') {
        result += SDKSTR ("\\");
    }
    Ensure (result[result.size () - 1] == '\\');
    Ensure (not createIfNotPresent or Directory (String::FromSDKString (result)).Exists ());
    return String::FromSDKString (result);
#else
    AssertNotImplemented ();
    return String ();
#endif
}

/*
 ********************************************************************************
 ******** FileSystem::WellKnownLocations::GetRuntimeVariableData ****************
 ********************************************************************************
 */
String FileSystem::WellKnownLocations::GetRuntimeVariableData ()
{
#if qPlatform_POSIX
    static const String_Constant kResult_{L"/var/run/"};
    return kResult_;
#elif qPlatform_Windows
    return GetTemporary ();
#else
    AssertNotImplemented ();
    return String ();
#endif
}

#if qPlatform_Windows
/*
 ********************************************************************************
 ************** FileSystem::WellKnownLocations::GetWinSxS ***********************
 ********************************************************************************
 */
String FileSystem::WellKnownLocations::GetWinSxS ()
{
    wchar_t fileBuf[MAX_PATH]{};
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
namespace {
    SDKString AssureDirectoryPathSlashTerminated_ (const SDKString& dirPath)
    {
        if (dirPath.empty ()) {
            AssertNotReached (); // not sure if this is an error or not. Not sure how code used.
            // put assert in there to find out... Probably should THROW!
            //      -- LGP 2009-05-12
            SDKChar tmp = FileSystem::kPathComponentSeperator;
            return SDKString (&tmp, &tmp + 1);
        }
        else {
            SDKChar lastChar = dirPath[dirPath.size () - 1];
            if (lastChar == kPathComponentSeperator) {
                return dirPath;
            }
            SDKString result = dirPath;
            result += kPathComponentSeperator;
            return result;
        }
    }
    SDKString GetTemporary_ ()
    {
#if qPlatform_POSIX
        // http://pubs.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap08.html
        const char* pPath = ::getenv ("TMPDIR");
        if (pPath != nullptr) {
            return AssureDirectoryPathSlashTerminated_ (pPath);
        }
        return "/tmp/";
#elif qPlatform_Windows
        // NB: internally GetTempPath looks at ENV VAR TMP, then TEMP, etc...
        SDKChar buf[4 * 1024];
        if (::GetTempPath (static_cast<DWORD> (NEltsOf (buf)), buf) == 0) {
            return SDKSTR ("c:\\Temp\\");
        }
        else {
            return AssureDirectoryPathSlashTerminated_ (buf);
        }
#else
        AssertNotImplemented ();
        return SDKSTR (L"/tmp/");
#endif
    }
}

String FileSystem::WellKnownLocations::GetTemporary ()
{
    // Cacheable because the environment variables should be set externally.
    // This has the defect that it misses setenv calls, but that SB so rare,
    // and not clearly a bug we ignore subsequent changes...
    static String kCachedResult_ = String::FromSDKString (GetTemporary_ ());
    return kCachedResult_;
}

/*
 ********************************************************************************
 ************* FileSystem::WellKnownLocations::GetTemporaryT ********************
 ********************************************************************************
 */
SDKString FileSystem::WellKnownLocations::GetTemporaryT ()
{
    // Cacheable because the environment variables should be set externally.
    // This has the defect that it misses setenv calls, but that SB so rare,
    // and not clearly a bug we ignore subsequent changes...
    static SDKString kCachedResult_ = GetTemporary_ ();
    return kCachedResult_;
}

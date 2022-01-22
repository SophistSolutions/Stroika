/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <shlobj.h>
#include <windows.h>
#elif qPlatoform_POSIX
#include <cstdlib>
#endif

#include "../../Execution/Throw.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif

#include "FileUtils.h"
#include "PathName.h"

#include "WellKnownLocations.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

using Characters::SDKChar;

/*
 ********************************************************************************
 ************ FileSystem::WellKnownLocations::GetMyDocuments ********************
 ********************************************************************************
 */
filesystem::path FileSystem::WellKnownLocations::GetMyDocuments (bool createIfNotPresent)
{
#if qPlatform_POSIX
    // @todo NYI createIfNotPresent - not sure we want/should???

    // Cacheable because the environment variables should be set externally.
    // This has the defect that it misses setenv calls, but that SB so rare,
    // and not clearly a bug we ignore subsequent changes...
    static const filesystem::path kCachedResult_ = [] () -> filesystem::path {
        // http://pubs.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap08.html
        const char* pPath = ::getenv ("HOME");
        if (pPath != nullptr) {
            return pPath;
        }
        return filesystem::path{};
    }();
    return kCachedResult_;
#elif qPlatform_Windows
    // @todo DO overlaod with no args, so we can CACHE - like we do for POSIX!

    wchar_t fileBuf[MAX_PATH]{};
    // note - https://docs.microsoft.com/en-us/windows/desktop/api/shlobj_core/nf-shlobj_core-shgetspecialfolderpathw not clear this properly sets GetLastError ()
    Execution::Platform::Windows::ThrowIfZeroGetLastError (::SHGetSpecialFolderPathW (nullptr, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
    filesystem::path result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = path (L"c:"); // shouldn't happen
    }
    Ensure (not createIfNotPresent or filesystem::is_directory (result));
    return result;
#else
    AssertNotImplemented ();
    return filesystem::path ();
#endif
}

/*
 ********************************************************************************
 ********* FileSystem::WellKnownLocations::GetSpoolDirectory ********************
 ********************************************************************************
 */
filesystem::path FileSystem::WellKnownLocations::GetSpoolDirectory ()
{
#if qPlatform_POSIX
    static const filesystem::path kVarSpool_{"/var/spool/"sv};
    return kVarSpool_;
#elif qPlatform_Windows
    /// Not sure what better than FOLDERID_ProgramData / "Spool"???
    SDKChar fileBuf[MAX_PATH]{};
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, false));
    filesystem::path result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = filesystem::path ("c:"); // shouldn't happen
    }
    if (filesystem::is_directory (filesystem::path (result))) {
        return filesystem::path (result);
    }
    else {
        return filesystem::path{};
    }
#else
    AssertNotImplemented ();
    return String{};
#endif
}

/*
 ********************************************************************************
 ******** FileSystem::WellKnownLocations::GetApplicationData ********************
 ********************************************************************************
 */
filesystem::path FileSystem::WellKnownLocations::GetApplicationData (bool createIfNotPresent)
{
#if qPlatform_POSIX
    // USED UNTIL STROIKA v2.0a207 - so watch out for older apps - backward compat - static String kVarLib_ = String_Constant{ L"/var/lib/" };
    static const filesystem::path kVarLib_{"/var/opt/"};
    return kVarLib_;
#elif qPlatform_Windows
    SDKChar fileBuf[MAX_PATH]{};
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, createIfNotPresent));
    filesystem::path result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = filesystem::path ("c:"); // shouldn't happen
    }
    Ensure (not createIfNotPresent or filesystem::is_directory (result));
    return result;
#else
    AssertNotImplemented ();
    return String{};
#endif
}

/*
 ********************************************************************************
 ******** FileSystem::WellKnownLocations::GetRuntimeVariableData ****************
 ********************************************************************************
 */
filesystem::path FileSystem::WellKnownLocations::GetRuntimeVariableData ()
{
#if qPlatform_POSIX
    static const filesystem::path kResult_{L"/var/run/"};
    return kResult_;
#elif qPlatform_Windows
    return GetTemporary ();
#else
    AssertNotImplemented ();
    return String{};
#endif
}

#if qPlatform_Windows
/*
 ********************************************************************************
 ************** FileSystem::WellKnownLocations::GetWinSxS ***********************
 ********************************************************************************
 */
filesystem::path FileSystem::WellKnownLocations::GetWinSxS ()
{
    wchar_t fileBuf[MAX_PATH]{};
    Verify (::SHGetSpecialFolderPathW (nullptr, fileBuf, CSIDL_WINDOWS, false));
    filesystem::path result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        return result;
    }
    result /= "WinSxS";
    if (not filesystem::is_directory (result)) {
        result.clear ();
    }
    Ensure (result.empty () or filesystem::is_directory (result));
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
        if (::GetTempPath (static_cast<DWORD> (Memory::NEltsOf (buf)), buf) == 0) {
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

filesystem::path FileSystem::WellKnownLocations::GetTemporary ()
{
    // Cacheable because the environment variables should be set externally.
    // This has the defect that it misses setenv calls, but that SB so rare,
    // and not clearly a bug we ignore subsequent changes...
    static const filesystem::path kCachedResult_ = [] () -> filesystem::path {
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
        if (::GetTempPath (static_cast<DWORD> (Memory::NEltsOf (buf)), buf) == 0) {
            return SDKSTR ("c:\\Temp\\");
        }
        else {
            return buf;
        }
#else
        AssertNotImplemented ();
        return SDKSTR (L"/tmp/");
#endif
    }();
    return kCachedResult_;
}

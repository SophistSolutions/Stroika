/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_Windows
#include <shlobj.h>
#include <windows.h>
#elif qPlatoform_POSIX
#include <cstdlib>
#endif

#include "Stroika/Foundation/Execution/Throw.h"
#if qStroika_Foundation_Common_Platform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif

//#include "Stroika/Foundation/Execution/IO/FileSystem/FileUtils.h"

#include "WellKnownLocations.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

using Characters::SDKChar;
using Characters::SDKString;

/*
 ********************************************************************************
 ************ FileSystem::WellKnownLocations::GetMyDocuments ********************
 ********************************************************************************
 */
filesystem::path FileSystem::WellKnownLocations::GetMyDocuments (bool createIfNotPresent)
{
#if qStroika_Foundation_Common_Platform_POSIX
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
#elif qStroika_Foundation_Common_Platform_Windows
    // @todo DO overlaod with no args, so we can CACHE - like we do for POSIX!

    wchar_t fileBuf[MAX_PATH] qStroika_Foundation_INDETERMINATE; // SHGetSpecialFolderPathW fills in with OUT parameter
    // note - https://docs.microsoft.com/en-us/windows/desktop/api/shlobj_core/nf-shlobj_core-shgetspecialfolderpathw not clear this properly sets GetLastError ()
    Execution::Platform::Windows::ThrowIfZeroGetLastError (::SHGetSpecialFolderPathW (nullptr, fileBuf, CSIDL_PERSONAL, createIfNotPresent));
    filesystem::path result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = filesystem::path{"c:"sv}; // shouldn't happen
    }
    Ensure (not createIfNotPresent or filesystem::is_directory (result));
    return result;
#else
    AssertNotImplemented ();
    return filesystem::path{};
#endif
}

/*
 ********************************************************************************
 ********* FileSystem::WellKnownLocations::GetSpoolDirectory ********************
 ********************************************************************************
 */
filesystem::path FileSystem::WellKnownLocations::GetSpoolDirectory ()
{
#if qStroika_Foundation_Common_Platform_POSIX
    static const filesystem::path kVarSpool_{"/var/spool/"sv};
    return kVarSpool_;
#elif qStroika_Foundation_Common_Platform_Windows
    /// Not sure what better than FOLDERID_ProgramData / "Spool"???
    wchar_t fileBuf[MAX_PATH] qStroika_Foundation_INDETERMINATE; // SHGetSpecialFolderPathW fills in with OUT parameter
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, false));
    filesystem::path result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = filesystem::path{"c:"sv}; // shouldn't happen
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
#if qStroika_Foundation_Common_Platform_POSIX
    // USED UNTIL STROIKA v2.0a207 - so watch out for older apps - backward compat - static String kVarLib_ = String_Constant{ L"/var/lib/" };
    static const filesystem::path kVarLib_{"/var/opt/"sv};
    return kVarLib_;
#elif qStroika_Foundation_Common_Platform_Windows
    wchar_t fileBuf[MAX_PATH] qStroika_Foundation_INDETERMINATE; // SHGetSpecialFolderPathW fills in with OUT parameter
    Verify (::SHGetSpecialFolderPath (nullptr, fileBuf, CSIDL_COMMON_APPDATA, createIfNotPresent));
    filesystem::path result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        result = filesystem::path{"c:"sv}; // shouldn't happen
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
#if qStroika_Foundation_Common_Platform_POSIX
    static const filesystem::path kResult_{"/var/run/"sv};
    return kResult_;
#elif qStroika_Foundation_Common_Platform_Windows
    return GetTemporary ();
#else
    AssertNotImplemented ();
    return String{};
#endif
}

#if qStroika_Foundation_Common_Platform_Windows
/*
 ********************************************************************************
 ************** FileSystem::WellKnownLocations::GetWinSxS ***********************
 ********************************************************************************
 */
filesystem::path FileSystem::WellKnownLocations::GetWinSxS ()
{
    wchar_t fileBuf[MAX_PATH] qStroika_Foundation_INDETERMINATE; // SHGetSpecialFolderPathW fills in with OUT parameter
    Verify (::SHGetSpecialFolderPathW (nullptr, fileBuf, CSIDL_WINDOWS, false));
    filesystem::path result = fileBuf;
    // Assure non-empty result
    if (result.empty ()) {
        return result;
    }
    result /= "WinSxS"sv;
    if (not filesystem::is_directory (result)) {
        result.clear ();
    }
    Ensure (result.empty () or filesystem::is_directory (result));
    return result;
}
#endif

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <windows.h>

#include <shellapi.h>
#include <shlobj.h>
#elif qPlatform_POSIX
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#if qPlatform_Linux
#include <linux/limits.h>
#endif
#include "../../Characters/Format.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"
#include "../../Containers/Collection.h"
#include "../../Containers/Set.h"
#include "../../Debug/Trace.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#include "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../../Execution/Exceptions.h"
#include "../../Execution/Finally.h"

#include "../../IO/FileSystem/Exception.h"

#include "DirectoryIterable.h"
#include "FileUtils.h"
#include "PathName.h"

#include "FileSystem.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 ********************************** FileSystem::Ptr *****************************
 ********************************************************************************
 */
bool IO::FileSystem::Ptr::Access (const filesystem::path& fileFullPath, AccessMode accessMode) const noexcept
{
// @todo FIX to only do ONE system call, not two!!!
#if qPlatform_Windows
    if ((accessMode & AccessMode::eRead) == AccessMode::eRead) {
        DWORD attribs = ::GetFileAttributesW (fileFullPath.c_str ());
        if (attribs == INVALID_FILE_ATTRIBUTES) {
            return false;
        }
    }
    if ((accessMode & AccessMode::eWrite) == AccessMode::eWrite) {
        DWORD attribs = ::GetFileAttributesW (fileFullPath.c_str ());
        if ((attribs == INVALID_FILE_ATTRIBUTES) or (attribs & FILE_ATTRIBUTE_READONLY)) {
            return false;
        }
    }
    return true;
#elif qPlatform_POSIX
    // Not REALLY right - but an OK hack for now... -- LGP 2011-09-26
    //http://linux.die.net/man/2/access
    if ((accessMode & AccessMode::eRead) == AccessMode::eRead) {
        if (access (fileFullPath.c_str (), R_OK) != 0) {
            return false;
        }
    }
    if ((accessMode & AccessMode::eWrite) == AccessMode::eWrite) {
        if (access (fileFullPath.c_str (), W_OK) != 0) {
            return false;
        }
    }
    return true;
#else
    AssertNotImplemented ();
    return false;
#endif
}

void IO::FileSystem::Ptr::CheckAccess (const filesystem::path& fileFullPath, AccessMode accessMode)
{
    // quick hack - not fully implemented - but since advsiory only - not too important...
    if (not Access (fileFullPath, accessMode)) {
        // @todo take into account 'accessMode' in the resulting message
        Execution::Throw (IO::FileSystem::Exception{make_error_code (errc::permission_denied), fileFullPath});
    }
}

void IO::FileSystem::Ptr::CheckAccess (const filesystem::path& fileFullPath, bool checkCanRead, bool checkCanWrite)
{
    if (checkCanRead and checkCanWrite) {
        CheckAccess (fileFullPath, IO::AccessMode::eReadWrite);
    }
    else if (checkCanRead) {
        CheckAccess (fileFullPath, IO::AccessMode::eRead);
    }
    else if (checkCanWrite) {
        CheckAccess (fileFullPath, IO::AccessMode::eWrite);
    }
}

optional<filesystem::path> IO::FileSystem::Ptr::FindExecutableInPath (const filesystem::path& /*filename*/) const
{
    // @TODO A
    //   const char* pPath = ::getenv ("PATH");
    // @todo windows https://msdn.microsoft.com/en-us/library/windows/desktop/aa365527(v=vs.85).aspx
    // on unix - get env path and just prepend and run access (EEXEC PERM)
    AssertNotImplemented ();
    return nullopt;
}

DateTime IO::FileSystem::Ptr::GetFileLastModificationDate (const filesystem::path& fileName)
{
#if qPlatform_POSIX
    struct stat s {
    };
    FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::stat (fileName.generic_string ().c_str (), &s), fileName);
    return DateTime{s.st_mtime};
#elif qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
    FileSystem::Exception::ThrowIfZeroGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData), fileName);
    return DateTime{fileAttrData.ftLastWriteTime};
#else
    AssertNotImplemented ();
    return DateTime{};
#endif
}

DateTime IO::FileSystem::Ptr::GetFileLastAccessDate (const filesystem::path& fileName)
{
#if qPlatform_POSIX
    struct stat s {
    };
    FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::stat (fileName.generic_string ().c_str (), &s), fileName);
    return DateTime{s.st_atime};
#elif qPlatform_Windows
    WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
    FileSystem::Exception::ThrowIfZeroGetLastError (::GetFileAttributesExW (fileName.c_str (), GetFileExInfoStandard, &fileAttrData), fileName);
    return DateTime{fileAttrData.ftLastAccessTime};
#else
    AssertNotImplemented ();
    return DateTime{};
#endif
}

/*
 ********************************************************************************
 **************************** FileSystem::Default *******************************
 ********************************************************************************
 */
IO::FileSystem::Ptr IO::FileSystem::Default ()
{
    static IO::FileSystem::Ptr sThe_;
    return sThe_;
}

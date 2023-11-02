/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <limits>

#if 1
#include <sys/stat.h>
#include <sys/types.h>

#if qPlatform_Windows
#include <aclapi.h>
#include <io.h>
#include <shlobj.h>
#include <windows.h>
#elif qPlatform_POSIX
#include <unistd.h>
#endif
#endif

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Execution/Common.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Module.h"
#include "../../Execution/Process.h"
#if qPlatform_Windows && 0
#include "../../Execution/Platform/Windows/Exception.h"
#include "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../../Containers/Common.h"
#include "../../Debug/Trace.h"
#include "../../IO/FileSystem/FileSystem.h"

#include "FileUtils.h"
#include "PathName.h"
#include "WellKnownLocations.h"

#include "TemporaryFile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Memory;

#if qPlatform_Windows && 0
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

/*
 ********************************************************************************
 *********************** FileSystem::AppTempFileManager *************************
 ********************************************************************************
 */
AppTempFileManager::AppTempFileManager ()
{
    filesystem::path tmpDir = WellKnownLocations::GetTemporary ();

    filesystem::path cleanedExePath = Execution::GetEXEPath ();

    filesystem::path exeFileName = cleanedExePath.filename ();
    exeFileName.replace_extension (""); // strip trailing .EXE
    // no biggie, but avoid spaces in tmpfile path name (but don't try too hard, should be
    // harmless)
    //  -- LGP 2009-08-16    // replace any spaces in name with -
    {
        auto u = exeFileName.u32string ();
        for (auto i = u.begin (); i != u.end (); ++i) {
            if (*i == ' ') {
                *i = '-';
            }
        }
        exeFileName = u;
    }

    // Need to include more than just the process-id, since code linked against this lib could be
    // loaded as a DLL into this process, and it would
    // use the same dir, and then delete it when that DLL exits (such as the rebranding DLL).
    //
    // But whatever we do, the DLL may do also, so we must use what is in the filesystem
    // to disambiguiate.
    //
    tmpDir /= "HealthFrameWorks"sv;     // @todo FIX THIS NAME - HISTORICAL...
    create_directories (tmpDir);
    for (int i = 0; i < INT_MAX; ++i) {
        filesystem::path trialD =
            tmpDir / ToPath (Format (L"%s-%d-%d", FromPath (exeFileName).c_str (), Execution::GetCurrentProcessID (), i + rand ()));

        if (create_directory (trialD) == false) {
            continue; // try again - DIRECTORY EXISTS
        }
        // we succeeded - good! Done... (create_directory throws if cannot create directory)
        tmpDir = trialD;
        break;
    }
    fTmpDir_ = tmpDir;
    DbgTrace (L"AppTempFileManager::CTOR: created '%s'", Characters::ToString (fTmpDir_).c_str ());
}

AppTempFileManager::~AppTempFileManager ()
{
    DbgTrace (L"AppTempFileManager::DTOR: clearing '%s'", Characters::ToString (fTmpDir_).c_str ());
    try {
        remove_all (fTmpDir_);
    }
    catch (...) {
        DbgTrace ("Ingoring exception clearly AppTempFileManager files: %s", Characters::ToString (current_exception ()).c_str ());
    }
}

filesystem::path AppTempFileManager::GetTempFile (const filesystem::path& fileBaseName)
{
    filesystem::path fn = AppTempFileManager::Get ().GetMasterTempDir ();
    create_directories (fn);
    String basename = FromPath (fileBaseName.stem ());
    String ext      = FromPath (fileBaseName.extension ());
    if (ext.empty ()) {
        ext = "txt"sv;
    }
    for (int attempts = 0; attempts < 5; ++attempts) {
        char    buf[1024];
        (void)snprintf (buf, NEltsOf (buf), "-%d", ::rand ());
        filesystem::path trialName = fn / ToPath (basename + buf + ext);
        if (not exists (trialName)) {
#if qPlatform_POSIX
            int fd = ::_open (trialName.generic_string ().c_str (), O_RDWR | O_CREAT);
#elif qPlatform_Windows
            int     fd;
            [[maybe_unused]] errno_t e  = ::_sopen_s (&fd, trialName.generic_string ().c_str (), (O_RDWR | O_CREAT), _SH_DENYNO, 0);
#endif
            if (fd >= 0) {
#if qPlatform_POSIX
                close (fd);
#elif qPlatform_Windows
                _close (fd);
#endif
                DbgTrace (L"AppTempFileManager::GetTempFile (): returning '%s'", Characters::ToString (trialName).c_str ());
                return trialName; 
            }
        }
        DbgTrace (L"Attempt to create file collided, so retrying (%d)", Characters::ToString (trialName).c_str (), attempts);
    }
    Execution::Throw (Exception{"Unknown error creating file"sv}, "AppTempFileManager::GetTempFile (): failed to create tempfile");
}

filesystem::path AppTempFileManager::GetTempDir (const String& dirNameBase)
{
    filesystem::path fn = AppTempFileManager::Get ().GetMasterTempDir ();
    create_directories (fn);
    for (int attempts = 0; attempts < 5; ++attempts) {
        char buf[1024];
        (void)snprintf (buf, NEltsOf (buf), "-%d", ::rand ());
        filesystem::path trialName = fn / ToPath (dirNameBase + buf);
        if (not is_directory (trialName)) {
            if (create_directories (trialName)) {
                DbgTrace (L"AppTempFileManager::GetTempDir (): returning '%s'", Characters::ToString (trialName).c_str ());
                return trialName;
            }
        }
        DbgTrace (L"Attempt to create directory collided, so retrying (%d)", Characters::ToString (trialName).c_str (), attempts);
    }
    Execution::Throw (Exception{"Unknown error creating temporary directory"sv}, "AppTempFileManager::GetTempDir (): failed to create tempdir");
}


/*
 ********************************************************************************
 **************************** FileSystem::ScopedTmpDir **************************
 ********************************************************************************
 */
ScopedTmpDir::ScopedTmpDir (const String& fileNameBase)
    : fTmpDir_{AppTempFileManager::Get ().GetTempDir (fileNameBase)}
{
}

ScopedTmpDir::~ScopedTmpDir ()
{
    try {
        DbgTrace (L"ScopedTmpDir::~ScopedTmpDir - removing contents for '%s'", Characters::ToString (fTmpDir_).c_str ());
        remove_all (fTmpDir_);
    }
    catch (...) {
        DbgTrace ("Ingoring exception clearing files in ScopedTmpDir DTOR: %s", Characters::ToString (current_exception ()).c_str ());
    }
}

/*
 ********************************************************************************
 *********************** FileSystem::ScopedTmpFile ******************************
 ********************************************************************************
 */
ScopedTmpFile::ScopedTmpFile (const filesystem::path& fileBaseName)
    : fTmpFile_{AppTempFileManager::Get ().GetTempFile (fileBaseName)}
{
}

ScopedTmpFile::~ScopedTmpFile ()
{
    try {
        DbgTrace (L"ScopedTmpFile::~ScopedTmpFile - removing '%s'", Characters::ToString (fTmpFile_).c_str ());
        remove (fTmpFile_);
    }
    catch (...) {
        DbgTrace ("Ingoring exception clearing files in ScopedTmpFile DTOR: %s", Characters::ToString (current_exception ()).c_str ());
    }
}

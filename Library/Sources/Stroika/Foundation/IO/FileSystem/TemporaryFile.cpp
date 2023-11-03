/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if qPlatform_Windows
#include <io.h>
#include <windows.h>
#elif qPlatform_POSIX
#include <unistd.h>
#endif

#include "../../Characters/Format.h"
#include "../../Debug/Trace.h"
#include "../../Execution/Common.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Module.h"
#include "../../Execution/Process.h"
#include "../../IO/FileSystem/FileSystem.h"

#include "PathName.h"
#include "WellKnownLocations.h"

#include "TemporaryFile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Memory;

/*
 ********************************************************************************
 *********************** FileSystem::AppTempFileManager *************************
 ********************************************************************************
 */
namespace {
    filesystem::path GetSysTmpRelativePath_ (const AppTempFileManager::Options& o)
    {
        return o.fRelativePathInsideTmpDir.value_or (GetEXEPath ().stem ()); // @todo optimize this use of value_or cuz evaluates args always (sad)
    }
}

AppTempFileManager::AppTempFileManager (const Options& options)
{
    filesystem::path tmpDir = WellKnownLocations::GetTemporary ();

    filesystem::path cleanedExePath = Execution::GetEXEPath ();

    filesystem::path exeFileName = cleanedExePath.stem ();
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
    tmpDir /= GetSysTmpRelativePath_ (options);
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
    if (not fTmpDir_.empty ()) {
        DbgTrace (L"AppTempFileManager::DTOR: clearing '%s'", Characters::ToString (fTmpDir_).c_str ());
        try {
            remove_all (fTmpDir_);
        }
        catch (...) {
            DbgTrace ("Ingoring exception clearly AppTempFileManager files: %s", Characters::ToString (current_exception ()).c_str ());
        }
    }
}

AppTempFileManager& AppTempFileManager::operator= (AppTempFileManager&& rhs)
{
    DbgTrace (L"AppTempFileManager::DTOR: clearing '%s'", Characters::ToString (fTmpDir_).c_str ());
    remove_all (fTmpDir_);
    fTmpDir_ = move (rhs.fTmpDir_); // prevents rhs DTOR from doing anything
    Assert (rhs.fTmpDir_.empty ()); // cuz of this...
    return *this;
}

filesystem::path AppTempFileManager::GetTempFile (const filesystem::path& fileBaseName)
{
    filesystem::path fn = GetRootTempDir ();
    create_directories (fn);
    String basename = FromPath (fileBaseName.stem ());
    String ext      = FromPath (fileBaseName.extension ());
    if (ext.empty ()) {
        ext = "txt"sv;
    }
    for (int attempts = 0; attempts < 5; ++attempts) {
        char buf[1024];
        (void)snprintf (buf, NEltsOf (buf), "-%d", ::rand ());
        filesystem::path trialName = fn / ToPath (basename + buf + ext);
        if (not exists (trialName)) {
#if qPlatform_POSIX
            constexpr mode_t kCreateMode_  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
            int fd = ::open (trialName.generic_string ().c_str (), O_RDWR | O_CREAT, kCreateMode_);
#elif qPlatform_Windows
            int                      fd;
            [[maybe_unused]] errno_t e = ::_sopen_s (&fd, trialName.generic_string ().c_str (), (O_RDWR | O_CREAT), _SH_DENYNO, 0);
#endif
            if (fd >= 0) {
#if qPlatform_POSIX
                close (fd);
#elif qPlatform_Windows
                _close (fd);
#endif
                DbgTrace (L"AppTempFileManager::GetTempFile (): returning '%s'", Characters::ToString (trialName).c_str ());
                WeakAssert (is_regular_file (trialName)); // possible for someone to have manually deleted, but unlikely
                return trialName;
            }
        }
        DbgTrace (L"Attempt to create file collided, so retrying (%d)", Characters::ToString (trialName).c_str (), attempts);
    }
    Execution::Throw (Exception{"Unknown error creating file"sv}, "AppTempFileManager::GetTempFile (): failed to create tempfile");
}

filesystem::path AppTempFileManager::GetTempDir (const String& dirNameBase)
{
    filesystem::path fn = GetRootTempDir ();
    create_directories (fn);
    for (int attempts = 0; attempts < 5; ++attempts) {
        char buf[1024];
        (void)snprintf (buf, NEltsOf (buf), "-%d", ::rand ());
        filesystem::path trialName = fn / ToPath (dirNameBase + buf);
        if (not is_directory (trialName)) {
            if (create_directories (trialName)) {
                DbgTrace (L"AppTempFileManager::GetTempDir (): returning '%s'", Characters::ToString (trialName).c_str ());
                WeakAssert (is_directory (trialName)); // possible for someone to have manually deleted, but unlikely
                return trialName;
            }
        }
        DbgTrace (L"Attempt to create directory collided, so retrying (%d)", Characters::ToString (trialName).c_str (), attempts);
    }
    Execution::Throw (Exception{"Unknown error creating temporary directory"sv},
                      "AppTempFileManager::GetTempDir (): failed to create tempdir");
}

/*
 ********************************************************************************
 **************************** FileSystem::ScopedTmpDir **************************
 ********************************************************************************
 */
ScopedTmpDir::ScopedTmpDir (const String& fileNameBase)
    : fTmpDir_{AppTempFileManager::sThe.GetTempDir (fileNameBase)}
{
}

ScopedTmpDir::~ScopedTmpDir ()
{
    Debug::TraceContextBumper ctx{L"ScopedTmpDir::~ScopedTmpDir", L"readfilename=%s", Characters::ToString (fTmpDir_).c_str ()};
    try {
        remove_all (fTmpDir_);
    }
    catch (...) {
        DbgTrace ("Ingoring exception DTOR: %s", Characters::ToString (current_exception ()).c_str ());
    }
}

/*
 ********************************************************************************
 *********************** FileSystem::ScopedTmpFile ******************************
 ********************************************************************************
 */
ScopedTmpFile::ScopedTmpFile (const filesystem::path& fileBaseName)
    : fTmpFile_{AppTempFileManager::sThe.GetTempFile (fileBaseName)}
{
}

ScopedTmpFile::~ScopedTmpFile ()
{
    try {
        remove (fTmpFile_);
    }
    catch (...) {
        DbgTrace ("Ingoring exception clearing file in ScopedTmpFile::~ScopedTmpFile: %s", Characters::ToString (current_exception ()).c_str ());
    }
}

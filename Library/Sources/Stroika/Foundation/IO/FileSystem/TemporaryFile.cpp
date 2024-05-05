/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if qPlatform_Windows
#include <io.h>
#include <windows.h>
#elif qPlatform_POSIX
#include <unistd.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/Process.h"
#include "Stroika/Foundation/IO/FileSystem/FileSystem.h"

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
 ************************ FileSystem::AppTmpFileManager *************************
 ********************************************************************************
 */
namespace {
    filesystem::path GetSysTmpRelativePath_ (const AppTmpFileManager::Options& o)
    {
        return o.fRelativePathInsideTmpDir.value_or (GetEXEPath ().stem ()); // @todo optimize this use of value_or cuz evaluates args always (sad)
    }
}

AppTmpFileManager::AppTmpFileManager (const Options& options)
{
    Debug::TraceContextBumper ctx{"AppTmpFileManager::CTOR"};
    filesystem::path          tmpDir         = WellKnownLocations::GetTemporary ();
    filesystem::path          cleanedExePath = Execution::GetEXEPath ();
    filesystem::path          exeFileName    = cleanedExePath.stem ();
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
    DbgTrace ("tmpDir={} (assuring created now...)"_f, tmpDir);
    try {
        create_directories (tmpDir);
    }
    catch (...) {
        DbgTrace (L"Error creating tmpdirs, so adjusting and retrying : {}"_f, current_exception ());
        // tmpDir == GetEXEPath (): happens in regtests - maybe better way to handle -
        tmpDir.replace_filename (GetEXEPath ().stem ().generic_string () + "-tmpdir");
        create_directories (tmpDir); // if that doesn't do it, just throw
    }
    for (int i = 0; i < INT_MAX; ++i) {
        filesystem::path trialD = tmpDir / ToPath (Format ("{}-{}-{}"_f, exeFileName, Execution::GetCurrentProcessID (), i + rand ()));

        if (create_directory (trialD) == false) {
            continue; // try again - DIRECTORY EXISTS
        }
        // we succeeded - good! Done... (create_directory throws if cannot create directory)
        tmpDir = trialD;
        break;
    }
    fTmpDir_ = tmpDir;
}

AppTmpFileManager::~AppTmpFileManager ()
{
    if (not fTmpDir_.empty ()) {
        DbgTrace (L"AppTmpFileManager::DTOR: clearing {}"_f, fTmpDir_);
        try {
            remove_all (fTmpDir_);
        }
        catch (...) {
            DbgTrace ("Ignoring exception clearly AppTmpFileManager files: {}"_f, current_exception ());
        }
    }
}

AppTmpFileManager& AppTmpFileManager::operator= (AppTmpFileManager&& rhs) noexcept
{
    DbgTrace ("AppTmpFileManager::DTOR: clearing {}"_f, fTmpDir_);
    remove_all (fTmpDir_);
    fTmpDir_ = move (rhs.fTmpDir_); // prevents rhs DTOR from doing anything
    Assert (rhs.fTmpDir_.empty ()); // cuz of this...
    return *this;
}

filesystem::path AppTmpFileManager::GetTmpFile (const String& fileBaseName)
{
    filesystem::path fn = GetRootTmpDir ();
    create_directories (fn);
    return CreateTmpFile (fileBaseName, fn);
}

filesystem::path AppTmpFileManager::GetTmpDir (const String& dirNameBase)
{
    filesystem::path fn = GetRootTmpDir ();
    create_directories (fn);
    for (int attempts = 0; attempts < 5; ++attempts) {
        char buf[1024];
        (void)snprintf (buf, NEltsOf (buf), "-%d", ::rand ());
        filesystem::path trialName = fn / ToPath (dirNameBase + buf);
        if (not is_directory (trialName)) {
            if (create_directories (trialName)) {
                DbgTrace ("AppTmpFileManager::GetTempDir (): returning '{}'"_f, trialName);
                WeakAssert (is_directory (trialName)); // possible for someone to have manually deleted, but unlikely
                return trialName;
            }
        }
        DbgTrace ("Attempt to create directory collided, so retrying ({})"_f, trialName, attempts);
    }
    Execution::Throw (Exception{"Unknown error creating temporary directory"sv},
                      "AppTmpFileManager::GetTmpDir (): failed to create tmpdir");
}

/*
 ********************************************************************************
 **************************** FileSystem::ScopedTmpDir **************************
 ********************************************************************************
 */
ScopedTmpDir::ScopedTmpDir (const String& fileNameBase)
    : fTmpDir_{AppTmpFileManager::sThe.GetTmpDir (fileNameBase)}
{
}

ScopedTmpDir::~ScopedTmpDir ()
{
    Debug::TraceContextBumper ctx{"ScopedTmpDir::~ScopedTmpDir", "readfilename={}"_f, fTmpDir_};
    try {
        remove_all (fTmpDir_);
    }
    catch (...) {
        DbgTrace ("Ignoring exception DTOR: {}"_f, current_exception ());
    }
}

/*
 ********************************************************************************
 *********************** FileSystem::ScopedTmpFile ******************************
 ********************************************************************************
 */
ScopedTmpFile::ScopedTmpFile (const String& fileBaseName)
    : fTmpFile_{AppTmpFileManager::sThe.GetTmpFile (fileBaseName)}
{
}

ScopedTmpFile::~ScopedTmpFile ()
{
    try {
        remove (fTmpFile_);
    }
    catch (...) {
        DbgTrace ("Ignoring exception clearing file in ScopedTmpFile::~ScopedTmpFile: {}"_f, current_exception ());
    }
}

/*
 ********************************************************************************
 ************************ FileSystem::CreateTmpFile *****************************
 ********************************************************************************
 */
filesystem::path FileSystem::CreateTmpFile (const String& baseName)
{
    return CreateTmpFile (baseName, WellKnownLocations::GetTemporary ());
}

filesystem::path FileSystem::CreateTmpFile (const String& baseName, const filesystem::path& inFolder)
{
    filesystem::path baseNamePath = ToPath (baseName);
    Require (not baseNamePath.has_root_path ());
    String basename = FromPath (baseNamePath.stem ());
    String ext      = FromPath (baseNamePath.extension ());
    if (ext.empty ()) {
        ext = "txt"sv;
    }
    constexpr int kMaxAttempts_{100};
    for (int attempts = 0; attempts < kMaxAttempts_; ++attempts) {
        char buf[1024];
        (void)snprintf (buf, NEltsOf (buf), "-%d", ::rand ());
        filesystem::path trialName = inFolder / ToPath (basename + buf + ext);
        if (not exists (trialName)) {
#if qPlatform_Windows
            if (HANDLE fd = ::CreateFile (trialName.native ().c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                          nullptr, CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY, nullptr);
                fd != INVALID_HANDLE_VALUE) {
                ::CloseHandle (fd);
                DbgTrace ("AppTmpFileManager::GetTmpFile (): returning {}"_f, trialName);
                WeakAssert (is_regular_file (trialName)); // possible for someone to have manually deleted, but unlikely
                return trialName;
            }
#else
            if (int fd = ::open (trialName.generic_string ().c_str (), O_RDWR | O_CREAT, filesystem::perms::all); fd >= 0) {
                close (fd);
                DbgTrace ("AppTmpFileManager::GetTmpFile (): returning {}"_f, trialName);
                WeakAssert (is_regular_file (trialName)); // possible for someone to have manually deleted, but unlikely
                return trialName;
            }
#endif
        }
        DbgTrace ("Attempt to create file ({}) collided, so retrying ({} attempts)"_f, trialName, attempts);
    }
    Execution::Throw (Exception{"Unknown error creating temporary file"sv});
}

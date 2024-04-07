/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
    Debug::TraceContextBumper ctx{"AppTempFileManager::CTOR"};
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
}

AppTempFileManager::~AppTempFileManager ()
{
    if (not fTmpDir_.empty ()) {
        DbgTrace (L"AppTempFileManager::DTOR: clearing {}"_f, fTmpDir_);
        try {
            remove_all (fTmpDir_);
        }
        catch (...) {
            DbgTrace ("Ignoring exception clearly AppTempFileManager files: {}"_f, current_exception ());
        }
    }
}

AppTempFileManager& AppTempFileManager::operator= (AppTempFileManager&& rhs)
{
    DbgTrace ("AppTempFileManager::DTOR: clearing {}"_f, fTmpDir_);
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
#if qPlatform_Windows
            if (HANDLE fd = ::CreateFile (trialName.native ().c_str (), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                          nullptr, CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY, nullptr);
                fd != INVALID_HANDLE_VALUE) {
                ::CloseHandle (fd);
                DbgTrace ("AppTempFileManager::GetTempFile (): returning {}"_f, Characters::ToString (trialName));
                WeakAssert (is_regular_file (trialName)); // possible for someone to have manually deleted, but unlikely
                return trialName;
            }
#else
            if (int fd = ::open (trialName.generic_string ().c_str (), O_RDWR | O_CREAT, filesystem::perms::all); fd >= 0) {
                close (fd);
                DbgTrace ("AppTempFileManager::GetTempFile (): returning {}"_f, trialName);
                WeakAssert (is_regular_file (trialName)); // possible for someone to have manually deleted, but unlikely
                return trialName;
            }
#endif
        }
        DbgTrace ("Attempt to create file ({}) collided, so retrying ({} attempts)"_f, Characters::ToString (trialName), attempts);
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
                DbgTrace ("AppTempFileManager::GetTempDir (): returning '{}'"_f, trialName);
                WeakAssert (is_directory (trialName)); // possible for someone to have manually deleted, but unlikely
                return trialName;
            }
        }
        DbgTrace ("Attempt to create directory collided, so retrying ({})"_f, Characters::ToString (trialName), attempts);
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
    Debug::TraceContextBumper ctx{"ScopedTmpDir::~ScopedTmpDir", "readfilename={}"_f, Characters::ToString (fTmpDir_)};
    try {
        remove_all (fTmpDir_);
    }
    catch (...) {
        DbgTrace ("Ignoring exception DTOR: {}"_f, Characters::ToString (current_exception ()));
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
        DbgTrace ("Ignoring exception clearing file in ScopedTmpFile::~ScopedTmpFile: {}"_f, current_exception ());
    }
}

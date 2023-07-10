/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <limits>
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

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"
#include "../../Execution/Common.h"
#include "../../Execution/Exceptions.h"
#if qPlatform_Windows
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

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

/*
 ********************************************************************************
 *********************** FileSystem::AppTempFileManager *************************
 ********************************************************************************
 */
AppTempFileManager::AppTempFileManager ()
    : fTmpDir ()
{
#if qPlatform_Windows
    String tmpDir = WellKnownLocations::GetTemporary ();

    wchar_t exePath[MAX_PATH]{};
    Verify (::GetModuleFileNameW (nullptr, exePath, static_cast<DWORD> (NEltsOf (exePath))));

    wstring exeFileName = exePath;
    {
        size_t i = exeFileName.rfind ('\\');
        if (i != wstring::npos) {
            exeFileName = exeFileName.substr (i + 1);
        }
        // strip trailing .EXE
        i = exeFileName.rfind ('.');
        if (i != SDKString::npos) {
            exeFileName = exeFileName.erase (i);
        }
        // no biggie, but avoid spaces in tmpfile path name (but don't try too hard, should be
        // harmless)
        //  -- LGP 2009-08-16
        for (auto p = exeFileName.begin (); p != exeFileName.end (); ++p) {
            if (*p == ' ') {
                *p = '-';
            }
        }
    }
    // Need to include more than just the process-id, since code linked against this lib could be
    // loaded as a DLL into this process, and it would
    // use the same dir, and then delete it when that DLL exits (such as the rebranding DLL).
    //
    // But whatever we do, the DLL may do also, so we must use what is in the filesystem
    // to disambiguiate.
    //
    tmpDir += L"HealthFrameWorks\\"sv;
    CreateDirectory (tmpDir);
    for (int i = 0; i < INT_MAX; ++i) {
        String d = tmpDir + Format (L"%s-%d-%d\\", exeFileName.c_str (), ::GetCurrentProcessId (), i + rand ());
        if (not ::CreateDirectoryW (d.c_str (), nullptr)) {
            DWORD error = ::GetLastError ();
            if (error == ERROR_ALREADY_EXISTS) {
                continue; // try again
            }
            else {
                DbgTrace ("bad news if we cannot create AppTempFileManager::fTmpDir: %d", error);
                Execution::ThrowSystemErrNo (error);
            }
        }
        // we succeeded - good! Done...
        tmpDir = d;
        break;
    }
    fTmpDir = tmpDir;
    DbgTrace (L"AppTempFileManager::CTOR: created '%s'", fTmpDir.c_str ());
#else
//AssertNotImplemented ();
#endif
}

AppTempFileManager::~AppTempFileManager ()
{
    DbgTrace (L"AppTempFileManager::DTOR: clearing '%s'", fTmpDir.c_str ());
    try {
        IO::FileSystem::Default ().RemoveDirectory (fTmpDir, RemoveDirectoryPolicy::eRemoveAnyContainedFiles);
    }
    catch (...) {
        WeakAssert (false); // not reached
    }
}

String AppTempFileManager::GetTempFile (const String& fileNameBase)
{
#if qPlatform_Windows
    String fn = AppTempFileManager::Get ().GetMasterTempDir () + fileNameBase;
    IO::FileSystem::CreateDirectoryForFile (fn);

    SDKString::size_type suffixStart = fn.rfind ('.');
    if (suffixStart == SDKString::npos) {
        fn += L".txt"sv;
        suffixStart = fn.rfind ('.');
    }
    int attempts = 0;
    while (attempts < 5) {
        wstring s = fn.As<wstring> ();
        char    buf[100];
        (void)snprintf (buf, NEltsOf (buf), "%d", ::rand ());
        s.insert (suffixStart, String::FromNarrowSDKString (buf).As<wstring> ());
        if (not IO::FileSystem::Default ().Access (s.c_str ())) {
            HANDLE f = ::CreateFileW (s.c_str (), FILE_ALL_ACCESS, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (f != nullptr) {
                ::CloseHandle (f);
                DbgTrace (L"AppTempFileManager::GetTempFile (): returning '%s'", s.c_str ());
                return s;
            }
        }
    }
#else
    AssertNotImplemented ();
#endif
    Execution::Throw (Exception{"Unknown error creating file"sv}, "AppTempFileManager::GetTempFile (): failed to create tempfile");
}

String AppTempFileManager::GetTempDir (const String& fileNameBase)
{
    String fn = AppTempFileManager::Get ().GetMasterTempDir () + fileNameBase;

    int attempts = 0;
    while (attempts < 5) {
        String s = fn;
        char   buf[100];
        (void)snprintf (buf, NEltsOf (buf), "%d\\", ::rand ());
        s += String{buf};
        if (not Directory{s}.Exists ()) {
            CreateDirectory (s, true);
            DbgTrace (L"AppTempFileManager::GetTempDir (): returning '%s'", s.c_str ());
            return s;
        }
    }
    Execution::Throw (Exception{"Unknown error creating temporary file"sv}, "AppTempFileManager::GetTempDir (): failed to create tempdir");
}
/*
 ********************************************************************************
 **************************** FileSystem::ScopedTmpDir **************************
 ********************************************************************************
 */
ScopedTmpDir::ScopedTmpDir (const String& fileNameBase)
    : fTmpDir{AppTempFileManager::Get ().GetTempDir (fileNameBase)}
{
}

ScopedTmpDir::~ScopedTmpDir ()
{
    try {
        String dirName = fTmpDir.As<String> ();
        DbgTrace (L"ScopedTmpDir::~ScopedTmpDir - removing contents for '%s'", dirName.c_str ());
        IO::FileSystem::Default ().RemoveDirectory (dirName, RemoveDirectoryPolicy::eRemoveAnyContainedFiles);
    }
    catch (...) {
        WeakAssert (false); // not reached
    }
}

/*
 ********************************************************************************
 *********************** FileSystem::ScopedTmpFile ******************************
 ********************************************************************************
 */
ScopedTmpFile::ScopedTmpFile (const String& fileNameBase)
    : fTmpFile{AppTempFileManager::Get ().GetTempFile (fileNameBase)}
{
}

ScopedTmpFile::~ScopedTmpFile ()
{
    try {
        DbgTrace (L"ScopedTmpFile::~ScopedTmpFile - removing '%s'", fTmpFile.c_str ());
#if qPlatform_Windows
        ThrowIfZeroGetLastError (::DeleteFileW (fTmpFile.c_str ()));
#else
        AssertNotImplemented ();
#endif
    }
    catch (...) {
        DbgTrace ("ignoring exception in ~ScopedTmpFile - removing tmpfile");
    }
}

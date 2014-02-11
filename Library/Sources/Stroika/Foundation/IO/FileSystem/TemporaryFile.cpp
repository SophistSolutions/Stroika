/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <sys/types.h>
#include    <sys/stat.h>
#include    <ctime>
#include    <limits>
#include    <fstream>
#include    <fcntl.h>
#include    <cstdio>

#if     qPlatform_Windows
#include    <aclapi.h>
#include    <io.h>
#include    <shlobj.h>
#include    <windows.h>
#elif   qPlatform_POSIX
#include    <unistd.h>
#endif

#include    "../../Characters/Format.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Characters/CString/Utilities.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#include    "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include    "../../Containers/Common.h"
#include    "../../Containers/STL/SetUtils.h"
#include    "../../Debug/Trace.h"
#include    "../../IO/FileAccessException.h"
#include    "../../IO/FileBusyException.h"
#include    "../../IO/FileFormatException.h"
#include    "../../Memory/SmallStackBuffer.h"
#include    "FileUtils.h"
#include    "PathName.h"
#include    "WellKnownLocations.h"

#include    "TemporaryFile.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Containers::STL;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;
using   namespace   Stroika::Foundation::Memory;


#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif



/*
 * Stuff  INSIDE try section raises exceptions. Catch and rethow SOME binding in a new filename (if none was known).
 * Otehr exceptions just ignore (so they auto-propagate)
 */
#define     CATCH_REBIND_FILENAMES_HELPER_(USEFILENAME) \
    catch (const FileBusyException& e) {    \
        if (e.GetFileName ().empty ()) {\
            Execution::DoThrow (FileBusyException (USEFILENAME));\
        }\
        Execution::DoReThrow ();\
    }\
    catch (const FileAccessException& e) {  \
        if (e.GetFileName ().empty ()) {\
            Execution::DoThrow (FileAccessException (USEFILENAME, e.GetFileAccessMode ()));\
        }\
        Execution::DoReThrow ();\
    }\
    catch (const FileFormatException& e) {  \
        if (e.GetFileName ().empty ()) {\
            Execution::DoThrow (FileFormatException (USEFILENAME));\
        }\
        Execution::DoReThrow ();\
    }\
     










/*
 ********************************************************************************
 **************** FileSystem::Private::FileUtilsModuleData_ *********************
 ********************************************************************************
 */
FileSystem::Private::FileUtilsModuleData_::FileUtilsModuleData_ ()
    : fAppTempFileManager ()
{
}

FileSystem::Private::FileUtilsModuleData_::~FileUtilsModuleData_ ()
{
}




/*
 ********************************************************************************
 *********************** FileSystem::AppTempFileManager *************************
 ********************************************************************************
 */
AppTempFileManager::AppTempFileManager ()
    : fTmpDir ()
{
#if     qPlatform_Windows
    String tmpDir  =   WellKnownLocations::GetTemporary ();

    wchar_t   exePath[MAX_PATH];
    memset (exePath, 0, sizeof exePath);
    Verify (::GetModuleFileNameW (nullptr, exePath, NEltsOf (exePath)));

    wstring exeFileName =   exePath;
    {
        size_t  i   =   exeFileName.rfind ('\\');
        if (i != wstring::npos) {
            exeFileName = exeFileName.substr (i + 1);
        }
        // strip trailing .EXE
        i = exeFileName.rfind ('.');
        if (i != SDKString::npos) {
            exeFileName = exeFileName.erase (i);
        }
        // no biggie, but avoid spaces in tmpfile path name (but dont try too hard, should be
        // harmless)
        //  -- LGP 2009-08-16
        for (auto i = exeFileName.begin (); i != exeFileName.end (); ++i) {
            if (*i == ' ') {
                *i = '-';
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
    tmpDir += String_Constant (L"HealthFrameWorks\\");
    CreateDirectory (tmpDir);
    for (int i = 0; i < INT_MAX; ++i) {
        String d   =   tmpDir + Format (L"%s-%d-%d\\", exeFileName.c_str (), ::GetCurrentProcessId (), i + rand ());
        if (not ::CreateDirectoryW (d.c_str (), nullptr)) {
            DWORD error = ::GetLastError ();
            if (error == ERROR_ALREADY_EXISTS) {
                continue;   // try again
            }
            else {
                DbgTrace ("bad news if we cannot create AppTempFileManager::fTmpDir: %d", error);
                Execution::DoThrow (Execution::Platform::Windows::Exception (error));
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
#if     qPlatform_Windows
    DeleteAllFilesInDirectory (fTmpDir, true);
    Verify (::RemoveDirectoryW (fTmpDir.c_str ()));
#else
    //AssertNotImplemented ();
#endif
}

String AppTempFileManager::GetTempFile (const String& fileNameBase)
{
#if     qPlatform_Windows
    String fn  =   AppTempFileManager::Get ().GetMasterTempDir () + fileNameBase;
    FileSystem::CreateDirectoryForFile (fn);

    SDKString::size_type  suffixStart = fn.rfind ('.');
    if (suffixStart == SDKString::npos) {
        fn += String_Constant (L".txt");
        suffixStart = fn.rfind ('.');
    }
    int attempts = 0;
    while (attempts < 5) {
        wstring s = fn.As<wstring> ();
        char    buf[100];
        (void)::snprintf (buf, NEltsOf (buf), "%d", ::rand ());
        s.insert (suffixStart, NarrowSDKStringToWide (buf));
        if (not FileExists (s.c_str ())) {
            HANDLE  f = ::CreateFileW (s.c_str (), FILE_ALL_ACCESS, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
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
    Execution::DoThrow (StringException (L"Unknown error creating file"), "AppTempFileManager::GetTempFile (): failed to create tempfile");
}

String AppTempFileManager::GetTempDir (const String& fileNameBase)
{
    String fn  =   AppTempFileManager::Get ().GetMasterTempDir () + fileNameBase;

    int attempts = 0;
    while (attempts < 5) {
        String s = fn;
        char    buf[100];
        (void)::snprintf (buf, NEltsOf (buf), "%d\\", ::rand ());
        s += String::FromAscii (buf);
        if (not Directory (s).Exists ()) {
            CreateDirectory (s, true);
            DbgTrace (L"AppTempFileManager::GetTempDir (): returning '%s'", s.c_str ());
            return s;
        }
    }
    Execution::DoThrow (StringException (String_Constant (L"Unknown error creating temporary file")), "AppTempFileManager::GetTempDir (): failed to create tempdir");
}






#if 0


/*
 ********************************************************************************
 **************************** TempFileLibrarian *********************************
 ********************************************************************************
 */
TempFileLibrarian::TempFileLibrarian (const String& privateDirectory, bool purgeDirectory, bool makeTMPDIRRel, bool deleteFilesOnDescruction)
    : fFiles ()
    , fPrivateDirectory (privateDirectory)
    , fMakeTMPDIRRel (makeTMPDIRRel)
    , fDeleteFilesOnDescruction (deleteFilesOnDescruction)
    , fCriticalSection_ ()
{
    ::srand (static_cast<unsigned int> (::time (0)));
    if (purgeDirectory and fPrivateDirectory.size () > 0) {
        DeleteAllFilesInDirectory (AppTempFileManager::Get ().GetMasterTempDir () + fPrivateDirectory + L"\\");
    }
}

TempFileLibrarian::~TempFileLibrarian ()
{
#if     qPlatform_Windows
    if (fDeleteFilesOnDescruction) {
        for (auto it = fFiles.begin (); it != fFiles.end (); ++it) {
            // item could be a file or directory, so see if dir delete works, and only if that fails,
            // then try to delete the item as a directory ... all silently ignoring failures...
            if (::DeleteFileW (it->c_str ()) == 0) {
                FileSystem::DeleteAllFilesInDirectory (*it);
                (void)::RemoveDirectoryW (it->c_str ());
            }
        }
        if (fPrivateDirectory.size () > 0) {
            (void)::RemoveDirectoryW ((AppTempFileManager::Get ().GetMasterTempDir () + fPrivateDirectory + L"\\").c_str ());
        }
    }
#else
    AssertNotImplemented ();
#endif
}

String TempFileLibrarian::GetTempFile (const String& fileNameBase)
{
#if     qPlatform_Windows
    String fn  =   fileNameBase;
    if (fn.find (':') == -1) {
        if (fPrivateDirectory.size () > 0) {
            fn = fPrivateDirectory + L"\\" + fn;
        }

        if (fMakeTMPDIRRel) {
            fn = AppTempFileManager::Get ().GetMasterTempDir () + fn;
        }
    }
    FileSystem::CreateDirectoryForFile (fn);

    SDKString::size_type  suffixStart = fn.rfind ('.');
    if (suffixStart == SDKString::npos) {
        fn += L".txt";
        suffixStart = fn.rfind ('.');
    }

    int attempts = 0;
    while (attempts < 5) {
        wstring s = fn.As<wstring> ();
        char    buf[100];
        (void)::snprintf (buf, NEltsOf (buf), "%d", ::rand ());
        s.insert (suffixStart, NarrowSDKStringToWide (buf));
        if (not FileSystem::FileExists (s.c_str ())) {
            HANDLE  f = ::CreateFileW (s.c_str (), FILE_ALL_ACCESS, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (f != nullptr) {
                CloseHandle (f);
                lock_guard<mutex> enterCriticalSection (fCriticalSection_);
                fFiles.insert (s);
                return s;
            }
        }
    }
    Execution::DoThrow (StringException (L"Unknown error creating file"));
#else
    AssertNotImplemented ();
#endif
}

String TempFileLibrarian::GetTempDir (const String& fileNameBase)
{
#if     qPlatform_Windows
    String fn  =   fileNameBase;
    if (fn.find (':') == -1) {
        if (fPrivateDirectory.size () > 0) {
            fn = fPrivateDirectory + L"\\" + fn;
        }
        if (fMakeTMPDIRRel) {
            fn = AppTempFileManager::Get ().GetMasterTempDir () + fn;
        }
    }

    int attempts = 0;
    while (attempts < 5) {
        wstring s = fn.As<wstring> ();
        char    buf[100];
        {
            // man page doesn't gaurantee thread-safety of rand ()
            lock_guard<mutex> enterCriticalSection (fCriticalSection_);
            (void)::snprintf (buf, NEltsOf (buf), "%d\\", ::rand ());
        }
        s.append (NarrowSDKStringToWide  (buf));
        if (not Directory (s).Exists ()) {
            FileSystem::CreateDirectory (s, true);
            lock_guard<mutex> enterCriticalSection (fCriticalSection_);
            fFiles.insert (s);
            return s;
        }
    }
#else
    AssertNotImplemented ();
#endif
    Execution::DoThrow (StringException (L"Unknown error creating temporary file"));
}

#endif





/*
 ********************************************************************************
 **************************** FileSystem::ScopedTmpDir **************************
 ********************************************************************************
 */
ScopedTmpDir::ScopedTmpDir (const String& fileNameBase)
    : fTmpDir (AppTempFileManager::Get ().GetTempDir (fileNameBase))
{
}

ScopedTmpDir::~ScopedTmpDir ()
{
    try {
        String dirName = fTmpDir.As<String> ();
        DbgTrace (L"ScopedTmpDir::~ScopedTmpDir - removing contents for '%s'", dirName.c_str ());
        DeleteAllFilesInDirectory (dirName);
#if     qPlatform_Windows
        Verify (::RemoveDirectoryW (dirName.c_str ()));
#else
        AssertNotImplemented ();
#endif
    }
    catch (...) {
        DbgTrace ("ignoring exception in ~ScopedTmpDir - removing tmpfiles");
    }
}






/*
 ********************************************************************************
 *********************** FileSystem::ScopedTmpFile ******************************
 ********************************************************************************
 */
ScopedTmpFile::ScopedTmpFile (const String& fileNameBase):
    fTmpFile (AppTempFileManager::Get ().GetTempFile (fileNameBase))
{
}

ScopedTmpFile::~ScopedTmpFile ()
{
    try {
        DbgTrace (L"ScopedTmpFile::~ScopedTmpFile - removing '%s'", fTmpFile.c_str ());
#if     qPlatform_Windows
        ThrowIfFalseGetLastError (::DeleteFileW (fTmpFile.c_str ()));
#else
        AssertNotImplemented ();
#endif
    }
    catch (...) {
        DbgTrace ("ignoring exception in ~ScopedTmpFile - removing tmpfile");
    }
}







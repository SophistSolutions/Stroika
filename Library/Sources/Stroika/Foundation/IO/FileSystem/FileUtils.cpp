/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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

#include "../../Characters/Format.h"
#include "../../Containers/Set.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#include "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include "../../Containers/Common.h"
#include "../../Debug/Trace.h"
#include "../../IO/FileSystem/Exception.h"
#include "../../IO/FileSystem/FileSystem.h"
#include "../../IO/FileSystem/PathName.h"
#include "../../Memory/SmallStackBuffer.h"
#include "PathName.h"
#include "WellKnownLocations.h"

#include "FileUtils.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Containers::STL;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Memory;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

/*
 ********************************************************************************
 ******************* FileSystem::FileSizeToDisplayString ************************
 ********************************************************************************
 */
String IO::FileSystem::FileSizeToDisplayString (FileOffset_t bytes)
{
    if (bytes < 1000) {
        return Format (L"%d bytes", static_cast<int> (bytes));
    }
    else if (bytes < 1000 * 1024) {
        return Format (L"%.1f K", static_cast<double> (bytes) / 1024.0f);
    }
    else {
        return Format (L"%.1f MB", static_cast<double> (bytes) / (1024 * 1024.0f));
    }
}

/*
 ********************************************************************************
 ************************ FileSystem::SetFileAccessWideOpened *******************
 ********************************************************************************
 */
/*
 * Sets permissions for users on a given folder to full control
 *
 *      Add 'Everyone' to have FULL ACCESS to the given argument file
 *
 */
void IO::FileSystem::SetFileAccessWideOpened (const filesystem::path& filePathName)
{
#if qPlatform_Windows
    static PACL pACL = nullptr; // Don't bother with ::LocalFree (pACL); - since we cache keeping this guy around for speed
    if (pACL == nullptr) {
        PSID pSIDEveryone = nullptr;

        {
            // Specify the DACL to use.
            // Create a SID for the Everyone group.
            SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
            if (!::AllocateAndInitializeSid (&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSIDEveryone)) {
                return; // if this fails - perhaps old OS with no security - just fail silently...
            }
        }

        EXPLICIT_ACCESS ea[1]{};
        // Set FULL access for Everyone.
        ea[0].grfAccessPermissions = GENERIC_ALL;
        ea[0].grfAccessMode        = SET_ACCESS;
        ea[0].grfInheritance       = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
        ea[0].Trustee.TrusteeForm  = TRUSTEE_IS_SID;
        ea[0].Trustee.TrusteeType  = TRUSTEE_IS_WELL_KNOWN_GROUP;
        ea[0].Trustee.ptstrName    = (LPTSTR)pSIDEveryone;

        if (ERROR_SUCCESS != ::SetEntriesInAcl (static_cast<DWORD> (NEltsOf (ea)), ea, nullptr, &pACL)) {
            ::FreeSid (pSIDEveryone);
            return; // silently ignore errors - probably just old OS etc....
        }
        ::FreeSid (pSIDEveryone);
    }

    // Try to modify the object's DACL.
    [[maybe_unused]] DWORD dwRes = ::SetNamedSecurityInfo (
        const_cast<SDKChar*> (filePathName.c_str ()), // name of the object
        SE_FILE_OBJECT,                               // type of object
        DACL_SECURITY_INFORMATION,                    // change only the object's DACL
        nullptr, nullptr,                             // don't change owner or group
        pACL,                                         // DACL specified
        nullptr);                                     // don't change SACL
                                                      // ignore error from this routine for now  - probably means either we don't have permissions or OS too old to support...
#elif qPlatform_POSIX
    ////TODO: Somewhat PRIMITIVE - TMPHACK
    if (filePathName.empty ()) [[UNLIKELY_ATTR]] {
        Execution::Throw (Exception{make_error_code (errc::no_such_file_or_directory), L"bad filename"_k});
    }
    struct stat s;
    IO::FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::stat (filePathName.generic_string ().c_str (), &s), filePathName);

    mode_t desiredMode = (S_IRUSR | S_IRGRP | S_IROTH) | (S_IWUSR | S_IWGRP | S_IWOTH);
    if (S_ISDIR (s.st_mode)) {
        desiredMode |= (S_IXUSR | S_IXGRP | S_IXOTH);
    }

    int result = 0;
    // Don't call chmod if mode is already open (because doing so could fail even though we already have what we wnat if were not the owner)
    if ((s.st_mode & desiredMode) != desiredMode) {
        result = ::chmod (filePathName.generic_string ().c_str (), desiredMode);
    }
    IO::FileSystem::Exception::ThrowPOSIXErrNoIfNegative (result, filePathName);
#else
    AssertNotImplemented ();
#endif
}

/*
 ********************************************************************************
 ************************ FileSystem::CreateDirectory ***************************
 ********************************************************************************
 */
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
void IO::FileSystem::CreateDirectory (const filesystem::path& directoryPathOrig, bool createParentComponentsIfNeeded)
{
    String directoryPath = FromPath (directoryPathOrig);
    /*
     * TODO:
     *      (o)     This implementation is HORRIBLE!!!! Major cleanup required!
     */
#if qPlatform_Windows
    if (createParentComponentsIfNeeded) {
        // walk path and break into parts, and from top down - try to create parent directory structure.
        // Ignore any failures - and just let the report of failure (if any must result) come from original basic
        // CreateDirectory call.
        size_t index = directoryPath.find ('\\');
        while (index != -1 and index + 1 < directoryPath.length ()) {
            String parentPath = directoryPath.substr (0, index);
            IgnoreExceptionsForCall (CreateDirectory (ToPath (parentPath), false));
            index = directoryPath.find ('\\', index + 1);
        }
    }

    if (not ::CreateDirectoryW (directoryPath.c_str (), nullptr)) {
        DWORD error = ::GetLastError ();
        if (error != ERROR_ALREADY_EXISTS) {
            IO::FileSystem::Exception::ThrowSystemErrNo (error, ToPath (directoryPath));
        }
    }
#elif qPlatform_POSIX
    if (createParentComponentsIfNeeded) {
        // walk path and break into parts, and from top down - try to create parent directory structure.
        // Ignore any failures - and just let the report of failure (if any must result) come from original basic
        // CreateDirectory call.
        vector<String> paths;
        size_t         index = directoryPath.find ('/');
        while (index != -1 and index + 1 < directoryPath.length ()) {
            if (index != 0) {
                String parentPath = directoryPath.substr (0, index);
                //IgnoreExceptionsForCall (CreateDirectory (parentPath, false));
                paths.push_back (parentPath);
            }
            index = directoryPath.find ('/', index + 1);
        }

        // Now go in reverse order - checking if the exist - and if so - stop going back
        for (auto i = paths.rbegin (); i != paths.rend (); ++i) {
            //NB: this avoids matching files - we know dir - cuz name ends in /
            if (access (ToPath (*i).generic_string ().c_str (), R_OK) == 0) {
                // ignore this one
            }
            else {
                // THEN - starting at the one that doesn't exist - go from top-down again
                int skipThisMany = (i - paths.rbegin ());
                Assert (skipThisMany < paths.size ());
                for (auto ii = paths.begin () + skipThisMany; ii != paths.end (); ++ii) {
                    CreateDirectory (ToPath (*ii), false);
                }
                break;
            }
        }
    }
    // Horrible - needs CLEANUP!!! -- LGP 2011-09-26
    if (::mkdir (directoryPathOrig.generic_string ().c_str (), 0755) != 0) {
        if (errno != EEXIST) {
            IO::FileSystem::Exception::ThrowPOSIXErrNo (errno, directoryPathOrig);
        }
    }
#else
    AssertNotImplemented ();
#endif
}
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")

/*
 ********************************************************************************
 ******************* FileSystem::CreateDirectoryForFile *************************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996) // use deprecated
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
void IO::FileSystem::CreateDirectoryForFile (const filesystem::path& filePath)
{
    if (filePath.empty ()) [[UNLIKELY_ATTR]] {
        // NOT sure this is the best exception to throw here?
        Execution::Throw (IO::FileSystem::Exception (make_error_code (errc::no_such_file_or_directory), filePath));
    }
    if (IO::FileSystem::Default ().Access (filePath)) {
        // were done
        return;
    }
    CreateDirectory (filePath.parent_path (), true); //CreateDirectory (ToPath (GetFileDirectory (FromPath (filePath))), true);
}
DISABLE_COMPILER_MSC_WARNING_END (4996) // use deprecated
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")

/*
 ********************************************************************************
 ************************** FileSystem::GetVolumeName ***************************
 ********************************************************************************
 */
String IO::FileSystem::GetVolumeName (const filesystem::path& driveLetterAbsPath)
{
#if qPlatform_Windows
    // SEM_FAILCRITICALERRORS needed to avoid dialog in call to GetVolumeInformation
    AdjustSysErrorMode errorModeAdjuster (AdjustSysErrorMode::GetErrorMode () | SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

    DWORD   ignored = 0;
    SDKChar volNameBuf[1024]{};
    SDKChar igBuf[1024]{};
    BOOL    result = ::GetVolumeInformation (
        driveLetterAbsPath.c_str (),
        volNameBuf,
        static_cast<DWORD> (NEltsOf (volNameBuf)),
        nullptr,
        &ignored,
        &ignored,
        igBuf,
        static_cast<DWORD> (NEltsOf (igBuf)));
    if (result) {
        return String::FromSDKString (volNameBuf);
    }
#else
    AssertNotImplemented ();
#endif
    return String{};
}

/*
 ********************************************************************************
 ******************************* FileSystem::CopyFile ***************************
 ********************************************************************************
 */
void IO::FileSystem::CopyFile (const filesystem::path& srcFile, const filesystem::path& destPath)
{
#if qPlatform_Windows
    // see if can be/should be rewritten to use Win32 API of same name!!!
    //
    // If I DON'T do that remapping to Win32 API, then redo this at least to copy / rename through tmpfile
    IO::FileSystem::Default ().CheckAccess (srcFile, IO::AccessMode::eRead);
    create_directories (destPath.parent_path ());
    ThrowIfZeroGetLastError (::CopyFile (destPath.c_str (), srcFile.c_str (), false));
#else
    AssertNotImplemented ();
#endif
}

/*
 ********************************************************************************
 ***************************** FileSystem::FindFiles ****************************
 ********************************************************************************
 */
vector<String> IO::FileSystem::FindFiles (const filesystem::path& path, const String& fileNameToMatch)
{
    vector<String> result;
    if (path.empty ()) {
        return result;
    }
#if qPlatform_Windows
    String          usePath       = AssureDirectoryPathSlashTerminated (FromPath (path));
    String          matchFullPath = usePath + (fileNameToMatch.empty () ? L"*" : fileNameToMatch);
    WIN32_FIND_DATA fd{};
    HANDLE          hFind = ::FindFirstFile (matchFullPath.AsSDKString ().c_str (), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        try {
            do {
                String fileName = String::FromSDKString (fd.cFileName);
                if (not(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    result.push_back (usePath + fileName);
                }
            } while (::FindNextFile (hFind, &fd));
        }
        catch (...) {
            ::FindClose (hFind);
            Execution::ReThrow ();
        }
        ::FindClose (hFind);
    }
#else
    AssertNotImplemented ();
#endif
    return result;
}

/*
 ********************************************************************************
 ************************* FileSystem::FindFilesOneDirUnder *********************
 ********************************************************************************
 */
vector<String> IO::FileSystem::FindFilesOneDirUnder (const filesystem::path& path, const String& fileNameToMatch)
{
    if (path.empty ()) {
        return vector<String> ();
    }

    Containers::Set<String> resultSet;
#if qPlatform_Windows
    String          usePath = AssureDirectoryPathSlashTerminated (FromPath (path));
    WIN32_FIND_DATA fd;
    memset (&fd, 0, sizeof (fd));
    HANDLE hFind = ::FindFirstFile ((usePath + L"*").AsSDKString ().c_str (), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            //SDKString fileName = (LPCTSTR)&fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                String              fileName = String::FromSDKString ((LPCTSTR)&fd.cFileName);
                static const String kDOT     = L"."sv;
                static const String kDOTDOT  = L".."sv;
                if ((fileName != kDOT) and (fileName != kDOTDOT)) {
                    resultSet += Containers::Set<String> (FindFiles (ToPath (usePath) / ToPath (fileName), fileNameToMatch));
                }
            }
        } while (::FindNextFile (hFind, &fd));
        ::FindClose (hFind);
    }
#else
    AssertNotImplemented ();
#endif
    return vector<String> (resultSet.begin (), resultSet.end ());
}

#if qPlatform_Windows
/*
 ********************************************************************************
 ********************* FileSystem::DirectoryChangeWatcher ***********************
 ********************************************************************************
 */
IO::FileSystem::DirectoryChangeWatcher::DirectoryChangeWatcher (const filesystem::path& directoryName, bool watchSubTree, DWORD notifyFilter)
    : fDirectory (FromPath (directoryName))
    , fWatchSubTree (watchSubTree)
    , fThread ()
    , fDoneEvent (::CreateEvent (nullptr, false, false, nullptr))
    , fWatchEvent (::FindFirstChangeNotification (fDirectory.AsSDKString ().c_str (), fWatchSubTree, notifyFilter))
    , fQuitting (false)
{
    fThread = Execution::Thread::New ([this] () { ThreadProc (this); }, Execution::Thread::eAutoStart, L"DirectoryChangeWatcher");
}

IO::FileSystem::DirectoryChangeWatcher::~DirectoryChangeWatcher ()
{
    fQuitting = true;
    if (fDoneEvent != INVALID_HANDLE_VALUE) {
        Verify (::SetEvent (fDoneEvent));
    }
    // critical we wait for finish of thread cuz it has bare 'this' pointer captured
    Thread::SuppressInterruptionInContext suppressInterruption;
    IgnoreExceptionsForCall (fThread.AbortAndWaitForDone ());
    if (fDoneEvent != INVALID_HANDLE_VALUE) {
        Verify (::CloseHandle (fDoneEvent));
    }
    if (fWatchEvent != INVALID_HANDLE_VALUE) {
        Verify (::FindCloseChangeNotification (fWatchEvent));
    }
}

void IO::FileSystem::DirectoryChangeWatcher::ValueChanged ()
{
}

void IO::FileSystem::DirectoryChangeWatcher::ThreadProc (void* lpParameter)
{
    DirectoryChangeWatcher* _THS_ = reinterpret_cast<DirectoryChangeWatcher*> (lpParameter);
    while (not _THS_->fQuitting and _THS_->fWatchEvent != INVALID_HANDLE_VALUE) {
        HANDLE events[2];
        events[0] = _THS_->fDoneEvent;
        events[1] = _THS_->fWatchEvent;
        ::WaitForMultipleObjects (static_cast<DWORD> (NEltsOf (events)), events, false, INFINITE);
        Verify (::FindNextChangeNotification (_THS_->fWatchEvent));
        if (not _THS_->fQuitting) {
            _THS_->ValueChanged ();
        }
    }
}
#endif

#if qPlatform_Windows
/*
 ********************************************************************************
 ********************** FileSystem::AdjustSysErrorMode **************************
 ********************************************************************************
 */
UINT AdjustSysErrorMode::GetErrorMode ()
{
    UINT good = ::SetErrorMode (0);
    ::SetErrorMode (good);
    return good;
}

AdjustSysErrorMode::AdjustSysErrorMode (UINT newErrorMode)
    : fSavedErrorMode (::SetErrorMode (newErrorMode))
{
}

AdjustSysErrorMode::~AdjustSysErrorMode ()
{
    (void)::SetErrorMode (fSavedErrorMode);
}
#endif

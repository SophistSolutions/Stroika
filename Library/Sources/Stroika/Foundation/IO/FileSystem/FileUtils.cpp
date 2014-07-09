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
#include    "../../Containers/Set.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#include    "../../Execution/Platform/Windows/HRESULTErrorException.h"
#endif
#include    "../../Containers/Common.h"
#include    "../../Debug/Trace.h"
#include    "../../IO/FileAccessException.h"
#include    "../../IO/FileBusyException.h"
#include    "../../IO/FileFormatException.h"
#include    "../../IO/FileSystem/FileSystem.h"
#include    "../../Memory/SmallStackBuffer.h"
#include    "PathName.h"
#include    "WellKnownLocations.h"

#include    "FileUtils.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Containers::STL;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;
using   namespace   Stroika::Foundation::Memory;


using   Characters::String_Constant;

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
void    IO::FileSystem::SetFileAccessWideOpened (const String& filePathName)
{
    try {
#if     qPlatform_Windows
        static  PACL pACL = nullptr;    // Don't bother with ::LocalFree (pACL); - since we cache keeping this guy around for speed
        if (pACL == nullptr) {
            PSID pSIDEveryone = nullptr;

            {
                // Specify the DACL to use.
                // Create a SID for the Everyone group.
                SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
                if (!::AllocateAndInitializeSid (&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSIDEveryone))  {
                    return;     // if this fails - perhaps old OS with no security - just fail silently...
                }
            }

            EXPLICIT_ACCESS ea[1];
            memset (&ea, 0, sizeof (ea));

            // Set FULL access for Everyone.
            ea[0].grfAccessPermissions = GENERIC_ALL;
            ea[0].grfAccessMode = SET_ACCESS;
            ea[0].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
            ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
            ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
            ea[0].Trustee.ptstrName = (LPTSTR) pSIDEveryone;

            if (ERROR_SUCCESS != ::SetEntriesInAcl (NEltsOf (ea), ea, nullptr, &pACL)) {
                ::FreeSid (pSIDEveryone);
                return; // silently ignore errors - probably just old OS etc....
            }
            ::FreeSid (pSIDEveryone);
        }

        // Try to modify the object's DACL.
        DWORD dwRes  = SetNamedSecurityInfo(
                           const_cast<SDKChar*> (filePathName.AsSDKString ().c_str ()),          // name of the object
                           SE_FILE_OBJECT,              // type of object
                           DACL_SECURITY_INFORMATION,   // change only the object's DACL
                           nullptr, nullptr,                  // don't change owner or group
                           pACL,                        // DACL specified
                           nullptr
                       );                       // don't change SACL
        // ignore error from this routine for now  - probably means either we don't have permissions or OS too old to support...
#elif   qPlatform_POSIX
        ////TODO: Somewhat PRIMITIVE - TMPHACK
        if (filePathName.empty ()) {
            Execution::DoThrow (StringException (String_Constant (L"bad filename")));
        }
        struct  stat    s;
        ThrowErrNoIfNegative (::stat (filePathName.AsSDKString ().c_str (), &s));

        mode_t  desiredMode =   (S_IRUSR | S_IRGRP | S_IROTH) | (S_IWUSR | S_IWGRP | S_IWOTH);
        if (S_ISDIR (s.st_mode)) {
            desiredMode |= (S_IXUSR | S_IXGRP | S_IXOTH);
        }

        int result = 0;
        // Don't call chmod if mode is already open (because doing so could fail even though we already have what we wnat if were not the owner)
        if ((s.st_mode & desiredMode) != desiredMode) {
            result = chmod (filePathName.AsSDKString ().c_str (), desiredMode);
        }
        ThrowErrNoIfNegative (result);
#else
        AssertNotImplemented ();
#endif
    }
    CATCH_REBIND_FILENAMES_HELPER_(filePathName);
}








/*
 ********************************************************************************
 ************************ FileSystem::CreateDirectory ***************************
 ********************************************************************************
 */
void    IO::FileSystem::CreateDirectory (const String& directoryPath, bool createParentComponentsIfNeeded)
{
    /*
     * TODO:
     *      (o)     This implementation is HORRIBLE!!!! Major cleanup required!
     */
    try {
#if     qPlatform_Windows
        if (createParentComponentsIfNeeded) {
            // walk path and break into parts, and from top down - try to create parent directory structure.
            // Ignore any failures - and just let the report of failure (if any must result) come from original basic
            // CreateDirectory call.
            size_t  index   =   directoryPath.find ('\\');
            while (index != -1 and index + 1 < directoryPath.length ()) {
                String parentPath = directoryPath.substr (0, index);
                IgnoreExceptionsForCall (CreateDirectory (parentPath, false));
                index = directoryPath.find ('\\', index + 1);
            }
        }

        if (not ::CreateDirectoryW (directoryPath.c_str (), nullptr)) {
            DWORD error = ::GetLastError ();
            if (error != ERROR_ALREADY_EXISTS) {
                Execution::DoThrow (Execution::Platform::Windows::Exception (error));
            }
        }
#elif   qPlatform_POSIX
        if (createParentComponentsIfNeeded) {
            // walk path and break into parts, and from top down - try to create parent directory structure.
            // Ignore any failures - and just let the report of failure (if any must result) come from original basic
            // CreateDirectory call.
            vector<String> paths;
            size_t  index   =   directoryPath.find ('/');
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
                if (access(i->AsSDKString ().c_str (), R_OK) == 0) {
                    // ignore this one
                }
                else {
                    // THEN - starting at the one that doesn't exist - go from top-down again
                    int skipThisMany    =   (i - paths.rbegin ());
                    Assert (skipThisMany < paths . size ())
                    for (auto ii = paths.begin () + skipThisMany; ii != paths.end (); ++ii) {
                        CreateDirectory (*ii, false);
                    }
                    break;
                }
            }
        }
        // Horrible - needs CLEANUP!!! -- LGP 2011-09-26
        if (::mkdir (directoryPath.AsSDKString ().c_str (), 0755) != 0) {
            if (errno != 0 and errno != EEXIST) {
                Execution::DoThrow (errno_ErrorException (errno));
            }
        }
#else
        AssertNotImplemented ();
#endif
    }
    CATCH_REBIND_FILENAMES_HELPER_(directoryPath);
}







/*
 ********************************************************************************
 ******************* FileSystem::CreateDirectoryForFile *************************
 ********************************************************************************
 */
void    IO::FileSystem::CreateDirectoryForFile (const String& filePath)
{
    if (filePath.empty ()) {
        // NOT sure this is the best exception to throw here?
        Execution::DoThrow (IO::FileAccessException ());
    }
    if (IO::FileSystem::FileSystem::Default ().Access (filePath)) {
        // were done
        return;
    }
    CreateDirectory (GetFileDirectory (filePath), true);
}






/*
 ********************************************************************************
 ************************** FileSystem::GetVolumeName ***************************
 ********************************************************************************
 */
String IO::FileSystem::GetVolumeName (const String& driveLetterAbsPath)
{
#if     qPlatform_Windows
    // SEM_FAILCRITICALERRORS needed to avoid dialog in call to GetVolumeInformation
    AdjustSysErrorMode  errorModeAdjuster (AdjustSysErrorMode::GetErrorMode () | SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

    DWORD   ignored =   0;
    SDKChar   volNameBuf[1024];
    memset (volNameBuf, 0, sizeof (volNameBuf));
    SDKChar   igBuf[1024];
    memset (igBuf, 0, sizeof (igBuf));
    BOOL    result  =   ::GetVolumeInformation (
                            AssureDirectoryPathSlashTerminated (driveLetterAbsPath).AsSDKString ().c_str (),
                            volNameBuf,
                            NEltsOf (volNameBuf),
                            nullptr,
                            &ignored,
                            &ignored,
                            igBuf,
                            NEltsOf (igBuf)
                        );
    if (result) {
        return String::FromSDKString (volNameBuf);
    }
#else
    AssertNotImplemented ();
#endif
    return String ();
}







/*
 ********************************************************************************
 ***************************** FileSystem::FileExists ***************************
 ********************************************************************************
 */
bool    IO::FileSystem::FileExists (const String& filePath)
{
#if     qPlatform_Windows
    DWORD attribs = ::GetFileAttributesW (filePath.c_str ());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return not (attribs & FILE_ATTRIBUTE_DIRECTORY);
#elif   qPlatform_POSIX
    // Not REALLY right - but an OK hack for now... -- LGP 2011-09-26
    //http://linux.die.net/man/2/access
    return access (filePath.AsSDKString().c_str (), R_OK) == 0;
#else
    AssertNotImplemented ();
    return false;
#endif
}






#if 0
/*
 ********************************************************************************
 ************************** FileSystem::DirectoryExists *************************
 ********************************************************************************
 */
bool    FileSystem::DirectoryExists (const String& filePath)
{
#if     qPlatform_Windows
    RequireNotNull (filePath);
    DWORD attribs = ::GetFileAttributesW (filePath.c_str ());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return !! (attribs & FILE_ATTRIBUTE_DIRECTORY);
#elif   qPlatform_POSIX
    struct  stat    s;
    if (::stat (filePath.AsSDKString ().c_str (), &s) < 0) {
        // If file doesn't exist - or other error reading, just say not exist
        return false;
    }
    return S_ISDIR (s.st_mode);
#else
    AssertNotImplemented ();
    return false;
#endif
}
#endif






/*
 ********************************************************************************
 ******************************* FileSystem::CopyFile ***************************
 ********************************************************************************
 */
void    IO::FileSystem::CopyFile (const String& srcFile, const String& destPath)
{
#if     qPlatform_Windows
// see if can be/should be rewritten to use Win32 API of same name!!!
//
// If I DONT do that remapping to Win32 API, then redo this at least to copy / rename through tmpfile
    IO::FileSystem::FileSystem::Default ().CheckAccess (srcFile, IO::FileAccessMode::eRead);
    CreateDirectoryForFile (destPath);
    ThrowIfFalseGetLastError (::CopyFile (destPath.AsSDKString ().c_str (), srcFile.AsSDKString ().c_str (), false));
#else
    AssertNotImplemented ();
#endif
}









/*
 ********************************************************************************
 ***************************** FileSystem::FindFiles ****************************
 ********************************************************************************
 */
vector<String> IO::FileSystem::FindFiles (const String& path, const String& fileNameToMatch)
{
    vector<String> result;
    if (path.empty ()) {
        return result;
    }
#if     qPlatform_Windows
    String usePath         =   AssureDirectoryPathSlashTerminated (path);
    String matchFullPath   =   usePath + (fileNameToMatch.empty () ? L"*" : fileNameToMatch);
    WIN32_FIND_DATA fd;
    memset (&fd, 0, sizeof (fd));
    HANDLE hFind = ::FindFirstFile (matchFullPath.AsSDKString ().c_str (), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        try {
            do {
                String fileName = String::FromSDKString (fd.cFileName);
                if (not (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    result.push_back (usePath + fileName);
                }
            }
            while (::FindNextFile (hFind, &fd));
        }
        catch (...) {
            ::FindClose (hFind);
            Execution::DoReThrow ();
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
vector<String> IO::FileSystem::FindFilesOneDirUnder (const String& path, const String& fileNameToMatch)
{
    if (path.empty ()) {
        return vector<String> ();
    }

    Containers::Set<String>    resultSet;
#if     qPlatform_Windows
    String usePath =   AssureDirectoryPathSlashTerminated (path);
    WIN32_FIND_DATA fd;
    memset (&fd, 0, sizeof (fd));
    HANDLE hFind = ::FindFirstFile ((usePath + L"*").AsSDKString ().c_str (), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            SDKString fileName = (LPCTSTR) &fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                String fileName = String::FromSDKString ((LPCTSTR) &fd.cFileName);
                const   String kDOT    =   String_Constant (L".");
                const   String kDOTDOT =   String_Constant (L"..");
                if ((fileName != kDOT) and (fileName != kDOTDOT)) {
                    resultSet += Containers::Set<String> (FindFiles (usePath + fileName, fileNameToMatch));
                }
            }
        }
        while (::FindNextFile (hFind, &fd));
        ::FindClose (hFind);
    }
#else
    AssertNotImplemented ();
#endif
    return vector<String> (resultSet.begin (), resultSet.end ());
}








/*
 ********************************************************************************
 ***************** FileSystem::DeleteAllFilesInDirectory ************************
 ********************************************************************************
 */
void    IO::FileSystem::DeleteAllFilesInDirectory (const String& path, bool ignoreErrors)
{
#if     qPlatform_Windows
    if (path.empty ()) {
        Execution::DoThrow (Execution::Platform::Windows::Exception (ERROR_INVALID_NAME));
    }
    String dir2Use =   AssureDirectoryPathSlashTerminated (path);

    WIN32_FIND_DATA fd;
    (void)::memset (&fd, 0, sizeof (fd));
    HANDLE          hFind = ::FindFirstFile ((dir2Use + String_Constant (L"*")).AsSDKString ().c_str (), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        try {
            do {
                String fileName = String::FromSDKString (fd.cFileName);
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if ((fileName != String_Constant (L".")) and (fileName != String_Constant (L".."))) {
                        DeleteAllFilesInDirectory (dir2Use + fileName + String_Constant (L"\\"), ignoreErrors);
                        try {
                            ThrowIfFalseGetLastError (::RemoveDirectory ((dir2Use + fileName).AsSDKString ().c_str ()));
                        }
                        catch (...) {
                            DbgTrace ("Exception %s calling RemoveDirectory on file '%s'", ignoreErrors ? L"(ignored)" : L"", (dir2Use + fileName).c_str ());
                            if (!ignoreErrors) {
                                Execution::DoReThrow ();
                            }
                        }
                    }
                }
                else {
                    try {
                        ThrowIfFalseGetLastError (::DeleteFile ((dir2Use + fileName).AsSDKString ().c_str ()));
                    }
                    catch (...) {
                        DbgTrace (SDKSTR ("Exception %s calling ::DeleteFile on file '%s'"), ignoreErrors ? SDKSTR ("(ignored)") : SDKSTR (""), (dir2Use + fileName).c_str ());
                        if (!ignoreErrors) {
                            Execution::DoReThrow ();
                        }
                    }
                }
            }
            while (::FindNextFile (hFind, &fd));
        }
        catch (...) {
            ::FindClose (hFind);
            Execution::DoReThrow ();
        }
        ::FindClose (hFind);
    }
#else
    AssertNotImplemented ();
#endif
}





#if     qPlatform_Windows
/*
 ********************************************************************************
 ********************* FileSystem::DirectoryChangeWatcher ***********************
 ********************************************************************************
 */
IO::FileSystem::DirectoryChangeWatcher::DirectoryChangeWatcher (const String& directoryName, bool watchSubTree, DWORD notifyFilter)
    : fDirectory (directoryName)
    , fWatchSubTree (watchSubTree)
    , fThread ()
    , fDoneEvent (::CreateEvent (nullptr, false, false, nullptr))
    , fWatchEvent (::FindFirstChangeNotification (fDirectory.AsSDKString ().c_str (), fWatchSubTree, notifyFilter))
    , fQuitting (false)
{
    fThread = Execution::Thread (bind (&ThreadProc, this));
    fThread.SetThreadName (L"DirectoryChangeWatcher");
    fThread.Start ();
}

IO::FileSystem::DirectoryChangeWatcher::~DirectoryChangeWatcher ()
{
    fQuitting = true;
    if (fDoneEvent != INVALID_HANDLE_VALUE) {
        Verify (::SetEvent (fDoneEvent));
    }
    // critical we wait for finish of thread cuz it has bare 'this' pointer captured
    Thread::SuppressAbortInContext  suppressAbort;
    IgnoreExceptionsForCall (fThread.AbortAndWaitForDone ());
    if (fDoneEvent != INVALID_HANDLE_VALUE) {
        Verify (::CloseHandle (fDoneEvent));
    }
    if (fWatchEvent != INVALID_HANDLE_VALUE) {
        Verify (::FindCloseChangeNotification (fWatchEvent));
    }
}

void    IO::FileSystem::DirectoryChangeWatcher::ValueChanged ()
{
}

void    IO::FileSystem::DirectoryChangeWatcher::ThreadProc (void* lpParameter)
{
    DirectoryChangeWatcher*     _THS_   =   reinterpret_cast<DirectoryChangeWatcher*> (lpParameter);
    while (not _THS_->fQuitting and _THS_->fWatchEvent != INVALID_HANDLE_VALUE) {
        HANDLE  events[2];
        events[0] = _THS_->fDoneEvent;
        events[1] = _THS_->fWatchEvent;
        ::WaitForMultipleObjects (NEltsOf (events), events, false, INFINITE);
        Verify (::FindNextChangeNotification (_THS_->fWatchEvent));
        if (not _THS_->fQuitting) {
            _THS_->ValueChanged ();
        }
    }
}
#endif







#if         qPlatform_Windows
/*
 ********************************************************************************
 ********************** FileSystem::AdjustSysErrorMode **************************
 ********************************************************************************
 */
UINT    AdjustSysErrorMode::GetErrorMode ()
{
    UINT    good    =   ::SetErrorMode (0);
    ::SetErrorMode (good);
    return good;
}

AdjustSysErrorMode::AdjustSysErrorMode (UINT newErrorMode):
    fSavedErrorMode (::SetErrorMode (newErrorMode))
{
}

AdjustSysErrorMode::~AdjustSysErrorMode ()
{
    (void)::SetErrorMode (fSavedErrorMode);
}
#endif








/*
 ********************************************************************************
 ************************ DirectoryContentsIterator *****************************
 ********************************************************************************
 */
DirectoryContentsIterator::DirectoryContentsIterator (const String& pathExpr)
#if         qPlatform_Windows
    : fHandle (INVALID_HANDLE_VALUE)
    , fFindFileData ()
    , fDirectory (pathExpr)
#endif
{
#if         qPlatform_Windows
    memset (&fFindFileData, 0, sizeof (fFindFileData));
    size_t i = fDirectory.rfind ('\\');
    if (i == wstring::npos) {
        Execution::DoThrow (StringException (L"Cannot find final '\\' in directory path"));
    }
    fDirectory = fDirectory.substr (0, i + 1);
    fHandle = ::FindFirstFile (pathExpr.AsSDKString ().c_str (), &fFindFileData);
#else
    AssertNotImplemented ();
#endif
}

DirectoryContentsIterator::~DirectoryContentsIterator ()
{
#if         qPlatform_Windows
    if (fHandle != INVALID_HANDLE_VALUE) {
        ::FindClose (fHandle);
    }
#else
    AssertNotImplemented ();
#endif
}

bool    DirectoryContentsIterator::NotAtEnd () const
{
#if         qPlatform_Windows
    return fHandle != INVALID_HANDLE_VALUE;
#else
    AssertNotImplemented ();
    return false;
#endif
}

String DirectoryContentsIterator::operator *() const
{
    Ensure (NotAtEnd ());
#if         qPlatform_Windows
    return fDirectory + String::FromSDKString (fFindFileData.cFileName);
#else
    AssertNotImplemented ();
    return String ();
#endif
}

void    DirectoryContentsIterator::operator++ ()
{
#if         qPlatform_Windows
    memset (&fFindFileData, 0, sizeof (fFindFileData));
    if (::FindNextFile (fHandle, &fFindFileData) == 0) {
        ::FindClose (fHandle);
        fHandle = INVALID_HANDLE_VALUE;
    }
#else
    AssertNotImplemented ();
#endif
}



/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileUtils_h_
#define _Stroika_Foundation_IO_FileSystem_FileUtils_h_ 1

#include "../../StroikaPreComp.h"

#include <filesystem>
#include <set>
#include <vector>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../../Characters/SDKChar.h"
#include "../../Configuration/Common.h"
#include "../../Debug/Assertions.h"
#include "../../Execution/Thread.h"
#include "../../Time/DateTime.h"

#include "../AccessMode.h"
#include "Common.h"
#include "Directory.h"

/**
 * TODO:
 *
 *          o   This file is obsolete, and will be gradually replaced - moving its contents out to other modules

 */

namespace Stroika::Foundation::IO::FileSystem {

    String FileSizeToDisplayString (FileOffset_t bytes);

    void SetFileAccessWideOpened (const filesystem::path& filePathName);

    /*
     * Possibly useful, even after using std::filesystem, as I dont think supported in std::filesystem.
     *
        * CreateDirectory makes sure the directory with the given name exists on the filesystem. If it already exists, this is NOT an error.
        * If it already exists, but is a FILE (anything but a directory) - then it is an error).
        *
        * If createParentComponentsIfNeeded - the function braks the directory paoth into parts, and recursively applies itself to each segment from
        * the root down.
        *
        * The net effect - this makes sure the given DIRECTORY exists or raises an exception.
        *
        *          TODO:
        *              (o) We need an overload which takes the directory permissions as argument.
        */
    [[deprecated ("Since Stroika 2.1b2, use create_directories() or create_directory()")]] void CreateDirectory (const filesystem::path& directoryPath, bool createParentComponentsIfNeeded = true);

    [[deprecated ("Since Stroika 2.1b2, use create_directories() or create_directory() with argument filePath.parent_path ()")]] void CreateDirectoryForFile (const filesystem::path& filePath);

    /**
     * Possibly useful, even after using std::filesystem, as I dont think supported in std::filesystem.
     * but @todo DOCUMENT WHAT THIS DOES ***
     */
    String GetVolumeName (const filesystem::path& driveLetterAbsPath);

    vector<String> FindFiles (const filesystem::path& path, const String& fileNameToMatch = L"*.*");

    vector<String> FindFilesOneDirUnder (const filesystem::path& path, const String& fileNameToMatch = L"*.*");

    void CopyFile (const filesystem::path& srcFile, const filesystem::path& destPath);

// COULD be made portable but alot of changes needed
#if qPlatform_Windows
    class DirectoryChangeWatcher {
    private:
        DirectoryChangeWatcher (const DirectoryChangeWatcher&) = delete;
        void operator= (const DirectoryChangeWatcher&) = delete;

    public:
        DirectoryChangeWatcher (const filesystem::path& directoryName, bool watchSubTree = false, DWORD notifyFilter = FILE_NOTIFY_CHANGE_LAST_WRITE);
        virtual ~DirectoryChangeWatcher ();

    protected:
        virtual void ValueChanged ();

    private:
        static void ThreadProc (void* lpParameter);

    private:
        String                 fDirectory;
        bool                   fWatchSubTree;
        Execution::Thread::Ptr fThread;
        HANDLE                 fDoneEvent;
        HANDLE                 fWatchEvent;
        bool                   fQuitting;
    };
#endif

// Should be in a PLATFORM_WINDOWS subfile or sub-namespace... And DOCUMENT!!!!
#if qPlatform_Windows
    struct AdjustSysErrorMode {
        static UINT GetErrorMode ();
        AdjustSysErrorMode (UINT newErrorMode);
        ~AdjustSysErrorMode ();
        UINT fSavedErrorMode;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileUtils.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileUtils_h_*/

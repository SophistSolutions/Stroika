/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileUtils_h_
#define _Stroika_Foundation_IO_FileSystem_FileUtils_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>
#include <set>
#include <vector>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "Stroika/Foundation/Characters/SDKChar.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Time/DateTime.h"

#include "Stroika/Foundation/IO/AccessMode.h"
#include "Stroika/Foundation/IO/FileSystem/Common.h"

/**
 * TODO:
 *
 *          o   This file is obsolete, and will be gradually replaced - moving its contents out to other modules

 */

namespace Stroika::Foundation::IO::FileSystem {

    String FileSizeToDisplayString (FileOffset_t bytes);

    void SetFileAccessWideOpened (const filesystem::path& filePathName);

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
        void operator= (const DirectoryChangeWatcher&)         = delete;

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

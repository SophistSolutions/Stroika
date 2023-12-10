/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_TemporaryFile_h_
#define _Stroika_Foundation_IO_FileSystem_TemporaryFile_h_ 1

#include "../../StroikaPreComp.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::IO::FileSystem {

    /**
     *  Generally not used directly - prefer using ScopedTmpDir or ScopedTmpFile.
     *
     *  If used directly, you can make your own, or use AppTempFileManager::sThe.
     * 
     *  AppTempFileManager::sThe CAN be overwritten, but best practice is to do so early in main (main thread)
     *  before any threads have been created (whihc might create a race).
     */
    class AppTempFileManager {
    public:
        /**
         */
        struct Options {
            optional<filesystem::path> fRelativePathInsideTmpDir; // defaults to EXE name (less extensions)
        };

    public:
        /**
         *  In typical use, this won't be directly constructed - just use the default sThe
         */
        AppTempFileManager (const AppTempFileManager&) = delete;
        AppTempFileManager (const Options& options = {});
        ~AppTempFileManager ();

    public:
        /**
         *  This DESTROYS any contained files if they existed in 'this' before assignment. If that destruction fails, so will this
         *  assignment.
         * 
         *  This maybe used as in main () {
         *      AppTempFileManager::sThe = AppTempFileManager{AppTempFileManager::Options{... custom options}}
         */
        AppTempFileManager& operator= (AppTempFileManager&& rhs);

    public:
        /**
         *  Note - this global variable is used by ScopedTmpDir, and ScopedTmpFile.
         *  This can be overwritten (assigned to), but following C++ thread safety rules, so best to do in the main thread before any
         *  thread could have begin accessing the object.
         */
        static AppTempFileManager sThe;

    public:
        nonvirtual filesystem::path GetRootTempDir () const;

    public:
        /**
        *  require root_path is empt - just filename and possibly extention. If extension missing, maybe added automatically
         */
        nonvirtual filesystem::path GetTempFile (const filesystem::path& fileBaseName);

    public:
        nonvirtual filesystem::path GetTempDir (const String& dirNameBase);

    private:
        filesystem::path fTmpDir_;
    };
    inline AppTempFileManager AppTempFileManager::sThe;

    /**
     *  Create the 'basenamed' temporary directory (no worries about name conflicts) from the argument filenameBase.
     * 
     *  Directory and its contents destroyed on destruction (exceptions caught internally and DbgTraced, but otherwise ignored).
     * 
     *  note references AppTempFileManager::sThe
     */
    class ScopedTmpDir {
    public:
        ScopedTmpDir (const String& fileNameBase);
        ScopedTmpDir (const ScopedTmpDir&) = delete;
        ~ScopedTmpDir ();
        ScopedTmpDir& operator= (const ScopedTmpDir&) = delete;

    public:
        operator filesystem::path () const;

    private:
        filesystem::path fTmpDir_;
    };

    /**
     *  Create the 'basenamed' temporary file (no worries about name conflicts) from the argument filenameBase (including file suffix).
     * 
     *  The file is removed on destruction (exceptions caught internally and DbgTraced, but otherwise ignored).
     * 
     *  note references AppTempFileManager::sThe
     */
    class ScopedTmpFile {
    public:
        ScopedTmpFile (const filesystem::path& fileBaseName);
        ScopedTmpFile (const ScopedTmpFile&) = delete;
        ~ScopedTmpFile ();
        ScopedTmpFile& operator= (const ScopedTmpFile&) = delete;

    public:
        operator filesystem::path () const;

    private:
        filesystem::path fTmpFile_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TemporaryFile.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_TemporaryFile_h_*/

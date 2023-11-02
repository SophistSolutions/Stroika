/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_TemporaryFile_h_
#define _Stroika_Foundation_IO_FileSystem_TemporaryFile_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/SDKChar.h"
#include "../../Configuration/Common.h"
#include "../../Debug/Assertions.h"
#include "../../Execution/Thread.h"
#include "../../Time/DateTime.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Very rough draft based on code used in HealthFrame. Major cleanups required.
 *
 */

namespace Stroika::Foundation::IO::FileSystem {

    class AppTempFileManager {
    private:
        AppTempFileManager ();
        ~AppTempFileManager ();

    public:
        static AppTempFileManager& Get ();

    public:
        nonvirtual filesystem::path GetMasterTempDir () const;

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

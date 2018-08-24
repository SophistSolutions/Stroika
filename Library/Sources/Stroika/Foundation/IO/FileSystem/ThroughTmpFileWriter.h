/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_h_
#define _Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_h_ 1

#include "../../StroikaPreComp.h"

#include <set>
#include <vector>

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../../Characters/SDKChar.h"
#include "../../Characters/String_Constant.h"
#include "../../Configuration/Common.h"
#include "../../Debug/Assertions.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Thread.h"
#include "../../Time/DateTime.h"

#include "../FileAccessMode.h"
#include "Directory.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::IO::FileSystem {

    using Characters::String;
    using Time::DateTime;

    /**
     * Specify the name of a file to write, and an optional file suffix for a tempfile, and
     * support writing through the tempfile, and then atomicly renaming the file when done.
     *
     * Even in case of failure, assure the tmpfile is removed.
     *
     * There is no need to first remove 'realFileName' - as this will not fail because it already exists (unless
     * we dont have permission to remove it).
     *
     * The point of this is to allow writing a file in such a way that the entire file write is
     * atomic. We don't want to partially  update a file and upon failure, leave it corrupted.
     *
     * Using this class, you create a tempfile, write to it, and the Commit () the change. NOTE,
     * it is REQUIRED you call Commit () after all writing to tmpfile is done (and closed),
     * otherwise the changes are abandoned.
     *
     *  \par Example Usage
     *      \code
     *      ThroughTmpFileWriter    tmpFile (targetFileName);
     *      IO::FileSystem::FileOutputStream::Ptr outStream = IO::FileSystem::FileOutputStream::New (tmpFile.GetFilePath ());
     *      YourCodeToWriteDataToStream (your_data, out);
     *      out.clear();        // close like this so we can throw exception - cannot throw if we count on DTOR
     *      tmpFile.Commit ();  // any exceptions cause the tmp file to be automatically cleaned up
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *      ThroughTmpFileWriter    tmpFile (targetFileName);
     *      {
     *          FileOutputStream  out (tmpFile.GetTmpFilePath ());
     *          YourCodeToWriteDataToStream (your_data, out);
     *          // use scope (braces) to force close before commit (or call out.clear())
     *      }
     *      tmpFile.Commit ();  // any exceptions cause the tmp file to be automatically cleaned up
     *      \endcode
     *
     */
    class ThroughTmpFileWriter {
    public:
        ThroughTmpFileWriter (const String& realFileName, const String& tmpSuffix = Characters::String_Constant{L".tmp"});
        ThroughTmpFileWriter (const ThroughTmpFileWriter&) = delete;
        ~ThroughTmpFileWriter ();

    public:
        nonvirtual ThroughTmpFileWriter& operator= (const ThroughTmpFileWriter&) = delete;

    public:
        /**
         */
        nonvirtual String GetTmpFilePath () const;

    public:
        /**
         */
        nonvirtual String GetRealFilePath () const;

    public:
        /**
         *  Before commit this returns the tmpfile name. After commit returns the eventual file name.
         */
        nonvirtual String GetFilePath () const;

    public:
        /**
         * tmpfile must have been closed the time we call Commit, and it atomicly renames the file
         *  to the target name. This CAN fail (in which case cleanup is handled automatically)
         */
        nonvirtual void Commit ();

    private:
        String fRealFilePath_;
        String fTmpFilePath_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ThroughTmpFileWriter.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_h_*/

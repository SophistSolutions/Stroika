/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_h_
#define _Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_h_  1

#include    "../../StroikaPreComp.h"

#include    <set>
#include    <vector>

#if         qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../../Characters/SDKChar.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Configuration/Common.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"
#include    "../../Time/DateTime.h"

#include    "../FileAccessMode.h"
#include    "Directory.h"



/**
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;
                using   Memory::Byte;
                using   Time::DateTime;


                /**
                 * Specify the name of a file to write, and an optional file suffix for a tempfile, and
                 * support writing through the tempfile, and then atomicly renaming the file when done.
                 *
                 * Even in case of failure, assure the tmpfile is removed.
                 *
                 * The point of this is to allow writing a file in such a way that the entire file write is
                 * atomic. We don't want to partially  update a file and upon failure, leave it corrupted.
                 *
                 * Using this class, you create a tempfile, write to it, and the Commit () the change. NOTE,
                 * it is REQUIRED you call Commit () after all writing to tmpfile is done (and closed),
                 * otehrwise the changes are abandoned.
                 */
                class   ThroughTmpFileWriter {
                public:
                    ThroughTmpFileWriter (const String& realFileName, const String& tmpSuffix = Characters::String_Constant (L".tmp"));
                    ThroughTmpFileWriter (const ThroughTmpFileWriter&) = delete;
                    ~ThroughTmpFileWriter ();

                public:
                    nonvirtual  ThroughTmpFileWriter& operator= (const ThroughTmpFileWriter&) = delete;

                public:
                    /**
                     */
                    nonvirtual  String GetTmpFilePath () const;

                public:
                    /**
                     */
                    nonvirtual  String GetRealFilePath () const;

                public:
                    /**
                     *  Before commit this returns the tmpfile name. After commit returns the eventual file name.
                     */
                    nonvirtual  String GetFilePath () const;

                public:
                    // tmpfile must have been closed the time we call Commit, and it atomicly renames the file
                    // to the target name. This CAN fail (in which case cleanup is handled automatically)
                    nonvirtual  void    Commit ();

                private:
                    String fRealFilePath_;
                    String fTmpFilePath_;
                };


            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ThroughTmpFileWriter.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_ThroughTmpFileWriter_h_*/

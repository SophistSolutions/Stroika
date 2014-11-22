/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_h_
#define _Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Streams/BinaryInputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Use Exeuction??xx?? - caller - handler for thread interupts..
 *
 *      @todo   Unclear if we need the mutexes here. Probably yes (necause our API promises re-entrancy but I'm unclear
 *              on filessytem reads/writes).
 *
 *      @todo   This code is just a first (probably working) draft. But it needs cleanup. Review
 *              older FileReader/FileWriter code - and see if the windows code should use CreateFile instead of s_open...
 *
 *      @todo   We need a BinaryFileInputOutputStream
 *
 *      @todo   Document these are not buffered, (well document). Document how easy to wrap buffered stream around.
 *
 *      @todo   Consider either adding directly here (optionally) the code from MemoryMappedFileReader (vintage 2013-09-02
 *              or earlier). CreateFileMapping etc. Worthless unless that happens to be more efficient, and I have
 *              no good reason to believe it will be. But maybe? Maybe cuz seeks more efficient (++ not syscall). Same
 *              for reads - if caller does lots of little reads.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;


                /**
                 */
                class   BinaryFileInputStream : public Streams::BinaryInputStream {
                private:
                    using   inherited   =   BinaryInputStream;
                private:
                    class   Rep_;

                public:
                    enum SeekableFlag { eSeekable, eNotSeekable };
                    BinaryFileInputStream (const String& fileName, SeekableFlag seekable = eSeekable);

                private:
                    BinaryFileInputStream (const shared_ptr<Rep_>& rep, SeekableFlag seekable);

                public:
                    enum BufferFlag { eBuffered, eUnbuffered };
                    static  BinaryInputStream   mk (const String& fileName, SeekableFlag seekable = eSeekable, BufferFlag bufferFlag = eBuffered);
                    static  BinaryInputStream   mk (const String& fileName, BufferFlag bufferFlag);
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
#include    "BinaryFileInputStream.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_h_*/

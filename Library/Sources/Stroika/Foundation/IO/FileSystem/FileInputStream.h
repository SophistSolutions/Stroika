/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileInputStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileInputStream_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Streams/InputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Add method to get access to FD  - PeekAtFD(), GetFD()???)
 *
 *      @todo   Consder adding 'getfd' and 'detach' methods to detach stream from fd, to match/work better
 *              with attach CTOR
 *
 *      @todo   Use Exeuction??xx?? - caller - handler for thread interrupts..
 *
 *      @todo   Unclear if we need the mutexes here. Probably yes (necause our API promises re-entrancy but I'm unclear
 *              on filessytem reads/writes).
 *
 *      @todo   This code is just a first (probably working) draft. But it needs cleanup. Review
 *              older FileReader/FileWriter code - and see if the windows code should use CreateFile instead of s_open...
 *
 *      @todo   We need a FileInputOutputStream
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
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#Must-Externally-Synchronize-Letter-Thread-Safety">Must-Externally-Synchronize-Letter-Thread-Safety</a>
                 */
                class   FileInputStream : public Streams::InputStream<Memory::Byte> {
                private:
                    using   inherited   =   InputStream<Memory::Byte>;
                private:
                    class   Rep_;

                public:
                    using   FileDescriptorType  =   int;

                public:
                    /**
                     *  The constructor overload with FileDescriptorType does an 'attach' - taking ownership (and thus later closing) the argument file descriptor.
                     */
                    enum SeekableFlag { eSeekable, eNotSeekable };
                    FileInputStream (const String& fileName, SeekableFlag seekable = eSeekable);
                    FileInputStream (FileDescriptorType fd, SeekableFlag seekable = eSeekable);

                public:
                    enum BufferFlag { eBuffered, eUnbuffered };

                public:
                    /**
                    */
                    static  InputStream<Memory::Byte> mk (const String& fileName, SeekableFlag seekable = eSeekable, BufferFlag bufferFlag = eBuffered);
                    static  InputStream<Memory::Byte> mk (const String& fileName, BufferFlag bufferFlag);
                    static  InputStream<Memory::Byte> mk (FileDescriptorType fd, SeekableFlag seekable = eSeekable, BufferFlag bufferFlag = eBuffered);
                    static  InputStream<Memory::Byte> mk (FileDescriptorType fd, BufferFlag bufferFlag);
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
#include    "FileInputStream.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_FileInputStream_h_*/

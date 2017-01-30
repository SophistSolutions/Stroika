/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileInputStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileInputStream_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Streams/InputStream.h"

#include    "FileStreamCommon.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
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
                 *
                 *  \note   We considered having a GetFD () method to retrieve the file descriptor, but that opened up too many
                 *          possabilities for bugs (like changing the blocking nature of the IO). If you wish - you can always
                 *          open the file descriptor yourself, track it yourself, and do what you will to it and pass it in,
                 *          but then the results are 'on you.
                 */
                class   FileInputStream : public Streams::InputStream<Memory::Byte>, public FileStreamCommon {
                private:
                    using   inherited   =   InputStream<Memory::Byte>;
                private:
                    class   Rep_;

                public:
                    /**
                     *  The constructor overload with FileDescriptorType does an 'attach' - taking ownership (and thus later closing) the argument file descriptor.
                     *
                     *  \req fd is a valid file descriptor (for that overload)
                     */
                    FileInputStream (const String& fileName, SeekableFlag seekable = SeekableFlag::eDEFAULT);
                    FileInputStream (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekable = SeekableFlag::eDEFAULT);

                public:
                    enum    class   BufferFlag {
                        eBuffered,
                        eUnbuffered,

                        eDEFAULT = eBuffered,
					
						Stroika_Define_Enum_Bounds(eBuffered, eUnbuffered)
					};
                    static  constexpr BufferFlag eBuffered = BufferFlag::eBuffered;
                    static  constexpr BufferFlag eUnbuffered = BufferFlag::eUnbuffered;

                public:
                    /**
                     * @see FileOutputStream constructor
                     */
                    static  InputStream<Memory::Byte> mk (const String& fileName, SeekableFlag seekable = SeekableFlag::eDEFAULT, BufferFlag bufferFlag = BufferFlag::eDEFAULT);
                    static  InputStream<Memory::Byte> mk (const String& fileName, BufferFlag bufferFlag);
                    static  InputStream<Memory::Byte> mk (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekable = SeekableFlag::eDEFAULT, BufferFlag bufferFlag = BufferFlag::eDEFAULT);
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

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileInputStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileInputStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Streams/InputStream.h"
#include "../../Streams/InternallySyncrhonizedInputStream.h"

#include "FileStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
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

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {

                using Characters::String;

                /**
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
                 *
                 *  \note   We considered having a GetFD () method to retrieve the file descriptor, but that opened up too many
                 *          possabilities for bugs (like changing the blocking nature of the IO). If you wish - you can always
                 *          open the file descriptor yourself, track it yourself, and do what you will to it and pass it in,
                 *          but then the results are 'on you.
                 */
                class FileInputStream : public Streams::InputStream<Memory::Byte>, public FileStream {
                public:
                    FileInputStream ()                       = delete;
                    FileInputStream (const FileInputStream&) = delete;

                public:
                    enum class BufferFlag {
                        eBuffered,
                        eUnbuffered,

                        eDEFAULT = eBuffered,

                        Stroika_Define_Enum_Bounds (eBuffered, eUnbuffered)
                    };
                    static constexpr BufferFlag eBuffered   = BufferFlag::eBuffered;
                    static constexpr BufferFlag eUnbuffered = BufferFlag::eUnbuffered;

                public:
                    class Ptr;

                public:
                    /**
                     *  The static New method is like a constructor, but it constructs a smart pointer of some appropriate subtype defined by its parameters.
                     *
                     *  The New overload with FileDescriptorType does an 'attach' - taking ownership (and thus later closing) the argument file descriptor.
                     *
                     *  \req fd is a valid file descriptor (for that overload)
                     *
                     *  \par Example Usage
                     *      \code
                     *          Ptr stream = FileInputStream::New (kProcCPUInfoFileName_, FileInputStream::eNotSeekable);
                     *      \endcode
                     */
                    static Ptr                            New (const String& fileName, SeekableFlag seekable = SeekableFlag::eDEFAULT);
                    static Ptr                            New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekable = SeekableFlag::eDEFAULT);
                    static Ptr                            New (Execution::InternallySyncrhonized internallySyncrhonized, const String& fileName, SeekableFlag seekable = SeekableFlag::eDEFAULT);
                    static Ptr                            New (Execution::InternallySyncrhonized internallySyncrhonized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekable = SeekableFlag::eDEFAULT);
                    static InputStream<Memory::Byte>::Ptr New (const String& fileName, SeekableFlag seekable, BufferFlag bufferFlag);
                    static InputStream<Memory::Byte>::Ptr New (const String& fileName, BufferFlag bufferFlag);
                    static InputStream<Memory::Byte>::Ptr New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable, BufferFlag bufferFlag);
                    static InputStream<Memory::Byte>::Ptr New (FileDescriptorType fd, BufferFlag bufferFlag);

                private:
                    class Rep_;

                private:
                    template <typename X>
                    using BLAH_            = FileInputStream;
                    using InternalSyncRep_ = Streams::InternallySyncrhonizedInputStream<Memory::Byte, BLAH_, FileInputStream::Rep_>;
                };

                /**
                 *  Ptr is a copyable smart pointer to a FileInputStream.
                 */
                class FileInputStream::Ptr : public Streams::InputStream<Memory::Byte>::Ptr {
                private:
                    using inherited = Streams::InputStream<Memory::Byte>::Ptr;

                public:
                    /**
                     *  \par Example Usage
                     *      \code
                     *          Memory::BLOB b = IO::FileSystem::FileInputStream::Ptr{ IO::FileSystem::FileInputStream (fileName) }.ReadAll ();
                     *      \endcode
                     */
                    Ptr ()                = delete;
                    Ptr (const Ptr& from) = default;

                protected:
                    Ptr (const shared_ptr<Rep_>& from);

                public:
                    nonvirtual Ptr& operator= (const Ptr& rhs) = default;

                private:
                    friend class FileInputStream;
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
#include "FileInputStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileInputStream_h_*/

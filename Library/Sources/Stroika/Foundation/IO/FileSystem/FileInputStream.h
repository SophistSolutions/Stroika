/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileInputStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileInputStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Streams/InputStream.h"
#include "../../Streams/InternallySynchronizedInputStream.h"

#include "Common.h"

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

namespace Stroika::Foundation::IO::FileSystem {

    using std::byte;

    using Characters::String;

    /**
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
     *
     *  \note   We considered having a GetFD () method to retrieve the file descriptor, but that opened up too many
     *          possabilities for bugs (like changing the blocking nature of the IO). If you wish - you can always
     *          open the file descriptor yourself, track it yourself, and do what you will to it and pass it in,
     *          but then the results are 'on you.
     */
    class FileInputStream : public Streams::InputStream<byte>, public FileStream {
    public:
        FileInputStream ()                       = delete;
        FileInputStream (const FileInputStream&) = delete;

    public:
        /**
         */
        enum class BufferFlag {
            eBuffered,
            eUnbuffered,

            Stroika_Define_Enum_Bounds (eBuffered, eUnbuffered)
        };

    public:
        /**
         */
        static constexpr BufferFlag eBuffered = BufferFlag::eBuffered;

    public:
        /**
         */
        static constexpr BufferFlag eUnbuffered = BufferFlag::eUnbuffered;

    public:
        /**
         */
        static constexpr BufferFlag kBufferFlag_DEFAULT = BufferFlag::eBuffered;

    public:
        /**
         */
        using SeekableFlag = Streams::SeekableFlag;

    public:
        /**
         */
        static constexpr SeekableFlag eSeekable = SeekableFlag::eSeekable;

    public:
        /**
         */
        static constexpr SeekableFlag eNotSeekable = SeekableFlag::eNotSeekable;

    public:
        /**
         *  \note - prior to v2.1d27, this defaulted to unseekable, but for files, it makes way more sense to default to seekable, since
         *        doing so typically costs nothing, and is pretty commonly useful.
         */
        static constexpr SeekableFlag kSeekableFlag_DEFAULT = SeekableFlag::eSeekable;

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
        static Ptr                    New (const filesystem::path& fileName, SeekableFlag seekable = kSeekableFlag_DEFAULT);
        static Ptr                    New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekable = kSeekableFlag_DEFAULT);
        static Ptr                    New (Execution::InternallySynchronized internallySynchronized, const filesystem::path& fileName, SeekableFlag seekable = kSeekableFlag_DEFAULT);
        static Ptr                    New (Execution::InternallySynchronized internallySynchronized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekable = kSeekableFlag_DEFAULT);
        static InputStream<byte>::Ptr New (const filesystem::path& fileName, SeekableFlag seekable, BufferFlag bufferFlag);
        static InputStream<byte>::Ptr New (const filesystem::path& fileName, BufferFlag bufferFlag);
        static InputStream<byte>::Ptr New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable, BufferFlag bufferFlag);
        static InputStream<byte>::Ptr New (FileDescriptorType fd, BufferFlag bufferFlag);

    private:
        class Rep_;

    protected:
        /**
         *  Utility to create a Ptr wrapper (to avoid having to subclass the Ptr class and access its protected constructor)
         */
        static Ptr _mkPtr (const shared_ptr<Rep_>& s);

    private:
        using InternalSyncRep_ = Streams::InternallySynchronizedInputStream<byte, FileInputStream, FileInputStream::Rep_>;
    };

    /**
     *  Ptr is a copyable smart pointer to a FileInputStream.
     */
    class FileInputStream::Ptr : public Streams::InputStream<byte>::Ptr {
    private:
        using inherited = Streams::InputStream<byte>::Ptr;

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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileInputStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileInputStream_h_*/

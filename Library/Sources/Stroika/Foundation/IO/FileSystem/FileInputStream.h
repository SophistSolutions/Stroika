/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileInputStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileInputStream_h_ 1

#include "../../StroikaPreComp.h"

#include <filesystem>

#include "../../Characters/String.h"
#include "../../Streams/InputStream.h"

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

namespace Stroika::Foundation::IO::FileSystem::FileInputStream {

    using Characters::String;
    using namespace FileStream;

    using Ptr = Streams::InputStream::Ptr<byte>;
    /**
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *
     *  \note   We considered having a GetFD () method to retrieve the file descriptor, but that opened up too many
     *          possabilities for bugs (like changing the blocking nature of the IO). If you wish - you can always
     *          open the file descriptor yourself, track it yourself, and do what you will to it and pass it in,
     *          but then the results are 'on you.
     */

    /**
     */
    enum class BufferFlag {
        eBuffered,
        eUnbuffered,

        Stroika_Define_Enum_Bounds (eBuffered, eUnbuffered)
    };

    /**
     */
    constexpr BufferFlag eBuffered = BufferFlag::eBuffered;

    /**
     */
    constexpr BufferFlag eUnbuffered = BufferFlag::eUnbuffered;

    /**
     */
    constexpr BufferFlag kBufferFlag_DEFAULT = BufferFlag::eBuffered;

    /**
     */
    using SeekableFlag = Streams::SeekableFlag;

    /**
     */
    constexpr SeekableFlag eSeekable = SeekableFlag::eSeekable;

    /**
     */
    constexpr SeekableFlag eNotSeekable = SeekableFlag::eNotSeekable;

    /**
     *  \note - prior to v2.1d27, this defaulted to unseekable, but for files, it makes way more sense to default to seekable, since
     *        doing so typically costs nothing, and is pretty commonly useful.
     */
    constexpr SeekableFlag kSeekableFlag_DEFAULT = SeekableFlag::eSeekable;

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
    Ptr New (const filesystem::path& fileName, SeekableFlag seekable = kSeekableFlag_DEFAULT);
    Ptr New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekable = kSeekableFlag_DEFAULT);
    Ptr New (Execution::InternallySynchronized internallySynchronized, const filesystem::path& fileName, SeekableFlag seekable = kSeekableFlag_DEFAULT);
    Ptr                             New (Execution::InternallySynchronized internallySynchronized, FileDescriptorType fd,
                                         AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekable = kSeekableFlag_DEFAULT);
    Streams::InputStream::Ptr<byte> New (const filesystem::path& fileName, SeekableFlag seekable, BufferFlag bufferFlag);
    Streams::InputStream::Ptr<byte> New (const filesystem::path& fileName, BufferFlag bufferFlag);
    Streams::InputStream::Ptr<byte> New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable, BufferFlag bufferFlag);
    Streams::InputStream::Ptr<byte> New (FileDescriptorType fd, BufferFlag bufferFlag);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileInputStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileInputStream_h_*/

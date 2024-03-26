/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Consider either adding directly here (optionally) the code from MemoryMappedFileReader (vintage 2013-09-02
 *              or earlier). CreateFileMapping etc. Worthless unless that happens to be more efficient, and I have
 *              no good reason to believe it will be. But maybe? Maybe cuz seeks more efficient (++ not syscall). Same
 *              for reads - if caller does lots of little reads.
 */

namespace Stroika::Foundation::IO::FileSystem::FileInputStream {

    using Characters::String;
    using namespace FileStream;

    using namespace Streams;
    using Ptr = Streams::InputStream::Ptr<byte>;

    /**
     *
     *  \note   We considered having a GetFD () method to retrieve the file descriptor, but that opened up too many
     *          possibilities for bugs (like changing the blocking nature of the IO). If you wish - you can always
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
     *          static const filesystem::path kProcCPUInfoFileName_{"/proc/cpuinfo"sv};
     *          Ptr stream = FileInputStream::New (kProcCPUInfoFileName_, FileInputStream::eNotSeekable);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Ptr stdinStream = FileInputStream::New (0, AdoptFDPolicy::eDisconnectOnDestruction);
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    Ptr New (const filesystem::path& fileName, SeekableFlag seekable = kSeekableFlag_DEFAULT);
    Ptr New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable = kSeekableFlag_DEFAULT);
    Ptr New (Execution::InternallySynchronized internallySynchronized, const filesystem::path& fileName, SeekableFlag seekable = kSeekableFlag_DEFAULT);
    Ptr New (Execution::InternallySynchronized internallySynchronized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy,
             SeekableFlag seekable = kSeekableFlag_DEFAULT);
    Ptr New (const filesystem::path& fileName, SeekableFlag seekable, BufferFlag bufferFlag);
    Ptr New (const filesystem::path& fileName, BufferFlag bufferFlag);
    Ptr New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekable, BufferFlag bufferFlag);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileInputStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileInputStream_h_*/

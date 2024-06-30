/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileOutputStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileOutputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/FileSystem/FileStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   Add optional file sharing parameters to stream readers, and maybe file-descriptor CTOR?
 */

namespace Stroika::Foundation::IO::FileSystem ::FileOutputStream {

    using Characters::String;

    using namespace FileStream;
    using namespace Streams;

    using Ptr = Streams::OutputStream::Ptr<byte>;

    /**
     *  This flag is used to configure if BinaryOutputStream::Flush will invoke the OS fsync() function
     *  to force data to disk (by default Flush just forces the data out of this object to the next object,
     *  for files, the operating system).
     *
     *  \note   Design note:
     *      It was explicitly chosen to not use enum class here for brevity sake, since this names are already well scoped.
     */
    enum class FlushFlag {
        eToOperatingSystem,
        eToDisk,

        eDEFAULT = eToOperatingSystem,

        Stroika_Define_Enum_Bounds (eToOperatingSystem, eToDisk)
    };
    constexpr FlushFlag eToOperatingSystem = FlushFlag::eToOperatingSystem;
    constexpr FlushFlag eToDisk            = FlushFlag::eToDisk;

    /**
     *  Default AppendFlag is eStartFromStart (truncation), not eAppend
     */
    enum class AppendFlag {
        eStartFromStart, // aka truncate
        eAppend,

        eDEFAULT = eStartFromStart,

        Stroika_Define_Enum_Bounds (eStartFromStart, eAppend)
    };
    constexpr AppendFlag eStartFromStart = AppendFlag::eStartFromStart;
    constexpr AppendFlag eAppend         = AppendFlag::eAppend;

    enum class BufferFlag {
        eBuffered,
        eUnbuffered,

        Stroika_Define_Enum_Bounds (eBuffered, eUnbuffered)
    };
    constexpr BufferFlag eBuffered   = BufferFlag::eBuffered;
    constexpr BufferFlag eUnbuffered = BufferFlag::eUnbuffered;

    /**
     */
    constexpr BufferFlag kBufferFlag_DEFAULT = BufferFlag::eBuffered;

    using SeekableFlag                           = Streams::SeekableFlag;
    constexpr SeekableFlag eSeekable             = SeekableFlag::eSeekable;
    constexpr SeekableFlag eNotSeekable          = SeekableFlag::eNotSeekable;
    constexpr SeekableFlag kSeekableFlag_DEFAULT = SeekableFlag::eNotSeekable;

    /**
     *  The constructor overload with FileDescriptorType does an 'attach' - taking ownership (and thus later closing) the argument file descriptor (depending on AdoptFDPolicy).
     *
     *  \req fd is a valid file descriptor (for that overload)
     *
     *  \note   We considered having a GetFD () method to retrieve the file descriptor, but that opened up too many
     *          possibilities for bugs (like changing the blocking nature of the IO). If you wish - you can always
     *          open the file descriptor yourself, track it yourself, and do what you will to it and pass it in,
     *          but then the results are 'on you.
     *
     *  \note   The overloads taking no BufferFlag produce a non-buffered stream.
     *
     *  \par Example Usage
     *      \code
     *          IO::FileSystem::FileOutputStream::New ("/tmp/foo").Write (Memory::BLOB {0x3});
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + "t.txt";
     *          JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          TextWriter tw { IO::FileSystem::FileOutputStream::New ("/tmp/fred.txt") };
     *          tw.PrintF (L"Hello %s\n", L"World");
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Ptr<byte> stdoutStream = FileOutputStream::New (1, FileStream::AdoptFDPolicy::eDisconnectOnDestruction);
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    Ptr New (const filesystem::path& fileName, FlushFlag flushFlag = FlushFlag::eDEFAULT);
    Ptr New (const filesystem::path& fileName, AppendFlag appendFlag, FlushFlag flushFlag = FlushFlag::eDEFAULT);
    Ptr New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag = kSeekableFlag_DEFAULT,
             FlushFlag flushFlag = FlushFlag::eDEFAULT);
    Ptr New (Execution::InternallySynchronized internallySynchronized, const filesystem::path& fileName, FlushFlag flushFlag = FlushFlag::eDEFAULT);
    Ptr New (Execution::InternallySynchronized internallySynchronized, const filesystem::path& fileName, AppendFlag appendFlag,
             FlushFlag flushFlag = FlushFlag::eDEFAULT);
    Ptr New (Execution::InternallySynchronized internallySynchronized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy,
             SeekableFlag seekableFlag = kSeekableFlag_DEFAULT, FlushFlag flushFlag = FlushFlag::eDEFAULT);
    Ptr New (const filesystem::path& fileName, FlushFlag flushFlag, BufferFlag bufferedFlag);
    Ptr New (const filesystem::path& fileName, AppendFlag appendFlag, FlushFlag flushFlag, BufferFlag bufferedFlag);
    Ptr New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag, FlushFlag flushFlag, BufferFlag bufferedFlag);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileOutputStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileOutputStream_h_*/

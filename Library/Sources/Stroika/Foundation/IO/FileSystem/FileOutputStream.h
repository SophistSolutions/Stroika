/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileOutputStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileOutputStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Streams/InternallySyncrhonizedOutputStream.h"
#include "../../Streams/OutputStream.h"

#include "../FileAccessMode.h"

#include "FileStream.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   Use Exeuction??xx?? - caller - handler for thread interrupts..
 *
 *      @todo   Add enum BufferFlag { eBuffered, eUnbuffered }; to mk () method, and like we have
 *              with FileOutputStream
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
 *      @todo   Add optional file sharing parameters to stream readers, and maybe file-descriptor CTOR?
 */

namespace Stroika::Foundation::IO::FileSystem {

    using std::byte;

    using Characters::String;

    /**
     *  \par Example Usage
     *      \code
     *      IO::FileSystem::FileOutputStream::New (L"/tmp/foo").Write (Memory::BLOB {0x3});
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *      String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"t.txt";
     *      JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream::New (fileName));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *      TextWriter tw { IO::FileSystem::FileOutputStream::New (L"/tmp/fred.txt") };
     *      tw.PrintF (L"Hello %s\n", L"World");
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
     */
    class FileOutputStream : public Streams::OutputStream<byte>, public FileStream {
    public:
        /**
         *  This flag is used to configure if BinaryOutputStream::Flush will invoke the OS fsync() funciton
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
        static constexpr FlushFlag eToOperatingSystem = FlushFlag::eToOperatingSystem;
        static constexpr FlushFlag eToDisk            = FlushFlag::eToDisk;

    public:
        /**
         *  Default AppendFlag is eStartFromStart (truncation), not eAppend
         */
        enum class AppendFlag {
            eStartFromStart, // aka truncate
            eAppend,

            eDEFAULT = eStartFromStart,

            Stroika_Define_Enum_Bounds (eStartFromStart, eAppend)
        };
        static constexpr AppendFlag eStartFromStart = AppendFlag::eStartFromStart;
        static constexpr AppendFlag eAppend         = AppendFlag::eAppend;

    public:
        enum class BufferFlag {
            eBuffered,
            eUnbuffered,

            eDEFAULT = eUnbuffered,

            Stroika_Define_Enum_Bounds (eBuffered, eUnbuffered)
        };
        static constexpr BufferFlag eBuffered   = BufferFlag::eBuffered;
        static constexpr BufferFlag eUnbuffered = BufferFlag::eUnbuffered;

    public:
        FileOutputStream ()                        = delete;
        FileOutputStream (const FileOutputStream&) = delete;

    public:
        class Ptr;

    public:
        /**
         *  The constructor overload with FileDescriptorType does an 'attach' - taking ownership (and thus later closing) the argument file descriptor (depending on AdoptFDPolicy).
         *
         *  \req fd is a valid file descriptor (for that overload)
         *
         *  \note   We considered having a GetFD () method to retrieve the file descriptor, but that opened up too many
         *          possabilities for bugs (like changing the blocking nature of the IO). If you wish - you can always
         *          open the file descriptor yourself, track it yourself, and do what you will to it and pass it in,
         *          but then the results are 'on you.
         *
         *  \note   The overloads taking no BufferFlag produce a non-buffered stream.
         */
        static Ptr                     New (const String& fileName, FlushFlag flushFlag = FlushFlag::eDEFAULT);
        static Ptr                     New (const String& fileName, AppendFlag appendFlag, FlushFlag flushFlag = FlushFlag::eDEFAULT);
        static Ptr                     New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekableFlag = SeekableFlag::eDEFAULT, FlushFlag flushFlag = FlushFlag::eDEFAULT);
        static Ptr                     New (Execution::InternallySyncrhonized internallySyncrhonized, const String& fileName, FlushFlag flushFlag = FlushFlag::eDEFAULT);
        static Ptr                     New (Execution::InternallySyncrhonized internallySyncrhonized, const String& fileName, AppendFlag appendFlag, FlushFlag flushFlag = FlushFlag::eDEFAULT);
        static Ptr                     New (Execution::InternallySyncrhonized internallySyncrhonized, FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy = AdoptFDPolicy::eDEFAULT, SeekableFlag seekableFlag = SeekableFlag::eDEFAULT, FlushFlag flushFlag = FlushFlag::eDEFAULT);
        static OutputStream<byte>::Ptr New (const String& fileName, FlushFlag flushFlag, BufferFlag bufferedFlag);
        static OutputStream<byte>::Ptr New (const String& fileName, AppendFlag appendFlag, FlushFlag flushFlag, BufferFlag bufferedFlag);
        static OutputStream<byte>::Ptr New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag, FlushFlag flushFlag, BufferFlag bufferedFlag);

    private:
        class Rep_;

    protected:
        /**
         *  Utility to create a Ptr wrapper (to avoid having to subclass the Ptr class and access its protected constructor)
         */
        static Ptr _mkPtr (const shared_ptr<Rep_>& s);

    private:
        using InternalSyncRep_ = Streams::InternallySyncrhonizedOutputStream<byte, FileOutputStream, FileOutputStream::Rep_>;
    };

    /**
     *  Ptr is a copyable smart pointer to a FileOutputStream.
     */
    class FileOutputStream::Ptr : public Streams::OutputStream<byte>::Ptr {
    private:
        using inherited = Streams::OutputStream<byte>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          
         *      \endcode
         */
        Ptr ()                = delete;
        Ptr (const Ptr& from) = default;

    protected:
        Ptr (const shared_ptr<Rep_>& from);

    public:
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;

    private:
        friend class FileOutputStream;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileOutputStream.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileOutputStream_h_*/

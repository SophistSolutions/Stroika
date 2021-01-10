/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_h_
#define _Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_h_ 1

#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#endif

#include <filesystem>

#include "Common.h"

/**
 * TODO:
 *          @todo   Add API variant that allows multiple sub-regions of a file to be mapped.
 *
 *          @todo   Add API controlling if  mapping is readonly or read/write
 *
 *          @todo   Consider adding a writable MemoryMappedFileReader variant
 *
 */

namespace Stroika::Foundation::IO::FileSystem {

    using std::byte;

    /**
     *  MemoryMappedFileReader is a utility to allow memory mapping of a file.
     */
    class MemoryMappedFileReader {
    public:
        MemoryMappedFileReader (const filesystem::path& fileName);
        MemoryMappedFileReader (const MemoryMappedFileReader&) = delete;
        ~MemoryMappedFileReader ();

    public:
        nonvirtual MemoryMappedFileReader& operator= (const MemoryMappedFileReader&) = delete;

    public:
        /**
         */
        nonvirtual const byte* begin () const;

    public:
        /**
         */
        nonvirtual const byte* end () const;

    public:
        /**
         */
        nonvirtual size_t size () const;

    private:
        const byte* fFileDataStart_{nullptr};
        const byte* fFileDataEnd_{nullptr};
#if qPlatform_Windows
        HANDLE fFileHandle_{INVALID_HANDLE_VALUE};
        HANDLE fFileMapping_{INVALID_HANDLE_VALUE};
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MemoryMappedFileReader.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_h_*/

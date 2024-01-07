/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
 */

namespace Stroika::Foundation::IO::FileSystem {

    /**
     *  MemoryMappedFileReader is a utility to allow memory mapping of a file.
     * 
     *  \note - this code doesn't facilitiate create a BLOB from this class, because doing so well requires application specific info
     *        about lifetimes. You can use Memory::BLOB::Adopt (mf.AsSpan<> ()) - but BEWARE - user doing so MUST make sure lifetime
     *        of MemoryMappedFileReader >= that of the produced BLOB (or any derivatives - its shared_rep).
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

    public:
        nonvirtual span<const byte> AsSpan () const;

    public:
        nonvirtual span<const byte> operator() () const;

    private:
        span<const byte> fSpan_;
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

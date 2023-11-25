/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_inl_
#define _Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ******************* IO::FileSystem::MemoryMappedFileReader *********************
     ********************************************************************************
     */
    inline const byte* MemoryMappedFileReader::begin () const
    {
        return fSpan_.data ();
    }
    inline const byte* MemoryMappedFileReader::end () const
    {
        return fSpan_.data () + fSpan_.size ();
    }
    inline size_t MemoryMappedFileReader::size () const
    {
        return fSpan_.size ();
    }
    inline span<const byte> MemoryMappedFileReader::AsSpan() const
    {
        return fSpan_;
    }
    inline span<const byte> MemoryMappedFileReader::operator() () const
    {
        return fSpan_;
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_inl_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
     ******************* FileSystem::MemoryMappedFileReader *************************
     ********************************************************************************
     */
    inline const Byte* MemoryMappedFileReader::begin () const
    {
        return fFileDataStart_;
    }
    inline const Byte* MemoryMappedFileReader::end () const
    {
        return fFileDataEnd_;
    }
    inline size_t MemoryMappedFileReader::size () const
    {
        return fFileDataEnd_ - fFileDataStart_;
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_MemoryMappedFileReader_inl_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileInputStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileInputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem::FileInputStream {

    /*
     ********************************************************************************
     ********************************** FileInputStream *****************************
     ********************************************************************************
     */
    inline Streams::InputStream<byte>::Ptr New (const filesystem::path& fileName, BufferFlag bufferFlag)
    {
        return New (fileName, kSeekableFlag_DEFAULT, bufferFlag);
    }
    inline Streams::InputStream<byte>::Ptr New (FileDescriptorType fd, BufferFlag bufferFlag)
    {
        return New (fd, AdoptFDPolicy::eDEFAULT, SeekableFlag::eSeekable, bufferFlag);
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_FileInputStream_inl_*/

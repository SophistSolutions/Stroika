/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileInputStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileInputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ********************************** FileInputStream *****************************
     ********************************************************************************
     */
    inline Streams::InputStream<byte>::Ptr FileInputStream::New (const String& fileName, BufferFlag bufferFlag)
    {
        return New (fileName, SeekableFlag::eDEFAULT, bufferFlag);
    }
    inline Streams::InputStream<byte>::Ptr FileInputStream::New (FileDescriptorType fd, BufferFlag bufferFlag)
    {
        return New (fd, AdoptFDPolicy::eDEFAULT, SeekableFlag::eSeekable, bufferFlag);
    }
    inline auto FileInputStream::_mkPtr (const shared_ptr<Rep_>& s) -> Ptr
    {
        return Ptr{s};
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_FileInputStream_inl_*/

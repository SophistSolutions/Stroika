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
namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {

                /*
                 ********************************************************************************
                 ********************************** FileInputStream *****************************
                 ********************************************************************************
                 */
                inline Streams::InputStream<Memory::Byte>::Ptr FileInputStream::New (const String& fileName, BufferFlag bufferFlag)
                {
                    return New (fileName, SeekableFlag::eDEFAULT, bufferFlag);
                }
                inline Streams::InputStream<Memory::Byte>::Ptr FileInputStream::New (FileDescriptorType fd, BufferFlag bufferFlag)
                {
                    return New (fd, AdoptFDPolicy::eDEFAULT, SeekableFlag::eSeekable, bufferFlag);
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_FileSystem_FileInputStream_inl_*/

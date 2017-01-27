/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileInputStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileInputStream_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                /*
                 ********************************************************************************
                 ********************************** BinaryInputStream ***************************
                 ********************************************************************************
                 */
                inline  Streams::InputStream<Memory::Byte>   FileInputStream::mk (const String& fileName, BufferFlag bufferFlag)
                {
                    return mk (fileName, SeekableFlag::eSeekable, bufferFlag);
                }
                inline  Streams::InputStream<Memory::Byte>   FileInputStream::mk (FileDescriptorType fd, BufferFlag bufferFlag)
                {
                    return mk (fd, AdoptFDPolicy::eCloseOnDestruction, SeekableFlag::eSeekable, bufferFlag);
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_FileInputStream_inl_*/

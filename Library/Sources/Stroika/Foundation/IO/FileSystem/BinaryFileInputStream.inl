/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_inl_
#define _Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_inl_    1


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
                inline  Streams::BinaryInputStream   BinaryFileInputStream::mk (const String& fileName, BufferFlag bufferFlag)
                {
                    return mk (fileName, eSeekable, bufferFlag);
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_BinaryFileInputStream_inl_*/

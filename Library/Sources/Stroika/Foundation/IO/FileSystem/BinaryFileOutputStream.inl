/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_BinaryFileOutputStream_inl_
#define _Stroika_Foundation_IO_FileSystem_BinaryFileOutputStream_inl_    1


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
                 ***************************** BinaryFileOutputStream ***************************
                 ********************************************************************************
                 */
                inline  Streams::BinaryOutputStream   BinaryFileOutputStream::mk (const String& fileName, FlushFlag flushFlag)
                {
                    return BinaryFileOutputStream (fileName, flushFlag);
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_BinaryFileOutputStream_inl_*/

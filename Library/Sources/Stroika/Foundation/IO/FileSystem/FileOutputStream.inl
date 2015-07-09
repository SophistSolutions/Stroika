/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_    1


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
                 ***************************** FileOutputStream ***************************
                 ********************************************************************************
                 */
                inline  Streams::OutputStream<Memory::Byte>   FileOutputStream::mk (const String& fileName, FlushFlag flushFlag)
                {
                    return FileOutputStream (fileName, flushFlag);
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_*/

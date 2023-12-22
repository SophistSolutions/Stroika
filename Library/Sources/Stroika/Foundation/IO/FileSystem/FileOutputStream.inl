/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Streams/BufferedOutputStream.h"

namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ******************************* FileOutputStream *******************************
     ********************************************************************************
     */
    inline Streams::OutputStream<byte>::Ptr FileOutputStream::New (const filesystem::path& fileName, FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream::New<byte> (FileOutputStream::New (fileName, flushFlag));
        }
        else {
            return FileOutputStream::New (fileName, flushFlag);
        }
    }
    inline Streams::OutputStream<byte>::Ptr FileOutputStream::New (const filesystem::path& fileName, AppendFlag appendFlag,
                                                                   FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream::New<byte> (FileOutputStream::New (fileName, appendFlag, flushFlag));
        }
        else {
            return FileOutputStream::New (fileName, appendFlag, flushFlag);
        }
    }
    inline Streams::OutputStream<byte>::Ptr FileOutputStream::New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy,
                                                                   SeekableFlag seekableFlag, FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream::New<byte> (FileOutputStream::New (fd, adoptFDPolicy, seekableFlag, flushFlag));
        }
        else {
            return FileOutputStream::New (fd, adoptFDPolicy, seekableFlag, flushFlag);
        }
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_*/

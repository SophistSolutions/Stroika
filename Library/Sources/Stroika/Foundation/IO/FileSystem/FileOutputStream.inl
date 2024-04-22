/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_
#define _Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Stroika/Foundation/Streams/BufferedOutputStream.h"

namespace Stroika::Foundation::IO::FileSystem::FileOutputStream {

    /*
     ********************************************************************************
     ******************************* FileOutputStream *******************************
     ********************************************************************************
     */
    inline Streams::OutputStream::Ptr<byte> New (const filesystem::path& fileName, FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream::New<byte> (FileOutputStream::New (fileName, flushFlag));
        }
        else {
            return New (fileName, flushFlag);
        }
    }
    inline Streams::OutputStream::Ptr<byte> New (const filesystem::path& fileName, AppendFlag appendFlag, FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream::New<byte> (New (fileName, appendFlag, flushFlag));
        }
        else {
            return New (fileName, appendFlag, flushFlag);
        }
    }
    inline Streams::OutputStream::Ptr<byte> New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag,
                                                 FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream::New<byte> (FileOutputStream::New (fd, adoptFDPolicy, seekableFlag, flushFlag));
        }
        else {
            return New (fd, adoptFDPolicy, seekableFlag, flushFlag);
        }
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_*/

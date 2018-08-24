/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    inline Streams::OutputStream<byte>::Ptr FileOutputStream::New (const String& fileName, FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream<byte>::New (FileOutputStream::New (fileName, flushFlag));
        }
        else {
            return FileOutputStream::New (fileName, flushFlag);
        }
    }
    inline Streams::OutputStream<byte>::Ptr FileOutputStream::New (const String& fileName, AppendFlag appendFlag, FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream<byte>::New (FileOutputStream::New (fileName, appendFlag, flushFlag));
        }
        else {
            return FileOutputStream::New (fileName, appendFlag, flushFlag);
        }
    }
    inline Streams::OutputStream<byte>::Ptr FileOutputStream::New (FileDescriptorType fd, AdoptFDPolicy adoptFDPolicy, SeekableFlag seekableFlag, FlushFlag flushFlag, BufferFlag bufferedFlag)
    {
        if (bufferedFlag == eBuffered) {
            return Streams::BufferedOutputStream<byte>::New (FileOutputStream::New (fd, adoptFDPolicy, seekableFlag, flushFlag));
        }
        else {
            return FileOutputStream::New (fd, adoptFDPolicy, seekableFlag, flushFlag);
        }
    }
    inline auto FileOutputStream::_mkPtr (const shared_ptr<Rep_>& s) -> Ptr
    {
        return Ptr{s};
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_FileOutputStream_inl_*/

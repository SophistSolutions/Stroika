/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::FileSystem::FileInputStream {

    /*
     ********************************************************************************
     ********************************** FileInputStream *****************************
     ********************************************************************************
     */
    inline Streams::InputStream::Ptr<byte> New (const filesystem::path& fileName, BufferFlag bufferFlag)
    {
        return New (fileName, kSeekableFlag_DEFAULT, bufferFlag);
    }

    /// /////////////DEPRECATED
    [[deprecated ("Since Stroika v3.0d6 - need adoptfdpolicy flag")]] inline Ptr New (FileDescriptorType fd, BufferFlag bufferFlag)
    {
        return New (fd, AdoptFDPolicy::eCloseOnDestruction, SeekableFlag::eSeekable, bufferFlag);
    }

}

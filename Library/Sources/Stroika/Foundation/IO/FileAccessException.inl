/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessException_inl_
#define _Stroika_Foundation_IO_FileAccessException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO {

    /*
     ********************************************************************************
     ***************************** IO::FileAccessException **************************
     ********************************************************************************
     */
    inline optional<String> FileAccessException::GetFileName () const
    {
        return fFileName_;
    }
    inline optional<FileAccessMode> FileAccessException::GetFileAccessMode () const
    {
        return fFileAccessMode_;
    }

}

#endif /*_Stroika_Foundation_IO_FileAccessException_inl_*/

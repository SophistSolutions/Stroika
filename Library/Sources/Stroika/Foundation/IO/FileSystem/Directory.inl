/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Directory_inl_
#define _Stroika_Foundation_IO_FileSystem_Directory_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PathName.h"

namespace Stroika::Foundation::IO::FileSystem {

    DISABLE_COMPILER_MSC_WARNING_START (4996) // class deprecated but still need to implement it
    template <>
    inline String Directory::As () const
    {
        return FromPath (fFileFullPath_);
    }
    template <>
    inline wstring Directory::As () const
    {
        return FromPath (fFileFullPath_).As<wstring> ();
    }
    inline SDKString Directory::AsSDKString () const
    {
        return FromPath (fFileFullPath_).AsSDKString ();
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)

}

#endif /*_Stroika_Foundation_IO_FileSystem_Directory_inl_*/

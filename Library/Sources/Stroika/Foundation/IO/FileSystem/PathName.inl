/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_PathName_inl_
#define _Stroika_Foundation_IO_FileSystem_PathName_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ***************************** IO::FileSystem::ToPath ***************************
     ********************************************************************************
     */
    inline path ToPath (const String& p)
    {
        return path{p.As<wstring> ()};
    }

    /*
     ********************************************************************************
     **************************** IO::FileSystem::FromPath **************************
     ********************************************************************************
     */
    inline String FromPath (const path& p)
    {
        return p.wstring ();
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_PathName_inl_*/

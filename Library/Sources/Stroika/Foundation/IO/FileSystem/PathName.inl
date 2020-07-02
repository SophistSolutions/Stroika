/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
    inline filesystem::path ToPath (const String& p)
    {
        return filesystem::path{p.As<wstring> ()};
    }
    inline optional<filesystem::path> ToPath (const optional<String>& p)
    {
        if (p) {
            return ToPath (*p);
        }
        return nullopt;
    }

    /*
     ********************************************************************************
     **************************** IO::FileSystem::FromPath **************************
     ********************************************************************************
     */
    inline String FromPath (const filesystem::path& p)
    {
        return p.wstring ();
    }
    inline optional<String> FromPath (const optional<filesystem::path>& p)
    {
        if (p) {
            return FromPath (*p);
        }
        return nullopt;
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_PathName_inl_*/

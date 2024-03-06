/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#if qStroika_Foundation_IO_FileSystem_PathName_AutoMapMSYSAndCygwin
        filesystem::path r = filesystem::path{p.As<wstring> ()};
        if (r.is_absolute ()) {
            if (r.root_name ().empty () and r.root_path () == "/cygdrive") {
                /*
                 * This case is easy, and probably pretty safe.
                 */
                r = r.relative_path ();
            }
            if (r.root_name ().empty () and r.root_path ().native ().length () == 2) {
                /*
                 * /a or /b etc... assume its a drive-letter (MSYS)
                 */
                r = r.relative_path ();
            }
        }
        return r;
#else
        return filesystem::path{p.As<wstring> ()};
#endif
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

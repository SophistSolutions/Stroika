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
        // CYGWIN creates paths like /cygdrive/c/folder for c:/folder
        // MSYS creates paths like /c/folder for c:/folder
        static const String kMSYSDrivePrefix_ = "/"sv;
        static const String kCygrivePrefix_   = "/cygdrive/"sv;
        if (p.StartsWith (kCygrivePrefix_)) {
            String ss = p.SubString (kCygrivePrefix_.length ());
            if (ss.length () > 1 and ss[0].IsASCII () and ss[0].IsAlphabetic () and ss[1] == '/') {
                wstring w = ss.As<wstring> (); // now map c/folder to c:/folder
                w.insert (w.begin () + 1, ':');
                return filesystem::path{w};
            }
        }
        if (p.StartsWith (kMSYSDrivePrefix_)) {
            String ss = p.SubString (kMSYSDrivePrefix_.length ());
            if (ss.length () > 1 and ss[0].IsASCII () and ss[0].IsAlphabetic () and ss[1] == '/') {
                wstring w = ss.As<wstring> (); // now map c/folder to c:/folder
                w.insert (w.begin () + 1, ':');
                return filesystem::path{w};
            }
        }
#endif
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

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#elif   qPlatform_POSIX
#include    <fstream>
#endif

#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"

#if     qPlatform_POSIX
#include    "../DataExchange/INI/Reader.h"
#include    "../Execution/ProcessRunner.h"
#include    "../Streams/iostream/FStreamSupport.h"
#endif


#include    "Platform.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;

using   Characters::String_Constant;






Platform    Platform::Get ()
{
    static  Platform    kCachedResult_ = []() ->Platform {
        Platform    tmp;
#if     qPlatform_Windows
        tmp.TokenName = String_Constant (L"Windows");
        /*
         *  Microslop declares this deprecated, but then fails to provide a reasonable alternative.
         *
         *  Sigh.
         *
         *  http://msdn.microsoft.com/en-us/library/windows/desktop/ms724429(v=vs.85).aspx -  GetFileVersionInfo (kernel32.dll)
         *  is a painful, and stupid alternative.
         */
        DISABLE_COMPILER_MSC_WARNING_START(4996)

        OSVERSIONINFOEX   osvi;
        memset(&osvi, 0, sizeof (osvi));
        osvi.dwOSVersionInfoSize = sizeof (osvi);
        Verify (::GetVersionEx (reinterpret_cast<LPOSVERSIONINFO> (&osvi)));
        DISABLE_COMPILER_MSC_WARNING_END(4996)
        if (osvi.dwMajorVersion == 6)
        {
            if (osvi.dwMinorVersion == 0) {
                if (osvi.wProductType == VER_NT_WORKSTATION )
                    tmp.ShortPrettyName = L"Windows Vista";
                else
                    tmp.ShortPrettyName = L"Windows Server 2008";
            }
            else if (osvi.dwMinorVersion == 1) {
                if (osvi.wProductType == VER_NT_WORKSTATION)
                    tmp.ShortPrettyName = L"Windows 7";
                else
                    tmp.ShortPrettyName = L"Windows Server 2008 R2";
            }
            else if (osvi.dwMinorVersion == 2) {
                if (osvi.wProductType == VER_NT_WORKSTATION)
                    tmp.ShortPrettyName = L"Windows 8";
                else
                    tmp.ShortPrettyName = L"Windows Server 2012";
            }
            else if (osvi.dwMinorVersion == 3) {
                if (osvi.wProductType == VER_NT_WORKSTATION)
                    tmp.ShortPrettyName = L"Windows 8.1";
            }
        }
        if (tmp.ShortPrettyName.empty ())
        {
            tmp.ShortPrettyName = Characters::Format (L"Windows %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        }
        tmp.PrettyNameWithMajorVersion = tmp.ShortPrettyName;
        tmp.MajorMinorVersionString = Characters::Format (L"%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        tmp.RFC1945CompatProductTokenWithVersion = Characters::Format (L"Windows/%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
#elif   qPlatform_POSIX
        tmp.TokenName = String_Constant (L"Unix");
        try {
            tmp.TokenName = Execution::ProcessRunner (SDKSTR ("uname")).Run ().Trim ();
        }
        catch (...)
        {
            DbgTrace ("Failure running uname");
        }
        try {
            ifstream s;
            Streams::iostream::OpenInputFileStream (&s, L"/etc/os-release");
            DataExchange::INI::Profile p = DataExchange::INI::Reader ().ReadProfile (s);
            tmp.ShortPrettyName = p.fUnnamedSection.fProperties.LookupValue (L"NAME");
            tmp.PrettyNameWithMajorVersion = p.fUnnamedSection.fProperties.LookupValue (L"PRETTY_NAME");
        }
        catch (...)
        {
            DbgTrace ("Failure reading /etc/os-release");
        }
        if (tmp.ShortPrettyName.empty ())
        {
            tmp.ShortPrettyName = tmp.TokenName;
        }
        if (tmp.PrettyNameWithMajorVersion.empty ())
        {
            tmp.PrettyNameWithMajorVersion = tmp.ShortPrettyName;
        }
        if (tmp.RFC1945CompatProductTokenWithVersion.empty ())
        {
            tmp.RFC1945CompatProductTokenWithVersion = tmp.ShortPrettyName.Trim ().ReplaceAll (L" ", L"-");
            if (not tmp.MajorMinorVersionString.empty ()) {
                tmp.RFC1945CompatProductTokenWithVersion += L"/" + tmp.MajorMinorVersionString;
            }
        }
#else
        AssertNotImplemented ();
#endif
        return tmp;
    } ();
    return kCachedResult_;
}

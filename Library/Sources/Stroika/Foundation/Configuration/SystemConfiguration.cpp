/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <unistd.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#endif

#include	<thread>

#include    "../Characters/SDKString.h"
#if     qPlatform_POSIX
#include    "../Execution/ErrNoException.h"
#elif   qPlatform_Windows
#include    "../Execution/Platform/Windows/Exception.h"
#endif
#include    "../Memory/SmallStackBuffer.h"

#include    "SystemConfiguration.h"






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





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;



using   Characters::String_Constant;

using   Characters::SDKChar;






/*
 ********************************************************************************
 *************** Configuration::GetSystemConfiguration_CPU **********************
 ********************************************************************************
 */
SystemConfiguration::CPU Configuration::GetSystemConfiguration_CPU ()
{
    using CPU = SystemConfiguration::CPU;
    CPU result;
	result.fNumberOfLogicalCores = std::thread::hardware_concurrency();
    return result;
}


/*
 ********************************************************************************
 ************** Configuration::GetSystemConfiguration_Memory ********************
 ********************************************************************************
 */
SystemConfiguration::Memory Configuration::GetSystemConfiguration_Memory ()
{
    using Memory = SystemConfiguration::Memory;

    Memory  result;
#if     qPlatform_POSIX
	result.fTotalPhysicalRAM = sysconf(_SC_PHYS_PAGES)) * sysconf(_SC_PAGESIZE);
#endif
    return result;
}




namespace {


    SystemConfiguration::OperatingSystem    GetPlatform_ ()
    {
        using   OperatingSystem =   SystemConfiguration::OperatingSystem;
        static  OperatingSystem    kCachedResult_ = []() ->OperatingSystem {
            OperatingSystem    tmp;
#if     qPlatform_Windows
            tmp.fTokenName = String_Constant (L"Windows");
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
                        tmp.fShortPrettyName = L"Windows Vista";
                    else
                        tmp.fShortPrettyName = L"Windows Server 2008";
                }
                else if (osvi.dwMinorVersion == 1) {
                    if (osvi.wProductType == VER_NT_WORKSTATION)
                        tmp.fShortPrettyName = L"Windows 7";
                    else
                        tmp.fShortPrettyName = L"Windows Server 2008 R2";
                }
                else if (osvi.dwMinorVersion == 2) {
                    if (osvi.wProductType == VER_NT_WORKSTATION)
                        tmp.fShortPrettyName = L"Windows 8";
                    else
                        tmp.fShortPrettyName = L"Windows Server 2012";
                }
                else if (osvi.dwMinorVersion == 3) {
                    if (osvi.wProductType == VER_NT_WORKSTATION)
                        tmp.fShortPrettyName = L"Windows 8.1";
                }
            }
            if (tmp.fShortPrettyName.empty ())
            {
                tmp.fShortPrettyName = Characters::Format (L"Windows %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
            }
            tmp.fPrettyNameWithMajorVersion = tmp.fShortPrettyName;
            tmp.fMajorMinorVersionString = Characters::Format (L"%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
            tmp.fRFC1945CompatProductTokenWithVersion = Characters::Format (L"Windows/%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
#elif   qPlatform_POSIX
            tmp.fTokenName = String_Constant (L"Unix");
            try {
                tmp.fTokenName = Execution::ProcessRunner (SDKSTR ("uname")).Run (String ()).Trim ();
            }
            catch (...)
            {
                DbgTrace ("Failure running uname");
            }
            try {
                ifstream s;
                Streams::iostream::OpenInputFileStream (&s, L"/etc/os-release");
                DataExchange::INI::Profile p = DataExchange::INI::Reader ().ReadProfile (s);
                tmp.fShortPrettyName = p.fUnnamedSection.fProperties.LookupValue (L"NAME");
                tmp.fPrettyNameWithMajorVersion = p.fUnnamedSection.fProperties.LookupValue (L"PRETTY_NAME");
            }
            catch (...)
            {
                DbgTrace ("Failure reading /etc/os-release");
            }
            if (tmp.fShortPrettyName.empty ())
            {
                tmp.fShortPrettyName = tmp.TokenName;
            }
            if (tmp.fPrettyNameWithMajorVersion.empty ())
            {
                tmp.fPrettyNameWithMajorVersion = tmp.ShortPrettyName;
            }
            if (tmp.fRFC1945CompatProductTokenWithVersion.empty ())
            {
                tmp.fRFC1945CompatProductTokenWithVersion = tmp.fShortPrettyName.Trim ().ReplaceAll (L" ", L"-");
                if (not tmp.fMajorMinorVersionString.empty ()) {
                    tmp.fRFC1945CompatProductTokenWithVersion += L"/" + tmp.fMajorMinorVersionString;
                }
            }
#else
            AssertNotImplemented ();
#endif
            return tmp;
        } ();
        return kCachedResult_;
    }
}


/*
 ********************************************************************************
 ***************** GetSystemConfiguration_ComputerNames *************************
 ********************************************************************************
 */
#if     0 && qPlatform_POSIX
// ALTERNATE APPROACH TO CONSIDER
string  name;
{
    struct addrinfo* res;
    struct addrinfo hints;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    int e = getaddrinfo(nullptr, nullptr, &hints, &res);
    if (e != 0) {
        //printf("failure %s\n", gai_strerror (e));
        return String ();
    }
    int sock = -1;
    for (struct addrinfo* r = res; r != NULL; r = r->ai_next) {
        name = r->ai_canonname ;
        break;
    }
    freeaddrinfo(res);
}
return String::FromSDKString (name);
#endif
SystemConfiguration::ComputerNames Configuration::GetSystemConfiguration_ComputerNames ()
{
    using ComputerNames = SystemConfiguration::ComputerNames;
    ComputerNames   result;
#if     qPlatform_POSIX
    char    nameBuf[1024];
    Execution::ThrowErrNoIfNegative (gethostname (nameBuf, NEltsOf (nameBuf)));
    nameBuf[NEltsOf (nameBuf) - 1] = '\0';  // http://linux.die.net/man/2/gethostname says not necessarily nul-terminated
    result.fHostname = String::FromNarrowSDKString (nameBuf);
#elif   qPlatform_Windows
    constexpr   COMPUTER_NAME_FORMAT    kUseNameFormat_ = ComputerNameNetBIOS;  // total WAG -- LGP 2014-10-10
    DWORD   dwSize = 0;
    (void) ::GetComputerNameEx (kUseNameFormat_, nullptr, &dwSize);
    Memory::SmallStackBuffer<SDKChar> buf(dwSize);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::GetComputerNameEx (kUseNameFormat_, buf, &dwSize));
    result.fHostname = String::FromSDKString (buf);
#else
    AssertNotImplemented ();
#endif
    return result;
}


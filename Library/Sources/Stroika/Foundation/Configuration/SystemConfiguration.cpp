/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <thread>

#if     qPlatform_POSIX
#include    <unistd.h>
#include    <fstream>
#include    <sys/sysinfo.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Characters/SDKString.h"
#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"
#include    "../Characters/String2Int.h"
#include    "../Containers/Set.h"
#if     qPlatform_POSIX
#include    "../Execution/ErrNoException.h"
#elif   qPlatform_Windows
#include    "../Execution/Platform/Windows/Exception.h"
#endif
#include    "../Memory/SmallStackBuffer.h"
#include    "../IO/FileSystem/FileInputStream.h"
#include    "../Streams/MemoryStream.h"
#include    "../Streams/TextReader.h"

#include    "SystemConfiguration.h"



#if     qPlatform_POSIX
#include    "../DataExchange/INI/Reader.h"
#include    "../Execution/ProcessRunner.h"
#include    "../Streams/iostream/FStreamSupport.h"
#endif





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Time;


using   Characters::String_Constant;
using   Characters::SDKChar;


// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1






/*
 ********************************************************************************
 ***************** Configuration::SystemConfiguration::CPU **********************
 ********************************************************************************
 */
unsigned int    SystemConfiguration::CPU::GetNumberOfSockets () const
{
    Set<unsigned int>   socketIds;
    for (auto i : fCores) {
        socketIds.Add (i.fSocketID);
    }
    return static_cast<unsigned int> (socketIds.size ());
}





/*
 ********************************************************************************
 ************* Configuration::GetSystemConfiguration_BootInformation ************
 ********************************************************************************
 */

SystemConfiguration::BootInformation Configuration::GetSystemConfiguration_BootInformation ()
{
    SystemConfiguration::BootInformation    result;
#if     qPlatform_POSIX
    struct  sysinfo     info;
    ::sysinfo (&info);
    result.fBootedAt = DateTime::Now ().AddSeconds (-info.uptime);
#elif   qPlatform_Windows
    // ::GetTickCount () is defined to return #seconds since boot
#if     _WIN32_WINNT >= 0x0600
    result.fBootedAt = DateTime::Now ().AddSeconds (-static_cast<int> (::GetTickCount64 () / 1000));
#else
    result.fBootedAt = DateTime::Now ().AddSeconds (-static_cast<int> (::GetTickCount () / 1000));
#endif
#else
    AssertNotImplemented ();
#endif
    return result;
}




/*
 ********************************************************************************
 *************** Configuration::GetSystemConfiguration_CPU **********************
 ********************************************************************************
 */
SystemConfiguration::CPU Configuration::GetSystemConfiguration_CPU ()
{
    using CPU = SystemConfiguration::CPU;
    CPU result;

#if     qPlatform_POSIX
    {
        using   Streams::TextReader;
        using   IO::FileSystem::FileInputStream;
        using   Characters::String2Int;
        const   String_Constant kProcCPUInfoFileName_ { L"/proc/cpuinfo" };

        CPU::CoreDetails    coreDetails;
        // Note - /procfs files always unseekable
        for (String line : TextReader (FileInputStream::mk (kProcCPUInfoFileName_, FileInputStream::eNotSeekable)).ReadLines ()) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"***in Configuration::GetSystemConfiguration_CPU capture_ line=%s", line.c_str ());
#endif
            static  const   String_Constant kModelNameLabel_ { L"model name	: " };
            //static    const   String_Constant kProcessorIDLabel_ { L"processor    : " };
            static  const   String_Constant kSocketIDLabel_ { L"physical id	: " };          // a bit of a guess?
            if (line.StartsWith (kModelNameLabel_)) {
                coreDetails.fModelName = line.SubString (kModelNameLabel_.length ()).Trim ();
            }
            else if (line.StartsWith (kSocketIDLabel_)) {
                unsigned int socketID = String2Int<unsigned int> (line.SubString (kSocketIDLabel_.length ()).Trim ());
                coreDetails.fSocketID = socketID;
            }
            if (line.Trim ().empty ()) {
                // ends each socket
                result.fCores.Append (coreDetails);
                coreDetails = CPU::CoreDetails ();
            }
        }
        if (coreDetails.fSocketID != 0 or not coreDetails.fModelName.empty ()) {
            result.fCores.Append (coreDetails);
        }
    }
#elif   qPlatform_Windows
    SYSTEM_INFO sysInfo;        // GetNativeSystemInfo cannot fail so no need to initialize data
    ::GetNativeSystemInfo (&sysInfo);
    //unclear if this is count of logical or physical cores, or how to compute the other.
    //@todo - fix as above for POSIX... maybe ask Sterl? But for now KISS
    //
    // Can use https://msdn.microsoft.com/en-us/library/hskdteyh%28v=vs.90%29.aspx?f=255&MSPPError=-2147217396
    //  __cpuid
    // to find this information (at least modelname string.
    //

    static  const   String  kProcessorType_ = [] () {
        int CPUInfo[4] = { -1};
        char CPUBrandString[0x40];
        // Get the information associated with each extended ID.
        __cpuid (CPUInfo, 0x80000000);
        uint32_t nExIds = CPUInfo[0];
        for (uint32_t i = 0x80000000; i <= nExIds; ++i)  {
            __cpuid (CPUInfo, i);
            // Interpret CPU brand string
            if  (i == 0x80000002)
                memcpy (CPUBrandString, CPUInfo, sizeof(CPUInfo));
            else if  (i == 0x80000003)
                memcpy (CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
            else if  (i == 0x80000004)
                memcpy (CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
        return String::FromAscii (CPUBrandString);
    } ();

    for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors; ++i) {
        // @todo understand if MSFT compiler bug or my confusion that since we have object wtih initializers why cannot use
        // aggregate uninform initialization?
        CPU::CoreDetails    tmp;
        tmp.fSocketID = static_cast<unsigned int> (i);
        tmp.fModelName = kProcessorType_;
        result.fCores.Append (tmp);
        //result.fCores.Append (CPU::CoreDetails { static_cast<unsigned int> (i), kProcessorType_ });
    }
#endif

    return result;
}


/*
 ********************************************************************************
 ************** Configuration::GetSystemConfiguration_Memory ********************
 ********************************************************************************
 */
SystemConfiguration::Memory Configuration::GetSystemConfiguration_Memory ()
{
    using   Memory = SystemConfiguration::Memory;
    Memory  result;
#if     qPlatform_POSIX
    result.fPageSize = ::sysconf (_SC_PAGESIZE);
    result.fTotalPhysicalRAM = ::sysconf (_SC_PHYS_PAGES) * result.fPageSize;
#elif   qPlatform_Windows
    SYSTEM_INFO sysInfo;
    ::GetNativeSystemInfo (&sysInfo);
    result.fPageSize = sysInfo.dwPageSize;

    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof (memStatus);
    Verify (::GlobalMemoryStatusEx (&memStatus));
    result.fTotalPhysicalRAM = memStatus.ullTotalPhys;
    result.fTotalVirtualRAM = memStatus.ullTotalVirtual;
#endif
    return result;
}


/*
 ********************************************************************************
 ******** Configuration::GetSystemConfiguration_OperatingSystem *****************
 ********************************************************************************
 */
#if   qPlatform_Windows
namespace {
    // @todo - Cleanup and better integrate with below code
    wstring GetWinOSDisplayString_ ()
    {
        typedef void (WINAPI * PGNSI)(LPSYSTEM_INFO);
        typedef BOOL (WINAPI * PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

        wstring result;

        OSVERSIONINFOEX osvi;
        ::ZeroMemory (&osvi, sizeof(osvi));
        osvi.dwOSVersionInfoSize = sizeof(osvi);
        if (::GetVersionEx ((OSVERSIONINFO*) &osvi)) {
            SYSTEM_INFO si;
            ::ZeroMemory (&si, sizeof(si));

            // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
            PGNSI pGNSI = (PGNSI) GetProcAddress (::GetModuleHandle (TEXT("kernel32.dll")), "GetNativeSystemInfo");
            if (pGNSI != nullptr) {
                pGNSI (&si);
            }
            else {
                GetSystemInfo (&si);
            }

            if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT and osvi.dwMajorVersion > 4) {
                result = L"Microsoft ";
                if (osvi.dwMajorVersion == 6) {
                    if (osvi.dwMinorVersion == 0)  {
                        result += (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows Vista " : L"Windows Server 2008 ";
                    }
                    else if (osvi.dwMinorVersion == 1 )  {
                        result += (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 7 " : L"Windows Server 2008 R2 ";
                    }
                    else if (osvi.dwMinorVersion == 2 )  {
                        result += (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 8 " : L"Windows Server 2012 ";
                    }
                    else if (osvi.dwMinorVersion == 3)  {
                        result += (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 8.1 " : L"Windows Server 2012 R2 ";
                    }

                    PGPI pGPI = (PGPI) GetProcAddress (GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
                    DWORD dwType = 0;
                    pGPI (osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

                    switch (dwType) {
                        case PRODUCT_ULTIMATE:
                            result += L"Ultimate Edition";
                            break;
                        case PRODUCT_PROFESSIONAL:
                            result += L"Professional";
                            break;
                        case PRODUCT_HOME_PREMIUM:
                            result += L"Home Premium Edition";
                            break;
                        case PRODUCT_HOME_BASIC:
                            result += L"Home Basic Edition";
                            break;
                        case PRODUCT_ENTERPRISE:
                            result += L"Enterprise Edition";
                            break;
                        case PRODUCT_BUSINESS:
                            result += L"Business Edition";
                            break;
                        case PRODUCT_STARTER:
                            result += L"Starter Edition";
                            break;
                        case PRODUCT_CLUSTER_SERVER:
                            result += L"Cluster Server Edition";
                            break;
                        case PRODUCT_DATACENTER_SERVER:
                            result += L"Datacenter Edition";
                            break;
                        case PRODUCT_DATACENTER_SERVER_CORE:
                            result += L"Datacenter Edition (core installation)";
                            break;
                        case PRODUCT_ENTERPRISE_SERVER:
                            result += L"Enterprise Edition";
                            break;
                        case PRODUCT_ENTERPRISE_SERVER_CORE:
                            result += L"Enterprise Edition (core installation)";
                            break;
                        case PRODUCT_ENTERPRISE_SERVER_IA64:
                            result += L"Enterprise Edition for Itanium-based Systems";
                            break;
                        case PRODUCT_SMALLBUSINESS_SERVER:
                            result += L"Small Business Server";
                            break;
                        case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
                            result += L"Small Business Server Premium Edition";
                            break;
                        case PRODUCT_STANDARD_SERVER:
                            result += L"Standard Edition";
                            break;
                        case PRODUCT_STANDARD_SERVER_CORE:
                            result += L"Standard Edition (core installation)";
                            break;
                        case PRODUCT_WEB_SERVER:
                            result += L"Web Server Edition";
                            break;
                    }
                }
                else if ( osvi.dwMajorVersion == 5 and osvi.dwMinorVersion == 0 )  {
                    result += L"Windows 2000 ";
                    if (osvi.wProductType == VER_NT_WORKSTATION) {
                        result += L"Professional";
                    }
                    else  {
                        if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                            result += L"Datacenter Server";
                        }
                        else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                            result += L"Advanced Server";
                        }
                        else {
                            result += L"Server";
                        }
                    }
                }
                else if (osvi.dwMajorVersion == 5 and osvi.dwMinorVersion == 1)  {
                    result += L"Windows XP ";
                    result += (osvi.wSuiteMask & VER_SUITE_PERSONAL) ? L"Home Edition" :  L"Professional";
                }
                else if (osvi.dwMajorVersion == 5 and osvi.dwMinorVersion == 2)  {
                    if (::GetSystemMetrics (SM_SERVERR2)) {
                        result += L"Windows Server 2003 R2, ";
                    }
                    else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER) {
                        result += L"Windows Storage Server 2003";
                    }
                    else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER) {
                        result += L"Windows Home Server";
                    }
                    else if (osvi.wProductType == VER_NT_WORKSTATION and si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
                        result += L"Windows XP Professional x64 Edition";
                    }
                    else {
                        result += L"Windows Server 2003, ";
                    }

                    // Test for the server type.
                    if (osvi.wProductType != VER_NT_WORKSTATION)  {
                        if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
                            if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                                result += L"Datacenter Edition for Itanium-based Systems";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                                result += L"Enterprise Edition for Itanium-based Systems";
                            }
                        }
                        else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ) {
                            if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                                result += L"Datacenter x64 Edition";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE ) {
                                result += L"Enterprise x64 Edition";
                            }
                            else {
                                result += L"Standard x64 Edition";
                            }
                        }
                        else {
                            if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER) {
                                result += L"Compute Cluster Edition";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                                result += L"Datacenter Edition";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                                result += L"Enterprise Edition";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_BLADE) {
                                result += L"Web Edition";
                            }
                            else {
                                result += L"Standard Edition";
                            }
                        }
                    }
                }

                wchar_t buf[80];
                wsprintf (buf, L" (build %d)",  osvi.dwBuildNumber);
                result += buf;

                if (osvi.dwMajorVersion >= 6) {
                    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
                        result += L", 64-bit";
                    }
                    else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
                        result += L", 32-bit";
                    }
                }
            }
        }
        return result;
    }
}
#endif

SystemConfiguration::OperatingSystem    Configuration::GetSystemConfiguration_OperatingSystem ()
{
    using   OperatingSystem =   SystemConfiguration::OperatingSystem;
    static  const   OperatingSystem    kCachedResult_ = []() ->OperatingSystem {
        OperatingSystem    tmp;
#if     qPlatform_POSIX
        tmp.fTokenName = String_Constant (L"Unix");
        try {
            tmp.fTokenName = Execution::ProcessRunner (L"uname").Run (String ()).Trim ();
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
            try {
                String n = Streams::TextReader (IO::FileSystem::FileInputStream::mk (String_Constant (L"/etc/centos-release"))).ReadAll ().Trim ();
                tmp.fShortPrettyName = L"Centos";
                tmp.fPrettyNameWithMajorVersion = n;
            }
            catch (...) {
                DbgTrace ("Failure reading /etc/centos-release");
            }
        }
        if (tmp.fShortPrettyName.empty ())
        {
            try {
                String n = Streams::TextReader (IO::FileSystem::FileInputStream::mk (String_Constant (L"/etc/redhat-release"))).ReadAll ().Trim ();
                tmp.fShortPrettyName = L"RedHat";
                tmp.fPrettyNameWithMajorVersion = n;
            }
            catch (...) {
                DbgTrace ("Failure reading /etc/redhat-release");
            }
        }
        if (tmp.fShortPrettyName.empty () and tmp.fTokenName == L"aix")
        {
            try {
                tmp.fShortPrettyName = L"AIX";
                tmp.fPrettyNameWithMajorVersion =  L"AIX " + Execution::ProcessRunner (L"oslevel").Run (String ()).Trim ();
            }
            catch (...) {
                DbgTrace ("Failure reading oslevel");
            }
        }
        if (tmp.fShortPrettyName.empty ())
        {
            tmp.fShortPrettyName = tmp.fTokenName;
        }
        if (tmp.fPrettyNameWithMajorVersion.empty ())
        {
            tmp.fPrettyNameWithMajorVersion = tmp.fShortPrettyName;
        }
        if (tmp.fRFC1945CompatProductTokenWithVersion.empty ())
        {
            tmp.fRFC1945CompatProductTokenWithVersion = tmp.fShortPrettyName.Trim ().ReplaceAll (L" ", L"-");
            if (not tmp.fMajorMinorVersionString.empty ()) {
                tmp.fRFC1945CompatProductTokenWithVersion += L"/" + tmp.fMajorMinorVersionString;
            }
        }
        //
        // @todo FIX/FIND BETTER WAY!
        //
        //http://docs.oracle.com/cd/E36784_01/html/E36874/sysconf-3c.html
        // Quite uncertain - this is not a good reference
        //      --LGP 2014-10-18
        //
#if     defined ( _POSIX_V6_LP64_OFF64)
        tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _POSIX_V6_LP64_OFF64 ? 64 : 32;
#elif   defined ( _V6_LP64_OFF64)
        //AIX
        tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _V6_LP64_OFF64 ? 64 : 32;
#else
        // could be a C+++ const - let it not compile if not available, and we'll dig...
        tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _POSIX_V6_LP64_OFF64 ? 64 : 32;
#endif
#elif   qPlatform_Windows
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
                tmp.fShortPrettyName = osvi.wProductType == VER_NT_WORKSTATION ? String_Constant (L"Windows Vista") : String_Constant (L"Windows Server 2008");
            }
            else if (osvi.dwMinorVersion == 1) {
                tmp.fShortPrettyName = osvi.wProductType == VER_NT_WORKSTATION ? String_Constant (L"Windows 7") : String_Constant (L"Windows Server 2008 R2");
            }
            else if (osvi.dwMinorVersion == 2) {
                tmp.fShortPrettyName = osvi.wProductType == VER_NT_WORKSTATION ? String_Constant (L"Windows 8") : String_Constant (L"Windows Server 2012");
            }
            else if (osvi.dwMinorVersion == 3) {
                if (osvi.wProductType == VER_NT_WORKSTATION)
                    tmp.fShortPrettyName = String_Constant (L"Windows 8.1");
            }
        }
        if (tmp.fShortPrettyName.empty ())
        {
            tmp.fShortPrettyName = Characters::Format (L"Windows %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        }
        tmp.fPrettyNameWithMajorVersion = tmp.fShortPrettyName;
        tmp.fPrettyNameWithMajorVersion = GetWinOSDisplayString_ ();    // Cleanup - see above
        tmp.fMajorMinorVersionString = Characters::Format (L"%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        tmp.fRFC1945CompatProductTokenWithVersion = Characters::Format (L"Windows/%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        if (sizeof (void*) == 4)
        {
            tmp.fBits = 32;
            //IsWow64Process is not available on all supported versions of Windows.
            //Use GetModuleHandle to get a handle to the DLL that contains the function
            //and GetProcAddress to get a pointer to the function if available.
            typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
            LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
            if(NULL != fnIsWow64Process) {
                BOOL    isWOW64 = false;
                (void)fnIsWow64Process (GetCurrentProcess(), &isWOW64);
                if (isWOW64) {
                    tmp.fBits = 64;
                }
            }
        }
        else {
            // In windows, a 64 bit app cannot run on 32-bit windows
            Assert (sizeof (void*) == 8);
            tmp.fBits = 64;
        }
#else
        AssertNotImplemented ();
#endif
        return tmp;
    } ();
    return kCachedResult_;
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





/*
 ********************************************************************************
 ****************** SystemConfiguration GetSystemConfiguration ******************
 ********************************************************************************
 */
SystemConfiguration Configuration::GetSystemConfiguration ()
{
    return SystemConfiguration {
        GetSystemConfiguration_BootInformation (),
        GetSystemConfiguration_CPU (),
        GetSystemConfiguration_Memory (),
        GetSystemConfiguration_OperatingSystem (),
        GetSystemConfiguration_ComputerNames ()
    };
}

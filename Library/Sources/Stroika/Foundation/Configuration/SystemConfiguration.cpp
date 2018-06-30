/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <thread>

#if qPlatform_POSIX
#include <fstream>
#include <unistd.h>
#if qPlatform_Linux
#include <sys/sysinfo.h>
#endif
#include <utmpx.h>
#elif qPlatform_Windows
#include <Windows.h>
#include <intrin.h>
#endif

#include "../Characters/FloatConversion.h"
#include "../Characters/Format.h"
#include "../Characters/SDKString.h"
#include "../Characters/String2Int.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"
#include "../Containers/Sequence.h"
#include "../Containers/Set.h"
#if qPlatform_POSIX
#include "../Execution/ErrNoException.h"
#elif qPlatform_Windows
#include "../Execution/Platform/Windows/Exception.h"
#endif
#include "../IO/FileSystem/FileInputStream.h"
#include "../IO/FileSystem/FileSystem.h"
#include "../Memory/SmallStackBuffer.h"
#include "../Streams/TextReader.h"

#include "SystemConfiguration.h"

#if qPlatform_POSIX
#include "../DataExchange/Variant/INI/Reader.h"
#include "../Execution/ProcessRunner.h"
#include "../Streams/iostream/FStreamSupport.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Configuration;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Time;

using Characters::SDKChar;
using Characters::String_Constant;
using Characters::StringBuilder;
using Memory::Byte;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    constexpr EnumNames<SystemConfiguration::OperatingSystem::InstallerTechnology> DefaultNames<SystemConfiguration::OperatingSystem::InstallerTechnology>::k;
}
#endif

/*
 ********************************************************************************
 ***************** SystemConfiguration::BootInformation *************************
 ********************************************************************************
 */
String SystemConfiguration::BootInformation::ToString () const
{

    StringBuilder sb;
    sb += L"{";
    sb += L"Booted-At: " + Characters::ToString (fBootedAt);
    sb += L"}";
    return sb.str ();
};

/*
 ********************************************************************************
 ***************** SystemConfiguration::CPU::CoreDetails ************************
 ********************************************************************************
 */

String SystemConfiguration::CPU::CoreDetails::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Socket-ID: " + Characters::ToString (fSocketID) + L", ";
    sb += L"Model-Name: " + Characters::ToString (fModelName);
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ************************** SystemConfiguration::CPU ****************************
 ********************************************************************************
 */
String SystemConfiguration::CPU::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Cores: " + Characters::ToString (fCores);
    sb += L"}";
    return sb.str ();
};

/*
 ********************************************************************************
 ************************** SystemConfiguration::Memory *************************
 ********************************************************************************
 */
String SystemConfiguration::Memory::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Page-Size: " + Characters::ToString (fPageSize) + L", ";
    sb += L"Total-Physical-RAM: " + Characters::ToString (fTotalPhysicalRAM) + L", ";
    sb += L"Total-Virtual-RAM: " + Characters::ToString (fTotalVirtualRAM);
    sb += L"}";
    return sb.str ();
};

/*
 ********************************************************************************
 ********************** SystemConfiguration::ComputerNames **********************
 ********************************************************************************
 */
String SystemConfiguration::ComputerNames::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Hostname: " + Characters::ToString (fHostname) + L", ";
    sb += L"}";
    return sb.str ();
};

/*
 ********************************************************************************
 ********************* SystemConfiguration::OperatingSystem *********************
 ********************************************************************************
 */
String SystemConfiguration::OperatingSystem::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Token-Name: " + Characters::ToString (fTokenName) + L", ";
    sb += L"Short-Pretty-Name: " + Characters::ToString (fShortPrettyName) + L", ";
    sb += L"Pretty-Name-With-Major-Version: " + Characters::ToString (fPrettyNameWithMajorVersion) + L", ";
    sb += L"Major-Minor-Version-String: " + Characters::ToString (fMajorMinorVersionString) + L", ";
    sb += L"RFC1945-Compat-Product-Token-With-Version: " + Characters::ToString (fRFC1945CompatProductTokenWithVersion) + L", ";
    sb += L"Bits: " + Characters::ToString (fBits) + L", ";
    if (fPreferedInstallerTechnology) {
        sb += L"Prefered-Installer-Technology: " + Characters::ToString (*fPreferedInstallerTechnology) + L", ";
    }
    sb += L"}";
    return sb.str ();
};

/*
 ********************************************************************************
 ******************************* SystemConfiguration ****************************
 ********************************************************************************
 */
String SystemConfiguration::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Boot-Information: " + Characters::ToString (fBootInformation) + L", ";
    sb += L"CPU: " + Characters::ToString (fCPU) + L", ";
    sb += L"Memory: " + Characters::ToString (fMemory) + L", ";
    sb += L"Operating-System: " + Characters::ToString (fOperatingSystem) + L", ";
    sb += L"Computer-Names: " + Characters::ToString (fComputerNames) + L", ";
    sb += L"}";
    return sb.str ();
};

/*
 ********************************************************************************
 ***************** Configuration::SystemConfiguration::CPU **********************
 ********************************************************************************
 */
unsigned int SystemConfiguration::CPU::GetNumberOfSockets () const
{
    Set<unsigned int> socketIds;
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
    SystemConfiguration::BootInformation result;
#if qPlatform_Linux
    struct sysinfo info;
    ::sysinfo (&info);
    result.fBootedAt = DateTime::Now ().AddSeconds (-info.uptime);
#elif qPlatform_POSIX
    {
        // @todo - I dont think /proc/uptime is POSIX ... NOT SURE HOW TO DEFINE THIS - MAYBE ONLY .... on LINUX?
        bool                  succeeded{false};
        const String_Constant kProcUptimeFileName_{L"/proc/uptime"};
        if (IO::FileSystem::Default ().Access (kProcUptimeFileName_)) {
            /*
             *  From https://www.centos.org/docs/5/html/5.1/Deployment_Guide/s2-proc-uptime.html
             *      "The first number is the total number of seconds the system has been up"
             */
            using Characters::String2Int;
            using IO::FileSystem::FileInputStream;
            using Streams::TextReader;
            for (String line : TextReader::New (FileInputStream::New (kProcUptimeFileName_, FileInputStream::eNotSeekable)).ReadLines ()) {
                Sequence<String> t = line.Tokenize ();
                if (t.size () >= 2) {
                    result.fBootedAt = DateTime::Now ().AddSeconds (-Characters::String2Float<double> (t[0]));
                    succeeded        = true;
                }
                break;
            }
        }
        if (not succeeded) {
            /*
             *  The hard way is to read /etc/utmp
             *
             *  http://pubs.opengroup.org/onlinepubs/009695399/basedefs/utmpx.h.html
             *
             *  This isn't threadsafe, or in any way reasonable. AIX has a non-standard threadsafe API, but I'm not sure of the
             *  need to fix this..????
             *      --LGP 2015-08-21
             */
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([]() noexcept { ::endutxent (); });
            ::setutxent ();
            for (const utmpx* i = ::getutxent (); i != nullptr; i = ::getutxent ()) {
                if (i->ut_type == BOOT_TIME) {
                    result.fBootedAt = DateTime (i->ut_tv);
                    succeeded        = true;
                }
            }
        }
        Assert (succeeded); // not a real assert, but sort of a warning if this ever gets triggered
    }
#elif qPlatform_Windows
// ::GetTickCount () is defined to return #seconds since boot
#if _WIN32_WINNT >= 0x0600
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
    // @todo - basically all these implementations assume same # logical cores per physical CPU socket
    // @todo - no API to capture (maybe not useful) # physical cores
    using CPU = SystemConfiguration::CPU;
    CPU result;
#if qPlatform_Linux
    {
        using Characters::String2Int;
        using IO::FileSystem::FileInputStream;
        using Streams::TextReader;
        const String_Constant kProcCPUInfoFileName_{L"/proc/cpuinfo"};
        /*
        * Example 1:
        *
        *       #uname -a && cat /proc/cpuinfo
        *       Linux lewis-UbuntuDevVM3 3.19.0-58-generic #64-Ubuntu SMP Thu Mar 17 18:30:04 UTC 2016 x86_64 x86_64 x86_64 GNU/Linux
        *       processor   : 0
        *       vendor_id   : GenuineIntel
        *       cpu family  : 6
        *       model       : 60
        *       model name  : Intel(R) Core(TM) i7-4810MQ CPU @ 2.80GHz
        *       stepping    : 3
        *       cpu MHz     : 2793.309
        *       cache size  : 6144 KB
        *       physical id : 0
        *       siblings    : 2
        *       core id     : 0
        *       cpu cores   : 2
        *       apicid      : 0
        *       initial apicid  : 0
        *       fpu     : yes
        *       fpu_exception   : yes
        *       cpuid level : 13
        *       wp      : yes
        *       flags       : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx rdtscp lm constant_tsc rep_good nopl xtopology nonstop_tsc pni pclmulqdq ssse3 cx16 sse4_1 sse4_2 movbe popcnt aes xsave avx rdrand lahf_lm abm
        *       bugs        :
        *       bogomips    : 5586.61
        *       clflush size    : 64
        *       cache_alignment : 64
        *       address sizes   : 39 bits physical, 48 bits virtual
        *       power management:
        *
        *       processor   : 1
        *       vendor_id   : GenuineIntel
        *       cpu family  : 6
        *       model       : 60
        *       model name  : Intel(R) Core(TM) i7-4810MQ CPU @ 2.80GHz
        *       stepping    : 3
        *       cpu MHz     : 2793.309
        *       cache size  : 6144 KB
        *       physical id : 0
        *       siblings    : 2
        *       core id     : 1
        *       cpu cores   : 2
        *       apicid      : 1
        *       initial apicid  : 1
        *       fpu     : yes
        *       fpu_exception   : yes
        *       cpuid level : 13
        *       wp      : yes
        *       flags       : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx rdtscp lm constant_tsc rep_good nopl xtopology nonstop_tsc pni pclmulqdq ssse3 cx16 sse4_1 sse4_2 movbe popcnt aes xsave avx rdrand lahf_lm abm
        *       bugs        :
        *       bogomips    : 5586.61
        *       clflush size    : 64
        *       cache_alignment : 64
        *       address sizes   : 39 bits physical, 48 bits virtual
        *       power management:
        *
        * Example 2:
        *
        *       root@q7imx6:/opt/BLKQCL# uname -a
        *       Linux q7imx6 3.0.35.Q7_IMX6-14.03.01 #2 SMP PREEMPT Thu May 5 01:12:05 UTC 2016 armv7l GNU/Linux
        *       root@q7imx6:/opt/BLKQCL# cat /proc/cpuinfo
        *       Processor       : ARMv7 Processor rev 10 (v7l)
        *       processor       : 0
        *       BogoMIPS        : 1988.28
        *
        *       processor       : 1
        *       BogoMIPS        : 1988.28
        *
        *       Features        : swp half thumb fastmult vfp edsp neon vfpv3
        *       CPU implementer : 0x41
        *       CPU architecture: 7
        *       CPU variant     : 0x2
        *       CPU part        : 0xc09
        *       CPU revision    : 10
        *
        *       Hardware        : MSC Q7-IMX6 Module
        *       Revision        : 63000
        *       Serial          : 0000000000000000
        */
        // Note - /procfs files always unseekable
        optional<String>       foundProcessor;
        optional<unsigned int> currentProcessorID;
        optional<String>       currentModelName;
        optional<unsigned int> currentSocketID;
        for (String line : TextReader::New (FileInputStream::New (kProcCPUInfoFileName_, FileInputStream::eNotSeekable)).ReadLines ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"in Configuration::GetSystemConfiguration_CPU capture_ line=%s", line.c_str ());
#endif
            static const String_Constant kOldProcessorLabel_{L"Processor"};
            static const String_Constant kProcessorIDLabel_{L"processor"};
            static const String_Constant kModelNameLabel_{L"model name"};
            static const String_Constant kSocketIDLabel_{L"physical id"}; // a bit of a guess?
            if (line.Trim ().empty ()) {
                Sequence<String> lineTokens = line.Tokenize (Set<Character>{':'});
                if (lineTokens.size () >= 2) {
                    String firstTrimedToken = lineTokens[0].Trim ();
                    size_t afterColon       = *line.Find (':') + 1;
                    if (firstTrimedToken == kOldProcessorLabel_) {
                        foundProcessor = line.SubString (afterColon).Trim ();
                    }
                    else if (firstTrimedToken == kProcessorIDLabel_) {
                        currentProcessorID = String2Int<unsigned int> (line.SubString (afterColon).Trim ());
                    }
                    else if (firstTrimedToken == kModelNameLabel_) {
                        currentModelName = line.SubString (afterColon).Trim ();
                    }
                    else if (firstTrimedToken == kSocketIDLabel_) {
                        currentSocketID = String2Int<unsigned int> (line.SubString (afterColon).Trim ());
                    }
                }

                // ends each socket
                if (currentProcessorID) {
                    String useModelName = foundProcessor.Value ();
                    currentModelName.CopyToIf (&useModelName); // currentModelName takes precedence but I doubt both present
                    result.fCores.Append (CPU::CoreDetails{currentSocketID.Value (), useModelName});
                }
                // intentionally dont clear foundProcessor cuz occurs once it appears
                currentProcessorID = nullopt;
                currentModelName = nullopt;
                currentSocketID = nullopt;
            }
        }
        if (currentProcessorID) {
            String useModelName = foundProcessor.Value ();
            Memory::CopyToIf (currentModelName, &useModelName);     // currentModelName takes precedence but I doubt both present
            result.fCores.Append (CPU::CoreDetails{currentSocketID.Value (), useModelName});
        }
    }
#elif qPlatform_Windows
    /*
     *  Based on https://msdn.microsoft.com/en-us/library/ms683194?f=255&MSPPError=-2147217396
     *
     *  I deleted code to capture NUMA nodes, and processor caches, and dont currently use number of physical cores, but could
     *  get that from original code.
     */
    DWORD logicalProcessorCount = 0;
    DWORD processorCoreCount    = 0;
    DWORD processorPackageCount = 0;
    {
        auto countSetBits = [](ULONG_PTR bitMask) -> DWORD {
            DWORD     LSHIFT      = sizeof (ULONG_PTR) * 8 - 1;
            DWORD     bitSetCount = 0;
            ULONG_PTR bitTest     = (ULONG_PTR)1 << LSHIFT;
            for (DWORD i = 0; i <= LSHIFT; ++i) {
                bitSetCount += ((bitMask & bitTest) ? 1 : 0);
                bitTest /= 2;
            }
            return bitSetCount;
        };
        typedef BOOL (WINAPI * LPFN_GLPI) (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
        DISABLE_COMPILER_MSC_WARNING_START (6387) // ignore check for null GetModuleHandle - if that fails - we have bigger problems and a crash sounds imminent
        LPFN_GLPI glpi = (LPFN_GLPI)::GetProcAddress (::GetModuleHandle (TEXT ("kernel32")), "GetLogicalProcessorInformation");
        DISABLE_COMPILER_MSC_WARNING_END (6387)
        AssertNotNull (glpi); // assume at least OS WinXP...
        Memory::SmallStackBuffer<Byte> buffer (sizeof (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        DWORD                          returnLength = 0;
        while (true) {
            DWORD rc = glpi (reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION> (buffer.begin ()), &returnLength);
            if (FALSE == rc) {
                if (GetLastError () == ERROR_INSUFFICIENT_BUFFER) {
                    buffer.GrowToSize (returnLength);
                }
                else {
                    Execution::Platform::Windows::ThrowIfNot_NO_ERROR (rc);
                }
            }
            else {
                break;
            }
        }
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr        = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION> (buffer.begin ());
        DWORD                                 byteOffset = 0;
        while (byteOffset + sizeof (SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) {
            switch (ptr->Relationship) {
                case RelationNumaNode:
                    break;
                case RelationProcessorCore:
                    processorCoreCount++;
                    // A hyperthreaded core supplies more than one logical processor.
                    logicalProcessorCount += countSetBits (ptr->ProcessorMask);
                    break;
                case RelationCache:
                    break;
                case RelationProcessorPackage:
                    // Logical processors share a physical package.
                    processorPackageCount++;
                    break;
                default:
                    DbgTrace ("Error: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n");
                    break;
            }
            byteOffset += sizeof (SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            ptr++;
        }
    }

    static const String kProcessorType_ = []() {
        int  CPUInfo[4] = {-1};
        char CPUBrandString[0x40];
        // Get the information associated with each extended ID.
        ::__cpuid (CPUInfo, 0x80000000);
        uint32_t nExIds = CPUInfo[0];
        for (uint32_t i = 0x80000000; i <= nExIds; ++i) {
            ::__cpuid (CPUInfo, i);
            // Interpret CPU brand string
            if (i == 0x80000002)
                (void)::memcpy (CPUBrandString, CPUInfo, sizeof (CPUInfo));
            else if (i == 0x80000003)
                (void)::memcpy (CPUBrandString + 16, CPUInfo, sizeof (CPUInfo));
            else if (i == 0x80000004)
                (void)::memcpy (CPUBrandString + 32, CPUInfo, sizeof (CPUInfo));
        }
        return String::FromASCII (CPUBrandString);
    }();

#if qDebug
    {
        SYSTEM_INFO sysInfo{}; // GetNativeSystemInfo cannot fail so no need to initialize data
        ::GetNativeSystemInfo (&sysInfo);
        Assert (sysInfo.dwNumberOfProcessors == logicalProcessorCount);
    }
#endif
    for (unsigned int socketNum = 0; socketNum < processorPackageCount; ++socketNum) {
        unsigned int logProcessorsPerSocket = logicalProcessorCount / processorPackageCount;
        for (DWORD i = 0; i < logProcessorsPerSocket; ++i) {
            result.fCores.Append (CPU::CoreDetails{socketNum, kProcessorType_});
        }
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
    using Memory = SystemConfiguration::Memory;
    Memory result;
#if qPlatform_POSIX
    // page size cannot change while running, but number of pages can
    // (e.g. https://pubs.vmware.com/vsphere-50/index.jsp?topic=%2Fcom.vmware.vsphere.vm_admin.doc_50%2FGUID-0B4C3128-F854-43B9-9D80-A20C0C8B0FF7.html)
    static const size_t kPageSize_{static_cast<size_t> (::sysconf (_SC_PAGESIZE))};
    result.fPageSize         = kPageSize_;
    result.fTotalPhysicalRAM = ::sysconf (_SC_PHYS_PAGES) * kPageSize_;
#elif qPlatform_Windows
    SYSTEM_INFO sysInfo;
    ::GetNativeSystemInfo (&sysInfo);
    result.fPageSize = sysInfo.dwPageSize;

    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof (memStatus);
    Verify (::GlobalMemoryStatusEx (&memStatus));
    result.fTotalPhysicalRAM = memStatus.ullTotalPhys;
    result.fTotalVirtualRAM  = memStatus.ullTotalVirtual;
#endif
    return result;
}

/*
 ********************************************************************************
 ******** Configuration::GetSystemConfiguration_OperatingSystem *****************
 ********************************************************************************
 */
#if qPlatform_Windows
namespace {
    /*
     *  Someday it would be nice to find a better way, but as of 2015-10-19, I've not been able to find one (without using WMI).
     */
    String GetWinOSDisplayString_ ()
    {
        OSVERSIONINFOEX osvi{};
        {
            osvi.dwOSVersionInfoSize = sizeof (osvi);
            // MSFT is crazy. They deprecate GetVersionEx, but then still require it for GetProductInfo, and provide no
            // other way to find the product description string?
            // I spent over an hour looking. Give up for now. Sigh...
            //      --LGP 2015-10-19
            DISABLE_COMPILER_MSC_WARNING_START (4996)
            DISABLE_COMPILER_MSC_WARNING_START (28159)
            if (not::GetVersionEx ((OSVERSIONINFO*)&osvi)) {
                return String ();
            }
            DISABLE_COMPILER_MSC_WARNING_END (28159)
            DISABLE_COMPILER_MSC_WARNING_END (4996)
        }

        HMODULE kernel32 = ::GetModuleHandle (TEXT ("kernel32.dll"));
        AssertNotNull (kernel32);

        if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) {
            return Characters::Format (L"Unknown Windows Version %d %d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        }
        if (osvi.dwMajorVersion <= 4) {
            return Characters::Format (L"Unknown Ancient Windows Version %d %d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        }

        SYSTEM_INFO si{};
        {
            // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
            using PGNSI = void(WINAPI*) (LPSYSTEM_INFO);
            PGNSI pGNSI = (PGNSI)::GetProcAddress (kernel32, "GetNativeSystemInfo");
            if (pGNSI == nullptr) {
                ::GetSystemInfo (&si);
            }
            else {
                (*pGNSI) (&si);
            }
        }

        String goodName;
        switch (osvi.dwMajorVersion) {
            case 10: {
                if (osvi.dwMinorVersion == 0) {
                    goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 10 " : L"Windows Server 2016 ";
                }
            } break;
            case 6: {
                if (osvi.dwMinorVersion == 0) {
                    goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows Vista " : L"Windows Server 2008 ";
                }
                else if (osvi.dwMinorVersion == 1) {
                    goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 7 " : L"Windows Server 2008 R2 ";
                }
                else if (osvi.dwMinorVersion == 2) {
                    goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 8 " : L"Windows Server 2012 ";
                }
                else if (osvi.dwMinorVersion == 3) {
                    goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 8.1 " : L"Windows Server 2012 R2 ";
                }
            } break;
            case 5: {
                if (osvi.dwMinorVersion == 0) {
                    goodName = L"Windows 2000 ";
                    if (osvi.wProductType == VER_NT_WORKSTATION) {
                        goodName += L"Professional";
                    }
                    else {
                        if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                            goodName += L"Datacenter Server";
                        }
                        else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                            goodName += L"Advanced Server";
                        }
                        else {
                            goodName += L"Server";
                        }
                    }
                }
                else if (osvi.dwMinorVersion == 1) {
                    goodName = L"Windows XP ";
                    goodName += (osvi.wSuiteMask & VER_SUITE_PERSONAL) ? L"Home Edition" : L"Professional";
                }
                else if (osvi.dwMinorVersion == 2) {
                    if (::GetSystemMetrics (SM_SERVERR2)) {
                        goodName = L"Windows Server 2003 R2, ";
                    }
                    else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER) {
                        goodName = L"Windows Storage Server 2003";
                    }
                    else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER) {
                        goodName = L"Windows Home Server";
                    }
                    else if (osvi.wProductType == VER_NT_WORKSTATION and si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
                        goodName = L"Windows XP Professional x64 Edition";
                    }
                    else {
                        goodName = L"Windows Server 2003, ";
                    }

                    // Test for the server type.
                    if (osvi.wProductType != VER_NT_WORKSTATION) {
                        if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
                            if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                                goodName += L"Datacenter Edition for Itanium-based Systems";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                                goodName += L"Enterprise Edition for Itanium-based Systems";
                            }
                        }
                        else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
                            if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                                goodName += L"Datacenter x64 Edition";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                                goodName += L"Enterprise x64 Edition";
                            }
                            else {
                                goodName += L"Standard x64 Edition";
                            }
                        }
                        else {
                            if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER) {
                                goodName += L"Compute Cluster Edition";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                                goodName += L"Datacenter Edition";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                                goodName += L"Enterprise Edition";
                            }
                            else if (osvi.wSuiteMask & VER_SUITE_BLADE) {
                                goodName += L"Web Edition";
                            }
                            else {
                                goodName += L"Standard Edition";
                            }
                        }
                    }
                }
            } break;
            default: {
                Assert (osvi.dwMajorVersion > 10); //. or we should have hit a case... (or msft went back in time???)
                // no need to fill in name, cuz just lik eother cases with no goodName
            } break;
        }

        Characters::StringBuilder result;
        result += L"Microsoft ";
        if (goodName.empty ()) {
            result += Characters::Format (L"Windows Version %d %d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        }
        else {
            result += goodName;
        }

        if (osvi.dwMajorVersion >= 6) {
            DWORD dwType = PRODUCT_UNDEFINED;
            {
                typedef BOOL (WINAPI * PGPI) (DWORD, DWORD, DWORD, DWORD, PDWORD);
                // OK cuz GetProductVersion introuced in vista (https://msdn.microsoft.com/en-us/library/windows/desktop/ms724358(v=vs.85).aspx)
                PGPI pGPI = (PGPI)::GetProcAddress (kernel32, "GetProductInfo");
                AssertNotNull (pGPI);
                (*pGPI) (osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);
            }
            switch (dwType) {
                case PRODUCT_CORE:
                    result += L"Home";
                    break;
                case PRODUCT_CORE_N:
                    result += L"Home N";
                    break;
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
                default:
                    result += Characters::Format (L"Unknown Edition %d", dwType);
                    break;
            }
        }

        result += Characters::Format (L" (build %d)", osvi.dwBuildNumber);

        if (osvi.dwMajorVersion >= 6) {
            if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
                result += L", 64-bit";
            }
            else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
                result += L", 32-bit";
            }
        }
        return result.str ();
    }
}
#endif

SystemConfiguration::OperatingSystem Configuration::GetSystemConfiguration_OperatingSystem ()
{
    using OperatingSystem                       = SystemConfiguration::OperatingSystem;
    static const OperatingSystem kCachedResult_ = []() -> OperatingSystem {
        OperatingSystem tmp;
#if qPlatform_POSIX
        tmp.fTokenName = String_Constant (L"Unix");
        try {
            tmp.fTokenName = Execution::ProcessRunner (L"uname").Run (String ()).Trim ();
        }
        catch (...) {
            DbgTrace ("Failure running uname");
        }
        try {
            ifstream s;
            Streams::iostream::OpenInputFileStream (&s, L"/etc/os-release");
            DataExchange::Variant::INI::Profile p = DataExchange::Variant::INI::Reader ().ReadProfile (s);
            tmp.fShortPrettyName                  = p.fUnnamedSection.fProperties.LookupValue (L"NAME");
            tmp.fPrettyNameWithMajorVersion       = p.fUnnamedSection.fProperties.LookupValue (L"PRETTY_NAME");
            tmp.fMajorMinorVersionString          = p.fUnnamedSection.fProperties.LookupValue (L"VERSION_ID");
        }
        catch (...) {
            DbgTrace ("Failure reading /etc/os-release");
        }
        if (tmp.fShortPrettyName.empty ()) {
            try {
                String n                        = Streams::TextReader::New (IO::FileSystem::FileInputStream::New (String_Constant (L"/etc/centos-release"))).ReadAll ().Trim ();
                tmp.fShortPrettyName            = L"Centos";
                tmp.fPrettyNameWithMajorVersion = n;
                Sequence<String> tokens         = n.Tokenize ();
                if (tokens.size () >= 3) {
                    tmp.fMajorMinorVersionString = tokens[2];
                }
            }
            catch (...) {
                DbgTrace ("Failure reading /etc/centos-release");
            }
        }
        if (tmp.fShortPrettyName.empty ()) {
            try {
                String n                        = Streams::TextReader::New (IO::FileSystem::FileInputStream::New (String_Constant (L"/etc/redhat-release"))).ReadAll ().Trim ();
                tmp.fShortPrettyName            = L"RedHat";
                tmp.fPrettyNameWithMajorVersion = n;
                Sequence<String> tokens         = n.Tokenize ();
                if (tokens.size () >= 3) {
                    tmp.fMajorMinorVersionString = tokens[2];
                }
            }
            catch (...) {
                DbgTrace ("Failure reading /etc/redhat-release");
            }
        }
        if (tmp.fShortPrettyName.empty ()) {
            tmp.fShortPrettyName = tmp.fTokenName;
        }
        if (tmp.fPrettyNameWithMajorVersion.empty ()) {
            tmp.fPrettyNameWithMajorVersion = tmp.fShortPrettyName;
        }
        if (tmp.fRFC1945CompatProductTokenWithVersion.empty ()) {
            tmp.fRFC1945CompatProductTokenWithVersion = tmp.fShortPrettyName.Trim ().ReplaceAll (L" ", L"-");
            if (not tmp.fMajorMinorVersionString.empty ()) {
                tmp.fRFC1945CompatProductTokenWithVersion += L"/" + tmp.fMajorMinorVersionString;
            }
        }

#if defined(_POSIX_V6_LP64_OFF64)
        //
        // @todo FIX/FIND BETTER WAY!
        //
        //http://docs.oracle.com/cd/E36784_01/html/E36874/sysconf-3c.html
        // Quite uncertain - this is not a good reference
        //      --LGP 2014-10-18
        //
        tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _POSIX_V6_LP64_OFF64 ? 64 : 32;
#elif defined(_V6_LP64_OFF64)
        //AIX? but maybe others??? -- LGP 2016-09-10 - not important to fix/remove
        tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _V6_LP64_OFF64 ? 64 : 32;
#else
        // could be a C+++ const - let it not compile if not available, and we'll dig...
        //tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _POSIX_V6_LP64_OFF64 ? 64 : 32;
        DbgTrace ("_SC_V6_LP64_OFF64/_POSIX_V6_LP64_OFF64 not available - so assuming 32-bit...");
#endif

        using Characters::CompareOptions;

        // No good way I can find to tell...
        if (not tmp.fPreferedInstallerTechnology.has_value ()) {
            if (tmp.fShortPrettyName.Equals (L"Centos", CompareOptions::eCaseInsensitive) or
                tmp.fShortPrettyName.Equals (L"RedHat", CompareOptions::eCaseInsensitive)) {
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eRPM;
            }
            else if (tmp.fShortPrettyName.Equals (L"Ubuntu", CompareOptions::eCaseInsensitive)) {
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eDPKG;
            }
        }
        // not a great way to test since some systems have both, like ubuntu
        if (not tmp.fPreferedInstallerTechnology.has_value ()) {
            try {
                Execution::ProcessRunner (L"dpkg --help").Run (String ());
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eDPKG;
            }
            catch (...) {
            }
        }
        if (not tmp.fPreferedInstallerTechnology.has_value ()) {
            try {
                Execution::ProcessRunner (L"rpm --help").Run (String ());
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eRPM;
            }
            catch (...) {
            }
        }
#elif qPlatform_Windows
        tmp.fTokenName = String_Constant (L"Windows");
        /*
         *  Microslop declares this deprecated, but then fails to provide a reasonable alternative.
         *
         *  Sigh.
         *
         *  http://msdn.microsoft.com/en-us/library/windows/desktop/ms724429(v=vs.85).aspx -  GetFileVersionInfo (kernel32.dll)
         *  is a painful, and stupid alternative.
         */
        OSVERSIONINFOEX osvi{};
        osvi.dwOSVersionInfoSize = sizeof (osvi);
        DISABLE_COMPILER_MSC_WARNING_START (4996)
        DISABLE_COMPILER_MSC_WARNING_START (28159)
        Verify (::GetVersionEx (reinterpret_cast<LPOSVERSIONINFO> (&osvi)));
        DISABLE_COMPILER_MSC_WARNING_END (28159)
        DISABLE_COMPILER_MSC_WARNING_END (4996)
        if (osvi.dwMajorVersion == 6) {
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
        if (tmp.fShortPrettyName.empty ()) {
            tmp.fShortPrettyName = Characters::Format (L"Windows %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        }
        tmp.fPrettyNameWithMajorVersion           = tmp.fShortPrettyName;
        tmp.fPrettyNameWithMajorVersion           = GetWinOSDisplayString_ (); // Cleanup - see above
        tmp.fMajorMinorVersionString              = Characters::Format (L"%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        tmp.fRFC1945CompatProductTokenWithVersion = Characters::Format (L"Windows/%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        if constexpr (sizeof (void*) == 4) {
            tmp.fBits = 32;
            //IsWow64Process is not available on all supported versions of Windows.
            //Use GetModuleHandle to get a handle to the DLL that contains the function
            //and GetProcAddress to get a pointer to the function if available.
            typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
            DISABLE_COMPILER_MSC_WARNING_START (6387) // ignore check for null GetModuleHandle - if that fails - we have bigger problems and a crash sounds imminent
            LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress (::GetModuleHandle (TEXT ("kernel32")), "IsWow64Process");
            DISABLE_COMPILER_MSC_WARNING_END (6387)
            if (NULL != fnIsWow64Process) {
                BOOL isWOW64 = false;
                (void)fnIsWow64Process (::GetCurrentProcess (), &isWOW64);
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
        tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eMSI;
#else
        AssertNotImplemented ();
#endif
        return tmp;
    }();
    return kCachedResult_;
}

/*
 ********************************************************************************
 ***************** GetSystemConfiguration_ComputerNames *************************
 ********************************************************************************
 */
#if 0 && qPlatform_POSIX
// ALTERNATE APPROACH TO CONSIDER
string  name;
{
    struct addrinfo* res;
    struct addrinfo hints {};
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
    ComputerNames result;
#if qPlatform_POSIX
    char nameBuf[1024];
    Execution::ThrowErrNoIfNegative (::gethostname (nameBuf, NEltsOf (nameBuf)));
    nameBuf[NEltsOf (nameBuf) - 1] = '\0'; // http://linux.die.net/man/2/gethostname says not necessarily nul-terminated
    result.fHostname               = String::FromNarrowSDKString (nameBuf);
#elif qPlatform_Windows
    constexpr COMPUTER_NAME_FORMAT kUseNameFormat_ = ComputerNameNetBIOS; // total WAG -- LGP 2014-10-10
    DWORD                          dwSize          = 0;
    (void)::GetComputerNameEx (kUseNameFormat_, nullptr, &dwSize);
    Memory::SmallStackBuffer<SDKChar> buf (dwSize);
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
    return SystemConfiguration{
        GetSystemConfiguration_BootInformation (),
        GetSystemConfiguration_CPU (),
        GetSystemConfiguration_Memory (),
        GetSystemConfiguration_OperatingSystem (),
        GetSystemConfiguration_ComputerNames ()};
}

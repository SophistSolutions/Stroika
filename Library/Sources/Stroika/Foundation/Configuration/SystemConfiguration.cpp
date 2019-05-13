/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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

#include <VersionHelpers.h>
#include <intrin.h>
#endif

#include "../Characters/FloatConversion.h"
#include "../Characters/Format.h"
#include "../Characters/SDKString.h"
#include "../Characters/String2Int.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"
#if qPlatform_Windows
#include "../Configuration/Platform/Windows/Registry.h"
#endif
#include "../Containers/Sequence.h"
#include "../Containers/Set.h"
#include "../Execution/Exceptions.h"
#if qPlatform_Windows
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

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Configuration;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Time;

using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

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
    sb += L"Pretty-Name-With-Details: " + Characters::ToString (fPrettyNameWithVersionDetails) + L", ";
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
    sb += L"Actual-Operating-System: " + Characters::ToString (fActualOperatingSystem) + L", ";
    sb += L"Apparent-Operating-System: " + Characters::ToString (fApparentOperatingSystem) + L", ";
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
        // @todo - I don't think /proc/uptime is POSIX ... NOT SURE HOW TO DEFINE THIS - MAYBE ONLY .... on LINUX?
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
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([] () noexcept { ::endutxent (); });
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
        const String kProcCPUInfoFileName_{L"/proc/cpuinfo"sv};
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
            static const String kOldProcessorLabel_{L"Processor"sv};
            static const String kProcessorIDLabel_{L"processor"sv};
            static const String kModelNameLabel_{L"model name"sv};
            static const String kSocketIDLabel_{L"physical id"sv}; // a bit of a guess?
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
                    String useModelName = Memory::ValueOrDefault (foundProcessor);
                    Memory::CopyToIf (currentModelName, &useModelName); // currentModelName takes precedence but I doubt both present
                    result.fCores.Append (CPU::CoreDetails{Memory::ValueOrDefault (currentSocketID), useModelName});
                }
                // intentionally don't clear foundProcessor cuz occurs once it appears
                currentProcessorID = nullopt;
                currentModelName   = nullopt;
                currentSocketID    = nullopt;
            }
        }
        if (currentProcessorID) {
            String useModelName = Memory::ValueOrDefault (foundProcessor);
            Memory::CopyToIf (currentModelName, &useModelName); // currentModelName takes precedence but I doubt both present
            result.fCores.Append (CPU::CoreDetails{Memory::ValueOrDefault (currentSocketID), useModelName});
        }
    }
#elif qPlatform_Windows
    /*
     *  Based on https://msdn.microsoft.com/en-us/library/ms683194?f=255&MSPPError=-2147217396
     *
     *  I deleted code to capture NUMA nodes, and processor caches, and don't currently use number of physical cores, but could
     *  get that from original code.
     */
    DWORD logicalProcessorCount = 0;
    DWORD processorCoreCount    = 0;
    DWORD processorPackageCount = 0;
    {
        auto countSetBits = [] (ULONG_PTR bitMask) -> DWORD {
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
        SmallStackBuffer<byte> buffer (SmallStackBufferCommon::eUninitialized, sizeof (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        DWORD                  returnLength = 0;
        while (true) {
            DWORD rc = glpi (reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION> (buffer.begin ()), &returnLength);
            if (FALSE == rc) {
                if (GetLastError () == ERROR_INSUFFICIENT_BUFFER) {
                    buffer.GrowToSize_uninitialized (returnLength);
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

    static const String kProcessorType_ = [] () {
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
SystemConfiguration::OperatingSystem Configuration::GetSystemConfiguration_ActualOperatingSystem ()
{
    using OperatingSystem                       = SystemConfiguration::OperatingSystem;
    static const OperatingSystem kCachedResult_ = [] () -> OperatingSystem {
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
            tmp.fShortPrettyName                  = p.fUnnamedSection.fProperties.LookupValue (L"NAME"sv);
            tmp.fPrettyNameWithMajorVersion       = p.fUnnamedSection.fProperties.LookupValue (L"PRETTY_NAME"sv);
            tmp.fMajorMinorVersionString          = p.fUnnamedSection.fProperties.LookupValue (L"VERSION_ID"sv);
        }
        catch (...) {
            DbgTrace ("Failure reading /etc/os-release");
        }
        if (tmp.fShortPrettyName.empty ()) {
            try {
                String n                        = Streams::TextReader::New (IO::FileSystem::FileInputStream::New (L"/etc/centos-release"sv)).ReadAll ().Trim ();
                tmp.fShortPrettyName            = L"Centos"sv;
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
                String n                        = Streams::TextReader::New (IO::FileSystem::FileInputStream::New (L"/etc/redhat-release"sv)).ReadAll ().Trim ();
                tmp.fShortPrettyName            = L"RedHat"sv;
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
        if (tmp.fPrettyNameWithVersionDetails.empty ()) {
            tmp.fPrettyNameWithVersionDetails = tmp.fPrettyNameWithMajorVersion;
        }
        if (tmp.fRFC1945CompatProductTokenWithVersion.empty ()) {
            tmp.fRFC1945CompatProductTokenWithVersion = tmp.fShortPrettyName.Trim ().ReplaceAll (L" "sv, L"-"sv);
            if (not tmp.fMajorMinorVersionString.empty ()) {
                tmp.fRFC1945CompatProductTokenWithVersion += L"/"sv + tmp.fMajorMinorVersionString;
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
            auto nameEqComparer = String::EqualsComparer{CompareOptions::eCaseInsensitive};
            if (nameEqComparer (tmp.fShortPrettyName, L"Centos"sv) or
                nameEqComparer (tmp.fShortPrettyName, L"RedHat"sv)) {
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eRPM;
            }
            else if (nameEqComparer (tmp.fShortPrettyName, L"Ubuntu"sv)) {
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
        tmp.fTokenName = L"Windows"sv;

        /*
         *  Best (of a bad bunch) official reference from Microsoft on finding version#s
         *      https://docs.microsoft.com/en-us/windows/desktop/SysInfo/operating-system-version
         */

        /*
         *  note
         *      Could consider using RtlGetVersion from https://stackoverflow.com/questions/36543301/detecting-windows-10-version
         *      except this only returns a #, and not the pretty name we get from the registry.
         */

        String kernelOSBuildVersion;
        String kernelVersion;
        {
            /*
             *  How you do this seems to change alot. But as of 2019-03-16:
             *      from https://docs.microsoft.com/en-us/windows/desktop/sysinfo/getting-the-system-version
             *          To obtain the full version number for the operating system, 
             *          call the GetFileVersionInfo function on one of the system DLLs,
             *          such as Kernel32.dll, then call VerQueryValue to obtain the 
             *          \\StringFileInfo\\\\ProductVersion subblock of the file version information
             */
            const wchar_t*         kkernel32_ = L"kernel32.dll";
            DWORD                  dummy;
            const auto             cbInfo = ::GetFileVersionInfoSizeExW (FILE_VER_GET_NEUTRAL, kkernel32_, &dummy);
            SmallStackBuffer<char> buffer (cbInfo);
            Verify (::GetFileVersionInfoExW (FILE_VER_GET_NEUTRAL, kkernel32_, dummy, static_cast<DWORD> (buffer.size ()), &buffer[0]));
            void* p    = nullptr;
            UINT  size = 0;
            Verify (::VerQueryValueW (buffer, L"\\", &p, &size));
            Assert (size >= sizeof (VS_FIXEDFILEINFO));
            Assert (p != nullptr);
            auto pFixed          = static_cast<const VS_FIXEDFILEINFO*> (p);
            kernelVersion        = Characters::Format (L"%d.%d", HIWORD (pFixed->dwFileVersionMS), LOWORD (pFixed->dwFileVersionMS));
            kernelOSBuildVersion = Characters::Format (L"%d.%d", HIWORD (pFixed->dwFileVersionLS), LOWORD (pFixed->dwFileVersionLS));
        }

        /*
         *  This trick is widely referenced on the internet, but does NOT appear in any official Microsoft documentaiton
         *  I can find, so is likely an implementation detail subject to change.
         *
         *  However, it is referenced  here for example:
         *      https://stackoverflow.com/questions/31072543/reliable-way-to-get-windows-version-from-registry
         */
        optional<String> platformVersion;
        optional<String> productName;
        optional<String> currentVersion; // windows major-minor version
        try {
            const Configuration::Platform::Windows::RegistryKey kWinVersionInfo_{HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"sv};
            if (auto o = kWinVersionInfo_.Lookup (L"ReleaseId"sv)) {
                platformVersion = o.As<String> ();
            }
            if (auto o = kWinVersionInfo_.Lookup (L"ProductName"sv)) {
                productName = o.As<String> ();
            }
            // try to get current version from CurrentMajorVersionNumber/CurrentMinorVersionNumber which appears
            // to be the new way
            try {
                if (auto oMajor = kWinVersionInfo_.Lookup (L"CurrentMajorVersionNumber"sv)) {
                    if (auto oMinor = kWinVersionInfo_.Lookup (L"CurrentMinorVersionNumber"sv)) {
                        currentVersion = oMajor.As<String> () + L"." + oMinor.As<String> ();
                    }
                }
            }
            catch (...) {
                // ignore - older OS may not have this so fallthrough (though that shouldn't cause exception but in case)
            }
            if (not currentVersion) {
                if (auto o = kWinVersionInfo_.Lookup (L"CurrentVersion"sv)) {
                    currentVersion = o.As<String> ();
                }
            }
        }
        catch (...) {
            DbgTrace (L"Exception suppressed looking up windows version in registry: %s", Characters::ToString (current_exception ()).c_str ());
        }

        if (tmp.fShortPrettyName.empty ()) {
            tmp.fShortPrettyName = productName.value_or (L"Windows"sv);
        }
        tmp.fPrettyNameWithMajorVersion = tmp.fShortPrettyName;

        {
            StringBuilder sb = tmp.fShortPrettyName;
            if (platformVersion) {
                sb += L" Version "sv + *platformVersion;
            }
            if (not kernelVersion.empty ()) {
                sb += L" (OS Build "sv + kernelOSBuildVersion + L")"sv;
            }
            tmp.fPrettyNameWithVersionDetails = sb.str ();
        }

        tmp.fMajorMinorVersionString              = currentVersion.value_or (L"unknown"sv);
        tmp.fRFC1945CompatProductTokenWithVersion = L"Windows/"sv + tmp.fMajorMinorVersionString;
        if constexpr (sizeof (void*) == 4) {
            tmp.fBits = 32;
            //IsWow64Process is not available on all supported versions of Windows.
            //Use GetModuleHandle to get a handle to the DLL that contains the function
            //and GetProcAddress to get a pointer to the function if available.
            typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
            DISABLE_COMPILER_MSC_WARNING_START (6387) // ignore check for null GetModuleHandle - if that fails - we have bigger problems and a crash sounds imminent
            LPFN_ISWOW64PROCESS isWow64Process = (LPFN_ISWOW64PROCESS)::GetProcAddress (::GetModuleHandle (TEXT ("kernel32")), "IsWow64Process");
            DISABLE_COMPILER_MSC_WARNING_END (6387)
            if (nullptr != isWow64Process) {
                BOOL isWOW64 = false;
                (void)isWow64Process (::GetCurrentProcess (), &isWOW64);
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

#if qPlatform_Windows
#pragma comment(lib, "Mincore.lib") // for stuff like IsWindows10OrGreater
#endif
SystemConfiguration::OperatingSystem Configuration::GetSystemConfiguration_ApparentOperatingSystem ()
{
    using OperatingSystem                       = SystemConfiguration::OperatingSystem;
    static const OperatingSystem kCachedResult_ = [] () -> OperatingSystem {
        OperatingSystem tmp{GetSystemConfiguration_ActualOperatingSystem ()};
        // not sure if/how to do this differently on linux? Probably pay MORE attention to stuff from uname and less to stuff like /etc/os-release
#if qPlatform_Windows
        // Dizzy numbering - from https://docs.microsoft.com/en-us/windows/desktop/sysinfo/operating-system-version
        optional<String> winCompatabilityVersionName;
        optional<String> winCompatabilityVersionNumber;
        {
            if (not winCompatabilityVersionName and IsWindows10OrGreater ()) {
                winCompatabilityVersionName   = L"10.0"sv;
                winCompatabilityVersionNumber = L"10.0"sv;
            }
            if (not winCompatabilityVersionName and IsWindows8Point1OrGreater ()) {
                winCompatabilityVersionName   = L"8.1"sv;
                winCompatabilityVersionNumber = L"6.3"sv;
            }
            if (not winCompatabilityVersionName and IsWindows8OrGreater ()) {
                winCompatabilityVersionName   = L"8.0"sv;
                winCompatabilityVersionNumber = L"6.2"sv;
            }
            if (not winCompatabilityVersionName and IsWindows7SP1OrGreater ()) {
                // unclear cuz 7.1 not listed as operating system on that page???
                winCompatabilityVersionName   = L"7.1"sv;
                winCompatabilityVersionNumber = L"6.2"sv;
            }
            if (not winCompatabilityVersionName and IsWindows7OrGreater ()) {
                winCompatabilityVersionName   = L"7.0"sv;
                winCompatabilityVersionNumber = L"6.1"sv;
            }
        }
        String useWinMajorMinorVersionNameStr     = winCompatabilityVersionName.value_or (L"unknown"sv);
        String useWinMajorMinorVersionNumberStr   = winCompatabilityVersionNumber.value_or (L"unknown"sv);
        tmp.fShortPrettyName                      = L"Windows "_k + useWinMajorMinorVersionNameStr;
        tmp.fPrettyNameWithMajorVersion           = tmp.fShortPrettyName;
        tmp.fPrettyNameWithVersionDetails         = tmp.fShortPrettyName;
        tmp.fMajorMinorVersionString              = useWinMajorMinorVersionNumberStr;
        tmp.fRFC1945CompatProductTokenWithVersion = L"Windows/"sv + useWinMajorMinorVersionNumberStr;
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
    Execution::ThrowPOSIXErrNoIfNegative (::gethostname (nameBuf, NEltsOf (nameBuf)));
    nameBuf[NEltsOf (nameBuf) - 1] = '\0'; // http://linux.die.net/man/2/gethostname says not necessarily nul-terminated
    result.fHostname               = String::FromNarrowSDKString (nameBuf);
#elif qPlatform_Windows
    constexpr COMPUTER_NAME_FORMAT kUseNameFormat_ = ComputerNameNetBIOS; // total WAG -- LGP 2014-10-10
    DWORD                          dwSize          = 0;
    (void)::GetComputerNameEx (kUseNameFormat_, nullptr, &dwSize);
    SmallStackBuffer<SDKChar> buf (SmallStackBufferCommon::eUninitialized, dwSize);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (::GetComputerNameEx (kUseNameFormat_, buf, &dwSize));
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
        GetSystemConfiguration_ActualOperatingSystem (),
        GetSystemConfiguration_ApparentOperatingSystem (),
        GetSystemConfiguration_ComputerNames ()};
}

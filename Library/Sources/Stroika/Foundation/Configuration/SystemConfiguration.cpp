/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <climits>
#include <filesystem>
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

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/SDKString.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Configuration/Platform/Windows/Registry.h"
#endif
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileSystem.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "SystemConfiguration.h"

#if qPlatform_POSIX
#include "Stroika/Foundation/DataExchange/Variant/INI/Reader.h"
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#include "Stroika/Foundation/Streams/iostream/FStreamSupport.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Configuration;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Time;

using Memory::StackBuffer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ***************** SystemConfiguration::BootInformation *************************
 ********************************************************************************
 */
String SystemConfiguration::BootInformation::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Booted-At: "sv << fBootedAt;
    sb << "}"sv;
    return sb;
};

/*
 ********************************************************************************
 ***************** SystemConfiguration::CPU::CoreDetails ************************
 ********************************************************************************
 */

String SystemConfiguration::CPU::CoreDetails::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Socket-ID: "sv << fSocketID << ", "sv;
    sb << "Model-Name: "sv << fModelName;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************** SystemConfiguration::CPU ****************************
 ********************************************************************************
 */
String SystemConfiguration::CPU::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Cores: "sv << fCores;
    sb << "}"sv;
    return sb;
};

/*
 ********************************************************************************
 ************************** SystemConfiguration::Memory *************************
 ********************************************************************************
 */
String SystemConfiguration::Memory::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Page-Size: "sv << fPageSize << ", "sv;
    sb << "Total-Physical-RAM: "sv << fTotalPhysicalRAM << ", "sv;
    sb << "Total-Virtual-RAM: "sv << fTotalVirtualRAM;
    sb << "}"sv;
    return sb;
};

/*
 ********************************************************************************
 ********************** SystemConfiguration::ComputerNames **********************
 ********************************************************************************
 */
String SystemConfiguration::ComputerNames::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Hostname: "sv << fHostname;
    sb << "}"sv;
    return sb;
};

/*
 ********************************************************************************
 ********************* SystemConfiguration::OperatingSystem *********************
 ********************************************************************************
 */
String SystemConfiguration::OperatingSystem::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Token-Name: "sv + fTokenName << ", "sv;
    sb << "Short-Pretty-Name: "sv + fShortPrettyName + ", "sv;
    sb << "Pretty-Name-With-Major-Version: "sv + fPrettyNameWithMajorVersion + ", "sv;
    sb << "Pretty-Name-With-Details: "sv + fPrettyNameWithVersionDetails + ", "sv;
    sb << "Major-Minor-Version-String: "sv + fMajorMinorVersionString + ", "sv;
    sb << "RFC1945-Compat-Product-Token-With-Version: "sv + fRFC1945CompatProductTokenWithVersion + ", "sv;
    sb << "Bits: "sv << fBits << ", "sv;
    if (fPreferedInstallerTechnology) {
        sb << "Preferred-Installer-Technology: "sv << *fPreferedInstallerTechnology;
    }
    sb << "}"sv;
    return sb;
};

/*
 ********************************************************************************
 ******************************* SystemConfiguration ****************************
 ********************************************************************************
 */
String SystemConfiguration::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Boot-Information: " << fBootInformation << ", "sv;
    sb << "CPU: "sv << fCPU << ", "sv;
    sb << "Memory: "sv << fMemory << ", "sv;
    sb << "Actual-Operating-System: "sv << fActualOperatingSystem << ", "sv;
    sb << "Apparent-Operating-System: "sv << fApparentOperatingSystem << ", "sv;
    sb << "Computer-Names: "sv << fComputerNames;
    sb << "}"sv;
    return sb;
};

/*
 ********************************************************************************
 ***************** Configuration::SystemConfiguration::CPU **********************
 ********************************************************************************
 */
unsigned int SystemConfiguration::CPU::GetNumberOfSockets () const
{
    Set<unsigned int> socketIds;
    for (const auto& i : fCores) {
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
        bool                          succeeded{false};
        static const filesystem::path kProcUptimeFileName_{"/proc/uptime"};
        if (IO::FileSystem::Default ().Access (kProcUptimeFileName_)) {
            /*
             *  From https://www.centos.org/docs/5/html/5.1/Deployment_Guide/s2-proc-uptime.html
             *      "The first number is the total number of seconds the system has been up"
             */
            using Characters::String2Int;
            for (const String& line :
                 TextReader::New (IO::FileSystem::FileInputStream::New (kProcUptimeFileName_, IO::FileSystem::FileInputStream::eNotSeekable))
                     .ReadLines ()) {
                Sequence<String> t = line.Tokenize ();
                if (t.size () >= 2) {
                    result.fBootedAt = DateTime::Now ().AddSeconds (-Characters::FloatConversion::ToFloat<double> (t[0]));
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
                    result.fBootedAt = DateTime{i->ut_tv};
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
        static const filesystem::path kProcCPUInfoFileName_{"/proc/cpuinfo"sv};
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
        for (const String& line :
             TextReader::New (IO::FileSystem::FileInputStream::New (kProcCPUInfoFileName_, IO::FileSystem::FileInputStream::eNotSeekable)).ReadLines ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("in Configuration::GetSystemConfiguration_CPU capture_ line={}"_f, line);
#endif
            static const String kOldProcessorLabel_{"Processor"sv};
            static const String kProcessorIDLabel_{"processor"sv};
            static const String kModelNameLabel_{"model name"sv};
            static const String kSocketIDLabel_{"physical id"sv}; // a bit of a guess?
            if (not line.Trim ().empty ()) {
                Sequence<String> lineTokens = line.Tokenize ({':'});
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
                    String useModelName = Memory::NullCoalesce (foundProcessor);
                    Memory::CopyToIf (&useModelName, currentModelName); // currentModelName takes precedence but I doubt both present
                    result.fCores.Append (CPU::CoreDetails{Memory::NullCoalesce (currentSocketID), useModelName});
                }
                // intentionally don't clear foundProcessor cuz occurs once it appears
                currentProcessorID = nullopt;
                currentModelName   = nullopt;
                currentSocketID    = nullopt;
            }
        }
        if (currentProcessorID) {
            String useModelName = Memory::NullCoalesce (foundProcessor);
            Memory::CopyToIf (&useModelName, currentModelName); // currentModelName takes precedence but I doubt both present
            result.fCores.Append (CPU::CoreDetails{Memory::NullCoalesce (currentSocketID), useModelName});
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
        StackBuffer<byte> buffer{Memory::eUninitialized, sizeof (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)};
        DWORD             returnLength = 0;
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
                    ++processorCoreCount;
                    // A hyperthreaded core supplies more than one logical processor.
                    logicalProcessorCount += countSetBits (ptr->ProcessorMask);
                    break;
                case RelationCache:
                    break;
                case RelationProcessorPackage:
                    // Logical processors share a physical package.
                    ++processorPackageCount;
                    break;
                default:
                    DbgTrace ("Error: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value."_f);
                    break;
            }
            byteOffset += sizeof (SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            ++ptr;
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
        return String{CPUBrandString};
    }();

    if constexpr (qDebug) {
        ::SYSTEM_INFO sysInfo{}; // GetNativeSystemInfo cannot fail so no need to initialize data
        ::GetNativeSystemInfo (&sysInfo);
        Assert (sysInfo.dwNumberOfProcessors == logicalProcessorCount);
    }
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
    ::SYSTEM_INFO sysInfo{};
    ::GetNativeSystemInfo (&sysInfo);
    result.fPageSize = sysInfo.dwPageSize;

    ::MEMORYSTATUSEX memStatus{};
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
        tmp.fTokenName = "Unix"sv;
        try {
            tmp.fTokenName = Execution::ProcessRunner{"uname"}.Run (String{}).Trim ();
        }
        catch (...) {
            DbgTrace ("Failure running uname"_f);
        }
        try {
            DataExchange::Variant::INI::Profile p =
                DataExchange::Variant::INI::Reader{}.ReadProfile (IO::FileSystem::FileInputStream::New ("/etc/os-release"sv));
            tmp.fShortPrettyName            = p.fUnnamedSection.fProperties.LookupValue ("NAME"sv);
            tmp.fPrettyNameWithMajorVersion = p.fUnnamedSection.fProperties.LookupValue ("PRETTY_NAME"sv);
            tmp.fMajorMinorVersionString    = p.fUnnamedSection.fProperties.LookupValue ("VERSION_ID"sv);
        }
        catch (...) {
            DbgTrace ("Failure reading /etc/os-release: {}"_f, current_exception ());
        }
        if (tmp.fShortPrettyName.empty ()) {
            try {
                String n = Streams::TextReader::New (IO::FileSystem::FileInputStream::New ("/etc/centos-release"sv)).ReadAll ().Trim ();
                tmp.fShortPrettyName            = "Centos"sv;
                tmp.fPrettyNameWithMajorVersion = n;
                Sequence<String> tokens         = n.Tokenize ();
                if (tokens.size () >= 3) {
                    tmp.fMajorMinorVersionString = tokens[2];
                }
            }
            catch (...) {
                DbgTrace ("Failure reading /etc/centos-release {}"_f, current_exception ());
            }
        }
        if (tmp.fShortPrettyName.empty ()) {
            try {
                String n = Streams::TextReader::New (IO::FileSystem::FileInputStream::New ("/etc/redhat-release"sv)).ReadAll ().Trim ();
                tmp.fShortPrettyName            = "RedHat"sv;
                tmp.fPrettyNameWithMajorVersion = n;
                Sequence<String> tokens         = n.Tokenize ();
                if (tokens.size () >= 3) {
                    tmp.fMajorMinorVersionString = tokens[2];
                }
            }
            catch (...) {
                DbgTrace ("Failure reading /etc/redhat-release {}"_f, current_exception ());
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
            tmp.fRFC1945CompatProductTokenWithVersion = tmp.fShortPrettyName.Trim ().ReplaceAll (" "sv, "-"sv);
            if (not tmp.fMajorMinorVersionString.empty ()) {
                tmp.fRFC1945CompatProductTokenWithVersion += "/"sv + tmp.fMajorMinorVersionString;
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
        DbgTrace ("_SC_V6_LP64_OFF64/_POSIX_V6_LP64_OFF64 not available - so assuming 32-bit..."_f);
#endif

        using Characters::CompareOptions;

        // No good way I can find to tell...
        if (not tmp.fPreferedInstallerTechnology.has_value ()) {
            auto nameEqComparer = String::EqualsComparer{eCaseInsensitive};
            if (nameEqComparer (tmp.fShortPrettyName, "Centos"sv) or nameEqComparer (tmp.fShortPrettyName, "RedHat"sv)) {
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eRPM;
            }
            else if (nameEqComparer (tmp.fShortPrettyName, "Ubuntu"sv)) {
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eDPKG;
            }
        }
        // not a great way to test since some systems have both, like ubuntu
        if (not tmp.fPreferedInstallerTechnology.has_value ()) {
            try {
                (void)Execution::ProcessRunner{"dpkg --help"}.Run (String{});
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eDPKG;
            }
            catch (...) {
            }
        }
        if (not tmp.fPreferedInstallerTechnology.has_value ()) {
            try {
                (void)Execution::ProcessRunner{"rpm --help"}.Run (String{});
                tmp.fPreferedInstallerTechnology = SystemConfiguration::OperatingSystem::InstallerTechnology::eRPM;
            }
            catch (...) {
            }
        }
#elif qPlatform_Windows
        tmp.fTokenName = "Windows"sv;

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
            const wchar_t*    kkernel32_ = L"kernel32.dll";
            DWORD             dummy;
            const auto        cbInfo = ::GetFileVersionInfoSizeExW (FILE_VER_GET_NEUTRAL, kkernel32_, &dummy);
            StackBuffer<char> buffer{Memory::eUninitialized, cbInfo};
            Verify (::GetFileVersionInfoExW (FILE_VER_GET_NEUTRAL, kkernel32_, dummy, static_cast<DWORD> (buffer.size ()), &buffer[0]));
            void* p    = nullptr;
            UINT  size = 0;
            Verify (::VerQueryValueW (buffer.data (), L"\\", &p, &size));
            Assert (size >= sizeof (VS_FIXEDFILEINFO));
            Assert (p != nullptr);
            auto pFixed          = static_cast<const VS_FIXEDFILEINFO*> (p);
            kernelVersion        = Characters::Format ("{}.{}"_f, HIWORD (pFixed->dwFileVersionMS), LOWORD (pFixed->dwFileVersionMS));
            kernelOSBuildVersion = Characters::Format ("{}.{}"_f, HIWORD (pFixed->dwFileVersionLS), LOWORD (pFixed->dwFileVersionLS));
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
            if (auto o = kWinVersionInfo_.Lookup ("ReleaseId"sv)) {
                platformVersion = o.As<String> ();
            }
            if (auto o = kWinVersionInfo_.Lookup ("ProductName"sv)) {
                productName = o.As<String> ();
            }
            // try to get current version from CurrentMajorVersionNumber/CurrentMinorVersionNumber which appears
            // to be the new way
            try {
                if (auto oMajor = kWinVersionInfo_.Lookup ("CurrentMajorVersionNumber"sv)) {
                    if (auto oMinor = kWinVersionInfo_.Lookup ("CurrentMinorVersionNumber"sv)) {
                        currentVersion = oMajor.As<String> () + "." + oMinor.As<String> ();
                    }
                }
            }
            catch (...) {
                // ignore - older OS may not have this so fallthrough (though that shouldn't cause exception but in case)
            }
            if (not currentVersion) {
                if (auto o = kWinVersionInfo_.Lookup ("CurrentVersion"sv)) {
                    currentVersion = o.As<String> ();
                }
            }
        }
        catch (...) {
            DbgTrace ("Exception suppressed looking up windows version in registry: {}"_f, current_exception ());
        }

        // https://stackoverflow.com/questions/74645458/how-to-detect-windows-11-programmatically
        if (currentVersion == "10.0"sv and Characters::FloatConversion::ToFloat<double> (kernelOSBuildVersion) >= 21996.0) {
            currentVersion = "11.0"sv;
            if (productName and productName->StartsWith ("Windows 10"sv)) {
                productName = "Windows 11"sv + productName->SubString (10); // not sure this is best way to fix? --LGP 2024-07-24
            }
            platformVersion = nullopt; // in this case, the number doesn't appear to be meaningful --LGP 2024-07-24
        }

        if (tmp.fShortPrettyName.empty ()) {
            tmp.fShortPrettyName = productName.value_or ("Windows"sv);
        }
        tmp.fPrettyNameWithMajorVersion = tmp.fShortPrettyName;

        {
            StringBuilder sb = tmp.fShortPrettyName;
            if (platformVersion) {
                sb << " Version "sv + *platformVersion;
            }
            if (not kernelVersion.empty ()) {
                sb << " (OS Build "sv + kernelOSBuildVersion + ")"sv;
            }
            tmp.fPrettyNameWithVersionDetails = sb.str ();
        }

        tmp.fMajorMinorVersionString              = currentVersion.value_or ("unknown"sv);
        tmp.fRFC1945CompatProductTokenWithVersion = "Windows/"sv + tmp.fMajorMinorVersionString;
        if constexpr (sizeof (void*) == 4) {
            tmp.fBits = 32;
            //IsWow64Process is not available on all supported versions of Windows.
            //Use GetModuleHandle to get a handle to the DLL that contains the function
            //and GetProcAddress to get a pointer to the function if available.
            typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
            DISABLE_COMPILER_MSC_WARNING_START (6387) // ignore check for null GetModuleHandle - if that fails - we have bigger problems and a crash sounds imminent
            LPFN_ISWOW64PROCESS isWow64Process =
                (LPFN_ISWOW64PROCESS)::GetProcAddress (::GetModuleHandle (TEXT ("kernel32")), "IsWow64Process");
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
        optional<String> winCompatibilityVersionName;
        optional<String> winCompatibilityVersionNumber;
        {
            if (not winCompatibilityVersionName and IsWindows10OrGreater ()) {
                winCompatibilityVersionName   = "10.0"sv;
                winCompatibilityVersionNumber = "10.0"sv;
            }
            if (not winCompatibilityVersionName and IsWindows8Point1OrGreater ()) {
                winCompatibilityVersionName   = "8.1"sv;
                winCompatibilityVersionNumber = "6.3"sv;
            }
            if (not winCompatibilityVersionName and IsWindows8OrGreater ()) {
                winCompatibilityVersionName   = "8.0"sv;
                winCompatibilityVersionNumber = "6.2"sv;
            }
            if (not winCompatibilityVersionName and IsWindows7SP1OrGreater ()) {
                // unclear cuz 7.1 not listed as operating system on that page???
                winCompatibilityVersionName   = "7.1"sv;
                winCompatibilityVersionNumber = "6.2"sv;
            }
            if (not winCompatibilityVersionName and IsWindows7OrGreater ()) {
                winCompatibilityVersionName   = "7.0"sv;
                winCompatibilityVersionNumber = "6.1"sv;
            }
        }
        String useWinMajorMinorVersionNameStr     = winCompatibilityVersionName.value_or ("unknown"sv);
        String useWinMajorMinorVersionNumberStr   = winCompatibilityVersionNumber.value_or ("unknown"sv);
        tmp.fShortPrettyName                      = "Windows "sv + useWinMajorMinorVersionNameStr;
        tmp.fPrettyNameWithMajorVersion           = tmp.fShortPrettyName;
        tmp.fPrettyNameWithVersionDetails         = tmp.fShortPrettyName;
        tmp.fMajorMinorVersionString              = useWinMajorMinorVersionNumberStr;
        tmp.fRFC1945CompatProductTokenWithVersion = "Windows/"sv + useWinMajorMinorVersionNumberStr;
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
        return String {};
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
#if defined(HOST_NAME_MAX)
    char nameBuf[HOST_NAME_MAX + 1]; // size from http://man7.org/linux/man-pages/man2/gethostname.2.html
#else
    char nameBuf[1024]; // Mac XCode 11 doesn't define HOST_NAME_MAX
#endif
    Execution::ThrowPOSIXErrNoIfNegative (::gethostname (nameBuf, Memory::NEltsOf (nameBuf)));
    nameBuf[Memory::NEltsOf (nameBuf) - 1] = '\0'; // http://linux.die.net/man/2/gethostname says not necessarily nul-terminated
    result.fHostname                       = String::FromNarrowSDKString (nameBuf);
#elif qPlatform_Windows
    constexpr COMPUTER_NAME_FORMAT kUseNameFormat_ = ComputerNameNetBIOS; // total WAG -- LGP 2014-10-10
    DWORD                          dwSize          = 0;
    (void)::GetComputerNameEx (kUseNameFormat_, nullptr, &dwSize);
    StackBuffer<SDKChar> buf{Memory::eUninitialized, dwSize};
    Execution::Platform::Windows::ThrowIfZeroGetLastError (::GetComputerNameEx (kUseNameFormat_, buf.data (), &dwSize));
    result.fHostname = String::FromSDKString (buf);
#else
    AssertNotImplemented ();
#endif
    return result;
}

/*
 ********************************************************************************
 ******************* Configuration::GetNumberOfLogicalCPUCores ******************
 ********************************************************************************
 */
unsigned int Configuration::GetNumberOfLogicalCPUCores (const chrono::duration<double>& allowedStaleness)
{
    [[maybe_unused]] static auto computeViaStdThreadHardwareConcurrency = [] () { return std::thread::hardware_concurrency (); };
    [[maybe_unused]] static auto computeViaGetSystemConfiguration_CPU   = [] () {
        return Configuration::GetSystemConfiguration_CPU ().GetNumberOfLogicalCores ();
    };
#if qDebug
    static auto compute = [=] () {
        unsigned int hc                = computeViaStdThreadHardwareConcurrency ();
        unsigned int sysConfigLogCores = computeViaGetSystemConfiguration_CPU ();
        WeakAssert (hc == sysConfigLogCores); // nice to test/find out if these ever differ
        return sysConfigLogCores;
    };
#else
    auto compute = computeViaGetSystemConfiguration_CPU; // maybe choose based on OS, etc???, like if I know which library does a good job with std::thread::hardware_concurrency
#endif

    static atomic<Time::TimePointSeconds> sCachedAt_    = Time::TimePointSeconds{};
    static atomic<unsigned int>           sCachedValue_ = compute ();
    Time::TimePointSeconds                now           = Time::GetTickCount ();
    if (now > sCachedAt_.load () + allowedStaleness) {
        sCachedValue_ = compute ();
        sCachedAt_    = now;
    }
    return sCachedValue_;
}

/*
 ********************************************************************************
 ****************** SystemConfiguration GetSystemConfiguration ******************
 ********************************************************************************
 */
SystemConfiguration Configuration::GetSystemConfiguration ()
{
    return SystemConfiguration{GetSystemConfiguration_BootInformation (),
                               GetSystemConfiguration_CPU (),
                               GetSystemConfiguration_Memory (),
                               GetSystemConfiguration_ActualOperatingSystem (),
                               GetSystemConfiguration_ApparentOperatingSystem (),
                               GetSystemConfiguration_ComputerNames ()};
}

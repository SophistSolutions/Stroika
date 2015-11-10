/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <thread>

#if     qPlatform_POSIX
#include    <unistd.h>
#include    <fstream>
#include    <sys/sysinfo.h>
#include    <utmpx.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Characters/SDKString.h"
#include    "../Characters/FloatConversion.h"
#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"
#include    "../Characters/StringBuilder.h"
#include    "../Characters/String2Int.h"
#include    "../Containers/Sequence.h"
#include    "../Containers/Set.h"
#if     qPlatform_POSIX
#include    "../Execution/ErrNoException.h"
#elif   qPlatform_Windows
#include    "../Execution/Platform/Windows/Exception.h"
#endif
#include    "../Memory/SmallStackBuffer.h"
#include    "../IO/FileSystem/FileInputStream.h"
#include    "../IO/FileSystem/FileSystem.h"
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
using   Memory::Byte;


// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1






#if     qPlatform_AIX
namespace {
    /*
     *      $ /usr/sbin/prtconf
     *      System Model: IBM,9133-55A
     *      Machine Serial Number: 101CD0H
     *      Processor Type: PowerPC_POWER5
     *      Processor Implementation Mode: POWER 5
     *      Processor Version: PV_5_3
     *      Number Of Processors: 2
     *      Processor Clock Speed: 1648 MHz
     *      CPU Type: 64-bit
     *      Kernel Type: 64-bit
     *      LPAR Info: 4 sovma156
     *      Memory Size: 3840 MB
     *      Good Memory Size: 3840 MB
     *      Platform Firmware level: SF240_418
     *      Firmware Version: IBM,SF240_418
     *      Console Login: enable
     *      Auto Restart: true
     *      Full Core: false
     *      NX Crypto Acceleration: Not Capable
     *
     *      Network Information
     *              Host Name: sovma156
     *              IP Address: 192.168.253.156
     *              Sub Netmask: 255.255.255.0
     *              Gateway: 192.168.253.1
     *              Name Server: 68.87.68.162
     *              Domain Name: siteox.com
     *
     *      Paging Space Information
     *              Total Paging Space: 11008MB
     *              Percent Used: 1%
     *
     *      Volume Groups Information
     *      ==============================================================================
     *      Active VGs
     *      ==============================================================================
     *      rootvg:
     *      PV_NAME           PV STATE          TOTAL PPs   FREE PPs    FREE DISTRIBUTION
     *      hdisk0            active            319         0           00..00..00..00..00
     *      hdisk1            active            798         0           00..00..00..00..00
     *      hdisk2            active            798         54          00..00..00..00..54
     *      ==============================================================================
     *
     *      INSTALLED RESOURCE LIST
     *
     *      The following resources are installed on the machine.
     *      +/- = Added or deleted from Resource List.
     *      *   = Diagnostic support not available.
     *
     *        Model Architecture: chrp
     *        Model Implementation: Multiple Processor, PCI bus
     *
     *      + sys0                                                             System Object
     *      + sysplanar0                                                       System Planar
     *      * vio0                                                             Virtual I/O Bus
     *      * vscsi9           U9133.55A.101CD0H-V4-C569-T1                    Virtual SCSI Client Adapter
     *      * vscsi8           U9133.55A.101CD0H-V4-C568-T1                    Virtual SCSI Client Adapter
     *      * vscsi7           U9133.55A.101CD0H-V4-C567-T1                    Virtual SCSI Client Adapter
     *      * vscsi6           U9133.55A.101CD0H-V4-C566-T1                    Virtual SCSI Client Adapter
     *      * vscsi5           U9133.55A.101CD0H-V4-C565-T1                    Virtual SCSI Client Adapter
     *      * vscsi4           U9133.55A.101CD0H-V4-C564-T1                    Virtual SCSI Client Adapter
     *      * hdisk2           U9133.55A.101CD0H-V4-C564-T1-L8100000000000000  Virtual SCSI Disk Drive
     *      * vscsi3           U9133.55A.101CD0H-V4-C563-T1                    Virtual SCSI Client Adapter
     *      * vscsi2           U9133.55A.101CD0H-V4-C562-T1                    Virtual SCSI Client Adapter
     *      * hdisk1           U9133.55A.101CD0H-V4-C562-T1-L8100000000000000  Virtual SCSI Disk Drive
     *      * vscsi1           U9133.55A.101CD0H-V4-C561-T1                    Virtual SCSI Client Adapter
     *      * vscsi0           U9133.55A.101CD0H-V4-C560-T1                    Virtual SCSI Client Adapter
     *      * hdisk0           U9133.55A.101CD0H-V4-C560-T1-L8100000000000000  Virtual SCSI Disk Drive
     *      * ent9             U9133.55A.101CD0H-V4-C359-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent8             U9133.55A.101CD0H-V4-C358-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent7             U9133.55A.101CD0H-V4-C357-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent6             U9133.55A.101CD0H-V4-C356-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent5             U9133.55A.101CD0H-V4-C355-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent4             U9133.55A.101CD0H-V4-C354-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent3             U9133.55A.101CD0H-V4-C353-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent2             U9133.55A.101CD0H-V4-C352-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent1             U9133.55A.101CD0H-V4-C351-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * ent0             U9133.55A.101CD0H-V4-C350-T1                    Virtual I/O Ethernet Adapter (l-lan)
     *      * vsa0             U9133.55A.101CD0H-V4-C0                         LPAR Virtual Serial Adapter
     *      * vty0             U9133.55A.101CD0H-V4-C0-L0                      Asynchronous Terminal
     *      + L2cache0                                                         L2 Cache
     *      + mem0                                                             Memory
     *      + proc0                                                            Processor
     *      + proc2                                                            Processor
     */
    struct  prtconf_ {
        String      ProcessorType;
        String      ProcessorImplementationMode;
        String      ProcessorVersion;
        uint        NumberOfProcessors {};
        String      ProcessorClockSpeed;
        uint64_t    MemorySize { };
    };
    prtconf_   get_prtconf_ ()
    {
        using   Memory::Byte;
        auto fetcher_ = [] () {
            try {
                using  Execution::ProcessRunner;
                ProcessRunner   pr (String_Constant {L"/usr/sbin/prtconf"});
                Streams::MemoryStream<Byte>   useStdOut;
                pr.SetStdOut (useStdOut);
                pr.Run ();
                Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
                prtconf_    result;
                for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                    Sequence<String>    tokens = i.Tokenize (Containers::Set<Character> {':'});
                    if (tokens.size () == 2 and tokens[0] == L"Processor Type") {
                        result.ProcessorType = tokens[1];
                    }
                    else if (tokens.size () == 2 and tokens[0] == L"Number Of Processors") {
                        result.NumberOfProcessors = Characters::String2Int<uint> (tokens[1]);
                    }
                    else if (tokens.size () == 2 and tokens[0] == L"Processor Implementation Mode") {
                        result.ProcessorImplementationMode = tokens[1];
                    }
                    else if (tokens.size () == 2 and tokens[0] == L"Processor Version") {
                        result.ProcessorVersion = tokens[1];
                    }
                    else if (tokens.size () == 2 and tokens[0] == L"Processor Clock Speed") {
                        result.ProcessorClockSpeed = tokens[1];
                    }
                    else if (tokens.size () == 2 and tokens[0] == L"Good Memory Size") {
                        Sequence<String>    memSizeTokenized    = tokens[1].Tokenize ();
                        if (not memSizeTokenized.empty ()) {
                            uint64_t            memSize = Characters::String2Int<uint64_t> (memSizeTokenized[0]);
                            if (memSizeTokenized.size () >= 2 and memSizeTokenized[1] == L"KB") {
                                memSize *= 1024;
                            }
                            else if (memSizeTokenized.size () >= 2 and memSizeTokenized[1] == L"MB") {
                                memSize *= 1024 * 1024;
                            }
                            else if (memSizeTokenized.size () >= 2 and memSizeTokenized[1] == L"GB") {
                                memSize *= 1024 * 1024 * 1024;
                            }
                            result.MemorySize = memSize;
                        }
                    }
                }
                return result;
            }
            catch (...) {
                DbgTrace ("FAILURE calling external /usr/sbin/prtconf");
                return prtconf_ ();
            }
        };
        static prtconf_    kCached_ = fetcher_ ();
        return kCached_;
    }
    /*
     *  $ /usr/bin/mpstat -h
     *
     *  System configuration: lcpu=4 ent=0.2 mode=Uncapped
     *
     *  cpu    pc   ilcs   vlcs
     *    0  0.00 750666 13711321
     *    1  0.00  87070 14246608
     *    2  0.00 291782 3685703
     *    3  0.00 1734190 2462243
     *  ALL  0.00 2863708 34105875
     */
    struct  mpstat_cpuinfo_ {
        uint        NumberOfCores {};
    };
    mpstat_cpuinfo_    get_mpstat_cpuinfo_ ()
    {
        auto fetcher_ = [] () {
            try {
                using  Execution::ProcessRunner;
                ProcessRunner   pr (String_Constant {L"/usr/bin/mpstat -h"});
                Streams::MemoryStream<Byte>   useStdOut;
                pr.SetStdOut (useStdOut);
                pr.Run ();
                Streams::TextReader   stdOut  =   Streams::TextReader (useStdOut);
                mpstat_cpuinfo_ result;
                bool    sawCPUHeader = false;
                bool    sawAllCPU = false;
                uint    nCPUs = 0;
                for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                    Sequence<String>    tokens = i.Tokenize ();
                    if (not sawCPUHeader) {
                        if (tokens.size () >= 2 and tokens[0] == L"cpu") {
                            sawCPUHeader = true;
                            continue;
                        }
                    }
                    else if (not sawAllCPU) {
                        if (tokens.size () >= 2) {
                            if (tokens[0] == L"ALL") {
                                sawAllCPU = true;
                            }
                            else {
                                nCPUs++;
                            }
                        }
                    }
                }
                result.NumberOfCores = nCPUs;
                return result;

            }
            catch (...) {
                DbgTrace ("FAILURE calling external /usr/bin/mpstat");
                return mpstat_cpuinfo_ ();
            }
        };
        static mpstat_cpuinfo_ kCached_ = fetcher_ ();
        return kCached_;
    }
}
#endif






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
#if     qPlatform_Linux
    struct  sysinfo     info;
    ::sysinfo (&info);
    result.fBootedAt = DateTime::Now ().AddSeconds (-info.uptime);
#elif   qPlatform_POSIX
    {
        // @todo - I dont think /proc/uptime is POSIX ... NOT SURE HOW TO DEFINE THIS - MAYBE ONLY .... on LINUX?
        bool    succeeded { false };
        const   String_Constant kProcUptimeFileName_ { L"/proc/uptime" };
        if (IO::FileSystem::FileSystem::Default ().Access (kProcUptimeFileName_)) {
            /*
             *  From https://www.centos.org/docs/5/html/5.1/Deployment_Guide/s2-proc-uptime.html
             *      "The first number is the total number of seconds the system has been up"
             */
            using   Streams::TextReader;
            using   IO::FileSystem::FileInputStream;
            using   Characters::String2Int;
            for (String line : TextReader (FileInputStream::mk (kProcUptimeFileName_, FileInputStream::eNotSeekable)).ReadLines ()) {
                Sequence<String>    t = line.Tokenize ();
                if (t.size () >= 2) {
                    result.fBootedAt = DateTime::Now ().AddSeconds (-Characters::String2Float<double> (t[0]));
                    succeeded = true;
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
            Execution::Finally cleanup ([] () {
                ::endutxent ();
            });
            ::setutxent ();
            for (const utmpx* i = ::getutxent (); i != nullptr; i = ::getutxent ()) {
                if (i->ut_type == BOOT_TIME) {
                    result.fBootedAt = DateTime (i->ut_tv);
                    succeeded = true;
                }
            }
        }
        Assert (succeeded); // not a real assert, but sort of a warning if this ever gets triggered
    }
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
#if     qPlatform_AIX
    prtconf_   ptrConf = get_prtconf_ ();
    String     showProcessorType = ptrConf.ProcessorType;
    if (not ptrConf.ProcessorImplementationMode.empty ()) {
        showProcessorType += L" " + ptrConf.ProcessorImplementationMode;
    }
    if (not ptrConf.ProcessorVersion.empty ()) {
        showProcessorType += L" " + ptrConf.ProcessorVersion;
    }
    if (not ptrConf.ProcessorClockSpeed.empty ()) {
        showProcessorType += L" " + ptrConf.ProcessorClockSpeed;
    }

    mpstat_cpuinfo_    mpConf = get_mpstat_cpuinfo_ ();

    // if anything looks bad, just say one processor and one core
    if (mpConf.NumberOfCores == 0) {
        DbgTrace ("BAD  = NumberOfCores=0");
        mpConf.NumberOfCores = 1;
    }
    if (ptrConf.NumberOfProcessors == 0) {
        DbgTrace ("BAD  = NumberOfProcessors=0");
        ptrConf.NumberOfProcessors = 1;
    }
    if (mpConf.NumberOfCores < ptrConf.NumberOfProcessors) {
        DbgTrace ("BAD  = mpConf.NumberOfCores (%d) < ptrConf.NumberOfProcessors(%d)", mpConf.NumberOfCores, ptrConf.NumberOfProcessors);
        ptrConf.NumberOfProcessors = mpConf.NumberOfCores;
    }
    if (mpConf.NumberOfCores % ptrConf.NumberOfProcessors != 0) {
        DbgTrace ("BAD  = mpConf.NumberOfCores (%d) % ptrConf.NumberOfProcessors(%d) != 0", mpConf.NumberOfCores, ptrConf.NumberOfProcessors);
        ptrConf.NumberOfProcessors = mpConf.NumberOfCores;
    }

    uint    nCoresPerSocket = mpConf.NumberOfCores / ptrConf.NumberOfProcessors;
    Assert (nCoresPerSocket >= 1);

    uint curSocketID = 0;
    uint curCoreForSocketIdx = 0;

    for (uint i = 0; i < mpConf.NumberOfCores; ++i) {
        Assert (curSocketID < ptrConf.NumberOfProcessors);
        CPU::CoreDetails    tmp;
        tmp.fSocketID = curSocketID;
        tmp.fModelName = showProcessorType;
        result.fCores.Append (tmp);
        curCoreForSocketIdx++;
        if (curCoreForSocketIdx >= nCoresPerSocket) {
            curCoreForSocketIdx = 0;
            curSocketID++;
        }
    }
#elif   qPlatform_Linux
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
#if     qPlatform_AIX
    prtconf_   ptrConf = get_prtconf_ ();
    result.fPageSize = ::sysconf (_SC_PAGESIZE);
    result.fTotalPhysicalRAM = ptrConf.MemorySize;      // @todo UNCLEAR why ::sysconf (_SC_PHYS_PAGES) * result.fPageSize different than this? --LGP 2015-09-08
#elif   qPlatform_POSIX
    // page size cannot change while running, but number of pages can
    // (e.g. https://pubs.vmware.com/vsphere-50/index.jsp?topic=%2Fcom.vmware.vsphere.vm_admin.doc_50%2FGUID-0B4C3128-F854-43B9-9D80-A20C0C8B0FF7.html)
    static  const   size_t  kPageSize_   { static_cast<size_t> (::sysconf (_SC_PAGESIZE)) };
    result.fPageSize = kPageSize_;
    result.fTotalPhysicalRAM = ::sysconf (_SC_PHYS_PAGES) * kPageSize_;
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
    /*
     *  Someday it would be nice to find a better way, but as of 2015-10-19, I've not been able to find one (without using WMI).
     */
    String  GetWinOSDisplayString_ ()
    {
        OSVERSIONINFOEX osvi {};
        {
            osvi.dwOSVersionInfoSize = sizeof(osvi);
            // MSFT is crazy. They deprecate GetVersionEx, but then still require it for GetProductInfo, and provide no
            // other way to find the product description string?
            // I spent over an hour looking. Give up for now. Sigh...
            //      --LGP 2015-10-19
            DISABLE_COMPILER_MSC_WARNING_START(4996)
            if (not ::GetVersionEx ((OSVERSIONINFO*) &osvi)) {
                return String ();
            }
            DISABLE_COMPILER_MSC_WARNING_END(4996)
        }

        HMODULE     kernel32 = ::GetModuleHandle (TEXT ("kernel32.dll"));
        AssertNotNull (kernel32);

        if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT) {
            return Characters::Format (L"Unknown Windows Version %d %d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        }
        if (osvi.dwMajorVersion <= 4) {
            return Characters::Format (L"Unknown Ancient Windows Version %d %d", osvi.dwMajorVersion, osvi.dwMinorVersion);
        }

        SYSTEM_INFO si {};
        {
            // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
            using PGNSI = void (WINAPI*)(LPSYSTEM_INFO);
            PGNSI pGNSI = (PGNSI)::GetProcAddress (kernel32, "GetNativeSystemInfo");
            if (pGNSI == nullptr) {
                ::GetSystemInfo (&si);
            }
            else {
                (*pGNSI) (&si);
            }
        }

        String  goodName;
        switch (osvi.dwMajorVersion) {
            case 10: {
                    if (osvi.dwMinorVersion == 0)  {
                        goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 10 " : L"Windows Server 2016 ";
                    }
                }
                break;
            case 6: {
                    if (osvi.dwMinorVersion == 0)  {
                        goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows Vista " : L"Windows Server 2008 ";
                    }
                    else if (osvi.dwMinorVersion == 1 )  {
                        goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 7 " : L"Windows Server 2008 R2 ";
                    }
                    else if (osvi.dwMinorVersion == 2 )  {
                        goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 8 " : L"Windows Server 2012 ";
                    }
                    else if (osvi.dwMinorVersion == 3)  {
                        goodName = (osvi.wProductType == VER_NT_WORKSTATION) ? L"Windows 8.1 " : L"Windows Server 2012 R2 ";
                    }
                }
                break;
            case 5: {
                    if (osvi.dwMinorVersion == 0) {
                        goodName = L"Windows 2000 ";
                        if (osvi.wProductType == VER_NT_WORKSTATION) {
                            goodName += L"Professional";
                        }
                        else  {
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
                    else if (osvi.dwMinorVersion == 1)  {
                        goodName = L"Windows XP ";
                        goodName += (osvi.wSuiteMask & VER_SUITE_PERSONAL) ? L"Home Edition" :  L"Professional";
                    }
                    else if ( osvi.dwMinorVersion == 2)  {
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
                        if (osvi.wProductType != VER_NT_WORKSTATION)  {
                            if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
                                if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                                    goodName += L"Datacenter Edition for Itanium-based Systems";
                                }
                                else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
                                    goodName += L"Enterprise Edition for Itanium-based Systems";
                                }
                            }
                            else if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ) {
                                if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
                                    goodName += L"Datacenter x64 Edition";
                                }
                                else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE ) {
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
                }
                break;
            default: {
                    Assert (osvi.dwMajorVersion > 10);  //. or we should have hit a case... (or msft went back in time???)
                    // no need to fill in name, cuz just lik eother cases with no goodName
                }
                break;
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
            DWORD   dwType = PRODUCT_UNDEFINED;
            {
                typedef BOOL (WINAPI * PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
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

        result += Characters::Format (L" (build %d)",  osvi.dwBuildNumber);

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

#if     defined (_POSIX_V6_LP64_OFF64)
        //
        // @todo FIX/FIND BETTER WAY!
        //
        //http://docs.oracle.com/cd/E36784_01/html/E36874/sysconf-3c.html
        // Quite uncertain - this is not a good reference
        //      --LGP 2014-10-18
        //
        tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _POSIX_V6_LP64_OFF64 ? 64 : 32;
#elif   defined ( _V6_LP64_OFF64)
        //AIX
        tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _V6_LP64_OFF64 ? 64 : 32;
#else
        // could be a C+++ const - let it not compile if not available, and we'll dig...
        //tmp.fBits = ::sysconf (_SC_V6_LP64_OFF64) == _POSIX_V6_LP64_OFF64 ? 64 : 32;
        DbgTrace ("_SC_V6_LP64_OFF64/_POSIX_V6_LP64_OFF64 not available - so assuming 32-bit...");
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
        OSVERSIONINFOEX   osvi {};
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
            if (NULL != fnIsWow64Process) {
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

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SystemConfiguration_h_
#define _Stroika_Foundation_Configuration_SystemConfiguration_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Containers/Sequence.h"
#include    "../Time/DateTime.h"

#include    "Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   For CPUInfo, we have socketcount, and #logical cores (what msft calls logical processors in some places)
 *              but we could use a placeholder for #physical cores, maybe. MSFT has this. Not sure its sueful. Document
 *              why/why not depnding on what we decide.
 *
 *      @todo   Quite incomplete, but hopefully usable implementation. Implment the rest of the attributes provided.
 *              some using linux /procfs, etc.
 *
 *      @todo   Review API provide, and document relationship with sysconf/etc (idea is simpler, and a bit more portable,
 *              but just subset).
 *
 *      @todo   BootInformation: consider adding info like 'last shutdown at' and last shutdown reason.
 *
 *      @todo   BootInformation: simplistic impl - may not handle 'hibernate' properly... - must verify/test, and timezone
 *              change etc... just uses seconds since boot on most plantforms??
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            using   Characters::String;


            /**
             */
            struct  SystemConfiguration {


                /**
                 */
                struct  BootInformation {
                    Time::DateTime  fBootedAt;
                };


                /**
                 */
                struct  CPU {
                public:
                    /**
                     *  Number of Physical Sockets/CPU chips. Also this is the number of distinct 'socketids' from the fCores.
                     */
                    nonvirtual  unsigned int    GetNumberOfSockets () const;

                public:
                    /**
                     *  Number of Logical Cores (aka  max concurrent logical thread count). This will generally be
                     *  divisible by fNumberOfSockets.
                     */
                    nonvirtual  unsigned int    GetNumberOfLogicalCores () const;

                public:
                    /**
                     *  Each socket will typically have the identical model name. This returns the value from the first.
                     *  check each fCore to see if they differ.
                     *
                     *  If the fCores is empty, this is safe, and returns an empty string.
                     */
                    nonvirtual  String    GetCPUModelPrintName () const;

                public:
                    /**
                     *  Details we track per CPU (socket/chip). There is much more info in /proc/cpuinfo, like
                     *  MHz, and cache size, and particular numerical model numbers. Possibly also add 'bogomips'?
                     */
                    struct  CoreDetails {
                        unsigned int    fSocketID {};       // /proc/cpuinfo 'physical id' - use to tell numper of sockets. Each distinct socketID is a different socket
                        String          fModelName {};      // /proc/cpuinfo 'model name' field - a semi-standardized representation of what you want to know about a CPU chip
                    };

                public:
                    /**
                     *  A computer may have multiple CPUCores, and in principle they can differ.
                     *  The number of filled 'cpu sockets' is fCPUs.length ().
                     *
                     *  Note that these are 'logical cores' and may not be physical cores.
                     */
                    Containers::Sequence<CoreDetails>   fCores;
                };


                /**
                 */
                struct  Memory {
                    /**
                     *  Size in bytes
                     */
                    size_t      fPageSize {};

                    /**
                     *  Size in bytes
                     */
                    uint64_t    fTotalPhysicalRAM {};

                    /**
                     *  Size in bytes
                     */
                    uint64_t    fTotalVirtualRAM {};
                };


                /**
                 */
                struct  OperatingSystem {
                    /**
                     *  e.g. Linux/MacOS/Windows/Unix (on POSIX systems - value of uname)
                     */
                    String  fTokenName;

                    /**
                     *  e.g. Linux/Ubuntu,RedHat, Windows XP, Windows 2000
                     */
                    String  fShortPrettyName;

                    /**
                     *  e.g. RedHat 3.5, Ubuntu 11, Windows XP, Windows 8, Windows 8.1
                     */
                    String  fPrettyNameWithMajorVersion;

                    /**
                     *  e.g. 1.0, 3.5, etc
                     */
                    String  fMajorMinorVersionString;

                    /**
                     *  http://tools.ietf.org/html/rfc1945#section-10.15
                     *  http://tools.ietf.org/html/rfc1945#section-3.7
                     *
                     *  e.g. MyProduct/1.0, Mozilla/3.5, etc
                     */
                    String  fRFC1945CompatProductTokenWithVersion;

                    /**
                     *  Number of bits the OS targets. Often a 64-bit OS will support 32-bits, and concievably other
                     *  combinations are possible. But this value returns the principle / primary number of bits supported
                     *  by the OS (bits of addressing).
                     */
                    unsigned int    fBits { 32 };
                };


                /**
                 *  This is very frequently NOT useful, not unique, but frequently desired/used,
                 *  so at least you can get to it uniformly, cross-platform.
                 */
                struct  ComputerNames {
                    /**
                     *  Returns the best OS dependent guess at a computer name we have.
                     *
                     *  On windows, this amounts to the NETBIOS name, and on UNIX, this amounts
                     *  to the result of 'gethostname' (man 2 hostname).
                     */
                    String  fHostname;
                };


                BootInformation fBootInformation;
                CPU             fCPU;
                Memory          fMemory;
                OperatingSystem fOperatingSystem;
                ComputerNames   fComputerNames;
            };


            /**
             */
            SystemConfiguration GetSystemConfiguration ();


            /**
             */
            SystemConfiguration::BootInformation GetSystemConfiguration_BootInformation ();


            /**
             */
            SystemConfiguration::CPU GetSystemConfiguration_CPU ();


            /**
             */
            SystemConfiguration::Memory GetSystemConfiguration_Memory ();


            /**
             */
            SystemConfiguration::OperatingSystem GetSystemConfiguration_OperatingSystem ();


            /**
             */
            SystemConfiguration::ComputerNames GetSystemConfiguration_ComputerNames ();


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SystemConfiguration.inl"

#endif  /*_Stroika_Foundation_Configuration_SystemConfiguration_h_*/

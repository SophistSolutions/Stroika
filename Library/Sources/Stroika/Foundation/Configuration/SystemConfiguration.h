/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SystemConfiguration_h_
#define _Stroika_Foundation_Configuration_SystemConfiguration_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Containers/Sequence.h"
#include    "Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Quite incomplete, but hopefully usable implementation. Implment the rest of the attributes provided.
 *              some using linux /procfs, etc.
 *
 *      @todo   Review API provide, and document relationship with sysconf/etc (idea is simpler, and a bit more portable,
 *              but just subset).
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
                        unsigned int    fSocketID {};       // /proc/cpuinfo 'physical id' - use to tell numper of sockets. Each distinct socketID is a differnt socket
                        String          fModelName {};      // /proc/cpuinfo 'model name' field - a semi-standardized representation of what you want to know about a CPU chip
                    };

                public:
                    /**
                     *  A computer may have multiple SPUCores, and in principle they can differ.
                     *  The number of filled 'cpu sockets' is fCPUs.length ()
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

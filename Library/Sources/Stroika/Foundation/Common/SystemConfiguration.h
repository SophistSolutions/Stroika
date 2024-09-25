/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_SystemConfiguration_h_
#define _Stroika_Foundation_Common_SystemConfiguration_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Time/DateTime.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   For CPUInfo, we have socketcount, and #logical cores (what msft calls logical processors in some places)
 *              but we could use a placeholder for #physical cores, maybe. MSFT has this. Not sure its sueful. Document
 *              why/why not depnding on what we decide.
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

namespace Stroika::Foundation::Common {

    using Characters::String;

    /**
     */
    struct SystemConfiguration {

        /**
         */
        struct BootInformation {
            optional<Time::DateTime> fBootedAt;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         */
        struct CPU {
        public:
            /**
             *  Number of Physical Sockets/CPU chips. Also this is the number of distinct 'socketids' from the fCores.
             */
            nonvirtual unsigned int GetNumberOfSockets () const;

        public:
            /**
             *  Number of Logical Cores (aka  max concurrent logical thread count). This will generally be
             *  divisible by fNumberOfSockets.
             * 
             *  @see Common::GetNumberOfLogicalCPUCores
             */
            nonvirtual unsigned int GetNumberOfLogicalCores () const;

        public:
            /**
             *  Each socket will typically have the identical model name. This returns the value from the first.
             *  check each fCore to see if they differ.
             *
             *  If the fCores is empty, this is safe, and returns an empty string.
             */
            nonvirtual String GetCPUModelPrintName () const;

        public:
            /**
             *  Details we track per CPU (socket/chip). There is much more info in /proc/cpuinfo, like
             *  MHz, and cache size, and particular numerical model numbers. Possibly also add 'bogomips'?
             */
            struct CoreDetails {
                /**
                 *  /proc/cpuinfo 'physical id' - use to tell numper of sockets. Each distinct socketID is a different socket
                 */
                unsigned int fSocketID{};

                /**
                 *  /proc/cpuinfo 'model name' field - a semi-standardized representation of what you want to know about a CPU chip
                 */
                String fModelName{};

                /**
                 */
                CoreDetails (unsigned int socketID = {}, const String& modelName = String{});

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual String ToString () const;
            };

        public:
            /**
             *  A computer may have multiple CPUCores, and in principle they can differ.
             *  The number of filled 'cpu sockets' is fCPUs.length ().
             *
             *  \note These are 'logical cores' and may not be physical cores.
             *  \note We have no way to capture physical cores (per socket). Not sure that is helpful/needed.
             */
            Containers::Sequence<CoreDetails> fCores;

        public:
            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         */
        struct Memory {
            /**
             *  Size in bytes
             */
            size_t fPageSize{};

            /**
             *  Size in bytes
             */
            uint64_t fTotalPhysicalRAM{};

            /**
             *  Size in bytes
             */
            uint64_t fTotalVirtualRAM{};

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         */
        struct OperatingSystem {
            /**
             *  e.g. Linux/MacOS/Windows/Unix (on POSIX systems - value of uname)
             */
            String fTokenName;

            /**
             *  e.g. Linux/Ubuntu,RedHat, Windows XP, Windows 2000
             */
            String fShortPrettyName;

            /**
             *  e.g. RedHat 3.5, Ubuntu 11, Windows XP, Windows 8, Windows 8.1, Windows 10
             */
            String fPrettyNameWithMajorVersion;

            /**
             *  Similar level of detail to what is printed by 'winver' application.
             *
             *  e.g. Windows 10 Version 1809 (OS Build 17763.379)
             */
            String fPrettyNameWithVersionDetails;

            /**
             *  e.g. 1.0, 3.5, etc. Note - this refers to the overall os (distribution -
             *  like for ubuntu 11.04, this would be 11.04, not the kernel version)
             */
            String fMajorMinorVersionString;

            /**
             *  http://tools.ietf.org/html/rfc1945#section-10.15
             *  http://tools.ietf.org/html/rfc1945#section-3.7
             *
             *  e.g. MyProduct/1.0, Mozilla/3.5, etc
             */
            String fRFC1945CompatProductTokenWithVersion;

            /**
             *  Number of bits the OS targets. Often a 64-bit OS will support 32-bits, and concievably other
             *  combinations are possible. But this value returns the principle / primary number of bits supported
             *  by the OS (bits of addressing).
             */
            unsigned int fBits{32};

            /**
             *
             *  \note   Common::DefaultNames<> supported
             */
            enum class InstallerTechnology {
                eRPM,
                eMSI,
                eDPKG,

                Stroika_Define_Enum_Bounds (eRPM, eDPKG)
            };

            /**
             *  Some UNIX systems use rpm (redhat, and many others), and others use dpkg (debian based).
             *  Windows uses MSI.
             *  But there are a wide variety of other choices (portage, ports, etc).
             */
            optional<InstallerTechnology> fPreferedInstallerTechnology;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         *  This is very frequently NOT useful, not unique, but frequently desired/used,
         *  so at least you can get to it uniformly, cross-platform.
         */
        struct ComputerNames {
            /**
             *  Returns the best OS dependent guess at a computer name we have.
             *
             *  On windows, this amounts to the NETBIOS name, and on UNIX, this amounts
             *  to the result of 'gethostname' (man 2 hostname).
             */
            String fHostname;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        BootInformation fBootInformation;
        CPU             fCPU;
        Memory          fMemory;

        /**
         *  Info about the actual operating system this software is running on (if possible to tell, it could be well hidden)
         *
         *  Often virtualization (things like compatibility mode in a manifest, or perhaps docker, or WSL) makes the apparent
         *  operating system different than the one you are actually running on).
         * 
         *  \note Actual Operating System is typically what you would report to a user (GUI).
         */
        OperatingSystem fActualOperatingSystem;

        /**
         *  Return info about the apparent operating system this software is running on.
         *
         *  Often virtualization (things like compatibility mode in a manifest, or perhaps docker, or WSL) makes the apparent
         *  operating system different than the one you are actually running on).
         */
        OperatingSystem fApparentOperatingSystem;
        ComputerNames   fComputerNames;

        /**
         */
        SystemConfiguration (const BootInformation& bi, const CPU& ci, const Memory& mi, const OperatingSystem& oi, const ComputerNames& cn);
        SystemConfiguration (const BootInformation& bi, const CPU& ci, const Memory& mi, const OperatingSystem& actualOS,
                             const OperatingSystem& apparentOS, const ComputerNames& cn);

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     * @brief Get the System Configuration object - note not a system global - because the configuration can change while the app is running
     * 
     * @return SystemConfiguration 
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
     *  Return info about the actual operating system this software is running on (if possible to tell, it could be well hidden)
     *
     *  Often virtualization (things like compatability mode in a manifest, or perhaps docker, or WSL) makes the apprarent
     *  operating system different than the one you are actually running on).
     */
    SystemConfiguration::OperatingSystem GetSystemConfiguration_ActualOperatingSystem ();

    /**
     *  Return info about the apparent operating system this software is running on.
     *
     *  Often virtualization (things like compatability mode in a manifest, or perhaps docker, or WSL) makes the apprarent
     *  operating system different than the one you are actually running on).
     */
    SystemConfiguration::OperatingSystem GetSystemConfiguration_ApparentOperatingSystem ();

    /**
     */
    SystemConfiguration::ComputerNames GetSystemConfiguration_ComputerNames ();

    /**
     *  \brief return the number of currently available CPU cores on this (virtual) machine
     * 
     *  This is very roughly GetSystemConfiguration_CPU ().GetNumberOfLogicalCores ()
     *  BUT - this CAN CHANGE over time, and this routine tries to provide a QUICK (cheap so not 100% guaranteed right) answer
     *  but generally will be quicker than GetSystemConfiguration_CPU ().GetNumberOfLogicalCores (), and provides
     *  stronger guaranteeds about accurance than std::thread::hardware_concurrency (https://en.cppreference.com/w/cpp/thread/thread/hardware_concurrency)
     * 
     *  @see GetSystemConfiguration_CPU
     *  @see SystemConfiguration::CPU::GetNumberOfLogicalCores
     */
    unsigned int GetNumberOfLogicalCPUCores (const chrono::duration<double>& allowedStaleness = 1min);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SystemConfiguration.inl"

#endif /*_Stroika_Foundation_Common_SystemConfiguration_h_*/

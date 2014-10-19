/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SystemConfiguration_h_
#define _Stroika_Foundation_Configuration_SystemConfiguration_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
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
                struct CPU {
                    /**
                     *  Number of Physical Cores
                     */
                    unsigned int    fNumberOfPhysicalCores {};

                    /**
                     *  Number of Logical Cores (aka  max concurrent logical thread count)
                     */
                    unsigned int    fNumberOfLogicalCores {};

                    /*
                     *  Not sure we want to support this.
                     *  Number of Sockets
                     *  unsigned int    fNumberOfCPUSockets {};
                     */

                    /*
                     *  @todo Portable?? CPUInfo descripotion? Category
                     */
                };


                /**
                 */
                struct  Memory {
                    size_t      fPageSize {};
                    //?             uint64_t    TotalPages;
                    uint64_t    fTotalPhysicalRAM {};
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


            inline  _DeprecatedFunction_ (String  GetComputerName (), "Instead use GetSystemConfiguration_ComputerNames() - to be removed after v2.0a48")
            {
                return GetSystemConfiguration_ComputerNames ().fHostname;
            }


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

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Platform_h_
#define _Stroika_Foundation_Configuration_Platform_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"

#include    "SystemConfiguration.h"



/**
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Consider using Version class
 *
 *      @todo   Needs review. Not sure about prior art to compare with? Gestalt? Windows VersionInfo?
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            using   Characters::String;

#if 1
            struct  _DeprecatedClass_ (Platform , "DEPRECATED in v2.0a48 - use SystemConfiguration"): SystemConfiguration::OperatingSystem {
                Platform (const OperatingSystem& o)
                    : OperatingSystem (o)
                {
                }
                static  Platform    Get ()
                {
                    return GetSystemConfiguration_OperatingSystem ();
                }
            };
#else
            /**
             */
            struct  Platform {
                /**
                 *  e.g. Linux/MacOS/Windows/Unix (on POSIX systems - value of uname)
                 */
                String  TokenName;

                /**
                 *  e.g. Linux/Ubuntu,RedHat, Windows XP, Windows 2000
                 */
                String  ShortPrettyName;

                /**
                 *  e.g. RedHat 3.5, Ubuntu 11, Windows XP, Windows 8, Windows 8.1
                 */
                String  PrettyNameWithMajorVersion;

                /**
                 *  e.g. 1.0, 3.5, etc
                 */
                String  MajorMinorVersionString;

                /**
                 *  http://tools.ietf.org/html/rfc1945#section-10.15
                 *  http://tools.ietf.org/html/rfc1945#section-3.7
                 *
                 *  e.g. MyProduct/1.0, Mozilla/3.5, etc
                 */
                String  RFC1945CompatProductTokenWithVersion;

                /**
                 *  Read OS parameters to compute the current OS configuraiton.
                 */
                static  Platform    Get ();
            };
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "Platform.inl"

#endif  /*_Stroika_Foundation_Configuration_Platform_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_
#define _Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 * Note - most of the LINUX/POSIX pathnames come from
 *      http://www.pathname.com/fhs/pub/fhs-2.3.html
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;
                using   Characters::SDKString;


                // These GetSpecialDir_XXX routines always return a valid directory (if createIfNotPresent) - and
                // it always ends in a '\\'
                //
                //
                // See also http://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard
                //
                namespace   WellKnownLocations {


                    String GetMyDocuments (bool createIfNotPresent = true);


                    /*
                     * Return directory which contains top-level application data - which should be persistent. Store long-term applicaiton
                     * data (which is not user specific) such as databases etc here.
                     */
                    String GetApplicationData (bool createIfNotPresent = true);


                    /*
                     * This returns the directory where an appliation may write temporary files - files which should not
                     * be preserved across reboots (though the OS may not enforce this). This directory may or may not be current-user-specific.
                     */
                    String      GetTemporary ();
                    SDKString   GetTemporaryT ();


#if         qPlatform_Windows
                    // empty string if doesn't exist
                    String GetWinSxS ();
#endif


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_*/

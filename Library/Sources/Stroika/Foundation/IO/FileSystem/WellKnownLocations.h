/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_
#define _Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Implement caching of computed well known directory locations (done add hoc in some cases)
 *
 *      @todo   Consider the utility of the 'create if not present' flag. Once considered, probably handle more
 *              uniformly.
 *
 *      @todo   Reimplement windows code with SHGetKnownFolderPath
 *              http://msdn.microsoft.com/en-us/library/windows/desktop/bb762188(v=vs.85).aspx
 *              http://msdn.microsoft.com/en-us/library/windows/desktop/dd378457(v=vs.85).aspx
 *
 *              Existing windows code uses deprecated APIs.
 */

namespace Stroika::Foundation::IO::FileSystem {
    using Characters::SDKString;
    using Characters::String;
}

/**
 *  These WellKnownLocations routines always return a valid directory (if createIfNotPresent)
 *  or empty string (if not valid???).
 *
 *  If a directory is returned, it is always terminated with FileSystem::kPathComponentSeperator
 *  so that its unambiguously a directory name.
 *
 *  Most of these names/concepts for pathnames come from:
 *      http://www.pathname.com/fhs/pub/fhs-2.3.html
 *  and/or
 *      http://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard
 */
namespace Stroika::Foundation::IO::FileSystem::WellKnownLocations {

    /**
     */
    String GetMyDocuments (bool createIfNotPresent = true);

    /**
     *  Returns:
     *      GetSpoolDirectory () returns the top level directory where applications store
     *      data which awaits later processing (like a mail spool or queue). This is in some sense
     *      temporary data, but data which should be preserved across boots.
     *
     *      Data stored here is typically deleted once its been processed.
     */
    String GetSpoolDirectory ();

    /**
     *  Returns:
     *      Return directory which contains top-level application data - which should
     *      be persistent. Store long-term applicaiton data (which is not user specific)
     *      such as databases etc here.
     *
     *      POSIX:
     *              /var/opt
     *                  though arguably should be /var/lib - fhs-2.3.html and Filesystem_Hierarchy_Standard
     *                  are pretty unclear - was /var/lib until Stroika v2.0a207
     */
    String GetApplicationData (bool createIfNotPresent = true);

    /**
     *  Returns:
     *      Return runtime variable data - like pid files.
     *
     *      PID files go there. Transient UNIX-domain sockets must place them in this directory.
     *
     *      POSIX:
     *              /var/run
     *      Windows:
     *              Alias for GetTemporary()
     */
    String GetRuntimeVariableData ();

    /**
     *  Returns:
     *      This returns the directory where an appliation may write temporary files -
     *      files which should not be preserved across reboots (though the OS may not enforce this).
     *      This directory may or may not be current-user-specific.
     *
     *  \note   The 'T' variant of the function has fewer dependencies on other Stroika components
     *          than the GetTemporary () function, and that is critical to its use in
     *          the DbgTrace () code when tracing to a temporary file.
     */
    String    GetTemporary ();
    SDKString GetTemporaryT ();

#if qPlatform_Windows
    /**
     *  Returns:
     *      empty string if the directory doesn't exist.
     */
    String GetWinSxS ();
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_IO_FileSystem_WellKnownLocations_h_*/

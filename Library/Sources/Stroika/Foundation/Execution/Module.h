/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Module_h_
#define _Stroika_Foundation_Execution_Module_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

#if qStroika_Foundation_Common_Platform_POSIX
#include <unistd.h>
#endif

#include "Stroika/Foundation/Characters/SDKString.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/LazyInitialized.h"

#if !defined(qHas_pid_t)
#error "qHas_pid_t must  be defined in StroikaConfig.h"
#endif

namespace Stroika::Foundation::Execution {

#if qHas_pid_t
    using pid_t = ::pid_t;
#else
#if qStroika_Foundation_Common_Platform_Windows
    using pid_t = DWORD;
#else
    using pid_t = int;
#endif
#endif

    /**
     *  The directory where the executable that is running this code is located. If this code is compiled into a DLL,
     *  this returns the executable directory for the underlying process/executable (not the DLL/so file).
     */
    filesystem::path GetEXEDir ();

    /**
     *  The path where the executable that is running this code is located. If this code is compiled into a DLL,
     *  this returns the executable for the underlying process/executable (not the DLL/so file).
     */
    filesystem::path GetEXEPath ();

    /**
     *  Return the full path to the given process (throws if not found).
     * 
     *  NYI for WINDOWS.
     */
    filesystem::path GetEXEPath (pid_t processID);

    /**
     *  The set of system locations to look for an executable (note order matters, which is why this is a Sequence)
     */
    extern const LazyInitialized<Containers::Sequence<filesystem::path>> kPath;

#if qStroika_Foundation_Common_Platform_Windows
    /**
     *  The set of system extensions to try for a given file to see if its an executable (note order matters, which is why this is a Sequence)
     * 
     *  https://wiki.tcl-lang.org/page/PATHEXT
     */
    extern const LazyInitialized<Containers::Sequence<filesystem::path>> kPathEXT;
#endif

    /**
     *  \brief convert getenv() to a Mapping of SDKString (in case some issue with charactor set conversion)
     * 
     *  \note LazyInitialized so if not used, nearly zero cost
     */
    extern const LazyInitialized<Containers::Mapping<Characters::SDKString, Characters::SDKString>> kRawEnvironment;

    /**
     *  \brief convert getenv() to a Mapping of Strings for easier access
     * 
     *  \note LazyInitialized so if not used, nearly zero cost
     */
    extern const LazyInitialized<Containers::Mapping<Characters::String, Characters::String>> kEnvironment;

    /**
     *  \brief If fn refers to an executable - return it (using kPATH, and kPathEXT as appropriate)
     * 
     *  If fn is an absolute path, use that (possibly with suffixes appended).
     *  If fn is not absolute, try appending it to each path from kPATH, and redo
     *  same check.
     * 
     *  On Windows, if ext is missing, also check kPathEXT
     * 
     *  If no matches, return nullopt.
     */
    optional<filesystem::path> FindExecutableInPath (const filesystem::path& fn);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Execution_Module_h_*/

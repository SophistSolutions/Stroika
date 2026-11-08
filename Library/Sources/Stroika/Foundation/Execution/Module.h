/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Module_h_
#define _Stroika_Foundation_Execution_Module_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>

#if qStroika_Platform_POSIX
#include <unistd.h>
#endif

#include "Stroika/Foundation/Characters/SDKString.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/LazyInitialized.h"

namespace Stroika::Foundation::Execution {

#if qStroika_Platform_POSIX
    using pid_t = ::pid_t;
#elif qStroika_Platform_Windows
    using pid_t = DWORD;
#else
    using pid_t = int;
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
     *  \brief Is all of s in the main executable's read-only image - so it has application lifetime, and a pointer into it
     *         may be kept, rather than its bytes copied?
     *
     *  That image is where the program's string literals and other constant data are (and Stroika's own, since Stroika is
     *  linked statically). Another module's read-only image does not count: it can be unloaded.
     *
     *  \returns true if so; false if not - s may yet have application lifetime (another module's image, a read-only mapping),
     *           but Stroika cannot tell; nullopt if Stroika cannot tell on this platform (it can on Linux, MacOS and Windows).
     *           An empty s is false.
     *
     *  Cheap: a few comparisons, once the image has been examined (at first use).
     *
     *  \see Design-Overview.md, "Application lifetime"
     */
    optional<bool> IsAddressReadOnlyApplicationLifetime (span<const byte> s);

    /**
     *  \brief For a precondition: may s have application lifetime? - false only where Stroika can tell it does not
     *
     *  IsAddressReadOnlyApplicationLifetime (s), but true where that cannot tell (nullopt), and true for an empty s, which has
     *  nothing to outlive:
     *      \code
     *          Require (Execution::MayHaveApplicationLifetime (as_bytes (span{s})));
     *      \endcode
     *
     *  Not inline, so that a low-level header can declare it, rather than include this one (as Common/Enumeration.inl and
     *  Characters/Format.h do).
     *
     *  \see Design-Overview.md, "Application lifetime"
     */
    bool MayHaveApplicationLifetime (span<const byte> s);

    /**
     *  The set of system locations to look for an executable (note order matters, which is why this is a Sequence)
     */
    extern const LazyInitialized<Containers::Sequence<filesystem::path>> kPath;

#if qStroika_Platform_Windows
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
     * 
     *  \note Captures data at first use, so if environment changes after that, this will not reflect those changes.
     *        If you need to reflect changes, getenv (or similar).
     * 
     *  \note On Windows, the Mapping key compare function is case-insensitive (like the OS). On other platforms, it is case-sensitive (like the OS).
     */
    extern const LazyInitialized<Containers::Mapping<Characters::SDKString, Characters::SDKString>> kRawEnvironment;

    /**
     *  \brief convert getenv() to a Mapping of Strings for easier access
     * 
     *  \note LazyInitialized so if not used, nearly zero cost
     * 
     *  \note Captures data at first use, so if environment changes after that, this will not reflect those changes.
     *        If you need to reflect changes, getenv (or similar).
     * 
     *  \note On Windows, the Mapping key compare function is case-insensitive (like the OS). On other platforms, it is case-sensitive (like the OS).
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
     * 
     *  \par Example Usage
     *      \code
     *          if (optional<filesystem::path> pp = FindExecutableInPath ("bash"sv)) {
     *              fArgs_ += String{*pp};
     *          }
     *          else {
     *              Throw (IO::FileSystem::Exception{make_error_code (errc::no_such_file_or_directory), filesystem::path{"bash"sv}});
     *          }
     *      \endcode
     */
    optional<filesystem::path> FindExecutableInPath (const filesystem::path& fn);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Module.inl"

#endif /*_Stroika_Foundation_Execution_Module_h_*/

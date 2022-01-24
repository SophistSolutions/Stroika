/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileSystem_h_
#define _Stroika_Foundation_IO_FileSystem_FileSystem_h_ 1

#include "../../StroikaPreComp.h"

#include <filesystem>

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Sequence.h"
#include "../../Time/DateTime.h"

#include "../AccessMode.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-685 - 
 *              MAJOR CHANGES required to accomodate std::filesystem 
 *              - maybe losing this alltogether, or at least big changes 
 *              (like using path class instead of String) - force use of ToPath ()
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-612
 *              Probably rename class IO::FileSystem::FileSystem to IO::FileSystem::Ptr (or FileSystemPtr)
 *
 *              OLD TODOS this subsumes:
 *                  >   Terrible name - class FileSystem inside namesapce FileSystem!
 *
 *                  >    Bad names. Unwise to have CLASS (singlton mgr) as same name as namespace. Not sure how to fix however.
 *
 *                  >   Add virtual interface and subdir for each concrete one. Tons more code to be factored in here.
 *                      (leave open but this woudl be a second step - once we have this in 'Ptr' doing rep a natural next step
 *
 *                  >   Great IDEA FROM KDJ. I USED TO support abstractfilesystem in stroika. Maybe in old code base. Used for Win32 FS versus UNIX versus MacOS FS.
 *                      KDJ's point is this idea should be resurected cuz its useful for stuff like TARFILEs and ZIPFILES or ISO files which act like a FS, and can be treated
 *                      that way.
 *
 *      @todo   Consider relationship between windows GetLongPathName() and our CanonicalizePathName...
 *
 */

namespace Stroika::Foundation::IO::FileSystem {

    /**
     */
    enum class RemoveDirectoryPolicy {
        eFailIfNotEmpty,
        eRemoveAnyContainedFiles, // note - this includes the case of included folders which include more files - fully recursive

        eDEFAULT = eFailIfNotEmpty,

        Stroika_Define_Enum_Bounds (eFailIfNotEmpty, eRemoveAnyContainedFiles)
    };

    static constexpr RemoveDirectoryPolicy eFailIfNotEmpty          = RemoveDirectoryPolicy::eFailIfNotEmpty;
    static constexpr RemoveDirectoryPolicy eRemoveAnyContainedFiles = RemoveDirectoryPolicy::eRemoveAnyContainedFiles;

    /**
     *  SUPER ROUGH DRAFT .... Move much code from Directory and FileUtils as methods here. See KDJ comment above. Do other filesystems.
     *  POSIX, WINDOWS, and MacOS, and ZIPFILE, etc...
     *
     *  \note SOON TODO REP class and shared_ptr here- https://stroika.atlassian.net/browse/STK-612 - 
     *
     *  \par Example Usage
     *      \code
     *          if (IO::FileSystem::Default ().Access (kProcUptimeFileName_)) {
     *              // do stuff
     *          }
     *      \endcode
     *
     */
    class Ptr {
    public:
        /**
         *  Return true if the given access is allowed (for the current user id).
         *  This works for a file or directory.
         *
         *  @see CheckAccess to avoid test, and just throw if missing
         *
         *  \par Example Usage
         *      \code
         *          if (IO::FileSystem::Default ().Access (kProcUptimeFileName_)) {
         *              // do stuff
         *          }
         *      \endcode
         *
         *  \note   Similar to IO.File.Exists () in .net https://msdn.microsoft.com/en-us/library/system.io.file.exists(v=vs.110).aspx
         *  \note   Similar to POSIX access () - https://linux.die.net/man/2/access
         */
        nonvirtual bool Access (const filesystem::path& fileFullPath, AccessMode accessMode = AccessMode::eRead) const noexcept;

    public:
        /**
         * Doesn't actually open the file. It's purely advisory. But its helpful to assure
         * a consistent set of error reporting across different styles of opens. Just call this first,
         * and it will throw exceptions if the file doesn't exist, or has access privileges issues.
         *
         *  This works for a file or directory.
         *
         *  @see Access to avoid throw
         *
         *  \par Example Usage
         *      \code
         *          void CopyFile (String srcPath, String desPath)
         *          {
         *              IO::FileSystem::Default ().CheckAccess (srcFile, IO::AccessMode::eRead);
         *              CreateDirectoryForFile (destPath);
         *              .. do actual copy ..
         *          }
         *      \endcode
         */
        nonvirtual void CheckAccess (const filesystem::path& fileFullPath, AccessMode accessMode = AccessMode::eRead);
        nonvirtual void CheckAccess (const filesystem::path& fileFullPath, bool checkCanRead, bool checkCanWrite);

    public:
        /**
         *  Walk the (environment variable) PATH and return the full path to the named executable, if any.
         */
        nonvirtual optional<filesystem::path> FindExecutableInPath (const filesystem::path& filename) const;

    public:
        // @todo see last_write_time
        nonvirtual DateTime GetFileLastModificationDate (const filesystem::path& fileName);

    public:
        // @todo doesn't appear supported by std::filesystem
        nonvirtual DateTime GetFileLastAccessDate (const filesystem::path& fileName);
    };

    /**
     *  \note   Design Note: why method 'Default ()' instead of just sThe, or something like that?
     *          There is one special interesting filesystem, but the intention was to someday allow different filesystems
     *          to be accessed. For example, treating a zipfile as a filesystem.
     */
    Ptr Default ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileSystem.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileSystem_h_*/

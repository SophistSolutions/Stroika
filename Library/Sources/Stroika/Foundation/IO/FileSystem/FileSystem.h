/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileSystem_h_
#define _Stroika_Foundation_IO_FileSystem_FileSystem_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Sequence.h"
#include "../../Time/DateTime.h"

#include "../FileAccessMode.h"
#include "Common.h"
#include "Directory.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
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

namespace Stroika::Foundation {
    namespace IO {
        namespace FileSystem {

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
                nonvirtual bool Access (const String& fileFullPath, FileAccessMode accessMode = FileAccessMode::eRead) const noexcept;

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
                 *              IO::FileSystem::Default ().CheckAccess (srcFile, IO::FileAccessMode::eRead);
                 *              CreateDirectoryForFile (destPath);
                 *              .. do actual copy ..
                 *          }
                 *      \endcode
                 */
                nonvirtual void CheckAccess (const String& fileFullPath, FileAccessMode accessMode = FileAccessMode::eRead);
                nonvirtual void CheckAccess (const String& fileFullPath, bool checkCanRead, bool checkCanWrite);

            public:
                /**
                 *  Walk the (environment variable) PATH and return the full path to the named executable, if any.
                 */
                nonvirtual optional<String> FindExecutableInPath (const String& filename) const;

            public:
                /**
                 *  If the given file is a shortcut (or symbolic link), returns what is pointed to by a shortcut.
                 *  On Windows - this refers to shortcut files, and on
                 *  LINUX this is the same as POSIX readlink.
                 *
                 *  If the target file exists and is not a shortcut, just return that file name.
                 *
                 *  Raise an exception if the given path name does not exist (OK if thing pointed to doesn't exist).
                 *
                 *  @see CanonicalizeName
                 */
                nonvirtual String ResolveShortcut (const String& path2FileOrShortcut);

            public:
                /**
                 *  The CanonicalizeName () function returns a String containing the canonicalized absolute pathname
                 *  corresponding to argument 'path2FileOrShortcut'. In the returned string, symbolic links
                 *  (or windows shortcuts) are resolved, as are . and .. pathname components. Consecutive slash (/)
                 *  characters are replaced by a single slash.
                 *
                 *  Note - if argument path is a relative path, it is relative to the process current-working-directory
                 *  or the argument relativeToDirectory
                 *
                 *  @todo CONSIDER
                 *      Raise an exception if the given path name does not exist (OK if thing pointed to doesn't exist).
                 *
                 *      a pathname component is unreadable or does not exist
                 *
                 *  @todo consider if this should assureDirectoryEndsInSlash - tricky cuz noit sre ure we know if its a dir
                 */
                nonvirtual String CanonicalizeName (const String& path2FileOrShortcut, bool throwIfComponentsNotFound = true);
                nonvirtual String CanonicalizeName (const String& path2FileOrShortcut, const String& relativeToDirectory, bool throwIfComponentsNotFound = true); //nyi

            public:
                /**
                 *  Take the given filename, and if it is a full path name, return it, and if not, combine the name with the
                 *  current working directory, to make a full path name.
                 *
                 *  This is NOT the same as @see CanonicalizeName()
                 *
                 *  \note   On Windows, this uses the Windows SDK GetFullPathName, but thats not available on all platforms, where it is
                 *          manually simulated.
                 */
                nonvirtual String GetFullPathName (const String& pathname);

            public:
                /**
                 *  This breaks a string at 'path-separator' boundaries, and returns each component.
                 *
                 *  This works with DOS filenames, as well as UNC filenames (and UNCW file names)
                 */
                struct Components {
                    enum AbsolueteOrRelative { eAbsolutePath,
                                               eRelativePath };
                    AbsolueteOrRelative fAbsolutePath;
#if qPlatform_Windows
                    struct ServerAndShare {
                        String fServer;
                        String fShare;
                    };
                    // REDO AS UNION BUT MUST BE CAREFUL OF DESTRUCTION/CONSTRUCTION
                    // can be C:, or \\SMB-DRIVE
                    //struct {
                    optional<String>         fDriveLetter; // this string incldues the colon, so example "C:"
                    optional<ServerAndShare> fServerAndShare;
#endif
                    Containers::Sequence<String> fPath;
                };

            public:
                /**
                 *  This breaks a string at 'path-separator' boundaries, and returns each component.
                 *
                 *  This works with DOS filenames, as well as UNC filenames (and UNCW file names)
                 */
                nonvirtual Components GetPathComponents (const String& fileName);

            public:
                nonvirtual FileOffset_t GetFileSize (const String& fileName);

            public:
                nonvirtual DateTime GetFileLastModificationDate (const String& fileName);

            public:
                nonvirtual DateTime GetFileLastAccessDate (const String& fileName);

            public:
                /**
                 *  Remove the given file. The filename can be a relative pathname, or absolute.
                 *  This throws on failure (e.g. because the file is not found).
                 *
                 *  @see RemoveFileIf
                 */
                nonvirtual void RemoveFile (const String& fileName);

            public:
                /**
                 *  Remove the given file if it exists. The filename can be a relative pathname, or absolute.
                 *  This throws if the remove fails, but has no effect (no throw) if the file doesn't exist.
                 *
                 *  return true iff it existed, and this succcessfully deleted it.
                 *
                 *  @see RemoveFile
                 */
                nonvirtual bool RemoveFileIf (const String& fileName);

            public:
                /**
                 *  @see RemoveDirectoryIf
                 *
                 *  Remove the given directory. The directory name can be a relative pathname, or absolute.
                 *  This throws on failure (e.g. because the directory is not found).
                 *
                 *  In any case, this will fail if the directory is not removed.
                 */
                nonvirtual void RemoveDirectory (const String& directory, RemoveDirectoryPolicy policy = RemoveDirectoryPolicy::eDEFAULT);

            public:
                /**
                 *  @see RemoveDirectory
                 *
                 *  Remove the given directory. The directory name can be a relative pathname, or absolute.
                 *  This throws on failure upon fail to delete (depending on the argument 'policy') - but will not fail,
                 *  and will silently ignore, if the argument directory name is missing.
                 *
                 *  In any case, except that the directory was not there to begin with, this will fail if the directory is not removed.
                 *
                 *  return true iff it existed, and this succcessfully deleted it.
                 */
                nonvirtual bool RemoveDirectoryIf (const String& directory, RemoveDirectoryPolicy policy = RemoveDirectoryPolicy::eDEFAULT);

            public:
                /**
                 *  o   http://pubs.opengroup.org/onlinepubs/009695399/functions/symlink.html
                 for now only works on unix
                 ln [OPTION]... [-T] TARGET LINK_NAME
                 create a link to TARGET with the name LINK_NAME

                 'linkName' is the full path to the symbolic link file to be created
                 'target' is the file POINTED TO by the symbolic link
                */
                nonvirtual void CreateSymbolicLink (const String& linkName, const String& target);

            public:
                /**
                 *  Wrapper on platform GetCurrentDirectory () or getcwd()
                 *
                 *  \ensure returns a 'slash terminated' pathname
                 */
                nonvirtual String GetCurrentDirectory () const;

            public:
                /**
                 *  Wrapper on platform SetCurrentDirectory () or chdir ()
                 */
                nonvirtual void SetCurrentDirectory (const String& newDir);
            };

            /**
             *  \note   Design Note: why method 'Default ()' instead of just sThe, or something like that?
             *          There is one special interesting filesystem, but the intention was to someday allow differnt filesystems
             *          to be accessed. For example, treating a zipfile as a filesystem.
             */
            Ptr Default ();
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileSystem.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_FileSystem_h_*/

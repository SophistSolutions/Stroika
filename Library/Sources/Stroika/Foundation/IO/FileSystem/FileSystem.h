/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileSystem_h_
#define _Stroika_Foundation_IO_FileSystem_FileSystem_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Containers/Sequence.h"
#include    "../../Time/DateTime.h"

#include    "../FileAccessMode.h"
#include    "Common.h"
#include    "Directory.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo   Terrible name - class FileSystem inside namesapce FileSystem!
 *
 *      @todo   Great IDEA FROM KDJ. I USED TO support abstractfilesystem in stroika. Maybe in old code base. Used for Win32 FS versus UNIX versus MacOS FS.
 *              KDJ's point is this idea should be resurected cuz its useful for stuff like TARFILEs and ZIPFILES or ISO files which act like a FS, and can be treated
 *              that way.
 *
 *      @todo   Bad names. Unwise to have CLASS (singlton mgr) as same name as namespace. Not sure how to fix however.
 *
 *      @todo   Add virtual interface and subdir for each concrete one. Tons more code to be factored in here.
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                /**
                 *  SUPER ROUGH DRAFT .... Move much code from Directory and FileUtils as methods here. See KDJ comment above. Do other filesystems.
                 *  POSIX, WINDOWS, and MacOS, and ZIPFILE, etc...
                 */
                class   FileSystem {
                public:
                    static  FileSystem  Default ();

                public:
                    /**
                     *  Return true if the given access is allowed (for the current user id).
                     *  This works for a file or directory.
                     */
                    nonvirtual  bool    Access (const String& fileFullPath, FileAccessMode accessMode = FileAccessMode::eRead) const;

                public:
                    /**
                     * doesn't actually open the file. It's purely advisory. But its helpful to assure
                     * a consistent set of error reporting across different styles of opens. Just call this first,
                     * and it will throw exceptions if the file doesn't exist, or has access privileges issues.
                     *
                     *  This works for a file or directory.
                     */
                    nonvirtual  void    CheckAccess (const String& fileFullPath, FileAccessMode accessMode = FileAccessMode::eRead);
                    nonvirtual  void    CheckFileAccess (const String& fileFullPath, bool checkCanRead, bool checkCanWrite);

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
                    nonvirtual  String  ResolveShortcut (const String& path2FileOrShortcut);

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
                    nonvirtual  String  CanonicalizeName (const String& path2FileOrShortcut, bool throwIfComponentsNotFound = true);
                    nonvirtual  String  CanonicalizeName (const String& path2FileOrShortcut, const String& relativeToDirectory, bool throwIfComponentsNotFound = true);    //nyi

                public:
                    /**
                     *  This breaks a string at 'path-separator' boundaries, and returns each component.
                     *
                     *  This works with DOS filenames, as well as UNC filenames (and UNCW file names)
                     */
                    struct  Components {
                        enum    AbsolueteOrRelative { eAbsolutePath, eRelativePath };
                        AbsolueteOrRelative  fAbsolutePath;
#if     qPlatform_Windows
                        struct ServerAndShare {
                            String        fServer;
                            String        fShare;
                        };
                        // REDO AS UNION BUT MUST BE CAREFUL OF DESTRUCTION/CONSTRUCTION
                        // can be C:, or \\SMB-DRIVE
                        //struct {
                        Memory::Optional<String>            fDriveLetter;   // this string incldues the colon, so example "C:"
                        Memory::Optional<ServerAndShare>    fServerAndShare;
                        //};
#endif
                        Containers::Sequence<String>    fPath;
                    };
                public:
                    /**
                     *  This breaks a string at 'path-separator' boundaries, and returns each component.
                     *
                     *  This works with DOS filenames, as well as UNC filenames (and UNCW file names)
                     */
                    nonvirtual  Components    GetPathComponents (const String& fileName);

                public:
                    nonvirtual  FileOffset_t    GetFileSize (const String& fileName);

                public:
                    nonvirtual  DateTime        GetFileLastModificationDate (const String& fileName);

                public:
                    nonvirtual  DateTime        GetFileLastAccessDate (const String& fileName);

                public:
                    /**
                     *  Remove the given file. The filename can be a relative pathname, or absolute.
                     *  This throws on failure (e.g. because the file is not found).
                     *
                     *  See Also:
                     *      @see RemoveFileIf
                     */
                    nonvirtual  void        RemoveFile (const String& fileName);

                public:
                    /**
                     *  Remove the given file if it exists. The filename can be a relative pathname, or absolute.
                     *  This throws if the remove fails, but has no effect (no throw) if the file doesn't exist.
                     *
                     *  See Also:
                     *      @see RemoveFile
                     */
                    nonvirtual  void        RemoveFileIf (const String& fileName);
                };


            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "FileSystem.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_FileSystem_h_*/

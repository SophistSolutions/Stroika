/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileSystem_h_
#define _Stroika_Foundation_IO_FileSystem_FileSystem_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Time/DateTime.h"

#include    "../FileAccessMode.h"
#include    "Common.h"
#include    "Directory.h"



/**
 * TODO:
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
                class FileSystem {
                public:
                    static  FileSystem  Default ();

                public:
                    // doesn't actually open the file. It's purely advisory. But its helpful to assure
                    // a consistent set of error reporting across different styles of opens. Just call this first,
                    // and it will throw exceptions if the file doesn't exist, or has access privileges issues.
                    nonvirtual  void    CheckFileAccess (const String& fileFullPath, bool checkCanRead = true, bool checkCanWrite = false);

                public:
                    nonvirtual  String ResolveShortcut (const String& path2FileOrShortcut);

                public:
                    nonvirtual  FileOffset_t    GetFileSize (const String& fileName);
                public:
                    nonvirtual  DateTime        GetFileLastModificationDate (const String& fileName);
                public:
                    nonvirtual  DateTime        GetFileLastAccessDate (const String& fileName);

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

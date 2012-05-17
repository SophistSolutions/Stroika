/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_PathName_h_
#define _Stroika_Foundation_IO_FileSystem_PathName_h_   1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/TChar.h"
#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"



/**
 * TODO:
 *
 *      o   See Stroika 1.0. It has some stuff about breaking paths into components. I'm not sure I like that design -but its woth looking
 *          at more closely. We need something like that here.
 *              -- LGP 2011-09-29
 *
 *
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {

                using   Characters::TChar;
                using   Characters::TString;

#if     qCompilerAndStdLib_Supports_constexpr
#if     qPlatform_Windows
                constexpr   TChar   kPathComponentSeperator =   '\\';
#elif   qPlatform_POSIX
                constexpr   TChar   kPathComponentSeperator =   '/';
#endif
#else
                extern  const   TChar   kPathComponentSeperator;
#endif

                /*
                 * This funciton presumes its argument is a directory, and makes sure it has a kPathComponentSeperator character
                 * at the end. Use this when given a directory from some source that isn't so careful, so code can generally
                 * operate with the assumption that directories have that trailing slash, so its easier to compose
                 * pathanmes.
                 */
                TString AssureDirectoryPathSlashTerminated (const TString& dirPath);

                // map ALL characters in the string to something safe to use for a filename (that is - get rid of slashes etc - if present)
                TString SafeFilenameChars (const TString& s);


                /*
                 * if Win32 'short-file-name' - 8.3 - extend and return associated longfilename
                 */
                TString AssureLongFileName (const TString& fileName);


                /*
                 * This returns the file suffix, including the leading '.'. So for "foo.txt" it returns ".txt".
                 * If the fail has no trailing suffix, it returns the empty string. Also - on Windows, if the source file is a 8.3 encoded name, the
                 * function automatically maps to a fullname to retrieve the full prefix.
                 *
                 *      (( The windows behaviro is that the code did - I'm not sure its wise, or even needed anymore?))
                 *      (( -- LGP 2011-09-29)
                 */
                TString GetFileSuffix (const TString& fileName);

                /*
                 *  get the base name (strippping path and suffix)
                 */
                TString GetFileBaseName (const TString& pathName);

                /*
                 *  // get the full path WITHOUT the file suffix at the end
                 */
                TString StripFileSuffix (const TString& pathName);

                /*
                 *      // get the directory part of the given pathname (if the path refers to a directory - ends in / - then return THAT name)
                 */
                TString GetFileDirectory (const TString& pathName);

            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_FileSystem_PathName_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "PathName.inl"

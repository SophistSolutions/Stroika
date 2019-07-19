/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_PathName_h_
#define _Stroika_Foundation_IO_FileSystem_PathName_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo   See Stroika 1.0. It has some stuff about breaking paths into components. I'm not sure I like that design -but its woth looking
 *              at more closely. We need something like that here.
 *              -- LGP 2011-09-29
 *
 *
 */

namespace Stroika::Foundation::IO::FileSystem {

    using Characters::String;

#if qPlatform_Windows
    constexpr wchar_t kPathComponentSeperator = '\\';
#elif qPlatform_POSIX
    constexpr wchar_t kPathComponentSeperator = '/';
#endif

    /**
     * This funciton presumes its argument is a directory, and makes sure it has a kPathComponentSeperator character
     * at the end. Use this when given a directory from some source that isn't so careful, so code can generally
     * operate with the assumption that directories have that trailing slash, so its easier to compose
     * pathanmes.
     */
    String AssureDirectoryPathSlashTerminated (const String& dirPath);

    /** 
     *  Map ALL characters in the string to something safe to use for a filename. This 
     *  gets rid of { ':', '/', '\', and '.' }
     *
     *      @todo - consider having optional parameter to specify which characters get mapped, and how.
     *              maybe static const Mapping<Character,String> kDefaultSafeFilenmaeCharsMapping = {{':', L" ", etc...}). 
     */
    String SafeFilenameChars (const String& s);

    /**
     * if Win32 'short-file-name' - 8.3 - extend and return associated longfilename
     */
    String AssureLongFileName (const String& fileName);

    /**
     * This returns the file suffix, including the leading '.'. So for "foo.txt" it returns ".txt".
     * If the fail has no trailing suffix, it returns the empty string. Also - on Windows, if the source file is a 8.3 encoded name, the
     * function automatically maps to a fullname to retrieve the full prefix.
     *
     *      (( The windows behavior is that the code did - I'm not sure its wise, or even needed anymore?))
     *      (( -- LGP 2011-09-29)
     */
    String GetFileSuffix (const String& fileName);

    /**
     *  get the base name (strippping path and suffix).
     *
     *  \note   this differs from the UNIX 'basename' - which does not strip file suffixes.
     *  \note   if the base filename consists of one '.' at the start, and no other ., then this
     *          ignores the file suffix.
     *
     *  \note   Before Stroika v2.0a126, this internally called GetLongPathName() - to get that result in the current
     *          version, use GetFileBaseName (AssureLongFileName (n))
     *  EXAMPLES:
     *      GetFileBaseName (L"foo") ==> L"foo";
     *      GetFileBaseName (L"foo.cpp") ==> L"foo";
     *      GetFileBaseName (L"foo.exe") ==> L"foo";
     *      GetFileBaseName (L".exe") ==> L".exe";
     *  POSIX:
     *      GetFileBaseName (L"/tmp/.CPUBurner") ==> L".CPUBurner";
     *  Windows:
     *      GetFileBaseName (L"c:\\tmp\\.CPUBurner") ==> L".CPUBurner";
     *
     *  @see ExtractDirAndBaseName
     */
    String GetFileBaseName (const String& pathName);

    /**
     *  Portable version of http://linux.die.net/man/3/basename  http://linux.die.net/man/3/dirname
     *
     *  ExtractDirAndBaseName () break a pathname string into directory and filename components.
     *  In the usual case, dirname() returns the string up to, but not including, the final '/',
     *  and basename() returns the component following the final '/'.
     *  Trailing '/' characters are not counted as part of the pathname.
     *
     *  Where this differs from dirname/basenmae is that the dirname we return always is a legal dirname
     *  ending with 'dir separator').
     *
     *  Examples based on examples (from http://linux.die.net/man/3/dirname):
     *      path         dirname    basename
     *
     *  POSIX:
     *      "/usr/lib"    "/usr/"    "lib"
     *      "/usr/"       "/"       "usr/"
     *      "usr"         "./"       "usr"
     *      "/"           "/"       ""
     *      "."           "./"       "."
     *      ".."          "./"       ".."
     *
     *  Windows:
     *      "\usr\lib"    "\usr\"    "lib"
     *      "\usr\"       "\"       "usr\"
     *      "usr"         ".\"       "usr"
     *      "\"           "\"       ""
     *      "."           ".\"       "."
     *      ".."          ".\"       ".."
     *      c:\h\m.t    "c:\h\"     "m.t"
     *
     *  \note   This function PRESERVES the trailing 'directory separator' if present, and this
     *          is another difference with 'basename' and 'dirname'
     *
     *  \req not pathName.empty ()
     */
    pair<String, String> ExtractDirAndBaseName (const String& pathName);

    /**
     *  // get the full path WITHOUT the file suffix at the end
     */
    String StripFileSuffix (const String& pathName);

    /**
     *      // get the directory part of the given pathname (if the path refers to a directory - ends in / - then return THAT name)
     */
    String GetFileDirectory (const String& pathName);

    /**
     *  Return true if the argument pathName is a directory name.
     *
     *  Basically - this just tests if the path name ends in a kPathComponentSeperator.
     *
     *  \req argument not empty string
     */
    bool IsDirectoryName (const String& pathName);

    /**
     *  Convert Stroika String to std::filesystem::path
     */
    path ToPath (const String& p);

    /**
     *  Convert  std::filesystem::path to String Stroika String
     *  \note like Characters::ToString () - but is for formatting display purposes, so may
     *        surround the name with quotes - this does a straight conversion.
     */
    String FromPath (const path& p);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PathName.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_PathName_h_*/

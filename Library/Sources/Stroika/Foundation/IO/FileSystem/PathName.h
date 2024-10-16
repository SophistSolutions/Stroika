/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_PathName_h_
#define _Stroika_Foundation_IO_FileSystem_PathName_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/IO/FileSystem/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
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

    /*
    * On windows, its helpful when mapping String to std::filesystem::pathname to map certain common name prefixes to things that will be found
    * on Windows.
    * 
    * MSYS creates paths like /c/folder for c:/folder
    * CYGWIN creates paths like /cygdrive/c/folder for c:/folder
    * 
    * Automatically map these (since Stroika v3.0d6) in ToPath
    * 
    *   \see https://www.msys2.org/docs/filesystem-paths/
     */
#ifndef qStroika_Foundation_IO_FileSystem_PathName_AutoMapMSYSAndCygwin
#define qStroika_Foundation_IO_FileSystem_PathName_AutoMapMSYSAndCygwin qPlatform_Windows
#endif

#if qPlatform_Windows
    constexpr wchar_t kPathComponentSeperator = '\\';
#elif qPlatform_POSIX
    constexpr wchar_t kPathComponentSeperator = '/';
#endif

    /**
     * This function presumes its argument is a directory, and makes sure it has a kPathComponentSeperator character
     * at the end. Use this when given a directory from some source that isn't so careful, so code can generally
     * operate with the assumption that directories have that trailing slash, so its easier to compose
     * pathnames.
     */
    String AssureDirectoryPathSlashTerminated (const String& dirPath);

    /** 
     *  Map ALL characters in the string to something safe to use for a filename. This 
     *  gets rid of { ':', '/', '\', and '.' }
     *
     *      @todo - consider having optional parameter to specify which characters get mapped, and how.
     *              maybe static const Mapping<Character,String> kDefaultSafeFilenmaeCharsMapping = {{':', " ", etc...}). 
     */
    String SafeFilenameChars (const String& s);

    /**
     * if Win32 'short-file-name' - 8.3 - extend and return associated longfilename
     */
    String AssureLongFileName (const String& fileName);

    /**
     *  Convert Stroika String to std::filesystem::path
     * 
     *  \note see qStroika_Foundation_IO_FileSystem_PathName_AutoMapMSYSAndCygwin
     *        this API is for getting strings from the commandline, or user input, or configuration files etc, where cygwin
     *        or msys style paths maybe present. APIs that talk directly to the OS are more likely to more directly produce
     *        filesystem::path than String. Anyhow - because of this, on windows, its probably more helpful than not to map
     *        the MSYS/cygdrive crap to a path more likely to actually work right. --LGP 2024-03-06
     */
    filesystem::path           ToPath (const String& p);
    optional<filesystem::path> ToPath (const optional<String>& p);

    /**
     *  Convert  std::filesystem::path to String Stroika String
     *  \note UNLIKE Characters::ToString () - so will NOT surround the name with quotes - this does a straight conversion.
     */
    String           FromPath (const filesystem::path& p);
    optional<String> FromPath (const optional<filesystem::path>& p);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PathName.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_PathName_h_*/

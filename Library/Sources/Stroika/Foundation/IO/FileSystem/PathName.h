/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_PathName_h_
#define _Stroika_Foundation_IO_FileSystem_PathName_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
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

#if qStroika_Foundation_Common_Platform_Windows
    constexpr wchar_t kPathComponentSeperator = '\\';
#elif qStroika_Foundation_Common_Platform_POSIX
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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PathName.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_PathName_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Memory/SmallStackBuffer.h"

#include "PathName.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;

/*
 ********************************************************************************
 ************** FileSystem::AssureDirectoryPathSlashTerminated ******************
 ********************************************************************************
 */
String FileSystem::AssureDirectoryPathSlashTerminated (const String& dirPath)
{
    if (dirPath.empty ()) {
        AssertNotReached (); // not sure if this is an error or not. Not sure how code used.
        // put assert in there to find out... Probably should THROW!
        //      -- LGP 2009-05-12
        return String (&kPathComponentSeperator, &kPathComponentSeperator + 1);
    }
    else {
        Character lastChar = dirPath[dirPath.size () - 1];
        if (lastChar == kPathComponentSeperator) {
            return dirPath;
        }
        String result = dirPath;
        result += kPathComponentSeperator;
        return result;
    }
}

/*
 ********************************************************************************
 *********************** FileSystem::SafeFilenameChars **************************
 ********************************************************************************
 */
String FileSystem::SafeFilenameChars (const String& s)
{
    // analyze as wide-char string so we don't mis-identify characters (by looking at lead bytes etc)
    wstring tmp = s.As<wstring> ();
Again:
    for (auto i = tmp.begin (); i != tmp.end (); ++i) {
        switch (*i) {
            case '\\':
                *i = '_';
                break;
            case '/':
                *i = '_';
                break;
            case ':':
                *i = ' ';
                break;
            case '.':
                tmp.erase (i);
                goto Again;
        }
    }
    return tmp;
}

/*
 ********************************************************************************
 ********************* FileSystem::AssureLongFileName ***************************
 ********************************************************************************
 */
String FileSystem::AssureLongFileName (const String& fileName)
{
#if qPlatform_Windows
    DWORD r = ::GetLongPathNameW (fileName.c_str (), nullptr, 0);
    if (r != 0) {
        SmallStackBuffer<wchar_t> buf (SmallStackBufferCommon::eUninitialized, r);
        r = ::GetLongPathNameW (fileName.c_str (), buf, r);
        if (r != 0) {
            return static_cast<const wchar_t*> (buf);
        }
    }
#endif
    return fileName;
}

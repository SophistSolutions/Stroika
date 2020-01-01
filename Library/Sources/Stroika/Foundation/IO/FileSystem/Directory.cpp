/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <limits>
#include <sys/stat.h>

#if qPlatform_Windows
#include <aclapi.h>
#include <io.h>
#include <shlobj.h>
#include <windows.h>
#endif
#if qPlatform_POSIX
#include <unistd.h>
#endif

#include "../../Characters/Format.h"
#include "../../Execution/Exceptions.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif
#include "../../Containers/Common.h"
#include "../../Debug/Trace.h"
#include "../../Memory/SmallStackBuffer.h"
#include "FileSystem.h"
#include "FileUtils.h"
#include "WellKnownLocations.h"

#include "Directory.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************************** IO::FileSystem::Directory ***************************
 ********************************************************************************
 */
Directory::Directory (const String& fileFullPath)
    : fFileFullPath_ (fileFullPath)
{
}

void Directory::AssureExists (bool createParentComponentsIfNeeded) const
{
    IO::FileSystem::CreateDirectory (fFileFullPath_, createParentComponentsIfNeeded);
}

void Directory::AssureDeleted (bool autoDeleteContentsAsNeeded) const
{
    IO::FileSystem::Default ().RemoveDirectoryIf (fFileFullPath_, autoDeleteContentsAsNeeded ? IO::FileSystem::RemoveDirectoryPolicy::eRemoveAnyContainedFiles : IO::FileSystem::RemoveDirectoryPolicy::eFailIfNotEmpty);
}

bool Directory::Exists () const
{
#if qPlatform_Windows
    DWORD attribs = ::GetFileAttributesW (fFileFullPath_.c_str ());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return !!(attribs & FILE_ATTRIBUTE_DIRECTORY);
#elif qPlatform_POSIX
    struct stat s;
    if (::stat (fFileFullPath_.AsSDKString ().c_str (), &s) < 0) {
        // If file doesn't exist - or other error reading, just say not exist
        return false;
    }
    return S_ISDIR (s.st_mode);
#else
    AssertNotImplemented ();
    return false;
#endif
}

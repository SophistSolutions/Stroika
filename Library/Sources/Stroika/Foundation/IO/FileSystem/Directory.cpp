/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <sys/stat.h>
#include    <ctime>
#include    <limits>
#include    <fstream>
#include    <fcntl.h>
#include    <cstdio>

#if     qPlatform_Windows
#include    <aclapi.h>
#include    <io.h>
#include    <shlobj.h>
#include    <windows.h>
#endif
#if     qPlatform_POSIX
#include    <unistd.h>
#endif

#include    "../../Characters/Format.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Execution/Exceptions.h"
#if     qPlatform_Windows
#include    "../../Execution/Platform/Windows/Exception.h"
#endif
#include    "../../Containers/Common.h"
#include    "../../Debug/Trace.h"
#include    "../../IO/FileAccessException.h"
#include    "../../Memory/SmallStackBuffer.h"
#include    "FileUtils.h"
#include    "WellKnownLocations.h"

#include    "Directory.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::FileSystem;

#if     qPlatform_Windows
using   Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif



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

void    Directory::AssureExists (bool createParentComponentsIfNeeded) const
{
    FileSystem::CreateDirectory (fFileFullPath_, createParentComponentsIfNeeded);
}

void    Directory::AssureDeleted (bool autoDeleteContentsAsNeeded) const
{
    if (autoDeleteContentsAsNeeded) {
        FileSystem::DeleteAllFilesInDirectory (fFileFullPath_);
    }
#if     qPlatform_Windows
    ThrowIfFalseGetLastError (::RemoveDirectoryW (fFileFullPath_.c_str ()));
#elif   qPlatform_POSIX
    Execution::ThrowErrNoIfNegative (::rmdir (fFileFullPath_.AsSDKString ().c_str ()));
#endif
}

bool    Directory::Exists () const
{
#if     qPlatform_Windows
    DWORD attribs = ::GetFileAttributesW (fFileFullPath_.c_str ());
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return !! (attribs & FILE_ATTRIBUTE_DIRECTORY);
#elif   qPlatform_POSIX
    struct  stat    s;
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


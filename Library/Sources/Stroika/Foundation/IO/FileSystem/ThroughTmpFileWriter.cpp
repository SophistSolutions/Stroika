/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <cstdio>

#if qPlatform_Windows
#include <windows.h>
#elif qPlatform_POSIX
#include <unistd.h>
#endif

#include "../../Characters/Format.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/Throw.h"
#if qPlatform_Windows
#include "../../Execution/Platform/Windows/Exception.h"
#endif
#include "../../Containers/Common.h"
#include "../../Debug/Trace.h"

#include "../FileAccessException.h"

#include "ThroughTmpFileWriter.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Memory;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfFalseGetLastError;
#endif

/*
 ********************************************************************************
 ************************ FileSystem::ThroughTmpFileWriter **********************
 ********************************************************************************
 */
ThroughTmpFileWriter::ThroughTmpFileWriter (const String& realFileName, const String& tmpSuffix)
    : fRealFilePath_ (realFileName)
    , fTmpFilePath_ (realFileName + tmpSuffix)
{
    Require (not realFileName.empty ());
    Require (not tmpSuffix.empty ());
}

ThroughTmpFileWriter::~ThroughTmpFileWriter ()
{
    if (not fTmpFilePath_.empty ()) {
        DbgTrace (L"ThroughTmpFileWriter::DTOR - tmpfile not successfully commited to '%s'", fRealFilePath_.c_str ());
// ignore errors on unlike, cuz nothing to be done in DTOR anyhow...
#if qPlatform_Windows
        (void)::DeleteFileW (fTmpFilePath_.c_str ());
#elif qPlatform_POSIX
        (void)::unlink (fTmpFilePath_.AsSDKString ().c_str ());
#else
        AssertNotImplemented ();
#endif
    }
}

void ThroughTmpFileWriter::Commit ()
{
    Require (not fTmpFilePath_.empty ()); // cannot Commit more than once
    // Also - NOTE - you MUST close fTmpFilePath (any file descriptors that have opened it) BEFORE the Commit!

    try {
#if qPlatform_Windows
        try {
            ThrowIfFalseGetLastError (::MoveFileExW (fTmpFilePath_.c_str (), fRealFilePath_.c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH));
        }
        catch (const Execution::Platform::Windows::Exception& we) {
            // On Win9x - this fails cuz OS not impl...
            if (static_cast<DWORD> (we) == ERROR_CALL_NOT_IMPLEMENTED) {
                ::DeleteFileW (fRealFilePath_.c_str ());
                ThrowIfFalseGetLastError (::MoveFileW (fTmpFilePath_.c_str (), fRealFilePath_.c_str ()));
            }
            else {
                Execution::ReThrow ();
            }
        }
#elif qPlatform_POSIX
        Execution::ThrowPOSIXErrNoIfNegative (::rename (fTmpFilePath_.AsSDKString ().c_str (), fRealFilePath_.AsSDKString ().c_str ()));
#else
        AssertNotImplemented ();
#endif
        fTmpFilePath_.clear ();
    }
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (fRealFilePath_, FileAccessMode::eWrite);
}

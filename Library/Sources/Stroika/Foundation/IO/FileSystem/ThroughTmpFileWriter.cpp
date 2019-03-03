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
#include "../../Execution/Activity.h"
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
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;
using namespace Stroika::Foundation::Memory;

#if qPlatform_Windows
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
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

    auto            ativity = LazyEvalActivity ([&]() -> String { return Characters::Format (L"committing temporary file %s to %s", Characters::ToString (fTmpFilePath_).c_str (), Characters::ToString (fRealFilePath_).c_str ()); });
    DeclareActivity currentActivity{&ativity};
#if qPlatform_POSIX
    ThrowPOSIXErrNoIfNegative (::rename (fTmpFilePath_.AsSDKString ().c_str (), fRealFilePath_.AsSDKString ().c_str ()));
#elif qPlatform_Windows
    try {
        ThrowIfZeroGetLastError (::MoveFileExW (fTmpFilePath_.c_str (), fRealFilePath_.c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH));
    }
    catch (const system_error& we) {
        // On Win9x - this fails cuz OS not impl...
        if (we.code () == error_code{ERROR_CALL_NOT_IMPLEMENTED, system_category ()}) {
            ::DeleteFileW (fRealFilePath_.c_str ());
            ThrowIfZeroGetLastError (::MoveFileW (fTmpFilePath_.c_str (), fRealFilePath_.c_str ()));
        }
        else {
            ReThrow ();
        }
    }
#else
    AssertNotImplemented ();
#endif
    fTmpFilePath_.clear ();
}

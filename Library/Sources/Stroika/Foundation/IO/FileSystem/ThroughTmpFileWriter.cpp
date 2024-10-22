/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#if qStroika_Foundation_Common_Platform_Windows
#include <windows.h>
#elif qStroika_Foundation_Common_Platform_POSIX
#include <unistd.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#if qStroika_Foundation_Common_Platform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"

#include "Exception.h"

#include "ThroughTmpFileWriter.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

#if qStroika_Foundation_Common_Platform_Windows
using Execution::Platform::Windows::ThrowIfZeroGetLastError;
#endif

/*
 ********************************************************************************
 ************************ FileSystem::ThroughTmpFileWriter **********************
 ********************************************************************************
 */
ThroughTmpFileWriter::ThroughTmpFileWriter (const filesystem::path& realFileName, const String& tmpSuffix)
    : fRealFilePath_{realFileName}
    , fTmpFilePath_{filesystem::path{realFileName}.operator+= (tmpSuffix.As<wstring> ())}
{
    Require (not realFileName.empty ());
    Require (not tmpSuffix.empty ());
}

ThroughTmpFileWriter::~ThroughTmpFileWriter ()
{
    if (not fTmpFilePath_.empty ()) {
        DbgTrace ("ThroughTmpFileWriter::DTOR - tmpfile not successfully commited to {}"_f, fRealFilePath_);
        // ignore errors on unlink, cuz nothing to be done in DTOR anyhow...(@todo perhaps should at least tracelog)
#if qStroika_Foundation_Common_Platform_POSIX
        (void)::unlink (fTmpFilePath_.c_str ());
#elif qStroika_Foundation_Common_Platform_Windows
        (void)::DeleteFileW (fTmpFilePath_.c_str ());
#else
        AssertNotImplemented ();
#endif
    }
}

void ThroughTmpFileWriter::Commit ()
{
    Require (not fTmpFilePath_.empty ()); // cannot Commit more than once
    // Also - NOTE - you MUST close fTmpFilePath (any file descriptors that have opened it) BEFORE the Commit!

    auto activity = LazyEvalActivity (
        [&] () -> String { return Characters::Format ("committing temporary file {} to {}"_f, fTmpFilePath_, fRealFilePath_); });
    DeclareActivity currentActivity{&activity};
#if qStroika_Foundation_Common_Platform_POSIX
    FileSystem::Exception::ThrowPOSIXErrNoIfNegative (::rename (fTmpFilePath_.c_str (), fRealFilePath_.c_str ()), fTmpFilePath_, fRealFilePath_);
#elif qStroika_Foundation_Common_Platform_Windows
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

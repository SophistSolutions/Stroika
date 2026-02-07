/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>
#include <fstream>
#include <random>

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
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Throw.h"
#if qStroika_Foundation_Common_Platform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/FileSystem/Exception.h"

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

namespace {
    // @todo - redo using open (O_CREAT, but as portably as possible - at least do windows/POSIX impl, and maybe fallback on fstream approach)
    bool tryCreateFile_ (const filesystem::path& p)
    {
        error_code ec;
        // Check if file already exists
        if (filesystem::exists (p, ec)) {
            if (!ec) {
                return false; // File already exists
            }
            else {
                DbgTrace ("Error checking file existence: {}"_f, String::FromNarrowSDKString (ec.message ()));
                return false;
            }
        }

        // Try creating the file - NOTE THIS IS STILL A RACE - TWO PROCESSES COULD DO SAME THING AT ONCE!
        ofstream ofs{p};
        if (ofs.is_open ()) {
            ofs.close ();
            return true; // File successfully created
        }
        else {
            return false; // Creation failed (e.g., permission denied, invalid path)
        }
    }
}

/*
 ********************************************************************************
 ************************ FileSystem::ThroughTmpFileWriter **********************
 ********************************************************************************
 */
ThroughTmpFileWriter::ThroughTmpFileWriter (const filesystem::path& realFileName, const String& tmpSuffix)
    : fRealFilePath_{realFileName}
{
    Require (not realFileName.empty ());
    Require (not tmpSuffix.empty ());
    // keep generating random names, and trying to create til we succeed
    filesystem::path useTmpPath = realFileName;
    useTmpPath.replace_extension ();
    String           baseStem{useTmpPath.stem ()};
    filesystem::path newExtension = tmpSuffix.As<filesystem::path> ();
    create_directories (useTmpPath.parent_path ());
    default_random_engine         gen{random_device{}()}; //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<int> distribution{1, 99999};
    while (true) {
        filesystem::path newFN = "{}-{}"_f(baseStem, distribution (gen)).As<filesystem::path> ();
        useTmpPath.replace_filename (newFN);
        useTmpPath.replace_extension (newExtension);
        if (tryCreateFile_ (useTmpPath)) {
            fTmpFilePath_ = useTmpPath;
            return;
        }
        DbgTrace ("randomfile name conflict, so trying again (should be rare): f={}"_f, useTmpPath);
        WeakAssertNotReached ();
    }
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

    auto activity = LazyEvalActivity ([&] () -> String { return "committing temporary file '{}' to '{}'"_f(fTmpFilePath_, fRealFilePath_); });
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
        // Sadly this happens pretty often on Windoze, due to virus scanners. But when that is the cause, retrying
        // a little later should do the trick --LGP 2026-02-07
        else if (we.code () == error_code{ERROR_SHARING_VIOLATION, system_category ()} or
                 we.code () == error_code{ERROR_ACCESS_DENIED, system_category ()}) {
            auto retryLoop = [&] () {
                if (fRetryOnSharingViolationFor != kRetryOnSharingViolationFor_Disable) {
                    DbgTrace ("ThroughTmpFileWriter::Commit: {}, so retrying for {}"_f,
                              we.code ().value () == ERROR_SHARING_VIOLATION ? "ERROR_SHARING_VIOLATION"_k : "ERROR_ACCESS_DENIED"_k,
                              fRetryOnSharingViolationFor.value_or (kRetryOnSharingViolationFor_Default));
                    Time::TimePointSeconds until = Time::GetTickCount () + fRetryOnSharingViolationFor.value_or (kRetryOnSharingViolationFor_Default);
                    unsigned int nRetries = 0;
                    do {
                        Execution::Sleep (nRetries * 10ms);
                        if (BOOL r = ::MoveFileExW (fTmpFilePath_.c_str (), fRealFilePath_.c_str (), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
                            DbgTrace ("retry worked!"_f);
                            return; // return from retryLoop, not ThroughTmpFileWriter::Commit
                        }
                        else {
                            DWORD lastError = ::GetLastError ();
                            Assert (lastError != 0);
                            if (lastError != ERROR_SHARING_VIOLATION) {
                                Execution::ThrowSystemErrNo (lastError);
                            }
                        }
                        nRetries++;
                    } while (until < Time::GetTickCount ());
                }
                ReThrow ();
            };
            retryLoop ();
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

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::IO::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Process.h"
#include "Stroika/Foundation/IO/FileSystem/DirectoryIterable.h"
#include "Stroika/Foundation/IO/FileSystem/DirectoryIterator.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileSystem.h"
#include "Stroika/Foundation/IO/FileSystem/MountedFilesystem.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO::FileSystem;

namespace {
    void Test1_DirectoryIterator_ ()
    {
        Debug::TraceContextBumper ctx ("Test1_DirectoryIterator_");
        {
            Debug::TraceContextBumper ctx1 ("simple test");
            for (DirectoryIterator i{WellKnownLocations::GetTemporary ()}; not i.Done (); ++i) {
                DbgTrace (L"filename = %s", i->c_str ());
            }
        }
        {
            Debug::TraceContextBumper ctx1 ("t2");
            DirectoryIterator         i{WellKnownLocations::GetTemporary ()};
            for (DirectoryIterator i2 = i; not i2.Done (); ++i2) {
                DbgTrace (L"filename = %s", i2->c_str ());
            }
        }
    }
}

namespace {
    void Test2_DirectoryIterable_ ()
    {
        Debug::TraceContextBumper ctx ("Test2_DirectoryIterable_");
        for (String filename : DirectoryIterable (WellKnownLocations::GetTemporary ())) {
            DbgTrace (L"filename = %s", filename.c_str ());
        }
        {
            Debug::TraceContextBumper            ctx1 ("test-known-dir");
            static const Containers::Set<String> kFileNamesForDir_{L"foo.txt", L"bar.png", L"t3.txt", L"blag.nope"};
            static const String                  kTestSubDir_ = AssureDirectoryPathSlashTerminated (WellKnownLocations::GetTemporary () + L"Regtest-write-files-" + Characters::ToString (Execution::GetCurrentProcessID ()));
            IO::FileSystem::Default ().RemoveDirectoryIf (kTestSubDir_, IO::FileSystem::eRemoveAnyContainedFiles);
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([] () noexcept {
                IgnoreExceptionsForCall (IO::FileSystem::Default ().RemoveDirectoryIf (kTestSubDir_, IO::FileSystem::eRemoveAnyContainedFiles));
            });
            IO::FileSystem::Directory (kTestSubDir_).AssureExists ();
            kFileNamesForDir_.Apply ([] (String i) { IO::FileSystem::FileOutputStream::New (kTestSubDir_ + i); });
            //DbgTrace (L"kTestSubDir_=%s", kTestSubDir_.c_str ());
            //DbgTrace (L"kFileNamesForDir_=%s", Characters::ToString (kFileNamesForDir_).c_str ());
            //DbgTrace (L"DirectoryIterable (kTestSubDir_)=%s", Characters::ToString (DirectoryIterable (kTestSubDir_)).c_str ());
            VerifyTestResult (kFileNamesForDir_ == DirectoryIterable (kTestSubDir_));
            {
                Containers::Set<String>     answers1;
                Containers::Set<String>     answers2;
                DirectoryIterable           tmp (kTestSubDir_);
                Traversal::Iterator<String> i2 = tmp.end (); // we had a bug with copying iterator - when refcnt != 1 - hangs - never advances... Windows only
                for (Traversal::Iterator<String> i = tmp.begin (); i != tmp.end (); ++i) {
                    answers1 += *i;
                    i2 = i;
                    answers2 += *i;
                }
                VerifyTestResult (kFileNamesForDir_ == answers1);
                VerifyTestResult (kFileNamesForDir_ == answers2);
            }
        }
    }
}

namespace {
    namespace Test3_Pathnames_ {
        void Test_ExtractDirAndBaseName_ ()
        {
// Tests from DOCS line in ExtractDirAndBaseName
#if qPlatform_POSIX
            VerifyTestResult ((ExtractDirAndBaseName (L"/usr/lib") == pair<String, String>{L"/usr/", L"lib"}));
            VerifyTestResult ((ExtractDirAndBaseName (L"/usr/") == pair<String, String>{L"/", L"usr/"}));
            VerifyTestResult ((ExtractDirAndBaseName (L"usr") == pair<String, String>{L"./", L"usr"}));
            VerifyTestResult ((ExtractDirAndBaseName (L"/") == pair<String, String>{L"/", L""}));
            VerifyTestResult ((ExtractDirAndBaseName (L".") == pair<String, String>{L"./", L"."}));
            VerifyTestResult ((ExtractDirAndBaseName (L"..") == pair<String, String>{L"./", L".."}));
#elif qPlatform_Windows
            VerifyTestResult ((ExtractDirAndBaseName (L"\\usr\\lib") == pair<String, String>{L"\\usr\\", L"lib"}));
            VerifyTestResult ((ExtractDirAndBaseName (L"\\usr\\") == pair<String, String>{L"\\", L"usr\\"}));
            VerifyTestResult ((ExtractDirAndBaseName (L"usr") == pair<String, String>{L".\\", L"usr"}));
            VerifyTestResult ((ExtractDirAndBaseName (L"\\") == pair<String, String>{L"\\", L""}));
            VerifyTestResult ((ExtractDirAndBaseName (L".") == pair<String, String>{L".\\", L"."}));
            VerifyTestResult ((ExtractDirAndBaseName (L"..") == pair<String, String>{L".\\", L".."}));
            VerifyTestResult ((ExtractDirAndBaseName (L"c:\\h\\m.t") == pair<String, String>{L"c:\\h\\", L"m.t"}));
#endif
        }
        void Test_GetFileBaseName_ ()
        {
            VerifyTestResult (GetFileBaseName (L"foo") == L"foo");
            VerifyTestResult (GetFileBaseName (L"foo.cpp") == L"foo");
            VerifyTestResult (GetFileBaseName (L"foo.exe") == L"foo");
            VerifyTestResult (GetFileBaseName (L".exe") == L".exe");
#if qPlatform_POSIX
            VerifyTestResult (GetFileBaseName (L"/tmp/.CPUBurner") == L".CPUBurner");
#elif qPlatform_Windows
            VerifyTestResult (GetFileBaseName (L"c:\\tmp\\.CPUBurner") == L".CPUBurner");
#endif
        }

        void DoTest ()
        {
            Debug::TraceContextBumper ctx ("Test3_Pathnames_");
            Test_ExtractDirAndBaseName_ ();
            Test_GetFileBaseName_ ();
        }
    }
}

namespace {
    namespace Test4_MountedFilesystems_ {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx ("Test4_MountedFilesystems_");
            for (auto i : IO::FileSystem::GetMountedFilesystems ()) {
                DbgTrace (L"fs=%s", Characters::ToString (i).c_str ());
            }
        }
    }
}

namespace {
    namespace Test5_DisksPresent_ {
        void DoTest ()
        {
            Debug::TraceContextBumper ctx ("Test5_DisksPresent_");
            for (auto i : IO::FileSystem::GetAvailableDisks ()) {
                DbgTrace (L"d=%s", Characters::ToString (i).c_str ());
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_DirectoryIterator_ ();
        Test2_DirectoryIterable_ ();
        Test3_Pathnames_::DoTest ();
        Test4_MountedFilesystems_::DoTest ();
        Test5_DisksPresent_::DoTest ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}

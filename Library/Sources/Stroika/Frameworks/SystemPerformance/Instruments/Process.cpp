/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#if qPlatform_Linux
#include <netinet/tcp.h>
#include <sys/sysinfo.h>
#elif qPlatform_Windows
#include <Windows.h>

#include <Wdbgexts.h>
#include <psapi.h>
#endif

#include <filesystem>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/SystemConfiguration.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/MultiSet.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileSystem.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Streams/iostream/FStreamSupport.h"
#include "Stroika/Foundation/Time/Duration.h"

#if qPlatform_POSIX
#include "Stroika/Foundation/Execution/Platform/POSIX/Users.h"
#elif qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#include "Stroika/Foundation/Execution/Platform/Windows/Users.h"
#endif

#include "Stroika/Frameworks/SystemPerformance/Support/InstrumentHelpers.h"

#include "Process.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;
using namespace Stroika::Frameworks::SystemPerformance::Instruments;

using std::byte;

using Time::DurationSeconds;
using Time::TimePointSeconds;

using Instruments::Process::CachePolicy;
using Instruments::Process::Info;
using Instruments::Process::MemorySizeType;
using Instruments::Process::Options;
using Instruments::Process::ProcessMapType;
using Instruments::Process::ProcessType;

// --LGP 2016-03-11
// Sadly - though ALMOST working - not quite.
// Produces bogus PIDs, and misses many, and thread counts sometimes right, and sometimes wrong (maybe due to WOW64?)
// Anyhow - disable til working reliably...
// Or permanently. But keep around a bit, as lots of good stuff there and gives one quick copy to get out alot of data we want,
// avoiding other calls
#define qUseWinInternalSupport_ 0
#ifndef qUseWinInternalSupport_
#define qUseWinInternalSupport_ qPlatform_Windows
#endif

// This appears to work, but I fear (not tested) its not super performant - performance not tested -- LGP 2016-03-11
//#define   qUseCreateToolhelp32SnapshotToCountThreads      0
#ifndef qUseCreateToolhelp32SnapshotToCountThreads
#define qUseCreateToolhelp32SnapshotToCountThreads qPlatform_Windows
#endif

// Still maybe needed for thread count -- but check with ifdefs
#define qUseWMICollectionSupport_ 0
#ifndef qUseWMICollectionSupport_
#define qUseWMICollectionSupport_ qPlatform_Windows && (!qUseCreateToolhelp32SnapshotToCountThreads and !qUseWinInternalSupport_)
#endif

#if qUseWinInternalSupport_
#if 1
//avoid redef warnings... maybe not needed -
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#else
#include <ntstatus.h>
#endif
#include <Winternl.h>
#if defined(_MSC_VER)
#pragma comment(lib, "Ntdll.lib") // Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#endif
#endif

#if qUseCreateToolhelp32SnapshotToCountThreads
#include <tlhelp32.h>
#if defined(_MSC_VER)
#pragma comment(lib, "Ntdll.lib") // Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#endif
#endif

#if qUseWMICollectionSupport_
#include "Stroika/Frameworks/SystemPerformance/Support/WMICollector.h"

using SystemPerformance::Support::WMICollector;
#endif

#if defined(_MSC_VER)
#pragma comment(lib, "psapi.lib") // Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#endif

#if qPlatform_Windows
namespace {
    struct SetPrivilegeInContext_ {
        enum IgnoreError {
            eIgnoreError
        };
        HANDLE    fToken_{INVALID_HANDLE_VALUE};
        SDKString fPrivilege_;
        SetPrivilegeInContext_ (LPCTSTR privilege)
            : fPrivilege_{privilege}
        {
            try {
                setupToken_ ();
                Execution::Platform::Windows::ThrowIfZeroGetLastError (SetPrivilege_ (fToken_, fPrivilege_.c_str (), true));
            }
            catch (...) {
                if (fToken_ != INVALID_HANDLE_VALUE) {
                    ::CloseHandle (fToken_); // no nee dto clear fToken_ cuz never fully constructed
                }
                Execution::ReThrow ();
            }
        }
        SetPrivilegeInContext_ (LPCTSTR privilege, IgnoreError)
            : fPrivilege_{privilege}
        {
            bool failed{false};
            try {
                setupToken_ ();
                if (not SetPrivilege_ (fToken_, fPrivilege_.c_str (), true)) {
                    DbgTrace ("Failed to set privilege: error#: {}"_f, ::GetLastError ()); // avoid through so we don't pollute log with throw/catch stuff
                    failed = true;                                                         // IgnoreError
                }
            }
            catch (...) {
                failed = true; // IgnoreError
            }
            if (failed) {
                if (fToken_ != INVALID_HANDLE_VALUE) {
                    ::CloseHandle (fToken_);
                    fToken_ = INVALID_HANDLE_VALUE; // do not double closed in DTOR
                }
            }
        }
        SetPrivilegeInContext_ (const SetPrivilegeInContext_&)            = delete;
        SetPrivilegeInContext_& operator= (const SetPrivilegeInContext_&) = delete;
        ~SetPrivilegeInContext_ ()
        {
            if (fToken_ != INVALID_HANDLE_VALUE) {
                SetPrivilege_ (fToken_, fPrivilege_.c_str (), false);
                Verify (::CloseHandle (fToken_));
            }
        }

    private:
        void setupToken_ ()
        {
            if (not ::OpenThreadToken (::GetCurrentThread (), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, false, &fToken_)) {
                if (::GetLastError () == ERROR_NO_TOKEN) {
                    Execution::Platform::Windows::ThrowIfZeroGetLastError (::ImpersonateSelf (SecurityImpersonation));
                    Execution::Platform::Windows::ThrowIfZeroGetLastError (
                        ::OpenThreadToken (::GetCurrentThread (), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &fToken_));
                }
                else {
                    Execution::ThrowSystemErrNo ();
                }
            }
        }
        bool SetPrivilege_ (HANDLE hToken, LPCTSTR privilege, bool bEnablePrivilege) noexcept
        {
            LUID luid;
            if (::LookupPrivilegeValue (nullptr, privilege, &luid) == 0) {
                return false;
            }

            //
            // first pass.  get current privilege setting
            //
            TOKEN_PRIVILEGES tp;
            tp.PrivilegeCount           = 1;
            tp.Privileges[0].Luid       = luid;
            tp.Privileges[0].Attributes = 0;

            TOKEN_PRIVILEGES tpPrevious;
            DWORD            cbPrevious = sizeof (tpPrevious);
            ::AdjustTokenPrivileges (hToken, false, &tp, sizeof (tp), &tpPrevious, &cbPrevious);
            if (::GetLastError () != ERROR_SUCCESS) {
                // weird but docs for AdjustTokenPrivileges unclear if you can check for failure with return value - or rahter if not updating all privs
                // counts as failure...
                return false;
            }

            //
            // second pass.  set privilege based on previous setting
            //
            tpPrevious.PrivilegeCount     = 1;
            tpPrevious.Privileges[0].Luid = luid;

            if (bEnablePrivilege) {
                tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
            }
            else {
                tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED & tpPrevious.Privileges[0].Attributes);
            }
            ::AdjustTokenPrivileges (hToken, false, &tpPrevious, cbPrevious, nullptr, nullptr);
            if (::GetLastError () != ERROR_SUCCESS) {
                // weird but docs for AdjustTokenPrivileges unclear if you can check for failure with return value - or rahter if not updating all privs
                // counts as failure...
                return false;
            }
            return true;
        }
    };
}
#endif

#if qUseCreateToolhelp32SnapshotToCountThreads
namespace {
    class ThreadCounter_ {
    private:
        MultiSet<pid_t> fThreads_;

    public:
        ThreadCounter_ ()
        {
            HANDLE hThreadSnap = ::CreateToolhelp32Snapshot (TH32CS_SNAPTHREAD, 0);
            if (hThreadSnap == INVALID_HANDLE_VALUE) {
                DbgTrace (L"CreateToolhelp32Snapshot failed: {}"_f, ::GetLastError ());
                return;
            }
            [[maybe_unused]] auto&& cleanup = Execution::Finally ([hThreadSnap] () noexcept { ::CloseHandle (hThreadSnap); });

            // Fill in the size of the structure before using it.
            THREADENTRY32 te32{};
            te32.dwSize = sizeof (THREADENTRY32);

            // Retrieve information about the first thread, and exit if unsuccessful
            if (not ::Thread32First (hThreadSnap, &te32)) {
                DbgTrace (L"CreateToolhelp32Snapshot failed: {}"_f, ::GetLastError ());
                return;
            }
            // Now walk the thread list of the system,
            do {
                fThreads_.Add (te32.th32OwnerProcessID);
            } while (::Thread32Next (hThreadSnap, &te32));
        }

    public:
        optional<unsigned int> CountThreads (pid_t pid) const
        {
            return fThreads_.OccurrencesOf (pid);
        }
    };
}
#endif

/*
 ********************************************************************************
 ****************** Instruments::Process::ProcessType ***************************
 ********************************************************************************
 */
String Instruments::Process::ProcessType::ToString () const
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (Process::Instrument::kObjectVariantMapper.FromObject (*this));
}

namespace {
    struct ModuleCommonContext_ : SystemPerformance::Support::Context {
        // skip reporting static (known at process start) data on subsequent reports
        // only used if fCachePolicy == CachePolicy::eOmitUnchangedValues
        Set<pid_t> fStaticSuppressedAgain;
    };
    template <typename CONTEXT>
    using InstrumentRepBase_ = SystemPerformance::Support::InstrumentRep_COMMON<Options, CONTEXT>;
}

#if qPlatform_Linux
namespace {
    /*
     *  Missing items we don't currently capture:
     *      >   fWorkingSetSize
     *
     *  Fragile or broken:
     *      >   fPrivateBytes   doesn't work on RedHat5 - must use /proc/PID/map (see http://stackoverflow.com/questions/1401359/understanding-linux-proc-id-maps)
     */
    struct PerfStats_ {
        TimePointSeconds          fCapturedAt;
        optional<DurationSeconds> fTotalCPUTimeEverUsed;
        optional<double>          fCombinedIOReadBytes;
        optional<double>          fCombinedIOWriteBytes;
    };
    struct _Context : ModuleCommonContext_ {
        Mapping<pid_t, PerfStats_> fMap;
    };

    struct InstrumentRep_Linux_ : InstrumentRepBase_<_Context> {

        using InstrumentRepBase_<_Context>::InstrumentRepBase_;

        ProcessMapType _InternalCapture ()
        {
            ProcessMapType result{};
            if (_fOptions.fAllowUse_ProcFS) {
                result = ExtractFromProcFS_ ();
            }
            else if (_fOptions.fAllowUse_PS) {
                result = capture_using_ps_ ();
            }
            return result;
        }

    private:
        // One character from the string "RSDZTW" where R is running,
        // S is sleeping in an interruptible wait, D is waiting in uninterruptible disk sleep,
        // Z is zombie, T is traced or stopped (on a signal), and W is paging.
        static optional<ProcessType::RunStatus> cvtStatusCharToStatus_ (char state)
        {
            switch (state) {
                case 'R':
                    return ProcessType::RunStatus::eRunning;
                case 'S':
                    return ProcessType::RunStatus::eSleeping;
                case 'D':
                    return ProcessType::RunStatus::eWaitingOnDisk;
                case 'Z':
                    return ProcessType::RunStatus::eZombie;
                case 'T':
                    return ProcessType::RunStatus::eSuspended;
                case 'W':
                    return ProcessType::RunStatus::eWaitingOnPaging;
            }
            return nullopt;
        }

        ProcessMapType ExtractFromProcFS_ ()
        {
            //
            /// Most status - like time - come from http://linux.die.net/man/5/proc
            ///proc/[pid]/stat
            //  Status information about the process. This is used by ps(1). It is defined in /usr/src/linux/fs/proc/array.c.
            //
            static const filesystem::path kCWDFilename_{"cwd"sv};
            static const filesystem::path kEXEFilename_{"exe"sv};
            static const filesystem::path kEnvironFilename_{"environ"sv};
            static const filesystem::path kRootFilename_{"root"sv};
            static const filesystem::path kCmdLineFilename_{"cmdline"sv};
            static const filesystem::path kStatFilename_{"stat"sv};
            static const filesystem::path kStatusFilename_{"status"sv};
            static const filesystem::path kIOFilename_{"io"sv};
            static const filesystem::path kNetTCPFilename_{"net/tcp"sv};

            ProcessMapType results;

            Mapping<pid_t, PerfStats_> newContextStats;

            /*
             *  NOTE: the Linux procfs allows access to PROCESS info or THREADINFO (what linux calls lightweight processes).
             *
             *  You can tell if a process is a real process id or thread id, by seeing if the tgid (sometimes tid) or task
             *  group id) is the same as the PID. If yes, its a process. If no, its a thread in the tgid process.
             *
             *  However, iterating over the files in the /proc filesystem APPEARS to only produce real processes, and to skip
             *  the lightweight process thread ids,  so we don't need to specially filter them out. However, I've not found
             *  this claim documented anywhere, so beware...
             */
            for (const auto& p :
                 filesystem::directory_iterator{"/proc", filesystem::directory_options{filesystem::directory_options::skip_permission_denied}}) {
                const filesystem::path& dir               = p.path (); // full-path
                String                  dirFileNameString = IO::FileSystem::FromPath (dir.filename ());
                bool                    isAllNumeric = not dirFileNameString.Find ([] (Character c) -> bool { return not c.IsDigit (); });
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{"...SystemPerformance::Instruments::Process::{}::ExtractFromProcFS_::reading /proc files"};
                DbgTrace (L"isAllNumeric=%d, dir= %s, is_dir=%d", isAllNumeric, ToString (dir).c_str (), p.is_directory ());
#endif
                if (isAllNumeric and p.is_directory ()) {
                    pid_t            pid{String2Int<pid_t> (dirFileNameString)};
                    TimePointSeconds now{Time::GetTickCount ()};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("reading for pid = %d", pid);
#endif
                    if (_fOptions.fRestrictToPIDs) {
                        if (not _fOptions.fRestrictToPIDs->Contains (pid)) {
                            continue;
                        }
                    }
                    if (_fOptions.fOmitPIDs) {
                        if (_fOptions.fOmitPIDs->Contains (pid)) {
                            continue;
                        }
                    }

                    bool grabStaticData = _fOptions.fCachePolicy == CachePolicy::eIncludeAllRequestedValues or
                                          not _fContext.cget ().cref ()->fStaticSuppressedAgain.Contains (pid);

                    ProcessType processDetails;

                    if (grabStaticData) {
                        processDetails.fEXEPath = OptionallyResolveShortcut_ (dir / kEXEFilename_);
                        if (processDetails.fEXEPath and IO::FileSystem::FromPath (*processDetails.fEXEPath).EndsWith (" (deleted)"sv)) {
                            processDetails.fEXEPath =
                                IO::FileSystem::ToPath (IO::FileSystem::FromPath (*processDetails.fEXEPath).SubString (0, -10));
                        }

                        if (_fOptions.fProcessNameReadPolicy == Options::eAlways or
                            (_fOptions.fProcessNameReadPolicy == Options::eOnlyIfEXENotRead and not processDetails.fEXEPath.has_value ())) {
                            processDetails.fProcessName =
                                OptionallyReadIfFileExists_<String> (dir / "comm"sv, [] (const Streams::InputStream::Ptr<byte>& in) {
                                    return TextReader::New (in).ReadAll ().Trim ();
                                });
                        }

                        /*
                         *      \note   In POSIX/fAllowUse_ProcFS mode Fix EXEPath/commandline for 'kernel' processes.
                         *              http://unix.stackexchange.com/questions/191594/how-can-i-determine-if-a-process-is-a-system-process
                         *              Can use value from reading EXE or is parent process id is 0, or parent process id is kernel process
                         *              (means kernel process/thread).
                         *
                         *  Improve this logic below - checking for exact error code from readlink..as they say in that article.
                         */
                        processDetails.fKernelProcess = not processDetails.fEXEPath.has_value ();
                        // Note - many kernel processes have commandline, so don't filter here based on that
                        if (_fOptions.fCaptureCommandLine and _fOptions.fCaptureCommandLine (pid, NullCoalesce (processDetails.fEXEPath))) {
                            processDetails.fCommandLine = ReadCmdLineString_ (dir / kCmdLineFilename_);
                        }
                        // kernel process cannot chroot (as far as I know) --LGP 2015-05-21
                        if (_fOptions.fCaptureRoot and processDetails.fKernelProcess == false) {
                            processDetails.fRoot = OptionallyResolveShortcut_ (dir / kRootFilename_);
                        }
                        // kernel process cannot have environment variables (as far as I know) --LGP 2015-05-21
                        if (_fOptions.fCaptureEnvironmentVariables and processDetails.fKernelProcess == false) {
                            processDetails.fEnvironmentVariables = OptionallyReadFileStringsMap_ (dir / kEnvironFilename_);
                        }
                    }

                    // kernel process cannot have current directory (as far as I know) --LGP 2015-05-21
                    if (_fOptions.fCaptureCurrentWorkingDirectory and processDetails.fKernelProcess == false) {
                        processDetails.fCurrentWorkingDirectory = OptionallyResolveShortcut_ (dir / kCWDFilename_);
                    }

                    static const double kClockTick_ = ::sysconf (_SC_CLK_TCK);

                    try {
                        StatFileInfo_ stats = ReadStatFile_ (dir / kStatFilename_);

                        processDetails.fRunStatus = cvtStatusCharToStatus_ (stats.state);

                        static const size_t kPageSizeInBytes_ = ::sysconf (_SC_PAGESIZE);

                        if (grabStaticData) {
                            static const time_t kUNIXEpochTimeOfBoot_ = [] () {
                                struct sysinfo info;
                                ::sysinfo (&info);
                                return ::time (NULL) - info.uptime;
                            }();
                            //starttime %llu (was %lu before Linux 2.6)
                            //(22) The time the process started after system boot. In kernels before Linux 2.6,
                            // this value was expressed in jiffies. Since Linux 2.6,
                            // the value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK)).
                            processDetails.fProcessStartedAt = DateTime{static_cast<time_t> (stats.start_time / kClockTick_ + kUNIXEpochTimeOfBoot_)};
                        }

                        processDetails.fTotalCPUTimeEverUsed = DurationSeconds{(double (stats.utime) + double (stats.stime)) / kClockTick_};
                        if (optional<PerfStats_> p = _fContext.load ()->fMap.Lookup (pid)) {
                            auto diffTime = now - p->fCapturedAt;
                            if (p->fTotalCPUTimeEverUsed and (diffTime >= _fOptions.fMinimumAveragingInterval)) {
                                processDetails.fAverageCPUTimeUsed =
                                    DurationSeconds{(*processDetails.fTotalCPUTimeEverUsed - *p->fTotalCPUTimeEverUsed) / diffTime.count ()};
                            }
                        }
                        if (stats.nlwp != 0) {
                            processDetails.fThreadCount = stats.nlwp;
                        }
                        if (grabStaticData) {
                            processDetails.fParentProcessID = stats.ppid;
                        }

                        processDetails.fPrivateVirtualMemorySize = stats.vsize;

                        // Don't know how to easily compute, but I'm sure not hard (add in shared memory of various sorts at worst) - or look at memory map, but
                        // very low priority (like smaps file below)
                        //processDetails.fTotalVirtualMemorySize = stats.vsize;

                        processDetails.fResidentMemorySize = stats.rss * kPageSizeInBytes_;

                        processDetails.fPageFaultCount      = stats.minflt + stats.majflt;
                        processDetails.fMajorPageFaultCount = stats.majflt;

                        /*
                         * Probably best to compute fPrivateBytes from:
                         *       grep  Private /proc/1912/smaps
                         */
                        processDetails.fPrivateBytes = ReadPrivateBytes_ (dir / "smaps");

#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        DbgTrace ("loaded processDetails.fProcessStartedAt={} wuit stats.start_time = {}"_f,
                                  processDetails.fProcessStartedAt, stats.start_time);
                        DbgTrace ("loaded processDetails.fTotalCPUTimeEverUsed={} wuit stats.utime = {}, stats.stime = {}"_f,
                                  processDetails.fTotalCPUTimeEverUsed->count (), stats.utime, stats.stime);
#endif
                    }
                    catch (...) {
                    }

                    if (_fOptions.fCaptureTCPStatistics) {
                        IgnoreExceptionsForCall (processDetails.fTCPStats = ReadTCPStats_ (dir / kNetTCPFilename_));
                    }

                    if (grabStaticData) {
                        try {
                            if (processDetails.fKernelProcess == true) {
                                // I think these are always running as root -- LGP 2015-05-21
                                processDetails.fUserName = "root"sv;
                            }
                            else {
                                proc_status_data_ stats  = Readproc_proc_status_data_ (dir / kStatusFilename_);
                                processDetails.fUserName = Execution::Platform::POSIX::uid_t2UserName (stats.ruid);
                            }
                        }
                        catch (...) {
                        }
                    }

                    try {
                        // @todo maybe able to optimize and not check this if processDetails.fKernelProcess == true
                        optional<proc_io_data_> stats = Readproc_io_data_ (dir / kIOFilename_);
                        if (stats.has_value ()) {
                            processDetails.fCombinedIOReadBytes  = (*stats).read_bytes;
                            processDetails.fCombinedIOWriteBytes = (*stats).write_bytes;
                            if (optional<PerfStats_> p = _fContext.load ()->fMap.Lookup (pid)) {
                                DurationSeconds diffTime = now - p->fCapturedAt;
                                if (diffTime >= _fOptions.fMinimumAveragingInterval) {
                                    if (p->fCombinedIOReadBytes) {
                                        processDetails.fCombinedIOReadRate =
                                            (*processDetails.fCombinedIOReadBytes - *p->fCombinedIOReadBytes) / diffTime.count ();
                                    }
                                    if (p->fCombinedIOWriteBytes) {
                                        processDetails.fCombinedIOWriteRate =
                                            (*processDetails.fCombinedIOWriteBytes - *p->fCombinedIOWriteBytes) / diffTime.count ();
                                    }
                                }
                            }
                        }
                    }
                    catch (...) {
                        DbgTrace ("ignored: {}"_f, current_exception ());
                    }

                    if (processDetails.fTotalCPUTimeEverUsed or processDetails.fCombinedIOReadBytes or processDetails.fCombinedIOWriteBytes) {
                        newContextStats.Add (pid, PerfStats_{now, processDetails.fTotalCPUTimeEverUsed, processDetails.fCombinedIOReadBytes,
                                                             processDetails.fCombinedIOWriteBytes});
                    }
                    results.Add (pid, processDetails);
                }
            }
            _NoteCompletedCapture ();
            _fContext.rwget ().rwref ()->fMap = newContextStats;
            if (_fOptions.fCachePolicy == CachePolicy::eOmitUnchangedValues) {
                _fContext.rwget ().rwref ()->fStaticSuppressedAgain = Set<pid_t>{results.Keys ()};
            }
            return results;
        }
        template <typename T>
        static optional<T> OptionallyReadIfFileExists_ (const filesystem::path&                                     fullPath,
                                                        const function<T (const Streams::InputStream::Ptr<byte>&)>& reader)
        {
            if (IO::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (
                    return reader (IO::FileSystem::FileInputStream::New (fullPath, IO::FileSystem::FileInputStream::eNotSeekable)));
            }
            return nullopt;
        }
        static Sequence<String> ReadFileStrings_ (const filesystem::path& fullPath)
        {
            Sequence<String> results;
            Streams::InputStream::Ptr<byte> in = IO::FileSystem::FileInputStream::New (fullPath, IO::FileSystem::FileInputStream::eNotSeekable);
            StringBuilder sb;
            for (optional<byte> b; (b = in.Read ()).has_value ();) {
                if ((*b) == byte{0}) {
                    results.Append (sb.As<String> ());
                    sb.clear ();
                }
                else {
                    sb.Append ((char)(*b)); // for now assume no charset
                }
            }
            return results;
        }
        static Mapping<String, String> ReadFileStringsMap_ (const filesystem::path& fullPath)
        {
            Mapping<String, String> results;
            for (const String& i : ReadFileStrings_ (fullPath)) {
                auto tokens = i.Tokenize ({'='});
                if (tokens.size () == 2) {
                    results.Add (tokens[0], tokens[1]);
                }
            }
            return results;
        }
        // if fails (cuz not readable) don't throw but return missing, but avoid noisy stroika exception logging
        static optional<String> ReadCmdLineString_ (const filesystem::path& fullPath2CmdLineFile)
        {
            // this reads /proc format files - meaning that a trialing nul-byte is the EOS
            auto ReadFileString_ = [] (const Streams::InputStream::Ptr<byte>& in) -> String {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{"Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::ReadCmdLineString_"};
#endif
                StringBuilder sb;
                bool          lastCharNullRemappedToSpace = false;
                for (optional<byte> b; (b = in.Read ()).has_value ();) {
                    if (*b == byte{0}) {
                        sb.Append (' '); // frequently - especially for kernel processes - we see nul bytes that really SB spaces
                        lastCharNullRemappedToSpace = true;
                    }
                    else {
                        sb.Append ((char)(*b)); // for now assume no charset
                        lastCharNullRemappedToSpace = false;
                    }
                }
                if (lastCharNullRemappedToSpace) {
                    Assert (sb.length () > 0 and sb.GetAt (sb.length () - 1) == ' ');
                    return sb.str ().SubString (0, sb.length () - 1);
                }
                else {
                    return sb.As<String> ();
                }
            };
            if (IO::FileSystem::Default ().Access (fullPath2CmdLineFile)) {
                IgnoreExceptionsExceptThreadAbortForCall (return ReadFileString_ (
                    IO::FileSystem::FileInputStream::New (fullPath2CmdLineFile, IO::FileSystem::FileInputStream::eNotSeekable)));
            }
            return nullopt;
        }
        // if fails (cuz not readable) don't throw but return missing, but avoid noisy stroika exception logging
        static optional<filesystem::path> OptionallyResolveShortcut_ (const filesystem::path& shortcutPath)
        {
            std::error_code ec{};
            if (filesystem::exists (shortcutPath, ec) and filesystem::is_symlink (shortcutPath, ec)) {
                auto r = filesystem::read_symlink (shortcutPath, ec);
                if (not ec) {
                    return r;
                }
            }
            return nullopt;
        }
        static optional<Mapping<String, String>> OptionallyReadFileStringsMap_ (const filesystem::path& fullPath)
        {
            if (IO::FileSystem::Default ().Access (fullPath)) {
                IgnoreExceptionsExceptThreadAbortForCall (return ReadFileStringsMap_ (fullPath));
            }
            return nullopt;
        }
        struct StatFileInfo_ {
            /*
             *  From  http://linux.die.net/man/5/proc,  search for '/proc/[pid]/stat'
             *
             *  (1)     pid %d
             *          The process ID.
             *
             *  (2)     comm %s
             *          The filename of the executable, in parentheses. This is visible whether or not the executable is swapped out.
             *
             *  (3)     state %c
             *          One character from the string "RSDZTW" where R is running, S is sleeping in an interruptible wait, D is waiting in uninterruptible disk sleep, Z is zombie, T is traced or stopped (on a signal), and W is paging.
             *
             *  (4)     ppid %d
             *          The PID of the parent.
             *
             *  (5)     pgrp %d
             *          The process group ID of the process.
             *
             *  (6)     session %d
             *          The session ID of the process.
             *
             *  (7)     tty_nr %d
             *          The controlling terminal of the process. (The minor device number is contained in the combination of bits 31 to 20 and 7 to 0; the major device number is in bits 15 to 8.)
             *
             *  (8)     tpgid %d
             *          The ID of the foreground process group of the controlling terminal of the process.
             *
             *  (9)     flags %u (%lu before Linux 2.6.22)
             *          The kernel flags word of the process. For bit meanings, see the PF_* defines in the Linux kernel source file include/linux/sched.h. Details depend on the kernel version.
             *
             *  (10)    minflt %lu
             *          The number of minor faults the process has made which have not required loading a memory page from disk.
             *
             *  (11)    cminflt %lu
             *          The number of minor faults that the process's waited-for children have made.
             *
             *  (12)    majflt %lu
             *          The number of major faults the process has made which have required loading a memory page from disk.
             *
             *  (13)    cmajflt %lu
             *          The number of major faults that the process's waited-for children have made.
             *
             *  (14)    utime %lu
             *          Amount of time that this process has been scheduled in user mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)). This includes guest time, guest_time (time spent running a virtual CPU, see below), so that applications that are not aware of the guest time field do not lose that time from their calculations.
             *
             *  (15)    stime %lu
             *          Amount of time that this process has been scheduled in kernel mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             *
             *  (16)    cutime %ld
             *          Amount of time that this process's waited-for children have been scheduled in user mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)). (See also times(2).) This includes guest time, cguest_time (time spent running a virtual CPU, see below).
             *
             *  (17)    cstime %ld
             *          Amount of time that this process's waited-for children have been scheduled in kernel mode, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             *
             *  (18)    priority %ld
             *          (Explanation for Linux 2.6) For processes running a real-time scheduling policy (policy below; see sched_setscheduler(2)), this is the negated scheduling priority, minus one; that is, a number in the range -2 to -100, corresponding to real-time priorities 1 to 99. For processes running under a non-real-time scheduling policy, this is the raw nice value (setpriority(2)) as represented in the kernel. The kernel stores nice values as numbers in the range 0 (high) to 39 (low), corresponding to the user-visible nice range of -20 to 19.
             *          Before Linux 2.6, this was a scaled value based on the scheduler weighting given to this process.
             *
             *  (19)    nice %ld
             *          The nice value (see setpriority(2)), a value in the range 19 (low priority) to -20 (high priority).
             *
             *  (20)    num_threads %ld
             *          Number of threads in this process (since Linux 2.6). Before kernel 2.6, this field was hard coded to 0 as a placeholder for an earlier removed field.
             *
             *  (21)    itrealvalue %ld
             *          The time in jiffies before the next SIGALRM is sent to the process due to an interval timer. Since kernel 2.6.17, this field is no longer maintained, and is hard coded as 0.
             *
             *  (22)    starttime %llu (was %lu before Linux 2.6)
             *          The time the process started after system boot. In kernels before Linux 2.6, this value was expressed in jiffies. Since Linux 2.6, the value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             *
             *  (23)    vsize %lu
             *          Virtual memory size in bytes.
             *
             *          from ps docs:
             *              size in physical pages of the core image of the process.  This includes text, data, and stack space.  Device mappings are currently excluded;
             *          Empirically thats what this appears to be.
             *
             *  (24)    rss %ld
             *          Resident Set Size: number of pages the process has in real memory. This is just the pages which count toward text, data, or stack space. This does not include pages which have not been demand-loaded in, or which are swapped out.
             *
             *  (25)    rsslim %lu
             *          Current soft limit in bytes on the rss of the process; see the description of RLIMIT_RSS in getrlimit(2).
             *
             *  (26)    startcode %lu
             *          The address above which program text can run.
             *
             *  (27)    endcode %lu
             *          The address below which program text can run.
             *
             *  (28)    startstack %lu
             *          The address of the start (i.e., bottom) of the stack.
             *
             *  (29)    kstkesp %lu
             *          The current value of ESP (stack pointer), as found in the kernel stack page for the process.
             *
             *  (30)    kstkeip %lu
             *          The current EIP (instruction pointer).
             *
             *  (31)    signal %lu
             *          The bitmap of pending signals, displayed as a decimal number. Obsolete, because it does not provide information on real-time signals; use /proc/[pid]/status instead.
             *
             *  (32)    blocked %lu
             *          The bitmap of blocked signals, displayed as a decimal number. Obsolete, because it does not provide information on real-time signals; use /proc/[pid]/status instead.
             *
             *  (33)    sigignore %lu
             *          The bitmap of ignored signals, displayed as a decimal number. Obsolete, because it does not provide information on real-time signals; use /proc/[pid]/status instead.
             *
             *  (34)    sigcatch %lu
             *          The bitmap of caught signals, displayed as a decimal number. Obsolete, because it does not provide information on real-time signals; use /proc/[pid]/status instead.
             *
             *  (35)    wchan %lu
             *          This is the "channel" in which the process is waiting. It is the address of a system call, and can be looked up in a namelist if you need a textual name. (If you have an up-to-date /etc/psdatabase, then try ps -l to see the WCHAN field in action.)
             *
             *  (36)    nswap %lu
             *          Number of pages swapped (not maintained).
             *
             *  (37)    cnswap %lu
             *          Cumulative nswap for child processes (not maintained).
             *
             *  (38)    exit_signal %d (since Linux 2.1.22)
             *          Signal to be sent to parent when we die.
             *
             *  (39)    processor %d (since Linux 2.2.8)
             *          CPU number last executed on.
             *
             *  (40)    rt_priority %u (since Linux 2.5.19; was %lu before Linux 2.6.22)
             *          Real-time scheduling priority, a number in the range 1 to 99 for processes scheduled under a real-time policy, or 0, for non-real-time processes (see sched_setscheduler(2)).
             *
             *  (41)    policy %u (since Linux 2.5.19; was %lu before Linux 2.6.22)
             *          Scheduling policy (see sched_setscheduler(2)). Decode using the SCHED_* constants in linux/sched.h.
             *
             *  (42)    delayacct_blkio_ticks %llu (since Linux 2.6.18)
             *          Aggregated block I/O delays, measured in clock ticks (centiseconds).
             *
             *  (43)    guest_time %lu (since Linux 2.6.24)
             *          Guest time of the process (time spent running a virtual CPU for a guest operating system), measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             *
             *  (44)    cguest_time %ld (since Linux 2.6.24)
             *          Guest time of the process's children, measured in clock ticks (divide by sysconf(_SC_CLK_TCK)).
             */
            char               state;      //  (3)
            int                ppid;       //  (4)
            unsigned long long utime;      //  (14)
            unsigned long long stime;      //  (15)
            int                nlwp;       //  (20)
            unsigned long long start_time; //  (22)
            unsigned long long vsize;      //  (23)
            unsigned long long rss;        //  (24)
            unsigned long      minflt;
            unsigned long      majflt;
        };
        static StatFileInfo_ ReadStatFile_ (const filesystem::path& fullPath)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::ReadStatFile_", "fullPath={}"_f, fullPath};
#endif
            StatFileInfo_ result{};
            Streams::InputStream::Ptr<byte> in = IO::FileSystem::FileInputStream::New (fullPath, IO::FileSystem::FileInputStream::eNotSeekable);
            byte   data[10 * 1024];
            size_t nBytes = in.ReadAll (span{data}).size ();
            Assert (nBytes <= NEltsOf (data));
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("nBytes read = {}"_f, nBytes);
#endif
            if (nBytes == NEltsOf (data)) {
                --nBytes; // ignore trailing byte so we can nul-terminate
            }
            data[nBytes] = byte{0}; // null-terminate so we can treat as c-string

            const char* S = reinterpret_cast<const char*> (data);
            {
                ///@TODO - FIX - THIS CODE UNSAFE - CAN CRASH! what if S not nul-terminated!
                S = ::strchr (S, '(') + 1;
                Assert (S < reinterpret_cast<const char*> (end (data)));
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("S = %x", S);
#endif
                const char* tmp = ::strrchr (S, ')');
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("S(tmp) = %x", tmp);
#endif
                S = tmp + 2; // skip ") "
                Assert (S < reinterpret_cast<const char*> (end (data)));
            }

            // MSVC SILLY WARNING ABOUT USING swscanf_s
            // (warning doesn't appear to check if we have mismatch between types and format args provided.
            //      --LGP 2015-01-07
            DISABLE_COMPILER_MSC_WARNING_START (4996)
            [[maybe_unused]] int                ignoredInt{};
            [[maybe_unused]] long               ignoredLong{};
            [[maybe_unused]] unsigned long      ignoredUnsignedLong{};
            [[maybe_unused]] unsigned long long ignoredUnsignedLongLong{};
            [[maybe_unused]] unsigned long int  ignored_unsigned_long{};
            [[maybe_unused]] int                num =
                ::sscanf (S,

                          // (3 - char state)...
                          "%c "

                          // (4 - 'int' - ppid,pgrp,session,tty_nr,tpgid)...
                          "%d %d %d %d %d "

                          // (9 unsigned long  - flags,minflt,cminflt,majflt,cmajflt - NB: flags now unsigned int not unsigned long but unsigned long fits new and old)...
                          "%lu %lu %lu %lu %lu "

                          // (14 - unint but use ulonglong for safety - utime stime)...
                          "%llu %llu "

                          // (16 - unint but use ulonglong for safety- cutime cstime - docs say signed int but thats crazy--LGP2015-09-16)...
                          "%llu %llu "

                          // (18 long priority, nice)...
                          "%ld %ld "

                          // (20  docs say long but thats nuts %ld   num_threads)...
                          "%d "

                          // (21  %ld - itrealvalue)...
                          "%d "

                          // (22 llu -   starttime %llu)...
                          "%llu "

                          // (23 unsigned long by docs but use ull   vsize, rss)...
                          "%llu %llu ",

                          // (3 - char state)...
                          &result.state,

                          // (4 - 'int' - ppid,pgrp,session,tty_nr,tpgid)...
                          &result.ppid, &ignoredInt, &ignoredInt, &ignoredInt, &ignoredInt,

                          // (9 unsigned long - flags,minflt,cminflt,majflt,cmajflt - NB: flags now unsigned int not unsigned long but unsigned long fits new and old)...
                          &ignoredUnsignedLong, &result.minflt, &ignoredUnsignedLong, &result.majflt, &ignoredUnsignedLong,

                          // (14 - unint but use ulonglong for safety - utime stime)...
                          &result.utime, &result.stime,

                          // (16 - unint but use ulonglong for safety- cutime cstime - docs say signed int but thats crazy--LGP2015-09-16)...
                          &ignoredUnsignedLongLong, &ignoredUnsignedLongLong,

                          // (18 long priority, nice)
                          &ignoredLong, &ignoredLong,

                          // (20  docs say long but thats nuts %ld   num_threads)
                          &result.nlwp,

                          // (21  %ld - itrealvalue)
                          &ignoredInt,

                          // (22 llu -   starttime %llu)...
                          &result.start_time,

                          // (23 unsigned long by docs but use ull   vsize, rss)...
                          &result.vsize, &result.rss);
            DISABLE_COMPILER_MSC_WARNING_END (4996) // MSVC SILLY WARNING ABOUT USING swscanf_s

            Assert (num == 22); // if not probably throw away???

#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("result.start_time=%lld", result.start_time);
            DbgTrace ("result.vsize=%ld", result.vsize);
            DbgTrace ("result.rss=%ld", result.rss);
            DbgTrace ("result.utime=%lld", result.utime);
#endif

            return result;
        }
        // https://www.kernel.org/doc/Documentation/filesystems/proc.txt
        // search for 'cat /proc/3828/io'
        struct proc_io_data_ {
            uint64_t read_bytes;
            uint64_t write_bytes;
        };
        static optional<proc_io_data_> Readproc_io_data_ (const filesystem::path& fullPath)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::Readproc_io_data_", "fullPath={}"_f, fullPath};
#endif
            if (not IO::FileSystem::Default ().Access (fullPath)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("Skipping read cuz no access");
#endif
                return nullopt;
            }
            proc_io_data_ result{};
            ifstream      r{fullPath, ios_base::binary | ios_base::in}; // no excption on failed open- just returns false immediately
            while (r) {
                char buf[1024];
                buf[0] = '\0';
                if (r.getline (buf, sizeof (buf))) {
                    constexpr char kReadLbl_[]  = "read_bytes:";
                    constexpr char kWriteLbl_[] = "write_bytes:";
                    if (::strncmp (buf, kReadLbl_, ::strlen (kReadLbl_)) == 0) {
                        result.read_bytes = Characters::CString::String2Int<decltype (result.read_bytes)> (buf + ::strlen (kReadLbl_));
                    }
                    else if (::strncmp (buf, kWriteLbl_, ::strlen (kWriteLbl_)) == 0) {
                        result.write_bytes = Characters::CString::String2Int<decltype (result.write_bytes)> (buf + ::strlen (kWriteLbl_));
                    }
                }
            }
            return result;
        }
        static optional<ProcessType::TCPStats> ReadTCPStats_ (const filesystem::path& fullPath)
        {
            /**
             *  root@q7imx6:/opt/BLKQCL# cat /proc/3431/net/tcp
             *      sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode
             *      0: 00000000:1F90 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 12925 1 e4bc8de0 300 0 0 2 -1
             *      1: 00000000:0050 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 6059 1 e466d720 300 0 0 2 -1
             */
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::ReadTCPStats_", "fullPath={}"_f, fullPath};
#endif

            if (not IO::FileSystem::Default ().Access (fullPath)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"Skipping read cuz no access");
#endif
                return nullopt;
            }
            ProcessType::TCPStats stats;
            bool                  didSkip = false;
            for (const String& i :
                 TextReader::New (IO::FileSystem::FileInputStream::New (fullPath, IO::FileSystem::FileInputStream::eNotSeekable)).ReadLines ()) { // @todo redo using .Skip(1) but crashes --LGP 2016-05-17
                if (not didSkip) {
                    didSkip = true;
                    continue;
                }
                Sequence<String> splits = i.Tokenize ({' '});
                if (splits.size () >= 4) {
                    int st = HexString2Int (splits[3]);
                    /*
                     *  The enum in ./include/net/tcp_states.h is hopefully (apparently) the same as that in netinet/tcp.h
                     */
                    if (st == TCP_ESTABLISHED) {
                        ++stats.fEstablished;
                    }
                    else if (st == TCP_LISTEN) {
                        ++stats.fListening;
                    }
                    else {
                        ++stats.fOther;
                    }
                }
            }
            return stats;
        }
        static optional<MemorySizeType> ReadPrivateBytes_ (const filesystem::path& fullPath)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::ReadPrivateBytes_", "fullPath={}"_f, fullPath};
#endif

            if (not IO::FileSystem::Default ().Access (fullPath)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"Skipping read cuz no access");
#endif
                return nullopt;
            }
            MemorySizeType result{};
            ifstream       r;
            Streams::iostream::OpenInputFileStream (&r, fullPath);
            while (r) {
                char buf[1024];
                buf[0] = '\0';
                if (r.getline (buf, sizeof (buf))) {
                    // I think always in KB
                    constexpr char kPrivate1Lbl_[] = "Private_Clean:";
                    constexpr char kPrivate2Lbl_[] = "Private_Dirty:";
                    // @todo - SHOULD pay attention to the labelm after the number. It may not always be kB? BUt not sure what it can be
                    if (::strncmp (buf, kPrivate1Lbl_, ::strlen (kPrivate1Lbl_)) == 0) {
                        result += Characters::CString::String2Int<MemorySizeType> (buf + strlen (kPrivate1Lbl_)) * 1024;
                    }
                    else if (::strncmp (buf, kPrivate2Lbl_, ::strlen (kPrivate2Lbl_)) == 0) {
                        result += Characters::CString::String2Int<MemorySizeType> (buf + ::strlen (kPrivate2Lbl_)) * 1024;
                    }
                }
            }
            return result;
        }
        // https://www.kernel.org/doc/Documentation/filesystems/proc.txt
        // search for 'cat /proc/PID/status'
        struct proc_status_data_ {
            uid_t ruid;
        };
        static proc_status_data_ Readproc_proc_status_data_ (const filesystem::path& fullPath)
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::Readproc_proc_status_data_",
                                          "fullPath={}"_f, fullPath};
#endif
            proc_status_data_ result{};
            ifstream          r;
            Streams::iostream::OpenInputFileStream (&r, fullPath);
            while (r) {
                char buf[1024];
                buf[0] = '\0';
                if (r.getline (buf, sizeof (buf))) {
                    constexpr char kUidLbl[] = "Uid:";
                    if (::strncmp (buf, kUidLbl, ::strlen (kUidLbl)) == 0) {
                        Assert (::strlen (buf) >= strlen (kUidLbl)); // because istream::getline returns valid C=string, and strncmp assures first 4 bytes match so must be NUL byte later
                        char* S = buf + ::strlen (kUidLbl);
                        Assert (S < std::end (buf));
                        int ruid = ::strtol (S, &S, 10);
                        Assert (S < std::end (buf));
                        [[maybe_unused]] int euid = ::strtol (S, &S, 10);
                        Assert (S < std::end (buf));
                        [[maybe_unused]] int suid = ::strtol (S, &S, 10);
                        Assert (S < std::end (buf));
                        [[maybe_unused]] int fuid = ::strtol (S, &S, 10);
                        Assert (S < std::end (buf));
                        result.ruid = ruid;
                    }
                }
            }
            return result;
        }
        // consider using this as a backup if /procfs/ not present...
        nonvirtual ProcessMapType capture_using_ps_ ()
        {
            Debug::TraceContextBumper ctx{"Stroika::Frameworks::SystemPerformance::Instruments::Process::{}::capture_using_ps_"};
            ProcessMapType            result;
            using Execution::ProcessRunner;
            /*
             *  Thought about STIME but too hard to parse???
             *
             *  EXAMPLE OUTPUT:
             *          $ ps -e -o "pid,ppid,s,time,rss,vsz,user,nlwp,cmd" | head
             *            PID  PPID S     TIME   RSS    VSZ USER     NLWP CMD
             *              1     0 S 00:00:01  3696 116896 root        1 /lib/systemd/systemd --system --deserialize 18
             *              2     0 S 00:00:00     0      0 root        1 [kthreadd]
             *              3     2 S 00:00:00     0      0 root        1 [ksoftirqd/0]
             *              5     2 S 00:00:00     0      0 root        1 [kworker/0:0H]
             *              7     2 S 00:00:08     0      0 root        1 [rcu_sched]
             *              8     2 S 00:00:00     0      0 root        1 [rcu_bh]
             *              9     2 S 00:00:06     0      0 root        1 [rcuos/0]
             *             10     2 S 00:00:00     0      0 root        1 [rcuob/0]
             *             11     2 S 00:00:00     0      0 root        1 [migration/0]
             */
            constexpr size_t                 kVSZ_Idx_{5};
            constexpr size_t                 kUser_Idx_{6};
            constexpr size_t                 kThreadCnt_Idx_{7};
            constexpr size_t                 kColCountIncludingCmd_{9};
            ProcessRunner                    pr{"ps -A -o \"pid,ppid,s,time,rss,vsz,user,nlwp,cmd\""sv};
            Streams::MemoryStream::Ptr<byte> useStdOut = Streams::MemoryStream::New<byte> ();
            pr.SetStdOut (useStdOut);
            pr.Run ();
            String                   out;
            Streams::TextReader::Ptr stdOut        = Streams::TextReader::New (useStdOut);
            bool                     skippedHeader = false;
            size_t                   headerLen     = 0;
            for (String i = stdOut.ReadLine (); not i.empty (); i = stdOut.ReadLine ()) {
                if (not skippedHeader) {
                    skippedHeader = true;
                    headerLen     = i.RTrim ().length ();
                    continue;
                }
                Sequence<String> l = i.Tokenize ();
                if (l.size () < kColCountIncludingCmd_) {
                    DbgTrace ("skipping line cuz len={}"_f, l.size ());
                    continue;
                }
                ProcessType processDetails;
                pid_t       pid                 = Characters::String2Int<int> (l[0].Trim ());
                processDetails.fParentProcessID = Characters::String2Int<int> (l[1].Trim ());
                {
                    String s = l[2].Trim ();
                    if (s.length () == 1) {
                        processDetails.fRunStatus = cvtStatusCharToStatus_ (static_cast<char> (s[0].As<wchar_t> ()));
                    }
                }
                {
                    string tmp     = l[3].AsUTF8<string> ();
                    int    hours   = 0;
                    int    minutes = 0;
                    int    seconds = 0;
                    sscanf (tmp.c_str (), "%d:%d:%d", &hours, &minutes, &seconds);
                    processDetails.fTotalCPUTimeEverUsed = DurationSeconds{hours * 60 * 60 + minutes * 60 + seconds};
                }
                processDetails.fResidentMemorySize = Characters::String2Int<int> (l[4].Trim ()) * 1024; // RSS in /proc/xx/stat is * pagesize but this is *1024
                processDetails.fPrivateVirtualMemorySize = Characters::String2Int<int> (l[kVSZ_Idx_].Trim ()) * 1024;
                processDetails.fUserName                 = l[kUser_Idx_].Trim ();
                processDetails.fThreadCount              = Characters::String2Int<unsigned int> (l[kThreadCnt_Idx_].Trim ());
                String cmdLine;
                {
                    // wrong - must grab EVERYHTING from i past a certain point
                    // Since our first line has headings, its length is our target, minus the 3 chars for CMD
                    const size_t kCmdNameStartsAt_ = headerLen - 3;
                    cmdLine                        = i.size () <= kCmdNameStartsAt_ ? String{} : i.SubString (kCmdNameStartsAt_).RTrim ();
                }
                {
                    processDetails.fKernelProcess = not cmdLine.empty () and cmdLine[0] == '[';
                    // Fake but usable answer
                    Sequence<String> t = cmdLine.Tokenize ();
                    if (not t.empty () and not t[0].empty () and t[0][0] == '/') {
                        processDetails.fEXEPath = IO::FileSystem::ToPath (t[0]);
                    }
                }
                if (_fOptions.fCaptureCommandLine and _fOptions.fCaptureCommandLine (pid, NullCoalesce (processDetails.fEXEPath))) {
                    processDetails.fCommandLine = cmdLine;
                }
                result.Add (pid, processDetails);
            }
            return result;
        }
    };
};
#endif

#if qPlatform_Windows
namespace {
    struct UNICODE_STRING {
        USHORT Length;
        USHORT MaximumLength;
        PWSTR  Buffer;
    };
    struct PROCESS_BASIC_INFORMATION {
        PVOID          Reserved1;
        PVOID /*PPEB*/ PebBaseAddress;
        PVOID          Reserved2[2];
        ULONG_PTR      UniqueProcessId;
        PVOID          Reserved3;
    };
    PVOID GetPebAddress_ (HANDLE ProcessHandle)
    {
        static LONG (WINAPI * NtQueryInformationProcess) (HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation,
                                                          ULONG ProcessInformationLength, PULONG ReturnLength) =
            (LONG (WINAPI*) (HANDLE, ULONG, PVOID, ULONG, PULONG))::GetProcAddress (::LoadLibraryA ("NTDLL.DLL"),
                                                                                    "NtQueryInformationProcess");
        PROCESS_BASIC_INFORMATION pbi{};
        NtQueryInformationProcess (ProcessHandle, 0, &pbi, sizeof (pbi), NULL);
        return pbi.PebBaseAddress;
    }
}
#endif

#if qUseWMICollectionSupport_
namespace {
    const String kProcessID_{"ID Process"sv};
    const String kThreadCount_{"Thread Count"sv};
    const String kIOReadBytesPerSecond_{"IO Read Bytes/sec"sv};
    const String kIOWriteBytesPerSecond_{"IO Write Bytes/sec"sv};
    const String kPercentProcessorTime_{"% Processor Time"sv}; // % Processor Time is the percentage of elapsed time that all of process threads
    // used the processor to execution instructions. An instruction is the basic unit of
    // execution in a computer, a thread is the object that executes instructions, and a
    // process is the object created when a program is run. Code executed to handle some
    // hardware interrupts and trap conditions are included in this count.
    const String kElapsedTime_{"Elapsed Time"sv}; // The total elapsed time, in seconds, that this process has been running.
}
#endif

#if qPlatform_Windows
namespace {
    struct PerfStats_ {
        TimePointSeconds          fCapturedAt;
        optional<DurationSeconds> fTotalCPUTimeEverUsed;
        optional<double>          fCombinedIOReadBytes;
        optional<double>          fCombinedIOWriteBytes;
    };
    struct _Context : ModuleCommonContext_ {
#if qUseWMICollectionSupport_
        WMICollector fProcessWMICollector_{"Process"sv,
                                           {WMICollector::kWildcardInstance},
                                           { kProcessID_,
                                             kThreadCount_,
                                             kIOReadBytesPerSecond_,
                                             kIOWriteBytesPerSecond_,
                                             kPercentProcessorTime_,
                                             kElapsedTime_ }};
#endif
        Mapping<pid_t, PerfStats_> fMap;
    };

    struct InstrumentRep_Windows_ : InstrumentRepBase_<_Context> {

        using InstrumentRepBase_<_Context>::InstrumentRepBase_;

        ProcessMapType _InternalCapture ()
        {
#if qUseWMICollectionSupport_
            processWMICollectorLock               = fProcessWMICollector_.rwget ();
            TimePointSeconds timeOfPrevCollection = processWMICollectorLock.rwref ().GetTimeOfLastCollection ();
            IgnoreExceptionsForCall (processWMICollectorLock.rwref ().Collect ()); // hack cuz no way to copy
            DurationSeconds timeCollecting{processWMICollectorLock.rwref ().GetTimeOfLastCollection () - timeOfPrevCollection};

#if USE_NOISY_TRACE_IN_THIS_MODULE_
            for (const String& i : processWMICollectorLock.rwref ().GetAvailableInstaces ()) {
                DbgTrace (L"WMI instance name %s", i.c_str ());
            }
#endif

            // NOTE THIS IS BUGGY - MUST READ BACK AS INT NOT DOUBLE
            Mapping<pid_t, String> pid2InstanceMap;
            for (const KeyValuePair<String, double>& i : processWMICollectorLock.rwref ().GetCurrentValues (kProcessID_)) {
                pid2InstanceMap.Add (static_cast<int> (i.fValue), i.fKey);
            }
#endif

            SetPrivilegeInContext_ s{SE_DEBUG_NAME, SetPrivilegeInContext_::eIgnoreError};
            ProcessMapType         results;

#if qUseWMICollectionSupport_
            Mapping<String, double> threadCounts_ByPID         = processWMICollectorLock.rwref ().GetCurrentValues (kThreadCount_);
            Mapping<String, double> ioReadBytesPerSecond_ByPID = processWMICollectorLock.rwref ().GetCurrentValues (kIOReadBytesPerSecond_);
            Mapping<String, double> ioWriteBytesPerSecond_ByPID = processWMICollectorLock.rwref ().GetCurrentValues (kIOWriteBytesPerSecond_);
            Mapping<String, double> pctProcessorTime_ByPID = processWMICollectorLock.rwref ().GetCurrentValues (kPercentProcessorTime_);
            Mapping<String, double> processStartAt_ByPID   = processWMICollectorLock.rwref ().GetCurrentValues (kElapsedTime_);
#endif

            TimePointSeconds now{Time::GetTickCount ()};

            Mapping<pid_t, PerfStats_> newContextStats;

#if qUseWinInternalSupport_
            struct AllSysInfo_ {
                AllSysInfo_ ()
                    : fBuf_ (2 * 0x4000) // arbitrary, but empirically seems to work pretty often
                {
                Again:
                    ULONG    returnLength{};
                    NTSTATUS status = ::NtQuerySystemInformation (mSystemProcessInformation, fBuf_.begin (),
                                                                  static_cast<ULONG> (fBuf_.GetSize ()), &returnLength);
                    if (status == STATUS_BUFFER_TOO_SMALL or status == STATUS_INFO_LENGTH_MISMATCH) {
                        fBuf_.GrowToSize (returnLength);
                        goto Again;
                    }
                    if (status != 0) {
                        Throw (Execution::Exception{"Bad result from NtQuerySystemInformation"sv});
                    }
                    fActualNumElts_ = returnLength / sizeof (SYSTEM_PROCESS_INFORMATION);
                }
                Buffer<byte>                      fBuf_;
                const SYSTEM_PROCESS_INFORMATION* GetProcessInfo () const
                {
                    return reinterpret_cast<const SYSTEM_PROCESS_INFORMATION*> (fBuf_.begin ());
                }
                static bool IsValidPID_ (pid_t p)
                {
                    return static_cast<make_signed_t<pid_t>> (p) > 0;
                }
                Set<pid_t> GetAllProcessIDs_ () const
                {
                    const SYSTEM_PROCESS_INFORMATION* start = GetProcessInfo ();
                    const SYSTEM_PROCESS_INFORMATION* end   = start + fActualNumElts_;
                    Set<pid_t>                        result;
                    for (const SYSTEM_PROCESS_INFORMATION* i = start; i < end; ++i) {
                        pid_t pid = reinterpret_cast<pid_t> (i->UniqueProcessId);
                        if (IsValidPID_ (pid)) {
                            result.Add (pid);
                        }
                    }
                    return result;
                }
                Mapping<pid_t, unsigned int> GetThreadCountMap () const
                {
                    if (not fThreadCntMap_.has_value ()) {
                        const SYSTEM_PROCESS_INFORMATION* start = GetProcessInfo ();
                        const SYSTEM_PROCESS_INFORMATION* end   = start + fActualNumElts_;
                        Mapping<pid_t, unsigned int>      tmp;
                        for (const SYSTEM_PROCESS_INFORMATION* i = start; i < end; ++i) {
                            pid_t pid = reinterpret_cast<pid_t> (i->UniqueProcessId);
                            if (IsValidPID_ (pid)) {
                                struct PRIVATE_SYSTEM_PROCESS_INFORMATION_ {
                                    ULONG NextEntryOffset;
                                    ULONG NumberOfThreads; // from ProcessHacker include/ntexapi.h
                                    //...
                                };
                                ULONG threadCount = reinterpret_cast<const PRIVATE_SYSTEM_PROCESS_INFORMATION_*> (i)->NumberOfThreads;
                                tmp.Add (pid, threadCount);
                            }
                        }
                        fThreadCntMap_ = tmp;
                    }
                    return *fThreadCntMap_;
                }
                unsigned int                                   fActualNumElts_;
                mutable optional<Mapping<pid_t, unsigned int>> fThreadCntMap_;
            };
            AllSysInfo_     allSysInfo;
            Iterable<pid_t> allPids = allSysInfo.GetAllProcessIDs_ ();
#else
            Iterable<pid_t> allPids = GetAllProcessIDs_ ();
#endif

#if qUseCreateToolhelp32SnapshotToCountThreads
            ThreadCounter_ threadCounter;
#endif

            for (pid_t pid : allPids) {
                if (_fOptions.fRestrictToPIDs) {
                    if (not _fOptions.fRestrictToPIDs->Contains (pid)) {
                        continue;
                    }
                }
                if (_fOptions.fOmitPIDs) {
                    if (_fOptions.fOmitPIDs->Contains (pid)) {
                        continue;
                    }
                }
                ProcessType processInfo;
                bool        grabStaticData = _fOptions.fCachePolicy == CachePolicy::eIncludeAllRequestedValues or
                                      not _fContext.cget ().cref ()->fStaticSuppressedAgain.Contains (pid);
                {
                    HANDLE hProcess = ::OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
                    if (hProcess != nullptr) {
                        [[maybe_unused]] auto&& cleanup = Execution::Finally ([hProcess] () noexcept { Verify (::CloseHandle (hProcess)); });
                        if (grabStaticData) {
                            optional<String>           processName;
                            optional<filesystem::path> processEXEPath;
                            optional<pid_t>            parentProcessID;
                            optional<String>           cmdLine;
                            optional<String>           userName;
                            LookupProcessPath_ (pid, hProcess, &processName, &processEXEPath, &parentProcessID,
                                                _fOptions.fCaptureCommandLine ? &cmdLine : nullptr, &userName);
                            if (_fOptions.fProcessNameReadPolicy == Options::eAlways or
                                (_fOptions.fProcessNameReadPolicy == Options::eOnlyIfEXENotRead and not processEXEPath.has_value ())) {
                                Memory::CopyToIf (&processInfo.fProcessName, processName);
                            }
                            Memory::CopyToIf (&processInfo.fEXEPath, processEXEPath);
                            Memory::CopyToIf (&processInfo.fParentProcessID, parentProcessID);
                            Memory::CopyToIf (&processInfo.fCommandLine, cmdLine);
                            Memory::CopyToIf (&processInfo.fUserName, userName);
                        }
                        {
                            PROCESS_MEMORY_COUNTERS_EX memInfo{};
                            if (::GetProcessMemoryInfo (hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*> (&memInfo), sizeof (memInfo))) {
                                processInfo.fWorkingSetSize = memInfo.WorkingSetSize;
                                processInfo.fPrivateBytes   = memInfo.PrivateUsage;
                                processInfo.fPageFaultCount = memInfo.PageFaultCount; // docs not 100% clear but I think this is total # pagefaults
                            }
                        }
                        {
                            auto convertFILETIME2DurationSeconds = [] (FILETIME ft) -> Time::DurationSeconds {
                                // From https://msdn.microsoft.com/en-us/library/windows/desktop/ms683223%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
                                // Process kernel mode and user mode times are amounts of time.
                                // For example, if a process has spent one second in kernel mode, this function
                                // will fill the FILETIME structure specified by lpKernelTime with a 64-bit value of ten million.
                                // That is the number of 100-nanosecond units in one second.
                                //
                                // Note - we go through this instead of a cast, since the FILETIME might not be 64-bit aligned
                                ULARGE_INTEGER tmp;
                                tmp.LowPart  = ft.dwLowDateTime;
                                tmp.HighPart = ft.dwHighDateTime;
                                return Time::DurationSeconds{static_cast<TimePointSeconds::rep> (tmp.QuadPart) * 100e-9};
                            };
                            FILETIME creationTime{};
                            FILETIME exitTime{};
                            FILETIME kernelTime{};
                            FILETIME userTime{};
                            if (::GetProcessTimes (hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
                                if (grabStaticData) {

                                    processInfo.fProcessStartedAt = DateTime{creationTime};
                                }
                                processInfo.fTotalCPUTimeEverUsed =
                                    convertFILETIME2DurationSeconds (kernelTime) + convertFILETIME2DurationSeconds (userTime);
                            }
                            else {
                                DbgTrace (L"error calling GetProcessTimes: {}"_f, ::GetLastError ());
                            }
                        }
                        {
                            IO_COUNTERS ioCounters{};
                            if (::GetProcessIoCounters (hProcess, &ioCounters)) {
                                processInfo.fCombinedIOReadBytes  = static_cast<double> (ioCounters.ReadTransferCount);
                                processInfo.fCombinedIOWriteBytes = static_cast<double> (ioCounters.WriteTransferCount);
                            }
                            else {
                                DbgTrace (L"error calling GetProcessIoCounters: {}"_f, ::GetLastError ());
                            }
                        }

#if qUseCreateToolhelp32SnapshotToCountThreads
                        processInfo.fThreadCount = threadCounter.CountThreads (pid);
#endif

#if qUseWinInternalSupport_
                        {
                            if (auto i = allSysInfo.GetThreadCountMap ().Lookup (pid)) {
                                processInfo.fThreadCount = *i;
                            }
                        }
#endif
                    }
                }
#if qUseWMICollectionSupport_
                {
                    String instanceVal = pid2InstanceMap.LookupValue (pid);
                    if (auto o = threadCounts_ByPID.Lookup (instanceVal)) {
                        processInfo.fThreadCount = static_cast<unsigned int> (*o);
                    }
                    if (auto o = ioReadBytesPerSecond_ByPID.Lookup (instanceVal)) {
                        processInfo.fCombinedIOReadRate = *o;
                    }
                    if (auto o = ioWriteBytesPerSecond_ByPID.Lookup (instanceVal)) {
                        processInfo.fCombinedIOWriteRate = *o;
                    }
                    if (auto o = pctProcessorTime_ByPID.Lookup (instanceVal)) {
                        processInfo.fAverageCPUTimeUsed = *o * 100.0;
                    }
                    if (grabStaticData) {
                        if (auto o = processStartAt_ByPID.Lookup (instanceVal)) {
                            processInfo.fProcessStartedAt = DateTime::Now ().AddSeconds (-static_cast<time_t> (*o));
                        }
                    }
                }
#endif
                if (not processInfo.fCombinedIOReadRate.has_value () or not processInfo.fCombinedIOWriteRate.has_value () or
                    not processInfo.fAverageCPUTimeUsed.has_value ()) {
                    if (optional<PerfStats_> p = _fContext.load ()->fMap.Lookup (pid)) {
                        auto diffTime = now - p->fCapturedAt;
                        if (diffTime >= _fOptions.fMinimumAveragingInterval) {
                            if (p->fCombinedIOReadBytes and processInfo.fCombinedIOReadBytes) {
                                processInfo.fCombinedIOReadRate = (*processInfo.fCombinedIOReadBytes - *p->fCombinedIOReadBytes) / diffTime.count ();
                            }
                            if (p->fCombinedIOWriteBytes and processInfo.fCombinedIOWriteBytes) {
                                processInfo.fCombinedIOWriteRate =
                                    (*processInfo.fCombinedIOWriteBytes - *p->fCombinedIOWriteBytes) / diffTime.count ();
                            }
                            if (p->fTotalCPUTimeEverUsed and processInfo.fTotalCPUTimeEverUsed) {
                                processInfo.fAverageCPUTimeUsed =
                                    (*processInfo.fTotalCPUTimeEverUsed - *p->fTotalCPUTimeEverUsed) / diffTime.count ();
                            }
                        }
                    }
#if 0
                    else {
                        /*
                         *  Considered something like:
                         *      if (not processInfo.fCombinedIOReadRate.has_value () and processInfo.fCombinedIOReadBytes.has_value ()) {
                         *          processInfo.fCombinedIOReadRate =  *processInfo.fCombinedIOReadBytes / (now - _GetCaptureContextTime ());
                         *      }
                         *
                         *  But cannot do, because we do capture_() once at CTOR, so if we get here and havent seen this process before
                         *  it started SOMETIME during this capture, but we don't know when (so we don't know the divisor).
                         */
                    }
#endif
                }

                // So next time we can compute 'diffs'
                newContextStats.Add (pid, PerfStats_{now, processInfo.fTotalCPUTimeEverUsed, processInfo.fCombinedIOReadBytes,
                                                     processInfo.fCombinedIOWriteBytes});

                results.Add (pid, processInfo);
            }
            _NoteCompletedCapture (now);
            _fContext.rwget ().rwref ()->fMap = newContextStats;
            if (_fOptions.fCachePolicy == CachePolicy::eOmitUnchangedValues) {
                _fContext.rwget ().rwref ()->fStaticSuppressedAgain = Set<pid_t>{results.Keys ()};
            }
            return results;
        }

    private:
        static Set<pid_t> GetAllProcessIDs_ ()
        {
            DWORD aProcesses[10 * 1024];
            DWORD cbNeeded;

            Set<pid_t> result;
            if (not ::EnumProcesses (aProcesses, sizeof (aProcesses), &cbNeeded)) {
                AssertNotReached ();
                return result;
            }

            // Calculate how many process identifiers were returned.
            DWORD cProcesses = cbNeeded / sizeof (DWORD);
            for (DWORD i = 0; i < cProcesses; ++i) {
                result.Add (aProcesses[i]);
            }
            return result;
        }
        nonvirtual void LookupProcessPath_ (pid_t pid, HANDLE hProcess, optional<String>* processName, optional<filesystem::path>* processEXEPath,
                                            optional<pid_t>* parentProcessID, optional<String>* cmdLine, optional<String>* userName)
        {
            RequireNotNull (hProcess);
            RequireNotNull (processEXEPath);
            RequireNotNull (parentProcessID);
            //CANBENULL (cmdLine);
            RequireNotNull (userName);
            HMODULE hMod{}; // note no need to free handles returned by EnumProcessModules () accorind to man-page for EnumProcessModules
            DWORD   cbNeeded{};
            if (::EnumProcessModules (hProcess, &hMod, sizeof (hMod), &cbNeeded)) {
                TCHAR moduleFullPath[MAX_PATH];
                moduleFullPath[0] = '\0';
                if (::GetModuleFileNameEx (hProcess, hMod, moduleFullPath, static_cast<DWORD> (NEltsOf (moduleFullPath))) != 0) {
                    *processEXEPath = filesystem::path{moduleFullPath};
                }
                if (processName != nullptr) {
                    TCHAR moduleBaseName[MAX_PATH];
                    moduleBaseName[0] = '\0';
                    if (::GetModuleBaseName (hProcess, hMod, moduleBaseName, static_cast<DWORD> (NEltsOf (moduleBaseName))) != 0) {
                        *processName = String::FromSDKString (moduleBaseName);
                    }
                }
            }
            if (cmdLine != nullptr) {
                if (_fOptions.fCaptureCommandLine == nullptr or not _fOptions.fCaptureCommandLine (pid, NullCoalesce (*processEXEPath))) {
                    cmdLine = nullptr;
                }
            }
            {
                const ULONG ProcessBasicInformation = 0;
                static LONG (WINAPI * NtQueryInformationProcess) (HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation,
                                                                  ULONG ProcessInformationLength, PULONG ReturnLength) =
                    (LONG (WINAPI*) (HANDLE, ULONG, PVOID, ULONG, PULONG))::GetProcAddress (::LoadLibraryA ("NTDLL.DLL"),
                                                                                            "NtQueryInformationProcess");
                if (NtQueryInformationProcess) {
                    ULONG_PTR pbi[6];
                    ULONG     ulSize = 0;
                    if (NtQueryInformationProcess (hProcess, ProcessBasicInformation, &pbi, sizeof (pbi), &ulSize) >= 0 && ulSize == sizeof (pbi)) {
                        *parentProcessID = static_cast<pid_t> (pbi[5]);

                        if (cmdLine != nullptr) {
                            // Cribbed from http://windows-config.googlecode.com/svn-history/r59/trunk/doc/cmdline/cmdline.cpp
                            void* pebAddress = GetPebAddress_ (hProcess);
                            if (pebAddress != nullptr) {
#ifdef _WIN64
                                constexpr int kUserProcParamsOffset_ = 0x20;
                                constexpr int kCmdLineOffset_        = 112;
#else
                                constexpr int kUserProcParamsOffset_ = 0x10;
                                constexpr int kCmdLineOffset_        = 0x40;
#endif
                                /* get the address of ProcessParameters */
                                void* rtlUserProcParamsAddress{};
                                if (not ::ReadProcessMemory (hProcess, (PCHAR)pebAddress + kUserProcParamsOffset_,
                                                             &rtlUserProcParamsAddress, sizeof (PVOID), NULL)) {
                                    goto SkipCmdLine_;
                                }
                                UNICODE_STRING commandLine;

                                /* read the CommandLine UNICODE_STRING structure */
                                if (not ::ReadProcessMemory (hProcess, (PCHAR)rtlUserProcParamsAddress + kCmdLineOffset_, &commandLine,
                                                             sizeof (commandLine), NULL)) {
                                    goto SkipCmdLine_;
                                }
                                {
                                    size_t                     strLen = commandLine.Length / sizeof (WCHAR); // length field in bytes
                                    Memory::StackBuffer<WCHAR> commandLineContents{strLen + 1};
                                    /* read the command line */
                                    if (not ::ReadProcessMemory (hProcess, commandLine.Buffer, commandLineContents.begin (), commandLine.Length, NULL)) {
                                        goto SkipCmdLine_;
                                    }
                                    commandLineContents[strLen] = 0;
                                    *cmdLine                    = commandLineContents.begin ();
                                }
                            SkipCmdLine_:;
                            }
                        }
                    }
                }
            }
            {
                /*
                 *  This can fail for a variety of reasons - mostly lack of security access. Capture the data if we can, and just don't
                 *  if we cannot.
                 */
                HANDLE processToken = 0;
                if (::OpenProcessToken (hProcess, TOKEN_QUERY, &processToken) != 0) {
                    [[maybe_unused]] auto&& cleanup =
                        Execution::Finally ([processToken] () noexcept { Verify (::CloseHandle (processToken)); });
                    DWORD nlen{};
                    // no idea why needed, but TOKEN_USER buffer not big enuf empirically - LGP 2015-04-30
                    //      https://msdn.microsoft.com/en-us/library/windows/desktop/aa379626(v=vs.85).aspx
                    //          TokenUser
                    //              The buffer receives a TOKEN_USER structure that contains the user account of the token.
                    byte        tokenUserBuf[1024];
                    TOKEN_USER* tokenUser = reinterpret_cast<TOKEN_USER*> (begin (tokenUserBuf));
                    if (::GetTokenInformation (processToken, TokenUser, tokenUser, sizeof (tokenUserBuf), &nlen) != 0) {
                        Assert (nlen >= sizeof (TOKEN_USER));
                        // @todo not sure we need this IgnoreExceptionsForCall
                        *userName = Execution::Platform::Windows::SID22UserName (tokenUser->User.Sid);
                        //IgnoreExceptionsForCall (*userName = Execution::Platform::Windows::SID22UserName (tokenUser->User.Sid));
                    }
                }
            }
        }
    };
};
#endif

namespace {
    struct ProcessInstrumentRep_
#if qPlatform_Linux
        : InstrumentRep_Linux_
#elif qPlatform_Windows
        : InstrumentRep_Windows_
#else
        : InstrumentRepBase_<ModuleCommonContext_>
#endif
    {
#if qPlatform_Linux
        using inherited = InstrumentRep_Linux_;
#elif qPlatform_Windows
        using inherited = InstrumentRep_Windows_;
#else
        using inherited = InstrumentRepBase_<ModuleCommonContext_>;
#endif
        ProcessInstrumentRep_ (const Options& options, const shared_ptr<_Context>& context = make_shared<_Context> ())
            : inherited{options, context}
        {
            Require (_fOptions.fMinimumAveragingInterval > 0s);
        }
        virtual MeasurementSet Capture () override
        {
            Debug::TraceContextBumper ctx{"SystemPerformance::Instrument...Process...ProcessInstrumentRep_::Capture ()"};
            MeasurementSet            results;
            Measurement               m{Instruments::Process::kProcessMapMeasurement,
                          Process::Instrument::kObjectVariantMapper.FromObject (Capture_Raw (&results.fMeasuredAt))};
            results.fMeasurements.Add (m);
            return results;
        }
        nonvirtual Info Capture_Raw (Range<TimePointSeconds>* outMeasuredAt)
        {
            auto before         = _GetCaptureContextTime ();
            Info rawMeasurement = _InternalCapture ();
            if (outMeasuredAt != nullptr) {
                using Traversal::Openness;
                *outMeasuredAt = Range<TimePointSeconds> (before, _GetCaptureContextTime (), Openness::eClosed, Openness::eClosed);
            }
            return rawMeasurement;
        }
        virtual unique_ptr<IRep> Clone () const override
        {
            return make_unique<ProcessInstrumentRep_> (_fOptions, _fContext.load ());
        }
        ProcessMapType _InternalCapture ()
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{"Instruments::ProcessDetails _InternalCapture"};
#endif
#if qPlatform_Linux or qPlatform_Windows
            return inherited::_InternalCapture ();
#else
            return ProcessMapType{};
#endif
        }
    };
}

/*
 ********************************************************************************
 ********************** Instruments::Process::Instrument ************************
 ********************************************************************************
 */
const ObjectVariantMapper Instruments::Process::Instrument::kObjectVariantMapper = [] () -> ObjectVariantMapper {
    ObjectVariantMapper mapper;
    mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<ProcessType::RunStatus> ());
    mapper.AddCommonType<optional<ProcessType::RunStatus>> ();
    mapper.AddCommonType<optional<pid_t>> ();
    mapper.AddCommonType<optional<MemorySizeType>> ();
    mapper.AddCommonType<optional<Time::Duration>> ();
    mapper.AddCommonType<optional<Mapping<String, String>>> ();
    mapper.AddClass<ProcessType::TCPStats> ({
        {"Established"sv, &ProcessType::TCPStats::fEstablished},
        {"Listening"sv, &ProcessType::TCPStats::fListening},
        {"Other"sv, &ProcessType::TCPStats::fOther},
    });
    mapper.AddCommonType<optional<ProcessType::TCPStats>> ();
    mapper.AddClass<ProcessType> ({
        {"Kernel-Process"sv, &ProcessType::fKernelProcess},
        {"Parent-Process-ID"sv, &ProcessType::fParentProcessID},
        {"Process-Name"sv, &ProcessType::fProcessName},
        {"User-Name"sv, &ProcessType::fUserName},
        {"Command-Line"sv, &ProcessType::fCommandLine},
        {"Current-Working-Directory"sv, &ProcessType::fCurrentWorkingDirectory},
        {"Environment-Variables"sv, &ProcessType::fEnvironmentVariables},
        {"EXE-Path"sv, &ProcessType::fEXEPath},
        {"Root"sv, &ProcessType::fRoot},
        {"Process-Started-At"sv, &ProcessType::fProcessStartedAt},
        {"Run-Status"sv, &ProcessType::fRunStatus},
        {"Private-Virtual-Memory-Size"sv, &ProcessType::fPrivateVirtualMemorySize},
        {"Total-Virtual-Memory-Size"sv, &ProcessType::fTotalVirtualMemorySize},
        {"Resident-Memory-Size"sv, &ProcessType::fResidentMemorySize},
        {"Private-Bytes"sv, &ProcessType::fPrivateBytes},
        {"Page-Fault-Count"sv, &ProcessType::fPageFaultCount},
        {"Major-Page-Fault-Count"sv, &ProcessType::fMajorPageFaultCount},
        {"Working-Set-Size"sv, &ProcessType::fWorkingSetSize},
        {"Private-Working-Set-Size"sv, &ProcessType::fPrivateWorkingSetSize},
        {"Average-CPUTime-Used"sv, &ProcessType::fAverageCPUTimeUsed},
        {"Thread-Count"sv, &ProcessType::fThreadCount},
        {"Combined-IO-Read-Rate"sv, &ProcessType::fCombinedIOReadRate},
        {"Combined-IO-Write-Rate"sv, &ProcessType::fCombinedIOWriteRate},
        {"Combined-IO-Read-Bytes"sv, &ProcessType::fCombinedIOReadBytes},
        {"Combined-IO-Write-Bytes"sv, &ProcessType::fCombinedIOWriteBytes},
        {"TCP-Stats"sv, &ProcessType::fTCPStats},
    });
    mapper.AddCommonType<ProcessMapType> ();
    return mapper;
}();

Instruments::Process::Instrument::Instrument (const Options& options)
    : SystemPerformance::Instrument{InstrumentNameType{"Process"sv},
                                    make_unique<ProcessInstrumentRep_> (options),
                                    {kProcessMapMeasurement},
                                    {KeyValuePair<type_index, MeasurementType>{typeid (Info), kProcessMapMeasurement}},
                                    kObjectVariantMapper}
{
}

/*
 ********************************************************************************
 ********* SystemPerformance::Instrument::CaptureOneMeasurement *****************
 ********************************************************************************
 */
template <>
Instruments::Process::Info SystemPerformance::Instrument::CaptureOneMeasurement (Range<TimePointSeconds>* measurementTimeOut)
{
    Debug::TraceContextBumper ctx{"SystemPerformance::Instrument::CaptureOneMeasurement<Process::Info>"};
    ProcessInstrumentRep_*    myCap = dynamic_cast<ProcessInstrumentRep_*> (fCaptureRep_.get ());
    AssertNotNull (myCap);
    return myCap->Capture_Raw (measurementTimeOut);
}

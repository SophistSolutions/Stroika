/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <sstream>

#if qStroika_Foundation_Common_Platform_POSIX
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/RegularExpression.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Trace.h"
#if qStroika_Foundation_Common_Platform_Windows
#include "Platform/Windows/Exception.h"
#endif
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/FileSystem/FileSystem.h"
#include "Stroika/Foundation/IO/FileSystem/FileUtils.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/TextToBinary.h"

#include "Sleep.h"
#include "Thread.h"

#include "ProcessRunner.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Traversal;

using Debug::TraceContextBumper;
using Memory::MakeSharedPtr;
using Memory::StackBuffer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if USE_NOISY_TRACE_IN_THIS_MODULE_
#include <fstream>
#endif

#if qStroika_Foundation_Common_Platform_POSIX
namespace {
    // no-except cuz the exception will show up in tracelog, and nothing useful to do, and could be quite bad to except cuz mostly used
    // in cleanup, and could cause leaks
    inline void CLOSE_ (int& fd) noexcept
    {
        if (fd >= 0) [[likely]] {
            IgnoreExceptionsForCall (Handle_ErrNoResultInterruption ([fd] () -> int { return ::close (fd); }));
            fd = -1;
        }
    }
}
#endif

#if qStroika_Foundation_Common_Platform_POSIX
namespace {
    static const int kMaxFD_ = [] () -> int {
        int            result{};
        constexpr bool kUseSysConf_ = true;
#if _BSD_SOURCE || _XOPEN_SOURCE >= 500
        [[maybe_unused]] constexpr bool kUseGetDTableSize_ = true;
#else
        [[maybe_unused]] constexpr bool kUseGetDTableSize_ = false;
#endif
        constexpr bool kUseGetRLimit_ = true;
        if constexpr (kUseSysConf_) {
            result = ::sysconf (_SC_OPEN_MAX);
            Assert (result > 20); // from http://man7.org/linux/man-pages/man3/sysconf.3.html - Must not be less than _POSIX_OPEN_MAX (20).
        }
        else if constexpr (kUseSysConf_) {
            result = getdtablesize ();
        }
        else if constexpr (kUseGetRLimit_) {
            struct rlimit fds{};
            if (::getrlimit (RLIMIT_NOFILE, &fds) == 0) {
                return fds.rlim_cur;
            }
            else {
                return 1024; // wag
            }
        }
        /*
         *  A little crazy, but in docker containers, this max# of files can get quite large (I've seen it over 1024*1024).
         *  Probably at that point its smart to use some other technique to close all the extra file descriptors (like look at
         *  lsof() or read /proc/sys/fs/file-nr? Something like that
         *
         *  -- LGP 2018-10-08
         */
        Assert (result > 5);               // sanity check - no real requirement
        Assert (result < 4 * 1024 * 1024); // ""  (if too big, looping to close all costly)
        DbgTrace ("::sysconf (_SC_OPEN_MAX) = {}"_f, result);
        return result;
    }();
}
#endif

#if qStroika_Foundation_Common_Platform_POSIX
namespace {
    pid_t DoFork_ ()
    {
        // we may want to use vfork or some such. But for AIX, it appears best to use f_fork
        //      https://www.ibm.com/support/knowledgecenter/ssw_aix_72/com.ibm.aix.basetrf1/fork.htm
        //  -- LGP 2016-03-31
        return ::fork ();
    }
}
#endif

#if qStroika_Foundation_Common_Platform_POSIX
#include <spawn.h>
namespace {
    //  https://www.ibm.com/support/knowledgecenter/ssw_aix_53/com.ibm.aix.basetechref/doc/basetrf1/posix_spawn.htm%23posix_spawn
    //  http://www.systutorials.com/37124/a-posix_spawn-example-in-c-to-create-child-process-on-linux/

    constexpr bool kUseSpawn_ = false; // 1/2 implemented
}
extern char** environ;
#endif

#if qStroika_Foundation_Common_Platform_Windows
namespace {
    class AutoHANDLE_ {
    public:
        AutoHANDLE_ (HANDLE h = INVALID_HANDLE_VALUE)
            : fHandle{h}
        {
        }
        AutoHANDLE_ (const AutoHANDLE_&) = delete;
        ~AutoHANDLE_ ()
        {
            Close ();
        }
        AutoHANDLE_& operator= (const AutoHANDLE_& rhs)
        {
            if (this != &rhs) {
                Close ();
                fHandle = rhs.fHandle;
            }
            return *this;
        }
        operator HANDLE () const
        {
            return fHandle;
        }
        HANDLE* operator& ()
        {
            return &fHandle;
        }
        void Close ()
        {
            if (fHandle != INVALID_HANDLE_VALUE) {
                Verify (::CloseHandle (fHandle));
                fHandle = INVALID_HANDLE_VALUE;
            }
        }
        void ReplaceHandleAsNonInheritable ()
        {
            HANDLE result = INVALID_HANDLE_VALUE;
            Verify (::DuplicateHandle (::GetCurrentProcess (), fHandle, ::GetCurrentProcess (), &result, 0, FALSE, DUPLICATE_SAME_ACCESS));
            Verify (::CloseHandle (fHandle));
            fHandle = result;
        }

    public:
        HANDLE fHandle;
    };
    inline void SAFE_HANDLE_CLOSER_ (HANDLE* h)
    {
        RequireNotNull (h);
        if (*h != INVALID_HANDLE_VALUE) {
            Verify (::CloseHandle (*h));
            *h = INVALID_HANDLE_VALUE;
        }
    }
}
#endif

namespace {
    template <Common::IAnyOf<char, wchar_t> CHAR_T>
    struct String2ContigArrayCStrs_ {
        StackBuffer<CHAR_T>                       fBytesBuffer;
        StackBuffer<CHAR_T*, 10 * sizeof (void*)> fPtrsBuffer;
        String2ContigArrayCStrs_ (const Mapping<basic_string<CHAR_T>, basic_string<CHAR_T>>& data)
            : String2ContigArrayCStrs_{data.template Map<Iterable<basic_string<CHAR_T>>> (
                  [] (auto kvp) -> basic_string<CHAR_T> { return kvp.fKey + SDKSTR ("=") + kvp.fValue; })}
        {
        }
        String2ContigArrayCStrs_ (const Iterable<basic_string<CHAR_T>>& data)
        {
            StackBuffer<size_t> argsIdx;
            size_t              bufferIndex = 0;
            for (const basic_string<CHAR_T>& i : data) {
                fBytesBuffer.push_back (span{i});
                fBytesBuffer.push_back ('\0');
                argsIdx.push_back (bufferIndex);
                bufferIndex = fBytesBuffer.GetSize ();
            }
            fBytesBuffer.push_back ('\0'); // not sure - maybe not needed for UNIX, but needed on windows (cuz not using double fPtrsBuffer)
            auto freeze = fBytesBuffer.begin ();
            for (size_t i : argsIdx) {
                fPtrsBuffer.push_back (freeze + i);
            }
            fPtrsBuffer.push_back (nullptr);
        }
        String2ContigArrayCStrs_ ()                                = delete;
        String2ContigArrayCStrs_ (const String2ContigArrayCStrs_&) = delete;
        String2ContigArrayCStrs_ (String2ContigArrayCStrs_&&)      = delete;
    };
}

#if qStroika_Foundation_Common_Platform_Windows
namespace {
// still unsure if needed/useful - I now think the PeekNamedPipe stuff is NOT needed, but
// I can turn it on if needed -- LGP 2009-05-07
//#define   qUsePeekNamedPipe_   1
#ifndef qUsePeekNamedPipe_
#define qUsePeekNamedPipe_ 0
#endif
    /*
     *  This code should all work with the smaller buffer sizes, but is more efficient with larger buffers.
     *  Just set to use the smaller buffers to stress test and debug.
     *
     *  There is some subtle but serious bug with my pipe code - and that APPEARS to just be that
     *  WaitForMultipleObjects doesn't work with PIPEs.
     *
     *  I COULD just rewrite a lot of this code to NOT use PIPES - but actual files. That might solve the problem
     *  because they never 'fill up'.
     *
     *  Alternatively - it might be that my switch to ASYNC mode (PIPE_NOWAIT) was a bad idea. Maybe if I got
     *  rid of that - the WAIT code could be made to work? Not sure.
     *
     *  Anyhow - this appears to be adequate for now...
     *
     *          -- LGP 2006-10-17
     */
    constexpr size_t kPipeBufSize_ = 256 * 1024;
    constexpr size_t kReadBufSize_ = 32 * 1024;
}
#endif

/*
 ********************************************************************************
 ***************** Execution::ProcessRunner::Exception **************************
 ********************************************************************************
 */
String ProcessRunner::Exception::mkMsg_ (const String& errorMessage, const optional<String>& stderrSubset,
                                         const optional<ExitStatusType>& wExitStatus, const optional<SignalID>& wTermSig)
{
    StringBuilder sb;
    sb << errorMessage;
    {
        StringBuilder extraMsg;
        if (wExitStatus) {
            extraMsg << "exit status {}"_f(int (*wExitStatus));
        }
        if (wTermSig) {
            if (not extraMsg.empty ()) {
                extraMsg << ", "sv;
            }
            extraMsg << "terminated by signal {}"_f(int (*wTermSig));
        }
        if (not extraMsg.empty ()) {
            sb << ": "sv << extraMsg;
        }
    }
    if (stderrSubset) {
        sb << " (captured stderr: "sv
           << stderrSubset->ReplaceAll ("\\s+"_RegEx, " "sv).LimitLength (100, StringShorteningPreference::ePreferKeepRight) << ")"sv;
    }
    return sb;
}

/*
 ********************************************************************************
 **************** Execution::ProcessRunner::ProcessResultType *******************
 ********************************************************************************
 */
void ProcessRunner::ProcessResultType::ThrowIfFailed ()
{
    if (fExitStatus and *fExitStatus != 0) {
        Throw (Exception{"Child process failed", nullopt, *fExitStatus});
    }
    if (fTerminatedByUncaughtSignalNumber and *fTerminatedByUncaughtSignalNumber != 0) {
        Throw (Exception{"Child process failed", nullopt, nullopt, *fTerminatedByUncaughtSignalNumber});
    }
}

/*
 ********************************************************************************
 **************** Execution::ProcessRunner::BackgroundProcess *******************
 ********************************************************************************
 */
ProcessRunner::BackgroundProcess::BackgroundProcess ()
    : fRep_{MakeSharedPtr<Rep_> ()}
{
}

void ProcessRunner::BackgroundProcess::PropagateIfException () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    Thread::Ptr                                    t{fRep_->fProcessRunner};
    t.ThrowIfDoneWithException ();
    if (auto o = GetProcessResult ()) {
        if (o->fExitStatus and o->fExitStatus != ExitStatusType{}) {
            AssertNotReached (); // I don't think this can happen since it should have resulted in a propagated exception
        }
        if (o->fTerminatedByUncaughtSignalNumber) {
            AssertNotReached (); // I don't think this can happen since it should have resulted in a propagated exception
        }
    }
}

void ProcessRunner::BackgroundProcess::WaitForStarted (Time::DurationSeconds timeout) const
{
    // tmphack impl
    using Time::DurationSeconds;
    DurationSeconds remaining = timeout;
    while (remaining > DurationSeconds{0}) {
        if (auto pr = GetChildProcessID ()) {
            return;
        }
        Sleep (1);
        remaining -= DurationSeconds{1};
    }
}
void ProcessRunner::BackgroundProcess::WaitForDone (Time::DurationSeconds timeout) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    Thread::Ptr                                    t{fRep_->fProcessRunner};
    t.WaitForDone (timeout);
}

void ProcessRunner::BackgroundProcess::Join (Time::DurationSeconds timeout) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    Thread::Ptr                                    t{fRep_->fProcessRunner};
    t.Join (timeout);
    // if he asserts in PropagateIfException () are wrong, I may need to call that here!
}

void ProcessRunner::BackgroundProcess::JoinUntil (Time::TimePointSeconds timeoutAt) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    Thread::Ptr                                    t{fRep_->fProcessRunner};
    t.JoinUntil (timeoutAt);
    // if he asserts in PropagateIfException () are wrong, I may need to call that here!
}

void ProcessRunner::BackgroundProcess::Terminate ()
{
    TraceContextBumper                             ctx{"ProcessRunner::BackgroundProcess::Terminate"};
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    // @todo? set thread to null when done -
    //
    // @todo - Note - UNTESTED, and probably not 100% right (esp error checking!!!
    //
    if (optional<pid_t> o = fRep_->fPID) {
#if qStroika_Foundation_Common_Platform_POSIX
        ::kill (SIGTERM, *o);
#elif qStroika_Foundation_Common_Platform_Windows
        // @todo - if this OpenProcess gives us any trouble, we can return the handle directory from the 'CreateRunnable' where we invoke the process
        HANDLE processHandle = ::OpenProcess (PROCESS_TERMINATE, false, *o);
        if (processHandle != nullptr) {
            ::TerminateProcess (processHandle, 1);
            ::CloseHandle (processHandle);
        }
        else {
            DbgTrace ("::OpenProcess returned null: GetLastError () = {}"_f, GetLastError ());
        }
#else
        AssertNotImplemented ();
#endif
    }
}

/*
 ********************************************************************************
 ************************** Execution::ProcessRunner ****************************
 ********************************************************************************
 */

namespace {
    Mapping<SDKString, SDKString> getEnv_ ()
    {
        return kRawEnvironment ();
    }
    Mapping<SDKString, SDKString> getEnv_ (const Mapping<SDKString, SDKString>& r)
    {
        return r;
    }
    Mapping<SDKString, SDKString> getEnv_ (const Mapping<String, String>& env)
    {
        Mapping<SDKString, SDKString> r;
        for (auto i : env) {
            r.Add (i.fKey.AsSDKString (), i.fValue.AsSDKString ());
        }
        return r;
    }
    Mapping<SDKString, SDKString> getEnv_ (const Sequence<filesystem::path>& replacePath)
    {
        Mapping<SDKString, SDKString> r = getEnv_ ();
#if qStroika_Foundation_Common_Platform_POSIX
        SDKString path = replacePath.Join<SDKString> ([] (const filesystem::path& p) -> SDKString { return p; }, SDKString{":"sv});
#elif qStroika_Foundation_Common_Platform_Windows
        SDKString path = replacePath.Join<SDKString> ([] (const filesystem::path& p) -> SDKString { return p; }, SDKString{L";"sv});
#endif
        r.Add (SDKSTR ("PATH"), path);
        return r;
    }
}

ProcessRunner::ProcessRunner (const String& commandLine, const Options& o)
    : ProcessRunner{commandLine.ContainsAny ({'\'', '\"', '<', '>', '|', '$', '{', '}'}) ? CommandLine{kDefaultShell, commandLine} : CommandLine{commandLine}, o}
{
}

void ProcessRunner::Run (const InputStream::Ptr<byte>& in, const OutputStream::Ptr<byte>& out, const OutputStream::Ptr<byte>& error,
                         ProgressMonitor::Updater progress, Time::DurationSeconds timeout)
{
    TraceContextBumper ctx{"ProcessRunner::Run"};
    auto               activity = LazyEvalActivity ([this] () -> String { return "running '{}'"_f(this->GetCommandLine ()); });
    DeclareActivity    currentActivity{&activity};
    if (timeout == Time::kInfinity) {
        fStdIn_  = in;
        fStdOut_ = out;
        fStdErr_ = error;
        Synchronized<optional<ProcessResultType>> pr;
        CreateRunnable_ (&pr, nullptr, progress) ();
        pr->value_or (ProcessResultType{}).ThrowIfFailed ();
    }
    else {
        // Use 'BackgroundProcess' to get a thread we can interrupt when time is up, for timeout
        BackgroundProcess       bp      = RunInBackground (in, out, error, progress);
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept { bp.Terminate (); });
        bp.Join (timeout);
        bp.PropagateIfException ();
        // If we didn't timeout, then the process must have completed, so we must have a process result
        bp.GetProcessResult ().value_or (ProcessResultType{}).ThrowIfFailed ();
    }
}
void ProcessRunner::Run (optional<ProcessResultType>* processResult, ProgressMonitor::Updater progress, Time::DurationSeconds timeout)
{
    TraceContextBumper ctx{"ProcessRunner::Run"}; //DEPREACTED API.... LOSE
    if (timeout == Time::kInfinity) {
        if (processResult == nullptr) {
            CreateRunnable_ (nullptr, nullptr, progress) ();
        }
        else {
            Synchronized<optional<ProcessResultType>> pr;
            [[maybe_unused]] auto&&                   cleanup = Finally ([&] () noexcept { *processResult = pr.load (); });
            CreateRunnable_ (&pr, nullptr, progress) ();
        }
    }
    else {
        if (processResult == nullptr) {
            Thread::Ptr t = Thread::New (CreateRunnable_ (nullptr, nullptr, progress), Thread::eAutoStart, "ProcessRunner thread"_k);
            t.Join (timeout);
        }
        else {
            Synchronized<optional<ProcessResultType>> pr;
            [[maybe_unused]] auto&&                   cleanup = Finally ([&] () noexcept { *processResult = pr.load (); });
            Thread::Ptr t = Thread::New (CreateRunnable_ (&pr, nullptr, progress), Thread::eAutoStart, "ProcessRunner thread"_k);
            t.Join (timeout);
        }
    }
}

auto ProcessRunner::Run (const String& cmdStdInValue, const StringOptions& stringOpts, ProgressMonitor::Updater progress,
                         Time::DurationSeconds timeout) -> tuple<String, String>
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    MemoryStream::Ptr<byte>                         useStdIn  = MemoryStream::New<byte> ();
    MemoryStream::Ptr<byte>                         useStdOut = MemoryStream::New<byte> ();
    MemoryStream::Ptr<byte>                         useStdErr = MemoryStream::New<byte> ();

    auto mkReadStream = [&] (const InputStream::Ptr<byte>& readFromBinStrm) {
        return stringOpts.fInputCodeCvt ? BinaryToText::Reader::New (readFromBinStrm, *stringOpts.fInputCodeCvt)
                                        : BinaryToText::Reader::New (readFromBinStrm);
    };
    try {
        // Prefill stream
        if (not cmdStdInValue.empty ()) {
            auto outStream = stringOpts.fOutputCodeCvt ? TextToBinary::Writer::New (useStdIn, *stringOpts.fOutputCodeCvt)
                                                       : TextToBinary::Writer::New (useStdIn);
            outStream.Write (cmdStdInValue);
        }
        Assert (useStdIn.GetReadOffset () == 0);

        Run (useStdIn, useStdOut, useStdErr, progress, timeout);

        // get and return results from 'useStdOut' etc
        Assert (useStdOut.GetReadOffset () == 0);
        Assert (useStdErr.GetReadOffset () == 0);
        return make_tuple (mkReadStream (useStdOut).ReadAll (), mkReadStream (useStdErr).ReadAll ());
    }
    catch (const Exception& e) {
        String out = mkReadStream (useStdOut).ReadAll ();
        String err = mkReadStream (useStdErr).ReadAll ();
#if qStroika_Foundation_Debug_DefaultTracingOn
        DbgTrace ("Captured stdout: {}"_f, out);
        DbgTrace ("Captured stderr: {}"_f, err);
#endif
        Throw (Exception{e.fFailureMessage, err, e.fExitStatus, e.fTermSignal});
        Throw (Exception{this->fArgs_.As<String> (), "{}: output: {}, stderr: {}"_f(e.As<String> (), out, err)});
    }
    catch (...) {
        String out = mkReadStream (useStdOut).ReadAll ();
        String err = mkReadStream (useStdErr).ReadAll ();
#if qStroika_Foundation_Debug_DefaultTracingOn
        DbgTrace ("Captured stdout: {}"_f, out);
        DbgTrace ("Captured stderr: {}"_f, err);
#endif
        exception_ptr e = current_exception ();
        Throw (NestedException{"{} (stderr: {})"_f(e, err), e});
    }
}

ProcessRunner::BackgroundProcess ProcessRunner::RunInBackground (const InputStream::Ptr<byte>& in, const OutputStream::Ptr<byte>& out,
                                                                 const OutputStream::Ptr<byte>& error, ProgressMonitor::Updater progress)
{
    TraceContextBumper ctx{"ProcessRunner::RunInBackground"};
    this->fStdIn_  = in;
    this->fStdOut_ = out;
    this->fStdErr_ = error;
    BackgroundProcess result;
    result.fRep_->fProcessRunner =
        Thread::New (CreateRunnable_ (&result.fRep_->fResult, nullptr, progress), Thread::eAutoStart, "ProcessRunner background thread"sv);
    return result;
}

ProcessRunner::BackgroundProcess ProcessRunner::RunInBackground (ProgressMonitor::Updater progress)
{
    TraceContextBumper ctx{"ProcessRunner::RunInBackground"}; // DEPRECATED OVERLOAD
    BackgroundProcess  result;
    result.fRep_->fProcessRunner =
        Thread::New (CreateRunnable_ (&result.fRep_->fResult, nullptr, progress), Thread::eAutoStart, "ProcessRunner background thread"sv);
    return result;
}

#if qStroika_Foundation_Common_Platform_POSIX
namespace {
    // @todo Good Candidate for REWRITE - this is a MESS!
    void Process_Runner_POSIX_ (Synchronized<optional<ProcessRunner::ProcessResultType>>* processResult, Synchronized<optional<pid_t>>* runningPID,
                                ProgressMonitor::Updater progress, [[maybe_unused]] const optional<filesystem::path>& executable,
                                const CommandLine& cmdLine, const ProcessRunner::Options& options, const InputStream::Ptr<byte>& in,
                                const OutputStream::Ptr<byte>& out, const OutputStream::Ptr<byte>& err)
    {
        optional<mode_t>   umask  = options.fChildUMask;
        filesystem::path   useCWD = options.fWorkingDirectory.value_or (IO::FileSystem::WellKnownLocations::GetTemporary ());
        TraceContextBumper ctx{"{}::Process_Runner_POSIX_", Stroika_Foundation_Debug_OptionalizeTraceArgs (
                                                                "...,cmdLine='{}',currentDir='{}',..."_f, cmdLine,
                                                                String{useCWD}.LimitLength (50, StringShorteningPreference::ePreferKeepRight))};

        // track the last few bytes of stderr to include in possible exception messages
        char   trailingStderrBuf[256];
        char*  trailingStderrBufNextByte2WriteAt = begin (trailingStderrBuf);
        size_t trailingStderrBufNWritten{};

        /*
         *  NOTE:
         *      From http://linux.die.net/man/2/pipe
         *          "The array pipefd is used to return two file descriptors referring to the ends
         *          of the pipe. pipefd[0] refers to the read end of the pipe. pipefd[1] refers to
         *          the write end of the pipe"
         */
        int                     jStdin[2]{-1, -1};
        int                     jStdout[2]{-1, -1};
        int                     jStderr[2]{-1, -1};
        [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept {
            ::CLOSE_ (jStdin[0]);
            ::CLOSE_ (jStdin[1]);
            ::CLOSE_ (jStdout[0]);
            ::CLOSE_ (jStdout[1]);
            ::CLOSE_ (jStderr[0]);
            ::CLOSE_ (jStderr[1]);
        });
        if (in) {
            Handle_ErrNoResultInterruption ([&jStdin] () -> int { return ::pipe (jStdin); });
        }
        else {
            jStdin[0] = ::open ("/dev/null", O_RDONLY);
        }
        if (out) {
            Handle_ErrNoResultInterruption ([&jStdout] () -> int { return ::pipe (jStdout); });
        }
        else {
            jStdout[1] = ::open ("/dev/null", O_WRONLY);
        }
        if (err) {
            Handle_ErrNoResultInterruption ([&jStderr] () -> int { return ::pipe (jStderr); });
        }
        else {
            jStderr[1] = ::open ("/dev/null", O_WRONLY);
        }
        // assert cuz code below needs to be more careful if these can overlap 0..2
        Assert (in == nullptr or (jStdin[0] >= 3 and jStdin[1] >= 3));
        Assert (out == nullptr or (jStdout[0] >= 3 and jStdout[1] >= 3));
        Assert (err == nullptr or (jStderr[0] >= 3 and jStderr[1] >= 3));
        DbgTrace ("jStdout[0-CHILD] = {} and jStdout[1-PARENT] = {}"_f, jStdout[0], jStdout[1]);

        /*
         *  Note: Important to do all this code before the fork, because once we fork, we, lose other threads
         *  but share copy of RAM, so they COULD have mutexes locked! And we could deadlock waiting on them, so after
         *  fork, we are VERY limited as to what we can safely do.
         */
        const char* thisEXEPath_cstr = nullptr;
        char**      thisEXECArgv     = nullptr;

        String2ContigArrayCStrs_<char> execDataArgs{
            cmdLine.GetArguments ().Map<Iterable<string>> ([] (auto si) { return si.AsNarrowSDKString (); })};
        thisEXEPath_cstr = execDataArgs.fBytesBuffer.data ();
        thisEXECArgv     = execDataArgs.fPtrsBuffer.data ();

        /*
         *  If the file is not accessible, and using fork/exec, we wont find that out til the execvp,
         *  and then there wont be a good way to propagate the error back to the caller.
         *
         *  @todo for now - this code only checks access for absolute/full path, and we should also check using
         *        PATH and https://linux.die.net/man/3/execvp confstr(_CS_PATH)
         */
        if (not kUseSpawn_ and thisEXEPath_cstr[0] == '/' and ::access (thisEXEPath_cstr, R_OK | X_OK) < 0) {
            errno_t e = errno; // save in case overwritten
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("failed to access exe path so throwing: exe path='{}'"_f, String::FromNarrowSDKString (thisEXEPath_cstr));
#endif
            ThrowPOSIXErrNo (e);
        }

        pid_t childPID{};
        if (kUseSpawn_) {
            posix_spawn_file_actions_t file_actions{};
            /// @see http://stackoverflow.com/questions/13893085/posix-spawnp-and-piping-child-output-to-a-string
            // not quite right - maybe not that close
            /*
             *  move arg stdin/out/err to 0/1/2 file-descriptors. Don't bother with variants that can handle errors/exceptions cuz we cannot really here...
             */
            {
                posix_spawn_file_actions_init (&file_actions);
                posix_spawn_file_actions_addclose (&file_actions, jStdin[0]);
                posix_spawn_file_actions_addclose (&file_actions, jStdin[0]);
                posix_spawn_file_actions_adddup2 (&file_actions, jStdout[1], 1);
                posix_spawn_file_actions_addclose (&file_actions, jStdout[0]);
                posix_spawn_file_actions_adddup2 (&file_actions, jStderr[1], 2);
                posix_spawn_file_actions_addclose (&file_actions, jStderr[1]);
            }
            posix_spawnattr_t* attr   = nullptr;
            int                status = ::posix_spawnp (&childPID, thisEXEPath_cstr, &file_actions, attr, thisEXECArgv, environ);
            if (status != 0) {
                ThrowPOSIXErrNo (status);
            }
        }
        else {
            childPID = DoFork_ ();
            ThrowPOSIXErrNoIfNegative (childPID);
            if (childPID == 0) {
                if (umask) {
                    (void)::umask (*umask);
                }
                try {
                    /*
                     *  In child process. Don't DBGTRACE here, or do anything that could raise an exception. In the child process
                     *  this would be bad...
                     */
                    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-result\"")
                    (void)::chdir (useCWD.c_str ());
                    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-result\"")
                    if (options.fDetached.value_or (false)) {
                        /*
                         *  See http://pubs.opengroup.org/onlinepubs/007904875/functions/setsid.html
                         *  This is similar to setpgrp () but makes doing setpgrp unnecessary.
                         *  This is also similar to setpgid (0, 0) - but makes doing that unneeded.
                         *
                         *  Avoid signals like SIGHUP when the terminal session ends as well as potentially SIGTTIN and SIGTTOU
                         *
                         *  @see http://stackoverflow.com/questions/8777602/why-must-detach-from-tty-when-writing-a-linux-daemon
                         *
                         *  Tried using 
                         *      #if defined _DEFAULT_SOURCE
                         *              daemon (0, 0);
                         *      #endif
                         *      to workaround systemd defaulting to KillMode=control-group
                         */
                        (void)::setsid ();
                    }
                    {
                        /*
                         *  move arg stdin/out/err to 0/1/2 file-descriptors. Don't bother with variants that can handle errors/exceptions cuz we cannot really here...
                         */
                        int useSTDIN  = jStdin[0];
                        int useSTDOUT = jStdout[1];
                        int useSTDERR = jStderr[1];
                        Assert (useSTDIN >= 0 and useSTDOUT >= 0 and useSTDERR >= 0); // parent can have -1 FDs, but child always has legit FDs
                        ::close (0);
                        ::close (1);
                        ::close (2);
                        ::dup2 (useSTDIN, 0);
                        ::dup2 (useSTDOUT, 1);
                        ::dup2 (useSTDERR, 2);
                        ::close (jStdin[0]);
                        ::close (jStdin[1]);
                        ::close (jStdout[0]);
                        ::close (jStdout[1]);
                        ::close (jStderr[0]);
                        ::close (jStderr[1]);
                    }
                    constexpr bool kCloseAllExtraneousFDsInChild_ = true;
                    if (kCloseAllExtraneousFDsInChild_) {
                        // close all but stdin, stdout, and stderr in child fork
                        for (int i = 3; i < kMaxFD_; ++i) {
                            ::close (i);
                        }
                    }
                    [[maybe_unused]] int r = ::execvp (thisEXEPath_cstr, thisEXECArgv);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    {
                        ofstream myfile;
                        myfile.open ("/tmp/Stroika-ProcessRunner-Exec-Failed-Debug-File.txt");
                        myfile << "thisEXEPath_cstr = " << thisEXEPath_cstr << endl;
                        myfile << "r = " << r << " and errno = " << errno << endl;
                    }
#endif
                    ::_exit (EXIT_FAILURE);
                }
                catch (...) {
                    ::_exit (EXIT_FAILURE);
                }
            }
        }
        // we got here, the spawn succeeded, or the fork succeeded, and we are the parent process
        Assert (childPID > 0);
        {
            constexpr size_t kStackBufReadAtATimeSize_ = 10 * 1024;

#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("In Parent Fork: child process PID={}"_f, childPID);
#endif
            if (runningPID != nullptr) {
                runningPID->store (childPID);
            }
            /*
            * WE ARE PARENT
            */
            int& useSTDIN  = jStdin[1];
            int& useSTDOUT = jStdout[0];
            int& useSTDERR = jStderr[0];
            {
                CLOSE_ (jStdin[0]);
                CLOSE_ (jStdout[1]);
                CLOSE_ (jStderr[1]);
            }

            // To incrementally read from stderr and stderr as we write to stdin, we must assure
            // our pipes are non-blocking
            if (useSTDIN != -1) {
                ThrowPOSIXErrNoIfNegative (::fcntl (useSTDIN, F_SETFL, fcntl (useSTDIN, F_GETFL, 0) | O_NONBLOCK));
            }
            if (useSTDOUT != -1) {
                ThrowPOSIXErrNoIfNegative (::fcntl (useSTDOUT, F_SETFL, fcntl (useSTDOUT, F_GETFL, 0) | O_NONBLOCK));
            }
            if (useSTDERR != -1) {
                ThrowPOSIXErrNoIfNegative (::fcntl (useSTDERR, F_SETFL, fcntl (useSTDERR, F_GETFL, 0) | O_NONBLOCK));
            }

            // Throw if any errors except EINTR (which is ignored) or EAGAIN (would block)
            auto readALittleFromProcess = [&] (int fd, const OutputStream::Ptr<byte>& stream, bool write2StdErrCache, bool* eof = nullptr,
                                               bool* maybeMoreData = nullptr) -> void {
                if (fd == -1) {
                    if (maybeMoreData != nullptr) {
                        *maybeMoreData = false;
                    }
                    if (eof != nullptr) {
                        *eof = true;
                    }
                    return;
                }
                uint8_t buf[kStackBufReadAtATimeSize_];
                int     nBytesRead = 0; // int cuz we must allow for errno = EAGAIN error result = -1,
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                int skipedThisMany{};
#endif
                while ((nBytesRead = ::read (fd, buf, sizeof (buf))) > 0) {
                    Assert (nBytesRead <= sizeof (buf));
                    if (stream != nullptr) {
                        stream.Write (span{buf, static_cast<size_t> (nBytesRead)});
                    }
                    if (write2StdErrCache) {
                        for (size_t i = 0; i < nBytesRead; ++i) {
                            Assert (&trailingStderrBuf[0] <= trailingStderrBufNextByte2WriteAt and
                                    trailingStderrBufNextByte2WriteAt < end (trailingStderrBuf));
                            *trailingStderrBufNextByte2WriteAt = buf[i];
                            ++trailingStderrBufNWritten;
                            ++trailingStderrBufNextByte2WriteAt;
                            if (trailingStderrBufNextByte2WriteAt == end (trailingStderrBuf)) {
                                trailingStderrBufNextByte2WriteAt = begin (trailingStderrBuf);
                            }
                            Assert (&trailingStderrBuf[0] <= trailingStderrBufNextByte2WriteAt and
                                    trailingStderrBufNextByte2WriteAt < end (trailingStderrBuf));
                        }
                    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    if (errno == EAGAIN) {
                        // If we get lots of EAGAINS, just skip logging them to avoid spamming the tracelog
                        if (skipedThisMany++ < 100) {
                            continue;
                        }
                        else {
                            DbgTrace ("skipped {} spamming EAGAINs"_f, skipedThisMany);
                            skipedThisMany = 0;
                        }
                    }
                    buf[(nBytesRead == std::size (buf)) ? (std::size (buf) - 1) : nBytesRead] = '\0';
                    DbgTrace ("read from process (fd={}) nBytesRead = {}: {}"_f, fd, nBytesRead,
                              String::FromNarrowSDKString (reinterpret_cast<const char*> (buf)));
#endif
                }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("from (fd={}) nBytesRead = {}, errno={}"_f, fd, nBytesRead, errno);
#endif
                if (nBytesRead < 0) {
                    if (errno != EINTR and errno != EAGAIN) {
                        ThrowPOSIXErrNo (errno);
                    }
                }
                if (eof != nullptr) {
                    *eof = (nBytesRead == 0);
                }
                if (maybeMoreData != nullptr) {
                    *maybeMoreData = (nBytesRead > 0) or (nBytesRead < 0 and errno == EINTR);
                }
            };
            auto readSoNotBlocking = [&] (int fd, const OutputStream::Ptr<byte>& stream, bool write2StdErrCache) {
                bool maybeMoreData = true;
                while (maybeMoreData) {
                    readALittleFromProcess (fd, stream, write2StdErrCache, nullptr, &maybeMoreData);
                }
            };
            auto readTilEOF = [&] (int fd, const OutputStream::Ptr<byte>& stream, bool write2StdErrCache) {
                if (fd == -1) {
                    return;
                }
                WaitForIOReady waiter{fd};
                bool           eof = false;
                while (not eof) {
                    (void)waiter.WaitQuietly (1s);
                    readALittleFromProcess (fd, stream, write2StdErrCache, &eof);
                }
            };

            if (in != nullptr) {
                byte stdinBuf[kStackBufReadAtATimeSize_];
                // read to 'in' til it reaches EOF (returns 0). But don't fully block, cuz we want to at least trickle in the stdout/stderr data
                // even if no input is ready to send to child.
                while (true) {
                    if (optional<span<byte>> bytesReadFromStdIn = in.ReadNonBlocking (span{stdinBuf})) {
                        Assert (bytesReadFromStdIn->size () <= std::size (stdinBuf));
                        if (bytesReadFromStdIn->empty ()) {
                            break;
                        }
                        else {
                            const byte* e = bytesReadFromStdIn->data () + bytesReadFromStdIn->size ();
                            for (const byte* i = bytesReadFromStdIn->data (); i != e;) {
                                // read stuff from stdout, stderr while pushing to stdin, so that we don't get the PIPE buf too full
                                readSoNotBlocking (useSTDOUT, out, false);
                                readSoNotBlocking (useSTDERR, err, true);
                                int bytesWritten = ThrowPOSIXErrNoIfNegative (Handle_ErrNoResultInterruption ([useSTDIN, i, e] () {
                                    int tmp = ::write (useSTDIN, i, e - i);
                                    // NOTE: https://linux.die.net/man/2/write appears to indicate on pipe full, write could return 0, or < 0 with errno = EAGAIN, or EWOULDBLOCK
                                    if (tmp < 0 and (errno == EAGAIN or errno == EWOULDBLOCK)) {
                                        tmp = 0;
                                    }
                                    return tmp;
                                }));
                                Assert (bytesWritten >= 0);
                                Assert (bytesWritten <= (e - i));
                                i += bytesWritten;
                                if (bytesWritten == 0) {
                                    // don't busy wait, but not clear how long to wait? Maybe should only sleep if readSoNotBlocking above returns no change
                                    //
                                    // OK - this is clearly wrong - @see http://stroika-bugs.sophists.com/browse/STK-589 - Fix performance of ProcessRunner - use select / poll instead of sleep when write to pipe returns 0
                                    //
                                    Sleep (1ms);
                                }
                            }
                        }
                    }
                    else {
                        // nothing on input stream, so pull from stdout, stderr, and wait a little to avoid busy-waiting
                        readSoNotBlocking (useSTDOUT, out, false);
                        readSoNotBlocking (useSTDERR, err, true);
                        Sleep (100ms);
                    }
                }
            }
            // in case child process reads from its STDIN to EOF
            CLOSE_ (useSTDIN);

            readTilEOF (useSTDOUT, out, false);
            readTilEOF (useSTDERR, err, true);

            // not sure we need?
            int status = 0;
            int flags  = 0; // FOR NOW - HACK - but really must handle sig-interruptions...
                            //  Wait for child
            int result =
                Handle_ErrNoResultInterruption ([childPID, &status, flags] () -> int { return ::waitpid (childPID, &status, flags); });
            // throw / warn if result other than child exited normally
            if (processResult != nullptr) {
                // not sure what it means if result != childPID??? - I think cannot happen cuz we pass in childPID, less result=-1
                processResult->store (ProcessRunner::ProcessResultType{WIFEXITED (status) ? WEXITSTATUS (status) : optional<int> (),
                                                                       WIFSIGNALED (status) ? WTERMSIG (status) : optional<int> ()});
            }
            else if (result != childPID or not WIFEXITED (status) or WEXITSTATUS (status) != 0) {
                // @todo fix this message
                DbgTrace ("childPID={}, result={}, status={}, WIFEXITED={}, WEXITSTATUS={}, WIFSIGNALED={}"_f, static_cast<int> (childPID),
                          result, status, WIFEXITED (status), WEXITSTATUS (status), WIFSIGNALED (status));
                if (processResult == nullptr) {
                    StringBuilder stderrMsg;
                    if (trailingStderrBufNWritten > std::size (trailingStderrBuf)) {
                        stderrMsg << "..."sv;
                        stderrMsg << String::FromLatin1 (Memory::ConstSpan (span{trailingStderrBufNextByte2WriteAt, end (trailingStderrBuf)}));
                    }
                    stderrMsg << String::FromLatin1 (Memory::ConstSpan (span{begin (trailingStderrBuf), trailingStderrBufNextByte2WriteAt}));
                    Throw (ProcessRunner::Exception{"Spawned program"sv, stderrMsg.str (),
                                                    WIFEXITED (status) ? WEXITSTATUS (status) : optional<uint8_t>{},
                                                    WIFSIGNALED (status) ? WTERMSIG (status) : optional<uint8_t>{}});
                }
            }
        }
    }
}
#endif

#if qStroika_Foundation_Common_Platform_Windows
namespace {
    void Process_Runner_Windows_ (Synchronized<optional<ProcessRunner::ProcessResultType>>* processResult,
                                  Synchronized<optional<pid_t>>* runningPID, [[maybe_unused]] ProgressMonitor::Updater progress,
                                  const optional<filesystem::path>& executable, const CommandLine& cmdLine, const ProcessRunner::Options& options,
                                  const InputStream::Ptr<byte>& in, const OutputStream::Ptr<byte>& out, const OutputStream::Ptr<byte>& err)
    {
        filesystem::path   useCWD = options.fWorkingDirectory.value_or (IO::FileSystem::WellKnownLocations::GetTemporary ());
        TraceContextBumper ctx{"{}::Process_Runner_Windows_", Stroika_Foundation_Debug_OptionalizeTraceArgs (
                                                                  "...,cmdLine='{}',currentDir={},..."_f, cmdLine,
                                                                  String{useCWD}.LimitLength (50, StringShorteningPreference::ePreferKeepRight))};

        /*
         *  o   Build directory into which we can copy the JAR file plugin,
         *  o   create STDIN/STDOUT file handles to send/grab results
         *  o   Run the process, waiting for it to finish.
         *  o   Grab results from STDOUT file.
         *  o   Cleanup created directory.
         */

        // use AutoHANDLE so these are automatically closed at the end of the procedure, whether it ends normally or via
        // exception.
        AutoHANDLE_ jStdin[2]{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};
        AutoHANDLE_ jStdout[2]{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};
        AutoHANDLE_ jStderr[2]{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE};

        PROCESS_INFORMATION processInfo{};
        processInfo.hProcess = INVALID_HANDLE_VALUE;
        processInfo.hThread  = INVALID_HANDLE_VALUE;

        try {
            {
                SECURITY_DESCRIPTOR sd{};
                Verify (::InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION));
                Verify (::SetSecurityDescriptorDacl (&sd, true, 0, false));
                SECURITY_ATTRIBUTES sa = {sizeof (SECURITY_ATTRIBUTES), &sd, true};
                if (in) {
                    Verify (::CreatePipe (&jStdin[1], &jStdin[0], &sa, kPipeBufSize_));
                }
                if (out) {
                    Verify (::CreatePipe (&jStdout[1], &jStdout[0], &sa, kPipeBufSize_));
                }
                if (err) {
                    Verify (::CreatePipe (&jStderr[1], &jStderr[0], &sa, kPipeBufSize_));
                }
                /*
                 *  Make sure the ends of the pipe WE hang onto are not inheritable, because otherwise the READ
                 *  wont return EOF (until the last one is closed).
                 */
                if (in) {
                    jStdin[0].ReplaceHandleAsNonInheritable ();
                }
                if (out) {
                    jStdout[1].ReplaceHandleAsNonInheritable ();
                }
                if (err) {
                    jStderr[1].ReplaceHandleAsNonInheritable ();
                }
            }

            STARTUPINFO startInfo{};
            startInfo.cb         = sizeof (startInfo);
            startInfo.hStdInput  = jStdin[1];
            startInfo.hStdOutput = jStdout[0];
            startInfo.hStdError  = jStderr[0];
            startInfo.dwFlags |= STARTF_USESTDHANDLES;

            DWORD createProcFlags{NORMAL_PRIORITY_CLASS};
            if (options.fCreateNoWindow) {
                createProcFlags |= CREATE_NO_WINDOW;
            }
            else if (options.fDetached.value_or (false)) {
                // DETACHED_PROCESS ignored if CREATE_NO_WINDOW
                createProcFlags |= DETACHED_PROCESS;
            }

            {
                // UNCLEAR; visual studio system() impl uses true; docs not clear
                // BUT - when I use false I get "unknown file: error: C++ exception with description "Spawned program 'echo hi mom' failed: error: 1" thrown in the test body." for some tests
                bool bInheritHandles = true;

                TCHAR cmdLineBuf[32768]; // crazy MSFT definition! - why this should need to be non-const!
                Characters::CString::Copy (cmdLineBuf, std::size (cmdLineBuf), cmdLine.As<String> ().AsSDKString ().c_str ());

                optional<filesystem::path> useEXEPath = executable;

                // WARN if EXE not in path...
#if qStroika_Foundation_Debug_AssertionsChecked
                if (useEXEPath) {
                    if (!FindExecutableInPath (*useEXEPath)) {
                        DbgTrace ("Warning: Cannot find exe '{}' in PATH ({})"_f, useEXEPath, kPath ());
                    }
                }
                else {
                    // not sure we want to do this? - since first thing could be magic interpreted by shell, like set
                    auto cmdArgs = cmdLine.GetArguments ();
                    if (cmdArgs.size () >= 1) {
                        filesystem::path exe2Find = cmdArgs[0].As<filesystem::path> ();
                        if (!FindExecutableInPath (exe2Find)) {
                            DbgTrace ("Warning: Cannot find exe '{}' in PATH ({})"_f, exe2Find, kPath ());
                        }
                    }
                }
#endif

                unique_ptr<String2ContigArrayCStrs_<SDKChar>> envBuffer;
                LPVOID                                        lpEnvironment = nullptr;
                if (options.fEnvironment) {
                    if (auto oep = get_if<Sequence<filesystem::path>> (&options.fEnvironment.value ())) {
                        envBuffer = make_unique<String2ContigArrayCStrs_<SDKChar>> (getEnv_ (*oep));
                    }
                    else if (auto om = get_if<Mapping<String, String>> (&options.fEnvironment.value ())) {
                        envBuffer = make_unique<String2ContigArrayCStrs_<SDKChar>> (getEnv_ (*om));
                    }
                    else if (auto oms = get_if<Mapping<SDKString, SDKString>> (&options.fEnvironment.value ())) {
                        envBuffer = make_unique<String2ContigArrayCStrs_<SDKChar>> (getEnv_ (*oms));
                    }
                    AssertNotNull (envBuffer);
                    lpEnvironment = envBuffer->fBytesBuffer.data (); // need to adjust createProcFlags for type used...
                    if constexpr (same_as<SDKChar, wchar_t>) {
                        createProcFlags |= CREATE_UNICODE_ENVIRONMENT;
                    }
                }

                // see https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
                // for complex rules for interpreting nullptr in appname (first) arg, and cmdLineBuf... But mostly - the idea - is
                // it runs the search path algorithm and tries to do the right thing
                Execution::Platform::Windows::ThrowIfZeroGetLastError (
                    ::CreateProcess (useEXEPath == nullopt ? nullptr : useEXEPath->c_str (), cmdLineBuf, nullptr, nullptr, bInheritHandles,
                                     createProcFlags, lpEnvironment, useCWD.c_str (), &startInfo, &processInfo));
            }

            if (runningPID != nullptr) {
                runningPID->store (processInfo.dwProcessId);
            }

            {
                /*
                 * Remove our copy of the stdin/stdout/stderr which belong to the child (so EOF will work properly).
                 */
                jStdin[1].Close ();
                jStdout[0].Close ();
                jStderr[0].Close ();
            }

            AutoHANDLE_& useSTDIN = jStdin[0];
            Assert (jStdin[1] == INVALID_HANDLE_VALUE);
            AutoHANDLE_& useSTDOUT = jStdout[1];
            Assert (jStdout[0] == INVALID_HANDLE_VALUE);
            AutoHANDLE_& useSTDERR = jStderr[1];
            Assert (jStderr[0] == INVALID_HANDLE_VALUE);

            constexpr size_t kStackBufReadAtATimeSize_ = 10 * 1024;

            auto readAnyAvailableAndCopy2StreamWithoutBlocking = [] (HANDLE p, const OutputStream::Ptr<byte>& o) {
                if (p == INVALID_HANDLE_VALUE) {
                    return;
                }
                byte buf[kReadBufSize_];
#if qUsePeekNamedPipe_
                DWORD nBytesAvail{};
#endif
                DWORD nBytesRead{};
                // Read normally blocks, we don't want to because we may need to write more before it can output
                // and we may need to timeout
                while (
#if qUsePeekNamedPipe_
                    ::PeekNamedPipe (p, nullptr, nullptr, nullptr, &nBytesAvail, nullptr) and nBytesAvail != 0 and
#endif
                    ::ReadFile (p, buf, sizeof (buf), &nBytesRead, nullptr) and nBytesRead > 0) {
                    if (o != nullptr) {
                        o.Write (span{buf, nBytesRead});
                    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    buf[(nBytesRead == std::size (buf)) ? (std::size (buf) - 1) : nBytesRead] = byte{'\0'};
                    DbgTrace ("read from process (fd={}) nBytesRead = {}: {}"_f, p, nBytesRead, buf);
#endif
                }
            };

            if (processInfo.hProcess != INVALID_HANDLE_VALUE) {
                {
                    {
                        /*
                         * Set the pipe endpoints to non-blocking mode.
                         */
                        auto mkPipeNoWait_ = [] (HANDLE ioHandle) -> void {
                            if (ioHandle != INVALID_HANDLE_VALUE) {
                                DWORD mode = 0;
                                Verify (::GetNamedPipeHandleState (ioHandle, &mode, nullptr, nullptr, nullptr, nullptr, 0));
                                mode |= PIPE_NOWAIT;
                                Verify (::SetNamedPipeHandleState (ioHandle, &mode, nullptr, nullptr));
                            }
                        };
                        mkPipeNoWait_ (useSTDIN);
                        mkPipeNoWait_ (useSTDOUT);
                        mkPipeNoWait_ (useSTDERR);
                    }

                    /*
                     *  Fill child-process' stdin with the source document.
                     */
                    if (in != nullptr) {
                        byte stdinBuf[kStackBufReadAtATimeSize_];
                        // blocking read to 'in' til it reaches EOF (returns 0)
                        while (size_t nbytes = in.ReadBlocking (span{stdinBuf}).size ()) {
                            Assert (nbytes <= std::size (stdinBuf));
                            const byte* p = begin (stdinBuf);
                            const byte* e = p + nbytes;
                            while (p < e) {
                                DWORD written = 0;
                                if (::WriteFile (useSTDIN, p, Math::PinToMaxForType<DWORD> (e - p), &written, nullptr) == 0) {
                                    DWORD lastErr = ::GetLastError ();
                                    // sometimes we fail because the target process hasn't read enough and the pipe is full.
                                    // Unfortunately - MSFT doesn't seem to have a single clear error message nor any clear
                                    // documentation about what WriteFile () returns in this case... So there maybe other errors
                                    // that are innocuous that may cause is to prematurely terminate our 'RunExternalProcess'.
                                    //      -- LGP 2009-05-07
                                    if (lastErr != ERROR_SUCCESS and lastErr != ERROR_NO_MORE_FILES and lastErr != ERROR_PIPE_BUSY and
                                        lastErr != ERROR_NO_DATA) {
                                        DbgTrace ("in RunExternalProcess_ - throwing {} while fill in stdin"_f, lastErr);
                                        ThrowSystemErrNo (lastErr);
                                    }
                                }
                                Assert (written <= static_cast<size_t> (e - p));
                                p += written;
                                // in case we are failing to write to the stdIn because of blocked output on an outgoing pipe
                                if (p < e) {
                                    readAnyAvailableAndCopy2StreamWithoutBlocking (useSTDOUT, out);
                                    readAnyAvailableAndCopy2StreamWithoutBlocking (useSTDERR, err);
                                }
                                if (p < e and written == 0) {
                                    // if we have more to write, but that the target process hasn't consumed it yet - don't spin trying to
                                    // send it data - back off a little
                                    Sleep (100ms);
                                }
#if 0
                                    // Do timeout handling at a higher level
                                    if (Time::GetTickCount () > timeoutAt) {
                                        DbgTrace (_T ("process timed out (writing initial data) - so throwing up!"));
                                        // then we've timed out - kill the process and DON'T return the partial result!
                                        (void)::TerminateProcess (processInfo.hProcess, -1);    // if it exceeded the timeout - kill it (could already be done by now - in which case - this will be ignored - fine...
                                        Throw (Execution::Platform::Windows::Exception (ERROR_TIMEOUT));
                                    }
#endif
                            }
                        }
                    }

                    // in case invoked sub-process is reading, and waiting for EOF before processing...
                    useSTDIN.Close ();
                }

                /*
                 *  Must keep reading while waiting - in case the child emits so much information that it
                 *  fills the OS PIPE buffer.
                 */
                int timesWaited = 0;
                while (true) {
                    /*
                     *  It would be nice to be able to WAIT on the PIPEs - but that doesn't appear to work when they
                     *  are in ASYNCRONOUS mode.
                     *
                     *  So - instead - just wait a very short period, and then retry polling the pipes for more data.
                     *          -- LGP 2006-10-17
                     */
                    HANDLE events[1] = {processInfo.hProcess};

                    // We don't want to busy wait too much, but if its fast (with java, that's rare ;-)) don't want to wait
                    // too long needlessly...
                    //
                    // Also - its not exactly a busy-wait. Its just a wait between reading stuff to avoid buffers filling. If the
                    // process actually finishes, it will change state and the wait should return immediately.
                    double remainingTimeout = (timesWaited <= 5) ? 0.1 : 0.5;
                    DWORD  waitResult       = ::WaitForMultipleObjects (static_cast<DWORD> (std::size (events)), events, false,
                                                                        static_cast<int> (remainingTimeout * 1000));
                    ++timesWaited;

                    readAnyAvailableAndCopy2StreamWithoutBlocking (useSTDOUT, out);
                    readAnyAvailableAndCopy2StreamWithoutBlocking (useSTDERR, err);
                    switch (waitResult) {
                        case WAIT_OBJECT_0: {
                            DbgTrace ("external process finished (DONE)"_f);
                            //                              timeoutAt = -1.0f;  // force out of loop
                            goto DoneWithProcess;
                        } break;
                        case WAIT_TIMEOUT: {
                            DbgTrace ("still waiting for external process output (WAIT_TIMEOUT)"_f);
                        }
                    }
                }

            DoneWithProcess:
                DWORD processExitCode{};
                Verify (::GetExitCodeProcess (processInfo.hProcess, &processExitCode));

                SAFE_HANDLE_CLOSER_ (&processInfo.hProcess);
                SAFE_HANDLE_CLOSER_ (&processInfo.hThread);

                readAnyAvailableAndCopy2StreamWithoutBlocking (useSTDOUT, out);
                readAnyAvailableAndCopy2StreamWithoutBlocking (useSTDERR, err);

                if (processResult == nullptr) {
                    if (processExitCode != 0) {
                        Throw (ProcessRunner::Exception{"Child process failed"sv, nullopt, processExitCode});
                    }
                }
                else {
                    processResult->store (ProcessRunner::ProcessResultType{static_cast<int> (processExitCode)});
                }
            }
        }
        catch (...) {
            // sadly and confusingly, CreateProcess() appears to set processInfo.hProcess and processInfo.hThread to nullptr - at least on some failures
            if (processInfo.hProcess != INVALID_HANDLE_VALUE and processInfo.hProcess != nullptr) {
                (void)::TerminateProcess (processInfo.hProcess, static_cast<UINT> (-1)); // if it exceeded the timeout - kill it
                SAFE_HANDLE_CLOSER_ (&processInfo.hProcess);
                SAFE_HANDLE_CLOSER_ (&processInfo.hThread);
            }
            ReThrow ();
        }
    }
}
#endif

function<void ()> ProcessRunner::CreateRunnable_ (Synchronized<optional<ProcessResultType>>* processResult,
                                                  Synchronized<optional<pid_t>>* runningPID, ProgressMonitor::Updater progress)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"ProcessRunner::CreateRunnable_"};
#endif
    AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
    return [processResult, runningPID, progress, exe = this->fExecutable_, cmdLine = this->fArgs_, options = fOptions_, in = fStdIn_,
            out = fStdOut_, err = fStdErr_] () {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContextBumper ctx{"ProcessRunner::CreateRunnable_::{}::Runner..."};
#endif
        auto            activity = LazyEvalActivity{[&] () { return "executing '{}'"_f(cmdLine); }};
        DeclareActivity currentActivity{&activity};
#if qStroika_Foundation_Common_Platform_POSIX
        Process_Runner_POSIX_ (processResult, runningPID, progress, exe, cmdLine, options, in, out, err);
#elif qStroika_Foundation_Common_Platform_Windows
        Process_Runner_Windows_ (processResult, runningPID, progress, exe, cmdLine, options, in, out, err);
#endif
    };
}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProcessRunner_h_
#define _Stroika_Foundation_Execution_ProcessRunner_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Containers/Sequence.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Memory/BLOB.h"
#include "../Streams/InputStream.h"
#include "../Streams/OutputStream.h"

#include "Process.h"
#include "ProgressMonitor.h"

/**
 *  TODO:
 *      @todo   Cleanup ProcessRunner::CreateRunnable_ () to use more Finally {} based cleanup, instead
 *              of more redundant try/catch style.
 *
 *      @todo   Redo POSIX impl using vfork () or http://linux.die.net/man/3/posix_spawn
 *
 *      @todo   Need EXCEPTION TYPE that includes PROCESS_STATUS and throw that instead of current exception
 *              On failure.
 *
 *      @todo   Windows implementation is weak, but appears fully functional.
 *
 *      @todo   Fix POSIX version to use vfork() instead of fork () - but carefully! Must setup data just so.
 *
 *      @todo   Fix POSIX version to properly handle reading and writing streams at the same time to avoid deadlock
 *              in finite kernel buffer sizes.
 *
 *      @todo   Fix POSIX version to use pipe2 and close appropriate open file descriptors (and other 'clean invoke' stuff.
 *
 *      @todo   Redo DWORD   waitResult  =   ::WaitForMultipleObjects()... logic to wait on thread and each read/write socket
 *              with select() AND somehow maybe eventually wait on streams (so we don't have to pre-read it all)
 *
 *      @todo   logic for THREADs and for PROGRESS support are NOT thought through, and just a rough first stab
 *
 *      @todo   Make sure it handles well without blocking
 *              (tricks I had todo in HF - forcing extra reads so writes woudlnt block).
 *              Currently structured to work off a single runnable, which implies works off a single thread. That implies
 *              it must use select() - probably a good idea anyhow - on each socket used for operations (windows and POSIX).
 *
 *              So data pusher/buffer loop does select on external streams to see if data availble.
 *
 *              This implies I must also be able to do the moral equivalent of selects on my BinaryInput/Output streams? Maybe,
 *              unless I do all the buffering... But at least for the stdin stream - I need to be able to check if/whjen there
 *              is new data available!!! TRICKY
 *
 *      @todo   Decide on/document semantics if there is a change in setting STDIN/SETDOUT etc while a runner exists?
 *              If error - always defectable?
 *
 *              And related - what if we create a runner, and then destroyu the object? How to assure runner fully
 *              destroyed? Blocking/waiting or error or detachted state?
 *
 *      @todo   Add optional hook to be run (at least for posix) inside the FORKED process, before the exec.
 *              Can be used to reset signals, and/or close file descriptors. Maybe have optional
 *              flag to auto-do this stuff and or have a preset value hook proc do do most standard things.
 *
 *  Design Goals:
 *      o   Be able to run simple processes and capture output with little overhead, and very easy to do
 *
 *      o   Be able to support pipes between processes
 *
 *      o   Support large data and blocking issues properly - automating avoidance of pipe full bugs
 *          which block processes
 *
 *      o   Efficient/Low performance overhead
 *
 *      o   For POSIX - simple to cleanly cleanup open sockets/resources (not needed on windows)
 *
 *      o   Separate threading implementation from API, so easy to externally specify the thread
 *          stuff  runs on (e.g. so you can use threadpools to run the processes).
 *
 *      o   Work with stroika streams so its easy to have user-defined producers and comsumers, and
 *          easy to hook together TextStreams (wrappers) - for format conversion/piping.
 *
 *
 *  \em Design Overview
 *      o
 *
 */

namespace Stroika::Foundation::Execution {

    using std::byte;

    using Characters::String;

    /**
     *  \brief Synchronously run the given command, and optionally support stdin/stdout/stderr as streams
     *
     *  Synchronously run the given command.
     *
     *  \note   ProcessRunner searches the PATH for the given executable: it need not be a full or even relative to
     *          cwd path.
     *
     *

        // IDEA HERE IS FROM KDJ - Do something like python/perl stuff for managing subprocesses easily.

    // Look input stream, output stream(or streams - stdout/stderr) - and some kind of extenral process control
    // so can say WIAT or Termiante.
    //
    // Simple portable wrapper.
    //
    // Could use simple singly threaded approach used in TypeNValue ReportDefinition::RunExternalProcess_ (const SDKString& cmdLine, const SDKString& currentDir, const BLOBs::BLOB& stdinBLOB, const ContentType& resultFormat, float timeout)
    // except that code has the defect that when the input pipe is full, and there is nothing in the output piples
    // it busy waits. We COULD fix this by doing a select.
    //
    // OR - as KDJ suggests - create 3 threads - one that just reads on stdout, one that just reads on stderr, and one that
    // spits into stdin.
    //
    // The caller of 'subprocess' then would just wait on each of the 3 subprocesses (or would implement the aforementioned
    // looping over reads/writes/selects etc).
    //
        *
        *  \par Example Usage
        *      \code
        *          String name = Execution::ProcessRunner (L"uname").Run (String ()).Trim ();
        *      \endcode
        *
        *      \code
        *          Memory::BLOB kData_{ Memory::BLOB::Raw ("this is a test")  };
        *          Streams::MemoryStream<byte>::Ptr processStdIn = Streams::MemoryStream<byte> { kData_ };
        *          Streams::MemoryStream<byte>::Ptr processStdOut = Streams::MemoryStream<byte> {};
        *          ProcessRunner               pr (L"cat", processStdIn, processStdOut);
        *          pr.Run ();
        *          VerifyTestResult (processStdOut.ReadAll () == kData_);
        *      \endcode
        *
        */
    class ProcessRunner : private Debug::AssertExternallySynchronizedLock {
    public:
        ProcessRunner ()                     = delete;
        ProcessRunner (const ProcessRunner&) = delete;

    public:
        ProcessRunner (const String& commandLine, const Streams::InputStream<byte>::Ptr& in = nullptr, const Streams::OutputStream<byte>::Ptr& out = nullptr, const Streams::OutputStream<byte>::Ptr& error = nullptr);
        ProcessRunner (const String& executable, const Containers::Sequence<String>& args, const Streams::InputStream<byte>::Ptr& in = nullptr, const Streams::OutputStream<byte>::Ptr& out = nullptr, const Streams::OutputStream<byte>::Ptr& error = nullptr);
#if !qTargetPlatformSDKUseswchar_t && 0
        ProcessRunner (const SDKString& commandLine, const Streams::InputStream<byte>::Ptr& in = nullptr, const Streams::OutputStream<byte>::Ptr& out = nullptr, const Streams::OutputStream<byte>::Ptr& error = nullptr)
            : ProcessRunner (String::FromSDKString (commandLine), in, out, error)
        {
        }
        ProcessRunner (const SDKString& executable, const Containers::Sequence<SDKString>& args, const Streams::InputStream<byte>::Ptr& in = nullptr, const Streams::OutputStream<byte>::Ptr& out = nullptr, const Streams::OutputStream<byte>::Ptr& error = nullptr)
            : ProcessRunner (String::FromSDKString (executable), args, in, out, error)
        {
        }
#endif

    public:
        nonvirtual ProcessRunner& operator= (const ProcessRunner&) = delete;

    public:
        class Exception;

    public:
        /**
         * defaults to 'missing'. If missing, then the OS default for new directory is used on created process (usually same as parent process)
         */
        nonvirtual optional<String> GetWorkingDirectory ();
        nonvirtual void             SetWorkingDirectory (const optional<String>& d);

    public:
        /**
         *  If empty, stdin will not be empty (redirected from /dev/null).
         *
         *  Otherwise, the stream will be 'read' by the ProcessRunner and 'fed' downstream to
         *  the running subprocess.
         */
        nonvirtual Streams::InputStream<byte>::Ptr GetStdIn () const;
        nonvirtual void                            SetStdIn (const Streams::InputStream<byte>::Ptr& in);
        nonvirtual void                            SetStdIn (const Memory::BLOB& in);

    public:
        /**
         *  If empty, stdout will not be captured (redirected to /dev/null)
         */
        nonvirtual Streams::OutputStream<byte>::Ptr GetStdOut () const;
        nonvirtual void                             SetStdOut (const Streams::OutputStream<byte>::Ptr& out);

    public:
        /**
         *  If empty, stderr will not be captured (redirected to /dev/null)
         */
        nonvirtual Streams::OutputStream<byte>::Ptr GetStdErr () const;
        nonvirtual void                             SetStdErr (const Streams::OutputStream<byte>::Ptr& err);

    public:
        /**
         *  Zero means success. Run() returns optional<ProcessResultType> by reference, and that
         *  value is only provided if the child process exited. If exited, we return the exit
         *  status and signal number (if any) - see waitpid - http://pubs.opengroup.org/onlinepubs/9699919799/functions/wait.html
         */
        struct ProcessResultType {
            optional<int> fExitStatus;
            optional<int> fTerminatedByUncaughtSignalNumber;
        };

    public:
        /**
         *  \brief Run the given external command/process (set by constructor) - with the given arguments, and block until that completes and return the results
         *
         *  Run the given external command/process (set by constructor) - with the given arguments, and block until
         *  that completes and return the results.
         *
         *  The Run() overload taking cmdStdInValue replaces the current stdin stream associated with the
         *  ProcessRunner, and replaces the stdout, and replaces its stdout stream with one that captures
         *  results as a string.
         *
         *  Each overload that takes a 'processResult' argument recieves the numeric value the process
         *  exited with (if it called exit - that is - didnt terminate by signal etc). However, if that
         *  parameter is missing (nullptr) - Run () wll throw an exception if the called process returns
         *  non-zero.
         *
         *  \note Exceptions:
         *        A number of issues before the process is run will generate an exception.
         *        If the argument processResult is null, failure (non SUCCESS exit or signal termination) will trigger an exception, and otherwise the
         *        parameter *processResult will be filled in.
         *
         *  @see RunInBackground
         */
        nonvirtual void Run (optional<ProcessResultType>* processResult = nullptr, ProgressMonitor::Updater progress = nullptr, Time::DurationSecondsType timeout = Time::kInfinite);
        nonvirtual Characters::String Run (const Characters::String& cmdStdInValue, optional<ProcessResultType>* processResult = nullptr, ProgressMonitor::Updater progress = nullptr, Time::DurationSecondsType timeout = Time::kInfinite);

    public:
        class BackgroundProcess;

    public:
        /**
         *  \brief Run the given external command/process (set by constructor) - with the given arguments in the background, and return a handle to the results.
         *
         *  @see Run
         */
        nonvirtual BackgroundProcess RunInBackground (ProgressMonitor::Updater progress = nullptr);

    private:
        /**
         *  Note that 'in' will be sent to the stdin of the subprocess, 'out' will be read from the
         *  stdout of the subprocess and error will be read from the stderr of the subprocess.
         *
         *  Each of these CAN be null, and will if so, that will be interpretted as an empty stream
         *  (for in/stdin), and for out/error, just means the results will be redirected to /dev/null.
         *
         *      \note not sure why this was ever public - so switched to private 2016-02-03 - Stk v2.0a126
         */
        nonvirtual function<void()> CreateRunnable_ (Synchronized<optional<ProcessResultType>>* processResult, Synchronized<optional<pid_t>>* runningPID, ProgressMonitor::Updater progress);

    private:
        nonvirtual String GetEffectiveCmdLine_ () const;

    private:
        optional<String>                 fCommandLine_;
        optional<String>                 fExecutable_;
        Containers::Sequence<String>     fArgs_; // ignored if fExecutable empty
        optional<String>                 fWorkingDirectory_;
        Streams::InputStream<byte>::Ptr  fStdIn_;
        Streams::OutputStream<byte>::Ptr fStdOut_;
        Streams::OutputStream<byte>::Ptr fStdErr_;
    };

    /**
     */
    class ProcessRunner::Exception : public StringException {
    private:
        using inherited = StringException;

    public:
        /**
         */
#if qPlatform_POSIX
        Exception (const String& cmdLine, const String& errorMessage, const optional<String>& stderrSubset = {}, const optional<uint8_t>& wExitStatus = optional<uint8_t>{}, const optional<uint8_t>& wTermSig = optional<uint8_t>{}, const optional<uint8_t>& wStopSig = optional<uint8_t>{});
#elif qPlatform_Windows
        Exception (const String& cmdLine, const String& errorMessage, const optional<String>& stderrSubset = {}, const optional<DWORD>& err = optional<DWORD>{});
#endif
    private:
#if qPlatform_POSIX
        static String mkMsg_ (const String& cmdLine, const String& errorMessage, const optional<String>& stderrSubset, const optional<uint8_t>& wExitStatus, const optional<uint8_t>& wTermSig, const optional<uint8_t>& wStopSig);
#elif qPlatform_Windows
        static String   mkMsg_ (const String& cmdLine, const String& errorMessage, const optional<String>& stderrSubset, const optional<DWORD>& err);
#endif
    private:
        String fCmdLine_;
        String fErrorMessage_;
#if qPlatform_POSIX
        optional<uint8_t> fWExitStatus_;
        optional<uint8_t> fWTermSig_;
        optional<uint8_t> fWStopSig_;
#elif qPlatform_Windows
        optional<DWORD> fErr_;
#endif
    };

    /**
     *      @todo warning: https://stroika.atlassian.net/browse/STK-585 - lots broken here
     */
    class ProcessRunner::BackgroundProcess : private Debug::AssertExternallySynchronizedLock {
    private:
        BackgroundProcess ();

    public:
        BackgroundProcess (const BackgroundProcess&) = default;

    public:
        /**
         * Return missing if process still running, and if completed, return the results.
         */
        nonvirtual optional<ProcessResultType> GetProcessResult () const;

    public:
        /**
         *
         *  @see Join ()
         *  @see JoinUntil ()
         */
        nonvirtual void WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite) const;

    public:
        /**
         *  \brief Join () does WaitForDone () and throw exception if there was any error (see PropagateIfException).
         *
         *  \note   Aliases - this used to be called WaitForDoneAndPropagateErrors; but used the name Join () to mimic the name used with Threads - NOT
         *          because thats used in the implementation, but because its essentially logically the same thing.
         *
         *  @see JoinUntil ()
         *  @see WaitForDone ()
         */
        nonvirtual void Join (Time::DurationSecondsType timeout = Time::kInfinite) const;

    public:
        /**
         *  \brief WaitForDoneUntil () and throw exception if there was any error (see PropagateIfException).
         *
         *  @see Join ()
         *  @see WaitForDone ()
         */
        nonvirtual void JoinUntil (Time::DurationSecondsType timeoutAt) const;

    public:
        [[deprecated ("Use Join instead - as of v2.1d16")]] void WaitForDoneAndPropagateErrors (Time::DurationSecondsType timeout = Time::kInfinite) const
        {
            Join (timeout);
        }

    public:
        /**
         *  If the process has completed with an error, throw exception reflecting that failure.
         *
         *  \note if the process has not completed, this likely does nothing.
         */
        nonvirtual void PropagateIfException () const;

    public:
        /**
         *   If the process is still running, terminate it.
         */
        nonvirtual void Terminate ();

    private:
        struct Rep_ {
            virtual ~Rep_ () = default;
            Thread::CleanupPtr                        fProcessRunner{Thread::CleanupPtr::eAbortBeforeWaiting};
            Synchronized<optional<pid_t>>             fPID{};
            Synchronized<optional<ProcessResultType>> fResult{};
        };
        shared_ptr<Rep_> fRep_;

    private:
        friend class ProcessRunner;
    };

    /**
     *  Setup stdin/out/error to refer to devnull (or closed), and then run the given process. This throws
     *  exceptions on failure to run and returns the created process id, but doesn't wait to monitor
     *  process progress.
     *
     *  For the commmandLine overload, it is TBD how the commandLine will be decoded (@todo) - maybe
     *  parse it here systematically, or maybe leave it to OS to do (sh or windows CreateProcess API).
     *
     *  For the executable/args overload, the first member of args will be assumed to be the application
     *  name (argv[0] - which CAN differ from the path to the executable). If this is omitted or the empty
     *  string, it will be generated automatically.
     *
     *  \note   On linux, this also detaches from the terminal driver, to avoid spurious SIGHUP
     *          and SIGTTIN and SIGTTOU
     *
     *  \note   DetachedProcessRunner searches the PATH for the given executable: it need not be a full or even relative to
     *          cwd path.
     */
    pid_t DetachedProcessRunner (const String& commandLine);
    pid_t DetachedProcessRunner (const String& executable, const Containers::Sequence<String>& args);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ProcessRunner.inl"

#endif /*_Stroika_Foundation_Execution_ProcessRunner_h_*/

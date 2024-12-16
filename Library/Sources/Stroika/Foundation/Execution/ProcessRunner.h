/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProcessRunner_h_
#define _Stroika_Foundation_Execution_ProcessRunner_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>
#include <optional>

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/InputStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"

#include "Process.h"
#include "ProgressMonitor.h"

/**
 *  TODO:
 *      @todo   After we lose DEPRECATED APIS (STREAM STUFF) - Run and RunInBackground can become const methods
 * 
 *      @todo   Cleanup ProcessRunner::CreateRunnable_ () to use more Finally {} based cleanup, instead
 *              of more redundant try/catch style.
 *
 *      @todo   Redo POSIX impl using vfork () or http://linux.die.net/man/3/posix_spawn
 *
 *      @todo   Need EXCEPTION TYPE that includes PROCESS_STATUS and throw that instead of current exception
 *              On failure.
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
 *              (tricks I had todo in HF - forcing extra reads so writes wouldn't block).
 *              Currently structured to work off a single runnable, which implies works off a single thread. That implies
 *              it must use select() - probably a good idea anyhow - on each socket used for operations (windows and POSIX).
 *
 *              So data pusher/buffer loop does select on external streams to see if data available.
 *
 *              This implies I must also be able to do the moral equivalent of selects on my BinaryInput/Output streams? Maybe,
 *              unless I do all the buffering... But at least for the stdin stream - I need to be able to check if/when there
 *              is new data available!!! TRICKY
 *
 *      @todo   Decide on/document semantics if there is a change in setting STDIN/SETDOUT etc while a runner exists?
 *              If error - always detectable?
 *
 *              And related - what if we create a runner, and then destroy the object? How to assure runner fully
 *              destroyed? Blocking/waiting or error or detached state?
 *
 *      @todo   Add optional hook to be run (at least for POSIX) inside the FORKED process, before the exec.
 *              Can be used to reset signals, and/or close file descriptors. Maybe have optional
 *              flag to auto-do this stuff and or have a preset value hook proc do do most standard things.
 *
 *  Design Goals:
 *      o   Be able to run simple processes and capture output with little overhead, and very easy to do
 *          (like perl backticks).
 *
 *      o   Be able to support pipes between processes (either within the shell, or between Stroika threads)
 *
 *      o   Support large data and blocking issues properly - automating avoidance of pipe full bugs
 *          which block processes
 *
 *      o   Efficient/Low performance overhead
 *
 *      o   For POSIX - simple to cleanly cleanup open sockets/resources (not needed on windows)
 *
 *      o   Separate threading implementation from API, so easy to externally specify the thread
 *          stuff  runs on (e.g. so you can use thread pools to run the processes).
 *
 *      o   Work with stroika streams so its easy to have user-defined producers and consumers, and
 *          easy to hook together TextStreams (wrappers) - for format conversion/piping.
 *
 *  \em Design Overview
 *      o
 *
 */

namespace Stroika::Foundation::Execution {

    using Characters::String;

    /**
     *  \brief Run the given command, and optionally support stdin/stdout/stderr as streams (either sync with Run, RunInBackground)
     *
     *  \note   ProcessRunner searches the PATH for the given executable: it need not be a full or even relative to
     *          cwd path.
     *
     *  \note   Historical Note:
     *          IDEA HERE IS FROM KDJ - Do something like python/perl stuff for managing subprocesses easily.
     *
     *          Look input stream, output stream(or streams - stdout/stderr) - and some kind of external process control
     *          so can say WIAT or Terminate.
     *
     *          Simple portable wrapper.
     *
     *          Could use simple singly threaded approach used in TypeNValue ReportDefinition::RunExternalProcess_ (const SDKString& cmdLine, const SDKString& currentDir, const BLOBs::BLOB& stdinBLOB, const ContentType& resultFormat, float timeout)
     *          except that code has the defect that when the input pipe is full, and there is nothing in the output pipes
     *          it busy waits. We COULD fix this by doing a select.
     *
     *          OR - as KDJ suggests - create 3 threads - one that just reads on stdout, one that just reads on stderr, and one that
     *          spits into stdin.
     *
     *          The caller of 'subprocess' then would just wait on each of the 3 subprocesses (or would implement the aforementioned
     *          looping over reads/writes/selects etc).
     *
     *  \par Example Usage
     *      \code
     *          String name = get<0> (ProcessRunner{"uname"}.Run (String {})).Trim ();
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          ProcessRunner pr{"echo hi mom"};
     *          auto [stdOutStr, stdErrStr] = pr.Run ("");
     *          EXPECT_EQ (stdOutStr.Trim (), "hi mom");
     *          EXPECT_EQ (stdErrStr, "");
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          ProcessRunner                    pr{"cat"};
     *          Memory::BLOB                     kData_{ Memory::BLOB::FromRaw ("this is a test")  };
     *          Streams::MemoryStream::Ptr<byte> processStdIn = Streams::MemoryStream::New<byte> (kData_ );
     *          Streams::MemoryStream::Ptr<byte> processStdOut = Streams::MemoryStream::New<byte> ();
     *          pr.Run (processStdIn, processStdOut).ThrowIfFailed ();
     *          EXPECT_EQ (processStdOut.ReadAll (), kData_);
     *      \endcode
     *
     */
    class ProcessRunner {
    public:
        static constexpr CommandLine::WrapInShell kDefaultShell =
#if qStroika_Foundation_Common_Platform_Windows
            CommandLine::WrapInShell::eWindowsCMD
#else
            CommandLine::WrapInShell::eBash
#endif
            ;

    public:
        /**
         */
        struct Options {
            /**
             *  \brief pwd/cwd of the created process
             *      defaults to 'missing'. If missing, then the OS default for new directory is used on created process (usually same as parent process)
             */
            optional<filesystem::path> fWorkingDirectory;

            /**
             *  If true, then any nullptr input / output pipes are replaced with /dev/null (or equivalent)
             *  And any 'terminal' associated with the calling process is eliminated from the child
             *  process.
             * 
             *  Case POSIX:
             *      This also detaches from the terminal driver, to avoid spurious SIGHUP
             *      and SIGTTIN and SIGTTOU (setsid);
             * 
             *  Case Windoze:
             *      From: https://learn.microsoft.com/en-us/windows/win32/procthread/process-creation-flags
             *      DETACHED_PROCESS - the new process does not inherit its parent's console
             *      This flag is ignored if the application is not a console application, or if it is used with either CREATE_NEW_CONSOLE or DETACHED_PROCESS
             * 
             *  \note replaces the Stroika v2.1 DetachedProcessRunner API
             */
            optional<bool> fDetached;

#if qStroika_Foundation_Common_Platform_POSIX
            /**
             *  \brief set umask of child process
             * 
             *  mostly harmless, not clearly needed, but suggested in http://codingfreak.blogspot.com/2012/03/daemon-izing-process-in-linux.html
             */
            optional<mode_t> fChildUMask{027};
#endif

#if qStroika_Foundation_Common_Platform_Windows
            /**
             *  From: https://learn.microsoft.com/en-us/windows/win32/procthread/process-creation-flags
             *  CONSOLE handle from this app not passed to child process. Obviates fDetachConsole.
             */
            bool fCreateNoWindow : 1 {true};

#endif
        };

    public:
        /**
         * \brief Construct ProcessRunner with a CommandLine to run (doesn't actually RUN til you call Run or RunInBackground).
         * 
         *  \note overload with executable allows specifying an alternate executable to run, even though args[0] will be what is reported
         *        to that application (a somewhat common trick in unix-land).
         * 
         *  \note overload with String commandLine:
         *        Simple commands are run directly, and strings with apparent shell-isms, like pipes and quotes etc, are run through kDefaultShell.
         *        This overload is handy, but easy to explicitly control shell used with CommandLine argument instead.
         */
        ProcessRunner ()                     = delete;
        ProcessRunner (const ProcessRunner&) = delete;

#if qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy
        ProcessRunner (const filesystem::path& executable, const CommandLine& args);
        ProcessRunner (const CommandLine& args);
        ProcessRunner (const String& commandLine);
        ProcessRunner (const filesystem::path& executable, const CommandLine& args, const Options& o);
        ProcessRunner (const CommandLine& args, const Options& o);
        ProcessRunner (const String& commandLine, const Options& o);
#else
        ProcessRunner (const filesystem::path& executable, const CommandLine& args, const Options& o = {});
        ProcessRunner (const CommandLine& args, const Options& o = {});
        ProcessRunner (const String& commandLine, const Options& o = {});
#endif

    public:
        nonvirtual ProcessRunner& operator= (const ProcessRunner&) = delete;

    public:
        class Exception;

    public:
        /**
         */
        nonvirtual CommandLine GetCommandLine () const;
        nonvirtual void        SetCommandLine (const CommandLine& args);

    public:
        /**
         */
        nonvirtual Options GetOptions () const;
        nonvirtual void    SetOptions (const Options& o);

    public:
        /**
         *  Zero means success. Run() returns optional<ProcessResultType> by reference, and that
         *  value is only provided if the child process exited. If exited, we return the exit
         *  status and signal number (if any) - see waitpid - http://pubs.opengroup.org/onlinepubs/9699919799/functions/wait.html
         */
        struct [[nodiscard]] ProcessResultType {
            optional<int> fExitStatus;
            optional<int> fTerminatedByUncaughtSignalNumber;

            void ThrowIfFailed ();
        };

    public:
        /**
         *  \brief Run () options for mapping Strings - what code page converters to use.
         * 
         *      \note defaults now are UTF-8, but don't count on this if you care about the encoding used (subject to change).
         */
        struct StringOptions {
            /**
             *  Input refers to the input of the sub-process being run. So this conversion is applied before sending the data to
             *  that process.
             */
            optional<Characters::CodeCvt<>> fInputCodeCvt;

            /**
             *  Output refers to the output of the sub-process being run. So this conversion is applied to the data retrieved
             *  from that process.
             */
            optional<Characters::CodeCvt<>> fOutputCodeCvt;
        };

    public:
        /**
         *  \brief Run the given external command/process (set by constructor) - with the given arguments, and block until that completes and return the results
         *
         *  Run the given external command/process (set by constructor) - with the given arguments, and block until
         *  that completes and return the results.
         *
         *  Run STREAMS overload:
         *      This overload takes input/output/error binary streams
         * 
         *      STDIN/STDOUT/STDERR:
         *          *  If nullptr/not specified, will redirected to /dev/null
         * 
         *  Run STRING overload:
         *      This is the simplest API. Just pass in a string, and get back a string (first one is stdout, second is stderr).
         * 
         *      The cmdStdInValue is passed as stdin (stream) to the subprocess.
         * 
         *  BOTH overloads will throw if there is any sort of error, including error exit from the process called.
         *  Use RunInBackground () to examine the results of the sub-process.
         * 
         *  \note Exceptions:
         *        A number of issues before the process is run will generate an exception.
         *        If the argument processResult is null, failure (non SUCCESS exit or signal termination) will trigger an exception, and otherwise the
         *        parameter *processResult will be filled in.
         * 
         *  \note if this is called with a timeout, and it times out, the child is killed immediately upon timeout.
         *        To avoid this behavior, use RunInBackground
         *
         *  \par Example Usage (using strings in/out)
         *      \code
         *          String name = get<0> (ProcessRunner{"uname"}.Run (String {})).Trim ();
         *      \endcode
         *
         *  \par Example Usage (using binary streams)
         *      \code
         *          ProcessRunner                    pr{"cat"};
         *          Memory::BLOB                     kData_{ Memory::BLOB::FromRaw ("this is a test") };
         *          Streams::MemoryStream::Ptr<byte> processStdIn = Streams::MemoryStream::New<byte> (kData_);
         *          Streams::MemoryStream::Ptr<byte> processStdOut = Streams::MemoryStream::New<byte> ();
         *          pr.Run (processStdIn, processStdOut);
         *          EXPECT_EQ (processStdOut.ReadAll (), kData_);
         *      \endcode
         *
         *  @see RunInBackground
         */
        nonvirtual void Run (const Streams::InputStream::Ptr<byte>& in = nullptr, const Streams::OutputStream::Ptr<byte>& out = nullptr,
                             const Streams::OutputStream::Ptr<byte>& error = nullptr, ProgressMonitor::Updater progress = nullptr,
                             Time::DurationSeconds timeout = Time::kInfinity);
        nonvirtual tuple<Characters::String, Characters::String> Run (const Characters::String& cmdStdInValue,
                                                                      const StringOptions& stringOpts = {}, ProgressMonitor::Updater progress = nullptr,
                                                                      Time::DurationSeconds timeout = Time::kInfinity);

    public:
        class BackgroundProcess;

    public:
        /**
         *  \brief Run the given external command/process (set by constructor) - with the given arguments in the background,
         *         and return a handle to the results.
         * 
         *  This function is generally quick, and non-blocking - just creates a thread todo the work.
         *
         *  \note it is perfectly legal to launch a subprocess, and not track it in any way, just ignoring (not saving)
         *        the BackgroundProcess object.
         *
         *  @see Run
         */
        nonvirtual BackgroundProcess RunInBackground (const Streams::InputStream::Ptr<byte>&  in       = nullptr,
                                                      const Streams::OutputStream::Ptr<byte>& out      = nullptr,
                                                      const Streams::OutputStream::Ptr<byte>& error    = nullptr,
                                                      ProgressMonitor::Updater                progress = nullptr);

    private:
        /**
         *  Note that 'in' will be sent to the stdin of the subprocess, 'out' will be read from the
         *  stdout of the subprocess and error will be read from the stderr of the subprocess.
         *
         *  Each of these CAN be null, and will if so, that will be interpreted as an empty stream
         *  (for in/stdin), and for out/error, just means the results will be redirected to /dev/null.
         */
        nonvirtual function<void ()> CreateRunnable_ (Synchronized<optional<ProcessResultType>>* processResult,
                                                      Synchronized<optional<pid_t>>* runningPID, ProgressMonitor::Updater progress);

    private:
        optional<filesystem::path>                                     fExecutable_; // if omitted, derived from fArgs[0]
        CommandLine                                                    fArgs_;
        Options                                                        fOptions_;
        Streams::InputStream::Ptr<byte>                                fStdIn_;  // just while we support deprecated API
        Streams::OutputStream::Ptr<byte>                               fStdOut_; // ""
        Streams::OutputStream::Ptr<byte>                               fStdErr_; // ""
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

    public:
        [[deprecated ("Since Stroika v3.0d12 - pass stdin/stdout/stderr to ProcessRunner Run() method (if needed)")]] ProcessRunner (
            const filesystem::path& executable, const CommandLine& args, const Streams::InputStream::Ptr<byte>& in,
            const Streams::OutputStream::Ptr<byte>& out = nullptr, const Streams::OutputStream::Ptr<byte>& error = nullptr);
        [[deprecated ("Since Stroika v3.0d12 - pass stdin/stdout/stderr to ProcessRunner Run() method (if needed)")]] ProcessRunner (
            const CommandLine& args, const Streams::InputStream::Ptr<byte>& in, const Streams::OutputStream::Ptr<byte>& out = nullptr,
            const Streams::OutputStream::Ptr<byte>& error = nullptr);
        [[deprecated ("Since Stroika v3.0d12 - pass stdin/stdout/stderr to ProcessRunner Run() method (if needed)")]] ProcessRunner (
            const String& commandLine, const Streams::InputStream::Ptr<byte>& in, const Streams::OutputStream::Ptr<byte>& out = nullptr,
            const Streams::OutputStream::Ptr<byte>& error = nullptr)
            : ProcessRunner{commandLine}
        {
            this->fStdIn_  = in;
            this->fStdOut_ = out;
            this->fStdErr_ = error;
        }

        [[deprecated ("Since Stroika v3.0d12 - use other overloads for ProcessRunner")]] ProcessRunner (
            const filesystem::path& executable, const Containers::Sequence<String>& args, const Streams::InputStream::Ptr<byte>& in = nullptr,
            const Streams::OutputStream::Ptr<byte>& out = nullptr, const Streams::OutputStream::Ptr<byte>& error = nullptr)
            : ProcessRunner{executable, CommandLine{args}}
        {
            this->fStdIn_  = in;
            this->fStdOut_ = out;
            this->fStdErr_ = error;
        }

        [[deprecated ("Since Stroika v3.0d12 - pass in/out/error streams(can be nullptr) to Run method instead of CTOR")]] void
        Run (optional<ProcessResultType>* processResult, ProgressMonitor::Updater progress = nullptr, Time::DurationSeconds timeout = Time::kInfinity);

        [[deprecated ("Since Stroika v3.0d12 pass in/out/error(can be nullptr) in RunInbackground() method")]] BackgroundProcess
        RunInBackground (ProgressMonitor::Updater progress);

    public:
        /**
         */
        [[deprecated ("Since Stroika v3.0d12 - use GetOptions().fWorkingDirectory")]] optional<filesystem::path> GetWorkingDirectory () const;
        [[deprecated ("Since Stroika v3.0d12 - use SetOptions({.fWorkingDirectory})")]] void SetWorkingDirectory (const optional<filesystem::path>& d);

    public:
        /**
         *  If empty, stdin will not be empty (redirected from /dev/null).
         *
         *  Otherwise, the stream will be 'read' by the ProcessRunner and 'fed' downstream to
         *  the running subprocess.
         */
        [[deprecated ("Since Stroika v3.0d12 - pass in/out/error streams(can be nullptr) to Run method instead of CTOR")]] Streams::InputStream::Ptr<byte>
        GetStdIn () const;
        [[deprecated ("Since Stroika v3.0d12 - pass in/out/error streams(can be nullptr) to Run method instead of CTOR")]] void
        SetStdIn (const Streams::InputStream::Ptr<byte>& in);

    public:
        /**
         *  If empty, stdout will not be captured (redirected to /dev/null)
         */
        [[deprecated ("Since Stroika v3.0d12 - pass in/out/error streams(can be nullptr) to Run method instead of CTOR")]] Streams::OutputStream::Ptr<byte>
        GetStdOut () const;
        [[deprecated ("Since Stroika v3.0d12 - pass in/out/error streams(can be nullptr) to Run method instead of CTOR")]] void
        SetStdOut (const Streams::OutputStream::Ptr<byte>& out);

    public:
        /**
         *  If empty, stderr will not be captured (redirected to /dev/null)
         */
        [[deprecated ("Since Stroika v3.0d12 - pass in/out/error streams(can be nullptr) to Run method instead of CTOR")]] Streams::OutputStream::Ptr<byte>
        GetStdErr () const;
        [[deprecated ("Since Stroika v3.0d12 - pass in/out/error streams(can be nullptr) to Run method instead of CTOR")]] void
        SetStdErr (const Streams::OutputStream::Ptr<byte>& err);
    };

    /**
     */
    class ProcessRunner::Exception : public RuntimeErrorException<> {
    private:
        using inherited = RuntimeErrorException<>;

    public:
        /**
         */
#if qStroika_Foundation_Common_Platform_POSIX
        Exception (const String& cmdLine, const String& errorMessage, const optional<String>& stderrSubset = {},
                   const optional<uint8_t>& wExitStatus = optional<uint8_t>{}, const optional<uint8_t>& wTermSig = optional<uint8_t>{},
                   const optional<uint8_t>& wStopSig = optional<uint8_t>{});
#elif qStroika_Foundation_Common_Platform_Windows
        Exception (const String& cmdLine, const String& errorMessage, const optional<String>& stderrSubset = {},
                   const optional<DWORD>& err = optional<DWORD>{});
#endif
    private:
#if qStroika_Foundation_Common_Platform_POSIX
        static String mkMsg_ (const String& cmdLine, const String& errorMessage, const optional<String>& stderrSubset,
                              const optional<uint8_t>& wExitStatus, const optional<uint8_t>& wTermSig, const optional<uint8_t>& wStopSig);
#elif qStroika_Foundation_Common_Platform_Windows
        static String mkMsg_ (const String& cmdLine, const String& errorMessage, const optional<String>& stderrSubset, const optional<DWORD>& err);
#endif
    private:
        String fCmdLine_;
        String fErrorMessage_;
#if qStroika_Foundation_Common_Platform_POSIX
        optional<uint8_t> fWExitStatus_;
        optional<uint8_t> fWTermSig_;
        optional<uint8_t> fWStopSig_;
#elif qStroika_Foundation_Common_Platform_Windows
        optional<DWORD> fErr_;
#endif
    };

    /**
     *  Support more controlled running of sub-process, where wait timeouts don't necessarily kill the child process.
     * 
     *  \note it is perfectly legal to launch a subprocess, and not track it in any way, just ignoring (not saving)
     *        the BackgroundProcess object.
     */
    class ProcessRunner::BackgroundProcess {
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
         *  \brief maybe missing if process not yet (or ever successfully) launched. Child process may have
         *         already exited by the time this is returned.
         */
        optional<pid_t> GetChildProcessID () const;

    public:
        /**
         *  \brief wait until GetChildProcessID () returns a valid answer, or until the process failed to start
         *         (in which case calls PropagateIfException).
         */
        nonvirtual void WaitForStarted (Time::DurationSeconds timeout = Time::kInfinity) const;

    public:
        /**
         *
         *  @see Join ()
         *  @see JoinUntil ()
         */
        nonvirtual void WaitForDone (Time::DurationSeconds timeout = Time::kInfinity) const;

    public:
        /**
         *  \brief Join () does WaitForDone () and throw exception if there was any error (see PropagateIfException).
         *
         *  \note   Aliases - this used to be called WaitForDoneAndPropagateErrors; but used the name Join () to mimic the name used with Threads - NOT
         *          because that's used in the implementation, but because its essentially logically the same thing.
         *
         *  @see JoinUntil ()
         *  @see WaitForDone ()
         */
        nonvirtual void Join (Time::DurationSeconds timeout = Time::kInfinity) const;

    public:
        /**
         *  \brief WaitForDoneUntil () and throw exception if there was any error (see PropagateIfException).
         *
         *  @see Join ()
         *  @see WaitForDone ()
         */
        nonvirtual void JoinUntil (Time::TimePointSeconds timeoutAt) const;

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
        shared_ptr<Rep_>                                               fRep_;
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

    private:
        friend class ProcessRunner;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ProcessRunner.inl"

#endif /*_Stroika_Foundation_Execution_ProcessRunner_h_*/

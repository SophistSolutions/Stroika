/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProcessRunner_h_
#define _Stroika_Foundation_Execution_ProcessRunner_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Characters/String.h"
#include    "../Containers/Sequence.h"
#include    "../Memory/BLOB.h"
#include    "../Memory/Optional.h"
#include    "../Streams/InputStream.h"
#include    "../Streams/OutputStream.h"

#include    "Process.h"
#include    "ProgressMonitor.h"



/**
 *  TODO:
 *
 *      @todo   Current IMPL throws on UNIX if bad status but doesnt on WINDoze. FIX TO THROW ON BOTH, and throw
 *              useful value, and be clear in docs and behavior!!!
 *
 *              However, EVEN IF WE THROW BAD VALUE, still try to read all output from process first, so that is available
 *              in the output stream object (like printout to stderr!!!)
 *
 *      @todo   Redo POSIX impl using vfork () or http://linux.die.net/man/3/posix_spawn
 *
 *      @todo   Need EXCEPTION TYPE that includes PROCESS_STATUS and throw that instead of current exception
 *              On failure.
 *
 *      @todo   PERHAPS add option to ignore process status return, and continue as if OK. Not really needed cuz
 *              you can do this yourself just ignroing the exception.
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
 *              with select() AND somehow maybe eventually wait on streams (so we dont have to pre-read it all)
 *
 *      @todo   logic for THREADs and for PROGRESS support are NOT thought through, and just a rough first stab
 *
 *      @todo   Make sure it handles well without blocking
 *              (tricks I had todo in HF - forcing extra reads so writes woudlnt block).
 *              Currently structured to work off a single runnable, which implies works off a single thread. That implies
 *              it must use select() - probably a good idea anyhow - on each socket used for operations (windows and POSIX).
 *
 *              So data pusher/buffer loop does select on external streams to see if data avialble.
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
 *  Design Overview:
 *      o
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            using   Characters::String;


            /**
             *  \brief Synchronously run the given command, and optionally support stdin/stdout/stderr as streams
             *
             *  Synchronously run the given command.
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
             *      String name = Execution::ProcessRunner (L"uname").Run (String ()).Trim ();
             *      \encode
             */
            class ProcessRunner {
            public:
                ProcessRunner () = delete;
                ProcessRunner (const ProcessRunner&) = delete;
            public:
                ProcessRunner (const String& commandLine, const Streams::InputStream<Byte>& in = nullptr, const Streams::OutputStream<Byte>& out = nullptr, const Streams::OutputStream<Byte>& error = nullptr);
                ProcessRunner (const String& executable, const Containers::Sequence<String>& args, const Streams::InputStream<Byte>& in = nullptr, const Streams::OutputStream<Byte>& out = nullptr, const Streams::OutputStream<Byte>& error = nullptr);
#if     !qTargetPlatformSDKUseswchar_t && 0
                ProcessRunner (const SDKString& commandLine, Streams::InputStream<Byte> in = nullptr, Streams::OutputStream<Byte> out = nullptr, Streams::OutputStream<Byte> error = nullptr)
                    : ProcessRunner (String::FromSDKString (commandLine), in, out, error)
                {
                }
                ProcessRunner (const SDKString& executable, const Containers::Sequence<SDKString>& args, Streams::InputStream<Byte> in = nullptr, Streams::OutputStream<Byte> out = nullptr, Streams::OutputStream<Byte> error = nullptr)
                    : ProcessRunner (String::FromSDKString (executable), args, in, out, error)
                {
                }
#endif

            public:
                nonvirtual  ProcessRunner& operator= (const ProcessRunner&) = delete;

            public:
                /**
                 * defaults to 'missing'. If missing, then the OS default for new directory is used on created process (usually same as parent process)
                 */
                nonvirtual  Memory::Optional<String>    GetWorkingDirectory ();
                nonvirtual  void                        SetWorkingDirectory (const Memory::Optional<String>& d);

            public:
                /**
                 *  If empty, stdin will not be empty (redirected from /dev/null).
                 *
                 *  Otherwise, the stream will be 'read' by the ProcessRunner and 'fed' downstream to
                 *  the running subprocess.
                 */
                nonvirtual  Streams::InputStream<Byte>      GetStdIn () const;
                nonvirtual  void                            SetStdIn (const Streams::InputStream<Byte>& in);
                nonvirtual  void                            SetStdIn (const Memory::BLOB& in);

            public:
                /**
                 *  If empty, stdout will not be captured (redirected to /dev/null)
                 */
                nonvirtual  Streams::OutputStream<Byte>     GetStdOut () const;
                nonvirtual  void                            SetStdOut (const Streams::OutputStream<Byte>& out);

            public:
                /**
                 *  If empty, stderr will not be captured (redirected to /dev/null)
                 */
                nonvirtual  Streams::OutputStream<Byte>     GetStdErr () const;
                nonvirtual  void                            SetStdErr (const Streams::OutputStream<Byte>& err);

            public:
                /**
                 *  Note that 'in' will be sent to the stdin of the subprocess, 'out' will be read from the
                 *  stdout of the subprocess and error will be read from the stderr of the subprocess.
                 *
                 *  Each of these CAN be null, and will if so, that will be interpretted as an empty stream
                 *  (for in/stdin), and for out/error, just means the results will be redirected to /dev/null.
                 */
                nonvirtual  function<void()>    CreateRunnable (ProgressMonitor::Updater progress = nullptr);

            public:
                /**
                 *  Creates the runnable above, and directly runs it in place (with the given timeout).
                 *  To be able to control cancelation, use CreateRunnable () directly.
                 *
                 *  The Run() overload taking cmdStdInValue replaces the current stdin stream associated with the
                 *  ProcessRunner, and replaces the stdout, and replaces its stdout stream with one that captures
                 *  results as a string.
                 */
                nonvirtual  void                Run (ProgressMonitor::Updater progress = nullptr, Time::DurationSecondsType timeout = Time::kInfinite);
                nonvirtual  Characters::String  Run (const Characters::String& cmdStdInValue, ProgressMonitor::Updater progress = nullptr, Time::DurationSecondsType timeout = Time::kInfinite);

            private:
                Memory::Optional<String>        fCommandLine_;
                Memory::Optional<String>        fExecutable_;
                Containers::Sequence<String>    fArgs_;         // ignored if fExecutable empty
                Memory::Optional<String>        fWorkingDirectory_;
                Streams::InputStream<Byte>      fStdIn_;
                Streams::OutputStream<Byte>     fStdOut_;
                Streams::OutputStream<Byte>     fStdErr_;
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
             *  \note   On linux, this also detaches from the terminal driver, to avoid spurrious SIGHUP
             *          and SIGTTIN and SIGTTOU
             */
            pid_t   DetachedProcessRunner (const String& commandLine);
            pid_t   DetachedProcessRunner (const String& executable, const Containers::Sequence<String>& args);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ProcessRunner.inl"

#endif  /*_Stroika_Foundation_Execution_ProcessRunner_h_*/

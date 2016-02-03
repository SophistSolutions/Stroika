/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <sstream>

#if     qPlatform_POSIX
#include    <fcntl.h>
#include    <sys/resource.h>
#include    <sys/stat.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <unistd.h>
#endif

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Characters/ToString.h"
#include    "../Characters/String_Constant.h"
#include    "../Containers/Sequence.h"
#include    "../Debug/Trace.h"
#if     qPlatform_Windows
#include    "Platform/Windows/Exception.h"
#endif
#include    "../Execution/CommandLine.h"
#include    "../Execution/ErrNoException.h"
#include    "../Execution/Finally.h"
#include    "../IO/FileSystem/FileSystem.h"
#include    "../IO/FileSystem/FileUtils.h"
#include    "../IO/FileSystem/PathName.h"
#include    "../Memory/SmallStackBuffer.h"
#include    "../Streams/MemoryStream.h"
#include    "../Streams/TextReader.h"
#include    "../Streams/TextWriter.h"

#include    "Sleep.h"
#include    "Thread.h"

#include    "ProcessRunner.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;

using   Debug::TraceContextBumper;





// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1




#if     qPlatform_POSIX
namespace {
    inline  void    CLOSE_ (int fd)
    {
        Execution::Handle_ErrNoResultInteruption ([fd] () -> int { return ::close (fd);});
    }
}
#endif

#if     qPlatform_POSIX
namespace {
    static  const   int kMaxFD_ = [] () -> int {
        struct rlimit fds {};
        if (::getrlimit (RLIMIT_NOFILE, &fds) == 0)
        {
            return fds.rlim_cur;
        }
        else {
            return 1024;    // wag
        }
    } ();
}
#endif




#if     qPlatform_Windows
namespace {
    class   AutoHANDLE_ {
    public:
        AutoHANDLE_ (HANDLE h = INVALID_HANDLE_VALUE):
            fHandle (h)
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
        void    Close ()
        {
            if (fHandle != INVALID_HANDLE_VALUE) {
                Verify (::CloseHandle (fHandle));
                fHandle = INVALID_HANDLE_VALUE;
            }
        }
        void    ReplaceHandleAsNonInheritable ()
        {
            HANDLE  result  =   INVALID_HANDLE_VALUE;
            Verify (::DuplicateHandle (::GetCurrentProcess (), fHandle, ::GetCurrentProcess (), &result , 0, FALSE, DUPLICATE_SAME_ACCESS));
            Verify (::CloseHandle (fHandle));
            fHandle = result;
        }
    public:
        HANDLE  fHandle;
    };
    inline  void    SAFE_HANDLE_CLOSER_ (HANDLE* h)
    {
        RequireNotNull (h);
        if (*h != INVALID_HANDLE_VALUE) {
            Verify (::CloseHandle (*h));
            *h = INVALID_HANDLE_VALUE;
        }
    }
}
#endif




#if     qPlatform_Windows
namespace {
// still unsure if needed/useful - I now think the PeekNamedPipe stuff is NOT needed, but
// I can turn it on if needed -- LGP 2009-05-07
//#define   qUsePeekNamedPipe   1
#ifndef qUsePeekNamedPipe
#define qUsePeekNamedPipe   0
#endif
    /*
     *  This code should all work with the smaller buffer sizes, but is more efficient with larger buffers.
     *  Just set to use the smaller buffers to stress test and debug.
     *
     *  There is some subtle but serious bug with my pipe code - and that APPEARS to just be that
     *  WaitForMultipleObjects doesn't work with PIPEs.
     *
     *  I COULD just rewrite alot of this code to NOT use PIPES - but actual files. That might solve the problem
     *  because they never 'fill up'.
     *
     *  Alternatively - it might be that my switch to ASYNC mode (PIPE_NOWAIT) was a bad idea. Maybe if I got
     *  rid of that - the WAIT code could be made to work? Not sure.
     *
     *  Anyhow - this appears to be adequate for now...
     *
     *          -- LGP 2006-10-17
     */
    constexpr size_t    kPipeBufSize    =   256 * 1024;
    constexpr size_t    kReadBufSize    =   32 * 1024;
}
#endif




#if     qPlatform_Windows
namespace {
    void    ReadAnyAvailableAndCopy2StreamWithoutBlocking_ (HANDLE p, Streams::OutputStream<Byte> o)
    {
        RequireNotNull (p);
        Byte    buf[kReadBufSize];
#if     qUsePeekNamedPipe
        DWORD   nBytesAvail {};
#endif
        DWORD   nBytesRead  {};
        // Read normally blocks, we don't want to because we may need to write more before it can output
        // and we may need to timeout
        while (
#if     qUsePeekNamedPipe
            ::PeekNamedPipe (p, nullptr, nullptr, nullptr, &nBytesAvail, nullptr) and
            nBytesAvail != 0 and
#endif
            ::ReadFile (p, buf, sizeof (buf), &nBytesRead, nullptr)
            and nBytesRead > 0
        ) {
            if (not o.empty ()) {
                o.Write (buf, buf + nBytesRead);
            }
        }
    }
}
#endif






/*
 ********************************************************************************
 ************************** Execution::ProcessRunner ****************************
 ********************************************************************************
 */
ProcessRunner::ProcessRunner (const String& commandLine, const Streams::InputStream<Byte>& in, const Streams::OutputStream<Byte>& out, const Streams::OutputStream<Byte>& error)
    : fCommandLine_ (commandLine)
    , fExecutable_ ()
    , fArgs_ ()
    , fStdIn_ (in)
    , fStdOut_ (out)
    , fStdErr_ (error)
{
}

ProcessRunner::ProcessRunner (const String& executable, const Containers::Sequence<String>& args, const Streams::InputStream<Byte>& in, const Streams::OutputStream<Byte>& out, const Streams::OutputStream<Byte>& error)
    : fCommandLine_ ()
    , fExecutable_ (executable)
    , fArgs_ (args)
    , fStdIn_ (in)
    , fStdOut_ (out)
    , fStdErr_ (error)
{
}

Memory::Optional<String> ProcessRunner::GetWorkingDirectory ()
{
    return fWorkingDirectory_;
}

void    ProcessRunner::SetWorkingDirectory (const Memory::Optional<String>& d)
{
    fWorkingDirectory_ = d;
}

Streams::InputStream<Byte>  ProcessRunner::GetStdIn () const
{
    return fStdIn_;
}

void      ProcessRunner::SetStdIn (const Streams::InputStream<Byte>& in)
{
    fStdIn_ = in;
}

void     ProcessRunner:: SetStdIn (const Memory::BLOB& in)
{
    fStdIn_ = in.As<Streams::InputStream<Byte>> ();
}

Streams::OutputStream<Byte> ProcessRunner::GetStdOut () const
{
    return fStdOut_;
}

void    ProcessRunner::SetStdOut (const Streams::OutputStream<Byte>& out)
{
    fStdOut_ = out;
}

Streams::OutputStream<Byte> ProcessRunner::GetStdErr () const
{
    return fStdErr_;
}

void    ProcessRunner::SetStdErr (const Streams::OutputStream<Byte>& err)
{
    fStdErr_ = err;
}

function<void()>    ProcessRunner::CreateRunnable_ (ProgressMonitor::Updater progress)
{
    TraceContextBumper  ctx ("ProcessRunner::CreateRunnable_");

    String                      cmdLine     =   fCommandLine_.Value ();
    Memory::Optional<String>    workingDir  =   GetWorkingDirectory ();
    Streams::InputStream<Byte>  in          =   GetStdIn ();
    Streams::OutputStream<Byte> out         =   GetStdOut ();
    Streams::OutputStream<Byte> err         =   GetStdErr ();

    return [progress, cmdLine, workingDir, in, out, err] () {
        TraceContextBumper  traceCtx ("ProcessRunner::CreateRunnable_::{}::Runner...");

        SDKString       currentDirBuf_;
        const SDKChar*  currentDir      =   workingDir ? (currentDirBuf_ = workingDir->AsSDKString (), currentDirBuf_.c_str ()) : nullptr;

        DbgTrace (L"cmdLine: %s", cmdLine.LimitLength (100, false).c_str ());
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (SDKSTR ("currentDir: %s"), currentDir == nullptr ? "nullptr" : Characters::CString::LimitLength (currentDir, 50, false).c_str ());
#endif

        // Horrible implementation - just designed to be quickie get started...
        Memory::BLOB    stdinBLOB;
        if (not in.empty ()) {
            stdinBLOB =   in.ReadAll ();
        }
#if     qPlatform_POSIX
        // @todo must fix to be smart about non-blocking deadlocks etc (like windows above)

        /*
         *  @todo   BELOW CODE NOT SAFE - IF YOU GET A THROW AFTER first PIPE call but before second, we leak 2 fds!!!
         *          NOT TOO BAD , but bad!!! Below 'finally' attempt is INADEQUATE
         */

        /*
         *  NOTE:
         *      From http://linux.die.net/man/2/pipe
         *          "The array pipefd is used to return two file descriptors referring to the ends
         *          of the pipe. pipefd[0] refers to the read end of the pipe. pipefd[1] refers to
         *          the write end of the pipe"
         */
        int  jStdin[2];
        int  jStdout[2];
        int  jStderr[2];
        Execution::Handle_ErrNoResultInteruption ([&jStdin] () -> int { return ::pipe (jStdin);});
        Execution::Handle_ErrNoResultInteruption ([&jStdout] () -> int { return ::pipe (jStdout);});
        Execution::Handle_ErrNoResultInteruption ([&jStderr] () -> int { return ::pipe (jStderr);});
        // assert cuz code below needs to be more careful if these can overlap 0..2
        Assert (jStdin[0] >= 3 and jStdin[1] >= 3);
        Assert (jStdout[0] >= 3 and jStdout[1] >= 3);
        Assert (jStderr[0] >= 3 and jStderr[1] >= 3);
        DbgTrace ("jStdout[0-CHILD] = %d and jStdout[1-PARENT] = %d", jStdout[0], jStdout[1]);

        /*
         *  Note: Important to do all this code before the fork, because once we fork, we, lose other threads
         *  but share copy of RAM, so they COULD have mutexes locked! And we could deadlock waiting on them, so after
         *  fork, we are VERY limited as to what we can safely do.
         */
        const   char*   thisEXEPath_cstr    =   nullptr;
        char**    thisEXECArgv              =   nullptr;
        Memory::SmallStackBuffer<char>      execDataArgsBuffer (0);
        Memory::SmallStackBuffer<char*>     execArgsPtrBuffer (0);
        {
            Sequence<String>    commandLine     { Execution::ParseCommandLine (cmdLine) };
            Sequence<size_t>    argsIdx;
            size_t              bufferIndex {};
            execArgsPtrBuffer.GrowToSize (commandLine.size () + 1);
            for (auto i : commandLine) {
                string  tmp { i.AsNarrowSDKString () };
                for (char c : tmp) {
                    execDataArgsBuffer.push_back (c);
                }
                execDataArgsBuffer.push_back ('\0');
                argsIdx.push_back (bufferIndex);
                bufferIndex = execDataArgsBuffer.GetSize ();
            }
            execDataArgsBuffer.push_back ('\0');
            for (size_t i = 0; i < commandLine.size (); ++i) {
                execArgsPtrBuffer[i] = execDataArgsBuffer.begin () + argsIdx[i];
            }
            execArgsPtrBuffer[commandLine.size ()] = nullptr;

            // no longer change buffers, and just make pointers point to right place
            thisEXEPath_cstr = execDataArgsBuffer;
            thisEXECArgv = execArgsPtrBuffer;
        }

        int childPID = ::fork ();
        Execution::ThrowErrNoIfNegative (childPID);
        if (childPID == 0) {
            try {
                /*
                 *  In child process. Dont DBGTRACE here, or do anything that could raise an exception. In the child process
                 *  this would be bad...
                 */
                if (currentDir != nullptr) {
                    DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Wunused-result\"")
                    (void)::chdir (currentDir);
                    DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Wunused-result\"")
                }
                {
                    /*
                     *  move arg stdin/out/err to 0/1/2 file-descriptors. Don't bother with variants that can handle errors/exceptions cuz we cannot really here...
                     */
                    int useSTDIN    =   jStdin[0];
                    int useSTDOUT   =   jStdout[1];
                    int useSTDERR   =   jStderr[1];
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
                int r   =   ::execvp (thisEXEPath_cstr, thisEXECArgv);
                ::_exit (EXIT_FAILURE);
            }
            catch (...) {
                ::_exit (EXIT_FAILURE);
            }
        }
        else {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("In Parent Fork: child process PID=%d", childPID);
#endif
            /*
             * WE ARE PARENT
             */
            int useSTDIN    =   jStdin[1];
            int useSTDOUT   =   jStdout[0];
            int useSTDERR   =   jStderr[0];
            {
                CLOSE_ (jStdin[0]);
                CLOSE_ (jStdout[1]);
                CLOSE_ (jStderr[1]);
            }

            Execution::Finally cleanup1 ([&useSTDIN, &useSTDOUT, &useSTDERR] {
                if (useSTDIN >= 0)
                {
                    IgnoreExceptionsForCall (CLOSE_ (useSTDIN));
                }
                if (useSTDOUT >= 0)
                {
                    IgnoreExceptionsForCall (CLOSE_ (useSTDOUT));
                }
                if (useSTDERR >= 0)
                {
                    IgnoreExceptionsForCall (CLOSE_ (useSTDERR));
                }
            });

// really need to do peicemail like above to avoid deadlock
            {
                const Byte* p   =   stdinBLOB.begin ();
                const Byte* e   =   p + stdinBLOB.GetSize ();
                if (p != e) {
                    for (const Byte* i = p; i != e;) {
                        int bytesWritten = ThrowErrNoIfNegative (Handle_ErrNoResultInteruption ([useSTDIN, i, e] () { return ::write (useSTDIN, i, e - i); }));
                        Assert (bytesWritten >= 0);
                        Assert (bytesWritten <= (e - i));
                        i += bytesWritten;
                    }
                }
                // in case child process reads from its STDIN to EOF
                CLOSE_ (useSTDIN);
                useSTDIN = -1;
            }
            // @todo READ STDERR - and do ALL in one bug loop so no deadlocks
            /*
             *  Read whatever is left...and blocking here is fine, since at this point - the subprocess should be closed/terminated.
             */
            if (not out.empty ()) {
                Byte    buf[1024];
                int     nBytesRead  =   0;

                // @todo not quite right - unless we have blocking
                // (NOTE - pretty sure this is blocking - but must handle EINTR)
                while ((nBytesRead = ::read (useSTDOUT, buf, sizeof (buf))) > 0) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("from stdout nBytesRead = %d", nBytesRead);
#endif
                    out.Write (buf, buf + nBytesRead);
                }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("from stdout nBytesRead = %d, errno=%d", nBytesRead, errno);
#endif
            }
            if (not err.empty ()) {
                Byte    buf[1024];
                int     nBytesRead  =   0;

                // @todo not quite right - unless we have blcokgin
                while ((nBytesRead = ::read (useSTDERR, buf, sizeof (buf))) > 0) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("from stderr nBytesRead = %d", nBytesRead);
#endif
                    err.Write (buf, buf + nBytesRead);
                }
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("from stderr nBytesRead = %d, errno=%d", nBytesRead, errno);
#endif
            }
            // not sure we need?
            int status = 0;
            int flags = 0;  // FOR NOW - HACK - but really must handle sig-interuptions...
            int result = ::waitpid (childPID, &status, flags);                /* Wait for child */
            // throw / warn if result other than child exited normally
            if (result != childPID or not WIFEXITED (status) or WEXITSTATUS(status) != 0) {
                // @todo fix this message
                DbgTrace ("childPID=%d, result=%d, status=%d, WIFEXITED=%d, WEXITSTATUS=%d, WIFSIGNALED=%d", childPID, result, status, WIFEXITED(status), WEXITSTATUS(status), WIFSIGNALED(status));
                Throw (StringException (L"sub-process failed"));
            }
        }
#elif   qPlatform_Windows
//      DbgTrace (_T ("timeout: %f"), timeout);
//  #if     qDefaultTracingOn
//      ContextCounter  ctxCounter;
//      DbgTrace (_T ("sNCurRunExtProcessCalls: %d"), sNCurRunExtProcessCalls);
//  #endif

        /*
         *  o   Build directory into which we can copy the JAR file plugin,
         *  o   create STDIN/STDOUT file handles to send/grab results
         *  o   Run the process, waiting for it to finish.
         *  o   Grab results from STDOUT file.
         *  o   Cleanup created directory.
         */

        // use AutoHANDLE so these are automatically closed at the end of the procedure, whether it ends normally or via
        // exception.
        AutoHANDLE_  jStdin[2];
        AutoHANDLE_  jStdout[2];
        AutoHANDLE_  jStderr[2];

        PROCESS_INFORMATION processInfo {};
        processInfo.hProcess = INVALID_HANDLE_VALUE;
        processInfo.hThread = INVALID_HANDLE_VALUE;

        try {
            {
                SECURITY_DESCRIPTOR sd {};
                Verify (::InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION));
                Verify (::SetSecurityDescriptorDacl (&sd, true, 0, false));
                SECURITY_ATTRIBUTES sa  =   {   sizeof (SECURITY_ATTRIBUTES), &sd, true };
                Verify (::CreatePipe (&jStdin[1], &jStdin[0], &sa, kPipeBufSize));
                Verify (::CreatePipe (&jStdout[1], &jStdout[0], &sa, kPipeBufSize));
                Verify (::CreatePipe (&jStderr[1], &jStderr[0], &sa, kPipeBufSize));
                /*
                 *  Make sure the ends of the pipe WE hang onto are not inheritable, because otherwise the READ
                 *  wont return EOF (until the last one is closed).
                 */
                jStdin[0].ReplaceHandleAsNonInheritable ();
                jStdout[1].ReplaceHandleAsNonInheritable ();
                jStderr[1].ReplaceHandleAsNonInheritable ();
            }

            STARTUPINFO startInfo {};
            startInfo.cb = sizeof (startInfo);
            startInfo.hStdInput = jStdin[1];
            startInfo.hStdOutput = jStdout[0];
            startInfo.hStdError = jStderr[0];
            startInfo.dwFlags |= STARTF_USESTDHANDLES;

            DWORD   createProcFlags { CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS | DETACHED_PROCESS };

            {
                bool    bInheritHandles     =   true;
                TCHAR   cmdLineBuf[32768];          // crazy MSFT definition! - why this should need to be non-const!
                Characters::CString::Copy (cmdLineBuf, NEltsOf (cmdLineBuf), cmdLine.AsSDKString ().c_str ());
                Execution::Platform::Windows::ThrowIfFalseGetLastError (
                    ::CreateProcess (nullptr, cmdLineBuf, nullptr, nullptr, bInheritHandles, createProcFlags, nullptr, currentDir, &startInfo, &processInfo)
                );
            }

            {
                /*
                 * Remove our copy of the stdin/stdout/stderr which belong to the child (so EOF will work properly).
                 */
                jStdin[1].Close ();
                jStdout[0].Close ();
                jStderr[0].Close ();
            }

            AutoHANDLE_& useSTDIN    =   jStdin[0];
            Assert (jStdin[1] == INVALID_HANDLE_VALUE);
            AutoHANDLE_& useSTDOUT   =   jStdout[1];
            Assert (jStdout[0] == INVALID_HANDLE_VALUE);
            AutoHANDLE_& useSTDERR   =   jStderr[1];
            Assert (jStderr[0] == INVALID_HANDLE_VALUE);

            if (processInfo.hProcess != INVALID_HANDLE_VALUE) {
                {
                    {
                        /*
                         * Set the pipe endpoints to non-blocking mode.
                         */
                        auto    mkPipeNoWait_ = [](HANDLE ioHandle) -> void {
                            DWORD   stdinMode = 0;
                            Verify (::GetNamedPipeHandleState (ioHandle, &stdinMode, nullptr, nullptr, nullptr, nullptr, 0));
                            stdinMode |= PIPE_NOWAIT;
                            Verify (::SetNamedPipeHandleState (ioHandle, &stdinMode, nullptr, nullptr));
                        };
                        mkPipeNoWait_ (useSTDIN);
                        mkPipeNoWait_ (useSTDOUT);
                        mkPipeNoWait_ (useSTDERR);
                    }

                    /*
                     *  Fill child-process' stdin with the source document.
                     */
                    if (not stdinBLOB.empty ()) {
                        const Byte* p   =   stdinBLOB.begin ();
                        const Byte* e   =   p + stdinBLOB.GetSize ();
                        while (p < e) {
                            DWORD   written     =   0;
                            if (::WriteFile (useSTDIN, p, Math::PinToMaxForType<DWORD> (e - p), &written, nullptr) == 0) {
                                DWORD   err = ::GetLastError ();
                                // sometimes we fail because the target process hasn't read enough and the pipe is full.
                                // Unfortunately - MSFT doesn't seem to have a single clear error message nor any clear
                                // documentation about what WriteFile () returns in this case... So there maybe other erorrs
                                // that are innocuous that may cause is to prematurely terminate our 'RunExternalProcess'.
                                //      -- LGP 2009-05-07
                                if (err != ERROR_SUCCESS and
                                        err != ERROR_NO_MORE_FILES and
                                        err != ERROR_PIPE_BUSY and
                                        err != ERROR_NO_DATA
                                   ) {
                                    DbgTrace ("in RunExternalProcess_ - throwing %d while fill in stdin", err);
                                    Execution::Platform::Windows::Exception::Throw (err);
                                }
                            }
                            Assert (written <= static_cast<size_t> (e - p));
                            p += written;
                            // in case we are failing to write to the stdIn because of blocked output on an outgoing pipe
                            if (p < e) {
                                ReadAnyAvailableAndCopy2StreamWithoutBlocking_ (useSTDOUT, out);
                                ReadAnyAvailableAndCopy2StreamWithoutBlocking_ (useSTDERR, err);
                            }
                            if (p < e and written == 0) {
                                // if we have more to write, but that the target process hasn't consumed it yet - don't spin trying to
                                // send it data - back off a little
                                Execution::Sleep (0.1f);
                            }
#if 0
                            // Do timeout handling at a higher level
                            if (Time::GetTickCount () > timeoutAt) {
                                DbgTrace (_T ("process timed out (writing initial data) - so throwing up!"));
                                // then we've timed out - kill the process and DONT return the partial result!
                                (void)::TerminateProcess (processInfo.hProcess, -1);    // if it exceeded the timeout - kill it (could already be done by now - in which case - this will be ignored - fine...
                                Execution::Throw (Execution::Platform::Windows::Exception (ERROR_TIMEOUT));
                            }
#endif
                        }
                    }

                    // in case invoked sub-process is reading, and waiting for EOF before processing...
                    useSTDIN.Close ();
                }


                /*
                 *  Must keep reading while waiting - in case the child emits so much information that it
                 *  fills the OS PIPE buffer.
                 */
                int     timesWaited =   0;
                while (true) {
                    /*
                     *  It would be nice to be able to WAIT on the PIPEs - but that doesn't appear to work when they
                     *  are in ASYNCRONOUS mode.
                     *
                     *  So - instead - just wait a very short period, and then retry polling the pipes for more data.
                     *          -- LGP 2006-10-17
                     */
                    HANDLE  events[1] = { processInfo.hProcess };

                    // We don't want to busy wait too much, but if its fast (with java, thats rare ;-)) don't want to wait
                    // too long needlessly...
                    //
                    // Also - its not exactly a busy-wait. Its just a wait between reading stuff to avoid buffers filling. If the
                    // process actually finishes, it will change state and the wait should return immediately.
                    double  remainingTimeout        =   (timesWaited <= 5) ? 0.1 : 0.5;
                    DWORD   waitResult  =   ::WaitForMultipleObjects (static_cast<DWORD> (NEltsOf (events)), events, false, static_cast<int> (remainingTimeout * 1000));
                    timesWaited++;

                    ReadAnyAvailableAndCopy2StreamWithoutBlocking_ (useSTDOUT, out);
                    ReadAnyAvailableAndCopy2StreamWithoutBlocking_ (useSTDERR, err);
                    //ReadAnyAvailableAndDumpMsgToTraceWithoutBlocking_ (useSTDERR);
                    switch (waitResult) {
                        case    WAIT_OBJECT_0: {
                                DbgTrace (_T ("process finished normally"));
//                              timeoutAt = -1.0f;  // force out of loop
                                goto DoneWithProcess;
                            }
                            break;
                        case    WAIT_TIMEOUT: {
                                DbgTrace ("still waiting for external process output (WAIT_TIMEOUT)");
                            }
                    }
                }

DoneWithProcess:
                SAFE_HANDLE_CLOSER_ (&processInfo.hProcess);
                SAFE_HANDLE_CLOSER_ (&processInfo.hThread);

                {
                    DWORD   stdoutMode  =   0;
                    Verify (::GetNamedPipeHandleState (useSTDOUT, &stdoutMode, nullptr, nullptr, nullptr, nullptr, 0));
                    stdoutMode &= ~PIPE_NOWAIT;
                    Verify (::SetNamedPipeHandleState (useSTDOUT, &stdoutMode, nullptr, nullptr));

                    /*
                     *  Read whatever is left...and blocking here is fine, since at this point - the subprocess should be closed/terminated.
                     */
                    if (not out.empty ()) {
                        Byte    buf[kReadBufSize];
                        DWORD   nBytesRead  =   0;
                        while (::ReadFile (useSTDOUT, buf, sizeof (buf), &nBytesRead, nullptr)) {
                            out.Write (buf, buf + nBytesRead);
                        }
                    }
                }

                // wait some reasonable amount of time for hte process to finish, and then KILL IT
            }

            // @todo MAYBE need to copy STDERRR TOO!!!
        }
        catch (...) {
            if (processInfo.hProcess != INVALID_HANDLE_VALUE) {
                (void)::TerminateProcess (processInfo.hProcess, -1);    // if it exceeded the timeout - kill it
                SAFE_HANDLE_CLOSER_ (&processInfo.hProcess);
                SAFE_HANDLE_CLOSER_ (&processInfo.hThread);
            }
            Execution::ReThrow ();
        }

        // now write the temps to the stream
#endif
    };
}

void    ProcessRunner::Run (ProgressMonitor::Updater progress, Time::DurationSecondsType timeout)
{
    TraceContextBumper  ctx ("ProcessRunner::Run");
    if (timeout == Time::kInfinite) {
        CreateRunnable_ (progress) ();
    }
    else {
        Thread t (CreateRunnable_ (progress));
        t.SetThreadName (L"ProcessRunner thread");
        t.Start ();
        t.WaitForDone (timeout);
        t.ThrowIfDoneWithException ();
    }
}

Characters::String  ProcessRunner::Run (const Characters::String& cmdStdInValue, ProgressMonitor::Updater progress, Time::DurationSecondsType timeout)
{
    Streams::InputStream<Byte>  oldStdIn    =   GetStdIn ();
    Streams::OutputStream<Byte> oldStdOut   =   GetStdOut ();
    try {
        Streams::MemoryStream<Memory::Byte>   useStdIn;
        Streams::MemoryStream<Memory::Byte>   useStdOut;

        // Prefill stream
        // @todo - decide if we should use Streams::TextWriter::Format::eUTF8WithoutBOM
        if (not cmdStdInValue.empty ()) {
            // for now while we write BOM, dont write empty string as just a BOM!
            Streams::TextWriter (useStdIn).Write (cmdStdInValue.c_str ());
        }
        Assert (useStdIn.GetReadOffset () == 0);

        SetStdIn (useStdIn);
        SetStdOut (useStdOut);

        Run (progress, timeout);

        SetStdIn (oldStdIn);
        SetStdOut (oldStdOut);

        // get from 'useStdOut'
        Assert (useStdOut.GetReadOffset () == 0);
        return Streams::TextReader (useStdOut).ReadAll ();
    }
    catch (...) {
        SetStdIn (oldStdIn);
        SetStdOut (oldStdOut);
        Execution::ReThrow ();
    }
}









/*
 ********************************************************************************
 ****************** Execution::DetachedProcessRunner ****************************
 ********************************************************************************
 */
pid_t   Execution::DetachedProcessRunner (const String& commandLine)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper  ctx ("Execution::DetachedProcessRunner");
    DbgTrace (L"(commandline=%s)", commandLine.c_str ());
#endif
    String              exe;
    Sequence<String>    args;
    {
        Sequence<String> tmp     { Execution::ParseCommandLine (commandLine) };
        if (tmp.size () == 0) {
            Execution::Throw (Execution::StringException (String_Constant (L"invalid command argument to DetachedProcessRunner")));
        }
        exe = tmp[0];
        args = tmp;
    }
    return DetachedProcessRunner (exe, args);
#if 0
    // OBSOLETED - I THINK RARELY IF EVER USED - 2016-02-01
#if     qPlatform_Windows
    PROCESS_INFORMATION processInfo {};
    processInfo.hProcess = INVALID_HANDLE_VALUE;
    processInfo.hThread = INVALID_HANDLE_VALUE;
    Finally cleanup = [&processInfo]() {
        SAFE_HANDLE_CLOSER_ (&processInfo.hProcess);
        SAFE_HANDLE_CLOSER_ (&processInfo.hThread);
    };

    STARTUPINFO startInfo {};
    startInfo.cb = sizeof (startInfo);
    startInfo.hStdInput = INVALID_HANDLE_VALUE;
    startInfo.hStdOutput = INVALID_HANDLE_VALUE;
    startInfo.hStdError = INVALID_HANDLE_VALUE;
    startInfo.dwFlags |= STARTF_USESTDHANDLES;

    DWORD   createProcFlags =   0;
    createProcFlags |= CREATE_NO_WINDOW;
    createProcFlags |= NORMAL_PRIORITY_CLASS;
    createProcFlags |= DETACHED_PROCESS;
    {
        bool    bInheritHandles     =   true;
        TCHAR   cmdLineBuf[32768];          // crazy MSFT definition! - why this should need to be non-const!
        Characters::CString::Copy (cmdLineBuf, NEltsOf (cmdLineBuf), commandLine.AsSDKString ().c_str ());
        Execution::Platform::Windows::ThrowIfFalseGetLastError (
            ::CreateProcess (nullptr, cmdLineBuf, nullptr, nullptr, bInheritHandles, createProcFlags, nullptr, nullptr, &startInfo, &processInfo)
        );
    }
    return processInfo.dwProcessId;
#elif   qPlatform_POSIX
    // consider using 'system' here...
#endif

    // @todo - better job both parsing separate args, and documenting how this is done!!!
    String              exe;
    Sequence<String>    args;
    {
        Sequence<String> tmp = commandLine.Tokenize (Set<Character> { ' ' });
        if (tmp.size () == 0) {
            Execution::Throw (Execution::StringException (String_Constant (L"invalid command argument to DetachedProcessRunner")));
        }
        exe = tmp[0];
        for (auto i = tmp.begin (); i != tmp.end (); ++i) {
            args.Append (*i);
        }
    }
    return DetachedProcessRunner (exe, args);
#endif
}

pid_t   Execution::DetachedProcessRunner (const String& executable, const Containers::Sequence<String>& args)
{
    TraceContextBumper  ctx ("Execution::DetachedProcessRunner");
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"(executable=%s, args=%s)", executable.c_str (), Characters::ToString (args).c_str ());
#endif
    //@todo CONSIDER USING new Filesystem::...FindExecutableInPath - to check the right location, but dont bother for
    // now...
    //IO::FileSystem::FileSystem::Default ().CheckAccess (RESULT OF FINEXUTABLEINPATH, true, false); - or something like that.

    Sequence<String>    useArgs;
    if (args.empty ()) {
        useArgs.Append (IO::FileSystem::GetFileBaseName (executable));
    }
    else {
        bool firstTimeThru = true;
        for (auto i = args.begin (); i != args.end (); ++i) {
            if (firstTimeThru and i->empty ()) {
                useArgs.Append (IO::FileSystem::GetFileBaseName (executable));
            }
            else {
                useArgs.Append (*i);
            }
            firstTimeThru = false;
        }
    }

#if     qPlatform_POSIX
    Characters::SDKString thisEXEPath =   executable.AsSDKString ();

    // Note - we do this OUTSIDE the fork since its unsafe to do mallocs etc inside forked space.

    // must map args to SDKString, and then right lifetime c-string pointers
    vector<SDKString> tmpTStrArgs;
    tmpTStrArgs.reserve (args.size ());
    for (String i : useArgs) {
        tmpTStrArgs.push_back (i.AsSDKString ());
    }
    vector<char*>   useArgsV;
    for (auto i = tmpTStrArgs.begin (); i != tmpTStrArgs.end (); ++i) {
        // POSIX API takes non-const strings, but I'm pretty sure this is safe, and I cannot imagine
        // their overwriting these strings!
        // -- LGP 2013-06-08
        useArgsV.push_back (const_cast<char*> (i->c_str ()));
    }
    useArgsV.push_back (nullptr);

    pid_t   pid =   Execution::ThrowErrNoIfNegative (::fork ());
    if (pid == 0) {
        /*
         * Very primitive code to detatch the console. No error checking cuz frankly we dont care.
         *
         * Possibly should close more descriptors?
         */
        for (int i = 0; i < 3; ++i) {
            ::close (i);
        }
        int id = ::open ("/dev/null", O_RDWR);
        (void)::dup2 (id, 0);
        (void)::dup2 (id, 1);
        (void)::dup2 (id, 2);

        // Avoid signals like SIGHUP when the terminal session ends as well as potentially SIGTTIN and SIGTTOU
        // @see http://stackoverflow.com/questions/8777602/why-must-detach-from-tty-when-writing-a-linux-daemon
        (void)::setsid ();

        // @todo - CONSIDER ALSO DOING -                 constexpr bool kCloseAllExtraneousFDsInChild_ = true;
        // check and extra closer

        // @todo - safer EXECVP like we did in ProcessRunner()!!!!
        int r   =   ::execvp (thisEXEPath.c_str (), std::addressof (*std::begin (useArgsV)));
        // no practical way to return this failure...
        // UNCLEAR if we want tod exit or _exit  () - avoiding static DTORS
        ::_exit (-1);
#if     qPlatform_AIX
        return 0;       // silence bogus compiler warnings
#endif
    }
    else {
        return pid;
    }
#elif   qPlatform_Windows
    PROCESS_INFORMATION processInfo {};
    processInfo.hProcess = INVALID_HANDLE_VALUE;
    processInfo.hThread = INVALID_HANDLE_VALUE;
    Finally cleanup = [&processInfo]() {
        SAFE_HANDLE_CLOSER_ (&processInfo.hProcess);
        SAFE_HANDLE_CLOSER_ (&processInfo.hThread);
    };

    STARTUPINFO startInfo {};
    startInfo.cb = sizeof (startInfo);
    startInfo.hStdInput = INVALID_HANDLE_VALUE;
    startInfo.hStdOutput = INVALID_HANDLE_VALUE;
    startInfo.hStdError = INVALID_HANDLE_VALUE;
    startInfo.dwFlags |= STARTF_USESTDHANDLES;

    DWORD   createProcFlags =   0;
    createProcFlags |= CREATE_NO_WINDOW;
    createProcFlags |= NORMAL_PRIORITY_CLASS;
    createProcFlags |= DETACHED_PROCESS;
    {
        bool    bInheritHandles     =   true;
        TCHAR   cmdLineBuf[32768];          // crazy MSFT definition! - why this should need to be non-const!
        cmdLineBuf[0] = '\0';
        for (String i : useArgs) {
            //quickie/weak impl...
            if (cmdLineBuf[0] != '\0') {
                Characters::CString::Cat (cmdLineBuf, NEltsOf (cmdLineBuf), SDKSTR(" "));
            }
            Characters::CString::Cat (cmdLineBuf, NEltsOf (cmdLineBuf), i.AsSDKString ().c_str ());
        }
        Execution::Platform::Windows::ThrowIfFalseGetLastError (
            ::CreateProcess (executable.AsSDKString ().c_str (), cmdLineBuf, nullptr, nullptr, bInheritHandles, createProcFlags, nullptr, nullptr, &startInfo, &processInfo)
        );
    }
    return processInfo.dwProcessId;
#endif
}

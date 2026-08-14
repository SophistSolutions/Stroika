/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************* Execution::ProcessRunner::Exception ************************
     ********************************************************************************
     */
    inline ProcessRunner::Exception::Exception (const String& failureMessage, const optional<String>& stderrFragment,
                                                const optional<ExitStatusType>& wExitStatus, const optional<SignalID>& wTermSig)
        : inherited{mkMsg_ (failureMessage, stderrFragment, wExitStatus, wTermSig)}
        , fFailureMessage{failureMessage}
        , fStderrFragment{stderrFragment}
        , fExitStatus{wExitStatus}
        , fTermSignal{wTermSig}
    {
    }

    /*
     ********************************************************************************
     *************************** Execution::ProcessRunner ***************************
     ********************************************************************************
     */
#if qCompilerAndStdLib_DefaultMemberInitializerNeededEnclosingForDefaultFunArg_Buggy
    inline ProcessRunner::ProcessRunner (const filesystem::path& executable, const CommandLine& args)
        : ProcessRunner{executable, args, Options{}}
    {
    }
    inline ProcessRunner::ProcessRunner (const CommandLine& args)
        : ProcessRunner{args, Options{}}
    {
    }
    inline ProcessRunner::ProcessRunner (const String& commandLine)
        : ProcessRunner{commandLine, Options{}}
    {
    }
#endif
    inline ProcessRunner::ProcessRunner (const filesystem::path& executable, const CommandLine& args, const Options& o)
        : fExecutable_{executable}
        , fArgs_{args}
        , fOptions_{o}
    {
    }
    inline ProcessRunner::ProcessRunner (const CommandLine& args, const Options& o)
        : fArgs_{args}
        , fOptions_{o}
    {
    }
    inline ProcessRunner::ProcessRunner (const filesystem::path& executable, const CommandLine& args, const Streams::InputStream::Ptr<byte>& in,
                                         const Streams::OutputStream::Ptr<byte>& out, const Streams::OutputStream::Ptr<byte>& error)
        : fExecutable_{executable}
        , fArgs_{args}
        , fStdIn_{in}
        , fStdOut_{out}
        , fStdErr_{error}
    {
    }
    inline ProcessRunner::ProcessRunner (const CommandLine& args, const Streams::InputStream::Ptr<byte>& in,
                                         const Streams::OutputStream::Ptr<byte>& out, const Streams::OutputStream::Ptr<byte>& error)
        : fExecutable_{}
        , fArgs_{args}
        , fStdIn_{in}
        , fStdOut_{out}
        , fStdErr_{error}
    {
    }
    inline CommandLine ProcessRunner::GetCommandLine () const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fArgs_;
    }
    inline void ProcessRunner::SetCommandLine (const CommandLine& args)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fArgs_ = args;
    }
    inline optional<filesystem::path> ProcessRunner::GetWorkingDirectory () const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fOptions_.fWorkingDirectory;
    }
    inline void ProcessRunner::SetWorkingDirectory (const optional<filesystem::path>& d)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fOptions_.fWorkingDirectory = d;
    }
    inline auto ProcessRunner::GetOptions () const -> Options
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fOptions_;
    }
    inline void ProcessRunner::SetOptions (const Options& o)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fOptions_ = o;
    }
    inline Streams::InputStream::Ptr<byte> ProcessRunner::GetStdIn () const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fStdIn_;
    }
    inline void ProcessRunner::SetStdIn (const Streams::InputStream::Ptr<byte>& in)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fStdIn_ = in;
    }
    inline Streams::OutputStream::Ptr<byte> ProcessRunner::GetStdOut () const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareWriteContext{fThisAssertExternallySynchronized_};
        return fStdOut_;
    }
    inline void ProcessRunner::SetStdOut (const Streams::OutputStream::Ptr<byte>& out)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fStdOut_ = out;
    }
    inline Streams::OutputStream::Ptr<byte> ProcessRunner::GetStdErr () const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fStdErr_;
    }
    inline void ProcessRunner::SetStdErr (const Streams::OutputStream::Ptr<byte>& err)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fStdErr_ = err;
    }

    /*
     ********************************************************************************
     ******************* Execution::ProcessRunner::BackgroundProcess ****************
     ********************************************************************************
     */
    inline optional<ProcessRunner::ProcessResultType> ProcessRunner::BackgroundProcess::GetProcessResult () const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
        AssertNotNull (fRep_);
        AssertNotNull (fRep_->fDetailedRunnableRep_);
        return fRep_->fDetailedRunnableRep_->fProcessResult;
    }
    inline optional<pid_t> ProcessRunner::BackgroundProcess::GetChildProcessID () const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
        AssertNotNull (fRep_);
        AssertNotNull (fRep_->fDetailedRunnableRep_);
        return fRep_->fDetailedRunnableRep_->fRunningPID;
    }

    /// DEPRECATED

    [[deprecated ("Since Stroika v3.0d13 - use ProcessRunner{Options{.fDetached=true}")]] inline pid_t
    DetachedProcessRunner (const filesystem::path& executable, const Containers::Sequence<String>& args)
    {
        ProcessRunner                    pr{executable, CommandLine{args}, ProcessRunner::Options{.fDetached = true}};
        ProcessRunner::BackgroundProcess bp = pr.RunInBackground ();
        bp.WaitForStarted ();
        return Memory::ValueOf (bp.GetChildProcessID ());
    }
    [[deprecated ("Since Stroika v3.0d13 - use ProcessRunner{Options{.fDetached=true}")]] inline pid_t DetachedProcessRunner (const String& commandLine)

    {
        ProcessRunner                    pr{CommandLine{commandLine}, ProcessRunner::Options{.fDetached = true}};
        ProcessRunner::BackgroundProcess bp = pr.RunInBackground ();
        bp.WaitForStarted ();
        return Memory::ValueOf (bp.GetChildProcessID ());
    }

}

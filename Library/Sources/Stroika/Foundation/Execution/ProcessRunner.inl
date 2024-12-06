/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     *************************** Execution::ProcessRunner ***************************
     ********************************************************************************
     */
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
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fArgs_;
    }
    inline void ProcessRunner::SetCommandLine (const CommandLine& args)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fArgs_ = args;
    }
    inline optional<filesystem::path> ProcessRunner::GetWorkingDirectory () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fOptions_.fWorkingDirectory;
    }
    inline void ProcessRunner::SetWorkingDirectory (const optional<filesystem::path>& d)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fOptions_.fWorkingDirectory = d;
    }
    inline auto ProcessRunner::GetOptions () const -> Options
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fOptions_;
    }
    inline void ProcessRunner::SetOptions (const Options& o)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fOptions_ = o;
    }
    inline Streams::InputStream::Ptr<byte> ProcessRunner::GetStdIn () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fStdIn_;
    }
    inline void ProcessRunner::SetStdIn (const Streams::InputStream::Ptr<byte>& in)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fStdIn_ = in;
    }
    inline Streams::OutputStream::Ptr<byte> ProcessRunner::GetStdOut () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareWriteContext{fThisAssertExternallySynchronized_};
        return fStdOut_;
    }
    inline void ProcessRunner::SetStdOut (const Streams::OutputStream::Ptr<byte>& out)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fStdOut_ = out;
    }
    inline Streams::OutputStream::Ptr<byte> ProcessRunner::GetStdErr () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fStdErr_;
    }
    inline void ProcessRunner::SetStdErr (const Streams::OutputStream::Ptr<byte>& err)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fStdErr_ = err;
    }

}

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
    inline ProcessRunner::ProcessRunner (const String& commandLine, const Streams::InputStream::Ptr<byte>& in,
                                         const Streams::OutputStream::Ptr<byte>& out, const Streams::OutputStream::Ptr<byte>& error)
        : ProcessRunner{commandLine.ContainsAny ({'\'', '\"', '<', '>', '|', '$', '{', '}'}) ? CommandLine{kDefaultShell, commandLine}
                                                                                             : CommandLine{commandLine},
                        in, out, error}
    {
    }

}

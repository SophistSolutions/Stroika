/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     *************************** Execution::ProcessRunner ***************************
     ********************************************************************************
     */
    inline ProcessRunner::ProcessRunner (const String& commandLine, const Streams::InputStream::Ptr<byte>& in,
                                         const Streams::OutputStream::Ptr<byte>& out, const Streams::OutputStream::Ptr<byte>& error)
        : ProcessRunner{CommandLine{commandLine}, in, out, error}
    {
    }

}

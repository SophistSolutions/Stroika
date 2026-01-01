/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::WebService::Server {

    [[deprecated ("Since Stroika v3.0d12 - use Response&")]] inline void
    WriteDocsPage (Response* response, const Sequence<WebServiceMethodDescription>& operations, const DocsOptions& docsOptions = {})
    {
        WriteDocsPage (*response, operations, docsOptions);
    }

}

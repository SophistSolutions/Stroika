/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_FileSystemRequestHandler_inl_
#define _Stroika_Frameworks_WebServer_FileSystemRequestHandler_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::WebServer {

    // DEPRECATED
    inline FileSystemRequestHandler::FileSystemRequestHandler (const filesystem::path& filesystemRoot, const optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames)
        : FileSystemRequestHandler{filesystemRoot, Options{urlPrefix2Strip, defaultIndexFileNames}}
    {
    }

}

#endif /*_Stroika_Frameworks_WebServer_FileSystemRequestHandler_inl_*/

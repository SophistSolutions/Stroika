/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Request_inl_
#define _Stroika_Frameworks_WebServer_Request_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ***************************** WebServer::Request *******************************
     ********************************************************************************
     */
    inline Streams::InputStream<byte>::Ptr Request::GetInputStream ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fInputStream_;
    }

}

#endif /*_Stroika_Frameworks_WebServer_Request_inl_*/

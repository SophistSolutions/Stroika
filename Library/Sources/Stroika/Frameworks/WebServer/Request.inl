/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ***************************** WebServer::Request *******************************
     ********************************************************************************
     */
    inline Streams::InputStream::Ptr<byte> Request::GetInputStream ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
        return fInputStream_;
    }

}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ***************************** WebServer::Request *******************************
     ********************************************************************************
     */
    inline Request::Request (Request&& src)
        : Request{src.fInputStream_}
    {
        fBodyInputStream_ = move (src.fBodyInputStream_);
        fBody_            = move (src.fBody_);
    }
    inline Request& Request::operator= (Request&& rhs) noexcept
    {
        inherited::operator= (move (rhs));
        fBodyInputStream_ = move (rhs.fBodyInputStream_);
        fBody_            = move (rhs.fBody_);
        return *this;
    }
    inline Streams::InputStream::Ptr<byte> Request::GetInputStream ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
        return fInputStream_;
    }

}

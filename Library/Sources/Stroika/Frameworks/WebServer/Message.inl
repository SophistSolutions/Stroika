/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ******************************** WebServer::Message ****************************
     ********************************************************************************
     */
    inline Message::Message (Message&& src) noexcept
        : Message{move (src.fRequest_), move (src.fResponse_), src.fPeerAddress_}
    {
    }
    inline Message& Message::operator= (Message&& rhs) noexcept
    {
        this->fPeerAddress_ = rhs.fPeerAddress_;
        this->fRequest_     = move (rhs.fRequest_);
        this->fResponse_    = move (rhs.fResponse_);
        return *this;
    }

}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ************************ IO::Network::HTTP::Response ***************************
     ********************************************************************************
     */
    inline Response::Response (Response&& src)
        : Response{}
    {
        fStatusAndOverrideReason_ = move (src.fStatusAndOverrideReason_);
        fHeaders_                 = move (src.fHeaders_);
    }
    inline Response& Response::operator= (Response&& rhs) noexcept
    {

        fStatusAndOverrideReason_ = move (rhs.fStatusAndOverrideReason_);
        fHeaders_                 = move (rhs.fHeaders_);
        return *this;
    }
}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ********************************* HTTP::Request ********************************
     ********************************************************************************
     */
    inline Request::Request (Request&& src)
        : Request{}
    {
        fHTTPVersion_ = move (src.fHTTPVersion_);
        fMethod_      = move (src.fMethod_);
        fURL_         = move (src.fURL_);
        fHeaders_     = move (src.fHeaders_);
    }

    inline Request& Request::operator= (Request&& src) noexcept
    {
        fHTTPVersion_ = move (src.fHTTPVersion_);
        fMethod_      = move (src.fMethod_);
        fURL_         = move (src.fURL_);
        fHeaders_     = move (src.fHeaders_);
        return *this;
    }

}

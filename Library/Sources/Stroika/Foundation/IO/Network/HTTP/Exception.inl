/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ********************************* HTTP::Exception ******************************
     ********************************************************************************
     */
    inline Status Exception::GetStatus () const
    {
        return fStatus_;
    }
    inline bool Exception::IsClientError () const
    {
        return HTTP::IsClientError (fStatus_);
    }
    inline bool Exception::IsServerError () const
    {
        return HTTP::IsServerError (fStatus_);
    }

}

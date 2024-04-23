/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ************************************ HTTP::Exception ***************************
     ********************************************************************************
     */
    inline Status Exception::GetStatus () const
    {
        return fStatus_;
    }
    inline bool Exception::IsHTTPStatusOK (Status status)
    {
        return 200 <= status and status <= 299;
    }
    inline bool Exception::IsClientError () const
    {
        return 400 <= GetStatus () and GetStatus () <= 499;
    }
    inline bool Exception::IsServerError () const
    {
        return 500 <= GetStatus () and GetStatus () <= 599;
    }

}

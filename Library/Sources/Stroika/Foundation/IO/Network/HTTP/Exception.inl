/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Exception_inl_
#define _Stroika_Foundation_IO_Network_HTTP_Exception_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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

}
#endif /*_Stroika_Foundation_IO_Network_HTTP_Exception_inl_*/

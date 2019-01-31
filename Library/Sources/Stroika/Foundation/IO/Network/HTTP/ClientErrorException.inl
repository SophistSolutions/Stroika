/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_ClientErrorException_inl_
#define _Stroika_Foundation_IO_Network_HTTP_ClientErrorException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ************************* HTTP::ClientErrorException ***************************
     ********************************************************************************
     */
    inline ClientErrorException::ClientErrorException (Status status, const String& reason)
        : inherited (status, reason)
    {
        Require (400 <= status and status < 500);
    }
    inline ClientErrorException::ClientErrorException (const String& reason)
        : ClientErrorException (StatusCodes::kBadRequest, reason)
    {
    }
    template <typename FUNCTION>
    inline void ClientErrorException::TreatExceptionsAsClientError (FUNCTION f)
    {
        try {
            f ();
        }
        catch (...) {
            Execution::Throw (ClientErrorException (current_exception ()));
        }
    }

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_ClientErrorException_inl_*/

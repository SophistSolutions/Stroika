/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ************************* HTTP::ClientErrorException ***************************
     ********************************************************************************
     */
    inline ClientErrorException::ClientErrorException (Status status, const String& reason)
        : inherited{status, reason}
    {
        Require (400 <= status and status < 500);
    }
    inline ClientErrorException::ClientErrorException (const String& reason)
        : ClientErrorException{StatusCodes::kBadRequest, reason}
    {
    }
    template <typename FUNCTION, typename RESULT_TYPE>
    inline RESULT_TYPE ClientErrorException::TreatExceptionsAsClientError (FUNCTION&& f)
    {
        try {
            return forward<FUNCTION> (f) ();
        }
        catch (...) {
            Execution::Throw (ClientErrorException{current_exception ()});
        }
    }

}

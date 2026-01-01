/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/Trace.h"

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
    template <invocable FUNCTION, typename RESULT_TYPE>
    inline RESULT_TYPE ClientErrorException::TreatExceptionsAsClientError (FUNCTION&& f)
    {
        try {
            return forward<FUNCTION> (f) ();
        }
        catch (const ClientErrorException&) {
            Execution::ReThrow (); // if it already is one, just rethrow
        }
        catch (...) {
            using namespace Characters;
            DbgTrace ("Translating exception {} to ClientErrorException"_f, current_exception ());
            Execution::Throw (ClientErrorException{current_exception ()});
        }
    }

}

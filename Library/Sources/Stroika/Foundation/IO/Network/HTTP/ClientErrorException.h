/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_ClientErrorException_h_
#define _Stroika_Foundation_IO_Network_HTTP_ClientErrorException_h_ 1

#include "../../../StroikaPreComp.h"

#include "Exception.h"

/*
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Foundation::Characters::String;

    /**
     *  \brief  ClientErrorException is to capture exceptions caused by a bad (e.g ill-formed) request.
     *
     *      This is typically what you want to throw (or translate to one of these) an exception in the webserver framework
     *      that should be treated as an error in the client request, and not a server failure (assignment of blame).
     *
     *  TODO:
     *      @todo consider adding a 'based-on' field so that original exception gets copied around.
     *
     *      @todo consider smarter mapping of basedOnInnerException to status codes - like TimeoutException to 408 Request Timeout
     */
    class ClientErrorException : public Exception {
    private:
        using inherited = Exception;

    public:
        /*
         * If no reason is given, a default is generated based on the status code, or other arguments.
         *
         *  If given an 'inner exception' to be based on, this takes that exception to form a message, and automatically
         *  generates a status (unless provided explicitly).
         *
         *  \req 400 <= status and status < 500
         */
        ClientErrorException (Status status = StatusCodes::kBadRequest, const String& reason = String{});
        ClientErrorException (const String& reason);
        ClientErrorException (const Exception& basedOnInnerException);
        ClientErrorException (Status status, const Exception& basedOnInnerException);
        ClientErrorException (const exception& basedOnInnerException);
        ClientErrorException (Status status, const exception& basedOnInnerException);
        ClientErrorException (const exception_ptr& basedOnInnerException);
        ClientErrorException (Status status, const exception_ptr& basedOnInnerException);

        /**
         *  Utility to map any functions thrown in the given (typically lambda) into a ClientErrorExcpetion
         */
        template <typename FUNCTION, typename RESULT_TYPE = std::invoke_result_t<FUNCTION>>
        static RESULT_TYPE TreatExceptionsAsClientError (FUNCTION f);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ClientErrorException.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_ClientErrorException_h_*/

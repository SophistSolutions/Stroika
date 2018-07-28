/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ClientErrorException_h_
#define _Stroika_Framework_WebServer_ClientErrorException_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/IO/Network/HTTP/Exception.h"

/*
 * TODO:
 *      @todo   Takes optionally orig exception
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::IO::Network;

    using Characters::String;
    using IO::Network::HTTP::Status;

    /**
     *  \brief  ClientErrorException is to capture exceptions caused by a bad (e.g ill-formed) request.
     *
     *  Idea is that you would have places in the code to translate and pass along
     *  ClientErrorException and that would generate a 400 class response instead of 500
     */
    struct ClientErrorException : public HTTP::Exception {
    private:
        using inherited = HTTP::Exception;

    public:
        /**
         *  \req 400 <= httpStatus and httpStatus < 500
         */
        ClientErrorException (Status httpStatus);
        ClientErrorException (const String& message, Status httpStatus = HTTP::StatusCodes::kBadRequest);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ClientErrorException.inl"

#endif /*_Stroika_Framework_WebServer_ClientErrorException_h_*/

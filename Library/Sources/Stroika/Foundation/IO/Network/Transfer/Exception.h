/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Exception_h_
#define _Stroika_Foundation_IO_Network_Transfer_Exception_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/Transfer/Response.h"

/**
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::IO::Network::Transfer {

    /**
     */
    struct Exception : HTTP::Exception {
    public:
        Exception (const Response& response);

    public:
        nonvirtual Response GetResponse () const;

    private:
        Response fResponse_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exception.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Exception_h_*/

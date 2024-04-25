/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

/*
 ********************************************************************************
 ***************************** Transfer::Exception ******************************
 ********************************************************************************
 */
Exception::Exception (const Response& response)
    : HTTP::Exception{response.GetStatus ()}
    , fResponse_{response}
{
}

Response Exception::GetResponse () const
{
    return fResponse_;
}

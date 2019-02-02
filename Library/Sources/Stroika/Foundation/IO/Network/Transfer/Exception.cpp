/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../HTTP/Headers.h"

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
    : HTTP::Exception (response.GetStatus ())
    , fResponse_ (response)
{
}

Response Exception::GetResponse () const
{
    return fResponse_;
}

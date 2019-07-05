/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Execution/RequiredComponentMissingException.h"

#include "Client.h"

#if qHasFeature_LibCurl
#include "Client_libcurl.h"
#endif
#if qHasFeature_WinHTTP
#include "Client_WinHTTP.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

/*
 ********************************************************************************
 ************************ Transfer::CreateConnection ****************************
 ********************************************************************************
 */
Connection::Ptr Transfer::CreateConnection (const Connection::Options& options)
{
#if qHasFeature_LibCurl
    return Connection_LibCurl::New (options);
#endif
#if qHasFeature_WinHTTP
    return Connection_WinHTTP::New (options);
#endif
    Execution::Throw (Execution::RequiredComponentMissingException (Execution::RequiredComponentMissingException::kIONetworkClientFactory));
}

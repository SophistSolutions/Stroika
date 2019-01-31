/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/ToString.h"

#include "ClientErrorException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network::HTTP;

/*
 ********************************************************************************
 ************************** ClientErrorException ********************************
 ********************************************************************************
 */
ClientErrorException::ClientErrorException (const StringException& basedOnInnerException)
    : ClientErrorException (400, Characters::ToString (basedOnInnerException))
{
}

ClientErrorException::ClientErrorException (Status status, const StringException& basedOnInnerException)
    : ClientErrorException (status, Characters::ToString (basedOnInnerException))
{
}

ClientErrorException::ClientErrorException (const exception& basedOnInnerException)
    : ClientErrorException (400, Characters::ToString (basedOnInnerException))
{
}

ClientErrorException::ClientErrorException (Status status, const exception& basedOnInnerException)
    : ClientErrorException (status, Characters::ToString (basedOnInnerException))
{
}

ClientErrorException::ClientErrorException (const exception_ptr& basedOnInnerException)
    : ClientErrorException (400, Characters::ToString (basedOnInnerException))
{
}

ClientErrorException::ClientErrorException (Status status, const exception_ptr& basedOnInnerException)
    : ClientErrorException (status, Characters::ToString (basedOnInnerException))
{
}

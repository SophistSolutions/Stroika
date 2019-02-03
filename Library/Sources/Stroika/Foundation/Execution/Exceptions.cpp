/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdio>

#include "../IO/FileAccessException.h"

#include "Throw.h"
#include "TimeOutException.h"

#include "ErrNoException.h"

using namespace Stroika;
using namespace Stroika::Foundation;

using namespace Characters;
using namespace Execution;

using Debug::TraceContextBumper;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ***************************** SystemException **********************************
 ********************************************************************************
 */
Characters::String SystemException::mkMsg_ (error_code errCode)
{
    return Characters::String::FromNarrowSDKString (errCode.message ());
}

Characters::String SystemException::mkMsg_ (error_code errCode, const Characters::String& message)
{
    return message + L": "sv + mkMsg_ (errCode);
}

void SystemException::ThrowPOSIXErrNo (int errNo)
{
	// @todo see errno_ErrorException::Throw () for additional needed mappings
    switch (static_cast<errc> (errNo)) {
        case errc::not_enough_memory: {
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
            DbgTrace ("SystemException::ThrowPOSIXErrNo (0x%x) - translating errc::not_enough_memory to bad_alloc", errNo);
#endif
            Execution::Throw (bad_alloc ());
        } break;
        default: {
            Throw (SystemException (errNo, generic_category ()));
        } break;
    }
}

void SystemException::ThrowSystemErrNo (int sysErr)
{
	// @todo see Execution::Platform::Windows::Exception::Throw - many more translations needed and this one needs testing
    error_code ec{sysErr, system_category ()};
    if (ec == errc::not_enough_memory) {
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
		DbgTrace ("SystemException::ThrowSystemErrNo (0x%x) - translating errc::not_enough_memory to bad_alloc", sysErr);
#endif
        Execution::Throw (bad_alloc ());
    }
    Throw (SystemException (ec));
}

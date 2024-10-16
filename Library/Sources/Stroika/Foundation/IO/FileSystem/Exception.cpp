/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 ******************************* FileSystem::Exception **************************
 ********************************************************************************
 */
Characters::String FileSystem::Exception::mkMsg_ (error_code errCode, const path& p1, const path& p2)
{
    StringBuilder sb = Execution::Private_::SystemErrorExceptionPrivate_::mkMsg_ (errCode);
    if (not p1.empty ()) {
        sb << " {"sv << String{p1.wstring ()}.LimitLength (25, StringShorteningPreference::ePreferKeepRight) << "}"sv;
    }
    if (not p2.empty ()) {
        sb << " {"sv << String{p2.wstring ()}.LimitLength (25, StringShorteningPreference::ePreferKeepRight) << "}"sv;
    }
    return sb;
}

Characters::String FileSystem::Exception::mkMsg_ (error_code errCode, const Characters::String& message, const path& p1, const path& p2)
{
    StringBuilder sb = Execution::Private_::SystemErrorExceptionPrivate_::mkCombinedMsg_ (errCode, message);
    if (not p1.empty ()) {
        sb << " {"sv << String{p1.wstring ()}.LimitLength (25, StringShorteningPreference::ePreferKeepRight) << "}"sv;
    }
    if (not p2.empty ()) {
        sb << " {"sv << String{p2.wstring ()}.LimitLength (25, StringShorteningPreference::ePreferKeepRight) << "}"sv;
    }
    return sb;
}

void Exception::ThrowPOSIXErrNo (errno_t errNo, const path& p1, const path& p2)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"IO::FileSystem::Exception::ThrowPOSIXErrNo", "sysErr={}, p1={}, p2={}"_f, errNo, p1, p2};
#endif
    Require (errNo != 0);
#if qPlatform_POSIX
    error_code ec{errNo, system_category ()};
#else
    error_code ec{errNo, generic_category ()};
#endif
    Throw (Exception{ec, p1, p2});
}

void Exception::ThrowSystemErrNo (int sysErr, const path& p1, const path& p2)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"IO::FileSystem::Exception::ThrowSystemErrNo", "sysErr={}, p1={}, p2={}"_f, sysErr, p1, p2};
#endif
    Require (sysErr != 0);
    error_code ec{sysErr, system_category ()};
    Throw (Exception{ec, p1, p2});
}

void Exception::ThrowSystemErrNo (const path& p1, const path& p2)
{
#if qPlatform_POSIX
    ThrowSystemErrNo (errno, p1, p2);
#elif qPlatform_Windows
    ThrowSystemErrNo (::GetLastError (), p1, p2);
#endif
}

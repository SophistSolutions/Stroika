/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/StringBuilder.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

/*
 ********************************************************************************
 ******************************* FileSystem::Exception **************************
 ********************************************************************************
 */
Characters::String FileSystem::Exception::mkMsg_ (error_code errCode, const path& p1, const path& p2)
{
    StringBuilder sb = Execution::Private_::SystemErrorExceptionPrivate_::mkMsg_ (errCode);
    if (not p1.empty ()) {
        sb += L" {" + String (p1.wstring ()).LimitLength (25, false) + L"}";
    }
    if (not p2.empty ()) {
        sb += L" {" + String (p2.wstring ()).LimitLength (25, false) + L"}";
    }
    return sb.str ();
}

Characters::String FileSystem::Exception::mkMsg_ (error_code errCode, const Characters::String& message, const path& p1, const path& p2)
{
    StringBuilder sb = Execution::Private_::SystemErrorExceptionPrivate_::mkCombinedMsg_ (errCode, message);
    if (not p1.empty ()) {
        sb += L" {" + String (p1.wstring ()).LimitLength (25, false) + L"}";
    }
    if (not p2.empty ()) {
        sb += L" {" + String (p2.wstring ()).LimitLength (25, false) + L"}";
    }
    return sb.str ();
}

void Exception::ThrowPOSIXErrNo (errno_t errNo, const path& p1, const path& p2)
{
#if Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContenxtBumper tctx (L"Exception::ThrowPOSIXErrNo (%d)", errNo);
#endif
    Require (errNo != 0);
#if qPlatform_POSIX
    error_code ec{errNo, system_category ()};
#else
    error_code ec{errNo, generic_category ()};
#endif
    Throw (Exception (ec, p1, p2));
}

void Exception::ThrowSystemErrNo (int sysErr, const path& p1, const path& p2)
{
#if Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContenxtBumper tctx (L"Exception::ThrowSystemErrNo (%d)", sysErr);
#endif
    Require (sysErr != 0);
    error_code ec{sysErr, system_category ()};
    Throw (Exception (ec, p1, p2));
}

#if qHasFeature_boost
Exception TranslateBoostFilesystemException2StandardExceptions (const boost::filesystem::filesystem_error& e)
{
    // translate error code
    auto mapCat = [&]() -> const error_category& {
        if (e.code ().category () == boost::system::generic_category ()) {
            return generic_category ();
        }
        else if (e.code ().category () == boost::system::system_category ()) {
            return system_category ();
        }
        else {
            class MapBoostCategory_ : public error_category {
            public:
                MapBoostCategory_ ()
                {
                }
                virtual const char* name () const noexcept
                {
                    return "Unknown boost error";
                }
                virtual string message ([[maybe_unused]] int e) const
                {
                    return "Unknown boost error";
                }
            };
            static MapBoostCategory_ sCat_; // cannot capture data in these categories cuz can only be one - no biggie - if this happens, identify more boost category mappings
            return sCat_;
        }
    };
    error_code ec{e.code ().value (), mapCat ()};
    return Exception (ec, String::FromNarrowSDKString (e.what ()), path (e.path1 ().wstring ()), path (e.path2 ().wstring ()));
}
#endif

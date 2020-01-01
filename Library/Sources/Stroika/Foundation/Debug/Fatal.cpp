/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/ToString.h"

#include "BackTrace.h"
#include "Debugger.h"
#include "Trace.h"

#include "Fatal.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Debug;

namespace {
    void _DefaultFatalErrorHandler_ (const SDKChar* msg) noexcept
    {
        DbgTrace (SDKSTR ("Fatal Error %s encountered"), msg);
        if (auto exc = current_exception ()) {
            DbgTrace (L"Uncaught exception", Characters::ToString (exc).c_str ());
        }
#if qDefaultTracingOn
        {
            wstring tmp{Debug::BackTrace ()};
            if (not tmp.empty ()) {
                DbgTrace (L"BackTrace: %s", tmp.c_str ());
            }
        }
#endif
        Debug::DropIntoDebuggerIfPresent ();
        abort ();
    }
    void (*sFatalErrorHandler_) (const SDKChar* msg) noexcept = nullptr; // our handlers can never get called until  RegisterDefaultFatalErrorHandlers () is called

    void TerminateHandler_ ()
    {
        (sFatalErrorHandler_) (SDKSTR ("std::terminate () called"));
    }
#if qPlatform_Windows
    void PurecallHandler_ ()
    {
        (sFatalErrorHandler_) (SDKSTR ("purecall_handler_ () called"));
    }
#endif
}

void Debug::RegisterDefaultFatalErrorHandlers (void (*fatalErrorHandler) (const SDKChar* msg) noexcept)
{
    sFatalErrorHandler_ = (fatalErrorHandler == nullptr) ? _DefaultFatalErrorHandler_ : fatalErrorHandler;
    set_terminate (TerminateHandler_);
#if qPlatform_Windows
    // Not C++ standard - just msvc error call
    (void)_set_purecall_handler (PurecallHandler_);
#endif
}

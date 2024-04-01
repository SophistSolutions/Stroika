/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Stroika/Foundation/Characters/SDKChar.h"
#include "Stroika/Foundation/Characters/ToString.h"

#include "BackTrace.h"
#include "Debugger.h"
#include "Trace.h"

#include "Fatal.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;

namespace {
    void _DefaultFatalErrorHandler_ ([[maybe_unused]] const SDKChar* msg) noexcept
    {
        DbgTrace ("Fatal Error {} encountered"_f, String::FromSDKString (msg));
        if (auto exc = current_exception ()) {
            DbgTrace ("Uncaught exception: {}"_f, Characters::ToString (exc));
        }
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        {
            wstring tmp{Debug::BackTrace::Capture ()};
            if (not tmp.empty ()) {
                DbgTrace ("BackTrace: {}"_f, tmp);
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

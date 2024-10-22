/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/SDKChar.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/BackTrace.h"
#include "Stroika/Foundation/Debug/Debugger.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Fatal.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ************************ Debug::DefaultFatalErrorHandler ***********************
 ********************************************************************************
 */
void Debug::DefaultFatalErrorHandler ([[maybe_unused]] const SDKChar* msg) noexcept
{
    DbgTrace ("Fatal Error {} encountered"_f, String::FromSDKString (msg));
    if (auto exc = current_exception ()) {
        DbgTrace ("Uncaught exception: {}"_f, exc);
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

/*
 ********************************************************************************
 ******************* Debug::RegisterDefaultFatalErrorHandlers *******************
 ********************************************************************************
 */
namespace {
    void (*sFatalErrorHandler_) (const SDKChar* msg) noexcept = nullptr; // our handlers can never get called until  RegisterDefaultFatalErrorHandlers () is called, so nullptr better (BSS storage in case never used)
    void TerminateHandler_ ()
    {
        (sFatalErrorHandler_) (SDKSTR ("std::terminate () called"));
    }
#if qStroika_Foundation_Common_Platform_Windows
    void PurecallHandler_ ()
    {
        (sFatalErrorHandler_) (SDKSTR ("purecall_handler_ () called"));
    }
#endif
}

void Debug::RegisterDefaultFatalErrorHandlers (void (*fatalErrorHandler) (const SDKChar* msg) noexcept)
{
    RequireNotNull (fatalErrorHandler);
    sFatalErrorHandler_ = fatalErrorHandler;
    set_terminate (TerminateHandler_);
#if qStroika_Foundation_Common_Platform_Windows
    // Not C++ standard - just msvc error call
    (void)_set_purecall_handler (PurecallHandler_);
#endif
}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Debugger.h"
#include    "Trace.h"

#include    "Fatal.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Debug;


namespace   {
    void    _DefaultFatalErrorHandler_ (const SDKChar* msg)
    {
        DbgTrace (SDKSTR ("Fatal Error %s encountered"), msg);
        if (auto exc = std::current_exception()) {
            DbgTrace ("uncaught exception");
        }
        Debug::DropIntoDebuggerIfPresent ();
        abort ();
    }
    void (*sFatalErrorHandler_) (const SDKChar* msg)      =   nullptr;        // our handlers can never get called until  RegisterDefaultFatalErrorHandlers () is called

    void    TerminateHandler_ ()
    {
        (sFatalErrorHandler_) (SDKSTR ("std::terminate () called"));
    }
    void    UnexpectedHandler_ ()
    {
        (sFatalErrorHandler_) (SDKSTR ("std::unexpected () called"));
    }
#if     qPlatform_Windows
    void    PurecallHandler_ ()
    {
        (sFatalErrorHandler_) (SDKSTR ("purecall_handler_ () called"));
    }
#endif
}





void    Debug::RegisterDefaultFatalErrorHandlers (void (*fatalErrorHandler) (const SDKChar* msg))
{
    sFatalErrorHandler_ = (fatalErrorHandler == nullptr) ? _DefaultFatalErrorHandler_ : fatalErrorHandler;
    set_terminate (TerminateHandler_);
    set_unexpected (UnexpectedHandler_);
#if     qPlatform_Windows
    // Not C++ standard - just msvc error call
    (void)_set_purecall_handler (PurecallHandler_);
#endif
}

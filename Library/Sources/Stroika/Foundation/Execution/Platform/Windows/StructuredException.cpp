/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Characters/String.h"
#include "../../../Common/Immortalize.h"
#include "../../../Configuration/Common.h"
#include "../../../Containers/Common.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/Exceptions.h"
#include "../../../Time/Realtime.h"

#include "StructuredException.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform;
using namespace Stroika::Foundation::Execution::Platform::Windows;

namespace {
    class StructuredException_error_category_ : public error_category {
    public:
        virtual const char* name () const noexcept override
        {
            return "Windows Structured Exception error";
        }
        virtual error_condition default_error_condition ([[maybe_unused]] int ev) const noexcept override
        {
            // @todo - not sure how todo this - except by defining new conditions
            //switch (ev) {
            //}
            return error_condition (errc::bad_message); // no idea what to return here
        }
        virtual string message (int u) const override
        {
            switch (u) {
                case EXCEPTION_ACCESS_VIOLATION:
                    return "EXCEPTION_ACCESS_VIOLATION";
                case EXCEPTION_DATATYPE_MISALIGNMENT:
                    return "EXCEPTION_DATATYPE_MISALIGNMENT";
                case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
                case EXCEPTION_FLT_DENORMAL_OPERAND:
                    return "EXCEPTION_FLT_DENORMAL_OPERAND";
                case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                    return "EXCEPTION_FLT_DENORMAL_OPERAND";
                case EXCEPTION_FLT_INEXACT_RESULT:
                    return "EXCEPTION_FLT_INEXACT_RESULT";
                case EXCEPTION_FLT_INVALID_OPERATION:
                    return "EXCEPTION_FLT_INVALID_OPERATION";
                case EXCEPTION_FLT_OVERFLOW:
                    return "EXCEPTION_FLT_OVERFLOW";
                case EXCEPTION_FLT_STACK_CHECK:
                    return "EXCEPTION_FLT_STACK_CHECK";
                case EXCEPTION_FLT_UNDERFLOW:
                    return "EXCEPTION_FLT_UNDERFLOW";
                case EXCEPTION_INT_DIVIDE_BY_ZERO:
                    return "EXCEPTION_INT_DIVIDE_BY_ZERO";
                case EXCEPTION_INT_OVERFLOW:
                    return "EXCEPTION_INT_OVERFLOW";
                case EXCEPTION_PRIV_INSTRUCTION:
                    return "EXCEPTION_PRIV_INSTRUCTION";
                case EXCEPTION_IN_PAGE_ERROR:
                    return "EXCEPTION_IN_PAGE_ERROR";
                case EXCEPTION_ILLEGAL_INSTRUCTION:
                    return "EXCEPTION_ILLEGAL_INSTRUCTION";
                case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                    return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
                case EXCEPTION_STACK_OVERFLOW:
                    return "EXCEPTION_STACK_OVERFLOW";
                case EXCEPTION_INVALID_DISPOSITION:
                    return "EXCEPTION_INVALID_DISPOSITION";
                case EXCEPTION_GUARD_PAGE:
                    return "EXCEPTION_GUARD_PAGE";
                case EXCEPTION_INVALID_HANDLE:
                    return "EXCEPTION_INVALID_HANDLE";
                default: {
                    char buf[1024];
                    (void)::snprintf (buf, NEltsOf (buf), "#0x%x", u);
                    return buf;
                }
            }
        }
    };
}

/*
 ********************************************************************************
 *********** Platform::Windows::StructuredException_error_category **************
 ********************************************************************************
 */
const std::error_category& Execution::Platform::Windows::StructuredException_error_category () noexcept
{
    return Common::Immortalize<StructuredException_error_category_> ();
}

namespace {
    void trans_func_ (unsigned int u, EXCEPTION_POINTERS* pExp)
    {
        TraceContextBumper ctx ("{}::trans_func_");
        {
            // I wish I knew how to get a PROCNAME of where the caller was...
            DbgTrace ("u = 0x%x (%s)", u, StructuredException_error_category ().message (u).c_str ());
            if (pExp != nullptr) {
                if (pExp->ContextRecord != nullptr) {
                    TraceContextBumper ctx1 ("ContextRecord");
                    DbgTrace ("ContextRecord->ContextFlags = 0x%x", pExp->ContextRecord->ContextFlags);
                    DbgTrace ("ContextRecord->Dr0 = 0x%x", pExp->ContextRecord->Dr0);
#if qPlatform_Win32
                    DbgTrace ("ContextRecord->Esp = 0x%x", pExp->ContextRecord->Esp);
#endif
                }
                if (pExp->ExceptionRecord != nullptr) {
                    TraceContextBumper ctx1 ("ExceptionRecord");
                    DbgTrace ("ExceptionRecord->ExceptionAddress = 0x%x", pExp->ExceptionRecord->ExceptionAddress);
                    DbgTrace ("ExceptionRecord->ExceptionCode = 0x%x", pExp->ExceptionRecord->ExceptionCode);
                    DbgTrace ("ExceptionRecord->ExceptionFlags = 0x%x", pExp->ExceptionRecord->ExceptionFlags);
                    DbgTrace ("ExceptionRecord->NumberParameters = %d", pExp->ExceptionRecord->NumberParameters);
                }
            }
        }
        DbgTrace ("Translating it into a Platform::Windows::StructuredException::THROW()");
        Assert (false); // in case debug turned on, helpful to drop into the debugger here!
        Execution::Throw (Execution::SystemErrorException (u, StructuredException_error_category ()));
    }
}

/*
********************************************************************************
****** Platform::Windows::RegisterDefaultHandler_StructuredException ***********
********************************************************************************
*/
void Execution::Platform::Windows::RegisterDefaultHandler_StructuredException ()
{
    DISABLE_COMPILER_MSC_WARNING_START (4535) // disable warning cuz MSVC appears to offer no way to check if this function is available, and harmless to register if not available
    _set_se_translator (trans_func_);
    DISABLE_COMPILER_MSC_WARNING_END (4535)
}

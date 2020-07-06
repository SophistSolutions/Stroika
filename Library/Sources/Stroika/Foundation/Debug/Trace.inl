
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_Trace_inl_
#define _Stroika_Foundation_Debug_Trace_inl_ 1

#include <filesystem>

CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qTraceToFile, qTraceToFile);
CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qDefaultTracingOn, qDefaultTracingOn);

namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     ******************************* Trace::Emitter *********************************
     ********************************************************************************
     */
    template <typename CHARTYPE>
    inline void Emitter::EmitUnadornedText (const CHARTYPE* p)
    {
        DoEmit_ (p);
    }

    /*
     ********************************************************************************
     ******************************* TraceContextBumper *****************************
     ********************************************************************************
     */
    inline TraceContextBumper::TraceContextBumper () noexcept
#if qDefaultTracingOn
        : fDoEndMarker (false)
#endif
    {
#if qDefaultTracingOn
        IncCount_ ();
#endif
    }
#if !qDefaultTracingOn
    inline TraceContextBumper::TraceContextBumper ([[maybe_unused]] const wchar_t* contextName) noexcept
    {
    }
    inline TraceContextBumper::TraceContextBumper ([[maybe_unused]] const wchar_t* contextName, [[maybe_unused]] const wchar_t* extraFmt, ...) noexcept
    {
    }
    inline TraceContextBumper::TraceContextBumper ([[maybe_unused]] const char* contextName) noexcept
    {
    }
    inline TraceContextBumper::~TraceContextBumper ()
    {
    }
#endif


    namespace Private_ {
        struct TraceModuleData_ {
            TraceModuleData_ ();
            ~TraceModuleData_ ();

            Emitter fEmitter;
#if qTraceToFile
            filesystem::path fTraceFileName;
#endif
        };
    }

}

namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Debug::Private_::TraceModuleData_> _Stroika_Foundation_Debug_Trace_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}

inline Stroika::Foundation::Debug::Emitter& Stroika::Foundation::Debug::Emitter::Get () noexcept
{
    return Execution::ModuleInitializer<Private_::TraceModuleData_>::Actual ().fEmitter;
}
#endif /*_Stroika_Foundation_Debug_Trace_inl_*/

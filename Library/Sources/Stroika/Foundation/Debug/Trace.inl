
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_Trace_inl_
#define _Stroika_Foundation_Debug_Trace_inl_ 1

#if qTraceToFile
#include <filesystem>
#endif
#include <mutex>

CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qTraceToFile, qTraceToFile);
CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qDefaultTracingOn, qDefaultTracingOn);

namespace Stroika::Foundation::Debug {

    namespace Private_ {
        void EmitFirstTime (Emitter& emitter);
    }

    /*
     ********************************************************************************
     ******************************* Trace::Emitter *********************************
     ********************************************************************************
     */
    inline Emitter& Emitter::Get () noexcept
    {
        static Emitter   sEmitter_;
        static once_flag sOnceFlag_;
        call_once (sOnceFlag_, [] () {
            Private_::EmitFirstTime (sEmitter_);
        });
        return sEmitter_;
    }
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

    /*
     ********************************************************************************
     ******************************* TraceContextBumper *****************************
     ********************************************************************************
     */
    inline TraceContextSuppressor::TraceContextSuppressor () noexcept
    {
        ++tSuppressCnt_;
    }
    inline TraceContextSuppressor::~TraceContextSuppressor ()
    {
        --tSuppressCnt_;
    }
    inline bool TraceContextSuppressor::GetSuppressTraceInThisThread ()
    {
        return tSuppressCnt_ > 0;
    }

}

#endif /*_Stroika_Foundation_Debug_Trace_inl_*/

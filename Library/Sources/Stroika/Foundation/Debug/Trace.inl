
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_Trace_inl_
#define _Stroika_Foundation_Debug_Trace_inl_ 1

#include <filesystem>
#include <mutex>

CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qTraceToFile, qStroika_Foundation_Debug_Trace_TraceToFile);
CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qDefaultTracingOn, qStroika_Foundation_Debug_Trace_DefaultTracingOn);

namespace Stroika::Foundation::Debug {

    namespace Private_ {
        struct ModuleInit_ final {
            ModuleInit_ () noexcept;
            ~ModuleInit_ ();
        };
        const inline ModuleInit_ _MI_;
    }

    class Private_::Emitter final {
    public:
        Emitter ()               = default;
        Emitter (const Emitter&) = delete;

    public:
        static Emitter& Get () noexcept;

#if qStroika_Foundation_Debug_Trace_TraceToFile
    public:
        static filesystem::path GetTraceFileName ();
#endif

    public:
        /**
         *   \note DbgTrace() is NOT a cancelation point, so you can call this freely without worrying about Throw (ThreadAbortException) etc
         */
        nonvirtual void EmitTraceMessage (const char* format, ...) noexcept;
        nonvirtual void EmitTraceMessage (const wchar_t* format, ...) noexcept;

        nonvirtual TraceLastBufferedWriteTokenType EmitTraceMessage (size_t bufferLastNChars, const char* format, ...) noexcept;
        nonvirtual TraceLastBufferedWriteTokenType EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...) noexcept;

    public:
        // if the last write matches the given token (no writes since then) and the timestamp is unchanged, abandon
        // the buffered characters and return true. Else flush(write) them, and return false.
        nonvirtual bool UnputBufferedCharactersForMatchingToken (TraceLastBufferedWriteTokenType token);

        template <typename CHARTYPE>
        nonvirtual void EmitUnadornedText (const CHARTYPE* p);

    private:
        // This is the same as EmitTraceMessage_ - but it takes a plain string - and assumes the caller does any 'sprintf' stuff...
        template <typename CHARTYPE>
        nonvirtual TraceLastBufferedWriteTokenType DoEmitMessage_ (size_t bufferLastNChars, const CHARTYPE* p, const CHARTYPE* e);

    private:
        size_t fLastNCharBufCharCount_{0}; // len of valid data in fLastNCharBuf_CHAR_ or fLastNCharBuf_WCHAR_
        char fLastNCharBuf_CHAR_[10]; // always filled in before used, so no need to initialize - NOT nul-terminated(see fLastNCharBufCharCount_)
        wchar_t fLastNCharBuf_WCHAR_[10];
        bool    fLastNCharBuf_WCHARFlag_{false}; // determines (if fLastNCharBufCharCount_!=0) which buffer CHAR or WCHAR to use
        TraceLastBufferedWriteTokenType fLastNCharBuf_Token_{0};
        Time::TimePointSeconds          fLastNCharBuf_WriteTickcount_{};

        nonvirtual void BufferNChars_ (size_t nChars, const char* p);
        nonvirtual void BufferNChars_ (size_t nChars, const wchar_t* p);

    private:
        nonvirtual void FlushBufferedCharacters_ ();

    private:
        nonvirtual void DoEmit_ (const char* p) noexcept;
        nonvirtual void DoEmit_ (const wchar_t* p) noexcept;
        nonvirtual void DoEmit_ (const char* p, const char* e) noexcept;
        nonvirtual void DoEmit_ (const wchar_t* p, const wchar_t* e) noexcept;
    };

    /*
     ********************************************************************************
     ******************************* TraceContextBumper *****************************
     ********************************************************************************
     */
    inline TraceContextBumper::TraceContextBumper () noexcept
    {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        IncCount_ ();
#endif
    }
#if !qStroika_Foundation_Debug_Trace_DefaultTracingOn
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

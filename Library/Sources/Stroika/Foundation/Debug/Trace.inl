
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

#include "Stroika/Foundation/Configuration/StdCompat.h"
#include "Stroika/Foundation/Time/Clock.h"

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

        template <typename CHAR_T, typename... Args>
        nonvirtual void EmitTraceMessage (Characters::FormatString<CHAR_T> fmt, Args&&... args) noexcept
        {
            try {
                if constexpr (same_as<CHAR_T, char>) {
                    EmitTraceMessage_ (qStroika_Foundation_Characters_FMT_PREFIX_::string_view{fmt.sv},
                                       Configuration::StdCompat::make_format_args (args...));
                }
                else if constexpr (same_as<CHAR_T, wchar_t>) {
                    EmitTraceMessage_ (qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fmt.sv},
                                       Configuration::StdCompat::make_wformat_args (args...));
                }
            }
            catch (...) {
            }
        }

        nonvirtual TraceLastBufferedWriteTokenType EmitTraceMessage (size_t bufferLastNChars, const char* format, ...) noexcept;
        nonvirtual TraceLastBufferedWriteTokenType EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...) noexcept;

    private:
        nonvirtual void EmitTraceMessage_ (wstring_view users_fmt, Configuration::StdCompat::wformat_args&& args) noexcept;
        nonvirtual void EmitTraceMessage_ (string_view users_fmt, Configuration::StdCompat::format_args&& args) noexcept;
        nonvirtual void EmitTraceMessage_ (const wstring& raw) noexcept;

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
        TraceLastBufferedWriteTokenType          fLastNCharBuf_Token_{0};
        Time::DisplayedRealtimeClock::time_point fLastNCharBuf_WriteTickcount_{};

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

    namespace Private_ {
        template <typename ARRAY_TYPE, size_t SIZE_OF_ARRAY>
        inline constexpr size_t NEltsOf ([[maybe_unused]] const ARRAY_TYPE (&arr)[SIZE_OF_ARRAY])
        {
            return SIZE_OF_ARRAY;
        }
    }
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
    template <typename CHAR_T>
    inline TraceContextBumper::TraceContextBumper ([[maybe_unused]] const CHAR_T* contextName) noexcept
        requires (same_as<CHAR_T, char> or same_as<CHAR_T, wchar_t>)
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        : TraceContextBumper{cvt2WChartArrayTrunc_ (contextName)}
#endif
    {
    }
    template <typename CHAR_T, typename FCHAR_T, typename... ARGS>
    TraceContextBumper::TraceContextBumper ([[maybe_unused]] const CHAR_T* contextName,
                                            [[maybe_unused]] Characters::FormatString<FCHAR_T> fmt, [[maybe_unused]] ARGS&&... args) noexcept
        requires ((same_as<CHAR_T, char> or same_as<CHAR_T, wchar_t>) and (same_as<FCHAR_T, char> or same_as<FCHAR_T, wchar_t>))
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        : TraceContextBumper{cvt2WChartArrayTrunc_ (contextName), ProcessFmtString_ (fmt, forward<ARGS> (args)...)}
#endif
    {
    }
#if !qStroika_Foundation_Debug_Trace_DefaultTracingOn
    inline TraceContextBumper::TraceContextBumper ([[maybe_unused]] const wchar_t* contextName, [[maybe_unused]] const wchar_t* extraFmt, ...) noexcept
    {
    }
#endif
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
    template <typename CHAR_T, typename... ARGS>
    auto TraceContextBumper::ProcessFmtString_ (Characters::FormatString<CHAR_T> fmt, ARGS&&... args) noexcept -> CHAR_ARRAY_T
        requires (same_as<CHAR_T, char> or same_as<CHAR_T, wchar_t>)
    {
        try {
            basic_string<CHAR_T> r;
            if constexpr (same_as<CHAR_T, wchar_t>) {
                r = Configuration::StdCompat::vformat (qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view{fmt.sv},
                                                       Configuration::StdCompat::make_wformat_args (args...));
            }
            else {
                r = Configuration::StdCompat::vformat (qStroika_Foundation_Characters_FMT_PREFIX_::string_view{fmt.sv},
                                                       Configuration::StdCompat::make_format_args (args...));
            }
            size_t       len = min<size_t> (r.size (), kMaxContextNameLen_);
            CHAR_ARRAY_T result;
            // Dont call Memory::Span here - but reproduce in loop to avoid deadly embrace
            //char_traits<CHAR_T>::copy (result.data (), r.data (), len);
            for (size_t i = 0; i < len; ++i) {
                result[i] = static_cast<wchar_t> (r[i]);
            }
            result[len] = '\0';
            return result;
        }
        catch (...) {
            // we are already deep in the guts of DbgTrace stuff - so not much chance of 'logging' this issue ;-)
            return {};
        }
    }
    template <typename CHAR_T>
    auto TraceContextBumper::cvt2WChartArrayTrunc_ (span<const CHAR_T> contextName) -> CHAR_ARRAY_T
    {
        // Return item with max size kMaxContextNameLen_+1 so we can tell if we need to add elipsis
        array<wchar_t, kMaxContextNameLen_ + 1> r;
        auto                                    ci = contextName.begin ();
        for (; ci != contextName.end (); ++ci) {
            size_t i = ci - contextName.begin ();
            if (i < kMaxContextNameLen_) {
                r[i] = *ci;
            }
            else {
                break;
            }
        }
        Assert (ci - contextName.begin () <= kMaxContextNameLen_);
        r[ci - contextName.begin ()] = '\0';
        return r;
    }
    template <typename CHAR_T>
    auto TraceContextBumper::cvt2WChartArrayTrunc_ (const CHAR_T* contextName) -> CHAR_ARRAY_T
    {
        return cvt2WChartArrayTrunc_ (span{contextName, char_traits<CHAR_T>::length (contextName)});
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

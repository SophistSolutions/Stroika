/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Trace_h_
#define _Stroika_Foundation_Debug_Trace_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <array>
#include <span>
#include <thread>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "CompileTimeFlagChecker.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Characters {
    template <typename CHAR_T>
    struct FormatString;
}

namespace Stroika::Foundation::Debug {

    /**
     *  If qStroika_Foundation_Debug_Trace_TraceToFile is set true, then DbgTrace () and other Trace calls all go both to the usual debugger output
     *  screen, and to an auto-generated logfile.
     *
     *  This can be handy for building a version of code to hand to customers with an irreproducible bug to get a detailed
     *  report of what happened. Its also handy for embedded or timing sensative programs where capturing the log
     *  of exactly what happened is helpful.
     */
#if !defined(qStroika_Foundation_Debug_Trace_TraceToFile)
#if defined(qTraceToFile)
#warning "use qStroika_Foundation_Debug_Trace_TraceToFile since Stroika v3.0d4 "
#define qStroika_Foundation_Debug_Trace_TraceToFile qTraceToFile
#define qTraceToFile 0
#else
#define qStroika_Foundation_Debug_Trace_TraceToFile 0
#endif
#endif

    /*
     *  qStroika_Foundation_Debug_Trace_DefaultTracingOn provides the default configuration for whether or not DbgTrace logs
     *  or just is 'compiled out' of target programs.
     * 
     *  Note ALSO - many Stroika modules (CPP files) contain a private
     *    // Comment this in to turn on tracing in this module
     *    //#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1
     *    define.
     *
     *    This is often not enabled by default because it could produce lots of uninteresting noise in logfiles
     *    (when tracing on).
     *
     *    Turn per-module USE_NOISY_TRACE_IN_THIS_MODULE_ flags on selectively just to enable extra detailed logging on a per module basis.
     */
#if !defined(qStroika_Foundation_Debug_Trace_DefaultTracingOn)

#if defined(qDefaultTracingOn)
#warning "use qStroika_Foundation_Debug_Trace_DefaultTracingOn since Stroika v3.0d4 "
#define qStroika_Foundation_Debug_Trace_DefaultTracingOn qDefaultTracingOn
#else
#define qStroika_Foundation_Debug_Trace_DefaultTracingOn qDebug
#endif
#endif

    /**
     * \brief if true, emit a much shorter thread ID, making - I suspect (testing) for terser and clearer tracelogs. 
     *        Only downside is that you must find first occurrence of that index to find real threadId, and use that in waits, etc.
     *
     *  This is defined as a #define, so you can turn it off when building Stroika. I'm unsure if the showing of thread indexes
     *  helps readability of the tracelog, but I think it does.
     *
     *  This is somewhat helpful on windows, but much more so on 64-bit linux with very long fairly random looking thread ids.
     */
#ifndef qStroika_Foundation_Debug_Trace_ShowThreadIndex
#define qStroika_Foundation_Debug_Trace_ShowThreadIndex 1
#endif

    namespace Private_ {
        // The 'TraceLastBufferedWriteTokenType' overload of EmitTraceMessage allows you to specify a set of (trailing) characters to
        // be temporarily buffered. These characters are not immediately emitted, and can be cleared via UnputBufferedCharactersForMatchingToken ().
        // They will eventually be flushed out on the next call to EmitTraceMessage ().
        using TraceLastBufferedWriteTokenType = int;
        class Emitter;
    }

    /**
     *  Just for debugging purposes, get the name displayed in the trace log for the given thread-id.
     */
    wstring GetDbgTraceThreadName (thread::id threadID);
    string  GetDbgTraceThreadName_A (thread::id threadID);

    /**
     *  Define a new start/end context (with optional label) for trace messages emitted with DbgTrace (), and indent future
     *  DbgTrace () messages (from this thread) during the lifetime of TraceContextBumper.
     *
     *  \par Example Usage
     *      \code
     *          Debug::TraceContextBumper ctx{"MyXercesMemMgr_::DUMPCurMemStats"};
     *          DbgTrace (L"x");
     *      \endcode
     *
     *      Generates log output:
     *          <MyXercesMemMgr_::DUMPCurMemStats>
     *            x
     *          </MyXercesMemMgr_::DUMPCurMemStats>
     *
     *  \par Example Usage
     *      \code
     *          Debug::TraceContextBumper ctx{L"OptionsFile::ReadRaw", L"readfilename=%s", GetReadFilePath_ ().c_str ()};
     *      \endcode
     *
     *      Generates log output (assuming ReadRaw is quick and doesn't do more DbgTrace calls):
     *          <OptionsFile::ReadRaw (readfilename=C:\Users\Lewis\AppData\Local\Temp\MyModule.json)/>
     *
     *  \par Example Usage
     *      \code
     *          Debug::TraceContextBumper ctx { Stroika_Foundation_Debug_OptionalizeTraceArgs (L"OptionsFile::ReadRaw", L"readfilename=%s", GetReadFilePath_ ().c_str ()) };
     *          DbgTrace (L"x");
     *      \endcode
     *
     *      Generates log output:
     *          <OptionsFile::ReadRaw (readfilename=C:\Users\Lewis\AppData\Local\Temp\MyModule.json)>
     *            x
     *          </OptionsFile::ReadRaw>
     *
     *  \par Example Usage
     *      \code
     *          struct X {
     *              // ... lots of data members - and you get a crash between constructor or destruction of some of them
     *              // ... use this trick to see dbgmessages BETWEEN construction and destruction of each member
     *              ComplexObject fComplexObject1;
     *              TraceContextBumper tmpNoteAfterComplexObj1 {"after fComplexObject1"};
     *              ComplexObject fComplexObject2;
     *              TraceContextBumper tmpNoteAfterComplexObj2 {"after fComplexObject2"};
     *          };
     *      \endcode
     *
     *  \note ***Not Cancelation Point*** - and uses  noexcept
     *  \note safe to call and does nothing if !qStroika_Foundation_Debug_Trace_DefaultTracingOn
     */
    class TraceContextBumper final {
    public:
        /**
        * 
        *   @todo REDO ALL THESE DOCS --- AS OF 2024-03-22
        * 
         *  If constructor taking const char* used, the argument must be ASCII characters.
         *
         *  The constructor with 'extraFmt', emits the extra data in the heading of the trace message, but
         *  not the close brace. This can allow for more terse TraceContextBumper messages, and more terse
         *  calling usage.
         *
         *  For TraceContextBumper (const wchar_t* contextName, const wchar_t* extraFmt, ...) usage, @see Stroika_Foundation_Debug_OptionalizeTraceArgs
         *  to optionally suppress side-effects.
         *
         *  \note ***Not Cancelation Point*** - and uses noexcept
         */
        TraceContextBumper () noexcept;
        template <typename CHAR_T>
        TraceContextBumper (const CHAR_T* contextName) noexcept
            requires (same_as<CHAR_T, char> or same_as<CHAR_T, wchar_t>);
        template <typename CHAR_T, typename FCHAR_T, typename... ARGS>
        TraceContextBumper (const CHAR_T* contextName, Characters::FormatString<FCHAR_T> fmt, ARGS&&... args) noexcept
            requires ((same_as<CHAR_T, char> or same_as<CHAR_T, wchar_t>) and (same_as<FCHAR_T, char> or same_as<FCHAR_T, wchar_t>));
        TraceContextBumper (const TraceContextBumper&) = delete;

    public:
        // legacy API
        [[deprecated ("Since Stroika v3.0d6 - use _f format strings")]] TraceContextBumper (const wchar_t* contextName,
                                                                                            const wchar_t* extraFmt, ...) noexcept;

#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
    private:
        // Nothing too important about this constant, but not so long as to be hard to read
        static constexpr size_t kMaxContextNameLen_{80};

        using CHAR_ARRAY_T = array<wchar_t, kMaxContextNameLen_ + 1>; // enforce always nul-terminated strings - but can copy by value

    private:
        // spans are outer bounds of strings - also look for NUL-term to find string length - require they are nul-terminated (so not sure why span is useful - @todo rethink)
        TraceContextBumper (CHAR_ARRAY_T mainName, CHAR_ARRAY_T extraTextAtTop = {}) noexcept;
#endif

    public:
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        ~TraceContextBumper () noexcept;
#else
        ~TraceContextBumper () noexcept = default;
#endif

    public:
        nonvirtual TraceContextBumper& operator= (const TraceContextBumper&) = delete;

#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
    public:
        bool fDoEndMarker{false};

    public:
        static unsigned int GetCount ();

    private:
        CHAR_ARRAY_T fSavedContextName_{};
        Private_::TraceLastBufferedWriteTokenType fLastWriteToken_{}; // used to COMBINE items into a single line if they happen quickly enuf

    private:
        template <typename CHAR_T>
        static CHAR_ARRAY_T cvt2WChartArrayTrunc_ (span<const CHAR_T> contextName);
        template <typename CHAR_T>
        static CHAR_ARRAY_T cvt2WChartArrayTrunc_ (const CHAR_T* contextName);
        template <typename CHAR_T, typename... ARGS>
        CHAR_ARRAY_T ProcessFmtString_ (Characters::FormatString<CHAR_T> fmt, ARGS&&... args) noexcept
            requires (same_as<CHAR_T, char> or same_as<CHAR_T, wchar_t>);

    private:
        static void IncCount_ () noexcept;
        static void DecrCount_ () noexcept;
#endif
    };

    /**
     *  Suppress trace messages (in this thread) for the lifetime of this object.
     *
     *  \par Example Usage
     *      \code
     *          {
     *              Debug::TraceContextSuppressor suppressTraceInThisBlock;
     *              DbgTrace (L"x");    // suppressed
     *          }
     *          DbgTrace (L"x");        // emitted
     *      \endcode
     */
    class TraceContextSuppressor final {
    public:
        /**
         */
        TraceContextSuppressor () noexcept;
        TraceContextSuppressor (const TraceContextSuppressor&)            = delete;
        TraceContextSuppressor& operator= (const TraceContextSuppressor&) = delete;

    public:
        ~TraceContextSuppressor ();

    public:
        static bool GetSuppressTraceInThisThread ();

    private:
        static inline thread_local unsigned int tSuppressCnt_;
    };

    /**
     * \def Stroika_Foundation_Debug_OptionalizeTraceArgs
     *
     *   This is meant to be used with the 2+ argument Debug::TraceContextBumper constructor (_f format string), to optionally suppress side-effects
     *   of trace arguments when tracing is disabled (at compile time).
     * 
     *  \note this may still be useful in Stroika v3 and later, but was mostly useful in before, switching to use of variadic templates, since now
     *        the compiler can see the variadic and format parameters are unused for anything, so probably is able to optimize them away.
     */
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
#define Stroika_Foundation_Debug_OptionalizeTraceArgs(...) __VA_ARGS__
#else
#define Stroika_Foundation_Debug_OptionalizeTraceArgs(...)
#endif

    /**
     * \def DbgTrace
     *
     *   This function either does NOTHING (trying to not even evaluate its arguments)
     *   or does a printf style PRINT function by delegating to @'EmitTraceMessage'. Which of
     *   these two behaviors you see is conditioned on @'qStroika_Foundation_Debug_Trace_DefaultTracingOn'</p>
     *
     *   \note ***Not Cancelation Point*** - and uses  noexcept
     *         So you can call this freely without worrying about Throw (ThreadAbortException) etc
     *         (though beware of passing arguments to DbgTrace() which may be cancelation points)
     * 
     *  \note - DbgTrace can take either printf style format strings, or _f format strings.
     * 
     *  \par Example Usage
     *      \code
     *          auto url = IO::Network::URI{"http://www.sophists.com"};
     *          DbgTrace ("u = {}"_f, url);     // output "u = 'http://www.sophists.com'"
     *      \endcode
     * 
     *  @todo - redo NOT as MACRO but as FUNCTION call (to emittracemessage)
     * 
     *   template <typename CHAR_T, typename... Args>
        nonvirtual void DbgTrace (Characters::FormatString<CHAR_T> fmt, Args&&... args) noexcept
        {
        or some such
            try {
                Stroika::Foundation::Debug::Private_::Emitter::Get ().EmitTraceMessage (fmt.get (), Configuration::StdCompat::make_wformat_args (args...));
            }
            catch (...) {
            }
        }
        but only after 'd' stage development so I can lose C-style FMT string overloads...
     */
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn

#define DbgTrace Stroika::Foundation::Debug::Private_::Emitter::Get ().EmitTraceMessage
#else
#define DbgTrace _NoOp_
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Trace.inl"

#endif /*_Stroika_Foundation_Debug_Trace_h_*/

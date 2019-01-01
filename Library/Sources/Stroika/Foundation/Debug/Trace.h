/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Trace_h_
#define _Stroika_Foundation_Debug_Trace_h_ 1

#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <windows.h>

#include <tchar.h>
#endif
#include <array>

#include "../Characters/SDKChar.h"
#include "../Characters/SDKString.h"
#include "../Configuration/Common.h"
#include "../Execution/ModuleInit.h"
#include "../Time/Realtime.h"
#include "CompileTimeFlagChecker.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::Debug {

    using Characters::SDKChar;
    using Characters::SDKString;

    /**
     *  If qTraceToFile is set true, then DbgTrace () and other Trace calls all go both to the usual debugger output
     *  screen, and to an auto-generated logfile.
     *
     *  This can be handy for building a version of code to hand to customers with an irreproducible bug to get a detailed
     *  report of what happened. Its also handy for embedded or timing sensative programs where capturing the log
     *  of exactly what happened is helpful.
     */
#if !defined(qTraceToFile)
#error "qTraceToFile should normally be defined indirectly by StroikaConfig.h"
#endif
    CompileTimeFlagChecker_HEADER (qTraceToFile, qTraceToFile);

    /*
     *  qDefaultTracingOn
     *
     *  <p>Led contains a limited amount of pre-built tracing code. This could be expanded someday,
     *   depending on how useful people find it. This defaults to being on only for Windows and if @'qDebug' is
     *   on (windows only cuz thats the only place I've implemented the trace message emitter so far).</p>
            *   @see    qDebug
            *   @see    DebugTrace

                        Note ALSO - many Stroika modules (CPP files) contain a private
                            // Comment this in to turn on tracing in this module
                            //#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1
                        define.

                        This is often not enabled by default because it could produce lots of unintersting noise in logfiles
                        (when tracing on).

                        Turn these flags on selectively just to enable extra detailed logging on a per module basis.

     */
#if !defined(qDefaultTracingOn)
#error "qDefaultTracingOn should normally be defined indirectly by StroikaConfig.h"
#endif

    CompileTimeFlagChecker_HEADER (qDefaultTracingOn, qDefaultTracingOn);

    namespace Private_ {
        struct TraceModuleData_;
    }

    /**
     *  Emitter is not meant to be used directly - but can be. Mostly users are expected to
     *  use DbgTrace () or TraceContextBumper.
     */
    class Emitter {
    private:
        Emitter ();

    public:
        static Emitter& Get () noexcept;

#if qTraceToFile
    public:
        nonvirtual SDKString GetTraceFileName () const;
#endif

    public:
        /**
         *   \note DbgTrace() is NOT a cancelation point, so you can call this freely without worrying about Throw (ThreadAbortException) etc
         */
        nonvirtual void EmitTraceMessage (const char* format, ...) noexcept;
        nonvirtual void EmitTraceMessage (const wchar_t* format, ...) noexcept;

        // The 'TraceLastBufferedWriteTokenType' overload of EmitTraceMessage allows you to specify a set of (trailing) characters to
        // be temporarily buffered. These characters are not immediately emitted, and can be cleared via UnputBufferedCharactersForMatchingToken ().
        // They will eventually be flushed out on the next call to EmitTraceMessage ().
        using TraceLastBufferedWriteTokenType = int;
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
        size_t                          fLastNCharBufCharCount_;
        char                            fLastNCharBuf_CHAR_[10];
        wchar_t                         fLastNCharBuf_WCHAR_[10];
        bool                            fLastNCharBuf_WCHARFlag_;
        TraceLastBufferedWriteTokenType fLastNCharBuf_Token_;
        Time::DurationSecondsType       fLastNCharBuf_WriteTickcount_;

        nonvirtual void BufferNChars_ (size_t nChars, const char* p);
        nonvirtual void BufferNChars_ (size_t nChars, const wchar_t* p);

    private:
        nonvirtual void FlushBufferedCharacters_ ();

    private:
        nonvirtual void DoEmit_ (const char* p) noexcept;
        nonvirtual void DoEmit_ (const wchar_t* p) noexcept;
        nonvirtual void DoEmit_ (const char* p, const char* e) noexcept;
        nonvirtual void DoEmit_ (const wchar_t* p, const wchar_t* e) noexcept;

    private:
        friend struct Private_::TraceModuleData_;
    };

    /**
     *  Define a new start/end context (with optional label) for trace messages emitted with DbgTrace (), and indent future
     *  DbgTrace () messages (from this thread) during the lifetime of TraceContextBumper.
     *
     *  \par Example Usage
     *      \code
     *          Debug::TraceContextBumper ctx ("MyXercesMemMgr_::DUMPCurMemStats");
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
     *          Debug::TraceContextBumper ctx (L"OptionsFile::ReadRaw", L"readfilename=%s", GetReadFilePath_ ().c_str ());
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
     *  \note ***Not Cancelation Point*** - and uses  noexcept
     */
    class TraceContextBumper {
    public:
        /**
         *  If constructor taking const char* used, the argument must be ASCII characters.
         *
         *  The constructor with 'extraFmt', emits the extra data in the heading of the trace message, but
         *  not the close brace. This can allow for more terse TraceContextBumper messages, and more terse
         *  calling usage.
         *
         *  For TraceContextBumper (const wchar_t* contextName, const wchar_t* extraFmt, ...) usage, @see Stroika_Foundation_Debug_OptionalizeTraceArgs
         *  to optionally supress side-effects.
         *
         *  \note ***Not Cancelation Point*** - and uses  noexcept
         */
        TraceContextBumper () noexcept;
        TraceContextBumper (const char* contextName) noexcept;
        TraceContextBumper (const wchar_t* contextName) noexcept;
        TraceContextBumper (const wchar_t* contextName, const wchar_t* extraFmt, ...) noexcept;
        TraceContextBumper (const TraceContextBumper&) = delete;

    public:
        ~TraceContextBumper ();

    public:
        nonvirtual TraceContextBumper& operator= (const TraceContextBumper&) = delete;

#if qDefaultTracingOn
    public:
        bool fDoEndMarker{false};

    public:
        static unsigned int GetCount ();

    private:
        // Nothing too important about this constant, but not so long as to be hard to read
        static constexpr size_t kMaxContextNameLen_{80};

    private:
        wchar_t                                  fSavedContextName_[kMaxContextNameLen_]{};
        Emitter::TraceLastBufferedWriteTokenType fLastWriteToken_{}; // used to COMBINE items into a single line if they happen quickly enuf

    private:
        static array<wchar_t, kMaxContextNameLen_> mkwtrfromascii_ (const char* contextName);

    private:
        static void IncCount_ () noexcept;
        static void DecrCount_ () noexcept;
#endif
    };

    /**
     * \def Stroika_Foundation_Debug_OptionalizeTraceArgs
     *
     *   This is meant to be used with the 2+ argument Debug::TraceContextBumper constructor, to optionally suppress side-effects
     *   of trace arguments when tracing is disabled (at compile time)
     */
#if qDefaultTracingOn
#define Stroika_Foundation_Debug_OptionalizeTraceArgs(...) __VA_ARGS__
#else
#define Stroika_Foundation_Debug_OptionalizeTraceArgs(...)
#endif

    /**
     * \def DbgTrace
     *
     *   This function either does NOTHING (trying to not even evaluate its arguments)
     *   or does a printf style PRINT function by delegating to @'EmitTraceMessage'. Which of
     *   these two behaviors you see is conditioned on @'qDefaultTracingOn'</p>
     *
     *   \note ***Not Cancelation Point*** - and uses  noexcept
     *         So you can call this freely without worrying about Throw (ThreadAbortException) etc
     *         (though beware of passing arguments to DbgTrace() which may be cancelation points)
     */
#ifndef DbgTrace
#if qDefaultTracingOn
#define DbgTrace Stroika::Foundation::Debug::Emitter::Get ().EmitTraceMessage
#else
#define DbgTrace _NoOp_
#endif
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Trace.inl"

#endif /*_Stroika_Foundation_Debug_Trace_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Debug {

            using Characters::SDKString;
            using Characters::SDKChar;

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

/**
            @CONFIGVAR:     qDefaultTracingOn
            @DESCRIPTION:   <p>Led contains a limited amount of pre-built tracing code. This could be expanded someday,
                        depending on how useful people find it. This defaults to being on only for Windows and if @'qDebug' is
                        on (windows only cuz thats the only place I've implemented the trace message emitter so far).</p>
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
             *  Define a new start/end context (with optional label) for trace messages emitted with DbgTrace ()
             *
             *  \par Example Usage
             *      \code
             *      Debug::TraceContextBumper ctx ("MyXercesMemMgr_::DUMPCurMemStats");
             *      \endcode
             *
             *  \note   TraceContextBumper is not a cancelation point (since noexcept)
             */
            class TraceContextBumper {
            public:
                /**
                 *  If constructor taking const char* used, the argument must be ASCII characters.
                 */
                TraceContextBumper () noexcept;
                TraceContextBumper (const char* contextName) noexcept;
                TraceContextBumper (const wchar_t* contextName) noexcept;
                TraceContextBumper (const TraceContextBumper&) = delete;
                ~TraceContextBumper ();

            public:
                nonvirtual TraceContextBumper& operator= (const TraceContextBumper&) = delete;

#if qDefaultTracingOn
            public:
                bool fDoEndMarker;

            public:
                static unsigned int GetCount ();

            private:
                // Nothing too important about this constant, but not so long as to be hard to read
                static constexpr size_t kMaxContextNameLen_{80};

            private:
                wchar_t                                  fSavedContextName_[kMaxContextNameLen_];
                Emitter::TraceLastBufferedWriteTokenType fLastWriteToken_; // used to COMBINE items into a single line if they happen quickly enuf

            private:
                static array<wchar_t, kMaxContextNameLen_> mkwtrfromascii_ (const char* contextName);

            private:
                static void IncCount_ () noexcept;
                static void DecrCount_ () noexcept;
#endif
            };

/**
            @METHOD:        DbgTrace
            @DESCRIPTION:   <p>This function either does NOTHING (trying to not even evaluate its arguments)
                        or does a printf style PRINT function by delegating to @'EmitTraceMessage'. Which of
                        these two behaviors you see is conditioned on @'qDefaultTracingOn'</p>
            *
            *   \note DbgTrace() is NOT a cancelation point, so you can call this freely without worrying about Throw (ThreadAbortException) etc
            */
#ifndef DbgTrace
#if qDefaultTracingOn
#define DbgTrace Stroika::Foundation::Debug::Emitter::Get ().EmitTraceMessage
#else
#define DbgTrace _NoOp_
#endif
#endif
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Trace.inl"

#endif /*_Stroika_Foundation_Debug_Trace_h_*/

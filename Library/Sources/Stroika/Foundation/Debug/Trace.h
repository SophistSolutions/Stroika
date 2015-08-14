/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Trace_h_
#define _Stroika_Foundation_Debug_Trace_h_  1

#include    "../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <windows.h>
#include    <tchar.h>
#endif
#include    <array>

#include    "../Characters/SDKChar.h"
#include    "../Characters/SDKString.h"
#include    "../Configuration/Common.h"
#include    "../Execution/ModuleInit.h"
#include    "../Time/Realtime.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            using   Characters::SDKString;
            using   Characters::SDKChar;


            /**
             *  If qTraceToFile is set true, then DbgTrace () and other Trace calls all go both to the usual debugger output
             *  screen, and to an auto-generated logfile.
             *
             *  This can be handy for building a version of code to hand to customers with an irreproducible bug to get a detailed
             *  report of what happened. Its also handy for embedded or timing sensative programs where capturing the log
             *  of exactly what happened is helpful.
             */
#if     !defined (qTraceToFile)
#error "qTraceToFile should normally be defined indirectly by StroikaConfig.h"
#endif


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
#if     !defined (qDefaultTracingOn)
#error "qDefaultTracingOn should normally be defined indirectly by StroikaConfig.h"
#endif


            namespace Private_ {
                struct  TraceModuleData_;
            }


            /**
            */
            class   Emitter {
            private:
                Emitter ();
            public:
                static  Emitter&    Get ();

#if     qTraceToFile
            public:
                nonvirtual  SDKString GetTraceFileName () const;
#endif

            public:
                nonvirtual  void    EmitTraceMessage (const char* format, ...);
                nonvirtual  void    EmitTraceMessage (const wchar_t* format, ...);

                // The 'TraceLastBufferedWriteTokenType' overload of EmitTraceMessage allows you to specify a set of (trailing) characters to
                // be temporarily buffered. These characters are not immediately emitted, and can be cleared via UnputBufferedCharactersForMatchingToken ().
                // They will eventually be flushed out on the next call to EmitTraceMessage ().
                using   TraceLastBufferedWriteTokenType     =   int;
                nonvirtual  TraceLastBufferedWriteTokenType EmitTraceMessage (size_t bufferLastNChars, const char* format, ...);
                nonvirtual  TraceLastBufferedWriteTokenType EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...);

            public:
                // if the last write matches the given token (no writes since then) and the timestamp is unchanged, abandon
                // the buffered characters and return true. Else flush(write) them, and return false.
                nonvirtual  bool    UnputBufferedCharactersForMatchingToken (TraceLastBufferedWriteTokenType token);

                template    <typename   CHARTYPE>
                nonvirtual  void    EmitUnadornedText (const CHARTYPE* p);

            private:
                // This is the same as EmitTraceMessage_ - but it takes a plain string - and assumes the caller does any 'sprintf' stuff...
                template    <typename   CHARTYPE>
                nonvirtual  TraceLastBufferedWriteTokenType DoEmitMessage_ (size_t bufferLastNChars, const CHARTYPE* p, const CHARTYPE* e);

            private:
                size_t                          fLastNCharBufCharCount_;
                char                            fLastNCharBuf_CHAR_[10];
                wchar_t                         fLastNCharBuf_WCHAR_[10];
                bool                            fLastNCharBuf_WCHARFlag_;
                TraceLastBufferedWriteTokenType fLastNCharBuf_Token_;
                Time::DurationSecondsType       fLastNCharBuf_WriteTickcount_;

                nonvirtual  void    BufferNChars_ (size_t nChars, const char* p);
                nonvirtual  void    BufferNChars_ (size_t nChars, const wchar_t* p);

            private:
                nonvirtual  void    FlushBufferedCharacters_ ();

            private:
                nonvirtual  void    DoEmit_ (const char* p);
                nonvirtual  void    DoEmit_ (const wchar_t* p);
                nonvirtual  void    DoEmit_ (const char* p, const char* e);
                nonvirtual  void    DoEmit_ (const wchar_t* p, const wchar_t* e);

            private:
                friend  struct  Private_::TraceModuleData_;
            };


            /**
            */
            class   TraceContextBumper {
            public:
                /**
                 *  If constructor taking const char* used, the argument must be ASCII characters.
                 */
                TraceContextBumper ();
                TraceContextBumper (const char* contextName);
                TraceContextBumper (const wchar_t* contextName);
                TraceContextBumper (const TraceContextBumper&) = delete;
                ~TraceContextBumper ();

            public:
                nonvirtual  TraceContextBumper& operator= (const TraceContextBumper&) = delete;

#if     qDefaultTracingOn
            public:
                bool    fDoEndMarker;

            public:
                static  unsigned int    GetCount ();

            private:
                // Nothing too important about this constant, but not so long as to be hard to read
#if     qCompilerAndStdLib_constexpr_Buggy
                DEFINE_CONSTEXPR_CONSTANT (size_t, kMaxContextNameLen_, 80);
#else
                static  constexpr size_t    kMaxContextNameLen_   { 80 };
#endif
            private:
                wchar_t                                     fSavedContextName_[kMaxContextNameLen_];
                Emitter::TraceLastBufferedWriteTokenType    fLastWriteToken_;           // used to COMBINE items into a single line if they happen quickly enuf

            private:
                static  array<wchar_t, kMaxContextNameLen_>  mkwtrfromascii_ (const char* contextName);

            private:
                static  void    IncCount_ ();
                static  void    DecrCount_ ();
#endif
            };


            /**
            @METHOD:        DbgTrace
            @DESCRIPTION:   <p>This function either does NOTHING (trying to not even evaluate its arguments)
                        or does a printf style PRINT function by delegating to @'EmitTraceMessage'. Which of
                        these two behaviors you see is conditioned on @'qDefaultTracingOn'</p>
            */
#ifndef DbgTrace
#if     qDefaultTracingOn
#define DbgTrace    Stroika::Foundation::Debug::Emitter::Get ().EmitTraceMessage
#else
#define DbgTrace    _NoOp_
#endif
#endif


            /**
             *  Simple utility to print debug trace timing values for procedures (or contexts).
             *
             *  \par Example Usage
             *      \code
             *      {
             *          TimingTrace ttrc;
             *          do_some_call();
             *      }
             *      \endcode
             *      \code
             *      OUTPUT IN TRACELOG:
             *          (timeTaken=3.4 seconds)
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *      {
             *          TraceContextBumper ctx ("do_some_call");
             *          TimingTrace         ttrc;
             *          do_some_call();
             *      }
             *      \endcode
             *      \code
             *      OUTPUT IN TRACELOG:
             *          <do_some_call>
             *              (timeTaken=3.4 seconds)
             *          </do_some_call>
             *      \endcode
             */
            struct  TimingTrace {
            public:
                /**
                 */
                TimingTrace (const TimingTrace&) = delete;
                TimingTrace (Time::DurationSecondsType warnIfLongerThan = 0);
                TimingTrace (const SDKChar* msg, Time::DurationSecondsType warnIfLongerThan = 0);

            public:
                /**
                 */
                ~TimingTrace ();

            private:
                Time::DurationSecondsType fStartedAt_;
                Time::DurationSecondsType fMinTime2Report_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Trace.inl"

#endif  /*_Stroika_Foundation_Debug_Trace_h_*/

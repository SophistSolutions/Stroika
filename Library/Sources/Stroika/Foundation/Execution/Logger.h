﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Logger_h_
#define _Stroika_Foundation_Execution_Logger_h_ 1

#include    "../StroikaPreComp.h"

#include    <cstdarg>

#include    "../Configuration/Common.h"
#include    "../Configuration/Enumeration.h"
#include    "../Characters/String.h"
#include    "../Debug/Assertions.h"
#include    "../Memory/Optional.h"
#include    "../Time/Realtime.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Finish support for Windows Event Manager Log Appender -- WindowsEventLogAppender. Its
 *              printing some data, but very minimally and wrongly handling categories etc. Probably could get close
 *              by specifying hardwired/hacked values in the CTOR args.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            using   namespace   Configuration;


            using   Characters::String;


            /**
             *  OVERVIEW:
             *
             *      The point of the Logging Module is to provide a simple, portable wrapper on end-user-targetted
             *  application logging. This form of logging is the kind of logging you leave builtin to your application,
             *  and write focused on end-user readability. It is NOT (primarily) for debugging (for that -
             *  use the Stroika::Foundation::Debug::Trace module).
             *
             *      Think of this as a simple/portable wrapper on syslog/log4j/WindowsEventlog.
             *
             *      The Logger is a singleton object. It can be set at any number of application logging
             *  levels. And it will write information to the backend logger its setup with. But default -
             *  this is none.
             *
             *      To use the logger and actually get logging - pick a logger rep, and call
             *  SetLoggingAppender ();
             *
             *      This logging API CANNOT be used before main () has started, or after main () has completed
             *  [if we choose to allow that we must use ModuleInit, but since this requires a call to setup a
             *  logger, that seems unlikely]
             *
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            class   Logger final    {
            public:
                // See syslog for enumeration of various targets/etc.

            public:
                class   IAppenderRep;
            public:
                using   IAppenderRepPtr     =   shared_ptr<IAppenderRep>;

            public:
#if     qHas_Syslog
                class   SysLogAppender;
#endif
                class   FileAppender;
#if     qPlatform_Windows
                class   WindowsEventLogAppender;
#endif

            public:
                static  Logger& Get ();
            private:
                Logger ();
            public:
                Logger (const Logger&) = delete;
                const Logger& operator= (const Logger&) = delete;

            public:
                nonvirtual  IAppenderRepPtr GetAppender () const;
                nonvirtual  void            SetAppender (const IAppenderRepPtr& rep);

            private:
                IAppenderRepPtr fAppender_;

            public:
                /**
                 *
                 *  Names are based on syslog from http://unix.superglobalmegacorp.com/Net2/newsrc/sys/syslog.h.html
                 *  But the NUMBERS do NOT correspond!
                 *
                 *  Lower numbers are less interesting (debug) and higher numbers more important (higher priority).
                 */
                enum    class  Priority : uint8_t {
                    eDebug              =   0,              // The message is debug info (not a good use of syslog - consider using DbgTrace)
                    eInfo               =   1,              // The message is purely informational
                    eNotice             =   2,              // The message describes a normal but important event
                    eWarning            =   3,              // The message is a warning
                    eError              =   4,              // The message describes an error
                    eCriticalError      =   5,              // The message states a critical condition
                    eAlertError         =   6,              // Action on the message must be taken immediately
                    eEmergency          =   7,              // The message says the system is unusable
                    Stroika_Define_Enum_Bounds(eDebug, eEmergency)
                };
                static  const EnumNames<Priority>   Stroika_Enum_Names(Priority);

            public:
                /**
                 *  This defaults to eInfo. Messages of lower priority (e.g. eDebug) will not be logged to the underlying log
                 *  system.
                 *
                 *  This is done inline, so in principle, the compiler may eliminate the calls.
                 *
                 *  The MIN is min inclusive.
                 */
                nonvirtual  Priority    GetMinLogLevel () const;

            public:
                /**
                 *      @see GetMinLogLevel
                 */
                nonvirtual  void        SetMinLogLevel (Priority minLogLevel);

            public:
                /**
                 *      @see GetMinLogLevel
                 *      This determines if a call to Log() with this argument log-level would
                 *      write anything.
                 */
                static  bool    WouldLog (Priority logLevel);

            public:
                /**
                 *      Log bufffering is DISABLED by default, since it has some cost. But if enabled, Log() calls
                 *  queue an internal message, which another thread wakes up to write. This CAN be critical for performance
                 *  reasons, so the caller can freely log things, and not have their thread blocked.
                 *
                 *      Beware, this feature CAN mean that something you log, wont make it out of the application if
                 *  the appliaction terminates before the log can be flushed. For example:
                 *
                 *              Log ("QUITTING");
                 *              exit (0);
                 *      probably won't get logged. To avoid this issue, call SetBufferingEnabled(false) before quitting, or
                 *      call FlushBuffer();
                 *
                 *      In one application (open-embedded arm linux) I saw a 3ms latency before I added this (2014-05-30).
                 */
                static  bool        GetBufferingEnabled ();

            public:
                /**
                 *      @see GetBufferingEnabled ()
                 */
                static  void        SetBufferingEnabled (bool logBufferingEnabled);

            public:
                /**
                 *      @see GetBufferingEnabled ()
                 *
                 *  This has no effect if the buffer is empty or buffering is disabled.
                 */
                static  void        FlushBuffer ();

            public:
                /**
                 *      If true, then a sequence of N (N > 1) identical messages will be replaced with two messages,
                 *      the second of which appears with the added label [N-2 suppressed].
                 *
                 *      The duration is the window of time after the last message we wait before emitting the
                 *      last message. A good default for this might be 5 or 10 seconds.
                 */
                static  Memory::Optional<Time::DurationSecondsType>     GetSuppressDuplicates ();

            public:
                /**
                 *      @see GetSuppressDuplicates ()
                 */
                static  void        SetSuppressDuplicates (const Memory::Optional<Time::DurationSecondsType>& suppressDuplicatesThreshold);

            public:
                /**
                 *  Log
                 *
                 *  Design Note:
                 *      The 'format' parameter must be defined a String (or const wchar_t*), but
                 *      not const String&, because:
                 *
                 *      18.10 Other runtime support [support.runtime]
                 *      ...
                 *      3 The restrictions that ISO C places on the second parameter to the va_start()
                 *      macro in header <stdarg.h> are diﬀerent in this International Standard.
                 *      The parameter parmN is the identiﬁer of the rightmost parameter in the variable parameter
                 *      list of the function deﬁnition (the one just before the ...). If the parameter
                 *      parmN is declared with a function, array, or reference type, or with a type that is
                 *      not compatible with the type that results when passing an argument for which there
                 *      is no parameter, the behavior is undeﬁned.
                 */
                static  void    Log (Priority logLevel, String format, ...); // varargs logger

            private:
                static  void    Log_ (Priority logLevel, const String& format, va_list argList);

            private:
                static  void    FlushDupsWarning_ ();

            private:
                static  void    UpdateBookkeepingThread_ ();

            private:
                Priority    fMinLogLevel_;
                bool        fBufferingEnabled_;

            private:
                static  Logger  sThe_;
            };


            /**
             */
            class   Logger::IAppenderRep {
            public:
                virtual ~IAppenderRep ();
            public:
                virtual void    Log (Priority logLevel, const String& message) = 0;
            };


#if     qHas_Syslog
            /**
             */
            class   Logger::SysLogAppender : public Logger::IAppenderRep {
            public:
                SysLogAppender (const String& applicationName);
                SysLogAppender (const String& applicationName, int facility);
                ~SysLogAppender ();
            public:
                virtual void    Log (Priority logLevel, const String& message) override;
            private:
                string fApplicationName_;  // must save like this because open-syslog appears to KEEP ahold of pointer (http://www.gnu.org/s/hello/manual/libc/openlog.html)
            };
#endif


            /**
             */
            class   Logger::FileAppender : public Logger::IAppenderRep {
            public:
                FileAppender (const String& fileName);
            public:
                virtual void    Log (Priority logLevel, const String& message) override;
            };


#if     qPlatform_Windows
            /**
             */
            class   Logger::WindowsEventLogAppender : public Logger::IAppenderRep {
            public:
                WindowsEventLogAppender (const String& eventSourceName);
            public:
                virtual void    Log (Priority logLevel, const String& message) override;
            private:
                String  fEventSourceName_;
            };
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Logger.inl"

#endif  /*_Stroika_Foundation_Execution_Logger_h_*/

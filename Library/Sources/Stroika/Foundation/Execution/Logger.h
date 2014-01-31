/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Logger_h_
#define _Stroika_Foundation_Execution_Logger_h_ 1

#include    "../StroikaPreComp.h"

#include    <cstdarg>

#include    "../Configuration/Common.h"
#include    "../Characters/String.h"
#include    "../Debug/Assertions.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   EITHER somehow automatically be smart about filtering out logging that misses log level filter,
 *              or add explicit data member saying to skip logging stuff below a given level. We probably need todo
 *              the later, and let the former be done by the backend appender.
 *
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
                nonvirtual  IAppenderRepPtr GetAppender () const;
                nonvirtual  void            SetAppender (const IAppenderRepPtr& rep);

            private:
                IAppenderRepPtr fAppender_;

            public:

                /*
                 * Stroika uses this facility for NO DEBUGGING - but and to be portbale, but
                    convenitnetly these numbers corredspond to teh SYSLOG values (so they can be substeid as needed).

                    NOTE: I don't think so many levels makes sense and I dont think these are well defined. I should probably trim the list,
                     and keep the

                     UPDATE DOCS
                 */
                enum class  Priority : uint8_t {
                    // Names and numbers for syslog from http://unix.superglobalmegacorp.com/Net2/newsrc/sys/syslog.h.html
                    eDebug              =   7,              // SYSLOG name LOG_DEBUG    -   The message is debug info (not a good use of syslog - consider using DbgTrace)
                    eInfo               =   6,              // SYSLOG name LOG_INFO     -   The message is purely informational
                    eNotice             =   5,              // SYSLOG name LOG_NOTICE   -   The message describes a normal but important event
                    eWarning            =   4,              // SYSLOG name LOG_WARNING  -   The message is a warning
                    eError              =   3,              // SYSLOG name LOG_ERR      -   The message describes an error
                    eCriticalError      =   2,              // SYSLOG name LOG_CRIT     -   The message states a critical condition
                    eAlertError         =   1,              // SYSLOG name LOG_ALERT    -   Action on the message must be taken immediately
                    eEmergency          =   0,              // SYSLOG name LOG_EMERG    -   The message says the system is unusable

                    Stroika_Define_Enum_Bounds(eEmergency, eDebug)
                };
            public:
                // EITHER RENAME AND DO ORDER DIFRNELY OR DONT TIE NNUMBERS TO SYSLOG NUMBERS
                Priority    fMinLogLevel_;  // SB PRIVATE
                // Get/Set LogLevel - this affects what WE EAT INLINE versus passon


            public:
                /**
                 *  Log
                 *
                 *  Design Note:
                 *      The 'format' parameter must be defined a String, not const String&, because:
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
                WindowsEventLogAppender ();
            public:
                virtual void    Log (Priority logLevel, const String& message) override;
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

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Logger_h_
#define _Stroika_Foundation_Execution_Logger_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdarg>
#include <filesystem>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Streams/OutputStream.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Synchronized.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   Finish support for Windows Event Manager Log Appender -- WindowsEventLogAppender. Its
 *              printing some data, but very minimally and wrongly handling categories etc. Probably could get close
 *              by specifying hardwired/hacked values in the CTOR args.
 *
 *      @todo   sMsgSentMaybeSuppressed_->ClearOlderThan (sMsgSentMaybeSuppressed_->Ago (sMaxWindow_.load () * kCleanupFactor_));
 *              cleanup. Find a better way (maybe this goes in cache code or here? to clear old values)
 */

namespace Stroika::Foundation::Execution {

    using namespace Configuration;

    using Characters::String;

    /**
     *  \brief A simple/portable wrapper on syslog/log4j/WindowsEventlog, with features like throttling, de-duping, varargs, etc.
     *
     *  OVERVIEW:
     *      The point of the Logging Module is to provide a simple, portable wrapper on end-user-targetted
     *  application logging. This form of logging is the kind of logging you leave builtin to your application,
     *  and write focused on end-user readability. It is NOT (primarily) for debugging (for that -
     *  use the Stroika::Foundation::Debug::Trace module).
     *
     *      The Logger is a singleton object. It can be set at any number of application logging
     *  levels. And it will write information to the backend logger its setup with. But default -
     *  this is none.
     *
     *      To use the logger and actually get logging - pick a logger rep, and call
     *  SetAppender ();
     *
     *  \note   It is legal to have no appender, in which case logging is silently suppressed.
     *
     *      This logging API CANNOT be used before main () has started, or after main () has completed
     *  (because this requires a call to setup a logger).
     *
     *  \note Future Note
     *      Some future version MIGHT handle this through some static configuration mechanism,
     *  which might then allow this (such as environment variables, linked variables etc).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  \par Example Usage
     *      \code
     *          // Usually near the top of 'main()'
     *          Logger::Activator loggerActivation{Logger::Options{
     *              .fLogBufferingEnabled         = true,
     *              .fSuppressDuplicatesThreshold = 15s,
     *          }};
     *          if (dockerContainerFlag) {
     *              Logger::sThe.AddAppender (make_shared<Logger::StreamAppender> (FileOutputStream::New (STDOUT_FILENO, AdoptFDPolicy::eDisconnectOnDestruction));
     *          }
     *          else {
     *              #if qHas_Syslog
     *                  Logger::sThe.AddAppender (make_shared<Logger::SysLogAppender> ("Stroika-Sample-Service"sv));
     *              #elif qPlatform_Windows
     *                  Logger::sThe.AddAppender (make_shared<Logger::WindowsEventLogAppender> ("Stroika-Sample-Service"sv));
     *              #endif
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          Logger::sThe.Log (Logger::eError, "Failed to correct something important in file {}"_f, fileName);
     *      \endcode
     * 
     *  @see DbgTrace
     */
    class Logger final {
    public:
        // See syslog for enumeration of various targets/etc.

    public:
        class IAppenderRep;

    public:
        using IAppenderRepPtr [[deprecated ("Since Stroika v3.0d6 - just use shared_ptr<IAppenderRep>")]] = shared_ptr<IAppenderRep>;

    public:
#if qHas_Syslog
        class SysLogAppender;
#endif
        class FileAppender;
        class StreamAppender;
#if qPlatform_Windows
        class WindowsEventLogAppender;
#endif

    public:
        /**
         *  \note - Callers who use this must arrange for Logger::Activator to be created first (and destroyed after)
         *          uses.
         */
        static Logger sThe;

    public:
        struct Options;

    public:
        struct Activator;

    public:
        Logger (const Logger&) = delete;
        Logger (Logger&&)      = delete;

    private:
        Logger ();

    public:
        Logger& operator= (const Logger&) = delete;
        Logger& operator= (Logger&&)      = delete;

#if qDebug
    private:
        ~Logger ();
#endif

    public:
        [[deprecated ("Since Stroika v3.0d6 use GetAppenders - plural")]] nonvirtual shared_ptr<IAppenderRep> GetAppender () const
        {
            return GetAppenders ().FirstValue (nullptr);
        }

    public:
        [[deprecated ("Since Stroika v3.0d6 use SetAppenders - plural")]] nonvirtual void SetAppender (const shared_ptr<IAppenderRep>& rep)
        {
            SetAppenders (rep);
        }

    public:
        /**
         *  Note - all Stroika provided appenders are internally synchronized.
         */
        nonvirtual Traversal::Iterable<shared_ptr<IAppenderRep>> GetAppenders () const;

    public:
        /**
         *  Note - all Stroika provided appenders are internally synchronized.
         *
         *  However, user-defined appenders are assumed internally synchronized (threadsafe).
         * 
         *  \note require all appenders != nullptr, but if a single rep given, that can be nullptr (and interpreted as removing all).
         */
        nonvirtual void SetAppenders (const shared_ptr<IAppenderRep>& rep);
        nonvirtual void SetAppenders (const Traversal::Iterable<shared_ptr<IAppenderRep>>& appenders);

    public:
        /**
         *  As if calls GetAppenders/SetAppenders
         */
        nonvirtual void AddAppender (const shared_ptr<IAppenderRep>& rep);

    public:
        /**
         *
         *  Names are based on syslog from http://unix.superglobalmegacorp.com/Net2/newsrc/sys/syslog.h.html
         *  But the NUMBERS do NOT correspond!
         *
         *  Lower numbers are less interesting (debug) and higher numbers more important (higher priority).
         *
         *  \note   Configuration::DefaultNames<> supported
         */
        enum class Priority : uint8_t {
            eDebug         = 0, // The message is debug info (not a good use of syslog - consider using DbgTrace)
            eInfo          = 1, // The message is purely informational
            eNotice        = 2, // The message describes a normal but important event
            eWarning       = 3, // The message is a warning
            eError         = 4, // The message describes an error
            eCriticalError = 5, // The message states a critical condition
            eAlertError    = 6, // Action on the message must be taken immediately
            eEmergency     = 7, // The message says the system is unusable

            Stroika_Define_Enum_Bounds (eDebug, eEmergency)
        };

    public:
        static constexpr Priority eDebug         = Priority::eDebug;
        static constexpr Priority eInfo          = Priority::eInfo;
        static constexpr Priority eNotice        = Priority::eNotice;
        static constexpr Priority eWarning       = Priority::eWarning;
        static constexpr Priority eCriticalError = Priority::eCriticalError;
        static constexpr Priority eError         = Priority::eError;
        static constexpr Priority eAlertError    = Priority::eAlertError;
        static constexpr Priority eEmergency     = Priority::eEmergency;

    private:
        nonvirtual void Shutdown_ ();

    public:
        /**
         *  This defaults to eInfo. Messages of lower priority (e.g. eDebug) will not be logged to the underlying log
         *  system.
         *
         *  This is done inline, so in principle, the compiler may eliminate the calls.
         *
         *  The MIN is min inclusive.
         */
        nonvirtual Priority GetMinLogLevel () const;

    public:
        /**
         *      @see GetMinLogLevel
         */
        nonvirtual void SetMinLogLevel (Priority minLogLevel);

    public:
        /**
         *      @see GetMinLogLevel
         *      This determines if a call to Log() with this argument log-level would
         *      write anything.
         */
        nonvirtual bool WouldLog (Priority logLevel) const;

    public:
        /**
         *      Log buffering is DISABLED by default, since it has some cost. But if enabled, Log () calls
         *  queue an internal message, which another thread wakes up to write. This CAN be critical for performance
         *  reasons, so the caller can freely log things, and not have their thread blocked.
         *
         *      Beware, this feature CAN mean that something you log, wont make it out of the application if
         *  the application terminates before the log can be flushed.
         *
         *  \par Example Usage
         *      \code
         *          Log ("QUITTING");
         *          _exit (0);
         *      \endcode
         *
         *      probably won't get logged. To avoid this issue, call myLogger.Shutdown () (@see ShutdownSingleton)
         *      before quitting, or call Flush ();
         *
         *      In one application (open-embedded arm linux) I saw a 3ms latency before I added this (2014-05-30).
         */
        nonvirtual bool GetBufferingEnabled () const;

    public:
        /**
         *      @see GetBufferingEnabled ()
         */
        nonvirtual void SetBufferingEnabled (bool logBufferingEnabled);

    public:
        /**
         *      @see GetBufferingEnabled ()
         *
         *  This has no effect if the buffer is empty or buffering is disabled.
         */
        nonvirtual void Flush ();

    public:
        /**
         *      If GetSuppressDuplicates ().has_value (), then a sequence of N (N > 1) identical messages will be replaced with two messages,
         *      the second of which appears with the added label [N-2 suppressed].
         *
         *      The duration is the window of time after the last message we wait before emitting the
         *      last message. A good default for this might be 5 or 10 seconds.
         */
        nonvirtual optional<Time::Duration> GetSuppressDuplicates () const;

    public:
        /**
         *      @see GetSuppressDuplicates ()
         *      @see Options::fSuppressDuplicatesThreshold
         * 
         *  \par Example Usage
         *      \code
         *          Logger::sThe.SetSuppressDuplicates (nullopt);         // disable the feature
         *          Logger::sThe.SetSuppressDuplicates ("PT5M"_duration); // anything within 5 minutes suppress
         *          Logger::sThe.SetSuppressDuplicates (5min);            // ""
         *      \endcode
         */
        nonvirtual void SetSuppressDuplicates (const optional<Time::Duration>& suppressDuplicatesThreshold);

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
         *
         *  \par Example Usage
         *      \code
         *          Logger::sThe.Log (Logger::eError, "Failed to correct something important in file {}"_f, fileName);
         *      \endcode
         */
        [[deprecated ("Since Stroika v3.0d6 - use _f strings for Logging")]] void Log (Priority logLevel, const wchar_t* format, ...);
        template <typename CHAR_T, typename... ARGS>
        nonvirtual void Log (Priority logLevel, Characters::FormatString<CHAR_T> fmt, ARGS&&... args);

    private:
        nonvirtual void Log_ (Priority logLevel, const String& msg);

    private:
        struct Rep_;

    private:
        shared_ptr<Rep_> fRep_; // unsure if we want to use shared_ptr or unique_ptr but shared among threads so easiest that way
        Priority fMinLogLevel_{Priority::eInfo}; // Keep out of rep only so we can reference from inlines and put the Rep_ in the .cpp file for better hiding
    };

    /**
     */
    struct Logger::Options {
        optional<bool>           fLogBufferingEnabled;
        optional<Time::Duration> fSuppressDuplicatesThreshold;
        optional<Priority>       fMinLogLevel;
    };

    /**
     *  At most one such object may exist. When it does, Logger::sThe is active and usable. 
     *  Its illegal to make calls on Logger::sThe when there is no Activator active.
     * 
     *  \par Example Usage
     *      \code
     *          main () {
     *              ...
     *              // near the beginning, before Logger::sThe used
     *              Execution::Logger::Activator logMgrActivator;
     *      \endcode
     */
    struct Logger::Activator {
        Activator (const Options& options = {});
        Activator (const Activator&) = delete;
        ~Activator ();
    };

    /**
     */
    class Logger::IAppenderRep {
    public:
        virtual ~IAppenderRep () = default;

    public:
        virtual void Log (Priority logLevel, const String& message) = 0;
    };

#if qHas_Syslog
    /**
     */
    class Logger::SysLogAppender : public Logger::IAppenderRep {
    public:
        SysLogAppender (const String& applicationName);
        SysLogAppender (const String& applicationName, int facility);
        ~SysLogAppender ();

    public:
        virtual void Log (Priority logLevel, const String& message) override;

    private:
        string fApplicationName_; // must save like this because open-syslog appears to KEEP ahold of pointer (http://www.gnu.org/s/hello/manual/libc/openlog.html)
    };
#endif

    /**
     */
    class Logger::StreamAppender : public Logger::IAppenderRep {
    public:
        StreamAppender (const Streams::OutputStream::Ptr<byte>& out);
        StreamAppender (const Streams::OutputStream::Ptr<Characters::Character>& out);

    public:
        virtual void Log (Priority logLevel, const String& message) override;

    private:
        struct Rep_;
        shared_ptr<Rep_> fRep_;
    };

    /**
     */
    class Logger::FileAppender : public Logger::IAppenderRep {
    public:
        FileAppender (const filesystem::path& fileName, bool truncateOnOpen = true);

    public:
        virtual void Log (Priority logLevel, const String& message) override;

    private:
        struct Rep_;
        shared_ptr<Rep_> fRep_;
    };

#if qPlatform_Windows
    /**
     */
    class Logger::WindowsEventLogAppender : public Logger::IAppenderRep {
    public:
        WindowsEventLogAppender (const String& eventSourceName);

    public:
        virtual void Log (Priority logLevel, const String& message) override;

    private:
        String fEventSourceName_;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Logger.inl"

#endif /*_Stroika_Foundation_Execution_Logger_h_*/

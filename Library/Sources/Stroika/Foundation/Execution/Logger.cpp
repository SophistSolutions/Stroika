/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#if qHas_Syslog
#include <syslog.h>
#endif

#include "../Cache/SynchronizedCallerStalenessCache.h"
#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../Characters/ToString.h"
#include "../Debug/Trace.h"
#include "../IO/FileSystem/FileOutputStream.h"
#include "../Streams/TextWriter.h"
#include "../Time/DateTime.h"
#include "BlockingQueue.h"
#include "Common.h"
#include "Process.h"
#include "Synchronized.h"
#include "Thread.h"
#include "TimeOutException.h"

#include "Logger.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Configuration;
using namespace Stroika::Foundation::Execution;
using namespace IO::FileSystem;

using Characters::SDKString;
using Containers::Mapping;
using Time::Duration;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ******************************** Execution::Logger *****************************
 ********************************************************************************
 */
struct Logger::Rep_ : enable_shared_from_this<Logger::Rep_> {
    using PriorityAndMessageType_ = pair<Logger::Priority, String>;
    bool                                   fBufferingEnabled_{false};
    Synchronized<IAppenderRepPtr>          fAppender_;
    BlockingQueue<PriorityAndMessageType_> fOutMsgQ_;
    // @todo FIX - fOutQMaybeNeedsFlush_ setting can cause race - maybe lose this optimization - pretty harmless, but can lose a message
    // race at end of Flush_()
    bool                             fOutQMaybeNeedsFlush_{true}; // slight optimization when using buffering
    Synchronized<optional<Duration>> fSuppressDuplicatesThreshold_;

    struct LastMsgInfoType_ {
        Time::TimePointSeconds fLastSentAt{};
        unsigned int           fRepeatCount_{};
    };
    Synchronized<Mapping<PriorityAndMessageType_, LastMsgInfoType_>> fLastMessages_; // if suppressing duplicates, must save all messages in timerange of suppression to compare with, and track suppression counts

    Synchronized<Execution::Thread::Ptr>                                  fBookkeepingThread_;
    atomic<Time::DurationSeconds>                                         fMaxWindow_{};
    Cache::SynchronizedCallerStalenessCache<pair<Priority, String>, bool> fMsgSentMaybeSuppressed_;

    Rep_ ()
    {
    }
    void FlushSuppressedDuplicates_ (bool forceEvenIfNotOutOfDate = false)
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Logger::Rep_::FlushSuppressedDuplicates_"};
#endif
        shared_ptr<IAppenderRep> tmp            = fAppender_; // avoid races and critical sections (appender internally threadsafe)
        auto                     lastMsgsLocked = fLastMessages_.rwget ();
        /**
         *  @todo restructure so we dont hold the lock while we call the tmp->Log() - since that append could in principle do something calling us back/deadlock
         *        Maybe queue up the messages/and push them at the end of the loop. Advantage of no deadlock, but disavantage of
         *        there being a window where messages could get lost (error on tmp->Log - not sure we can handle that anyhow).
         *          -- LGP 2022-08-24
         */
        if (not lastMsgsLocked->empty ()) {
            Time::Duration suppressDuplicatesThreshold = fSuppressDuplicatesThreshold_.cget ()->value_or (0s);
            for (auto i = lastMsgsLocked->begin (); i != lastMsgsLocked->end ();) {
                bool writeThisOne = forceEvenIfNotOutOfDate or i->fValue.fLastSentAt + suppressDuplicatesThreshold < Time::GetTickCount ();
                if (writeThisOne) {
                    switch (i->fValue.fRepeatCount_) {
                        case 0:
                            break; // nothing to write
                        case 1:
                            tmp->Log (i->fKey.first, i->fKey.second);
                            break;
                        default:
                            tmp->Log (i->fKey.first, Characters::Format (L"[%d duplicates suppressed]: %s", i->fValue.fRepeatCount_ - 1,
                                                                         i->fKey.second.As<wstring> ().c_str ()));
                            break;
                    }
                    lastMsgsLocked->Remove (i, &i);
                }
                else {
                    ++i;
                }
            }
        }
    }
    void Flush_ ()
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Logger::Rep_::Flush_"};
#endif
        shared_ptr<IAppenderRep> tmp = fAppender_; // avoid races and critical sections (between check and invoke)
        if (tmp != nullptr) {
            while (true) {
                optional<PriorityAndMessageType_> p = fOutMsgQ_.RemoveHeadIfPossible ();
                if (p.has_value ()) {
                    tmp->Log (p->first, p->second);
                }
                else {
                    return; // no more entries
                }
            }
        }
        fOutQMaybeNeedsFlush_ = false;
    }
    void UpdateBookkeepingThread_ ()
    {
        Debug::TraceContextBumper ctx{"Logger::Rep_::UpdateBookkeepingThread_"};
        {
            auto bktLck = fBookkeepingThread_.rwget ();
            if (bktLck.cref () != nullptr) {
                bktLck->AbortAndWaitForDone ();
                bktLck.store (Thread::Ptr{});
            }
        }

        Time::Duration      suppressDuplicatesThreshold = fSuppressDuplicatesThreshold_.cget ()->value_or (0s);
        bool                suppressDuplicates          = suppressDuplicatesThreshold > 0s;
        static const String kThreadName_{"Logger Bookkeeping"sv};
        if (suppressDuplicates or fBufferingEnabled_) {
            Thread::Ptr      newBookKeepThread;
            shared_ptr<Rep_> useRepInThread = shared_from_this (); // capture by value the shared_ptr
            if (suppressDuplicates) {
                newBookKeepThread = Thread::New (
                    [suppressDuplicatesThreshold, useRepInThread] () {
                        Debug::TraceContextBumper ctx1{"Logger::Rep_::UpdateBookkeepingThread_... internal thread/1"};
                        while (true) {
                            Duration time2Wait = max<Duration> (2s, suppressDuplicatesThreshold); // never wait less than this
                            useRepInThread->FlushSuppressedDuplicates_ ();
                            if (auto p = useRepInThread->fOutMsgQ_.RemoveHeadIfPossible (time2Wait)) {
                                shared_ptr<IAppenderRep> tmp = useRepInThread->fAppender_; // avoid races and critical sections (between check and invoke)
                                if (tmp != nullptr) {
                                    IgnoreExceptionsExceptThreadAbortForCall (tmp->Log (p->first, p->second));
                                }
                            }
                        }
                    },
                    kThreadName_);
            }
            else {
                newBookKeepThread = Thread::New (
                    [useRepInThread] () {
                        Debug::TraceContextBumper ctx1{"Logger::Rep_::UpdateBookkeepingThread_... internal thread/2"};
                        while (true) {
                            AssertNotNull (useRepInThread);
                            auto p = useRepInThread->fOutMsgQ_.RemoveHead ();
                            shared_ptr<IAppenderRep> tmp = useRepInThread->fAppender_; // avoid races and critical sections (between check and invoke)
                            if (tmp != nullptr) {
                                tmp->Log (p.first, p.second);
                            }
                        }
                    },
                    kThreadName_);
            }
            newBookKeepThread.SetThreadPriority (Thread::Priority::eBelowNormal);
            newBookKeepThread.Start ();
            fBookkeepingThread_ = newBookKeepThread;
        }

        // manually push out pending messages
        Flush_ ();
    }
};

Logger::Logger ()
    : fRep_{nullptr}
{
}

#if qDebug
Logger::~Logger ()
{
    Assert (&sThe == this);
    Assert (fRep_ == nullptr); // since Stroika 2.1.1, must use activator and destroy it before this is destroyed
}
#endif

void Logger::Shutdown_ ()
{
    Debug::TraceContextBumper ctx{"Logger::Shutdown"};
    // @todo FIX to assure all shutdown properly...
    // But this is OK for now pragmatically
#if 1
    // see https://stroika.atlassian.net/browse/STK-917
    bool changed = false;
    RequireNotNull (fRep_); // not yet destroyed
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fRep_->fSuppressDuplicatesThreshold_};
        if (fRep_->fSuppressDuplicatesThreshold_.load ()) {
            fRep_->fSuppressDuplicatesThreshold_.store (nullopt);
            changed = true;
        }
    }
    if (fRep_->fBufferingEnabled_) {
        fRep_->fBufferingEnabled_ = false;
        changed                   = true;
    }
    if (changed) {
        fRep_->UpdateBookkeepingThread_ ();
    }
#endif
    SetSuppressDuplicates (nullopt);
    SetBufferingEnabled (false);
    Flush ();
    Ensure (fRep_->fBookkeepingThread_.load () == nullptr);
}

auto Logger::GetAppender () const -> IAppenderRepPtr
{
    AssertNotNull (fRep_); // must be called while Logger::Activator exists
    return fRep_->fAppender_;
}

void Logger::SetAppender (const IAppenderRepPtr& rep)
{
    AssertNotNull (fRep_); // must be called while Logger::Activator exists
    fRep_->fAppender_ = rep;
}

void Logger::Log_ (Priority logLevel, const String& msg)
{
    AssertNotNull (fRep_);                            // must be called while Logger::Activator exists
    shared_ptr<IAppenderRep> tmp = fRep_->fAppender_; // avoid races/deadlocks and critical sections (between check and invoke)
    if (tmp != nullptr) {
        auto p = make_pair (logLevel, msg);
        if (fRep_->fSuppressDuplicatesThreshold_.cget ()->has_value ()) {
            auto lastMsgLocked = fRep_->fLastMessages_.rwget ();
            // @todo fix performance when we fix https://stroika.atlassian.net/browse/STK-928 -
            if (auto msgInfo = lastMsgLocked->Lookup (p)) {
                Rep_::LastMsgInfoType_ mi = *msgInfo;
                ++mi.fRepeatCount_;
                mi.fLastSentAt = Time::GetTickCount ();
                lastMsgLocked->Add (p, mi); // this is the 928 - part - above - that could be Update
                return;                     // so will be handled later
            }
            else {
                lastMsgLocked->Add (p, Rep_::LastMsgInfoType_{Time::GetTickCount ()}); // add empty one so we can recognize this as a DUP
            }
        }
        if (GetBufferingEnabled ()) {
            fRep_->fOutQMaybeNeedsFlush_ = true;
            fRep_->fOutMsgQ_.AddTail (p);
        }
        else {
            if (fRep_->fOutQMaybeNeedsFlush_) {
                fRep_->Flush_ (); // in case recently disabled
            }
            tmp->Log (p.first, p.second);
        }
    }
}

void Logger::SetBufferingEnabled (bool logBufferingEnabled)
{
    Debug::TraceContextBumper ctx{L"Logger::SetBufferingEnabled", L"logBufferingEnabled=%d", logBufferingEnabled};
    RequireNotNull (fRep_);
    if (fRep_->fBufferingEnabled_ != logBufferingEnabled) {
        fRep_->fBufferingEnabled_ = logBufferingEnabled;
        fRep_->UpdateBookkeepingThread_ ();
    }
}

void Logger::Flush ()
{
    Debug::TraceContextBumper ctx{"Logger::Flush"};
    RequireNotNull (fRep_);
    fRep_->Flush_ ();
}

bool Logger::GetBufferingEnabled () const
{
    RequireNotNull (fRep_);
    return fRep_->fBufferingEnabled_;
}

optional<Time::Duration> Logger::GetSuppressDuplicates () const
{
    RequireNotNull (fRep_);
    return fRep_->fSuppressDuplicatesThreshold_.load ();
}

void Logger::SetSuppressDuplicates (const optional<Duration>& suppressDuplicatesThreshold)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        L"Logger::SetSuppressDuplicates", L"suppressDuplicatesThreshold=%s", Characters::ToString (suppressDuplicatesThreshold).c_str ())};
    Require (not suppressDuplicatesThreshold.has_value () or *suppressDuplicatesThreshold > 0.0s);
    RequireNotNull (fRep_); // not destroyed
    [[maybe_unused]] auto&& critSec = lock_guard{fRep_->fSuppressDuplicatesThreshold_};
    if (fRep_->fSuppressDuplicatesThreshold_ != suppressDuplicatesThreshold) {
        fRep_->fSuppressDuplicatesThreshold_ = suppressDuplicatesThreshold;
        fRep_->UpdateBookkeepingThread_ ();
    }
}

#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
void Logger::Log (Priority logLevel, const wchar_t* format, ...)
{
    va_list argsList;
    va_start (argsList, format);
    String msg = Characters::FormatV (format, argsList);
    va_end (argsList);
    DbgTrace (L"Logger::Log (%s, \"%s\")", Characters::ToString (logLevel).c_str (), msg.c_str ());
    if (WouldLog (logLevel)) {
        Log_ (logLevel, msg);
    }
    else {
        DbgTrace (L"...suppressed by WouldLog");
    }
}
#endif

#if qHas_Syslog
/*
 ********************************************************************************
 ************************ Execution::SysLogAppender *****************************
 ********************************************************************************
 */
namespace {
    string mkMsg_ (const String& applicationName)
    {
        return Characters::CString::Format ("%s[%d]", applicationName.AsNarrowSDKString (Characters::eIgnoreErrors).c_str (), GetCurrentProcessID ());
    }
}
Logger::SysLogAppender::SysLogAppender (const String& applicationName)
    : fApplicationName_{mkMsg_ (applicationName)}
{
    ::openlog (fApplicationName_.c_str (), 0, LOG_DAEMON); // not sure what facility to pass?
}

Logger::SysLogAppender::SysLogAppender (const String& applicationName, int facility)
    : fApplicationName_{mkMsg_ (applicationName)}
{
    ::openlog (fApplicationName_.c_str (), 0, facility);
}

Logger::SysLogAppender::~SysLogAppender ()
{
    ::closelog ();
}

void Logger::SysLogAppender::Log (Priority logLevel, const String& message)
{
    DbgTrace (L"SYSLOG: %s: %s", DefaultNames<Logger::Priority> ().GetName (logLevel), message.As<wstring> ().c_str ());
    int sysLogLevel = LOG_NOTICE; // doesn't matter cuz assert error if hit
    switch (logLevel) {
        case Priority::eDebug:
            sysLogLevel = LOG_DEBUG;
            break;
        case Priority::eInfo:
            sysLogLevel = LOG_INFO;
            break;
        case Priority::eNotice:
            sysLogLevel = LOG_NOTICE;
            break;
        case Priority::eWarning:
            sysLogLevel = LOG_WARNING;
            break;
        case Priority::eError:
            sysLogLevel = LOG_ERR;
            break;
        case Priority::eCriticalError:
            sysLogLevel = LOG_CRIT;
            break;
        case Priority::eAlertError:
            sysLogLevel = LOG_ALERT;
            break;
        case Priority::eEmergency:
            sysLogLevel = LOG_EMERG;
            break;
        default:
            RequireNotReached ();
    }
    // According to http://pubs.opengroup.org/onlinepubs/000095399/functions/xsh_chap02_09.html#tag_02_09_01 this is threadsafe
    ::syslog (sysLogLevel, "%s", message.AsNarrowSDKString (Characters::eIgnoreErrors).c_str ());
}
#endif

/*
 ********************************************************************************
 ************************** Execution::StreamAppender ***************************
 ********************************************************************************
 */
struct Logger::StreamAppender::Rep_ {
public:
    template <typename T>
    Rep_ (const T& out)
        : fWriter_ (Streams::TextWriter::New (out))
    {
    }
    void Log (Priority logLevel, const String& message)
    {
        //@todo tmphack - write date and write logLevel??? and use TextStream API that does \r or \r\n as appropriate
        fWriter_.rwget ()->Write (Characters::Format (L"[%5s][%16s] %s\n", Configuration::DefaultNames<Logger::Priority>{}.GetName (logLevel),
                                                      Time::DateTime::Now ().Format ().c_str (), message.As<wstring> ().c_str ()));
    }

private:
    Synchronized<Streams::OutputStream<Characters::Character>::Ptr> fWriter_; // All Stroika-provided appenders must be internally synchronized - https://stroika.atlassian.net/browse/STK-610
};

Logger::StreamAppender::StreamAppender (const Streams::OutputStream<byte>::Ptr& out)
    : fRep_ (make_shared<Rep_> (out))
{
}

Logger::StreamAppender::StreamAppender (const Streams::OutputStream<Characters::Character>::Ptr& out)
    : fRep_ (make_shared<Rep_> (out))
{
}

void Logger::StreamAppender::Log (Priority logLevel, const String& message)
{
    fRep_->Log (logLevel, message);
}

/*
 ********************************************************************************
 ************************** Execution::FileAppender *****************************
 ********************************************************************************
 */
struct Logger::FileAppender::Rep_ {
public:
    Rep_ (const filesystem::path& fileName, bool truncateOnOpen)
        : fOut_ (StreamAppender (FileOutputStream::New (fileName, truncateOnOpen ? FileOutputStream::eStartFromStart : FileOutputStream::eAppend)))
    {
    }
    void Log (Priority logLevel, const String& message)
    {
        fOut_.Log (logLevel, message);
    }

private:
    StreamAppender fOut_; // no need to synchronize since our Logger::StreamAppender class is internally synchronized
};

Logger::FileAppender::FileAppender (const filesystem::path& fileName, bool truncateOnOpen)
    : fRep_ (make_shared<Rep_> (fileName, truncateOnOpen))
{
}

void Logger::FileAppender::Log (Priority logLevel, const String& message)
{
    fRep_->Log (logLevel, message);
}

#if qPlatform_Windows
/*
 ********************************************************************************
 ********************** Execution::WindowsEventLogAppender **********************
 ********************************************************************************
 */
Logger::WindowsEventLogAppender::WindowsEventLogAppender (const String& eventSourceName)
    : fEventSourceName_{eventSourceName}
{
}

void Logger::WindowsEventLogAppender::Log (Priority logLevel, const String& message)
{
    /*
     * VERY QUICK HACK - AT LEAST DUMPS SOME INFO TO EVENTLOG - BUT MUCH TWEAKING LEFT TODO
     */
    WORD eventType = EVENTLOG_ERROR_TYPE;
    switch (logLevel) {
        case Priority::eDebug:
            eventType = EVENTLOG_INFORMATION_TYPE;
            break;
        case Priority::eInfo:
            eventType = EVENTLOG_INFORMATION_TYPE;
            break;
        case Priority::eNotice:
            eventType = EVENTLOG_INFORMATION_TYPE;
            break;
        case Priority::eWarning:
            eventType = EVENTLOG_WARNING_TYPE;
            break;
        case Priority::eError:
            eventType = EVENTLOG_ERROR_TYPE;
            break;
        case Priority::eAlertError:
            eventType = EVENTLOG_ERROR_TYPE;
            break;
        case Priority::eEmergency:
            eventType = EVENTLOG_ERROR_TYPE;
            break;
    }
#define CATEGORY_Normal 0x00000001L
    WORD eventCategoryID = CATEGORY_Normal;
// See SPR#565 for wierdness - where I cannot really get these paid attention to
// by the Windows EventLog. So had to use the .Net eventlogger. It SEEMS
#define EVENT_Message 0x00000064L
    const DWORD kEventID     = EVENT_Message;
    HANDLE      hEventSource = ::RegisterEventSource (NULL, fEventSourceName_.AsSDKString ().c_str ());
    Verify (hEventSource != NULL);
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([hEventSource] () noexcept { Verify (::DeregisterEventSource (hEventSource)); });
    SDKString               tmp     = message.AsSDKString ();
    const Characters::SDKChar* msg  = tmp.c_str ();
    Verify (::ReportEvent (hEventSource, eventType, eventCategoryID, kEventID, NULL, (WORD)1, 0, &msg, NULL));
}
#endif

/*
 ********************************************************************************
 ************************** Logger::Activator ***********************************
 ********************************************************************************
 */
Logger::Activator::Activator (const Options& options)
{
    Debug::TraceContextBumper ctx{"Logger::Activator::Activator"};
    Assert (sThe.fRep_ == nullptr); // only one acivator object at a time
    sThe.fRep_ = make_shared<Rep_> ();
    sThe.SetSuppressDuplicates (options.fSuppressDuplicatesThreshold);
    if (options.fLogBufferingEnabled) {
        sThe.SetBufferingEnabled (*options.fLogBufferingEnabled);
    }
    if (options.fMinLogLevel) {
        sThe.SetMinLogLevel (*options.fMinLogLevel);
    }
}

Logger::Activator::~Activator ()
{
    Debug::TraceContextBumper ctx{"Logger::Activator::~Activator"};
    Assert (sThe.fRep_ != nullptr); // this is the only way it gets cleared so better not be null
    sThe.Shutdown_ ();              // @todo maybe cleanup this code now that we have activator architecture?
    sThe.fRep_.reset ();
}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qHas_Syslog
#include    <syslog.h>
#endif

#include    "../Cache/CallerStalenessCache.h"
#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Characters/ToString.h"
#include    "../Debug/Trace.h"
#include    "BlockingQueue.h"
#include    "Common.h"
#include    "Process.h"
#include    "Synchronized.h"
#include    "Thread.h"
#include    "TimeOutException.h"
#include    "../IO/FileSystem/FileOutputStream.h"
#include    "../Streams/TextWriter.h"
#include    "../Time/DateTime.h"

#include    "Logger.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;
using   namespace   Stroika::Foundation::Execution;

using   Memory::Optional;
using   Time::DurationSecondsType;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
#if     qCompilerAndStdLib_constexpr_Buggy
            template    <>
            const EnumNames<Logger::Priority>   DefaultNames<Logger::Priority>::k
#if     qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy
                =
#endif
            {
                EnumNames<Logger::Priority>::BasicArrayInitializer {
                    {
                        { Logger::Priority::eDebug, L"Debug" },
                        { Logger::Priority::eInfo, L"Info" },
                        { Logger::Priority::eNotice, L"Notice" },
                        { Logger::Priority::eWarning, L"Warning" },
                        { Logger::Priority::eError, L"Error" },
                        { Logger::Priority::eCriticalError, L"CriticalError" },
                        { Logger::Priority::eAlertError, L"AlertError" },
                        { Logger::Priority::eEmergency, L"Emergency" },
                    }
                }
            };
#else
            constexpr   EnumNames<Logger::Priority>    DefaultNames<Logger::Priority>::k;
#endif
        }
    }
}






/*
 ********************************************************************************
 ******************************** Execution::Logger *****************************
 ********************************************************************************
 */
Logger  Logger::sThe_;

struct  Logger::Rep_ : enable_shared_from_this<Logger::Rep_> {
    bool                                                                    fBufferingEnabled_              { false };
    Synchronized<IAppenderRepPtr>                                           fAppender_;
    BlockingQueue<pair<Logger::Priority, String>>                           fOutMsgQ_;
    bool                                                                    fOutQMaybeNeedsFlush_           { true };       // slight optimization of not using buffering
    Synchronized<Memory::Optional<DurationSecondsType>>                     fSuppressDuplicatesThreshold_;

    struct LastMsg_ {
        pair<Logger::Priority, String>      fLastMsgSent_   {};
        Time::DurationSecondsType           fLastSentAt     {};
        unsigned int                        fRepeatCount_   {};
    };
    Synchronized<LastMsg_>                                                  fLastMsg_;

    Synchronized<Execution::Thread>                                         fBookkeepingThread_;
    atomic<Time::DurationSecondsType>                                       fMaxWindow_ {};
    Synchronized<Cache::CallerStalenessCache<pair<Priority, String>, bool>> fMsgSentMaybeSuppressed_;

    void    FlushDupsWarning_ ()
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Logger::Rep_::FlushDupsWarning_");
#endif
        auto    lastMsgLocked = fLastMsg_.GetReference ();
        if (lastMsgLocked->fRepeatCount_ > 0) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"fLastMsg_.fRepeatCount_ = %d", lastMsgLocked->fRepeatCount_);
#endif
            shared_ptr<IAppenderRep> tmp =   fAppender_;   // avoid races and critical sections (appender internally threadsafe)
            if (tmp != nullptr) {
                if (lastMsgLocked->fRepeatCount_ == 1) {
                    tmp->Log (lastMsgLocked->fLastMsgSent_.first, lastMsgLocked->fLastMsgSent_.second);
                }
                else {
                    tmp->Log (lastMsgLocked->fLastMsgSent_.first,  Characters::Format (L"[%d duplicates suppressed]: %s", lastMsgLocked->fRepeatCount_ - 1, lastMsgLocked->fLastMsgSent_.second.c_str ()));
                }
            }
            lastMsgLocked->fRepeatCount_ = 0;
            lastMsgLocked->fLastMsgSent_.second.clear ();
        }
    }
    void        FlushBuffer_ ()
    {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("Logger::Rep_::FlushBuffer_");
#endif
        shared_ptr<IAppenderRep> tmp =   fAppender_;   // avoid races and critical sections (between check and invoke)
        if (tmp != nullptr) {
            while (true) {
                Optional<pair<Logger::Priority, String>> p = fOutMsgQ_.RemoveHeadIfPossible ();
                if (p.IsPresent ()) {
                    tmp->Log (p->first, p->second);
                }
                else {
                    return; // no more entries
                }
            }
        }
        fOutQMaybeNeedsFlush_ = false;
    }
    void    UpdateBookkeepingThread_ ()
    {
        Debug::TraceContextBumper ctx ("Logger::Rep_::UpdateBookkeepingThread_");
        {
            auto bktLck =      fBookkeepingThread_.get ();
            bktLck->AbortAndWaitForDone ();
            bktLck.store (Thread ());  // so null
        }

        Time::DurationSecondsType   suppressDuplicatesThreshold =   fSuppressDuplicatesThreshold_->Value (0);
        bool                        suppressDuplicates          =   suppressDuplicatesThreshold > 0;
        if (suppressDuplicates or fBufferingEnabled_) {
            Thread              newBookKeepThread;
            shared_ptr<Rep_>    useRepInThread = shared_from_this (); // capture by value the shared_ptr
            if (suppressDuplicates) {
                newBookKeepThread = Thread ([suppressDuplicatesThreshold, useRepInThread] () {
                    Debug::TraceContextBumper ctx1 ("Logger::Rep_::UpdateBookkeepingThread_... internal thread/1");
                    while (true) {
                        DurationSecondsType time2Wait = max (static_cast<DurationSecondsType> (2), suppressDuplicatesThreshold);    // never wait less than this
                        if (auto p = useRepInThread->fOutMsgQ_.RemoveHeadIfPossible (time2Wait)) {
                            shared_ptr<IAppenderRep> tmp =   useRepInThread->fAppender_;   // avoid races and critical sections (between check and invoke)
                            if (tmp != nullptr) {
                                IgnoreExceptionsExceptThreadAbortForCall (tmp->Log (p->first, p->second));
                            }
                        }
                        {
                            auto    lastMsgLocked = useRepInThread->fLastMsg_.GetReference ();
                            if (lastMsgLocked->fRepeatCount_ > 0 and lastMsgLocked->fLastSentAt + suppressDuplicatesThreshold < Time::GetTickCount ()) {
                                IgnoreExceptionsExceptThreadAbortForCall (useRepInThread->FlushDupsWarning_ ());
                            }
                        }
                    }
                });
            }
            else {
                newBookKeepThread = Thread ([useRepInThread] () {
                    Debug::TraceContextBumper ctx1 ("Logger::Rep_::UpdateBookkeepingThread_... internal thread/2");
                    while (true) {
                        AssertNotNull (useRepInThread);
                        auto p = useRepInThread->fOutMsgQ_.RemoveHead ();
                        shared_ptr<IAppenderRep> tmp =   useRepInThread->fAppender_;   // avoid races and critical sections (between check and invoke)
                        if (tmp != nullptr) {
                            tmp->Log (p.first, p.second);
                        }
                    }
                });
            }
            newBookKeepThread.SetThreadName (L"Logger Bookkeeping");
            newBookKeepThread.SetThreadPriority (Thread::Priority::eBelowNormal);
            newBookKeepThread.Start ();
            fBookkeepingThread_ = newBookKeepThread;
        }

        // manually push out pending messages
        FlushBuffer_ ();
    }

};

Logger::Logger ()
    : fRep_ { make_shared<Rep_> () }
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    if (&sThe_ == this) {
        DbgTrace (L"Allocating global logger: %p", this);
    }
#endif
}

#if     qDebug
Logger::~Logger ()
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    if (&sThe_ == this) {
        DbgTrace (L"Destroying global logger: %p", this);
    }
#endif
    RequireNotNull (fRep_);
    fRep_.reset ();   // so more likely debug checks for null say its null
}
#endif

Logger::IAppenderRepPtr Logger::GetAppender () const
{
    RequireNotNull (fRep_);
    return fRep_->fAppender_;
}

void    Logger::SetAppender (const shared_ptr<IAppenderRep>& rep)
{
    RequireNotNull (fRep_);
    fRep_->fAppender_ = rep;
}

void    Logger::Log_ (Priority logLevel, const String& msg)
{
    AssertNotNull (fRep_);
    shared_ptr<IAppenderRep> tmp =   fRep_->fAppender_;   // avoid races and critical sections (between check and invoke)
    if (tmp != nullptr) {
        auto p = pair<Priority, String> (logLevel, msg);
        if (fRep_->fSuppressDuplicatesThreshold_->IsPresent ()) {
            auto    lastMsgLocked = fRep_->fLastMsg_.GetReference ();
            if (p == lastMsgLocked->fLastMsgSent_) {
                lastMsgLocked->fRepeatCount_++;
                lastMsgLocked->fLastSentAt = Time::GetTickCount ();
                return; // so will be handled later
            }
            else {
                if (lastMsgLocked->fRepeatCount_ > 0) {
                    fRep_->FlushDupsWarning_ ();
                }
                lastMsgLocked->fLastMsgSent_ = p;
                lastMsgLocked->fRepeatCount_ = 0;
                lastMsgLocked->fLastSentAt = Time::GetTickCount ();
            }
        }
        if (GetBufferingEnabled ()) {
            fRep_->fOutQMaybeNeedsFlush_ = true;
            fRep_->fOutMsgQ_.AddTail (p);
        }
        else {
            if (fRep_->fOutQMaybeNeedsFlush_) {
                fRep_->FlushBuffer_ (); // in case recently disabled
            }
            tmp->Log (p.first, p.second);
        }
    }
}

void        Logger::SetBufferingEnabled (bool logBufferingEnabled)
{
    Debug::TraceContextBumper ctx ("Logger::SetBufferingEnabled");
    DbgTrace (L"(logBufferingEnabled=%d)", logBufferingEnabled);
    RequireNotNull (fRep_);
    if (fRep_->fBufferingEnabled_ != logBufferingEnabled) {
        fRep_->fBufferingEnabled_ = logBufferingEnabled;
        fRep_->UpdateBookkeepingThread_ ();
    }
}

void        Logger::FlushBuffer ()
{
    RequireNotNull (fRep_);
    fRep_->FlushBuffer_ ();
}

bool        Logger::GetBufferingEnabled () const
{
    RequireNotNull (fRep_);
    return fRep_->fBufferingEnabled_;
}

Memory::Optional<Time::DurationSecondsType> Logger::GetSuppressDuplicates () const
{
    RequireNotNull (fRep_);
    return Memory::Optional<Time::DurationSecondsType> (fRep_->fSuppressDuplicatesThreshold_);
}

void    Logger::SetSuppressDuplicates (const Memory::Optional<DurationSecondsType>& suppressDuplicatesThreshold)
{
    Debug::TraceContextBumper ctx ("Logger::SetSuppressDuplicates");
    DbgTrace (L"(suppressDuplicatesThreshold=%f)", suppressDuplicatesThreshold.Value (-1));
    Require (suppressDuplicatesThreshold.IsMissing () or * suppressDuplicatesThreshold > 0.0);
    RequireNotNull (fRep_); // not destroyed
    auto    critSec { Execution::make_unique_lock (fRep_->fSuppressDuplicatesThreshold_) };
    if (fRep_->fSuppressDuplicatesThreshold_ != suppressDuplicatesThreshold) {
        fRep_->fSuppressDuplicatesThreshold_ = suppressDuplicatesThreshold;
        fRep_->UpdateBookkeepingThread_ ();
    }
}

#if     qDefaultTracingOn
void    Logger::Log (Priority logLevel, String format, ...)
{
    va_list     argsList;
    va_start (argsList, format);
    String      msg     =   Characters::FormatV (format.c_str (), argsList);
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

void    Logger::LogIfNew (Priority logLevel, Time::DurationSecondsType suppressionTimeWindow, String format, ...)
{
    Require (suppressionTimeWindow > 0);
    RequireNotNull (fRep_);
    fRep_->fMaxWindow_.store (max (suppressionTimeWindow, fRep_->fMaxWindow_.load ()));   // doesn't need to be synchronized
    va_list     argsList;
    va_start (argsList, format);
    String      msg     =   Characters::FormatV (format.c_str (), argsList);
    va_end (argsList);
    DbgTrace (L"Logger::LogIfNew (%s, %f, \"%s\")", Characters::ToString (logLevel).c_str (), suppressionTimeWindow, msg.c_str ());
    if (WouldLog (logLevel)) {
        if (fRep_->fMsgSentMaybeSuppressed_->Lookup (pair<Priority, String> { logLevel, msg }, fRep_->fMsgSentMaybeSuppressed_->Ago (suppressionTimeWindow), false)) {
            DbgTrace (L"...suppressed by fMsgSentMaybeSuppressed_->Lookup ()");
        }
        else {
            Log_ (logLevel, msg);
            fRep_->fMsgSentMaybeSuppressed_->Add (pair<Priority, String> { logLevel, msg }, true);
        }
    }
    else {
        DbgTrace (L"...suppressed by WouldLog");
    }
    /*
     *  Spend a modicum of effort, so that at least very old strings are purged. This limits how large a cache sMsgSentMaybeSuppressed_
     *  can become.
     */
    constexpr   double  kCleanupFactor_ { 2.0 };
    fRep_->fMsgSentMaybeSuppressed_->ClearOlderThan (fRep_->fMsgSentMaybeSuppressed_->Ago (fRep_->fMaxWindow_.load () * kCleanupFactor_));
}






#if     qHas_Syslog
/*
 ********************************************************************************
 ************************ Execution::SysLogAppender *****************************
 ********************************************************************************
 */
namespace   {
    string mkMsg_ (const String& applicationName)
    {
        return Characters::CString::Format ("%s[%d]", applicationName.AsNarrowSDKString ().c_str (), GetCurrentProcessID ());
    }
}
Logger::SysLogAppender::SysLogAppender (const String& applicationName)
    : fApplicationName_ (mkMsg_ (applicationName))
{
    ::openlog (fApplicationName_.c_str (), 0, LOG_DAEMON);    // not sure what facility to pass?
}

Logger::SysLogAppender::SysLogAppender (const String& applicationName, int facility)
    : fApplicationName_ (mkMsg_ (applicationName))
{
    ::openlog (fApplicationName_.c_str (), 0, facility);
}

Logger::SysLogAppender::~SysLogAppender ()
{
    ::closelog ();
}

void    Logger::SysLogAppender::Log (Priority logLevel, const String& message)
{
    DbgTrace (L"SYSLOG: %s: %s", DefaultNames<Logger::Priority> ().GetName (logLevel), message.c_str ());
    int sysLogLevel = LOG_NOTICE;   // doesnt matter cuz assert error if hit
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
    ::syslog (sysLogLevel, "%s", message.AsNarrowSDKString ().c_str ());
}
#endif





/*
 ********************************************************************************
 ************************** Execution::StreamAppender ***************************
 ********************************************************************************
 */
struct  Logger::StreamAppender::Rep_ {
    using   TextWriter = Streams::TextWriter;
public:
    template    <typename T>
    Rep_ (const Streams::OutputStream<T>& out)
        : fWriter_ (out)
    {
    }
    void    Log (Priority logLevel, const String& message)
    {
        //@todo tmphack - write date and write logLevel??? and use TextStream API that does \r or \r\n as appropriate
        fWriter_->Write (Characters::Format (L"[%5s][%16s] %s\n", Configuration::DefaultNames<Logger::Priority>::k.GetName (logLevel), Time::DateTime::Now ().Format ().c_str (), message.c_str ()));
    }
private:
    Synchronized<TextWriter>    fWriter_;   // All Stroika-provided appenders must be internally synchonized
};

Logger::StreamAppender::StreamAppender (const Streams::OutputStream<Byte>& out)
    : fRep_ (make_shared<Rep_> (out))
{
}

Logger::StreamAppender::StreamAppender (const Streams::OutputStream<Characters::Character>& out)
    : fRep_ (make_shared<Rep_> (out))
{
}

void    Logger::StreamAppender::Log (Priority logLevel, const String& message)
{
    fRep_->Log (logLevel, message);
}





/*
 ********************************************************************************
 ************************** Execution::FileAppender *****************************
 ********************************************************************************
 */
struct  Logger::FileAppender::Rep_ {
    using   FileOutputStream = IO::FileSystem::FileOutputStream;
public:
    Rep_ (const String& fileName, bool truncateOnOpen)
        : fOut_ (StreamAppender (FileOutputStream::mk (fileName, truncateOnOpen ? FileOutputStream::eStartFromStart : FileOutputStream::eAppend)))
    {
    }
    void    Log (Priority logLevel, const String& message)
    {
        fOut_.Log (logLevel, message);
    }
private:
    StreamAppender    fOut_;        // no need to synchronize since our Logger::StreamAppender class is internally synchonized
};

Logger::FileAppender::FileAppender (const String& fileName, bool truncateOnOpen)
    : fRep_ (make_shared<Rep_> (fileName, truncateOnOpen))
{
}

void    Logger::FileAppender::Log (Priority logLevel, const String& message)
{
    fRep_->Log (logLevel, message);
}






#if     qPlatform_Windows
/*
 ********************************************************************************
 ************************ Execution::SysLogAppender *****************************
 ********************************************************************************
 */
Logger::WindowsEventLogAppender::WindowsEventLogAppender (const String& eventSourceName)
    : fEventSourceName_ (eventSourceName)
{
}

void    Logger::WindowsEventLogAppender::Log (Priority logLevel, const String& message)
{
    /*
     * VERY QUICK HACK - AT LEAST DUMPS SOME INFO TO EVENTLOG - BUT MUCH TWEAKING LEFT TODO
     */
    WORD    eventType   =   EVENTLOG_ERROR_TYPE;
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
#define CATEGORY_Normal                  0x00000001L
    WORD    eventCategoryID =   CATEGORY_Normal;
    // See SPR#565 for wierdness - where I cannot really get these paid attention to
    // by the Windows EventLog. So had to use the .Net eventlogger. It SEEMS
#define EVENT_Message                    0x00000064L
    const   DWORD   kEventID            =   EVENT_Message;
    HANDLE  hEventSource = ::RegisterEventSource (NULL, fEventSourceName_.AsSDKString ().c_str ());
    Verify (hEventSource != NULL);
    Finally                     cleanup ([hEventSource] ()      { Verify (::DeregisterEventSource (hEventSource)); });
    SDKString                   tmp = message.AsSDKString ();
    const Characters::SDKChar*  msg = tmp.c_str ();
    Verify (::ReportEvent (
                hEventSource,
                eventType,
                eventCategoryID,
                kEventID,
                NULL,
                (WORD)1,
                0,
                &msg,
                NULL
            )
           );
}
#endif

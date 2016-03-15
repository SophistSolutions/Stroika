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
#if     qSUPPORT_LEGACY_Stroika_Enum_Names
const EnumNames<Logger::Priority>   Logger::Stroika_Enum_Names(Priority) = DefaultNames<Logger::Priority>::k;
#endif






/*
 ********************************************************************************
 ******************************** Execution::Logger *****************************
 ********************************************************************************
 */
Logger  Logger::sThe_;




namespace {
    BlockingQueue<pair<Logger::Priority, String>>       sOutMsgQ_;
    Execution::Thread                                   sBookkeepingThread_;
    bool                                                sOutQMaybeNeedsFlush_ = true;       // sligt optimziation of not using buffering

    Synchronized<Memory::Optional<DurationSecondsType>> sSuppressDuplicatesThreshold_;

    struct LastMsg_ {
        pair<Logger::Priority, String>      fLastMsgSent_   {};
        Time::DurationSecondsType           fLastSentAt     {};
        unsigned int                        fRepeatCount_   {};
    };
    Synchronized<LastMsg_>    sLastMsg_;
}


Logger::Logger ()
    : fAppender_ ()
    , fMinLogLevel_ (Priority::eInfo)
    , fBufferingEnabled_ (false)
#if     qDebug
    , fConstructed_ (true)
#endif
{
}

#if     qDebug
Logger::~Logger ()
{
    fConstructed_ = false;
}
#endif

void    Logger::SetAppender (const shared_ptr<IAppenderRep>& rep)
{
    fAppender_ = rep;
}

void    Logger::Log_ (Priority logLevel, const String& msg)
{
    Require (sThe_.fConstructed_);
    shared_ptr<IAppenderRep> tmp =   Get ().GetAppender ();   // avoid races and critical sections
    if (tmp != nullptr) {
        auto p = pair<Priority, String> (logLevel, msg);
        if (sSuppressDuplicatesThreshold_->IsPresent ()) {
            auto    lastMsgLocked = sLastMsg_.GetReference ();
            if (p == lastMsgLocked->fLastMsgSent_) {
                lastMsgLocked->fRepeatCount_++;
                lastMsgLocked->fLastSentAt = Time::GetTickCount ();
                return; // so will be handled later
            }
            else {
                if (lastMsgLocked->fRepeatCount_ > 0) {
                    FlushDupsWarning_ ();
                }
                lastMsgLocked->fLastMsgSent_ = p;
                lastMsgLocked->fRepeatCount_ = 0;
                lastMsgLocked->fLastSentAt = Time::GetTickCount ();
            }
        }
        if (GetBufferingEnabled ()) {
            sOutQMaybeNeedsFlush_ = true;
            sOutMsgQ_.AddTail (p);
        }
        else {
            if (sOutQMaybeNeedsFlush_) {
                FlushBuffer (); // in case recently disabled
            }
            tmp->Log (p.first, p.second);
        }
    }
}

void        Logger::SetBufferingEnabled (bool logBufferingEnabled)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Logger::SetBufferingEnabled");
    DbgTrace (L"(logBufferingEnabled=%d)", logBufferingEnabled);
#endif
    sThe_.fBufferingEnabled_ = logBufferingEnabled;
    UpdateBookkeepingThread_ ();
}

void        Logger::FlushBuffer ()
{
    Require (sThe_.fConstructed_);
    shared_ptr<IAppenderRep> tmp =   Get ().GetAppender ();   // avoid races and critical sections
    if (tmp != nullptr) {
        while (true) {
            Optional<pair<Logger::Priority, String>> p = sOutMsgQ_.RemoveHeadIfPossible ();
            if (p.IsPresent ()) {
                tmp->Log (p->first, p->second);
            }
            else {
                return; // no more entries
            }
        }
    }
    sOutQMaybeNeedsFlush_ = false;
}

Memory::Optional<Time::DurationSecondsType> Logger::GetSuppressDuplicates ()
{
    return Memory::Optional<Time::DurationSecondsType> (sSuppressDuplicatesThreshold_);
}

void    Logger::SetSuppressDuplicates (const Memory::Optional<DurationSecondsType>& suppressDuplicatesThreshold)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Logger::SetSuppressDuplicates");
    DbgTrace (L"(suppressDuplicatesThreshold=%f)", suppressDuplicatesThreshold.Value (-1));
#endif
    Require (suppressDuplicatesThreshold.IsMissing () or * suppressDuplicatesThreshold > 0.0);
    auto    critSec { Execution::make_unique_lock (sSuppressDuplicatesThreshold_) };
    if (sSuppressDuplicatesThreshold_ != suppressDuplicatesThreshold) {
        sSuppressDuplicatesThreshold_ = suppressDuplicatesThreshold;
        UpdateBookkeepingThread_ ();
    }
}

void    Logger::FlushDupsWarning_ ()
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Logger::FlushDupsWarning_");
#endif
    auto    lastMsgLocked = sLastMsg_.GetReference ();
    if (lastMsgLocked->fRepeatCount_ > 0) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"sLastMsg_.fRepeatCount_ = %d", lastMsgLocked->fRepeatCount_);
#endif
        Require (sThe_.fConstructed_);
        shared_ptr<IAppenderRep> tmp =   Get ().GetAppender ();   // avoid races and critical sections
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

void    Logger::UpdateBookkeepingThread_ ()
{
    sBookkeepingThread_.AbortAndWaitForDone ();
    sBookkeepingThread_ = Thread ();  // so null

    Time::DurationSecondsType   suppressDuplicatesThreshold =   sSuppressDuplicatesThreshold_->Value (0);
    bool                        suppressDuplicates          =   suppressDuplicatesThreshold > 0;
    if (suppressDuplicates or GetBufferingEnabled ()) {
        if (suppressDuplicates) {
            sBookkeepingThread_ = Thread ([suppressDuplicatesThreshold] () {
                while (true) {
                    DurationSecondsType time2Wait = max (static_cast<DurationSecondsType> (2), suppressDuplicatesThreshold);    // never wait less than this
                    /// Not ready fo prime time because BlockinqQ RemoveHeadIfPossible currently ignores time2Wait... Must fix that first...
                    if (auto p = sOutMsgQ_.RemoveHeadIfPossible (time2Wait)) {
                        shared_ptr<IAppenderRep> tmp =   Get ().GetAppender ();   // avoid races and critical sections
                        if (tmp != nullptr) {
                            IgnoreExceptionsExceptThreadAbortForCall (tmp->Log (p->first, p->second));
                        }
                    }
                    {
                        auto    lastMsgLocked = sLastMsg_.GetReference ();
                        if (lastMsgLocked->fRepeatCount_ > 0 and lastMsgLocked->fLastSentAt + suppressDuplicatesThreshold < Time::GetTickCount ()) {
                            IgnoreExceptionsExceptThreadAbortForCall (FlushDupsWarning_ ());
                        }
                    }
                }
            });
        }
        else {
            sBookkeepingThread_ = Thread ([] () {
                while (true) {
                    auto p = sOutMsgQ_.RemoveHead ();
                    shared_ptr<IAppenderRep> tmp =   Get ().GetAppender ();   // avoid races and critical sections
                    if (tmp != nullptr) {
                        tmp->Log (p.first, p.second);
                    }
                }
            });
        }
        sBookkeepingThread_.SetThreadName (L"Logger Bookkeeping");
        sBookkeepingThread_.SetThreadPriority (Thread::Priority::eBelowNormal);
        sBookkeepingThread_.Start ();
    }

    // manually push out pending messages
    FlushBuffer ();
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
    static  atomic<Time::DurationSecondsType>                                       sMaxWindow_ {};
    sMaxWindow_.store (max (suppressionTimeWindow, sMaxWindow_.load ()));   // doesn't need to be synchronized
    static  Synchronized<Cache::CallerStalenessCache<pair<Priority, String>, bool>> sMsgSentMaybeSuppressed_;
    va_list     argsList;
    va_start (argsList, format);
    String      msg     =   Characters::FormatV (format.c_str (), argsList);
    va_end (argsList);
    DbgTrace (L"Logger::LogIfNew (%s, %f, \"%s\")", Characters::ToString (logLevel).c_str (), suppressionTimeWindow, msg.c_str ());
    if (WouldLog (logLevel)) {
        if (sMsgSentMaybeSuppressed_->Lookup (pair<Priority, String> { logLevel, msg }, sMsgSentMaybeSuppressed_->Ago (suppressionTimeWindow), false)) {
            DbgTrace (L"...suppressed by sMsgSentMaybeSuppressed_->Lookup ()");
        }
        else {
            Log_ (logLevel, msg);
            sMsgSentMaybeSuppressed_->Add (pair<Priority, String> { logLevel, msg }, true);
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
    sMsgSentMaybeSuppressed_->ClearOlderThan (sMsgSentMaybeSuppressed_->Ago (sMaxWindow_.load () * kCleanupFactor_));
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
    Rep_ (Streams::OutputStream<T> out)
        : fWriter_ (out)
    {
    }
    void    Log (Priority logLevel, const String& message)
    {
        //@todo tmphack - write date and write logLevel??? and use TextStream API that does \r or \r\n as appropriate
        fWriter_.Write (Characters::Format (L"[%5s][%16s] %s\n", Configuration::DefaultNames<Logger::Priority>::k.GetName (logLevel), Time::DateTime::Now ().Format ().c_str (), message.c_str ()));
    }
private:
    TextWriter          fWriter_;
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
    StreamAppender    fOut_;
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
    HANDLE  hEventSource = RegisterEventSource (NULL, fEventSourceName_.AsSDKString ().c_str ());
    Verify (hEventSource != NULL);
    SDKString tmp = message.AsSDKString ();
    const Characters::SDKChar* msg = tmp.c_str ();
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
    Verify (::DeregisterEventSource (hEventSource));
}
#endif

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qHas_Syslog
#include    <syslog.h>
#endif

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/Format.h"
#include    "../Debug/Trace.h"
#include    "BlockingQueue.h"
#include    "Common.h"
#include    "Process.h"
#include    "Thread.h"
#include    "TimeOutException.h"

#include    "Logger.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;
using   namespace   Stroika::Foundation::Execution;

using   Time::DurationSecondsType;



/*
 ********************************************************************************
 ******************************** Execution::Logger *****************************
 ********************************************************************************
 */
Logger  Logger::sThe_;

const EnumNames<Logger::Priority>   Logger::Stroika_Enum_Names(Priority) = {
    { Logger::Priority::eDebug, L"Debug" },
    { Logger::Priority::eInfo, L"Info" },
    { Logger::Priority::eNotice, L"Notice" },
    { Logger::Priority::eWarning, L"Warning" },
    { Logger::Priority::eError, L"Error" },
    { Logger::Priority::eCriticalError, L"CriticalError" },
    { Logger::Priority::eAlertError, L"AlertError" },
    { Logger::Priority::eEmergency, L"Emergency" },
};



namespace {
    BlockingQueue<pair<Logger::Priority, String>>   sOutMsgQ_;
    Execution::Thread                               sBookkeepingThread_;
    bool                                            sOutQMaybeNeedsFlush_ = true;       // sligt optimziation of not using buffering

    Synchronized<Memory::Optional<DurationSecondsType>>       sSuppressDuplicatesThreshold_;

    struct LastMsg_ {
        mutex                               fMutex_;     // mutex so we can update related variables together
        pair<Logger::Priority, String>      fLastMsgSent_;
        Time::DurationSecondsType           fLastSentAt = 0.0;
        unsigned int                        fRepeatCount_ = 0;
    };
    LastMsg_    sLastMsg_;
}


Logger::Logger ()
    : fAppender_ ()
    , fMinLogLevel_ (Priority::eInfo)
    , fBufferingEnabled_ (false)
{
}

void    Logger::SetAppender (const shared_ptr<IAppenderRep>& rep)
{
    fAppender_ = rep;
}

void    Logger::Log_ (Priority logLevel, const String& format, va_list argList)
{
    shared_ptr<IAppenderRep> tmp =   sThe_.fAppender_;   // avoid races and critical sections
    if (tmp.get () != nullptr) {
        auto p = pair<Logger::Priority, String> (logLevel, Characters::FormatV (format.c_str (), argList));
        if (sSuppressDuplicatesThreshold_.IsPresent ()) {
            auto    critSec { make_unique_lock (sLastMsg_.fMutex_) };
            if (p == sLastMsg_.fLastMsgSent_) {
                sLastMsg_.fRepeatCount_++;
                sLastMsg_.fLastSentAt = Time::GetTickCount ();
                return; // so will be handled later
            }
            else {
                if (sLastMsg_.fRepeatCount_ > 0) {
                    FlushDupsWarning_ ();
                }
                sLastMsg_.fLastMsgSent_ = p;
                sLastMsg_.fRepeatCount_ = 0;
                sLastMsg_.fLastSentAt = Time::GetTickCount ();
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
    sThe_.fBufferingEnabled_ = logBufferingEnabled;
    UpdateBookkeepingThread_ ();
}

void        Logger::FlushBuffer ()
{
    shared_ptr<IAppenderRep> tmp =   sThe_.fAppender_;   // avoid races and critical sections
    if (tmp != nullptr) {
        while (true) {
            auto p = sOutMsgQ_.RemoveHeadIfPossible ();
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
    Require (suppressDuplicatesThreshold.IsMissing () or * suppressDuplicatesThreshold > 0.0);
    if (sSuppressDuplicatesThreshold_ != suppressDuplicatesThreshold) {
        sSuppressDuplicatesThreshold_ = suppressDuplicatesThreshold;
        UpdateBookkeepingThread_ ();
    }
}

void    Logger::FlushDupsWarning_ ()
{
    if (sLastMsg_.fRepeatCount_ > 0) {
        shared_ptr<IAppenderRep> tmp =   sThe_.fAppender_;   // avoid races and critical sections
        if (tmp != nullptr) {
            if (sLastMsg_.fRepeatCount_ == 1) {
                tmp->Log (sLastMsg_.fLastMsgSent_.first, sLastMsg_.fLastMsgSent_.second);
            }
            else {
                tmp->Log (sLastMsg_.fLastMsgSent_.first,  Characters::Format (L"[%d duplicates suppressed]: %s", sLastMsg_.fRepeatCount_ - 1, sLastMsg_.fLastMsgSent_.second.c_str ()));
            }
        }
        sLastMsg_.fRepeatCount_ = 0;
        sLastMsg_.fLastMsgSent_.second.clear ();
    }
}

void    Logger::UpdateBookkeepingThread_ ()
{
    sBookkeepingThread_.AbortAndWaitForDone ();
    sBookkeepingThread_ = Thread ();  // so null

    Time::DurationSecondsType   suppressDuplicatesThreshold =   sSuppressDuplicatesThreshold_.Value (0);
    bool                        suppressDuplicates          =   suppressDuplicatesThreshold > 0;
    if (suppressDuplicates or GetBufferingEnabled ()) {
        if (suppressDuplicates) {
            sBookkeepingThread_ = Thread ([suppressDuplicatesThreshold] () {
                while (true) {
                    DurationSecondsType time2Wait = max (static_cast<DurationSecondsType> (2), suppressDuplicatesThreshold);    // never wait less than this
                    try {
                        auto p = sOutMsgQ_.RemoveHead (time2Wait);
                        shared_ptr<IAppenderRep> tmp =   sThe_.fAppender_;   // avoid races and critical sections
                        if (tmp != nullptr) {
                            tmp->Log (p.first, p.second);
                        }
                    }
                    catch (const TimeOutException&) {
                    }
                    {
                        auto    critSec { make_unique_lock (sLastMsg_.fMutex_) };
                        if (sLastMsg_.fRepeatCount_ > 0 and sLastMsg_.fLastSentAt + suppressDuplicatesThreshold < Time::GetTickCount ()) {
                            FlushDupsWarning_ ();
                        }
                    }
                }
            });
        }
        else {
            sBookkeepingThread_ = Thread ([] () {
                while (true) {
                    auto p = sOutMsgQ_.RemoveHead ();
                    shared_ptr<IAppenderRep> tmp =   sThe_.fAppender_;   // avoid races and critical sections
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






/*
 ********************************************************************************
 ************************** Execution::IAppenderRep *****************************
 ********************************************************************************
 */
Logger::IAppenderRep::~IAppenderRep ()
{
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
    openlog (fApplicationName_.c_str (), 0, LOG_DAEMON);    // not sure what facility to pass?
}

Logger::SysLogAppender::SysLogAppender (const String& applicationName, int facility)
    : fApplicationName_ (mkMsg_ (applicationName))
{
    openlog (fApplicationName_.c_str (), 0, facility);
}

Logger::SysLogAppender::~SysLogAppender ()
{
    closelog ();
}

void    Logger::SysLogAppender::Log (Priority logLevel, const String& message)
{
    DbgTrace (L"SYSLOG: %s: %s", Stroika_Enum_Names(Priority).GetName (logLevel), message.c_str ());
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
            sysLogLevel = LOG_ERROR;
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
    syslog (sysLogLevel, "%s", message.AsNarrowSDKString ().c_str ());
}
#endif






/*
 ********************************************************************************
 ************************** Execution::FileAppender *****************************
 ********************************************************************************
 */
Logger::FileAppender::FileAppender (const String& fileName)
{
    AssertNotImplemented ();
}

void    Logger::FileAppender::Log (Priority logLevel, const String& message)
{
    AssertNotImplemented ();
}






#if     qPlatform_Windows
/*
 ********************************************************************************
 ************************ Execution::SysLogAppender *****************************
 ********************************************************************************
 */
Logger::WindowsEventLogAppender::WindowsEventLogAppender ()
{
}

void    Logger::WindowsEventLogAppender::Log (Priority logLevel, const String& message)
{
    /*
     * VERY QUICK HACK - AT LEAST DUMPS SOME INFO TO EVENTLOG - BUT MUCH TWEAKING LEFT TODO
     */
    const   TCHAR   kEventSourceName[]  =   _T ("xxxtest");
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
    HANDLE  hEventSource = RegisterEventSource (NULL, kEventSourceName);
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
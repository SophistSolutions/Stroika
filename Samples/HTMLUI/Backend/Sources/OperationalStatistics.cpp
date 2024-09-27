/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Common/Property.h"
// #include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Math/Statistics.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "OperationalStatistics.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using Memory::BLOB;
using Stroika::Foundation::Common::ConstantProperty;
using Stroika::Foundation::Common::GUID;
// using Stroika::Foundation::DataExchange::ObjectVariantMapper;

using namespace Stroika::Samples::HTMLUI;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

// @todo Lose DIGEST code and use new UUID::CreateNew () method when available.

/*
 ********************************************************************************
 ***************** OperationalStatisticsMgr::ProcessAPICmd **********************
 ********************************************************************************
 */
OperationalStatisticsMgr::ProcessAPICmd::~ProcessAPICmd ()
{
    Time::TimePointSeconds now{Time::GetTickCount ()};
    sThe.Add_ (Rec_{Rec_::Kind::eAPI, now, now - fStart_});
}

void OperationalStatisticsMgr::ProcessAPICmd::NoteError ()
{
    Time::TimePointSeconds now{Time::GetTickCount ()};
    sThe.Add_ (Rec_{.fKind = Rec_::Kind::eAPIError, .fAt = now, .fDuration = 0s});
}

/*
 ********************************************************************************
 ***************************** OperationalStatisticsMgr *************************
 ********************************************************************************
 */
void OperationalStatisticsMgr::RecordActiveRunningTasksCount (size_t length)
{
    Time::TimePointSeconds now{Time::GetTickCount ()};
    sThe.Add_ (Rec_{.fKind = Rec_::Kind::eAPIActiveRunningTasks, .fAt = now, .fDuration = 0s, .fLength = length});
}

void OperationalStatisticsMgr::RecordOpenConnectionCount (size_t length)
{
    Time::TimePointSeconds now{Time::GetTickCount ()};
    sThe.Add_ (Rec_{.fKind = Rec_::Kind::eAPIOpenConnectionCount, .fAt = now, .fDuration = 0s, .fLength = length});
}

void OperationalStatisticsMgr::RecordProcessingConnectionCount (size_t length)
{
    Time::TimePointSeconds now{Time::GetTickCount ()};
    sThe.Add_ (Rec_{.fKind = Rec_::Kind::eAPIProcessingConnectionCount, .fAt = now, .fDuration = 0s, .fLength = length});
}

auto OperationalStatisticsMgr::GetStatistics () const -> Statistics
{
    Statistics result;

    using Time::Duration;
    using Time::DurationSeconds;
    using Time::TimePointSeconds;
    // hit every entry and just skip those with null events
    TimePointSeconds skipBefore = Time::GetTickCount () - kLookbackInterval;

    // could optimize slightly and skip a bunch in a row, but not worht the trouble probably
    Iterable<Rec_> allApplicable = [&] () {
        lock_guard lk{fMutex_};
        return Sequence<Rec_>{begin (fRollingHistory_), end (fRollingHistory_)}.Where (
            [&] (const Rec_& r) { return r.fAt >= skipBefore and r.fKind != Rec_::Kind::eNull; });
    }();

    {
        Iterable<DurationSeconds> apiTimes = allApplicable.Map<Iterable<DurationSeconds>> ([] (const Rec_& r) -> optional<DurationSeconds> {
            if (r.fKind == Rec_::Kind::eAPI)
                return r.fDuration;
            return nullopt;
        });
        if (not apiTimes.empty ()) {
            result.fRecentAPI.fMeanDuration   = Duration{Math::Mean (apiTimes)};
            result.fRecentAPI.fMedianDuration = Duration{Math::Median (apiTimes)};
            result.fRecentAPI.fMaxDuration    = Duration{*apiTimes.Max ()};
        }
        result.fRecentAPI.fCallsCompleted = static_cast<unsigned int> (apiTimes.length ());
        result.fRecentAPI.fErrors =
            static_cast<unsigned int> (allApplicable.Count ([] (const Rec_& r) { return r.fKind == Rec_::Kind::eAPIError; }));
    }
    {
        Iterable<float> openWSConnections = allApplicable.Map<Iterable<float>> ([] (const Rec_& r) -> optional<float> {
            if (r.fKind == Rec_::Kind::eAPIOpenConnectionCount)
                return static_cast<float> (r.fLength);
            return nullopt;
        });
        if (not openWSConnections.empty ()) {
            result.fRecentAPI.fMedianWebServerConnections = Math::Median (openWSConnections);
        }
    }
    {
        Iterable<float> processingWSConnections = allApplicable.Map<Iterable<float>> ([] (const Rec_& r) -> optional<float> {
            if (r.fKind == Rec_::Kind::eAPIOpenConnectionCount)
                return static_cast<float> (r.fLength);
            return nullopt;
        });
        if (not processingWSConnections.empty ()) {
            result.fRecentAPI.fMedianProcessingWebServerConnections = Math::Median (processingWSConnections);
        }
    }
    {
        Iterable<float> activeRunningWSAPITasks = allApplicable.Map<Iterable<float>> ([] (const Rec_& r) -> optional<float> {
            if (r.fKind == Rec_::Kind::eAPIActiveRunningTasks)
                return static_cast<float> (r.fLength);
            return nullopt;
        });
        if (not activeRunningWSAPITasks.empty ()) {
            result.fRecentAPI.fMedianRunningAPITasks = Math::Median (activeRunningWSAPITasks);
        }
    }
    {
        Iterable<float> activeWSConnections = allApplicable.Map<Iterable<float>> ([] (const Rec_& r) -> optional<float> {
            if (r.fKind == Rec_::Kind::eAPIOpenConnectionCount)
                return static_cast<float> (r.fLength);
            return nullopt;
        });
        if (not activeWSConnections.empty ()) {
            result.fRecentAPI.fMedianProcessingWebServerConnections = Math::Median (activeWSConnections);
        }
    }
    return result;
}
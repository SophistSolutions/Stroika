/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/StroikaVersion.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "Stroika/Frameworks/SystemPerformance/Capturer.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/CPU.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/Memory.h"
#include "Stroika/Frameworks/SystemPerformance/Instruments/Process.h"
#include "Stroika/Frameworks/SystemPerformance/Measurement.h"
#include "Stroika/Frameworks/WebService/OpenAPI/Specification.h"

#if qStroika_HasComponent_boost
#include <boost/version.hpp>
#endif
#if qStroika_HasComponent_OpenSSL
#include <openssl/opensslv.h>
#endif

#include "AppVersion.h"

#include "OperationalStatistics.h"

#include "WSImpl.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::Streams;

using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::SystemPerformance;

using IO ::Network::HTTP::ClientErrorException;

using namespace Stroika::Samples::HTMLUI;

using IO::Network::URI;
using Memory::BLOB;

namespace {
    namespace Resources_ {
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wunused-variable\"");
        constexpr
#include "api.json.embed"
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wunused-variable\"");
    }
}

namespace {
    const Duration     kCaptureFrequency_ = 30s;
    struct MyCapturer_ final : Capturer {
    public:
        Instruments::CPU::Instrument     fCPUInstrument{};
        Instruments::Process::Instrument fProcessInstrument{
            Instruments::Process::Options{.fRestrictToPIDs = Set<pid_t>{Execution::GetCurrentProcessID ()}}};
        MyCapturer_ ()
        {
            AddCaptureSet (CaptureSet{kCaptureFrequency_, {fCPUInstrument, fProcessInstrument}});
        }
    };
}

/*
 ********************************************************************************
 ************************************* WSImpl ***********************************
 ********************************************************************************
 */
struct WSImpl::Rep_ {
    MyCapturer_                                       fMyCapturer;
    function<void (const WithWebServerCallbackType&)> fAccessWebServer;
};
WSImpl::WSImpl (function<void (const WithWebServerCallbackType&)> passWS2Callback)
    : fRep_{make_shared<Rep_> ()}
{
    fRep_->fAccessWebServer = passWS2Callback;
}

OpenAPI::Specification WSImpl::GetOpenAPISpecification () const
{
    static const auto kSpec_ = OpenAPI::Specification{BLOB::Attach (Resources_::api_json), OpenAPI::kMediaType};
    return kSpec_;
}

About WSImpl::about_GET () const
{
    OperationalStatisticsMgr::ProcessAPICmd statsGather;
    using APIServerInfo   = About::APIServerInfo;
    using ComponentInfo   = APIServerInfo::ComponentInfo;
    using OperatingSystem = APIServerInfo::OperatingSystem;
    using CurrentMachine  = APIServerInfo::CurrentMachine;
    using CurrentProcess  = APIServerInfo::CurrentProcess;
    using APIEndpoint     = APIServerInfo::APIEndpoint;
    using Database        = APIServerInfo::Database;

    static const Sequence<ComponentInfo> kAPIServerComponents_{initializer_list<ComponentInfo>{
        ComponentInfo{"Stroika"sv, Version{kStroika_Version_FullVersion}.AsPrettyVersionString (), URI{"https://github.com/SophistSolutions/Stroika"sv}}
#if qStroika_HasComponent_boost
        ,
        ComponentInfo{"boost"sv, String{BOOST_LIB_VERSION}}
#endif
#if qStroika_HasComponent_OpenSSL
        ,
        ComponentInfo{"OpenSSL"sv, OPENSSL_VERSION_TEXT, URI{"https://www.openssl.org/"sv}}
#endif
    }};
    auto now = DateTime::Now ();
    auto measurements = fRep_->fMyCapturer.pMostRecentMeasurements (); // capture results on a regular cadence with MyCapturer, and just report the latest stats

    CurrentMachine machineInfo = [this, now, &measurements] () {
        CurrentMachine    result;
        static const auto kOS_  = OperatingSystem{GetSystemConfiguration_ActualOperatingSystem ().fTokenName,
                                                 GetSystemConfiguration_ActualOperatingSystem ().fPrettyNameWithVersionDetails};
        result.fOperatingSystem = kOS_;
        if (auto o = GetSystemConfiguration_BootInformation ().fBootedAt) {
            result.fMachineUptime = now - *o;
        }
        if (auto om = fRep_->fMyCapturer.fCPUInstrument.MeasurementAs<Instruments::CPU::Info> (measurements)) {
            result.fRunQLength    = om->fRunQLength;
            result.fTotalCPUUsage = om->fTotalCPUUsage;
        }
        return result;
    }();

    CurrentProcess processInfo = [this, now, &measurements] () {
        CurrentProcess result;
        if (auto om = fRep_->fMyCapturer.fProcessInstrument.MeasurementAs<Instruments::Process::Info> (measurements)) {
            Assert (om->size () == 1);
            Instruments::Process::ProcessType thisProcess = (*om)[Execution::GetCurrentProcessID ()];
            if (auto o = thisProcess.fProcessStartedAt) {
                result.fProcessUptime = now - *o;
            }
            result.fAverageCPUTimeUsed = thisProcess.fAverageCPUTimeUsed ? thisProcess.fAverageCPUTimeUsed->count () : optional<double>{};
            result.fWorkingOrResidentSetSize = Memory::NullCoalesce (thisProcess.fWorkingSetSize, thisProcess.fResidentMemorySize);
            result.fCombinedIOReadRate       = thisProcess.fCombinedIOReadRate;
            result.fCombinedIOWriteRate      = thisProcess.fCombinedIOWriteRate;
            DbgTrace ("capturing PROCESSINFO to report - from stroika capturer thisProcess={}"_f, thisProcess);
            DbgTrace ("and resulting reported result={}"_f, result);
        }
        return result;
    }();

    OperationalStatisticsMgr::Statistics stats    = OperationalStatisticsMgr::sThe.GetStatistics ();
    APIEndpoint                          apiStats = [&] () {
        APIEndpoint r;
        r.fCallsCompleted = stats.fRecentAPI.fCallsCompleted;
        r.fCallTimes      = CommonStatistics<Duration>{
                                          .fMax = stats.fRecentAPI.fMaxDuration, .fMean = stats.fRecentAPI.fMeanDuration, .fMedian = stats.fRecentAPI.fMedianDuration};
        r.fErrors                               = stats.fRecentAPI.fErrors;
        r.fMedianWebServerConnections           = stats.fRecentAPI.fMedianWebServerConnections;
        r.fMedianProcessingWebServerConnections = stats.fRecentAPI.fMedianProcessingWebServerConnections;
        r.fMedianRunningAPITasks                = stats.fRecentAPI.fMedianRunningAPITasks;
        return r;
    }();
    APIServerInfo::WebServer webServerStats = [&] () {
        About::APIServerInfo::WebServer r;
        fRep_->fAccessWebServer ([&] (const Stroika::Frameworks::WebServer::ConnectionManager& cm) {
            Stroika::Frameworks::WebServer::ConnectionManager::Statistics rr = cm.statistics ();
            r.fThreadPool.fThreads = static_cast<unsigned int> (rr.fThreadPool.fThreadEntryCount); // todo beginning of data to report
            r.fThreadPool.fTasksStillQueued                   = rr.fThreadPool.fNumberOfTasksAdded - rr.fThreadPool.fNumberOfTasksCompleted;
            r.fThreadPool.fAverageTaskRunTime                 = rr.fThreadPool.GetMeanTimeConsumed ();
            r.fConnections.fNumberOfOpenConnections           = rr.fConnections.fNumberOfOpenConnections;
            r.fConnections.fNumberOfActiveConnections         = rr.fConnections.fNumberOfActiveConnections;
            r.fConnections.fDurationOfOpenConnections         = rr.fConnections.fDurationOfOpenConnections;
            r.fConnections.fDurationOfOpenConnectionsRequests = rr.fConnections.fDurationOfOpenConnectionsRequests;
            r.fConnections.fDurationOfActiveConnectionsRequests = rr.fConnections.fDurationOfActiveConnectionsRequests;
            r.fConnections.fConnectionsPiningForTheFjords       = rr.fConnections.fConnectionsPiningForTheFjords;
            return r;
        });
        return r;
    }();
    optional<Database> dbStats; // no DB in this demo
    auto               healthcheck = healthcheck_GET ();

    return About{AppVersion::kVersion,
                 APIServerInfo{AppVersion::kVersion, kAPIServerComponents_, machineInfo, processInfo, apiStats, webServerStats, dbStats}, healthcheck};
}

HealthStatus WSImpl::healthcheck_GET () const
{
    HealthStatus result;
    fRep_->fAccessWebServer ([&] (const Stroika::Frameworks::WebServer::ConnectionManager& cm) {
        Stroika::Frameworks::WebServer::ConnectionManager::Statistics rr = cm.statistics ();
        if (rr.fConnections.fConnectionsPiningForTheFjords != 0) {
            // add warnings; grab connections and add warnings for bad ones...
            Sequence<String> warnings;
            warnings += "connectionsPiningForTheFjords: {}"_f(rr.fConnections.fConnectionsPiningForTheFjords);
            auto connections = cm.connections ();
            auto now         = Time::GetTickCount ();
            for (auto c : connections) {
                if (c.fActive == true and c.fMostRecentMessage) {
                    Duration d = c.fMostRecentMessage->ReplaceEnd (min (c.fMostRecentMessage->GetUpperBound (), now)).GetDistanceSpanned ();
                    if (d >= cm.options ().fConnectionPiningForTheFjordsDelay) {
                        warnings += "connection: {}"_f(c);
                    }
                }
            }
            result.fWarnings = warnings;
        }
        result.fOK = true; // @todo add period interval check for webserver stats - and report to LOGGER when bad as well
    });
    return result;
}

TypedBLOB WSImpl::resource_GET (const String& name) const
{
    using namespace IO::Network::HTTP;
    if (name == "api.json"sv) {
        return TypedBLOB{.fData = GetOpenAPISpecification ().As (Frameworks::WebService::OpenAPI::kMediaType),
            .fType = Frameworks::WebService::OpenAPI::kMediaType,
        };
    }
    Execution::Throw (ClientErrorException{StatusCodes::kNotFound});
}

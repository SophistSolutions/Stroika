/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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

#if qHasFeature_boost
#include <boost/version.hpp>
#endif
#if qHasFeature_OpenSSL
#include <openssl/opensslv.h>
#endif

#include "AppVersion.h"

#include "OperationalStatistics.h"

#include "WSImpl.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

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
    MyCapturer_                                  fMyCapturer;
    function<About::APIServerInfo::WebServer ()> fWebServerStatsFetcher;
};
WSImpl::WSImpl (function<About::APIServerInfo::WebServer ()> webServerStatsFetcher)
    : fRep_{make_shared<Rep_> ()}
{
    fRep_->fWebServerStatsFetcher = webServerStatsFetcher;
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
#if qHasFeature_boost
        ,
        ComponentInfo{"boost"sv, String{BOOST_LIB_VERSION}}
#endif
#if qHasFeature_OpenSSL
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
        r.fCallsCompleted                       = stats.fRecentAPI.fCallsCompleted;
        r.fMeanDuration                         = stats.fRecentAPI.fMeanDuration;
        r.fMedianDuration                       = stats.fRecentAPI.fMedianDuration;
        r.fMaxDuration                          = stats.fRecentAPI.fMaxDuration;
        r.fErrors                               = stats.fRecentAPI.fErrors;
        r.fMedianWebServerConnections           = stats.fRecentAPI.fMedianWebServerConnections;
        r.fMedianProcessingWebServerConnections = stats.fRecentAPI.fMedianProcessingWebServerConnections;
        r.fMedianRunningAPITasks                = stats.fRecentAPI.fMedianRunningAPITasks;
        return r;
    }();
    APIServerInfo::WebServer webServerStats = [&] () { return fRep_->fWebServerStatsFetcher (); }();
    optional<Database>       dbStats; // no DB in this demo

    return About{AppVersion::kVersion,
                 APIServerInfo{AppVersion::kVersion, kAPIServerComponents_, machineInfo, processInfo, apiStats, webServerStats, dbStats}};
}

tuple<BLOB, InternetMediaType> WSImpl::resource_GET (const String& name) const
{
    using namespace IO::Network::HTTP;
    if (name == "api.json"sv) {
        return make_tuple (GetOpenAPISpecification ().As (Frameworks::WebService::OpenAPI::kMediaType), Frameworks::WebService::OpenAPI::kMediaType);
    }
    Execution::Throw (ClientErrorException{StatusCodes::kNotFound});
}

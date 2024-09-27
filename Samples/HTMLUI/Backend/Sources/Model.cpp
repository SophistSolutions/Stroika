/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Set.h"

#include "Model.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Samples::HTMLUI;
using namespace Stroika::Samples::HTMLUI::Model;

/*
 ********************************************************************************
 ************ Model::About::APIServerInfo::OperatingSystem **********************
 ********************************************************************************
 */
String About::APIServerInfo::OperatingSystem::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "majorOSCategory: "sv << fMajorOSCategory << ", "sv;
    sb << "fullVersionedOSName: "sv << fFullVersionedOSName;
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper About::APIServerInfo::OperatingSystem::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper.AddClass<OperatingSystem> ({
        {"majorOSCategory"sv, &OperatingSystem::fMajorOSCategory},
        {"fullVersionedName"sv, &OperatingSystem::fFullVersionedOSName},
    });
    return mapper;
}();

String About::APIServerInfo::ComponentInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Name: "sv << fName << ", "sv;
    sb << "Version: "sv << fVersion << ", "sv;
    sb << "URL: "sv << fURL;
    sb << "}"sv;
    return sb;
}

String About::APIServerInfo::CurrentMachine::ToString () const
{
    StringBuilder sb;
    sb << "{";
    sb << "Operating-System: " << fOperatingSystem << ", "sv;
    sb << "Machine-Uptime: " << fMachineUptime << ", "sv;
    sb << "Total-CPU-Usage: " << fTotalCPUUsage << ", "sv;
    sb << "Run-Q-Length: " << fRunQLength;
    sb << "}"sv;
    return sb;
}

String About::APIServerInfo::CurrentProcess::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "ProcessUptime: "sv << fProcessUptime << ", "sv;
    sb << "AverageCPUTimeUsed: "sv << fAverageCPUTimeUsed << ", "sv;
    sb << "WorkingOrResidentSetSize: "sv << fWorkingOrResidentSetSize << ", "sv;
    sb << "CombinedIOReadRate: "sv << fCombinedIOReadRate << ", "sv;
    sb << "CombinedIOWriteRate: "sv << fCombinedIOWriteRate;
    sb << "}"sv;
    return sb;
}

String About::APIServerInfo::APIEndpoint::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "CallsCompleted: "sv << fCallsCompleted << ", "sv;
    sb << "Errors: "sv << fErrors << ", "sv;
    sb << "MedianDuration: "sv << fMedianDuration << ", "sv;
    sb << "MeanDuration: "sv << fMeanDuration << ", "sv;
    sb << "MaxDuration: "sv << fMaxDuration << ", "sv;
    sb << "MedianWebServerConnections: "sv << fMedianWebServerConnections << ", "sv;
    sb << "MedianProcessingWebServerConnections: "sv << fMedianProcessingWebServerConnections << ", "sv;
    sb << "MedianRunningAPITasks: "sv << fMedianRunningAPITasks;
    sb << "}"sv;
    return sb.str ();
}

String About::APIServerInfo::WebServer::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "ThreadPool: {"sv;
    sb << "Threads: " << fThreadPool.fThreads << ", "sv;
    sb << "TasksStillQueued: " << fThreadPool.fTasksStillQueued << ", "sv;
    sb << "AverageTaskRunTime: " << fThreadPool.fAverageTaskRunTime;
    sb << "}"sv;
    return sb.str ();
}

String About::APIServerInfo::Database::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Reads: "sv << fReads << ", "sv;
    sb << "Writes: "sv << fWrites << ", "sv;
    sb << "Errors: "sv << fErrors << ", "sv;
    sb << "MeanReadDuration: "sv << fMeanReadDuration << ", "sv;
    sb << "MedianReadDuration: "sv << fMedianReadDuration << ", "sv;
    sb << "MeanWriteDuration: "sv << fMeanWriteDuration << ", "sv;
    sb << "MedianWriteDuration: "sv << fMedianWriteDuration << ", "sv;
    sb << "MaxDuration: "sv << fMaxDuration << ", "sv;
    sb << "FileSize: "sv << fFileSize;
    sb << "}"sv;
    return sb;
}

String About::APIServerInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Version: "sv << fVersion << ", "sv;
    sb << "Component-Versions: "sv << fComponentVersions << ", "sv;
    sb << "Current-Machine: "sv << fCurrentMachine << ", "sv;
    sb << "Current-Process: "sv << fCurrentProcess << ", "sv;
    sb << "API-Endpoint: "sv << fAPIEndpoint << ", "sv;
    sb << "WebServer: "sv << fWebServer << ", "sv;
    sb << "Database: "sv << fDatabase;
    sb << "}"sv;
    return sb;
}

String About::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Overall-Application-Version: "sv << fOverallApplicationVersion << ", "sv;
    sb << "API-Server-Info: "sv << fAPIServerInfo;
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper About::kMapper = [] () {
    ObjectVariantMapper mapper;

    mapper += APIServerInfo::OperatingSystem::kMapper;

    mapper.AddCommonType<optional<double>> ();

    mapper.Add<Common::Version> ([] ([[maybe_unused]] const ObjectVariantMapper& mapper,
                                     const Common::Version* obj) -> VariantValue { return obj->AsPrettyVersionString (); },
                                 [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, Common::Version* intoObj) -> void {
                                     *intoObj = Common::Version::FromPrettyVersionString (d.As<String> ());
                                 });

    mapper.AddClass<About::APIServerInfo::ComponentInfo> ({
        {"name"sv, &About::APIServerInfo::ComponentInfo::fName},
        {"version"sv, &About::APIServerInfo::ComponentInfo::fVersion},
        {"URL"sv, &About::APIServerInfo::ComponentInfo::fURL},
    });
    mapper.AddCommonType<Sequence<About::APIServerInfo::ComponentInfo>> ();

    mapper.AddClass<About::APIServerInfo::CurrentMachine> ({
        {"operatingSystem"sv, &About::APIServerInfo::CurrentMachine::fOperatingSystem},
        {"machineUptime"sv, &About::APIServerInfo::CurrentMachine::fMachineUptime},
        {"totalCPUUsage"sv, &About::APIServerInfo::CurrentMachine::fTotalCPUUsage},
        {"runQLength"sv, &About::APIServerInfo::CurrentMachine::fRunQLength},
    });

    mapper.AddClass<About::APIServerInfo::CurrentProcess> ({
        {"processUptime"sv, &About::APIServerInfo::CurrentProcess::fProcessUptime},
        {"averageCPUTimeUsed"sv, &About::APIServerInfo::CurrentProcess::fAverageCPUTimeUsed},
        {"workingOrResidentSetSize"sv, &About::APIServerInfo::CurrentProcess::fWorkingOrResidentSetSize},
        {"combinedIOReadRate"sv, &About::APIServerInfo::CurrentProcess::fCombinedIOReadRate},
        {"combinedIOWriteRate"sv, &About::APIServerInfo::CurrentProcess::fCombinedIOWriteRate},
    });

    mapper.AddCommonType<optional<float>> ();
    mapper.AddClass<About::APIServerInfo::APIEndpoint> ({
        {"callsCompleted"sv, &About::APIServerInfo::APIEndpoint::fCallsCompleted},
        {"errors"sv, &About::APIServerInfo::APIEndpoint::fErrors},
        {"medianDuration"sv, &About::APIServerInfo::APIEndpoint::fMedianDuration},
        {"meanDuration"sv, &About::APIServerInfo::APIEndpoint::fMeanDuration},
        {"maxDuration"sv, &About::APIServerInfo::APIEndpoint::fMaxDuration},
        {"medianWebServerConnections"sv, &About::APIServerInfo::APIEndpoint::fMedianWebServerConnections},
        {"medianProcessingWebServerConnections"sv, &About::APIServerInfo::APIEndpoint::fMedianProcessingWebServerConnections},
        {"medianRunningAPITasks"sv, &About::APIServerInfo::APIEndpoint::fMedianRunningAPITasks},
    });
    mapper.AddCommonType<optional<About::APIServerInfo::APIEndpoint>> ();

    mapper.AddClass<About::APIServerInfo::WebServer::ThreadPool> ({
        {"threads"sv, &About::APIServerInfo::WebServer::ThreadPool::fThreads},
        {"tasksStillQueued"sv, &About::APIServerInfo::WebServer::ThreadPool::fTasksStillQueued},
        {"averageTaskRunTime"sv, &About::APIServerInfo::WebServer::ThreadPool::fAverageTaskRunTime},
    });

    mapper.AddClass<About::APIServerInfo::WebServer> ({
        {"threadPool"sv, &About::APIServerInfo::WebServer::fThreadPool},
    });
    mapper.AddCommonType<optional<About::APIServerInfo::WebServer>> ();

    mapper.AddClass<About::APIServerInfo::Database> ({
        {"reads"sv, &About::APIServerInfo::Database::fReads},
        {"writes"sv, &About::APIServerInfo::Database::fWrites},
        {"errors"sv, &About::APIServerInfo::Database::fErrors},
        {"meanReadDuration"sv, &About::APIServerInfo::Database::fMeanReadDuration},
        {"medianReadDuration"sv, &About::APIServerInfo::Database::fMedianReadDuration},
        {"meanWriteDuration"sv, &About::APIServerInfo::Database::fMeanWriteDuration},
        {"medianWriteDuration"sv, &About::APIServerInfo::Database::fMedianWriteDuration},
        {"maxDuration"sv, &About::APIServerInfo::Database::fMaxDuration},
        {"fileSize"sv, &About::APIServerInfo::Database::fFileSize},
    });
    mapper.AddCommonType<optional<About::APIServerInfo::Database>> ();

    mapper.AddClass<About::APIServerInfo> ({
        {"version"sv, &About::APIServerInfo::fVersion},
        {"componentVersions"sv, &About::APIServerInfo::fComponentVersions},
        {"currentMachine"sv, &About::APIServerInfo::fCurrentMachine},
        {"currentProcess"sv, &About::APIServerInfo::fCurrentProcess},
        {"apiEndpoint"sv, &About::APIServerInfo::fAPIEndpoint},
        {"webServer"sv, &About::APIServerInfo::fWebServer},
        {"database"sv, &About::APIServerInfo::fDatabase},
    });

    mapper.AddClass<About> ({
        {"applicationVersion"sv, &About::fOverallApplicationVersion},
        {"serverInfo"sv, &About::fAPIServerInfo},
    });

    return mapper;
}();

const ObjectVariantMapper Stroika::Samples::HTMLUI::Model::kMapper = [] () {
    ObjectVariantMapper mapper;

    mapper.AddCommonType<Collection<String>> ();

    mapper.AddCommonType<Common::GUID> ();
    mapper.AddCommonType<Collection<Common::GUID>> ();

    mapper += About::kMapper;

    // Read/Write real/complex numbers
    mapper.Add<Number> (
        [] (const ObjectVariantMapper& mapper [[maybe_unused]], const Number* obj) -> VariantValue {
            static const FloatConversion::ToStringOptions kFloat2StringOptions_{};
            StringBuilder                                 sb;
            if (obj->real () != 0) {
                if (obj->imag () == 0) {
                    return obj->real (); // return a number in this case, not a string
                }
                sb << FloatConversion::ToString (obj->real (), kFloat2StringOptions_);
            }
            if (obj->imag () != 0) {
                if (not sb.empty ()) {
                    sb << ' ';
                }
                if (obj->imag () > 0 and sb.length () > 1) {
                    sb << "+ "sv;
                }
                if (obj->imag () == 1) {
                    // skip
                }
                else if (obj->imag () == -1) {
                    sb << "- "sv;
                }
                else {
                    sb << FloatConversion::ToString (obj->imag (), kFloat2StringOptions_);
                }
                sb << "i"sv;
            }
            if (sb.empty ()) {
                return "0"_k;
            }
            return sb.str ();
        },
        [] (const ObjectVariantMapper& mapper [[maybe_unused]], const VariantValue& vv, Number* intoObj) -> void {
            // Parse complex numbers of the form a + bi, handling special cases of a, and bi.
            // Trick: parse one number, and then accumulate second number (if any)
            // Works with a few minor rewrites
            String remainingNumber2Parse = vv.As<String> ();
            remainingNumber2Parse = remainingNumber2Parse.ReplaceAll (" "_k, String{}); // strip spaces - not needed and cause 2 + 4i to not parse (where 2+4i would) with simple trick we use (parse numbers and accum)
            Number accum{};
            for (unsigned int cnt = 0; cnt < 2 and not remainingNumber2Parse.empty (); ++cnt) {
                // special case rewrite bare 'i' as '1i' with +/- cases
                if (remainingNumber2Parse.StartsWith ("i"_k)) {
                    static_assert (is_base_of_v<String, decltype (remainingNumber2Parse)>);
                    remainingNumber2Parse = "1i"_k + remainingNumber2Parse.Skip (1);
                }
                else if (remainingNumber2Parse.StartsWith ("+i"_k)) {
                    remainingNumber2Parse = "+1i"_k + remainingNumber2Parse.Skip (2);
                }
                else if (remainingNumber2Parse.StartsWith ("-i"_k)) {
                    remainingNumber2Parse = "-1i"_k + remainingNumber2Parse.Skip (2);
                }
                Number::value_type d = Characters::FloatConversion::ToFloat<Number::value_type> (remainingNumber2Parse, &remainingNumber2Parse);
                if (remainingNumber2Parse.StartsWith ("i"_k)) {
                    accum += Number{0, d};
                    remainingNumber2Parse = remainingNumber2Parse.Skip (1);
                }
                else {
                    accum += Number{d};
                }
            }
            if (not remainingNumber2Parse.empty ()) {
                Execution::Throw (Execution::Exception{"invalid complex number: "sv + vv.As<String> ()});
            }
            *intoObj = accum;
        });

    mapper.AddCommonType<Collection<String>> ();

    return mapper;
}();

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Set.h"

#include "Stroika/Frameworks/Auth/OAuth/Configuration.h"

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
 ******************************** Model::HealthStatus ***************************
 ********************************************************************************
 */
String HealthStatus::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "ok: "sv << fOK;
    if (fWarnings) {
        sb << ", warnings: "sv << fWarnings;
    }
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper HealthStatus::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper.AddCommonType<Sequence<String>> ();
    mapper.AddCommonType<optional<Sequence<String>>> ();
    mapper.AddClass<HealthStatus> ({
        {"ok"sv, &HealthStatus::fOK},
        {"warnings"sv, &HealthStatus::fWarnings},
    });
    return mapper;
}();

/*
 ********************************************************************************
 ****************************** Auth::TokenRequest ******************************
 ********************************************************************************
 */
String Auth::TokenRequest::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "OAuthProvider: "sv << fOAuthProvider;
    sb << ", applicationID: "sv << fApplicationID;
    sb << ", redirectURL: "sv << fRedirectURL;
    if (fAuthorizationCode) {
        sb << ", authorizationCode: "sv << fAuthorizationCode;
    }
    if (fRefreshToken) {
        sb << ", refreshToken: "sv << fRefreshToken;
    }
    if (fCodeVerifier) {
        sb << ", codeVerifier: "sv << fCodeVerifier;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ****************************** Auth::TokenResponse *****************************
 ********************************************************************************
 */
String Auth::TokenResponse::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "access_token: "sv << access_token;
    sb << ", expires_at: "sv << expires_at;
    sb << ", scopes: "sv << scopes;
    if (refresh_token) {
        sb << ", refresh_token: "sv << refresh_token;
    }
    if (id_token) {
        sb << ", id_token: "sv << id_token;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************ Auth::TokenRevocationRequest **************************
 ********************************************************************************
 */
String Auth::TokenRevocationRequest::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "fOAuthProvider: "sv << fOAuthProvider;
    sb << "access_token: "sv << fAccessToken;
    if (fRefreshToken) {
        sb << ", fRefreshToken: "sv << fRefreshToken;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ********************************* Auth::UserInfo *******************************
 ********************************************************************************
 */
String Auth::UserInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Name: "sv << fName;
    sb << ", email: "sv << fEmail;
    sb << ", personImage: "sv << fPersonImage;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ********************************** Auth::kMapper *******************************
 ********************************************************************************
 */
const ObjectVariantMapper Auth::kMapper = [] () {
    ObjectVariantMapper mapper;

    mapper += Stroika::Frameworks::Auth::OAuth::ClientConfiguration::kMapper;
    mapper.AddCommonType<Stroika::Frameworks::Auth::OAuth::ClientConfigurations> ();
    mapper += Stroika::Frameworks::Auth::OAuth::ProviderConfiguration::kMapper;
    mapper.AddCommonType<Stroika::Frameworks::Auth::OAuth::ProvidersConfigurations> ();
    mapper.AddClass<Auth::Configuration> ({
        {"clients"sv, &Auth::Configuration::fClients},
        {"providers"sv, &Auth::Configuration::fProviders},
    });

    mapper.AddCommonType<String> ();
    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<URI> ();
    mapper.AddClass<Auth::TokenRequest> ({
        {"applicationId"sv, &Auth::TokenRequest::fApplicationID},
        {"provider"sv, &Auth::TokenRequest::fOAuthProvider},
        {"redirectURL"sv, &Auth::TokenRequest::fRedirectURL},
        {"authorizationCode"sv, &Auth::TokenRequest::fAuthorizationCode},
        {"refreshToken"sv, &Auth::TokenRequest::fRefreshToken},
        {"codeVerifier"sv, &Auth::TokenRequest::fCodeVerifier},
    });

    mapper.AddCommonType<Set<String>> ();
    mapper.AddClass<Auth::TokenResponse> ({
        {"access_token"sv, &Auth::TokenResponse::access_token},
        {"expires_at"sv, &Auth::TokenResponse::expires_at},
        {"scopes"sv, &Auth::TokenResponse::scopes},
        {"refresh_token"sv, &Auth::TokenResponse::refresh_token},
        {"id_token"sv, &Auth::TokenResponse::id_token},
    });

    mapper.AddClass<Auth::TokenRevocationRequest> ({
        {"provider"sv, &Auth::TokenRevocationRequest::fOAuthProvider},
        {"access_token"sv, &Auth::TokenRevocationRequest::fAccessToken},
        {"refresh_token"sv, &Auth::TokenRevocationRequest::fRefreshToken},
    });

    mapper.AddCommonType<optional<String>> ();
    mapper.AddCommonType<optional<URI>> ();
    mapper.AddClass<Auth::UserInfo> ({
        {"personName"sv, &Auth::UserInfo::fName},
        {"email"sv, &Auth::UserInfo::fEmail},
        {"personImageURL"sv, &Auth::UserInfo::fPersonImage},
    });

    return mapper;
}();

/*
 ********************************************************************************
 **************** Model::About::APIServerInfo::OperatingSystem ******************
 ********************************************************************************
 */
String About::APIServerInfo::OperatingSystem::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "majorOSCategory: "sv << fMajorOSCategory;
    sb << ", fullVersionedOSName: "sv << fFullVersionedOSName;
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

/*
 ********************************************************************************
 ************** Model::About::APIServerInfo::ComponentInfo **********************
 ********************************************************************************
 */
String About::APIServerInfo::ComponentInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "name: "sv << fName;
    sb << ", version: "sv << fVersion;
    sb << ", URL: "sv << fURL;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************** Model::About::APIServerInfo::CurrentMachine *********************
 ********************************************************************************
 */
String About::APIServerInfo::CurrentMachine::ToString () const
{
    StringBuilder sb;
    sb << "{";
    sb << "Operating-System: " << fOperatingSystem;
    sb << ", machine-Uptime: " << fMachineUptime;
    sb << ", total-CPU-Usage: " << fTotalCPUUsage;
    sb << ", run-Q-Length: " << fRunQLength;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************** Model::About::APIServerInfo::CurrentProcess *********************
 ********************************************************************************
 */
String About::APIServerInfo::CurrentProcess::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "processUptime: "sv << fProcessUptime;
    sb << ", averageCPUTimeUsed: "sv << fAverageCPUTimeUsed;
    sb << ", workingOrResidentSetSize: "sv << fWorkingOrResidentSetSize;
    sb << ", combinedIOReadRate: "sv << fCombinedIOReadRate;
    sb << ", combinedIOWriteRate: "sv << fCombinedIOWriteRate;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ***************** Model::About::APIServerInfo::APIEndpoint *********************
 ********************************************************************************
 */
String About::APIServerInfo::APIEndpoint::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "callsCompleted: "sv << fCallsCompleted;
    sb << ", errors: "sv << fErrors;
    sb << ", callTimes: "sv << fCallTimes;
    sb << ", medianWebServerConnections: "sv << fMedianWebServerConnections;
    sb << ", medianProcessingWebServerConnections: "sv << fMedianProcessingWebServerConnections;
    sb << ", medianRunningAPITasks: "sv << fMedianRunningAPITasks;
    sb << "}"sv;
    return sb.str ();
}

/*
 ********************************************************************************
 ******************* Model::About::APIServerInfo::WebServer *********************
 ********************************************************************************
 */
String About::APIServerInfo::WebServer::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "threadPool: {"sv;
    sb << ", threads: " << fThreadPool.fThreads;
    sb << ", tasksStillQueued: " << fThreadPool.fTasksStillQueued;
    sb << ", averageTaskRunTime: " << fThreadPool.fAverageTaskRunTime;
    sb << "}}"sv;
    return sb.str ();
}

/*
 ********************************************************************************
 ******************** Model::About::APIServerInfo::Database *********************
 ********************************************************************************
 */
String About::APIServerInfo::Database::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "reads: "sv << fReads;
    sb << ", writes: "sv << fWrites;
    sb << ", errors: "sv << fErrors;
    sb << ", meanReadDuration: "sv << fMeanReadDuration;
    sb << ", medianReadDuration: "sv << fMedianReadDuration;
    sb << ", meanWriteDuration: "sv << fMeanWriteDuration;
    sb << ", medianWriteDuration: "sv << fMedianWriteDuration;
    sb << ", maxDuration: "sv << fMaxDuration;
    sb << ", fileSize: "sv << fFileSize;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************** Model::About::APIServerInfo *************************
 ********************************************************************************
 */
String About::APIServerInfo::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "version: "sv << fVersion;
    sb << ", component-versions: "sv << fComponentVersions;
    sb << ", current-machine: "sv << fCurrentMachine;
    sb << ", current-process: "sv << fCurrentProcess;
    sb << ", API-Endpoint: "sv << fAPIEndpoint;
    sb << ", webServer: "sv << fWebServer;
    sb << ", database: "sv << fDatabase;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 *********************************** Model::About *******************************
 ********************************************************************************
 */
String About::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Overall-Application-Version: "sv << fOverallApplicationVersion;
    sb << ", API-Server-Info: "sv << fAPIServerInfo;
    if (fHealthStatus) {
        sb << ", healthStatus: "sv << fHealthStatus;
    }
    sb << "}"sv;
    return sb;
}

const ObjectVariantMapper About::kMapper = [] () {
    ObjectVariantMapper mapper;

    mapper += HealthStatus::kMapper;
    mapper.AddCommonType<optional<HealthStatus>> ();

    mapper.AddCommonType<Math::CommonStatistics<Duration>> ();

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
        {"callTimes"sv, &About::APIServerInfo::APIEndpoint::fCallTimes},
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

    mapper.AddCommonType<Math::CommonStatistics<Duration>> ();

    mapper.AddClass<About::APIServerInfo::WebServer::ConnectionStatistics> ({
        {"open"sv, &About::APIServerInfo::WebServer::ConnectionStatistics::fNumberOfOpenConnections},
        {"active"sv, &About::APIServerInfo::WebServer::ConnectionStatistics::fNumberOfActiveConnections},
        {"openConnectionsLifetime"sv, &About::APIServerInfo::WebServer::ConnectionStatistics::fDurationOfOpenConnections},
        {"openConnectionsRequests"sv, &About::APIServerInfo::WebServer::ConnectionStatistics::fDurationOfOpenConnectionsRequests},
        {"activeConnectionsRequests"sv, &About::APIServerInfo::WebServer::ConnectionStatistics::fDurationOfActiveConnectionsRequests},
        {"piningForTheFjords"sv, &About::APIServerInfo::WebServer::ConnectionStatistics::fConnectionsPiningForTheFjords},
    });

    mapper.AddClass<About::APIServerInfo::WebServer> ({
        {"threadPool"sv, &About::APIServerInfo::WebServer::fThreadPool},
        {"connections"sv, &About::APIServerInfo::WebServer::fConnections},
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
        {"healthstatus"sv, &About::fHealthStatus},
    });

    return mapper;
}();

/*
 ********************************************************************************
 ************************************ Model::kMapper ****************************
 ********************************************************************************
 */
const ObjectVariantMapper Stroika::Samples::HTMLUI::Model::kMapper = [] () {
    ObjectVariantMapper mapper;

    mapper += Auth::kMapper;

    mapper.AddCommonType<Collection<String>> ();

    mapper.AddCommonType<Common::GUID> ();
    mapper.AddCommonType<Collection<Common::GUID>> ();

    mapper += About::kMapper;
    mapper += HealthStatus::kMapper;

    mapper.AddCommonType<Collection<String>> ();

    return mapper;
}();

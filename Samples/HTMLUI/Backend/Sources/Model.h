/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_Model_h_
#define _StroikaSample_WebServices_Model_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <complex>

#include "Stroika/Foundation/Common/Version.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/Auth/OAuth/Client.h"

/**
 * \brief: The Model module defines all the objects, which can appear in web service requests (on the request or response side).
 *
 *  These objects appear (are used) in the IWSAPI.
 *
 *  This module also provides ObjectVariantMapper objects which translate between DataExchange::VariantValue objects and the C++ objects defined here
 *  (which is what allows these objects to be marshalled through HTTP).
 */
namespace Stroika::Samples::HTMLUI::Model {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Containers::Sequence;
    using Containers::Set;
    using DataExchange::InternetMediaType;
    using DataExchange::ObjectVariantMapper;
    using DataExchange::TypedBLOB;
    using IO::Network::URI;
    using Math::CommonStatistics;
    using Time::DateTime;
    using Time::Duration;

    /**
     * @brief  
     * 
     */
    struct HealthStatus {
        bool fOK{false};

        optional<Sequence<String>> fWarnings;

        nonvirtual String ToString () const;

        static const DataExchange::ObjectVariantMapper kMapper;
    };

    namespace Auth {

        /**
         * Configuration returned by webservices - combo of client and provider configuration data
         * ('joined by provider field')
         */
        struct Configuration {
            Stroika::Frameworks::Auth::OAuth::ClientConfigurations    fClients;
            Stroika::Frameworks::Auth::OAuth::ProvidersConfigurations fProviders;
        };

        /**
         * @brief  OAuth2 works with gui client requesting a short-lived authorization_code from the web browser (typically)
         *   and then the GUI client exchanges that for a longer-lived access_token (and sometimes a refresh_token, and/or id_token).
         */
        struct TokenRequest {
            /**
             * @brief e.g. google - index into ??
             */
            String fOAuthProvider;
            String fApplicationID;
            URI    fRedirectURL;

            // magic code typically extracted from redirect URL parameters (not fRedirectURL above but what is ACTAULLY sent to that URL)
            // note this somehow 'contains' - perhaps via pointer - the requested scopes, id_tokens, etc....
            // note also - be careful to URL-decode the value when extracting from a URL, before placing here
            optional<String> fAuthorizationCode;
            optional<String> fRefreshToken;

            optional<String> fCodeVerifier;

            nonvirtual String ToString () const;

            template <Common::IAnyOf<Stroika::Frameworks::Auth::OAuth::TokenRequest> T>
            nonvirtual T As () const;
        };
        template <>
        Stroika::Frameworks::Auth::OAuth::TokenRequest Auth::TokenRequest::As () const;

        /**
         */
        struct TokenResponse {

            TokenResponse () = default;
            TokenResponse (const Stroika::Frameworks::Auth::OAuth::TokenResponse& tr);

            /**
             */
            String access_token;

            /** OAuth uses expires_in, but we convert to an expires_at since better to track (in UTC) */
            DateTime expires_at{DateTime::NowUTC ()};

            Set<String>      scopes;
            optional<String> refresh_token;
            optional<String> id_token;

            nonvirtual String ToString () const;
        };

        /**
         */
        struct TokenRevocationRequest {
            /**
             * @brief e.g. google - index into ??
             */
            String           fOAuthProvider;
            String           fAccessToken;
            optional<String> fRefreshToken;

            nonvirtual String ToString () const;

            template <Common::IAnyOf<Stroika::Frameworks::Auth::OAuth::TokenRevocationRequest> T>
            nonvirtual T As () const;
        };
        template <>
        Stroika::Frameworks::Auth::OAuth::TokenRequest Auth::TokenRequest::As () const;

        /**
         * @brief  Information from OAuth2 server about the user.
         */
        struct UserInfo {

            /**
             */
            UserInfo () = default;
            UserInfo (const Stroika::Frameworks::Auth::OAuth::UserInfo& ui);

            optional<String> fName;
            optional<String> fEmail;
            optional<URI>    fPersonImage;

            nonvirtual String ToString () const;
        };

        extern const ObjectVariantMapper kMapper;

    }

    /**
     * @brief  
     * 
     */
    struct About {
        Common::Version fOverallApplicationVersion;

        struct APIServerInfo {

            Common::Version fVersion;
            struct ComponentInfo {
                String        fName;
                String        fVersion;
                optional<URI> fURL;

                nonvirtual String ToString () const;
            };
            Sequence<ComponentInfo> fComponentVersions;

            /**
             */
            struct OperatingSystem {
                /**
                 *  Open enum, but basic names are:
                 *      o   Linux
                 *      o   Windows
                 *      o   MacOS
                 *      o   BSD
                 *      o   Unix            (for cases not BSD or Linux)
                 */
                optional<String> fMajorOSCategory;

                /**
                 *  For Linux, try to show distribution.
                 *  e.g. "Ubuntu 18.04", "Red-Hat 9", "Windows 10 Version 1809 (OS Build 17763.349)", "Linux (unknown)"
                 */
                optional<String> fFullVersionedOSName;

#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
                DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
                auto operator<=> (const OperatingSystem&) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
                DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif

                /**
                 *  @see Characters::ToString ();
                 */
                nonvirtual String ToString () const;

                static const DataExchange::ObjectVariantMapper kMapper;
            };

            struct CurrentMachine {
                OperatingSystem    fOperatingSystem;
                optional<Duration> fMachineUptime;
                optional<double>   fTotalCPUUsage{};
                optional<double>   fRunQLength{};

                nonvirtual String ToString () const;
            };
            CurrentMachine fCurrentMachine;

            struct CurrentProcess {
                optional<Duration> fProcessUptime;
                optional<double>   fAverageCPUTimeUsed;
                optional<uint64_t> fWorkingOrResidentSetSize;
                optional<double>   fCombinedIOReadRate;
                optional<double>   fCombinedIOWriteRate;

                nonvirtual String ToString () const;
            };
            CurrentProcess fCurrentProcess;

            /**
             * WSAPI related stats - for now - averaged over the last 5 minutes.
             * 
             *      Note - some of these stats are a bit redundant, given the WebServer
             *      ConnectionStatistics; could lose them, but maybe useful anyhow.
             */
            struct APIEndpoint {
                unsigned int               fCallsCompleted{};
                CommonStatistics<Duration> fCallTimes;
                unsigned int               fErrors{};
                optional<float>            fMedianWebServerConnections;
                optional<float>            fMedianProcessingWebServerConnections;
                optional<float>            fMedianRunningAPITasks;

                nonvirtual String ToString () const;
            };
            optional<APIEndpoint> fAPIEndpoint;

            /**
              * Statistics from the WebServer (@todo TBD timing interval for interpretation)
              *     also should add much more in the way of stats! Like backlog, endpoints, etc... Number of alive connections; Number of new connections in last N seconds;
              */
            struct WebServer {
                struct ThreadPool {
                    unsigned int       fThreads{};
                    unsigned int       fTasksStillQueued{};
                    optional<Duration> fAverageTaskRunTime;
                };
                ThreadPool fThreadPool;

                struct ConnectionStatistics {
                    size_t                     fNumberOfOpenConnections{};
                    size_t                     fNumberOfActiveConnections{};
                    CommonStatistics<Duration> fDurationOfOpenConnections;
                    CommonStatistics<Duration> fDurationOfOpenConnectionsRequests;
                    CommonStatistics<Duration> fDurationOfActiveConnectionsRequests;
                    size_t                     fConnectionsPiningForTheFjords{};
                };
                ConnectionStatistics fConnections;

                nonvirtual String ToString () const;
            };
            optional<WebServer> fWebServer;

            /**
             * Database related stats - for now - averaged over the last 5 minutes.
             */
            struct Database {
                unsigned int        fReads{};
                unsigned int        fWrites{};
                unsigned int        fErrors{};
                optional<Duration>  fMeanReadDuration;
                optional<Duration>  fMedianReadDuration;
                optional<Duration>  fMeanWriteDuration;
                optional<Duration>  fMedianWriteDuration;
                optional<Duration>  fMaxDuration;
                optional<uintmax_t> fFileSize;

                nonvirtual String ToString () const;
            };
            optional<Database> fDatabase;

            nonvirtual String ToString () const;
        };
        APIServerInfo          fAPIServerInfo;
        optional<HealthStatus> fHealthStatus;

        nonvirtual String ToString () const;

        static const DataExchange::ObjectVariantMapper kMapper;
    };

    /**
     * ObjectVariantMapper which maps all the model data objects to/from VariantValue objects.
     */
    extern const DataExchange::ObjectVariantMapper kMapper;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Model.inl"

#endif /*_StroikaSample_WebServices_Model_h_*/

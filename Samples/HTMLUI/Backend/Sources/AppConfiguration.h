/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _RFL_AskHealthFrame_AppConfiguration_h_
#define _RFL_AskHealthFrame_AppConfiguration_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/IO/Network/Port.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Time/Duration.h"

/**
 */

namespace Stroika::Samples::HTMLUI {

    using namespace std;
    using namespace Stroika::Foundation;

    using Characters::String;
    using IO::Network::URI;

    /**
     *  This is the type of object stored in application configuration.
     */
    struct AppConfigurationType {

        // KISS for now, but will need to replace with more general bindings object, spec addr/port/cert/other options
        optional<IO::Network::PortType>        WebServerPort;
        static constexpr IO::Network::PortType kWebServerPort_Default = 9080;

        // apparent URI often 'localhost:port#' - but for things like webgui or openapi url listings, better
        // to list external URL
        optional<URI> ShowAsExternalURL;

        struct Logging {
            optional<bool>        ToStdOut;
            static constexpr bool kToStdOut_Default = false;
#if qPlatform_POSIX
            optional<bool>        ToSysLog;
            static constexpr bool kToSysLog_Default = true;
#endif
#if qPlatform_Windows
            optional<bool>        ToWindowsEventLog;
            static constexpr bool kToWindowsEventLog_Default = true;
#endif
        };
        optional<Logging> fLogging;

        static const DataExchange::ObjectVariantMapper kMapper;
    };

    namespace Private_ {
        /*
         *  This is a bit of magic used to encode knowledge of how/where to persist the configuration data.
         */
        struct AppConfiguration_Storage_IMPL_ {
            AppConfiguration_Storage_IMPL_ ();
            AppConfigurationType      Get () const;
            void                      Set (const AppConfigurationType& v);
            DataExchange::OptionsFile fOptionsFile_;
            AppConfigurationType      fActualCurrentConfigData_;
        };
    }

    /**
     *  gAppConfiguration is automatically internally synchronized ... - just call update / set / get to access options freely.
     */
    inline Execution::ModuleGetterSetter<AppConfigurationType, Private_::AppConfiguration_Storage_IMPL_> gAppConfiguration;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "AppConfiguration.inl"

#endif /*_RFL_AskHealthFrame_AppConfiguration_h_*/

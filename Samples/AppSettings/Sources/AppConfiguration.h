/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _StroikaSample_AppSettings_AppConfiguration_h_
#define _StroikaSample_AppSettings_AppConfiguration_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/IO/Network/Port.h"
#include "Stroika/Foundation/Time/Duration.h"

/**
 *  A simple example of app configuration management using the Stroika toolkit
 */

namespace StroikaSample::AppSettings {

    using namespace std;
    using namespace Stroika::Foundation;

    /**
     *  This is the type of object stored in configuration.
     */
    struct AppConfigurationType {
        optional<IO::Network::PortType>        WebServerPort;
        static constexpr IO::Network::PortType kWebServerPort_Default = 80;

        /*
         *  See the documentation on ObjectVariantMapper for more info on how to define mappers. But this maps between C++ structs, and VariantValue objects which
         *  can be serialized/deserialized to/from JSON configuration files (for example).
         */
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

#endif /*_StroikaSample_AppSettings_AppConfiguration_h_*/

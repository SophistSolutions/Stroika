/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
//#include "Stroika/Foundation/Common/Property.h"
//#include "Stroika/Foundation/Debug/Trace.h"
//#include "Stroika/Foundation/Execution/Synchronized.h"
//#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"

#include "AppConfiguration.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

using namespace StroikaSample;
using namespace StroikaSample::AppSettings;

using StroikaSample::AppSettings::Private_::AppConfiguration_Storage_IMPL_;

/*
 ********************************************************************************
 *************************** AppConfigurationType *******************************
 ********************************************************************************
 */
const ObjectVariantMapper AppConfigurationType::kMapper = [] () {
    ObjectVariantMapper mapper;
    mapper.AddCommonType<optional<IO::Network::PortType>> ();
    mapper.AddClass<AppConfigurationType> ({
        {"WebServerPort"sv, StructFieldMetaInfo{&AppConfigurationType::WebServerPort}},
    });
    return mapper;
}();

/*
 ********************************************************************************
 ******************** Private_::AppConfiguration_Storage_IMPL_ ******************
 ********************************************************************************
 */
AppConfiguration_Storage_IMPL_::AppConfiguration_Storage_IMPL_ ()
    : fOptionsFile_{
          /*
            * Any module name will do. This will map (by default) to a AppSettings.json file in XXX.
            * If you require a single configuration file 'Main" might be a better module name.
            * But if you have multiple modules with configuration data, pick a name that matches that module,
            * and they will all be stored under a folder for all your apps configuration.
            */
          "AppSettings"sv, 
          
          /*
           */
          AppConfigurationType::kMapper, 
          
            /*
             * Hooks for versioning, to manage as your application evolves and the configuration data changes
             */
            OptionsFile::kDefaultUpgrader, 

          /*
           */
OptionsFile::mkFilenameMapper (L"Put-Your-App-Name-Here"sv)}
    , fActualCurrentConfigData_{fOptionsFile_.Read<AppConfigurationType> (AppConfigurationType{})}
{
    Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
}

AppConfigurationType AppConfiguration_Storage_IMPL_::Get () const
{
    return fActualCurrentConfigData_;
}

void AppConfiguration_Storage_IMPL_::Set (const AppConfigurationType& v)
{
    fActualCurrentConfigData_ = v;
    fOptionsFile_.Write (v);
}

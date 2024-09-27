/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"

#include "AppConfiguration.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Samples::HTMLUI;

using Stroika::Samples::HTMLUI::Private_::AppConfiguration_Storage_IMPL_;

/*
 ********************************************************************************
 *************************** AppConfigurationType *******************************
 ********************************************************************************
 */
const ObjectVariantMapper AppConfigurationType::kMapper = [] () {
    // note - set fOmitNullEntriesInFromObject=false so easier review config file and see where values are defaulted/defaulting
    ObjectVariantMapper mapper;
    mapper.AddCommonType<optional<IO::Network::PortType>> ();
    mapper.AddClass<AppConfigurationType::Logging> (
        {
            {"ToStdOut"sv, &AppConfigurationType::Logging::ToStdOut},
#if qPlatform_POSIX
                {"ToSysLog"sv, &AppConfigurationType::Logging::ToSysLog},
#endif
#if qPlatform_Windows
                {"ToWindowsEventLog"sv, &AppConfigurationType::Logging::ToWindowsEventLog},
#endif
        },
        {.fOmitNullEntriesInFromObject = false});
    mapper.AddCommonType<optional<AppConfigurationType::Logging>> ();
    mapper.AddCommonType<optional<IO::Network::URI>> ();
    mapper.AddClass<AppConfigurationType> (
        {
            {"WebServerPort"sv, &AppConfigurationType::WebServerPort},
            {"Logging"sv, &AppConfigurationType::fLogging},
            {"ShowAsExternalURL"sv, &AppConfigurationType::ShowAsExternalURL},
        },
        {.fOmitNullEntriesInFromObject = false});
    return mapper;
}();

/*
 ********************************************************************************
 ******************** Private_::AppConfiguration_Storage_IMPL_ ******************
 ********************************************************************************
 */
AppConfiguration_Storage_IMPL_::AppConfiguration_Storage_IMPL_ ()
    : fOptionsFile_{"Server"sv, AppConfigurationType::kMapper, OptionsFile::kDefaultUpgrader, OptionsFile::mkFilenameMapper ("Ask-HealthFrame"sv)}
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

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SystemConfiguration_inl_
#define _Stroika_Foundation_Configuration_SystemConfiguration_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation {
    namespace Configuration {

        /*
            ********************************************************************************
            **************** SystemConfiguration::CPU::CoreDetails *************************
            ********************************************************************************
            */
        inline SystemConfiguration::CPU::CoreDetails::CoreDetails (unsigned int socketID, const String& modelName)
            : fSocketID (socketID)
            , fModelName (modelName)
        {
        }

        /*
            ********************************************************************************
            **************************** SystemConfiguration::CPU **************************
            ********************************************************************************
            */
        inline unsigned int SystemConfiguration::CPU::GetNumberOfLogicalCores () const
        {
            return static_cast<unsigned int> (fCores.size ());
        }
        inline String SystemConfiguration::CPU::GetCPUModelPrintName () const
        {
            return fCores.empty () ? String () : fCores[0].fModelName;
        }
    }
}
namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology> : EnumNames<Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology> {
        static constexpr EnumNames<Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology> k{
            EnumNames<Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology>::BasicArrayInitializer{
                {
                    {Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology::eRPM, L"RPM"},
                    {Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology::eMSI, L"MSI"},
                    {Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology::eDPKG, L"DPKG"},
                }}};
        DefaultNames ()
            : EnumNames<Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology> (k)
        {
        }
    };
}
#endif /*_Stroika_Foundation_Configuration_SystemConfiguration_inl_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Configuration {

    /*
     ********************************************************************************
     **************** SystemConfiguration::CPU::CoreDetails *************************
     ********************************************************************************
     */
    inline SystemConfiguration::SystemConfiguration (const BootInformation& bi, const CPU& ci, const Memory& mi, const OperatingSystem& oi,
                                                     const ComputerNames& cn)
        : SystemConfiguration{bi, ci, mi, oi, oi, cn}
    {
    }
    inline SystemConfiguration::SystemConfiguration (const BootInformation& bi, const CPU& ci, const Memory& mi, const OperatingSystem& actualOS,
                                                     const OperatingSystem& apparentOS, const ComputerNames& cn)
        : fBootInformation{bi}
        , fCPU{ci}
        , fMemory{mi}
        , fActualOperatingSystem{actualOS}
        , fApparentOperatingSystem{apparentOS}
        , fComputerNames{cn}
    {
    }

    /*
     ********************************************************************************
     **************** SystemConfiguration::CPU::CoreDetails *************************
     ********************************************************************************
     */
    inline SystemConfiguration::CPU::CoreDetails::CoreDetails (unsigned int socketID, const String& modelName)
        : fSocketID{socketID}
        , fModelName{modelName}
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
        return fCores.empty () ? String{} : fCores[0].fModelName;
    }

}

namespace Stroika::Foundation::Configuration {

    template <>
    constexpr EnumNames<Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology>
        DefaultNames<Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology>::k{{{
            {Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology::eRPM, L"RPM"},
            {Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology::eMSI, L"MSI"},
            {Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology::eDPKG, L"DPKG"},
        }}};

}

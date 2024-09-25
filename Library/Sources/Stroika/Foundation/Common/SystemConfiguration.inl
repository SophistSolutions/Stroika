/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Common {

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

namespace Stroika::Foundation::Common {

    template <>
    constexpr EnumNames<Common::SystemConfiguration::OperatingSystem::InstallerTechnology>
        DefaultNames<Common::SystemConfiguration::OperatingSystem::InstallerTechnology>::k{{{
            {Common::SystemConfiguration::OperatingSystem::InstallerTechnology::eRPM, L"RPM"},
            {Common::SystemConfiguration::OperatingSystem::InstallerTechnology::eMSI, L"MSI"},
            {Common::SystemConfiguration::OperatingSystem::InstallerTechnology::eDPKG, L"DPKG"},
        }}};

}

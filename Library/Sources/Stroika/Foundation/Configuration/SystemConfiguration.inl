/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SystemConfiguration_inl_
#define _Stroika_Foundation_Configuration_SystemConfiguration_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {

    /*
     ********************************************************************************
     **************** SystemConfiguration::CPU::CoreDetails *************************
     ********************************************************************************
     */
    inline SystemConfiguration::SystemConfiguration (const BootInformation& bi, const CPU& ci, const Memory& mi, const OperatingSystem& oi, const ComputerNames& cn)
        : SystemConfiguration (bi, ci, mi, oi, oi, cn)
    {
    }
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    inline SystemConfiguration::SystemConfiguration (const BootInformation& bi, const CPU& ci, const Memory& mi, const OperatingSystem& actualOS, const OperatingSystem& apparentOS, const ComputerNames& cn)
        : fBootInformation{bi}
        , fCPU{ci}
        , fMemory{mi}
        , fActualOperatingSystem{actualOS}
        , fApparentOperatingSystem{apparentOS}
        , fComputerNames{cn}
    {
    }
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")

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

    [[deprecated ("use fActualOperatingSystem or fApparentOperatingSystem since Stroika v2.1d22")]] inline SystemConfiguration::OperatingSystem GetSystemConfiguration_OperatingSystem ()
    {
        return GetSystemConfiguration_ActualOperatingSystem ();
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
        constexpr DefaultNames ()
            : EnumNames<Configuration::SystemConfiguration::OperatingSystem::InstallerTechnology> (k)
        {
        }
    };

}

#endif /*_Stroika_Foundation_Configuration_SystemConfiguration_inl_*/

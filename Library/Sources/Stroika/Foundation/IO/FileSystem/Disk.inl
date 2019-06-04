/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Disk_inl_
#define _Stroika_Foundation_IO_FileSystem_Disk_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem {
}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Foundation::IO::FileSystem::BlockDeviceKind> DefaultNames<Foundation::IO::FileSystem::BlockDeviceKind>::k{
        EnumNames<Foundation::IO::FileSystem::BlockDeviceKind>::BasicArrayInitializer{
            {
                {Foundation::IO::FileSystem::BlockDeviceKind::eRemovableDisk, L"Removable-Disk"},
                {Foundation::IO::FileSystem::BlockDeviceKind::eLocalDisk, L"Local-Disk"},
                {Foundation::IO::FileSystem::BlockDeviceKind::eNetworkDrive, L"Network-Drive"},
                {Foundation::IO::FileSystem::BlockDeviceKind::eTemporaryFiles, L"Temporary-Files"},
                {Foundation::IO::FileSystem::BlockDeviceKind::eReadOnlyEjectable, L"Read-Only-Ejectable"},
                {Foundation::IO::FileSystem::BlockDeviceKind::eSystemInformation, L"System-Information"},
            }}};
}

#endif /*_Stroika_Foundation_IO_FileSystem_Disk_inl_*/

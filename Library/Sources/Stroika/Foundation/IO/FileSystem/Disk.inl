/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::IO::FileSystem {
}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Foundation::IO::FileSystem::BlockDeviceKind> DefaultNames<Foundation::IO::FileSystem::BlockDeviceKind>::k{{{
        {Foundation::IO::FileSystem::BlockDeviceKind::eRemovableDisk, L"Removable-Disk"},
        {Foundation::IO::FileSystem::BlockDeviceKind::eLocalDisk, L"Local-Disk"},
        {Foundation::IO::FileSystem::BlockDeviceKind::eNetworkDrive, L"Network-Drive"},
        {Foundation::IO::FileSystem::BlockDeviceKind::eTemporaryFiles, L"Temporary-Files"},
        {Foundation::IO::FileSystem::BlockDeviceKind::eReadOnlyEjectable, L"Read-Only-Ejectable"},
        {Foundation::IO::FileSystem::BlockDeviceKind::eSystemInformation, L"System-Information"},
    }}};
}

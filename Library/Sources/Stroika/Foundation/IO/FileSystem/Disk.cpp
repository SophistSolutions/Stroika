/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#include <winioctl.h>
#endif

#include "../../Characters/Format.h"
#include "../../Characters/String.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/String_Constant.h"
#include "../../Characters/ToString.h"
#include "../../Memory/SmallStackBuffer.h"

#include "Disk.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::FileSystem;

/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            constexpr EnumNames<Foundation::IO::FileSystem::BlockDeviceKind> DefaultNames<Foundation::IO::FileSystem::BlockDeviceKind>::k;
        }
    }
}

/*
 ********************************************************************************
 ************************** IO::FileSystem::DiskInfoType ************************
 ********************************************************************************
 */
String DiskInfoType::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    sb += L"Device-Name: '" + fDeviceName + L"', ";
    if (fPersistentVolumeID) {
        sb += L"Persist-Volume-ID: " + Characters::ToString (*fPersistentVolumeID) + L", ";
    }
    if (fDeviceKind) {
        sb += L"Device-Kind: '" + Characters::ToString (*fDeviceKind) + L"', ";
    }
    if (fSizeInBytes) {
        sb += L"Size-In-Bytes: " + Characters::ToString (*fSizeInBytes) + L", ";
    }
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 **************************** FileSystem::GetAvailableDisks *********************
 ********************************************************************************
 */
namespace {
    String GetPhysNameForDriveNumber_ (unsigned int i)
    {
        // This format is NOT super well documented, and was mostly derived from reading the remarks section
        // of https://msdn.microsoft.com/en-us/library/windows/desktop/aa363216%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
        // (DeviceIoControl function)
        return Characters::Format (L"\\\\.\\PhysicalDrive%d", i);
    }
}
Collection<DiskInfoType> FileSystem::GetAvailableDisks ()
{
    Collection<DiskInfoType> result{};

// @todo for linux
// https://github.com/karelzak/util-linux/blob/master/misc-utils/lsblk.c
// iterate_block_devices
#if qPlatform_Windows
    for (int i = 0; i < 100; i++) {
        HANDLE hHandle = ::CreateFileW (GetPhysNameForDriveNumber_ (i).c_str (), GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hHandle == INVALID_HANDLE_VALUE) {
            break;
        }
        GET_LENGTH_INFORMATION li{};
        DWORD                  dwBytesReturned{};
        BOOL                   bResult = ::DeviceIoControl (hHandle, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &li, sizeof (li), &dwBytesReturned, NULL);
        ::CloseHandle (hHandle);

        Optional<String> persistenceVolumeID;

        /*
         *  Is the 'disk' a 'remote' device (network),  CD-ROM, direct-attached hard disk (e.g. internal) or removable drive,
         */
        Optional<BlockDeviceKind> deviceKind;
        DiskInfoType              di{GetPhysNameForDriveNumber_ (i), persistenceVolumeID, deviceKind, static_cast<uint64_t> (li.Length.QuadPart)};
        result.Add (di);
    }
#endif
    return result;
}

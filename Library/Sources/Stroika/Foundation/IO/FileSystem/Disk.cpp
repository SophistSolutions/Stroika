/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
 *  I thought this might be useful. maybe at some point? But so far it doesn't appear super useful
 *  or needed.
 *  But we can use this to find out the disk kind for physical devices
 *  --LGP 2015-09-30
 */
#ifndef qCaptureDiskDeviceInfoWindows_
#define qCaptureDiskDeviceInfoWindows_ 0
#endif

#if qCaptureDiskDeviceInfoWindows_
#include <devguid.h>
#include <regstr.h>
#include <setupapi.h>
#pragma comment(lib, "Setupapi.lib")
DEFINE_GUID (GUID_DEVINTERFACE_DISK, 0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);
#endif

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    template<>
    constexpr EnumNames<Foundation::IO::FileSystem::BlockDeviceKind> DefaultNames<Foundation::IO::FileSystem::BlockDeviceKind>::k;
}
#endif

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
#if qCaptureDiskDeviceInfoWindows_
namespace {
    list<wstring> GetPhysicalDiskDeviceInfo_ ()
    {
        HDEVINFO      hDeviceInfoSet;
        ULONG         ulMemberIndex;
        ULONG         ulErrorCode;
        BOOL          bFound = FALSE;
        BOOL          bOk;
        list<wstring> disks;

        // create a HDEVINFO with all present devices
        hDeviceInfoSet = ::SetupDiGetClassDevs (&GUID_DEVINTERFACE_DISK, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (hDeviceInfoSet == INVALID_HANDLE_VALUE) {
            _ASSERT (FALSE);
            return disks;
        }

        // enumerate through all devices in the set
        ulMemberIndex = 0;
        while (TRUE) {
            // get device info
            SP_DEVINFO_DATA deviceInfoData;
            deviceInfoData.cbSize = sizeof (SP_DEVINFO_DATA);
            if (!::SetupDiEnumDeviceInfo (hDeviceInfoSet, ulMemberIndex, &deviceInfoData)) {
                if (::GetLastError () == ERROR_NO_MORE_ITEMS) {
                    // ok, reached end of the device enumeration
                    break;
                }
                else {
                    // error
                    _ASSERT (FALSE);
                    ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                    return disks;
                }
            }

            // get device interfaces
            SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
            deviceInterfaceData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);
            if (!::SetupDiEnumDeviceInterfaces (hDeviceInfoSet, NULL, &GUID_DEVINTERFACE_DISK, ulMemberIndex, &deviceInterfaceData)) {
                if (::GetLastError () == ERROR_NO_MORE_ITEMS) {
                    // ok, reached end of the device enumeration
                    break;
                }
                else {
                    // error
                    _ASSERT (FALSE);
                    ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                    return disks;
                }
            }

            // process the next device next time
            ulMemberIndex++;

            // get hardware id of the device
            ULONG ulPropertyRegDataType = 0;
            ULONG ulRequiredSize        = 0;
            ULONG ulBufferSize          = 0;
            BYTE* pbyBuffer             = NULL;
            if (!::SetupDiGetDeviceRegistryProperty (hDeviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, &ulPropertyRegDataType, NULL, 0, &ulRequiredSize)) {
                if (::GetLastError () == ERROR_INSUFFICIENT_BUFFER) {
                    pbyBuffer    = (BYTE*)::malloc (ulRequiredSize);
                    ulBufferSize = ulRequiredSize;
                    if (!::SetupDiGetDeviceRegistryProperty (hDeviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID, &ulPropertyRegDataType, pbyBuffer, ulBufferSize, &ulRequiredSize)) {
                        // getting the hardware id failed
                        _ASSERT (FALSE);
                        ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                        ::free (pbyBuffer);
                        return disks;
                    }
                }
                else {
                    // getting device registry property failed
                    _ASSERT (FALSE);
                    ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                    return disks;
                }
            }
            else {
                // getting hardware id of the device succeeded unexpectedly
                _ASSERT (FALSE);
                ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                return disks;
            }

            // pbyBuffer is initialized now!
            LPCWSTR pszHardwareId = (LPCWSTR)pbyBuffer;

            // retrieve detailed information about the device
            // (especially the device path which is needed to create the device object)
            SP_DEVICE_INTERFACE_DETAIL_DATA* pDeviceInterfaceDetailData      = NULL;
            ULONG                            ulDeviceInterfaceDetailDataSize = 0;
            ulRequiredSize                                                   = 0;
            bOk                                                              = ::SetupDiGetDeviceInterfaceDetail (hDeviceInfoSet, &deviceInterfaceData, pDeviceInterfaceDetailData, ulDeviceInterfaceDetailDataSize, &ulRequiredSize, NULL);
            if (!bOk) {
                ulErrorCode = ::GetLastError ();
                if (ulErrorCode == ERROR_INSUFFICIENT_BUFFER) {
                    // insufficient buffer space
                    // => that's ok, allocate enough space and try again
                    pDeviceInterfaceDetailData         = (SP_DEVICE_INTERFACE_DETAIL_DATA*)::malloc (ulRequiredSize);
                    pDeviceInterfaceDetailData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
                    ulDeviceInterfaceDetailDataSize    = ulRequiredSize;
                    deviceInfoData.cbSize              = sizeof (SP_DEVINFO_DATA);
                    bOk                                = ::SetupDiGetDeviceInterfaceDetail (hDeviceInfoSet, &deviceInterfaceData, pDeviceInterfaceDetailData, ulDeviceInterfaceDetailDataSize, &ulRequiredSize, &deviceInfoData);
                    ulErrorCode                        = ::GetLastError ();
                }

                if (!bOk) {
                    // retrieving detailed information about the device failed
                    _ASSERT (FALSE);
                    ::free (pbyBuffer);
                    ::free (pDeviceInterfaceDetailData);
                    ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                    return disks;
                }
            }
            else {
                // retrieving detailed information about the device succeeded unexpectedly
                _ASSERT (FALSE);
                ::free (pbyBuffer);
                ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);
                return disks;
            }

            disks.push_back (pDeviceInterfaceDetailData->DevicePath);

            // free buffer for device interface details
            ::free (pDeviceInterfaceDetailData);

            // free buffer
            ::free (pbyBuffer);
        }

        // destroy device info list
        ::SetupDiDestroyDeviceInfoList (hDeviceInfoSet);

        return disks;
    }
}
#endif

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

#if qPlatform_Windows
#if qCaptureDiskDeviceInfoWindows_ && 0
    for (auto s : GetPhysicalDiskDeviceInfo_ ()) {
        DbgTrace (L"s=%s", s.c_str ());
    }
#endif

    for (int i = 0; i < 64; i++) {
        HANDLE hHandle = ::CreateFileW (GetPhysNameForDriveNumber_ (i).c_str (), GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hHandle == INVALID_HANDLE_VALUE) {
            break;
        }
        GET_LENGTH_INFORMATION li{};
        {
            DWORD dwBytesReturned{};
            BOOL  bResult = ::DeviceIoControl (hHandle, IOCTL_DISK_GET_LENGTH_INFO, nullptr, 0, &li, sizeof (li), &dwBytesReturned, nullptr);
            if (bResult == 0) {
                DbgTrace (L"failed - DeviceIoControl - IOCTL_DISK_GET_LENGTH_INFO - ignored");
                continue;
            }
        }
        DISK_GEOMETRY driveInfo{};
        {
            DWORD dwBytesReturned{};
            BOOL  bResult = ::DeviceIoControl (hHandle, IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &driveInfo, sizeof (driveInfo), &dwBytesReturned, nullptr);
            if (bResult == 0) {
                DbgTrace (L"failed - DeviceIoControl - IOCTL_DISK_GET_DRIVE_GEOMETRY - ignored");
                continue;
            }
        }
        ::CloseHandle (hHandle);

        /*
         *  Is the 'disk' a 'remote' device (network),  CD-ROM, direct-attached hard disk (e.g. internal) or removable drive,
         */
        optional<BlockDeviceKind> deviceKind;
        switch (driveInfo.MediaType) {
            case FixedMedia:
                deviceKind = BlockDeviceKind::eLocalDisk;
                break;
            case RemovableMedia:
                deviceKind = BlockDeviceKind::eRemovableDisk;
                break;
        }
        DiskInfoType di{GetPhysNameForDriveNumber_ (i), deviceKind, static_cast<uint64_t> (li.Length.QuadPart)};
        result.Add (di);
    }
#endif
    return result;
}

/*
* Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
*/
#ifndef _Stroika_Foundation_IO_FileSystem_MountedFilesystem_h_
#define _Stroika_Foundation_IO_FileSystem_MountedFilesystem_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Collection.h"
#include "../../Containers/Set.h"

#include "Disk.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *       @todo - migrate more static - non -performance data from Frameowrks/Instrumnet/Filesystem
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {

                using Characters::String;
                using Memory::Optional;

                /**
                 *  A 'MountedFilesystem' is a particular mount point, in a particular filesystem format. There is an N <-->M relationship
                 *  between devices (disks) and filesystem mount points.
                 *
                 *  A filesystem can span multiple disks (as with RAID), or multiple filesystems cover a single disk (as with partitioning).
                 *
                 *  In UNIX, a filesystem has only a single point point, where as in windows (???), it CAN have multiple (we dont
                 *  currently model that, but we could make mount point be a set).
                 */
                struct MountedFilesystemType {

                    /**
                     *  This is where the data for this disk/filesystem appear in the filesystem.
                     *
                     *  On unix, it mighth be a place like '/', or '/mnt/usbdisk1'.
                     */
                    String fMountedOn;

                    /**
                     * on UNIX, this is the device path to the raw disk mounted onto.
                     *   e.g. /dev/sda1, or /dev/sda
                     *
                     *  On Windows, this is typically one name (e.g. '\\.\PhysicalDrive1'), but can be multiple, if the filesystem 'spans' multiple physical disks.
                     *
                     *  This value maybe missing if its unable to be collected, but will be present and empty if there are no associated devices.
                     *
                     *  \note - for now - fDevicePaths can only be collected on Windows if you have administrator privilges
                     *
                     *  \note - on Linux - this may not be a full path, but just a special name, like tmpfs, or proc, or udev etc - for these special devices
                     */
                    Optional<Containers::Set<String>> fDevicePaths;

                    /**
                     *  This is an open enumeration indicating the format of the given filesystem:
                     *
                     *  Common values include:
                     *         o    "ext2"
                     *         o    "ext4"
                     *         o    "jfs2"
                     *         o    "nfs"
                     *         o    "nfs3"
                     *         o    "vboxsf"
                     *         o    "NTFS"
                     *         o    "procfs"
                     */
                    Optional<String> fFileSystemType;

                    /**
                     *  This refers - for now - to a windows concept - Volume ID. I think this is roughly equivilent to physical disk
                     *  but I'm not sure.
                     *
                     *  Examples include:
                     *          \\?\Volume{e99304ff-4c5d-11e4-824c-806e6f6e6963}\'
                     *
                     *  This MIGHT be similar to a UNIX GUID that appears on some kinds of disks, but is currently only implemented for Windows.
                     */
                    Optional<String> fVolumeID;

                    /**
                     *  @see Characters::ToString ();
                     */
                    nonvirtual String ToString () const;
                };

                /**
                 *  Fetch all the mounted filesystems on this computer. Somewhat analagous to the UNIX 'df' command, or cat /etc/mtab
                 *
                 *  @todo redo with KeyedCollection - once thats implemented
                 */
                Containers::Collection<MountedFilesystemType> GetMountedFilesystems ();
            }
        }
    }
}

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#include "MountedFilesystem.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_MountedFilesystem_h_*/

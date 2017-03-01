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
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {

                using Characters::String;
                using Memory::Optional;

                /**
                 *  Filesytem mount points are strings, but this type alias is used for documentation.
                 */
                using MountedFilesystemNameType = String;

                // @todo - migrate static - non -performacne data from Frameowrks/Instrumnet/Filesystem

                /**
                &&&&
                *  A volume is analagous to a Windows Volume (@see ) or a unix Filesystem (@see).
                *
                *  In UNIX, a filesystem has only a single point point, where as in windows, it CAN have multiple (we dont
                *  currently model that, but we could make mount point be a set).
                */
                struct MountedFilesystemType {

                    /*
                     *  This is where the data for this disk/filesystem appear in the filesystem.
                     *
                     *  On unix, it mighth be a place like '/', or '/mnt/usbdisk1'.
                     */
                    MountedFilesystemNameType fMountedOn;

                    /**
                     * on UNIX, this is the device path to the raw disk mounted onto.
                     *   e.g. /dev/sda1, or /dev/sda
                     *
                     *  On Windows, this is typically one name, but can be multiple, if the filesystem 'spans' multiple physical disks.
                     */
                    Containers::Set<String> fDevicePaths;

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
                     *  @todo document
                     * WINDOWS SPECIFIC??? FOR NOW??? - unless maybe this belongs as part of the disk object and would be some sort of disk id/signature?
                     */
                    Optional<String> fVolumeID;

                    /**
                     *  @see Characters::ToString ();
                     */
                    nonvirtual String ToString () const;
                };

                /**
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

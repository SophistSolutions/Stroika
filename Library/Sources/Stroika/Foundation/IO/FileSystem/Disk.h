/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Disk_h_
#define _Stroika_Foundation_IO_FileSystem_Disk_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Collection.h"
#include "../../Memory/Optional.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Underneath DiskInfoType - include partitions (which I think maybe like volumnes for windows - maybe not)
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace FileSystem {

                using Characters::String;
                using Memory::Optional;

                /**
                 *  \note   Configuration::DefaultNames<> supported
                 *  \note   These print names are mostly for display and debugging purposes, and they are not guarantied to be safe for
                 *          persistence (so be sure to version).
                 */
                enum BlockDeviceKind {
                    /**
                     *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Removable Disk" or
                     *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_REMOVABLE
                     */
                    eRemovableDisk,

                    /**
                     *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Local Disk" or
                     *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_FIXED
                     */
                    eLocalDisk,

                    /**
                     *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Network Drive" or
                     *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_REMOTE
                     */
                    eNetworkDrive,

                    /**
                     *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "RAM Disk" or
                     *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_RAMDISK
                     *  On Linux, this is tmpfs
                     */
                    eTemporaryFiles,

                    /**
                     *  On Windoze, corresponds to https://msdn.microsoft.com/en-us/library/aa394173%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396 "Compact Disc" or
                     *  https://msdn.microsoft.com/en-us/library/windows/desktop/aa364939%28v=vs.85%29.aspx DRIVE_CDROM
                     */
                    eReadOnlyEjectable,

                    /**
                     *  e.g. Linux procfs
                     */
                    eSystemInformation,

                    Stroika_Define_Enum_Bounds (eRemovableDisk, eSystemInformation)
                };

                // @todo - migrate static - non -performacne data from Frameowrks/Instrumnet/Filesystem

                /**
                 */
                struct DiskInfoType {
                    String fDeviceName;
                    /*
                    *  This is a UNIQUE ID scribbled onto the disk itself, like
                    *  \\\\?\\Volume{e99304fe-4c5d-11e4-824c-806e6f6e6963}\\ This could be used to track when a disk is moved
                    *  from one SATA or SCSI address to another.
                    */
                    Optional<String> fPersistentVolumeID;

                    /*
                    *  Is the 'disk' a 'remote' device (network),  CD-ROM, direct-attached hard disk (e.g. internal) or removable drive,
                    */
                    Optional<BlockDeviceKind> fDeviceKind;

                    /*
                    *  This is the size of the physical block device. All the filesystems must fit in it.
                    */
                    Optional<uint64_t> fSizeInBytes;

                    /**
                     *  @see Characters::ToString ();
                     */
                    nonvirtual String ToString () const;
                };

                /**
                 *  Fetch all th...
                 *
                 *  @todo redo with KeyedCollection - once thats implemented
                 */
                Containers::Collection<DiskInfoType> GetAvailableDisks ();
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Disk.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_Disk_h_*/

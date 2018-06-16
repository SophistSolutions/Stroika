/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Interface_h_
#define _Stroika_Foundation_IO_Network_Interface_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/Enumeration.h"
#include "../../Containers/Set.h"
#include "../../Memory/Optional.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   Use http://stackoverflow.com/questions/14264371/how-to-get-nic-details-from-a-c-program to grab ethernet speed
 *              for inteface (bandwidth of network interface)
 *
 *      @todo   Handle case of multiple address bindings to a single interface (groupby).
 *
 *      @todo   Incomplete POSIX IMPL
 *              IPV6 compat for POSIX (already done for windows)
 *
 *      @todo   Fix use of assert - SB exceptions mostly...
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                using Characters::String;

                /**
                 *  Capture details describing a network interface.
                 */
                struct Interface {
                    /**
                     *      This is a somewhat artificial concept - which is introduced in Stroika. This is only guaranteed
                     *   unique or the life of one program lifetime (@todo - not even sure we can do that much).
                     *
                     *      UNIX:
                     *          interface name - e.g. eth0 - in the first column reported in ifconfig.
                     *
                     *      WINDOWS:
                     *          IP_ADAPTER_ADDRESSES::fAdapterName
                     *
                     *          This is interface AdapterName, which is not particularly printable (usualy a GUID)
                     */
                    String fInternalInterfaceID;

#if qPlatform_POSIX
                    /**
                     *  On unix, its the interface name, e.g. eth0, eth1, etc.
                     *  On Windows, this is concept doesn't really exist.
                     */
                    nonvirtual String GetInterfaceName () const;
#endif

                    /**
                     *  This is a generally good display name to describe a network interface.
                     *
                     *      WINDOWS:
                     *          IP_ADAPTER_ADDRESSES::FriendlyName
                     *
                     *          A user-friendly name for the adapter. For example: "Local Area Connection 1." This name appears
                     *          in contexts such as the ipconfig command line program and the Connection folder.
                     *
                     *          Note this name is often user-editable
                     */
                    String fFriendlyName;

                    /**
                     *  This description of the adpapter is typically a short string describing the hardware, such as
                     *      "Intel(R) Dual Band Wireless-AC 7260"
                     */
                    Memory::Optional<String> fDescription;

                    /**
                     *
                     *  \note   Configuration::DefaultNames<> supported
                     */
                    enum class Type {
                        eLoopback,
                        eWiredEthernet,
                        eWIFI,
                        eTunnel,
                        eOther,

                        Stroika_Define_Enum_Bounds (eLoopback, eOther)
                    };

                    /**
                     */
                    Memory::Optional<Type> fType;

                    /**
                     *  This - if present - is typically an ethernet macaddr (6 bytes in hex separated by :)
                     */
                    Memory::Optional<String> fHardwareAddress;

                    /**
                     *  bits per second
                     */
                    Memory::Optional<uint64_t> fTransmitSpeedBaud;

                    /**
                     *  bits per second
                     */
                    Memory::Optional<uint64_t> fReceiveLinkSpeedBaud;

                    /**
                     */
                    Containers::Set<InternetAddress> fBindings; // can be IPv4 or IPv6

                    /**
                     * @todo document these - 'eRunning' == LINUX RUNNING
                     *
                     *  \note   Configuration::DefaultNames<> supported
                     */
                    enum class Status {
                        eConnected,
                        eRunning,

                        Stroika_Define_Enum_Bounds (eConnected, eRunning)
                    };

                    /**
                     */
                    Memory::Optional<Containers::Set<Status>> fStatus;

                    /**
                     *  @see Characters::ToString ();
                     */
                    nonvirtual String ToString () const;
                };

                /**
                 *  Collect all the interfaces (and their status) from the operating system.
                 */
                Traversal::Iterable<Interface> GetInterfaces ();

                /**
                 *  Find the interface object with the given ID.
                 *
                 *  @see Interface::fInternalInterfaceID
                 */
                Memory::Optional<Interface> GetInterfaceById (const String& internalInterfaceID);
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Interface.inl"

#endif /*_Stroika_Foundation_IO_Network_Interface_h_*/

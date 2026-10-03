/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_LinkMonitor_h_
#define _Stroika_Foundation_IO_Network_LinkMonitor_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Platform_POSIX
#include <arpa/inet.h>
#elif qStroika_Platform_Windows
#include <WinSock2.h>

#include <in6addr.h>
#include <inaddr.h>
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Execution/Function.h"
#include "Stroika/Foundation/IO/Network/Interface.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Should this API be renamed InterfaceMonitor? Probably yes?
 *
 *      @todo   LinkMonitor rnetlink support DELETE
 *
 *      @todo   Support remove callback (once we have new FUNCTION helper - copyable stdfunction) - and usethat
 *              for SignalHandlers as well.
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    /**
     *  Create an instance of this class, and add callbacks to it, and they will be notified
     *  when a network connection comes up or down.
     *
     *  All LinkMonitors share one watcher of the OS's address changes - on Linux and macOS a thread, on Windows an OS
     *  registration (NotifyUnicastIpAddressChange) - started by the first to add a callback, and stopped when the last of those
     *  goes. So several cost about what one does.
     *
     *  \note  A change from when AddCallback () returns is reported. Where the OS cannot tell (e.g. some containers),
     *         AddCallback () throws.
     *
     *  \note  Callbacks run on that thread (on Windows, the OS's), not the caller's - so a slow one delays the others' - and
     *         one LinkMonitor's one at a time. An exception from one is logged and ignored.
     *
     *  \note  Once RemoveCallback () returns, or the LinkMonitor is destroyed, that callback is not running and is not called
     *         again (except that, done from within the callback itself, the call already running finishes). A callback may
     *         add or remove callbacks, and create or destroy LinkMonitors - but not destroy the last one: that would have to
     *         stop the thread calling it.
     *
     *  \note  AddCallback () and RemoveCallback () may be called from any thread, at once.
     *
     *  @todo  POSIX code is not really posix but assumes linux==posix =- relaly need separate define to check for netlink
     *         and a windoze impl.
     *
     *  @todo   Decide if this should always auto-call the callback when first loaded? Might be an easier to use
     *          API (with added - for any existing interfaces)? Windoze makes that easy, but not sure about rnetlink?
     */
    struct LinkMonitor {
        LinkMonitor ();
        LinkMonitor (LinkMonitor&& rhs) noexcept = default;
        LinkMonitor (const LinkMonitor&)         = delete;

        LinkMonitor& operator= (LinkMonitor&& rhs) noexcept = default;
        LinkMonitor& operator= (const LinkMonitor&)         = delete;

        /**
         *  \note   Common::DefaultNames<> supported
         */
        enum class LinkChange {
            eAdded,
            eRemoved,

            Stroika_Define_Enum_Bounds (eAdded, eRemoved)
        };

        /**
         *  \brief What a callback is told: an address added to (or removed from) an interface.
         *
         *  \note IPv4 and IPv6 addresses. Only an address's becoming usable, and ceasing to be, is reported - not a change that
         *        leaves it as it was (its lifetime renewed, say). So an address still checking it is no duplicate is reported
         *        once that is done (Linux, Windows), and on Windows - which keeps the address of a network that goes (Wi-Fi off,
         *        within its DHCP lease), marking it deprecated - the network's going and coming back is reported as the
         *        address's removal and addition.
         */
        struct Event {
            /**
             *  Whether fAddress was added to the interface, or removed from it.
             */
            LinkChange fChange;

            /**
             *  The interface's Interface::fInterfaceID - so SystemInterfacesMgr::GetById () finds the rest of it, if it is still
             *  there. Empty where the OS no longer knows the interface (an address removed along with it, say).
             */
            Interface::SystemIDType fInterfaceID;

            /**
             *  The address added (or removed) - always given: each Event is about one address. Linux: the netlink message's
             *  IFA_LOCAL (else IFA_ADDRESS, as for IPv6); macOS: the routing message's RTAX_IFA; Windows: MIB_UNICASTIPADDRESS_ROW::Address.
             */
            InternetAddress fAddress;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        using Callback = Execution::Function<void (const Event&)>;
        nonvirtual void AddCallback (const Callback& callback);
        nonvirtual void RemoveCallback (const Callback& callback);

    private:
        struct Rep_;
        shared_ptr<Rep_> fRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LinkMonitor.inl"

#endif /*_Stroika_Foundation_IO_Network_LinkMonitor_h_*/

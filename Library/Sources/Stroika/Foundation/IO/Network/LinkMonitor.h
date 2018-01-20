/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_LinkMonitor_h_
#define _Stroika_Foundation_IO_Network_LinkMonitor_h_ 1

#include "../../StroikaPreComp.h"

#if qPlatform_POSIX
#include <arpa/inet.h>
#elif qPlatform_Windows
#include <WinSock2.h>

#include <in6addr.h>
#include <inaddr.h>
#endif

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Execution/Function.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Should this API be renamed InterfaceMonitor? Probably yes?
 *
 *      @todo   Optimize of listener is STATIC - and so we only register one (and esp on linux with threads)
 *              only one thread - and fan out to all subscribers. SB pretty easy - just make data static,
 *              but I guess one trick is then you need to be able to remove callbacks (ours - need Function).
 *
 *      @todo   LinkMonitor rnetlink support DELETE
 *
 *      @todo   Support remove callback (once we have new FUNCTION helper - copyable stdfunction) - and usethat
 *              for SignalHandlers as well.
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                using Characters::String;

                //
                //  @todo Move to Interace API???
                //
                //// add options for filtering out disabled ones (not onlnie)
                /// add optikon for fitlering out localhost
                //
                // This isn't always well defined, but is typically. This is the primary ip address used to address this machine.
                // This CAN return an empty address if none available (like not connected to a network).
                //. (if no net do we return localhost?))
                //
                //  @todo THIS WHOLE API SUCKS (IPV4 or IPV6, and what doees this man for multihomed devices?
                InternetAddress GetPrimaryInternetAddress ();

                //
                //  @todo Move to Interace API???
                //
                // Usefull as a (semi)persisent seed for GUIDs etc
                // Otherwise, not super-well defined
                // THINK OUT RELATION BETWEENT HIS AND GetPrimaryInternetAddress???
                //
                // think through and document error conditions
                String GetPrimaryNetworkDeviceMacAddress ();

                /**
                 *  Create an instance of this class, and add callbacks to it, and they will be notified
                 *  when a network connection comes up or down.
                 *
                 *  @todo  POSIX code is not really posix but assumes linux==posix =- relaly need separate define to check for netlink
                 *         and a windoze impl.
                 *
                 *  @todo   Decide if this should always auto-call the callback when first loaded? Might be an easier to use
                 *          API (with added - for any existing interfaces)? Windoze makes that easy, but not sure about rnetlink?
                 */
                struct LinkMonitor {
                    LinkMonitor ();
                    LinkMonitor (const LinkMonitor&& rhs);
                    LinkMonitor (const LinkMonitor&) = delete;
                    LinkMonitor& operator= (const LinkMonitor&) = delete;

                    enum class LinkChange {
                        eAdded,
                        eRemoved,
                    };
                    using Callback = Execution::Function<void(LinkChange, const String& linkName, const String& ipAddr)>;
                    nonvirtual void AddCallback (const Callback& callback);
                    nonvirtual void RemoveCallback (const Callback& callback);

                private:
                    struct Rep_;
                    shared_ptr<Rep_> fRep_;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LinkMonitor.inl"

#endif /*_Stroika_Foundation_IO_Network_LinkMonitor_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_NetworkInterfaces_h_
#define _Stroika_Foundation_IO_Network_NetworkInterfaces_h_    1

#include    "../../StroikaPreComp.h"

#if     qPlatform_POSIX
#include    <arpa/inet.h>
#elif   qPlatform_Windows
#include    <WinSock2.h>
#include    <inaddr.h>
#include    <in6addr.h>
#endif

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"

#include    "InternetAddress.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                using   Characters::String;


                //// @todo Add somthing to return
                /// struct INTERFCE { name; set<IPADDR> ()}
                //

                // This isn't always well defined, but is typically. This is the primary ip address used to address this machine.
                // This CAN return an empty address if none available (like not connected to a network).
                //. (if no net do we return localhost?))
                InternetAddress GetPrimaryInternetAddress ();


                //// ADD API for listint all interfaces (as above)

                //// add options for filtering out disabled ones (not onlnie)
                /// add optikon for fitlering out localhost


                // Usefull as a (semi)persisent seed for GUIDs etc
                // Otherwise, not super-well defined
                // THINK OUT RELATION BETWEENT HIS AND GetPrimaryInternetAddress???
                //
                // think through and document erorr conditions
                String GetPrimaryNetworkDeviceMacAddress ();


                /**
                 *  Create an instance of this class, and add callbacks to it, and they will be notified
                 *  when a network connection comes up or down.
                 *
                 *  @todo - IF we add a new stroika Function() class (like std::function but that you can compare)
                 *          we can then add a RemoveCallback () funciton!!! HANDY IDEA
                 *
                 *  @todo  POSIX code is not really posix but assumes linux==posix =- relaly need separate define to check for netlink
                 *         and a windoze impl.
                 *
                 *  @todo   Decide if this should always auto-call the callback when first loaded? Might be an easier to use
                 *          API (with added - for any existing interfaces)? Windoze makes that easy, but not sure about rnetlink?
                 */
                struct LinkMonitor {
                    LinkMonitor ();
                    LinkMonitor (const LinkMonitor&) = delete;
                    const LinkMonitor& operator= (const LinkMonitor&) = delete;

                    enum class LinkChange {
                        eAdded,
                        eRemoved,
                    };
                    nonvirtual  void    AddCallback (const function<void(LinkChange, const String& linkName, const String& ipAddr)>& callback);

                private:
                    struct      Rep_;
                    shared_ptr<Rep_>    fRep_;
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
#include    "LinkMonitor.inl"

#endif  /*_Stroika_Foundation_IO_Network_NetworkInterfaces_h_*/

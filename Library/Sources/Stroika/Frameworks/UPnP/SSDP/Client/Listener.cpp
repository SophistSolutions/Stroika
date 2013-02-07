/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Execution/ErrNoException.h"
#include    "../../../../Foundation/IO/Network/Socket.h"

#include    "Listener.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;
using   namespace   Stroika::Frameworks::UPnP::SSDP::Client;

/*
 *  See http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
 *  for details on the SSDP specification.
 *
 *  And http://www.upnp-hacks.org/upnp.html for more hints.
 */


namespace {
    constexpr   char    SSDP_MULTICAST[]    =      "239.255.255.250";
    constexpr   int     SSDP_PORT           =       1900;
}



// must run this on another thread
class   Listener::Rep_ {
public:
    Rep_ (const std::function<void(const Device& d)>& callOnFinds)
        : fCallOnFinds_ (callOnFinds)
        , fSocket_ (Socket::SocketKind::DGRAM) {
        Socket::BindFlags   bindFlags   =   Socket::BindFlags ();
        bindFlags.fReUseAddr = true;
        fSocket_.Bind (SocketAddress (V4::kAddrAny, SSDP_PORT), bindFlags);
        fSocket_.JoinMulticastGroup (InternetAddress (SSDP_MULTICAST));

        // unclear how much of this SB here, and how much in RUN method... Doing here makes more latency on construction but better error reporting(exceptions thrown here)

        // IN RUN method...
        while (1) {
            Byte    buf[1024];
            SocketAddress   from;
            size_t nBytesRead = fSocket_.ReceiveFrom (StartOfArray (buf), EndOfArray (buf), 0, &from);
            // maybe dont pass from - we ignore..
            // Bind buffer to input text stream (readonly) - and read strings from it...
            // then see if it looks like SSDP notify (see ssdp spec above for format - or run wireshark and see what devcices send out)
            // then call callback with results..
        }
    }

private:
    std::function<void(const Device& d)>    fCallOnFinds_;
    Socket                                  fSocket_;
};






Listener::Listener (const std::function<void(const Device& d)>& callOnFinds)
    : fRep_ (new Rep_ (callOnFinds))
{
}




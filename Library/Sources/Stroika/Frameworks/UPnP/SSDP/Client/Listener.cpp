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
        fSocket_.Bind (SocketAddress (InternetAddress (SSDP_MULTICAST, InternetAddress::AddressFamily::V4), SSDP_PORT), bindFlags);
    }

private:
    std::function<void(const Device& d)>    fCallOnFinds_;
    Socket                                  fSocket_;
};






Listener::Listener (const std::function<void(const Device& d)>& callOnFinds)
    : fRep_ (new Rep_ (callOnFinds))
{
}




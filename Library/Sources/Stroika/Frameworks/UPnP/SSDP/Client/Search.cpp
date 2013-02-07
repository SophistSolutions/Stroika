/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    <sstream>

#include    "../../../../Foundation/Execution/ErrNoException.h"
#include    "../../../../Foundation/IO/Network/Socket.h"

#include    "Search.h"


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



// @todo must run this on another thread
class   Search::Rep_ {
public:
    Rep_ (const String& serviceType, const std::function<void(const Device& d)>& callOnFinds)
        : fCallOnFinds_ (callOnFinds)
        , fSocket_ (Socket::SocketKind::DGRAM) {

        SocketAddress multicastSSDPDestAddr = SocketAddress (InternetAddress (SSDP_MULTICAST, InternetAddress::AddressFamily::V4), SSDP_PORT);
        /// Note - most of thjis must run in another thread

        /// better error handling - maybe a separate callback for errors?


        string  request;
        {
            const   int kMaxHops_   =   3;
            stringstream    requestBuf;
            requestBuf << "M-SEARCH * HTTP/1.1\r\n";
            requestBuf << "Host: " << SSDP_MULTICAST << ":" << SSDP_PORT << "\r\n";
            requestBuf << "Man: \"ssdp:discover\"\r\n";
            requestBuf << "ST: " << serviceType.c_str () << "\r\n";
            requestBuf << "MX: " << kMaxHops_ << "\r\n";
            requestBuf << "\r\n";
            request = requestBuf.str ();
        }
        fSocket_.SendTo (reinterpret_cast<const Byte*> (request.c_str ()), reinterpret_cast<const Byte*> (request.c_str () + request.length ()), multicastSSDPDestAddr);

        fd_set fds;
        timeval timeout;
        FD_ZERO(&fds);
        FD_SET(fSocket_.GetNativeSocket (), &fds);
        timeout.tv_sec = 30;        // think out timeouts..., and use stroika time code..
        timeout.tv_usec = 0;

        // maybe just lose throw here - and intentionally ignore errors?
        Execution::ThrowErrNoIfNegative (select (fSocket_.GetNativeSocket () + 1, &fds, NULL, NULL, &timeout));
        if (FD_ISSET (fSocket_.GetNativeSocket (), &fds)) {

            Byte buf[4 * 1024]; // not sure what right size here would be?
            try {
                SocketAddress clientSock;
                size_t  len = fSocket_.ReceiveFrom (StartOfArray (buf), EndOfArray (buf), 0, &clientSock);
                buf[len] = '\0';
            }
            catch (...) {
                // ignore errors here/?? maybe??
            }

            /* Check the HTTP response code */
            if(strncmp(reinterpret_cast<char*> (buf), "HTTP/1.1 200 OK", 12) != 0) {
                //printf("err: ssdp parsing ");
                // must loop and ignore these
//                return -1;
            }
            // do callback - but first must parse buffer...
            //fCallOnFinds_ (buffer);
        }
    }

private:
    std::function<void(const Device& d)>    fCallOnFinds_;
    Socket                                  fSocket_;
};






Search::Search (const String& serviceType, const std::function<void(const Device& d)>& callOnFinds)
    : fRep_ (new Rep_ (serviceType, callOnFinds))
{
}


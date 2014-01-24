/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/Format.h"
#include    "../../../../Foundation/Execution/Sleep.h"
#include    "../../../../Foundation/Execution/Thread.h"
#include    "../../../../Foundation/IO/Network/Socket.h"
#include    "../../../../Foundation/Streams/BasicBinaryOutputStream.h"
#include    "../../../../Foundation/Streams/TextOutputStreamBinaryAdapter.h"

#include    "../Advertisement.h"
#include    "../Common.h"
#include    "PeriodicNotifier.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::UPnP;
using   namespace   Stroika::Frameworks::UPnP::SSDP;
using   namespace   Stroika::Frameworks::UPnP::SSDP::Server;



// Comment this in to turn on tracing in this module
//#define   USE_TRACE_IN_THIS_MODULE_       1




/*
********************************************************************************
******************************** PeriodicNotifier ******************************
********************************************************************************
*/
PeriodicNotifier::PeriodicNotifier ()
    : fListenThread_ ()
{
}

PeriodicNotifier::~PeriodicNotifier ()
{
    // Even though no this pointer captured, we must shutdown any running threads before this object terminated else it would run
    // after main exists...
    Execution::Thread::SuppressAbortInContext  suppressAbort;
    fListenThread_.AbortAndWaitForDone ();
}

void    PeriodicNotifier::Run (const Iterable<Advertisement>& advertisements, const FrequencyInfo& fi)
{
    fListenThread_ = Execution::Thread ([advertisements, fi]() {
        Socket s (Socket::SocketKind::DGRAM);
        Socket::BindFlags   bindFlags = Socket::BindFlags ();
        bindFlags.fReUseAddr = true;
        s.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), bindFlags);
#if     qDefaultTracingOn
        bool    firstTimeThru   =   true;
#endif
        while (true) {
#if     qDefaultTracingOn
            if (firstTimeThru) {
                Debug::TraceContextBumper ctx (SDKSTR ("SSDP PeriodicNotifier - first time notifications"));
                for (auto a : advertisements) {
                    DbgTrace (L"(alive,loc=%s,usn=%s,...)", a.fLocation.c_str (), a.fUSN.c_str ());
                }
                firstTimeThru = false;
            }
#endif
#if     USE_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx (SDKSTR ("SSDP PeriodicNotifier - notifications"));
            for (auto a : advertisements) {
#if     USE_TRACE_IN_THIS_MODULE_
                String msg;
                msg += L"alive,";
                msg += String (L"location=") + a.fLocation + L", ";
                msg += String (L"ST=") + a.fST + L", ";
                msg += String (L"USN=") + a.fUSN;
                DbgTrace (L"(%s)", msg.c_str ());
#endif
            }
#endif
            try {
                for (auto a : advertisements) {
                    a.fAlive = true;   // periodic notifier must announce alive (we dont support 'going down' yet
                    Memory::BLOB    data = SSDP::Serialize (L"NOTIFY * HTTP/1.1", a);
                    s.SendTo (data.begin (), data.end (), UPnP::SSDP::V4::kSocketAddress);
                }
            }
            catch (const Execution::errno_ErrorException& e) {
                // Error ENETUNREACH is common when you have network connection issues, for example on boot before
                // full connection
                DbgTrace (L"Ignoring inability to send SSDP notify packets: %s (try again later)", String::FromSDKString (e.LookupMessage ()).c_str ());
            }
            catch (const Execution::ThreadAbortException&) {
                Execution::DoReThrow ();
            }
            catch (...) {
                DbgTrace (L"Ignoring inability to send SSDP notify packets (try again later)");
            }
            Execution::Sleep (30.0);
        }
    });
    fListenThread_.SetThreadName (L"SSDP Periodic Notifier thread");
    fListenThread_.Start ();
    fListenThread_.WaitForDone ();
}

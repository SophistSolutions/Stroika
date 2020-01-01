/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../../Foundation/Characters/Format.h"
#include "../../../../Foundation/Characters/String_Constant.h"
#include "../../../../Foundation/Characters/ToString.h"
#include "../../../../Foundation/Execution/Sleep.h"
#include "../../../../Foundation/Execution/Thread.h"
#include "../../../../Foundation/IO/Network/ConnectionlessSocket.h"

#include "../Advertisement.h"
#include "../Common.h"
#include "PeriodicNotifier.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SSDP;
using namespace Stroika::Frameworks::UPnP::SSDP::Server;

// Comment this in to turn on tracing in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ******************************** PeriodicNotifier ******************************
 ********************************************************************************
 */
void PeriodicNotifier::Run (const Iterable<Advertisement>& advertisements, const FrequencyInfo& fi)
{
#if qDebug
    for (auto a : advertisements) {
        Require (not a.fTarget.empty ());
    }
#endif
    static const String kThreadName_{String_Constant{L"SSDP Periodic Notifier"}};
    fListenThread_ = Execution::Thread::New (
        [advertisements, fi] () {
            ConnectionlessSocket::Ptr s = ConnectionlessSocket::New (SocketAddress::INET, Socket::DGRAM);
            s.Bind (SocketAddress (Network::V4::kAddrAny, UPnP::SSDP::V4::kSocketAddress.GetPort ()), Socket::BindFlags{true});
//s.Bind (SocketAddress (Network::V6::kAddrAny, UPnP::SSDP::V6::kSocketAddress.GetPort ()), Socket::BindFlags{true});
#if qDefaultTracingOn
            bool firstTimeThru = true;
#endif
            while (true) {
#if qDefaultTracingOn
                if (firstTimeThru) {
                    Debug::TraceContextBumper ctx ("SSDP PeriodicNotifier - first time notifications");
                    for (auto a : advertisements) {
                        DbgTrace (L"(alive,loc=%s,usn=%s,...)", Characters::ToString (a.fLocation).c_str (), a.fUSN.c_str ());
                    }
                    firstTimeThru = false;
                }
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx ("SSDP PeriodicNotifier - notifications");
                for (auto a : advertisements) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    String msg;
                    msg += L"alive," sz;
                    msg += L"location=" sz + a.fLocation + L", " sz;
                    msg += L"ST=" sz + a.fST + L", " sz;
                    msg += L"USN=" sz + a.fUSN;
                    DbgTrace (L"(%s)", msg.c_str ());
#endif
                }
#endif
                try {
                    for (auto a : advertisements) {
                        a.fAlive          = true; // periodic notifier must announce alive (we don't support 'going down' yet)
                        Memory::BLOB data = SSDP::Serialize (L"NOTIFY * HTTP/1.1", SearchOrNotify::Notify, a);
                        s.SendTo (data.begin (), data.end (), UPnP::SSDP::V4::kSocketAddress);
                    }
                }
                catch ([[maybe_unused]] const system_error& e) {
                    // Error ENETUNREACH is common when you have network connection issues, for example on boot before
                    // full connection
                    DbgTrace (L"Ignoring inability to send SSDP notify packets: %s (try again later)", Characters::ToString (e).c_str ());
                }
                catch (const Execution::Thread::AbortException&) {
                    Execution::ReThrow ();
                }
                catch (...) {
                    DbgTrace (L"Ignoring inability to send SSDP notify packets (try again later)");
                }
                Execution::Sleep (fi.fRepeatInterval);
            }
        },
        Execution::Thread::eAutoStart, kThreadName_);
    fListenThread_.Join ();
}

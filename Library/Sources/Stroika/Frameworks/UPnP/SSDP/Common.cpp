/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <random>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "Common.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP::SSDP;

namespace {
    constexpr char     SSDP_MULTICAST_[] = "239.255.255.250";
    constexpr uint16_t SSDP_PORT_        = 1900;
}

const SocketAddress UPnP::SSDP::V4::kSocketAddress = SocketAddress{InternetAddress{SSDP_MULTICAST_, InternetAddress::AddressFamily::V4}, SSDP_PORT_};
const SocketAddress UPnP::SSDP::V6::kSocketAddress = SocketAddress{InternetAddress{"FF02::C"sv, InternetAddress::AddressFamily::V6}, SSDP_PORT_};

String UPnP::SSDP::MakeServerHeaderValue (const String& useProductTokenWithVersion, const String& usePlatformTokenAndVersion, const String& useUPNPVersion)
{
    Require (not useProductTokenWithVersion.empty ());
    Require (not usePlatformTokenAndVersion.empty ());
    Require (not useUPNPVersion.empty ());
    static const String kSpace_{" "sv};
    return usePlatformTokenAndVersion + kSpace_ + useUPNPVersion + kSpace_ + useProductTokenWithVersion;
}

/*
 ********************************************************************************
 *************************** SSDP::DefaultInterfaceFilter ***********************
 ********************************************************************************
 */
bool UPnP::SSDP::DefaultInterfaceFilter (const Interface& i)
{
    return i.fType != Interface::Type::eLoopback and i.fStatus and i.fStatus->Contains (Interface::Status::eRunning);
}

/*
 ********************************************************************************
 ********************** SSDP::Private_::GetSSDPInterfaces ***********************
 ********************************************************************************
 */
InterfacesByID UPnP::SSDP::Private_::GetSSDPInterfaces (const InterfaceFilter& filter)
{
    return SystemInterfacesMgr{}.GetAll ().Where ([&] (const Interface& i) { return filter (i); });
}

/*
 ********************************************************************************
 ********************* SSDP::Private_::JoinOnEveryInterface *********************
 ********************************************************************************
 */
InterfacesByID UPnP::SSDP::Private_::JoinOnEveryInterface (const Traversal::Iterable<pair<ConnectionlessSocket::Ptr, InternetAddress>>& socketsAndGroups,
                                                           const InterfaceFilter& filter)
{
    InterfacesByID joinedOn;
    for (const Interface& i : GetSSDPInterfaces (filter)) {
        bool joined = false;
        for (const pair<ConnectionlessSocket::Ptr, InternetAddress>& sg : socketsAndGroups) {
            if (not i.fBindings.fAddresses.Any (
                    [&] (const InternetAddress& a) { return a.GetAddressFamily () == sg.second.GetAddressFamily (); })) {
                continue;
            }
            try {
                sg.first.JoinMulticastGroup (sg.second, i);
                joined = true;
            }
            catch (const Execution::Thread::AbortException&) {
                Execution::ReThrow ();
            }
            catch (const system_error& e) {
                // already a member there - as after a network appears, when this joins again on every interface - counts as joined.
                // POSIX says so (EADDRINUSE); Windows only that an argument is invalid (WSAEINVAL, as of Windows 11 10.0.26200), which
                // a join otherwise is not - the group being SSDP's, and the interface one with an address of its family
                if (Execution::IsA (e, errc::address_in_use) or (qStroika_Platform_Windows and Execution::IsA (e, errc::invalid_argument))) {
                    joined = true;
                }
                else {
                    DbgTrace ("SSDP: could not join {} on {}: {}"_f, sg.second, i.fInterfaceID, current_exception ());
                }
            }
            catch (...) {
                DbgTrace ("SSDP: could not join {} on {}: {}"_f, sg.second, i.fInterfaceID, current_exception ());
            }
        }
        if (joined) {
            joinedOn.Add (i);
        }
    }
    return joinedOn;
}

/*
 ********************************************************************************
 ******************** SSDP::Private_::NetworkChangeFollower *********************
 ********************************************************************************
 */
namespace {
    // how long address additions must stop for, before acting on them - a network coming up adds its addresses within moments
    constexpr Time::DurationSeconds kNetworkChangesQuietPeriod_{1.0};
}
struct UPnP::SSDP::Private_::NetworkChangeFollower::Rep_ {
    Rep_ (const function<void ()>& onNetworkAppeared)
        : fOnNetworkAppeared{onNetworkAppeared}
    {
    }

    // on fLinkMonitor's thread: an address was added - so a burst begins, or goes on
    void AddressAdded ()
    {
        static const String         kThreadName_ = "SSDP network changes"sv;
        [[maybe_unused]] lock_guard critSec{fMutex};
        fQuietAt = Time::GetTickCount () + kNetworkChangesQuietPeriod_;
        if (not fFollowingBurst) {
            if (fThread != nullptr) {
                fThread.WaitForDone (); // the last burst's, ending: it takes fMutex no more
            }
            fThread         = Execution::Thread::New ([this] () { FollowBurst_ (); }, Execution::Thread::eAutoStart, kThreadName_);
            fFollowingBurst = true;
        }
    }

    // on fThread: once the burst goes quiet, act on it - and on any that comes meanwhile; then end
    void FollowBurst_ ()
    {
        while (true) {
            optional<Time::TimePointSeconds> waitUntil;
            {
                [[maybe_unused]] lock_guard critSec{fMutex};
                if (not fQuietAt) {
                    fFollowingBurst = false;
                    return;
                }
                if (Time::GetTickCount () < *fQuietAt) {
                    waitUntil = fQuietAt;
                }
                else {
                    fQuietAt = nullopt; // so an address added from now on is acted on again
                }
            }
            if (waitUntil) {
                Execution::SleepUntil (*waitUntil);
                continue;
            }
            Debug::TraceContextBumper ctx{"SSDP: a network appeared"};
            try {
                fOnNetworkAppeared ();
            }
            catch (const Execution::Thread::AbortException&) {
                Execution::ReThrow ();
            }
            catch (...) {
                DbgTrace ("SSDP: could not act on the network that appeared: {}"_f, current_exception ());
            }
        }
    }

    const function<void ()>          fOnNetworkAppeared;
    mutex                            fMutex;
    optional<Time::TimePointSeconds> fQuietAt; // guarded by fMutex: when the burst going on is over, unless another address comes first
    bool fFollowingBurst{false}; // guarded by fMutex: fThread is running FollowBurst_, so looks at fQuietAt again before it ends
    Execution::Thread::CleanupPtr fThread{Execution::Thread::CleanupPtr::eAbortBeforeWaiting}; // guarded by fMutex
    LinkMonitor                   fLinkMonitor; // last, so destroyed first: no burst begins while fThread stops
};

UPnP::SSDP::Private_::NetworkChangeFollower::NetworkChangeFollower (const function<void ()>& onNetworkAppeared)
    : fRep_{make_unique<Rep_> (onNetworkAppeared)}
{
    fRep_->fLinkMonitor.AddCallback ([rep = fRep_.get ()] (const LinkMonitor::Event& e) {
        if (e.fChange == LinkMonitor::LinkChange::eAdded) {
            DbgTrace ("SSDP: a network address appeared: {}"_f, e);
            rep->AddressAdded ();
        }
    });
}
UPnP::SSDP::Private_::NetworkChangeFollower::NetworkChangeFollower (NetworkChangeFollower&&) noexcept                    = default;
auto UPnP::SSDP::Private_::NetworkChangeFollower::operator= (NetworkChangeFollower&&) noexcept -> NetworkChangeFollower& = default;
UPnP::SSDP::Private_::NetworkChangeFollower::~NetworkChangeFollower ()                                                   = default;

/*
 ********************************************************************************
 ********************* SSDP::Private_::FollowNetworkChanges *********************
 ********************************************************************************
 */
optional<UPnP::SSDP::Private_::NetworkChangeFollower> UPnP::SSDP::Private_::FollowNetworkChanges (const function<void ()>& onNetworkAppeared)
{
    try {
        return NetworkChangeFollower{onNetworkAppeared};
    }
    catch (const Execution::Thread::AbortException&) {
        Execution::ReThrow ();
    }
    catch (...) {
        DbgTrace ("SSDP: cannot follow network changes here: {}"_f, current_exception ());
        return nullopt;
    }
}

/*
 ********************************************************************************
 ************************ SSDP::Private_::RandomDuration ************************
 ********************************************************************************
 */
Time::DurationSeconds UPnP::SSDP::Private_::RandomDuration (Time::DurationSeconds atMost)
{
    Require (atMost >= 0s);
    if (atMost == 0s) {
        return 0s;
    }
    static thread_local mt19937 sGenerator_{random_device{}()};
    return Time::DurationSeconds{uniform_real_distribution<double>{0, atMost.count ()}(sGenerator_)};
}

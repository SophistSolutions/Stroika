/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <atomic>
#include <cmath>
#include <mutex>
#include <utility>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#if qStroika_Foundation_DataExchange_XML_SupportDOM
#include "Stroika/Foundation/DataExchange/XML/DOM.h"
#endif
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"
#include "Stroika/Foundation/IO/Network/DNS.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"

#include "Subscriber.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::GENA;

namespace {
    // how often to try again, a renewal and a new subscription having failed
    constexpr Time::DurationSeconds kRetryAfter_{30.0};

    // a response header, by name - whatever its case
    optional<String> Header_ (const Transfer::Response& r, const String& name)
    {
        for (const KeyValuePair<String, String>& h : r.GetHeaders ()) {
            if (String::EqualsComparer{eCaseInsensitive}(h.fKey, name)) {
                return h.fValue.Trim ();
            }
        }
        return nullopt;
    }

    // a TIMEOUT header's duration - Second-, then a number of seconds (UPnP Device Architecture 1.1, section 4.1.1) - as Publisher's
    optional<Time::DurationSeconds> Timeout_ (const optional<String>& timeout)
    {
        if (timeout and timeout->StartsWith ("Second-"sv, eCaseInsensitive)) {
            if (int64_t seconds = String2Int<int64_t> (timeout->SubString (7)); seconds > 0) { // 0: not a number
                return Time::DurationSeconds{static_cast<double> (seconds)};
            }
        }
        return nullopt;
    }

    // a NOTIFY's body's variables: each e:property's one element, by name (UPnP Device Architecture 1.1, section 4.3.2) - their
    // values plain text, as UPnP's types are. Read with the XML DOM: without one, Start throws, so none is read
    Mapping<String, String> PropertySet_ ([[maybe_unused]] const Memory::BLOB& body)
    {
        Mapping<String, String> result;
#if qStroika_Foundation_DataExchange_XML_SupportDOM
        using namespace DataExchange::XML::DOM;
        Document::Ptr doc = Document::New (body.As<Streams::InputStream::Ptr<std::byte>> ()); // kept: its elements do not keep it
        if (Element::Ptr propertySet = doc.GetRootElement (); propertySet != nullptr) {
            for (const Element::Ptr& property : propertySet.GetChildElements ()) {
                for (const Element::Ptr& variable : property.GetChildElements ()) {
                    result.Add (variable.GetName ().fName, variable.GetValue ());
                }
            }
        }
#endif
        return result;
    }
}

/*
 ********************************************************************************
 ************************* UPnP::GENA::Subscriber::Rep_ *************************
 ********************************************************************************
 */
class Subscriber::Rep_ {
public:
    Rep_ (const URI& eventSubURL, const URI& callbackURL, const function<void (const Event&)>& onEvent, const Options& options)
        : fEventSubURL_{eventSubURL}
        , fCallbackURL_{callbackURL}
        , fOnEvent_{onEvent}
        , fOptions_{options}
    {
    }
    ~Rep_ ()
    {
        Stop ();
    }

public:
    void Start ()
    {
#if not qStroika_Foundation_DataExchange_XML_SupportDOM
        Throw (Exception<runtime_error>{"GENA events cannot be read: this build has no XML parser"sv});
#endif
        lock_guard l{fLifecycleMutex_};
        if (fActive_) {
            return;
        }
        fActive_ = true; // before the SUBSCRIBE: its first event can come before its answer
        try {
            auto [sid, timeout] = Subscribe_ (nullopt);
            fSID_.store (sid);
        }
        catch (...) {
            fActive_ = false;
            throw;
        }
        {
            lock_guard r{fRenewalMutex_};
            fStopped_ = false;
        }
        RenewIn_ (fGranted_ / 2);
    }
    void Stop ()
    {
        lock_guard l{fLifecycleMutex_};
        if (not fActive_) {
            return;
        }
        fActive_ = false; // no more events reported
        optional<IntervalTimer::TimerID> renewal;
        {
            lock_guard r{fRenewalMutex_};
            fStopped_ = true; // so a renewal under way schedules no other
            renewal   = exchange (fRenewal_, nullopt);
        }
        if (renewal) {
            IntervalTimer::Manager::sThe.Remove (*renewal); // waits for it, if under way
        }
        if (optional<String> sid = fSID_.load ()) {
            try {
                Send_ ("UNSUBSCRIBE"sv, Mapping<String, String>{{"SID"sv, *sid}});
            }
            catch (...) {
                DbgTrace ("GENA UNSUBSCRIBE {} failed (it lapses at its TIMEOUT): {}"_f, *sid, current_exception ());
            }
        }
        fSID_.store (nullopt);
    }

public:
    // a SUBSCRIBE or UNSUBSCRIBE at the event URL
    Transfer::Response Send_ (const String& method, const Mapping<String, String>& headers) const
    {
        Transfer::Connection::Ptr c = Transfer::Connection::New ();
        c.SetSchemeAndAuthority (fEventSubURL_.GetSchemeAndAuthority ());
        Transfer::Request r;
        r.fMethod               = method;
        r.fAuthorityRelativeURL = fEventSubURL_.GetAuthorityRelativeResource<URI> ();
        r.fOverrideHeaders      = headers;
        return c.SendAndThrowOnFailure (r);
    }
    // a SUBSCRIBE - new, or the renewal of sid - and what it granted: the SID, and TIMEOUT (also kept in fGranted_)
    pair<String, Time::DurationSeconds> Subscribe_ (const optional<String>& sid)
    {
        const String       timeout = "Second-{}"_f(std::llround (fOptions_.fTimeout.count ()));
        Transfer::Response r =
            Send_ ("SUBSCRIBE"sv,
                   sid ? Mapping<String, String>{{"SID"sv, *sid}, {"TIMEOUT"sv, timeout}}
                       : Mapping<String, String>{{"CALLBACK"sv, "<{}>"_f(fCallbackURL_)}, {"NT"sv, "upnp:event"sv}, {"TIMEOUT"sv, timeout}});
        optional<String> newSID = Header_ (r, "SID"sv);
        if (not newSID) {
            Throw (Exception<runtime_error>{"GENA SUBSCRIBE answered with no SID"sv});
        }
        fGranted_ = Timeout_ (Header_ (r, "TIMEOUT"sv)).value_or (fOptions_.fTimeout);
        return {*newSID, fGranted_};
    }
    // renew in this long - unless stopping
    void RenewIn_ (Time::DurationSeconds after)
    {
        lock_guard l{fRenewalMutex_};
        if (not fStopped_) {
            fRenewal_ = IntervalTimer::Manager::sThe.AddOneShot ([this] () { Renew_ (); }, after);
        }
    }
    // on IntervalTimer's thread: renew - or, failing, subscribe again; failing that too, try again a little later
    void Renew_ ()
    {
        try {
            auto [sid, timeout] = Subscribe_ (fSID_.load ());
            fSID_.store (sid);
            RenewIn_ (timeout / 2);
            return;
        }
        catch (...) {
            DbgTrace ("GENA renewal of {} failed: {} - subscribing again"_f, fSID_.load (), current_exception ());
        }
        try {
            fSID_.store (nullopt); // till the answer: its first event may come before it
            auto [sid, timeout] = Subscribe_ (nullopt);
            fSID_.store (sid);
            RenewIn_ (timeout / 2);
        }
        catch (...) {
            DbgTrace ("GENA SUBSCRIBE to {} failed: {} - trying again later"_f, fEventSubURL_, current_exception ());
            RenewIn_ (kRetryAfter_);
        }
    }

public:
    const URI                           fEventSubURL_;
    const URI                           fCallbackURL_;
    const function<void (const Event&)> fOnEvent_;
    const Options                       fOptions_;
    mutex                               fLifecycleMutex_; // Start and Stop
    atomic<bool>                        fActive_{false};  // started, and not stopped: events are reported
    Synchronized<optional<String>>      fSID_;            // nullopt while subscribing, or not subscribed
    Time::DurationSeconds               fGranted_{0.0};   // the TIMEOUT last granted - only on the thread subscribing
    mutex                               fRenewalMutex_;
    bool                                fStopped_{true}; // (fRenewalMutex_) so no renewal is scheduled
    optional<IntervalTimer::TimerID>    fRenewal_;       // (fRenewalMutex_) the one-shot timer to renew at
};

/*
 ********************************************************************************
 *************************** UPnP::GENA::Subscriber *****************************
 ********************************************************************************
 */
Subscriber::Subscriber (const URI& eventSubURL, const URI& callbackURL, const function<void (const Event&)>& onEvent, const Options& options)
    : fRep_{make_unique<Rep_> (eventSubURL, callbackURL, onEvent, options)}
{
}

Subscriber::~Subscriber () = default;

void Subscriber::Start ()
{
    fRep_->Start ();
}

void Subscriber::Stop ()
{
    fRep_->Stop ();
}

optional<String> Subscriber::GetSID () const
{
    return fRep_->fSID_.load ();
}

void Subscriber::HandleNotify (WebServer::Message& m)
{
    const WebServer::Request& request  = m.request ();
    WebServer::Response&      response = m.rwResponse ();
    const optional<String>    nt       = request.headers ().LookupOne ("NT"sv);
    const optional<String>    nts      = request.headers ().LookupOne ("NTS"sv);
    if (not nt or not nts) {
        response.status = HTTP::StatusCodes::kBadRequest;
        return;
    }
    const optional<String> sid     = request.headers ().LookupOne ("SID"sv);
    const optional<String> current = fRep_->fSID_.load ();
    if (not fRep_->fActive_ or nt != "upnp:event"sv or nts != "upnp:propchange"sv or (current and sid != current)) {
        response.status = HTTP::StatusCodes::kPreconditionFailed;
        return;
    }
    Event e{.fSEQ = String2Int<uint32_t> (request.headers ().LookupOne ("SEQ"sv).value_or (String{}))};
    try {
        e.fVariables = PropertySet_ (m.rwRequest ().GetBody ());
    }
    catch (const DataExchange::BadFormatException&) {
        response.status = HTTP::StatusCodes::kBadRequest; // not a property set
        return;
    }
    fRep_->fOnEvent_ (e);
}

URI Subscriber::MakeCallbackURL (const URI& eventSubURL, PortType port, const String& path)
{
    optional<URI::Authority> a = eventSubURL.GetAuthority ();
    Require (a and a->GetHost ());
    Sequence<InternetAddress> addresses;
    if (optional<InternetAddress> ia = a->GetHost ()->AsInternetAddress ()) {
        addresses += *ia;
    }
    else {
        addresses = DNS::kThe.GetHostAddresses (a->GetHost ()->AsRegisteredName ().value_or (String{}));
    }
    exception_ptr lastFailure;
    for (const InternetAddress& ia : addresses) {
        try {
            const SocketAddress                 service{ia, eventSubURL.GetPortValue ()};
            ConnectionOrientedStreamSocket::Ptr s = ConnectionOrientedStreamSocket::New (service.GetAddressFamily (), Socket::STREAM);
            s.Connect (service, 5s);
            if (optional<SocketAddress> local = s.GetLocalAddress ()) {
                return URI{URI::SchemeType{"http"sv}, URI::Authority{URI::Host{local->GetInternetAddress ()}, port}, path};
            }
        }
        catch (...) {
            lastFailure = current_exception ();
        }
    }
    if (lastFailure) {
        rethrow_exception (lastFailure);
    }
    Throw (Exception<runtime_error>{"no address for the GENA service"sv});
}

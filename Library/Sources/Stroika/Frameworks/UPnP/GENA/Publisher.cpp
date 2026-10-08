/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cmath>
#include <limits>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/XML/WriterUtils.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/ThreadPool.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "Publisher.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::GENA;

namespace {
    // a NOTIFY's body: each variable an e:property of an e:propertyset (UPnP Device Architecture 1.1, section 4.3.2)
    string PropertySet_ (const Mapping<String, String>& variables)
    {
        StringBuilder sb;
        sb << "<?xml version=\"1.0\"?>\r\n<e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\">"sv;
        for (const KeyValuePair<String, String>& v : variables) {
            sb << "<e:property><"sv << v.fKey << ">"sv << DataExchange::XML::QuoteForXMLW (v.fValue) << "</"sv << v.fKey << "></e:property>"sv;
        }
        sb << "</e:propertyset>\r\n"sv;
        return sb.str ().AsUTF8<string> ();
    }

    // a CALLBACK header's URLs: each in angle brackets, tried in order (UPnP Device Architecture 1.1, section 4.1.1) - http ones only
    Sequence<URI> CallbackURLs_ (const String& callback)
    {
        Sequence<URI> result;
        for (size_t at = 0; optional<size_t> start = callback.Find ('<', at);) {
            optional<size_t> end = callback.Find ('>', *start);
            if (not end) {
                break;
            }
            try {
                URI u = URI::Parse (callback.SubString (*start + 1, *end));
                if (u.GetScheme () == URI::SchemeType{"http"sv} and u.GetAuthority ()) {
                    result += u;
                }
            }
            catch (...) {
                // not a URL: passed by
            }
            at = *end + 1;
        }
        return result;
    }

    // a TIMEOUT header's duration - Second-, then a number of seconds (UPnP Device Architecture 1.1, section 4.1.1) - or nullopt:
    // none, Second-infinite (which UPnP 1.1 no longer allows), or not understood
    optional<Time::DurationSeconds> Timeout_ (const optional<String>& timeout)
    {
        if (timeout and timeout->StartsWith ("Second-"sv, eCaseInsensitive)) {
            if (int64_t seconds = String2Int<int64_t> (timeout->SubString (7)); seconds > 0) { // 0: not a number
                return Time::DurationSeconds{static_cast<double> (seconds)};
            }
        }
        return nullopt;
    }

    String TimeoutHeader_ (Time::DurationSeconds timeout)
    {
        return "Second-{}"_f(std::llround (timeout.count ()));
    }
}

/*
 ********************************************************************************
 ************************* UPnP::GENA::Publisher::Rep_ **************************
 ********************************************************************************
 */
class Publisher::Rep_ {
public:
    Rep_ (const function<Mapping<String, String> ()>& currentState, const Options& options)
        : fCurrentState_{currentState}
        , fOptions_{options}
    {
    }

public:
    struct Subscription_ {
        Sequence<URI>          fCallbacks;
        Time::TimePointSeconds fExpiresAt;
        uint32_t               fNextSEQ{0};
        bool                   fPending{true}; // till its SUBSCRIBE's answer is sent: Notify passes it by, as its first event is to come
    };

public:
    // holding fSubscriptions_: drop what has expired
    static void DropExpired_ (Mapping<String, Subscription_>* subscriptions)
    {
        Time::TimePointSeconds now = Time::GetTickCount ();
        subscriptions->RemoveAll ([&] (const KeyValuePair<String, Subscription_>& s) { return s.fValue.fExpiresAt <= now; });
    }
    // the TIMEOUT a subscription gets for this SUBSCRIBE's
    Time::DurationSeconds Grant_ (const optional<String>& asked) const
    {
        optional<Time::DurationSeconds> t = Timeout_ (asked);
        return t and *t < fOptions_.fTimeout ? *t : fOptions_.fTimeout;
    }
    // holding fSubscriptions_: queue an event for this subscription, its SEQ taken now - so the events go in SEQ order. variables
    // nullopt: its first event, all of them, as they are when it is sent
    void Send_ (const String& sid, Subscription_* s, const optional<Mapping<String, String>>& variables)
    {
        uint32_t seq = s->fNextSEQ;
        s->fNextSEQ  = seq == numeric_limits<uint32_t>::max () ? 1 : seq + 1; // 0 only ever the first (section 4.3.2)
        fSender_.AddTask ([sid, callbacks = s->fCallbacks, seq, variables, currentState = fCurrentState_] () {
            Deliver_ (sid, callbacks, seq, PropertySet_ (variables ? *variables : currentState ()));
        });
    }
    // on fSender_'s thread: a NOTIFY to the first of the callback URLs that takes it
    static void Deliver_ (const String& sid, const Sequence<URI>& callbacks, uint32_t seq, const string& body)
    {
        for (const URI& u : callbacks) {
            try {
                Transfer::Connection::Ptr c = Transfer::Connection::New ();
                c.SetSchemeAndAuthority (u.GetSchemeAndAuthority ());
                Transfer::Request r;
                r.fMethod               = "NOTIFY"sv;
                r.fAuthorityRelativeURL = u.GetAuthorityRelativeResource<URI> ();
                r.fOverrideHeaders      = Mapping<String, String>{{"Content-Type"sv, "text/xml; charset=\"utf-8\""sv},
                                                                  {"NT"sv, "upnp:event"sv},
                                                                  {"NTS"sv, "upnp:propchange"sv},
                                                                  {"SID"sv, sid},
                                                                  {"SEQ"sv, "{}"_f(seq)}};
                r.fData                 = Memory::BLOB{as_bytes (span{body})};
                if (c.Send (r).GetSucceeded ()) {
                    return;
                }
            }
            catch (...) {
                DbgTrace ("GENA NOTIFY to {} failed: {}"_f, u, current_exception ());
            }
        }
        DbgTrace ("GENA NOTIFY {} (SEQ {}) went to none of its callbacks"_f, sid, seq);
    }

public:
    const function<Mapping<String, String> ()>   fCurrentState_;
    const Options                                fOptions_;
    Synchronized<Mapping<String, Subscription_>> fSubscriptions_; // by SID
    // last, so destroyed first: its tasks use nothing else of this
    ThreadPool fSender_{ThreadPool::Options{.fThreadCount = 1}};
};

/*
 ********************************************************************************
 **************************** UPnP::GENA::Publisher *****************************
 ********************************************************************************
 */
Publisher::Publisher (const function<Mapping<String, String> ()>& currentState, const Options& options)
    : fRep_{make_unique<Rep_> (currentState, options)}
{
}

Publisher::~Publisher () = default;

void Publisher::HandleRequest (WebServer::Message& m)
{
    using HTTP::StatusCodes::kBadRequest;
    using HTTP::StatusCodes::kPreconditionFailed;
    const WebServer::Request& request  = m.request ();
    WebServer::Response&      response = m.rwResponse ();
    const optional<String>    sid      = request.headers ().LookupOne ("SID"sv);
    const optional<String>    nt       = request.headers ().LookupOne ("NT"sv);
    const optional<String>    callback = request.headers ().LookupOne ("CALLBACK"sv);
    auto                      answer   = [&] (const String& subscriptionID, Time::DurationSeconds timeout) {
        response.rwHeaders ().Set ("SID"sv, subscriptionID);
        response.rwHeaders ().Set ("TIMEOUT"sv, TimeoutHeader_ (timeout));
    };
    const String method = request.httpMethod ();
    if (sid and (nt or callback)) {
        response.status = kBadRequest; // a SID says which subscription; NT and CALLBACK make a new one
        return;
    }
    if (method == "SUBSCRIBE"sv and not sid) {
        Sequence<URI> callbacks = callback ? CallbackURLs_ (*callback) : Sequence<URI>{};
        if (nt != "upnp:event"sv or callbacks.empty ()) {
            response.status = kPreconditionFailed;
            return;
        }
        const String                newSID  = "uuid:" + Common::GUID::GenerateNew ().As<String> ();
        const Time::DurationSeconds timeout = fRep_->Grant_ (request.headers ().LookupOne ("TIMEOUT"sv));
        {
            auto subscriptions = fRep_->fSubscriptions_.rwget ();
            Rep_::DropExpired_ (&subscriptions.rwref ());
            subscriptions->Add (newSID, Rep_::Subscription_{.fCallbacks = callbacks, .fExpiresAt = Time::GetTickCount () + timeout});
        }
        answer (newSID, timeout);
        // its first event, once this answer - with the SID it needs to know it by - is sent
        bool answered      = response.End ();
        auto subscriptions = fRep_->fSubscriptions_.rwget ();
        if (optional<Rep_::Subscription_> s = subscriptions->Lookup (newSID)) {
            if (answered) {
                s->fPending = false;
                fRep_->Send_ (newSID, &*s, nullopt);
                subscriptions->Add (newSID, *s);
            }
            else {
                subscriptions->Remove (newSID); // the subscriber never heard of it
            }
        }
    }
    else if (method == "SUBSCRIBE"sv) {
        auto subscriptions = fRep_->fSubscriptions_.rwget ();
        Rep_::DropExpired_ (&subscriptions.rwref ());
        optional<Rep_::Subscription_> s = subscriptions->Lookup (*sid);
        if (not s) {
            response.status = kPreconditionFailed;
            return;
        }
        const Time::DurationSeconds timeout = fRep_->Grant_ (request.headers ().LookupOne ("TIMEOUT"sv));
        s->fExpiresAt                       = Time::GetTickCount () + timeout;
        subscriptions->Add (*sid, *s);
        answer (*sid, timeout);
    }
    else if (method == "UNSUBSCRIBE"sv) {
        auto subscriptions = fRep_->fSubscriptions_.rwget ();
        Rep_::DropExpired_ (&subscriptions.rwref ());
        if (not sid or not subscriptions->RemoveIf (*sid)) {
            response.status = kPreconditionFailed;
        }
    }
    else {
        response.status = HTTP::StatusCodes::kMethodNotAllowed;
    }
}

void Publisher::Notify (const Mapping<String, String>& changed)
{
    auto subscriptions = fRep_->fSubscriptions_.rwget ();
    Rep_::DropExpired_ (&subscriptions.rwref ());
    for (const KeyValuePair<String, Rep_::Subscription_>& i : Mapping<String, Rep_::Subscription_>{subscriptions.rwref ()}) { // a copy: changed as it goes
        if (not i.fValue.fPending) {
            Rep_::Subscription_ s = i.fValue;
            fRep_->Send_ (i.fKey, &s, changed);
            subscriptions->Add (i.fKey, s);
        }
    }
}

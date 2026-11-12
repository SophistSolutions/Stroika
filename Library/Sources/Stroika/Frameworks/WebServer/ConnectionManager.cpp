/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/IO/Network/HTTP/Exception.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "Stroika/Frameworks/WebServer/DefaultFaultInterceptor.h"

#include "ConnectionManager.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Math;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Time;
using namespace Stroika::Foundation::Traversal;

using Memory::MakeSharedPtr;

using namespace Stroika::Frameworks;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks::WebServer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

// Comment this in to turn on aggressive noisy DbgTrace in this module
// As name suggests, DANGEROUS because called from threads BESIDES the threadpool ones, and so can easily cause assert failures cuz Connection
// objects CHECK they are not used un-externally-synrchonized!
// issue is the DbgTrace functions DEREFERENCE the shared_ptrs (in print function) and they do so to OTHER connections
// than their own (GetActiveConnections/GetInactiveConnections).
// #define USE_NOISY_TRACE_IN_THIS_MODULE_DANGEROUS_ASSERT_FAILURY_ 1

using Options = ConnectionManager::Options;

namespace {
    Sequence<Interceptor> mkEarlyInterceptors_ (const optional<Interceptor>& defaultFaultHandler)
    {
        Sequence<Interceptor> interceptors;
        if (defaultFaultHandler) {
            interceptors += *defaultFaultHandler;
        }
        return interceptors;
    }
    InterceptorChain mkInterceptorChain_ (const Router& router, const Sequence<Interceptor>& earlyInterceptors,
                                          const Sequence<Interceptor>& beforeInterceptors, const Sequence<Interceptor>& afterInterceptors)
    {
        Sequence<Interceptor> interceptors;
        interceptors += earlyInterceptors;
        interceptors += beforeInterceptors;
        interceptors += router;
        interceptors += afterInterceptors;
        return InterceptorChain{interceptors};
    }
}

namespace {
    using TypeOfMonitor = WaitForIOReady_Support::WaitForIOReady_Base::TypeOfMonitor;
    const Set<TypeOfMonitor> kInactiveSocketMonitorEvents2Watch4_{TypeOfMonitor::eRead};

    /*
     *  Answer a request 503 (Service Unavailable) and close, waiting for nothing - so a thread that serves every connection can
     *  call it. Read what of the request has arrived first: closing a socket with unread data sends a TCP reset, which the client
     *  may see instead of the 503.
     */
    void AnswerServiceUnavailableAndClose_ (const ConnectionOrientedStreamSocket::Ptr& s) noexcept
    {
        // the whole response (Retry-After in seconds)
        static const string kResponse_ = [] () {
            using namespace IO::Network::HTTP;
            return "HTTP/1.1 "s + to_string (StatusCodes::kServiceUnavailable) + " Service Unavailable\r\n"s + string{HeaderName::kConnection} +
                   ": close\r\n"s + string{HeaderName::kContentLength} + ": 0\r\n"s + string{HeaderName::kRetryAfter} + ": 1\r\n\r\n"s;
        }();
        try {
            s.SetAutomaticTCPDisconnectOnClose (nullopt); // that graceful close waits for the client
            Stroika_ATTRIBUTE_INDETERMINATE byte buf[4 * 1024];
            for (size_t nRead = 0; nRead < 64 * 1024;) { // bounded: a client still sending cannot hold the caller
                optional<span<byte>> r = s.ReadNonBlocking (span{buf});
                if (not r or r->empty ()) {
                    break;
                }
                nRead += r->size ();
            }
            s.Write (as_bytes (span{kResponse_}));
            s.Close ();
        }
        catch (...) {
            DbgTrace ("Could not answer {} 503 - the client is likely gone ({})"_f, s, current_exception ());
        }
    }

    // Close an idle connection (one no handler has) at once - the graceful close waits for the client, and an idle one may never answer
    void CloseIdleConnection_ (const shared_ptr<WebServer::Connection>& c) noexcept
    {
        try {
            c->socket ().SetAutomaticTCPDisconnectOnClose (nullopt);
            c->socket ().Close ();
        }
        catch (...) {
            DbgTrace ("Closing {} failed - ignored ({})"_f, c, current_exception ());
        }
    }
}

/*
 ********************************************************************************
 ************ WebServer::ConnectionManager::Statistics::ThreadPool **************
 ********************************************************************************
 */
String WebServer::ConnectionManager::Statistics::ThreadPool::ToString () const
{
    StringBuilder sb = this->ThreadPool::Statistics::ToString ().SubString (0, -1);
    sb << ", threadEntryCount: "sv << fThreadEntryCount;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ******* WebServer::ConnectionManager::Statistics::ConnectionStatistics *********
 ********************************************************************************
 */
String WebServer::ConnectionManager::Statistics::ConnectionStatistics::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "nOpenConnections: "sv << fNumberOfOpenConnections;
    sb << ", nActiveConnections: "sv << fNumberOfActiveConnections;
    sb << ", durationOpenConnections: "sv << fDurationOfOpenConnections;
    sb << ", durationActiveRequests: "sv << fDurationOfOpenConnectionsRequests;
    sb << ", durationOpenRequests: "sv << fDurationOfActiveConnectionsRequests;
    sb << ", nConnectionsPiningForTheFjords: "sv << fConnectionsPiningForTheFjords;
    sb << ", nRequestsRejected: "sv << fRequestsRejected;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 **************** WebServer::ConnectionManager::Statistics **********************
 ********************************************************************************
 */
Characters::String WebServer::ConnectionManager::Statistics::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "threadPool: "sv << fThreadPool;
    sb << ", connections: "sv << fConnections;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************ WebServer::ConnectionManager **************************
 ********************************************************************************
 */
ConnectionManager::ConnectionManager (const SocketAddress& bindAddress, const Sequence<Route>& routes, const Options& options)
    : ConnectionManager{Sequence<SocketAddress>{bindAddress}, routes, options}
{
}

namespace {
    inline unsigned int ComputeThreadPoolSize_ (const ConnectionManager::Options& options)
    {
        constexpr unsigned int kMinDefaultThreadCnt_{3u}; // @see Options::fMaxConcurrentlyHandledConnections
        return Math::AtLeast (1u, options.fMaxConcurrentlyHandledConnections.value_or (
                                      Math::AtLeast (kMinDefaultThreadCnt_, options.fMaxConnections / 12))); // one, even asked for none
    }
    inline unsigned int ComputeConnectionBacklog_ (const ConnectionManager::Options& options)
    {
        constexpr unsigned int kMinDefaultTCPBacklog_{3u};
        return options.fTCPBacklog.value_or (Math::AtLeast (kMinDefaultTCPBacklog_, options.fMaxConnections / 4));
    }
    ConnectionManager::Options FillInDefaults_ (const ConnectionManager::Options& o)
    {
        using Options = ConnectionManager::Options;
        Options result{o};
        result.fMaxConcurrentlyHandledConnections = NullCoalesce (result.fMaxConcurrentlyHandledConnections, ComputeThreadPoolSize_ (result));
        result.fBindFlags               = NullCoalesce (result.fBindFlags, Options::kDefault_BindFlags);
        result.fDefaultResponseHeaders  = NullCoalesce (result.fDefaultResponseHeaders, Options::kDefault_Headers);
        result.fAutoComputeETagResponse = NullCoalesce (result.fAutoComputeETagResponse, Options::kDefault_AutoComputeETagResponse);
        result.fAutomaticTCPDisconnectOnClose = NullCoalesce (result.fAutomaticTCPDisconnectOnClose, Options::kDefault_AutomaticTCPDisconnectOnClose);
        result.fLinger     = NullCoalesce (result.fLinger, Options::kDefault_Linger); // for now this is special and can be null/optional
        result.fTCPNoDelay = NullCoalesce (result.fTCPNoDelay, Options::kDefault_TCPNoDelay);
        // result.fThreadPoolName; can remain nullopt
        result.fTCPBacklog = NullCoalesce (result.fTCPBacklog, ComputeConnectionBacklog_ (result));

        {
            // Not super clear. BUT - it appears that if you combine CORS with Caching, then the value returned from
            // the cache maybe WRONG due to not having a (correct) Origin header. Seems most logical that the CORS
            // logic would not check the origin if its coming from the cache, but ... who knows...
            // According to https://blog.keul.it/chrome-cors-issue-due-to-cache/ Chrome does, and this fits with the
            // sporadic bug I'm seeing (hard to tell cuz what seems to happen is the web server not called, but Chrome debugger reports
            // CORS error and Chrome debugger not super clear here - not letting me see what it pulled from cache or even
            // mentioning - so I'm guessing - that it came from the cache.
            // Anyhow, piecing things together, it appears that IF you are using CORS, you must set the vary header
            // to vary on origin.
            //
            // Note - we emit it as a default for all responses because https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Vary says:
            //      "The same Vary header value should be used on all responses for a given URL,
            //      including 304 Not Modified responses and the "default" response."
            //
            //      -- LGP 2022-12-09
            HTTP::Headers s = NullCoalesce (result.fDefaultResponseHeaders);
            Set<String>   v = NullCoalesce (s.vary ());
            v += HTTP::HeaderName::kOrigin;
            s.vary                         = v;
            result.fDefaultResponseHeaders = s;
        }
        return result;
    }
}

ConnectionManager::ConnectionManager (const Iterable<SocketAddress>& bindAddresses, const Sequence<Route>& routes, const Options& options)
    : afterInterceptors{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Sequence<Interceptor> {
              const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::afterInterceptors);
              return thisObj->fAfterInterceptors_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& afterInterceptors_) {
              ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::afterInterceptors);
              thisObj->fAfterInterceptors_ = afterInterceptors_;
              thisObj->DeriveConnectionDefaultOptionsFromEffectiveOptions_ ();
          }}
    , beforeInterceptors{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Sequence<Interceptor> {
              const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::beforeInterceptors);
              return thisObj->fBeforeInterceptors_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& beforeInterceptors) {
              ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::beforeInterceptors);
              thisObj->fBeforeInterceptors_ = beforeInterceptors;
              thisObj->DeriveConnectionDefaultOptionsFromEffectiveOptions_ ();
          }}
    , bindings{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Iterable<SocketAddress> {
        const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::bindings);
        return thisObj->fBindings_;
    }}
    , connections{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> ConnectionStatsCollection {
        const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::connections);
        scoped_lock critSec{thisObj->fActiveConnections_}; // fActiveConnections_ lock used for inactive connections too (only for exchanges between the two lists)
        Ensure (Set<shared_ptr<Connection>>{thisObj->fActiveConnections_.load ()}.Intersection (thisObj->GetInactiveConnections_ ()).empty ());
        ConnectionStatsCollection r;
        for (auto i : thisObj->fActiveConnections_.load ()) {
            auto s    = i->stats ();
            s.fActive = true;
            r += s;
        }
        for (auto i : thisObj->GetInactiveConnections_ ()) {
            auto s    = i->stats ();
            s.fActive = false;
            r += s;
        }
        return r;
    }}
    , defaultErrorHandler{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<Interceptor> {
              const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::defaultErrorHandler);
              return thisObj->fDefaultErrorHandler_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& defaultErrorHandler) {
              ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::defaultErrorHandler);
              if (thisObj->fDefaultErrorHandler_ != defaultErrorHandler) {
                  thisObj->ReplaceInEarlyInterceptor_ (thisObj->fDefaultErrorHandler_.load (), defaultErrorHandler);
                  thisObj->fDefaultErrorHandler_ = defaultErrorHandler;
              }
          }}
    , earlyInterceptors{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Sequence<Interceptor> {
              const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::earlyInterceptors);
              return thisObj->fEarlyInterceptors_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& earlyInterceptors) {
              ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::earlyInterceptors);
              thisObj->fEarlyInterceptors_ = earlyInterceptors;
              thisObj->DeriveConnectionDefaultOptionsFromEffectiveOptions_ ();
          }}
    , options{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Options& {
        const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::options);
        return thisObj->fEffectiveOptions_;
    }}
    , statistics{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Statistics {
        const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::statistics);
        // NOTE - this computation can be expensive, so consider caching so only recomputed at most every 30 seconds or so...???
        // would need to be controlled via OPTIONS!
        return thisObj->ComputeStatistics_ ();
    }}
    , fEffectiveOptions_{FillInDefaults_ (options)}
    , fBindings_{bindAddresses}
    , fDefaultErrorHandler_{DefaultFaultInterceptor{}}
    , fEarlyInterceptors_{mkEarlyInterceptors_ (fDefaultErrorHandler_)}
    , fBeforeInterceptors_{}
    , fAfterInterceptors_{}
    , fRouter_{routes, fEffectiveOptions_.fCORS}
    // note since Stroika v3.0d5, we set fQMax = so we don't get lots of useless requests that fill the Q. Probably shouldn't happen
    // anyhow, since we have set 'backlog' - but in case, better failure mode ; arguably could be zero length, but for latency of threads waking up to pickup work;
    // --LGP 2023-11-27
    , fActiveConnectionThreads_{ThreadPool::Options{.fThreadCount    = *fEffectiveOptions_.fMaxConcurrentlyHandledConnections,
                                                    .fThreadPoolName = fEffectiveOptions_.fThreadPoolName,
                                                    .fQMax = ThreadPool::QMax{*fEffectiveOptions_.fMaxConcurrentlyHandledConnections},
                                                    .fCollectStatistics = fEffectiveOptions_.fCollectStatistics}}
    , fWaitForReadyConnectionThread_{Thread::CleanupPtr::eAbortBeforeWaiting,
                                     Thread::New ([this] () { WaitForReadyConnectionLoop_ (); }, "WebServer-ConnectionMgr-Wait4IOReady"_k)}
    , fListener_{bindAddresses, *fEffectiveOptions_.fBindFlags, [this] (const ConnectionOrientedStreamSocket::Ptr& s) { onConnect_ (s); },
                 *fEffectiveOptions_.fTCPBacklog}
{
    // validate fDefaultResponseHeaders contains no bad/inappropriate headers (like Content-Length), probably CORS headers worth a warning as well
    // just a bunch of sanity checks for things you really DONT want to set here for any reason I can think of
    if (fEffectiveOptions_.fDefaultResponseHeaders) {
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->allow () == nullopt); // unsure
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->cookie ().cookieDetails ().empty ());
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->connection () == nullopt);
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->contentLength () == nullopt);
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->ETag () == nullopt);
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->location () == nullopt);
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->origin () == nullopt);      // request only header
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->ifNoneMatch () == nullopt); // request only header
        WeakAssert (fEffectiveOptions_.fDefaultResponseHeaders->setCookie ().cookieDetails ().empty ());
    }
    Require (options.fIdleConnectionTimeout > 0s);
    Require (options.fRequestHeaderTimeout > 0s);
    Require (options.fClientStallTimeout > 0s);
    DeriveConnectionDefaultOptionsFromEffectiveOptions_ ();

    DbgTrace ("Constructing WebServer::ConnectionManager ({}), with threadpoolSize={}, backlog={}, and listening on {}"_f,
              static_cast<const void*> (this), fActiveConnectionThreads_.GetPoolSize (), ComputeConnectionBacklog_ (options),
              Characters::ToString (bindAddresses));
    fWaitForReadyConnectionThread_.Start (); // start here instead of AutoStart so a guaranteed initialized before thread main starts - see https://github.com/SophistSolutions/Stroika/issues/840 (STK-706)
}

#if qStroika_Foundation_Debug_DefaultTracingOn
ConnectionManager::~ConnectionManager ()
{
    DbgTrace ("Starting destructor for WebServer::ConnectionManager ({})"_f, static_cast<const void*> (this));
}
#endif

void ConnectionManager::DeriveConnectionDefaultOptionsFromEffectiveOptions_ ()
{
#if qStroika_Foundation_Debug_DefaultTracingOn
    auto prev = fUseDefaultConnectionOptions_.fInterceptorChain;
#endif
    fUseDefaultConnectionOptions_ = Connection::Options{
        .fInterceptorChain              = mkInterceptorChain_ (fRouter_, fEarlyInterceptors_, fBeforeInterceptors_, fAfterInterceptors_),
        .fDefaultResponseHeaders        = *fEffectiveOptions_.fDefaultResponseHeaders,
        .fDefaultGETResponseHeaders     = fEffectiveOptions_.fDefaultGETResponseHeaders,
        .fAutoComputeETagResponse       = *fEffectiveOptions_.fAutoComputeETagResponse,
        .fAutomaticTransferChunkSize    = fEffectiveOptions_.fAutomaticTransferChunkSize,
        .fSupportedCompressionEncodings = fEffectiveOptions_.fSupportedCompressionEncodings,
    };
    // @todo could add trace messages on other values changing...
#if qStroika_Foundation_Debug_DefaultTracingOn
    if (prev != fUseDefaultConnectionOptions_.fInterceptorChain) {
        DbgTrace ("Updated InterceptorChain: {}"_f, fUseDefaultConnectionOptions_.fInterceptorChain);
    }
#endif
}

void ConnectionManager::onConnect_ (const ConnectionOrientedStreamSocket::Ptr& s)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"ConnectionManager::onConnect_", "s={}"_f, s};
#endif
    s.SetTCPNoDelay (*fEffectiveOptions_.fTCPNoDelay);
    s.SetAutomaticTCPDisconnectOnClose (*fEffectiveOptions_.fAutomaticTCPDisconnectOnClose);
    s.SetLinger (fEffectiveOptions_.fLinger); // 'missing' has meaning (feature disabled) for socket, so allow setting that too - doesn't mean don't pass on/use-default

    // bound each blocking read and write - a request's body, its response (@see Options::fClientStallTimeout); a request's
    // header is read only once it has come (@see CloseIdleConnections_ for its deadline)
    if (Time::DurationSeconds stall = fEffectiveOptions_.fClientStallTimeout; stall != Time::kInfinity) {
        s.SetReceiveTimeout (stall);
        s.SetSendTimeout (stall);
    }

    /*
     *  At fMaxConnections, make room by closing the connection idle longest - or, every one handling a request, answer this one
     *  503. (Only this thread - the listener's - adds connections, so the count cannot grow between this check and the Add.)
     */
    shared_ptr<Connection> longestIdle;
    bool                   atLimit = false;
    {
        scoped_lock                        critSec{fActiveConnections_}; // as every move between the active and inactive lists
        Collection<shared_ptr<Connection>> inactive = GetInactiveConnections_ ();
        if (fActiveConnections_.cget ()->size () + inactive.size () >= fEffectiveOptions_.fMaxConnections) {
            atLimit = true;
            for (const shared_ptr<Connection>& c : inactive) {
                // not one whose next request (or close) has come: the poller hands that to a handler
                if (c->socket ().AvailableToRead () == nullopt and (longestIdle == nullptr or c->GetIdleSince () < longestIdle->GetIdleSince ())) {
                    longestIdle = c;
                }
            }
            if (longestIdle != nullptr) {
                // traced here: once in fToClose_, the wait thread may be closing it
                DbgTrace ("Closing {}, the one idle longest, to make room for a new connection (fMaxConnections={})"_f, longestIdle,
                          fEffectiveOptions_.fMaxConnections);
                fToClose_.rwget ()->Add (longestIdle);
                fInactiveSockSetPoller_.Remove (longestIdle); // waking the wait thread, which closes it
            }
        }
    }
    if (atLimit and longestIdle == nullptr) {
        ++fRequestsRejected_;
        DbgTrace ("Answering {} 503 (Service Unavailable): fMaxConnections ({}) open, none idle"_f, s, fEffectiveOptions_.fMaxConnections);
        AnswerServiceUnavailableAndClose_ (s);
        return;
    }

    shared_ptr<Connection> conn = MakeSharedPtr<Connection> (s, fUseDefaultConnectionOptions_);
    fInactiveSockSetPoller_.Add (conn, kInactiveSocketMonitorEvents2Watch4_);
#if USE_NOISY_TRACE_IN_THIS_MODULE_DANGEROUS_ASSERT_FAILURY_
    {
        scoped_lock critSec{fActiveConnections_}; // fActiveConnections_ lock used for inactive connections too (only for exchanges between the two lists)
        DbgTrace ("In onConnect_ (after adding connection {}): fActiveConnections_={}, inactiveOpenConnections_={}"_f, conn,
                  fActiveConnections_.load (), GetInactiveConnections_ ());
    }
#endif
}

void ConnectionManager::WaitForReadyConnectionLoop_ ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"ConnectionManager::WaitForReadyConnectionLoop_"};
#endif

    // sweep for idle connections, and late request headers, this often - so each is closed within its timeout and half again
    // (or 15 seconds)
    auto sweepFor = [] (Time::DurationSeconds timeout) {
        return timeout == Time::kInfinity ? Time::kInfinity : min<Time::DurationSeconds> (timeout / 2, 15s);
    };
    const Time::DurationSeconds sweepEvery =
        min (sweepFor (fEffectiveOptions_.fIdleConnectionTimeout), sweepFor (fEffectiveOptions_.fRequestHeaderTimeout));
    Time::TimePointSeconds nextSweepAt = Time::GetTickCount () + sweepEvery;

    // run til thread aborted
    while (true) {
        try {
            Thread::CheckForInterruption ();
            if (Time::GetTickCount () >= nextSweepAt) {
                CloseIdleConnections_ ();
                nextSweepAt = Time::GetTickCount () + sweepEvery;
            }
            for (const shared_ptr<Connection>& c : exchange (fToClose_.rwget ().rwref (), Collection<shared_ptr<Connection>>{})) {
                CloseIdleConnection_ (c); // here, on the thread that polls (@see fToClose_)
            }

#if USE_NOISY_TRACE_IN_THIS_MODULE_DANGEROUS_ASSERT_FAILURY_
            {
                // DANGEROUS - OK to print out POINTERS, but Connection is NOT re-entrant and could be in use in threadpool or other thread.
                // So CAREFUL not to call even CONST methods of those Connection objects here!!!
                scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                DbgTrace ("At top of WaitForReadyConnectionLoop_: fActiveConnections_={}, inactiveOpenConnections_={}"_f,
                          fActiveConnections_.cget ().cref (), GetInactiveConnections_ ());
            }
#endif
            for (shared_ptr<Connection> readyConnection : fInactiveSockSetPoller_.WaitQuietlyUntil (nextSweepAt)) {

                auto handleActivatedConnection = [this, readyConnection] () mutable {
                /*
                 *  This ENTIRE lambda runs in a single threadpool task, and is the only thing that reads/writes
                 *  the readyConnection object, so no locking needed for that object.
                 */
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
                        "ConnectionManager::...handleActivatedConnection", "readyConnection={}"_f, readyConnection)};
#endif

                    /*
                     * Process the request
                     */
                    bool keepAlive = (readyConnection->ReadAndProcessMessage () == Connection::eTryAgainLater);

                    /*
                     * Handle the Connection object, moving it to the appropriate list etc...
                     */
                    try {
                        scoped_lock critSec{fActiveConnections_}; // lock not strictly needed here, but used to assure consistency between the active/inactive lists
                        fActiveConnections_.rwget ().rwref ().Remove (readyConnection); // no matter what, remove from active connections
                        if (keepAlive) {
                            fInactiveSockSetPoller_.Add (readyConnection, kInactiveSocketMonitorEvents2Watch4_);
                        }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                        else {
                            DbgTrace ("Closing connection {}"_f, readyConnection); // cuz it goes out of scope, and is no longer referenced in either list
                        }
#endif
                    }
                    catch (...) {
                        AssertNotReached (); // these two lists need to be kept in sync, so really assume updating them cannot fail/break
                        ReThrow ();
                    }

#if USE_NOISY_TRACE_IN_THIS_MODULE_DANGEROUS_ASSERT_FAILURY_
                    {
                        scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                        DbgTrace ("at end of read&process task (keepAlive={}) for connection {}: fActiveConnections_={}, inactiveOpenConnections_={}"_f,
                                  keepAlive, readyConnection, fActiveConnections_.cget ().cref (), GetInactiveConnections_ ());
                    }
#endif
                };

                try {
                    scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                    // not one another thread took from the poller since the wait returned - onConnect_ closing it, to make room
                    if (not fInactiveSockSetPoller_.GetDescriptors ().Any ([&] (const auto& d) { return d.first == readyConnection; })) {
                        continue;
                    }
                    fInactiveSockSetPoller_.Remove (readyConnection);
                    fActiveConnections_.rwget ().rwref ().Add (readyConnection);
                }
                catch (...) {
                    AssertNotReached (); // these two lists need to be kept in sync, so really assume updating them cannot fail/break
                    ReThrow ();
                }
                try {
                    fActiveConnectionThreads_.AddTask (handleActivatedConnection);
                }
                catch (const Thread::AbortException&) {
                    ReThrow ();
                }
                catch (...) {
                    /*
                     *  Every handler thread busy, and the queue full: the server has fallen behind, so answer 503 at once - not wait
                     *  for room (ready connections would pile up in the poller without limit), nor retry (a ready connection comes
                     *  straight back: a spin).
                     */
                    ++fRequestsRejected_;
                    DbgTrace ("Answering {} 503 (Service Unavailable): every handler thread busy, and the queue full ({})"_f,
                              readyConnection, current_exception ());
                    {
                        scoped_lock critSec{fActiveConnections_};
                        fActiveConnections_.rwget ().rwref ().Remove (readyConnection);
                    }
                    AnswerServiceUnavailableAndClose_ (readyConnection->socket ());
                }
            }
        }
        catch (const Thread::AbortException&) {
            ReThrow ();
        }
        catch (...) {
            DbgTrace ("Internal exception in WaitForReadyConnectionLoop_ loop suppressed: {}"_f, current_exception ());
        }
    }
}

Collection<shared_ptr<Connection>> ConnectionManager::GetInactiveConnections_ () const
{
    return fInactiveSockSetPoller_.GetDescriptors ().Map<Collection<shared_ptr<Connection>>> ([] (const auto& i) { return i.first; });
}

void ConnectionManager::ReplaceInEarlyInterceptor_ (const optional<Interceptor>& oldValue, const optional<Interceptor>& newValue)
{
    // replace old error handler in the interceptor chain, in the same spot if possible, and otherwise append
    auto                  rwLock = this->fEarlyInterceptors_.rwget ();
    Sequence<Interceptor> newInterceptors;
    bool                  addedDefault = false;
    for (const Interceptor& i : rwLock.load ()) {
        if (oldValue == i) {
            if (newValue) {
                newInterceptors += *newValue;
            }
            addedDefault = true;
        }
        else {
            newInterceptors += i;
        }
    }
    if (newValue and not addedDefault) {
        newInterceptors += *newValue;
    }
    rwLock.store (newInterceptors);
    DeriveConnectionDefaultOptionsFromEffectiveOptions_ ();
}

void ConnectionManager::AbortConnection (const shared_ptr<Connection>& /*conn*/)
{
    AssertNotImplemented ();
}

auto ConnectionManager::ComputeStatistics_ () const -> Statistics
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    constexpr bool kExtraDebugging_ = true;
#else
    constexpr bool kExtraDebugging_ = false;
#endif
    ConnectionStatsCollection conns;
    {
        scoped_lock critSec{fActiveConnections_}; // fActiveConnections_ lock used for inactive connections too (only for exchanges between the two lists)
        Assert (Set<shared_ptr<Connection>>{fActiveConnections_.load ()}.Intersection (GetInactiveConnections_ ()).empty ());
        for (auto i : fActiveConnections_.load ()) {
            auto s    = i->stats ();
            s.fActive = true;
            conns += s;
            if constexpr (kExtraDebugging_) {
                DbgTrace ("Active Conn={}"_f, s);
            }
        }
        for (auto i : GetInactiveConnections_ ()) {
            auto s    = i->stats ();
            s.fActive = false;
            conns += s;
            if constexpr (kExtraDebugging_) {
                DbgTrace ("Inactive Conn={}"_f, s);
            }
        }
    }
    Statistics::ThreadPool threadPoolStats = [&] () {
        if (fEffectiveOptions_.fCollectStatistics) {
            return Statistics::ThreadPool{{fActiveConnectionThreads_.GetCurrentStatistics ()}, fActiveConnectionThreads_.GetPoolSize ()};
        }
        else {
            return Statistics::ThreadPool{{}, fActiveConnectionThreads_.GetPoolSize ()};
        }
    }();
    Statistics::ConnectionStatistics connectionStats;
    connectionStats.fNumberOfActiveConnections = conns.Count ([] (const Connection::Stats& s) { return s.fActive == true; });
    connectionStats.fNumberOfOpenConnections   = conns.size ();
    connectionStats.fRequestsRejected          = fRequestsRejected_.load ();

    TimePointSeconds now                       = Time::GetTickCount ();
    connectionStats.fDurationOfOpenConnections = ComputeCommonStatistics (
        conns.Map<Iterable<Duration>> ([&] (const Connection::Stats& cs) -> optional<Duration> { return now - cs.fCreatedAt; }));

#if qStroika_Frameworks_WebServer_Connection_TrackExtraStats
    connectionStats.fDurationOfOpenConnectionsRequests =
        ComputeCommonStatistics (conns.Map<Iterable<Duration>> ([&] (const Connection::Stats& cs) -> optional<Duration> {
            if (cs.fMostRecentMessage) {
                return cs.fMostRecentMessage->ReplaceEnd (min (cs.fMostRecentMessage->GetUpperBound (), now)).GetDistanceSpanned ();
            }
            return nullopt;
        }));
    connectionStats.fDurationOfActiveConnectionsRequests =
        ComputeCommonStatistics (conns.Map<Iterable<Duration>> ([&] (const Connection::Stats& cs) -> optional<Duration> {
            if (cs.fActive == false)
                return nullopt;
            if (cs.fMostRecentMessage) {
                return cs.fMostRecentMessage->ReplaceEnd (min (cs.fMostRecentMessage->GetUpperBound (), now)).GetDistanceSpanned ();
            }
            return nullopt;
        }));
    connectionStats.fConnectionsPiningForTheFjords =
        conns
            .Map<Iterable<Duration>> ([&] (const Connection::Stats& cs) -> optional<Duration> {
                if (cs.fActive == false)
                    return nullopt;
                if (cs.fMostRecentMessage) {
                    return cs.fMostRecentMessage->ReplaceEnd (min (cs.fMostRecentMessage->GetUpperBound (), now)).GetDistanceSpanned ();
                }
                return nullopt;
            })
            .Count ([&] (const Duration& d) { return d > fEffectiveOptions_.fConnectionPiningForTheFjordsDelay; });
#endif
    return Statistics{.fThreadPool = threadPoolStats, .fConnections = connectionStats};
}

void ConnectionManager::CloseIdleConnections_ ()
{
    const Time::TimePointSeconds       now               = Time::GetTickCount ();
    const Time::TimePointSeconds       idleSinceBefore   = now - fEffectiveOptions_.fIdleConnectionTimeout;
    const Time::TimePointSeconds       headerSinceBefore = now - fEffectiveOptions_.fRequestHeaderTimeout;
    Collection<shared_ptr<Connection>> idle;
    Collection<shared_ptr<Connection>> headerLate;
    {
        scoped_lock critSec{fActiveConnections_}; // as every move between the active and inactive lists, so both stay consistent
        for (const shared_ptr<Connection>& c : GetInactiveConnections_ ()) {
            if (optional<TimePointSeconds> headerSince = c->GetIncompleteHeaderSince (); headerSince and *headerSince < headerSinceBefore) {
                fInactiveSockSetPoller_.Remove (c);
                headerLate += c;
            }
            // not one whose next request (or close) has come: the poller hands that to a handler
            else if (c->GetIdleSince () < idleSinceBefore and c->socket ().AvailableToRead () == nullopt) {
                fInactiveSockSetPoller_.Remove (c);
                idle += c;
            }
        }
    }
    for (const shared_ptr<Connection>& c : headerLate) {
        DbgTrace ("Closing {}: its request's header not all here {} after it began"_f, c, fEffectiveOptions_.fRequestHeaderTimeout);
        CloseIdleConnection_ (c);
    }
    for (const shared_ptr<Connection>& c : idle) {
        DbgTrace ("Closing {}: idle longer than {}"_f, c, fEffectiveOptions_.fIdleConnectionTimeout);
        CloseIdleConnection_ (c);
    }
}

void ConnectionManager::AddInterceptor (const Interceptor& i, InterceptorAddRelativeTo relativeTo)
{
    switch (relativeTo) {
        case ePrependsToEarly:
            fEarlyInterceptors_.rwget ()->Prepend (i);
            break;
        case ePrepend:
            fBeforeInterceptors_.rwget ()->Prepend (i);
            break;
        case eAppend:
            fAfterInterceptors_.rwget ()->Append (i);
            break;
        case eAfterBeforeInterceptors:
            fBeforeInterceptors_.rwget ()->Append (i);
            break;
    }
    DeriveConnectionDefaultOptionsFromEffectiveOptions_ ();
}

void ConnectionManager::RemoveInterceptor (const Interceptor& i)
{
    bool found = false;
    {
        auto b4 = fBeforeInterceptors_.rwget ();
        if (optional<size_t> idx = b4->IndexOf (i)) {
            b4->Remove (*idx);
            found = true;
        }
    }
    if (not found) {
        auto after = fAfterInterceptors_.rwget ();
        if (optional<size_t> idx = after->IndexOf (i)) {
            after->Remove (*idx);
            found = true;
        }
    }
    Require (found);
    DeriveConnectionDefaultOptionsFromEffectiveOptions_ ();
}
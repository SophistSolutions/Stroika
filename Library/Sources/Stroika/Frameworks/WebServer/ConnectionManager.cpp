/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/Sleep.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/IO/Network/HTTP/Exception.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Methods.h"
#include "../../Foundation/Memory/Optional.h"

#include "DefaultFaultInterceptor.h"

#include "ConnectionManager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks::WebServer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

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

/*
 ********************************************************************************
 **************** WebServer::ConnectionManager::Statistics **********************
 ********************************************************************************
 */
Characters::String WebServer::ConnectionManager::Statistics::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "ThreadPool-Size: "sv << fThreadPoolSize << ", "sv;
    sb << "ThreadPool-Statistics: "sv << fThreadPoolStatistics << ", "sv;
    sb << "}"sv;
    return sb.str ();
}

/*
 ********************************************************************************
 ************************* WebServer::ConnectionManager *************************
 ********************************************************************************
 */
ConnectionManager::ConnectionManager (const SocketAddress& bindAddress, const Sequence<Route>& routes, const Options& options)
    : ConnectionManager{Sequence<SocketAddress>{bindAddress}, routes, options}
{
}

namespace {
    inline unsigned int ComputeThreadPoolSize_ (const ConnectionManager::Options& options)
    {
        using Options = ConnectionManager::Options;
        constexpr unsigned int kMinThreadCnt_{1u}; // one enough now that we support separate thread doing epoll/select and one read when data avail
        return Math::AtLeast (kMinThreadCnt_, options.fMaxConcurrentlyHandledConnections.value_or (
                                                  options.fMaxConnections.value_or (Options::kDefault_MaxConnections) / 10));
    }
    inline unsigned int ComputeConnectionBacklog_ (const ConnectionManager::Options& options)
    {
        using Options = ConnectionManager::Options;
        constexpr unsigned int kMinDefaultTCPBacklog_{3u};
        return options.fTCPBacklog.value_or (
            Math::AtLeast (kMinDefaultTCPBacklog_, options.fMaxConnections.value_or (Options::kDefault_MaxConnections) * 3 / 4));
    }
    ConnectionManager::Options FillInDefaults_ (const ConnectionManager::Options& o)
    {
        using Options = ConnectionManager::Options;
        Options result{o};
        result.fCORS           = Memory::NullCoalesce (result.fCORS, Options::kDefault_CORS ());
        result.fMaxConnections = Memory::NullCoalesce (result.fMaxConnections, Options::kDefault_MaxConnections);
        result.fMaxConcurrentlyHandledConnections = Memory::NullCoalesce (result.fMaxConcurrentlyHandledConnections, ComputeThreadPoolSize_ (result));
        result.fBindFlags               = Memory::NullCoalesce (result.fBindFlags, Options::kDefault_BindFlags);
        result.fDefaultResponseHeaders  = Memory::NullCoalesce (result.fDefaultResponseHeaders, Options::kDefault_Headers);
        result.fAutoComputeETagResponse = Memory::NullCoalesce (result.fAutoComputeETagResponse, Options::kDefault_AutoComputeETagResponse);
        result.fAutomaticTCPDisconnectOnClose =
            Memory::NullCoalesce (result.fAutomaticTCPDisconnectOnClose, Options::kDefault_AutomaticTCPDisconnectOnClose);
        result.fLinger = Memory::NullCoalesce (result.fLinger, Options::kDefault_Linger); // for now this is special and can be null/optional
        // result.fThreadPoolName; can remain nullopt
        result.fTCPBacklog = Memory::NullCoalesce (result.fTCPBacklog, ComputeConnectionBacklog_ (result));

        {
            // Not super clear. BUT - it appears that if you combine CORS with Caching, then the value returned from
            // the cache maybe WRONG due to not having a (correct) Origin header. Seems most logical that the CORS
            // logic would not check the origin if its coming from the cache, but ... who knows...
            // According to https://blog.keul.it/chrome-cors-issue-due-to-cache/ Chrome does, and this fits with the
            // sporadic bug I'm seeing (hard to tell cuz what seems to happen is the web server not called, but Chrome debugger reports
            // CORS error and Chrome debugger not super clear here - not letting me see what it pulled from cache or even
            // mentioning - so I'm guessing - that it came from the cache.
            // Anyhow, peicing things together, it appears that IF you are using CORS, you must set the vary header
            // to vary on origin.
            //
            // Note - we emit it as a default for all responses because https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Vary says:
            //      "The same Vary header value should be used on all responses for a given URL,
            //      including 304 Not Modified responses and the "default" response."
            //
            //      -- LGP 2022-12-09
            HTTP::Headers s = Memory::NullCoalesce (result.fDefaultResponseHeaders);
            Set<String>   v = Memory::NullCoalesce (s.vary ());
            v += HTTP::HeaderName::kOrigin;
            s.vary                         = v;
            result.fDefaultResponseHeaders = s;
        }
        return result;
    }
}

ConnectionManager::ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Sequence<Route>& routes, const Options& options)
    : options{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Options& {
        const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::options);
        return thisObj->fEffectiveOptions_;
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
              thisObj->FixupInterceptorChain_ ();
          }}
    , beforeInterceptors{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Sequence<Interceptor> {
              const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::beforeInterceptors);
              return thisObj->fBeforeInterceptors_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& beforeInterceptors) {
              ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::beforeInterceptors);
              thisObj->fBeforeInterceptors_ = beforeInterceptors;
              thisObj->FixupInterceptorChain_ ();
          }}
    , afterInterceptors{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Sequence<Interceptor> {
              const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::afterInterceptors);
              return thisObj->fAfterInterceptors_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& afterInterceptors_) {
              ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::afterInterceptors);
              thisObj->fAfterInterceptors_ = afterInterceptors_;
              thisObj->FixupInterceptorChain_ ();
          }}
    , pConnections{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Collection<shared_ptr<Connection>> {
        const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::pConnections);
        scoped_lock critSec{thisObj->fActiveConnections_}; // fActiveConnections_ lock used for inactive connections too (only for exchanges between the two lists)
        Ensure (Set<shared_ptr<Connection>>{thisObj->fActiveConnections_.load ()}.Intersection (thisObj->GetInactiveConnections_ ()).empty ());
        return thisObj->GetInactiveConnections_ () + thisObj->fActiveConnections_.load ();
    }}
    , pActiveConnections{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Collection<shared_ptr<Connection>> {
        const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::pActiveConnections);
        return thisObj->fActiveConnections_.load ();
    }}
    , pStatistics{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Statistics {
        const ConnectionManager* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::pStatistics);
        Require (thisObj->fEffectiveOptions_.fCollectStatistics);
        return Statistics{.fThreadPoolSize       = thisObj->fActiveConnectionThreads_.GetPoolSize (),
                          .fThreadPoolStatistics = thisObj->fActiveConnectionThreads_.GetCurrentStatistics ()};
    }}
    , fEffectiveOptions_{FillInDefaults_ (options)}
    , fDefaultErrorHandler_{DefaultFaultInterceptor{}}
    , fEarlyInterceptors_{mkEarlyInterceptors_ (fDefaultErrorHandler_)}
    , fBeforeInterceptors_{}
    , fAfterInterceptors_{}
    , fRouter_{routes, *fEffectiveOptions_.fCORS}
    , fInterceptorChain_{mkInterceptorChain_ (fRouter_, fEarlyInterceptors_, fBeforeInterceptors_, fAfterInterceptors_)}
    // note since Stroika v3.0d5, we set fQMax = so we don't get lots of useless requests that fill the Q. Probably shouldn't happen
    // anyhow, since we have set 'backlog' - but in case, better failure mode ; arguably could be zero length, but for latency of threads waking up to pickup work;
    // --LGP 2023-11-27
    , fActiveConnectionThreads_{ThreadPool::Options{.fThreadCount    = *fEffectiveOptions_.fMaxConcurrentlyHandledConnections,
                                                    .fThreadPoolName = fEffectiveOptions_.fThreadPoolName,
                                                    .fQMax = ThreadPool::QMax{*fEffectiveOptions_.fMaxConcurrentlyHandledConnections},
                                                    .fCollectStatistics = fEffectiveOptions_.fCollectStatistics}}
    , fWaitForReadyConnectionThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting,
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

    DbgTrace ("Constructing WebServer::ConnectionManager (%p), with threadpoolSize=%d, backlog=%d, and listening on %s"_f,
              static_cast<const void*> (this), fActiveConnectionThreads_.GetPoolSize (), ComputeConnectionBacklog_ (options),
              Characters::ToString (bindAddresses));
    fWaitForReadyConnectionThread_.Start (); // start here instead of AutoStart so a guaranteed initialized before thread main starts - see https://stroika.atlassian.net/browse/STK-706
}

#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
ConnectionManager::~ConnectionManager ()
{
    DbgTrace ("Starting destructor for WebServer::ConnectionManager (%p)"_f, static_cast<const void*> (this));
}
#endif

void ConnectionManager::onConnect_ (const ConnectionOrientedStreamSocket::Ptr& s)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"ConnectionManager::onConnect_", "s={}"_f, s};
#endif
    s.SetAutomaticTCPDisconnectOnClose (*fEffectiveOptions_.fAutomaticTCPDisconnectOnClose);
    s.SetLinger (fEffectiveOptions_.fLinger); // 'missing' has meaning (feature disabled) for socket, so allow setting that too - doesn't mean don't pass on/use-default
    shared_ptr<Connection> conn = make_shared<Connection> (s, fInterceptorChain_, *fEffectiveOptions_.fDefaultResponseHeaders,
                                                           fEffectiveOptions_.fDefaultGETResponseHeaders, *fEffectiveOptions_.fAutoComputeETagResponse);
    fInactiveSockSetPoller_.Add (conn);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
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

    // run til thread aborted
    while (true) {
        try {
            Thread::CheckForInterruption ();

#if USE_NOISY_TRACE_IN_THIS_MODULE_
            {
                scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                DbgTrace (L"At top of WaitForReadyConnectionLoop_: fActiveConnections_=%s, inactiveOpenConnections_=%s",
                          Characters::ToString (fActiveConnections_.cget ().cref ()).c_str (),
                          Characters::ToString (GetInactiveConnections_ ()).c_str ());
            }
#endif
            for (shared_ptr<Connection> readyConnection : fInactiveSockSetPoller_.WaitQuietly ()) {

                auto handleActivatedConnection = [this, readyConnection] () mutable {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    Debug::TraceContextBumper ctx{
                        Stroika_Foundation_Debug_OptionalizeTraceArgs ("ConnectionManager::...processConnectionLoop")};
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
                            fInactiveSockSetPoller_.Add (readyConnection);
                        }
                    }
                    catch (...) {
                        AssertNotReached (); // these two lists need to be kept in sync, so really assume updating them cannot fail/break
                        Execution::ReThrow ();
                    }

// @todo lose this code after a bit of testing that never triggered - LGP 2021-03-02
// I think logic was corner case and backwards (sb if keep alive)
//anyhow should never happen  except for incomplete header in which case we dont want to END
#if 0
                    if (not keepAlive) {
                        Assert (readyConnection->response ().responseCompleted ()); // don't think this test is needed - I think we always assure this in Connection - LGP 2021-03-02
                        if (not readyConnection->response ().responseCompleted ()) {
                            IgnoreExceptionsForCall (readyConnection->rwResponse ().End ());
                        }
                    }
#endif

#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    {
                        scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                        DbgTrace (L"at end of read&process task (keepAlive=%s) for connection %s: fActiveConnections_=%s, "
                                  L"inactiveOpenConnections_=%s",
                                  Characters::ToString (keepAlive).c_str (), Characters::ToString (readyConnection).c_str (),
                                  Characters::ToString (fActiveConnections_.cget ().cref ()).c_str (),
                                  Characters::ToString (GetInactiveConnections_ ()).c_str ());
                    }
#endif
                };

                try {
                    scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                    fInactiveSockSetPoller_.Remove (readyConnection);
                    fActiveConnections_.rwget ().rwref ().Add (readyConnection);
                }
                catch (...) {
                    AssertNotReached (); // these two lists need to be kept in sync, so really assume updating them cannot fail/break
                    Execution::ReThrow ();
                }
                fActiveConnectionThreads_.AddTask (handleActivatedConnection);
            }
        }
        catch (const Thread::AbortException&) {
            Execution::ReThrow ();
        }
        catch (...) {
            DbgTrace ("Internal exception in WaitForReadyConnectionLoop_ loop suppressed: {}"_f, Characters::ToString (current_exception ()));
        }
    }
}

void ConnectionManager::FixupInterceptorChain_ ()
{
    fInterceptorChain_ = InterceptorChain{mkInterceptorChain_ (fRouter_, fEarlyInterceptors_, fBeforeInterceptors_, fAfterInterceptors_)};
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
    FixupInterceptorChain_ ();
}

void ConnectionManager::AbortConnection (const shared_ptr<Connection>& /*conn*/)
{
    AssertNotImplemented ();
}

void ConnectionManager::AddInterceptor (const Interceptor& i, InterceptorAddRelativeTo relativeTo)
{
    switch (relativeTo) {
        case InterceptorAddRelativeTo::ePrependsToEarly:
            fEarlyInterceptors_.rwget ()->Prepend (i);
            break;
        case InterceptorAddRelativeTo::ePrepend:
            fBeforeInterceptors_.rwget ()->Prepend (i);
            break;
        case InterceptorAddRelativeTo::eAppend:
            fAfterInterceptors_.rwget ()->Append (i);
            break;
        case InterceptorAddRelativeTo::eAfterBeforeInterceptors:
            fBeforeInterceptors_.rwget ()->Append (i);
            break;
    }
    FixupInterceptorChain_ ();
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
    FixupInterceptorChain_ ();
}
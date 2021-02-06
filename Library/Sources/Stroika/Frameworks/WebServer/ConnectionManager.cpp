/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/Sleep.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/Execution/WaitForIOReady.h"
#include "../../Foundation/IO/Network/HTTP/Exception.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Methods.h"
#include "../../Foundation/Memory/Optional.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

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
    struct ServerHeadersInterceptor_ : public Interceptor {
        struct Rep_ : Interceptor::_IRep {
            Rep_ (const String& serverHeader)
                : fServerHeader_{serverHeader}
            {
            }
            virtual void HandleMessage (Message* m) override
            {
                Response& response = *m->PeekResponse ();
                response.UpdateHeader ([this] (auto* header) { RequireNotNull (header); header->server = fServerHeader_; });
            }
            const String fServerHeader_; // no need for synchronization cuz constant - just set on construction
        };
        ServerHeadersInterceptor_ (const String& serverHeader)
            : Interceptor{make_shared<Rep_> (serverHeader)}
        {
        }
    };
}

namespace {
    Sequence<Interceptor> mkEarlyInterceptors_ (const optional<Interceptor>& defaultFaultHandler, const Interceptor& serverEtcInterceptor)
    {
        Sequence<Interceptor> interceptors;
        interceptors += serverEtcInterceptor;
        if (defaultFaultHandler) {
            interceptors += *defaultFaultHandler;
        }
        return interceptors;
    }
    InterceptorChain mkInterceptorChain_ (const Router& router, const Sequence<Interceptor>& earlyInterceptors, const Sequence<Interceptor>& beforeInterceptors, const Sequence<Interceptor>& afterInterceptors)
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
        return Math::AtLeast (kMinThreadCnt_, options.fMaxConcurrentlyHandledConnections.value_or (options.fMaxConnections.value_or (Options::kDefault_MaxConnections) / 10));
    }
    inline unsigned int ComputeConnectionBacklog_ (const ConnectionManager::Options& options)
    {
        using Options = ConnectionManager::Options;
        constexpr unsigned int kMinDefaultTCPBacklog_{3u};
        return options.fTCPBacklog.value_or (Math::AtLeast (kMinDefaultTCPBacklog_, options.fMaxConnections.value_or (Options::kDefault_MaxConnections) * 3 / 4));
    }
    ConnectionManager::Options FillInDefaults_ (const ConnectionManager::Options& o)
    {
        using Options = ConnectionManager::Options;
        Options result{o};
        result.fCORS                              = Memory::NullCoalesce (result.fCORS, Options::kDefault_CORS ());
        result.fMaxConnections                    = Memory::NullCoalesce (result.fMaxConnections, Options::kDefault_MaxConnections);
        result.fMaxConcurrentlyHandledConnections = Memory::NullCoalesce (result.fMaxConcurrentlyHandledConnections, ComputeThreadPoolSize_ (result));
        result.fBindFlags                         = Memory::NullCoalesce (result.fBindFlags, Options::kDefault_BindFlags);
        result.fServerHeader                      = Memory::NullCoalesce (result.fServerHeader, Options::kDefault_ServerHeader);
        result.fAutomaticTCPDisconnectOnClose     = Memory::NullCoalesce (result.fAutomaticTCPDisconnectOnClose, Options::kDefault_AutomaticTCPDisconnectOnClose);
        result.fLinger                            = Memory::NullCoalesce (result.fLinger, Options::kDefault_Linger); // for now this is special and can be null/optional
        // result.fThreadPoolName; can remain nullopt
        result.fTCPBacklog = Memory::NullCoalesce (result.fTCPBacklog, ComputeConnectionBacklog_ (result));
        return result;
    }
}

ConnectionManager::ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Sequence<Route>& routes, const Options& options)
    : defaultErrorHandler{
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
              ConnectionManager* thisObj  = qStroika_Foundation_Common_Property_OuterObjPtr (property, &ConnectionManager::beforeInterceptors);
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
    , fEffectiveOptions_{FillInDefaults_ (options)}
    , fServerAndCORSEtcInterceptor_{ServerHeadersInterceptor_{*fEffectiveOptions_.fServerHeader}}
    , fDefaultErrorHandler_{DefaultFaultInterceptor{}}
    , fEarlyInterceptors_{mkEarlyInterceptors_ (fDefaultErrorHandler_, fServerAndCORSEtcInterceptor_)}
    , fBeforeInterceptors_{}
    , fAfterInterceptors_{}
    , fRouter_{routes, *fEffectiveOptions_.fCORS}
    , fInterceptorChain_{mkInterceptorChain_ (fRouter_, fEarlyInterceptors_, fBeforeInterceptors_, fAfterInterceptors_)}
    , fActiveConnectionThreads_{*fEffectiveOptions_.fMaxConcurrentlyHandledConnections, fEffectiveOptions_.fThreadPoolName}
    , fWaitForReadyConnectionThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New ([this] () { WaitForReadyConnectionLoop_ (); }, L"WebServer-ConnectionMgr-Wait4IOReady"_k)}
    , fListener_{bindAddresses,
                 *fEffectiveOptions_.fBindFlags,
                 [this] (const ConnectionOrientedStreamSocket::Ptr& s) { onConnect_ (s); },
                 *fEffectiveOptions_.fTCPBacklog}
{
    DbgTrace (L"Constructing WebServer::ConnectionManager (%p), with threadpoolSize=%d, backlog=%d", this, fActiveConnectionThreads_.GetPoolSize (), ComputeConnectionBacklog_ (options));
    fWaitForReadyConnectionThread_.Start (); // start here instead of autostart so a guaranteed initialized before thead main starts - see https://stroika.atlassian.net/browse/STK-706
}

#if qDefaultTracingOn
ConnectionManager::~ConnectionManager ()
{
    DbgTrace (L"Starting destructor for WebServer::ConnectionManager (%p)");
}
#endif

void ConnectionManager::onConnect_ (const ConnectionOrientedStreamSocket::Ptr& s)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::onConnect_", L"s=%s", Characters::ToString (s).c_str ())};
#endif
    s.SetAutomaticTCPDisconnectOnClose (*fEffectiveOptions_.fAutomaticTCPDisconnectOnClose);
    s.SetLinger (fEffectiveOptions_.fLinger); // 'missing' has meaning (feature disabled) for socket, so allow setting that too - doesn't mean don't pass on/use-default
    shared_ptr<Connection> conn = make_shared<Connection> (s, fInterceptorChain_);
    fInactiveSockSetPoller_.Add (conn);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    {
        scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
        DbgTrace (L"In onConnect_ (after adding connection %s): fActiveConnections_=%s, inactiveOpenConnections_=%s", Characters::ToString (conn).c_str (), Characters::ToString (fActiveConnections_.load ()).c_str (), Characters::ToString (GetInactiveConnections_ ()).c_str ());
    }
#endif
}

void ConnectionManager::WaitForReadyConnectionLoop_ ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::WaitForReadyConnectionLoop_")};
#endif

    // run til thread aborted
    while (true) {
        try {
            Thread::CheckForInterruption ();

#if USE_NOISY_TRACE_IN_THIS_MODULE_
            {
                scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                DbgTrace (L"At top of WaitForReadyConnectionLoop_: fActiveConnections_=%s, inactiveOpenConnections_=%s", Characters::ToString (fActiveConnections_.cget ().cref ()).c_str (), Characters::ToString (GetInactiveConnections_ ()).c_str ());
            }
#endif
            for (shared_ptr<Connection> readyConnection : fInactiveSockSetPoller_.WaitQuietly ()) {

                auto handleActivatedConnection = [this, readyConnection] () mutable {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::...processConnectionLoop")};
#endif

                    /*
                     * Process the request
                     */
                    bool keepAlive = (readyConnection->ReadAndProcessMessage () == Connection::eTryAgainLater);

                    /*
                     * Handle the Connection object, moving it to the appropriate list etc...
                     */
                    {
                        scoped_lock critSec{fActiveConnections_};
                        fActiveConnections_.rwget ().rwref ().Remove (readyConnection); // no matter what, remove from active connections
                        if (keepAlive) {
                            fInactiveSockSetPoller_.Add (readyConnection);
                        }
                    }

                    if (not keepAlive) {
                        if (readyConnection->response ().GetState () != Response::State::eCompleted) {
                            IgnoreExceptionsForCall (readyConnection->response ().End ());
                        }
                    }

#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    {
                        scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                        DbgTrace (L"at end of read&process task (keepAlive=%s) for connection %s: fActiveConnections_=%s, inactiveOpenConnections_=%s", Characters::ToString (keepAlive).c_str (), Characters::ToString (readyConnection).c_str (), Characters::ToString (fActiveConnections_.cget ().cref ()).c_str (), Characters::ToString (GetInactiveConnections_ ()).c_str ());
                    }
#endif
                };

                {
                    scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
                    fInactiveSockSetPoller_.Remove (readyConnection);
                    fActiveConnections_.rwget ().rwref ().Add (readyConnection);
                }
                fActiveConnectionThreads_.AddTask (handleActivatedConnection);
            }
        }
        catch (Thread::AbortException&) {
            Execution::ReThrow ();
        }
        catch (Thread::InterruptException&) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"Normal receipt of interruption because connection added to open but inactive connections.");
#endif
            // OK - completely ignore - this just means the list of sockets to watch has changed
        }
        catch (...) {
            DbgTrace (L"Internal exception in WaitForReadyConnectionLoop_ loop suppressed: %s", Characters::ToString (current_exception ()).c_str ());
        }
    }
}

void ConnectionManager::FixupInterceptorChain_ ()
{
    fInterceptorChain_ = InterceptorChain{mkInterceptorChain_ (fRouter_, fEarlyInterceptors_, fBeforeInterceptors_, fAfterInterceptors_)};
}

Collection<shared_ptr<Connection>> ConnectionManager::GetInactiveConnections_ () const
{
    return fInactiveSockSetPoller_.GetDescriptors ().Select<shared_ptr<Connection>> ([] (auto i) { return i.first; });
}

void ConnectionManager::ReplaceInEarlyInterceptor_ (const optional<Interceptor>& oldValue, const optional<Interceptor>& newValue)
{
    // replace old error handler in the interceptor chain, in the same spot if possible, and otherwise append
    auto                  rwLock = this->fEarlyInterceptors_.rwget ();
    Sequence<Interceptor> newInterceptors;
    bool                  addedDefault = false;
    for (Interceptor i : rwLock.load ()) {
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

Collection<shared_ptr<Connection>> ConnectionManager::GetConnections () const
{
    scoped_lock critSec{fActiveConnections_}; // Any place SWAPPING between active and inactive, hold this lock so both lists reamain consistent
    Ensure (Set<shared_ptr<Connection>>{fActiveConnections_.load ()}.Intersection (GetInactiveConnections_ ()).empty ());
    return GetInactiveConnections_ () + fActiveConnections_.load ();
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

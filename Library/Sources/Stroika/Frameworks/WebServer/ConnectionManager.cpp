/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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

namespace {
    struct ServerHeadersInterceptor_ : public Interceptor {
        struct Rep_ : Interceptor::_IRep {
            Rep_ (const optional<String>& serverHeader, ConnectionManager::CORSModeSupport corsSupportMode)
                : fServerHeader_ (serverHeader)
                , fCORSModeSupport (corsSupportMode)
            {
            }
            virtual void HandleFault ([[maybe_unused]] Message* m, [[maybe_unused]] const exception_ptr& e) noexcept override
            {
            }
            virtual void HandleMessage (Message* m) override
            {
                Response& response = *m->PeekResponse ();
                if (fServerHeader_) {
                    response.AddHeader (IO::Network::HTTP::HeaderName::kServer, *fServerHeader_);
                }
                if (fCORSModeSupport == ConnectionManager::CORSModeSupport::eSuppress) {
                    /*
                     *  From what I gather from https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS - the Allow-Origin is needed
                     *  on all responses, but these others just when you do a preflight - using OPTIONS.
                     */
                    response.AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowOrigin, L"*"sv);
                    if (m->PeekRequest ()->GetHTTPMethod () == IO::Network::HTTP::Methods::kOptions) {
                        response.AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowCredentials, L"true"sv);
                        response.AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowHeaders, L"Accept, Access-Control-Allow-Origin, Authorization, Cache-Control, Content-Type, Connection, Pragma, X-Requested-With"sv);
                        response.AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowMethods, L"DELETE, GET, OPTIONS, POST, PUT, TRACE, UPDATE"sv);
                        response.AddHeader (IO::Network::HTTP::HeaderName::kAccessControlMaxAge, L"86400"sv);
                    }
                }
            }
            const optional<String>                   fServerHeader_; // no need for synchronization cuz constant - just set on construction
            const ConnectionManager::CORSModeSupport fCORSModeSupport;
        };
        ServerHeadersInterceptor_ (const optional<String>& serverHeader, ConnectionManager::CORSModeSupport corsSupportMode)
            : Interceptor (make_shared<Rep_> (serverHeader, corsSupportMode))
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
 ***************** WebServer::ConnectionManager::Options ************************
 ********************************************************************************
 */
constexpr unsigned int                       ConnectionManager::Options::kDefault_MaxConnections;
constexpr Socket::BindFlags                  ConnectionManager::Options::kDefault_BindFlags;
const optional<String>                       ConnectionManager::Options::kDefault_ServerHeader = L"Stroika/2.0"sv;
constexpr ConnectionManager::CORSModeSupport ConnectionManager::Options::kDefault_CORSModeSupport;
constexpr Time::DurationSecondsType          ConnectionManager::Options::kDefault_AutomaticTCPDisconnectOnClose;
constexpr optional<int>                      ConnectionManager::Options::kDefault_Linger;

/*
 ********************************************************************************
 ************************* WebServer::ConnectionManager *************************
 ********************************************************************************
 */
const ConnectionManager::Options ConnectionManager::kDefaultOptions;

ConnectionManager::ConnectionManager (const SocketAddress& bindAddress, const Router& router, const Options& options)
    : ConnectionManager (Sequence<SocketAddress>{bindAddress}, router, options)
{
}

namespace {
    inline unsigned int ComputeThreadPoolSize_ (const ConnectionManager::Options& options)
    {
        using Options = ConnectionManager::Options;
        constexpr unsigned int kMinThreadCnt_{1u}; // one enough now that we support separate thread doing epoll/select and one read when data avail
        return Math::AtLeast (kMinThreadCnt_, options.fMaxConnections.value_or (Options::kDefault_MaxConnections) / 10);
    }
    inline unsigned int ComputeConnectionBacklog_ (const ConnectionManager::Options& options)
    {
        using Options = ConnectionManager::Options;
        constexpr unsigned int kMinDefaultTCPBacklog_{3u};
        return options.fTCPBacklog.value_or (Math::AtLeast (kMinDefaultTCPBacklog_, options.fMaxConnections.value_or (Options::kDefault_MaxConnections) * 3 / 4));
    }
}

ConnectionManager::ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Router& router, const Options& options)
    : fServerHeader_{Memory::OptionalValue (options.fServerHeader, Options::kDefault_ServerHeader)}
    , fCORSModeSupport_{options.fCORSModeSupport.value_or (Options::kDefault_CORSModeSupport)}
    , fServerAndCORSEtcInterceptor_{ServerHeadersInterceptor_{fServerHeader_, fCORSModeSupport_}}
    , fDefaultErrorHandler_{DefaultFaultInterceptor{}}
    , fEarlyInterceptors_{mkEarlyInterceptors_ (fDefaultErrorHandler_, fServerAndCORSEtcInterceptor_)}
    , fBeforeInterceptors_{}
    , fAfterInterceptors_{}
    , fLinger_{Memory::OptionalValue (options.fLinger, Options::kDefault_Linger)}
    , fAutomaticTCPDisconnectOnClose_{options.fAutomaticTCPDisconnectOnClose.value_or (Options::kDefault_AutomaticTCPDisconnectOnClose)}
    , fRouter_{router}
    , fInterceptorChain_{mkInterceptorChain_ (fRouter_, fEarlyInterceptors_, fBeforeInterceptors_, fAfterInterceptors_)}
    , fActiveConnectionThreads_{ComputeThreadPoolSize_ (options), options.fThreadPoolName} // implementation detail - due to EXPENSIVE blcoking read strategy - see https://stroika.atlassian.net/browse/STK-638
    , fListener_{bindAddresses,
                 options.fBindFlags.value_or (Options::kDefault_BindFlags),
                 [this] (const ConnectionOrientedStreamSocket::Ptr& s) { onConnect_ (s); },
                 ComputeConnectionBacklog_ (options)}
    , fWaitForReadyConnectionThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New ([this] () { WaitForReadyConnectionLoop_ (); }, L"WebServer-ConnectionMgr-Wait4IOReady"_k)}
{
    fWaitForReadyConnectionThread_.Start (); // start here instead of autostart so a guaranteed initialized before thead main starts - see https://stroika.atlassian.net/browse/STK-706
}

void ConnectionManager::onConnect_ (const ConnectionOrientedStreamSocket::Ptr& s)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::onConnect_", L"s=%s", Characters::ToString (s).c_str ()));
#endif
    s.SetAutomaticTCPDisconnectOnClose (GetAutomaticTCPDisconnectOnClose ());
    s.SetLinger (GetLinger ()); // 'missing' has meaning (feature disabled) for socket, so allow setting that too - doesn't mean don't pass on/use-default
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
    Debug::TraceContextBumper ctx (Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::WaitForReadyConnectionLoop_"));
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
                    Debug::TraceContextBumper ctx (Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::...processConnectionLoop"));
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
                        if (readyConnection->GetResponse ().GetState () != Response::State::eCompleted) {
                            IgnoreExceptionsForCall (readyConnection->GetResponse ().End ());
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

void ConnectionManager::SetServerHeader (optional<String> server)
{
    if (fServerHeader_ != server) {
        Interceptor old               = fServerAndCORSEtcInterceptor_;
        fServerAndCORSEtcInterceptor_ = ServerHeadersInterceptor_{server, fCORSModeSupport_};
        fServerHeader_                = server;
        ReplaceInEarlyInterceptor_ (old, fServerAndCORSEtcInterceptor_);
    }
}

void ConnectionManager::SetCORSModeSupport (CORSModeSupport support)
{
    if (fCORSModeSupport_ != support) {
        Interceptor old               = fServerAndCORSEtcInterceptor_;
        fServerAndCORSEtcInterceptor_ = ServerHeadersInterceptor_{fServerHeader_, fCORSModeSupport_};
        fCORSModeSupport_             = support;
        ReplaceInEarlyInterceptor_ (old, fServerAndCORSEtcInterceptor_);
    }
}

void ConnectionManager::SetDefaultErrorHandler (const optional<Interceptor>& defaultErrorHandler)
{
    if (fDefaultErrorHandler_ != defaultErrorHandler) {
        ReplaceInEarlyInterceptor_ (fDefaultErrorHandler_.load (), defaultErrorHandler);
        fDefaultErrorHandler_ = defaultErrorHandler;
    }
}

void ConnectionManager::SetEarlyInterceptors (const Sequence<Interceptor>& earlyInterceptors)
{
    fEarlyInterceptors_ = earlyInterceptors;
    FixupInterceptorChain_ ();
}

void ConnectionManager::SetBeforeInterceptors (const Sequence<Interceptor>& beforeInterceptors)
{
    fBeforeInterceptors_ = beforeInterceptors;
    FixupInterceptorChain_ ();
}

void ConnectionManager::SetAfterInterceptors (const Sequence<Interceptor>& afterInterceptors)
{
    fAfterInterceptors_ = afterInterceptors;
    FixupInterceptorChain_ ();
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

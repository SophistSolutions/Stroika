/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Characters/String_Constant.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/Exceptions.h"
#include "../../Foundation/Execution/Sleep.h"
#include "../../Foundation/IO/Network/HTTP/Exception.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Methods.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "DefaultFaultInterceptor.h"

#include "ConnectionManager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Frameworks::WebServer;

using Execution::Thread;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace {
    struct ServerHeadersInterceptor_ : public Interceptor {
        struct Rep_ : Interceptor::_IRep {
            Rep_ (const Optional<String>& serverHeader, ConnectionManager::CORSModeSupport corsSupportMode)
                : fServerHeader_ (serverHeader)
                , fCORSModeSupport (corsSupportMode)
            {
            }
            virtual void HandleFault (Message* m, const exception_ptr& e) noexcept override
            {
            }
            virtual void HandleMessage (Message* m) override
            {
                if (fServerHeader_) {
                    m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kServer, *fServerHeader_);
                }
                if (fCORSModeSupport == ConnectionManager::CORSModeSupport::eSuppress) {
                    /*
                     *  From what I gather from https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS - the Allow-Origin is needed
                     *  on all responses, but these others just when you do a preflight - using OPTIONS.
                     */
                    m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowOrigin, String_Constant{L"*"});
                    if (m->PeekRequest ()->GetHTTPMethod () == IO::Network::HTTP::Methods::kOptions) {
                        m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowCredentials, String_Constant{L"true"});
                        m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowHeaders, String_Constant{L"Accept, Access-Control-Allow-Origin, Authorization, Cache-Control, Content-Type, Connection, Pragma, X-Requested-With"});
                        m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowMethods, String_Constant{L"DELETE, GET, OPTIONS, POST, PUT, TRACE, UPDATE"});
                        m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kAccessControlMaxAge, String_Constant{L"86400"});
                    }
                }
            }
            const Optional<String>                   fServerHeader_; // no need for synchronization cuz constant - just set on construction
            const ConnectionManager::CORSModeSupport fCORSModeSupport;
        };
        ServerHeadersInterceptor_ (const Optional<String>& serverHeader, ConnectionManager::CORSModeSupport corsSupportMode)
            : Interceptor (make_shared<Rep_> (serverHeader, corsSupportMode))
        {
        }
    };
}

namespace {
    Sequence<Interceptor> mkEarlyInterceptors_ (const Optional<Interceptor>& defaultFaultHandler, const Interceptor& serverEtcInterceptor)
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
const Optional<String>                       ConnectionManager::Options::kDefault_ServerHeader = String_Constant{L"Stroika/2.0"};
constexpr ConnectionManager::CORSModeSupport ConnectionManager::Options::kDefault_CORSModeSupport;
constexpr Time::DurationSecondsType          ConnectionManager::Options::kDefault_AutomaticTCPDisconnectOnClose;
constexpr Optional<int>                      ConnectionManager::Options::kDefault_Linger;

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
        return Math::AtLeast (kMinThreadCnt_, options.fMaxConnections.Value (Options::kDefault_MaxConnections) / 10);
    }
    inline unsigned int ComputeConnectionBacklog_ (const ConnectionManager::Options& options)
    {
        using Options = ConnectionManager::Options;
        constexpr unsigned int kMinDefaultTCPBacklog_{3u};
        return options.fTCPBacklog.Value (Math::AtLeast (kMinDefaultTCPBacklog_, options.fMaxConnections.Value (Options::kDefault_MaxConnections) * 3 / 4));
    }
}

ConnectionManager::ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Router& router, const Options& options)
    : fServerHeader_ (options.fServerHeader.OptionalValue (Options::kDefault_ServerHeader))
    , fCORSModeSupport_ (options.fCORSModeSupport.Value (Options::kDefault_CORSModeSupport))
    , fServerAndCORSEtcInterceptor_{ServerHeadersInterceptor_{fServerHeader_, fCORSModeSupport_}}
    , fDefaultErrorHandler_ (DefaultFaultInterceptor{})
    , fEarlyInterceptors_{mkEarlyInterceptors_ (fDefaultErrorHandler_, fServerAndCORSEtcInterceptor_)}
    , fBeforeInterceptors_{}
    , fAfterInterceptors_{}
    , fLinger_ (options.fLinger.OptionalValue (Options::kDefault_Linger))
    , fAutomaticTCPDisconnectOnClose_ (options.fAutomaticTCPDisconnectOnClose.Value (Options::kDefault_AutomaticTCPDisconnectOnClose))
    , fRouter_ (router)
    , fInterceptorChain_{mkInterceptorChain_ (fRouter_, fEarlyInterceptors_, fBeforeInterceptors_, fAfterInterceptors_)}
    , fActiveConnectionThreads_{ComputeThreadPoolSize_ (options), options.fThreadPoolName} // implementation detail - due to EXPENSIVE blcoking read strategy - see https://stroika.atlassian.net/browse/STK-638
    , fListener_{bindAddresses,
                 options.fBindFlags.Value (Options::kDefault_BindFlags),
                 [this](const ConnectionOrientedSocket::Ptr& s) { onConnect_ (s); },
                 ComputeConnectionBacklog_ (options)}
    , fWaitForReadyConnectionThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting, Thread::New ([=]() { WaitForReadyConnectionLoop_ (); }, Thread::eAutoStart, L"ConnectionMgr-Wait4IOReady")}
{
}

void ConnectionManager::onConnect_ (const ConnectionOrientedSocket::Ptr& s)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::onConnect_", L"s=%s", Characters::ToString (s).c_str ()));
#endif
    s.SetAutomaticTCPDisconnectOnClose (GetAutomaticTCPDisconnectOnClose ());
    s.SetLinger (GetLinger ()); // 'missing' has meaning (feature disabled) for socket, so allow setting that too - doesn't mean dont pass on/use-default
    shared_ptr<Connection> conn = make_shared<Connection> (s, fInterceptorChain_);
    fInactiveOpenConnections_.rwget ()->Add (conn, s.GetNativeSocket ());
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"In onConnect_ (after adding connection): fActiveConnections_=%s, fInactiveOpenConnections_=%s", Characters::ToString (fActiveConnections_.cget ().cref ()).c_str (), Characters::ToString (fInactiveOpenConnections_.cget ().cref ()).c_str ());
#endif
}

void ConnectionManager::WaitForReadyConnectionLoop_ ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::WaitForReadyConnectionLoop_"));
#endif
    // run til thread aboorted
    while (true) {
        Execution::CheckForThreadInterruption ();

        Bijection<shared_ptr<Connection>, Execution::WaitForIOReady::FileDescriptorType> seeIfReady = fInactiveOpenConnections_.cget ().cref ();

#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"At top of WaitForReadyConnectionLoop_: fActiveConnections_=%s, fInactiveOpenConnections_=%s", Characters::ToString (fActiveConnections_.cget ().cref ()).c_str (), Characters::ToString (fInactiveOpenConnections_.cget ().cref ()).c_str ());
#endif

        // tricky - need this PLUS one more for magic wakeup - when somehting outside changes, like new connection
        // tmphac
        if (seeIfReady.Image ().empty ()) {
            Execution::Sleep (1);
            continue;
        }
        Execution::WaitForIOReady sockSetPoller{seeIfReady.Image ()};
        //tmphack - waitquietly (1) - to deal with fact we need to interuppt wait when set of sockets changes...
        for (auto readyFD : sockSetPoller.WaitQuietly (1).Value ()) {
            shared_ptr<Connection> conn = *seeIfReady.InverseLookup (readyFD);

            // @todo these three steps SB atomic/and transactional
            fInactiveOpenConnections_.rwget ().rwref ().RemoveDomainElement (conn);
            fActiveConnections_.rwget ().rwref ().Add (conn);

            fActiveConnectionThreads_.AddTask (
                [this, conn]() mutable {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    Debug::TraceContextBumper ctx (Stroika_Foundation_Debug_OptionalizeTraceArgs (L"ConnectionManager::...processConnectionLoop"));
#endif
                    bool keepAlive = false;
                    IgnoreExceptionsForCall (keepAlive = conn->ReadAndProcessMessage ());

                    // no matter what, remove from active connecitons
                    fActiveConnections_.rwget ().rwref ().Remove (conn);

                    if (keepAlive) {
                        fInactiveOpenConnections_.rwget ().rwref ().Add (conn, conn->GetSocket ().GetNativeSocket ());
                    }
                    else {
                        if (conn->GetResponse ().GetState () != Response::State::eCompleted) {
                            IgnoreExceptionsForCall (conn->GetResponse ().End ());
                        }
                    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"at end of read&process task (keepAlive=%s) for connection %s: fActiveConnections_=%s, fInactiveOpenConnections_=%s", Characters::ToString (keepAlive).c_str (), Characters::ToString (conn).c_str (), Characters::ToString (fActiveConnections_.cget ().cref ()).c_str (), Characters::ToString (fInactiveOpenConnections_.cget ().cref ()).c_str ());
#endif
                });
        }

        // for now - tmphack - just wait up to 1 second, and then retry
    }
}

void ConnectionManager::FixupInterceptorChain_ ()
{
    fInterceptorChain_ = InterceptorChain{mkInterceptorChain_ (fRouter_, fEarlyInterceptors_, fBeforeInterceptors_, fAfterInterceptors_)};
}

void ConnectionManager::ReplaceInEarlyInterceptor_ (const Optional<Interceptor>& oldValue, const Optional<Interceptor>& newValue)
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

#if 0
void    ConnectionManager::Start ()
{
    fThreads_.AddTask (bind (&ConnectionManager::DoMainConnectionLoop_, this));
}

void    ConnectionManager::Abort ()
{
    fThreads_.Abort ();
}

void    ConnectionManager::WaitForDone (Time::DurationSecondsType timeout)
{
    fThreads_.WaitForDone (timeout);
}

void    ConnectionManager::AbortAndWaitForDone (Time::DurationSecondsType timeout)
{
    fThreads_.AbortAndWaitForDone ();
}
#endif

#if 0
void    ConnectionManager::AddHandler (const shared_ptr<RequestHandler>& h)
{
    fHandlers_->push_back (h);
}

void    ConnectionManager::RemoveHandler (const shared_ptr<RequestHandler>& h)
{
    auto    readWriteObj = fHandlers_.GetReference ();
    for (auto i = readWriteObj->begin (); i != readWriteObj->end (); ++i) {
        if (*i == h) {
            readWriteObj->erase (i);
            return;
        }
    }
    RequireNotReached ();   // you must specify a valid handler to remove
}
#endif

#if 0
void ConnectionManager::AddConnection (const shared_ptr<Connection>& conn)
{
    fActiveConnections_.rwget ()->Add (conn);
}
#endif

void ConnectionManager::AbortConnection (const shared_ptr<Connection>& conn)
{
    AssertNotImplemented ();
}

Collection<shared_ptr<Connection>> ConnectionManager::GetConnections () const
{
    return Collection<shared_ptr<Connection>>{fInactiveOpenConnections_.cget ().cref ().Preimage ()} + fActiveConnections_.load ();
}

void ConnectionManager::SetServerHeader (Optional<String> server)
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

void ConnectionManager::SetDefaultErrorHandler (const Optional<Interceptor>& defaultErrorHandler)
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
    FixupInterceptorChain_ ();
}

void ConnectionManager::RemoveInterceptor (const Interceptor& i)
{
    bool found = false;
    {
        auto b4 = fBeforeInterceptors_.rwget ();
        if (Memory::Optional<size_t> idx = b4->IndexOf (i)) {
            b4->Remove (*idx);
            found = true;
        }
    }
    if (not found) {
        auto after = fAfterInterceptors_.rwget ();
        if (Memory::Optional<size_t> idx = after->IndexOf (i)) {
            after->Remove (*idx);
            found = true;
        }
    }
    Require (found);
    FixupInterceptorChain_ ();
}

#if 0
void    ConnectionManager::DoMainConnectionLoop_ ()
{
    // MUST DO MAJOR CRITICAL SECTION WORK HERE
    while (true) {
        Execution::Sleep (0.1); // hack - need smarter wait on available data
        shared_ptr<Connection>   conn;
        {
            auto    readWriteObj = fActiveConnections_.GetReference ();
            if (readWriteObj->empty ()) {
                conn = readWriteObj->front ();
            }
        }

        if (conn.get () != nullptr) {

// REALLY should create NEW TASK we subbit to the threadpool...
            DoOneConnection_ (conn);

            auto    readWriteObj = fActiveConnections_.GetReference ();
            for (auto i = readWriteObj->begin (); i != readWriteObj->end (); ++i) {
                if (*i == conn) {
                    readWriteObj->erase (i);
                    break;
                }
            }
        }

        // now go again...
    }
}

void    ConnectionManager::DoOneConnection_ (shared_ptr<Connection> c)
{
    // prevent exceptions for now cuz outer code not handling them...
    try {
        c->ReadHeaders ();

        AssertNotImplemented ();
#if 0
        shared_ptr<RequestHandler>   h;
        {
            auto    readWriteObj = fHandlers_.GetReference ();
            for (auto i = readWriteObj->begin (); i != readWriteObj->end (); ++i) {
                if ((*i)->CanHandleRequest (*c)) {
                    h = *i;
                }
            }
        }
        if (h.get () != nullptr) {
            h->HandleRequest (*c);
            c->GetResponse ().End ();
            c->Close ();//tmphack
            return;
        }
#endif
        Execution::Throw (Network::HTTP::Exception (StatusCodes::kNotFound));
    }
    catch (...) {
        c->GetResponse ().End ();
        c->Close ();//tmphack
    }
}
#endif

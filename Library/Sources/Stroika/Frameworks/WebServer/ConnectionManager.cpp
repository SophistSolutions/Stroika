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

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

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
                    m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowOrigin, String_Constant{L"*"});
                    m->PeekResponse ()->AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowHeaders, String_Constant{L"Origin, X-Requested-With, Content-Type, Accept, Authorization"});
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
    , fThreads_ (options.fMaxConnections.Value (Options::kDefault_MaxConnections), options.fThreadPoolName) // implementation detail - due to EXPENSIVE blcoking read strategy
    , fListener_ (bindAddresses, options.fBindFlags.Value (Options::kDefault_BindFlags), [this](const ConnectionOrientedSocket::Ptr& s) { onConnect_ (s); }, options.fMaxConnections.Value (Options::kDefault_MaxConnections) / 2)
{
}

void ConnectionManager::onConnect_ (ConnectionOrientedSocket::Ptr s)
{
    // @todo - MAKE Connection OWN the threadtask (or have all the logic below) - and then AddConnection adds the task,
    // and that way wehn we call 'remove connection- ' we can abort the task (if needed)
    fThreads_.AddTask (
        [this, s]() mutable {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx (L"ConnectionManager::onConnect_::...runConnectionOnAnotherThread");
#endif
            s.SetAutomaticTCPDisconnectOnClose (GetAutomaticTCPDisconnectOnClose ());
            s.SetLinger (GetLinger ()); // 'missing' has meaning (feature disabled) for socket, so allow setting that too - doesn't mean dont pass on/use-default
            Connection conn (s, fInterceptorChain_);
            auto&&     cleanup = Execution::Finally ([&conn]() { if (conn.GetResponse ().GetState () != Response::State::eCompleted) {conn.GetResponse ().End (); } });
            // Now read and process each method - currently wasteful - throwing a thread at context
            // Also - if keep-alive - keep reading
            while (conn.ReadAndProcessMessage ())
                ;
        });
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

void ConnectionManager::AddConnection (const shared_ptr<Connection>& conn)
{
    fActiveConnections_.rwget ()->push_back (conn);
}

void ConnectionManager::AbortConnection (const shared_ptr<Connection>& conn)
{
    AssertNotImplemented ();
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

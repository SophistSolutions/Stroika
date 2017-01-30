/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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
    InterceptorChain mkInterceptorChain_ (const Router& router, const Optional<Interceptor>& defaultFaultHandler, const Optional<String>& serverHeader, ConnectionManager::CORSModeSupport corsSupportMode, const Sequence<Interceptor>& beforeInterceptors, const Sequence<Interceptor>& afterInterceptors)
    {
        Sequence<Interceptor> interceptors;
        interceptors += ServerHeadersInterceptor_{serverHeader, corsSupportMode};
        if (defaultFaultHandler) {
            interceptors += *defaultFaultHandler;
        }
        interceptors += router;
        return InterceptorChain{interceptors};
    }
}

/*
 ********************************************************************************
 ************************* WebServer::ConnectionManager *************************
 ********************************************************************************
 */
ConnectionManager::ConnectionManager (const SocketAddress& bindAddress, const Router& router, size_t maxConnections)
    : ConnectionManager (Sequence<SocketAddress>{bindAddress}, Socket::BindFlags{}, router, maxConnections)
{
}

ConnectionManager::ConnectionManager (const SocketAddress& bindAddress, const Socket::BindFlags& bindFlags, const Router& router, size_t maxConnections)
    : ConnectionManager (Sequence<SocketAddress>{bindAddress}, bindFlags, router, maxConnections)
{
}

ConnectionManager::ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Router& router, size_t maxConnections)
    : ConnectionManager (bindAddresses, Socket::BindFlags{}, router)
{
}

ConnectionManager::ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Socket::BindFlags& bindFlags, const Router& router, size_t maxConnections)
    : fDefaultErrorHandler_ (DefaultFaultInterceptor{})
    , fServerHeader_ (String_Constant{L"Stroika/2.0"})
    , fRouter_ (router)
    , fInterceptorChain_{mkInterceptorChain_ (fRouter_, fDefaultErrorHandler_, fServerHeader_.load (), fCORSModeSupport_, fBeforeInterceptors_, fAfterInterceptors_)}
    , fThreads_ (maxConnections) // implementation detail - due to EXPENSIVE blcoking read strategy
    , fListener_ (bindAddresses, bindFlags, [this](Socket s) { onConnect_ (s); }, maxConnections / 2)
{
}

void ConnectionManager::onConnect_ (Socket s)
{
    // @todo - MAKE Connection OWN the threadtask (or have all the logic below) - and then AddConnection adds the task,
    // and that way wehn we call 'remove connection- ' we can abort the task (if needed)
    fThreads_.AddTask (
        [this, s]() {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx (L"ConnectionManager::onConnect_::...runConnectionOnAnotherThread");
#endif
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
    fInterceptorChain_ = InterceptorChain{mkInterceptorChain_ (fRouter_, fDefaultErrorHandler_, fServerHeader_.load (), fCORSModeSupport_, fBeforeInterceptors_, fAfterInterceptors_)};
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
    fServerHeader_ = server;
    FixupInterceptorChain_ ();
}

void ConnectionManager::SetCORSModeSupport (CORSModeSupport support)
{
    fCORSModeSupport_ = support;
    FixupInterceptorChain_ ();
}

void ConnectionManager::SetDefaultErrorHandler (const Optional<Interceptor>& defaultErrorHandler)
{
    fDefaultErrorHandler_ = defaultErrorHandler;
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
        auto   b4  = fBeforeInterceptors_.rwget ();
        size_t idx = b4->IndexOf (i);
        if (idx != kBadSequenceIndex) {
            b4->Remove (idx);
            found = true;
        }
    }
    if (not found) {
        auto   after = fAfterInterceptors_.rwget ();
        size_t idx   = after->IndexOf (i);
        if (idx != kBadSequenceIndex) {
            after->Remove (idx);
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

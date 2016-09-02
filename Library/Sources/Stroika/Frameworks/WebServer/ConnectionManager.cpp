/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../../Foundation/Characters/String_Constant.h"
#include    "../../Foundation/Characters/ToString.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/DataExchange/BadFormatException.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Debug/Trace.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Execution/Sleep.h"
#include    "../../Foundation/IO/Network/HTTP/Headers.h"
#include    "../../Foundation/IO/Network/HTTP/Exception.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "ConnectionManager.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Frameworks::WebServer;






// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



/*
 ********************************************************************************
 ************************* WebServer::ConnectionManager *************************
 ********************************************************************************
 */
ConnectionManager::ConnectionManager (const SocketAddress& bindAddress, const Router& router, size_t maxConnections)
    : ConnectionManager (bindAddress, Socket::BindFlags {}, router)
{
    fThreads_.SetPoolSize (maxConnections); // implementation detail - due to EXPENSIVE blcoking read strategy
}

ConnectionManager::ConnectionManager (const SocketAddress& bindAddress, const Socket::BindFlags& bindFlags, const Router& router, size_t maxConnections)
    : fServerHeader_ (String_Constant { L"Stroika/2.0" })
    , fRouter_ (router)
    , fInterceptorChain_ { Sequence<Interceptor> { fRouter_ } }
, fThreads_ (maxConnections) // implementation detail - due to EXPENSIVE blcoking read strategy
, fListener_  (bindAddress, bindFlags, [this](Socket s)  { onConnect_ (s); }, maxConnections / 2)
{
}

void    ConnectionManager::onConnect_ (Socket s)
{
    // @todo - MAKE Connection OWN the threadtask (or have all the logic below) - and then AddConnection adds the task,
    // and that way wehn we call 'remove connection- ' we can abort the task (if needed)
    fThreads_.AddTask (
    [this, s] () {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx (L"ConnectionManager::onConnect_::...runConnectionOnAnotherThread");
#endif
        // now read
        Connection  conn (s, fInterceptorChain_);
        conn.ReadHeaders ();    // bad API. Must rethink...
        if (Optional<String> o = fServerHeader_.cget ()) {
            conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kServer, *o);
        }
        if (GetCORSModeSupport () == CORSModeSupport::eSuppress) {
            conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowOrigin, String_Constant { L"*" });
            conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowHeaders, String_Constant { L"Origin, X-Requested-With, Content-Type, Accept, Authorization" });
        }
        constexpr bool kSupportHTTPKeepAlives_ { false };
        if (not kSupportHTTPKeepAlives_) {
            // From https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
            //      HTTP/1.1 applications that do not support persistent connections MUST include the "close" connection option in every message.
            conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kConnection, String_Constant { L"close" });
        }
        String url = conn.GetRequest ().fURL.GetFullURL ();
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Serving page %s", url.c_str ());
#endif
        try {
            fInterceptorChain_.HandleMessage (&conn.fMessage_);
        }
        catch (const IO::Network::HTTP::Exception& e) {
            conn.GetResponse ().SetStatus (e.GetStatus (), e.GetReason ());
            conn.GetResponse ().printf (L"<html><body><p>Exception: %s</p></body></html>", Characters::ToString (e).c_str ());
            conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
        }
        catch (...) {
            conn.GetResponse ().SetStatus (HTTP::StatusCodes::kInternalError);
            conn.GetResponse ().printf (L"<html><body><p>Exception: %s</p></body></html>", Characters::ToString (std::current_exception ()).c_str ());
            conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
        }
        conn.GetResponse ().End ();
    }
    );
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

void    ConnectionManager::AddConnection (const shared_ptr<Connection>& conn)
{
    fActiveConnections_.rwget ()->push_back (conn);
}

void    ConnectionManager::AbortConnection (const shared_ptr<Connection>& conn)
{
    AssertNotImplemented ();
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

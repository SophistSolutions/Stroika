/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>

#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/DataExchange/BadFormatException.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Execution/Sleep.h"
#include    "../../Foundation/IO/Network/HTTP/Headers.h"
#include    "../../Foundation/IO/Network/HTTP/Exception.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "ConnectionManager.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Frameworks::WebServer;



/*
 ********************************************************************************
 ************************* WebServer::ConnectionManager *************************
 ********************************************************************************
 */
ConnectionManager::ConnectionManager (const SocketAddress& bindAddress, const Router& router)
    : fRouter_ (router)
    , fListener_  (bindAddress, [this](Socket s)  { onConnect_ (s); })
, fServerHeader_ (L"stroika-web-server-demo")
{
}

void ConnectionManager::onConnect_ (Socket s)
{
    Execution::Thread runConnectionOnAnotherThread ([this, s]() {
        // now read
        Connection conn (s);
        conn.ReadHeaders ();    // bad API. Must rethink...
        if (fServerHeader_) {
            conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kServer, *fServerHeader_);
        }
        if (fIgnoreSillyCORS_) {
            conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowOrigin, L"*");
            conn.GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kAccessControlAllowHeaders, L"Origin, X-Requested-With, Content-Type, Accept, Authorization");
        }
        String url = conn.GetRequest ().fURL.GetFullURL ();
        DbgTrace (L"Serving page %s", url.c_str ());
        try {
            Optional<RequestHandler>    handler = fRouter_.Lookup (conn.GetRequest ());
            if (handler) {
                (*handler) (&conn.GetRequest (), &conn.GetResponse ());
            }
            else {
                Execution::Throw (IO::Network::HTTP::Exception (HTTP::StatusCodes::kNotFound));
            }
        }
        catch (const IO::Network::HTTP::Exception& e) {
            conn.GetResponse ().SetStatus (e.GetStatus (), e.GetReason ());
            conn.GetResponse ().writeln (L"<html><body><p>OOPS</p></body></html>");
            conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
        }
        catch (...) {
            conn.GetResponse ().SetStatus (HTTP::StatusCodes::kInternalError);
            conn.GetResponse ().writeln (L"<html><body><p>OOPS</p></body></html>");
            conn.GetResponse ().SetContentType (DataExchange::PredefinedInternetMediaType::Text_HTML_CT ());
        }
        conn.GetResponse ().End ();
    });
    runConnectionOnAnotherThread.SetThreadName (L"Connection Thread");  // Could use a fancier name (connection#, from remote address?)
    runConnectionOnAnotherThread.Start ();
    runConnectionOnAnotherThread.WaitForDone ();    // maybe save these in connection mgr so we can force them all shut down...
};

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
    fActiveConnections_->push_back (conn);
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
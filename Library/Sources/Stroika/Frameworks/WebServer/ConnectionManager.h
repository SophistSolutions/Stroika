/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ConnectionManager_h_
#define _Stroika_Framework_WebServer_ConnectionManager_h_   1

#include    "../StroikaPreComp.h"

#include    <list>
#include    <memory>

#include    "../../Foundation/Execution/Lockable.h"
#include    "../../Foundation/Execution/ThreadPool.h"

#include    "Request.h"
#include    "RequestHandler.h"
#include    "Response.h"
#include    "Connection.h"



/*
 * TODO:
 *
 *      o   First get basics working
 *
 *      o   Handle thread safety (very tricky - watch adding connection handlers!)
 *
 *      o   Do a single background thread first to run handlers.
 *
 *      o   THen do threadpooling.
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;
            using   namespace   Stroika::Foundation::IO;
            using   Characters::String;
            using   std::shared_ptr;


            /**
             *  This class is a useful helper for managing a set of connections. You can start it and stop it
             *  (it maintains internal threads). And you can hand it Connections, along with a set of handlers,
             *  and it will monitor the connections, and when any is ready with more input, it will assign the
             *  appropriate handler to handle the request, and produce the response.
             *
             *  This doesn't CURRENTLY support keepalives.
             *  This doesn't CURRENTLY (really) support a threadpool (it has one but just puts one thread inside).
             */
            class   ConnectionManager {
            public:
                ConnectionManager () = default;
                ConnectionManager (const ConnectionManager&) = delete;
                ~ConnectionManager () = default;

            public:
                nonvirtual  const ConnectionManager& operator= (const ConnectionManager&) = delete;

            public:
                nonvirtual  void    Start ();
                nonvirtual  void    Abort ();
                nonvirtual  void    WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);
                nonvirtual  void    AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                nonvirtual  void    AddHandler (const shared_ptr<RequestHandler>& h);
                nonvirtual  void    RemoveHandler (const shared_ptr<RequestHandler>& h);

            public:
                nonvirtual  void    AddConnection (const shared_ptr<Connection>& conn);
                nonvirtual  void    AbortConnection (const shared_ptr<Connection>& conn);

            public:
                /**
                 *  We need some sort of status flag on connections - saying of they are OPEN or not - or done.
                 *  But this will return just those which are not 'done'. Of course - due to asyncrhony,
                 *  by the time one looks at the list, some may already be done.
                 */
                nonvirtual  vector<shared_ptr<Connection>> GetConnections () const;

            private:
                //VERY VERY SUPER PRIMITIVE FIFRST DRAFT OF CONNECTION HANDLING
                nonvirtual  void    DoMainConnectionLoop_ ();
                nonvirtual  void    DoOneConnection_ (shared_ptr<Connection> c);

            private:
                // REALLY could use Stroika threadsafe lists here!!! - so could just iterate and forget!
                Execution::Lockable<list<shared_ptr<RequestHandler>>>    fHandlers_;
                Execution::Lockable<list<shared_ptr<Connection>>>        fActiveConnections_;

                // we may eventually want two thread pools - one for managing bookkeeping/monitoring harvests, and one for actually handling
                // connections. Or maybe a single thread for the bookkeeping, and the pool for handling ongoing connections?
                //
                // But for now - KISS
                //
                // Note - for now - we dont even handle servicing connections in the threadpool!!! - just one thread
                Execution::ThreadPool                           fThreads_;
            };

        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ConnectionManager.inl"

#endif  /*_Stroika_Framework_WebServer_ConnectionManager_h_*/

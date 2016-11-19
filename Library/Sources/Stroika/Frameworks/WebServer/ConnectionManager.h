/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ConnectionManager_h_
#define _Stroika_Framework_WebServer_ConnectionManager_h_   1

#include    "../StroikaPreComp.h"

#include    <list>
#include    <memory>

#include    "../../Foundation/Execution/Synchronized.h"
#include    "../../Foundation/Execution/ThreadPool.h"
#include    "../../Foundation/IO/Network/SocketAddress.h"
#include    "../../Foundation/IO/Network/Listener.h"

#include    "Request.h"
#include    "Response.h"
#include    "Router.h"
#include    "Connection.h"



/*
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
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
            using   Characters::String;
            using   IO::Network::SocketAddress;
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
                ConnectionManager (const SocketAddress& bindAddress, const Router& router, size_t maxConnections = 1);
                ConnectionManager (const SocketAddress& bindAddress, const Socket::BindFlags& bindFlags, const Router& router, size_t maxConnections = 1);
                ConnectionManager (const ConnectionManager&) = delete;
                ~ConnectionManager () = default;

            public:
                nonvirtual  const ConnectionManager& operator= (const ConnectionManager&) = delete;

            public:
                /**
                 */
                nonvirtual  Optional<String> GetServerHeader () const;

            public:
                /**
                 */
                nonvirtual  void    SetServerHeader (Optional<String> server);

            public:
                /**
                 *  CORSMode
                 *
                 *  \brief  If eNone this does nothing, and if CORSModeSupport == eSuppress, CORS Headers are emitted which suppress the CORS features.
                 *
                 *  @see GetCORSModeSupport (), SetCORSModeSupport ()
                 */
                enum    class   CORSModeSupport {
                    eNone,
                    eSuppress,

                    Stroika_Define_Enum_Bounds (eNone, eSuppress)

                    eDEFAULT = eSuppress,
                };

            public:
                /**
                 *  @see CORSModeSupport
                 */
                nonvirtual  CORSModeSupport GetCORSModeSupport () const;

            public:
                /**
                 *  @see CORSModeSupport
                 */
                nonvirtual  void SetCORSModeSupport (CORSModeSupport support);

            public:
                /**
                 *  Get the list of interceptors before the private ConnectionManager interceptors (e.g. router).
                 */
                nonvirtual  Sequence<Interceptor> GetBeforeInterceptors () const;

            public:
                /**
                 */
                nonvirtual  void    SetBeforeInterceptors (const Sequence<Interceptor>& beforeInterceptors);

            private:
                Execution::Synchronized<Sequence<Interceptor>>               fBeforeInterceptors_;

            public:
                /**
                 *  Get the list of interceptors after the private ConnectionManager interceptors (e.g. router).
                 */
                nonvirtual  Sequence<Interceptor> GetAfterInterceptors () const;

            public:
                /**
                 */
                nonvirtual  void    SetAfterInterceptors (const Sequence<Interceptor>& afterInterceptors);

            private:
                Execution::Synchronized<Sequence<Interceptor>>               fAfterInterceptors_;

#if 0
            public:
                nonvirtual  void    Start ();
                nonvirtual  void    Abort ();
                nonvirtual  void    WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);
                nonvirtual  void    AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);
#endif
#if 0
            public:
                nonvirtual  void    AddHandler (const shared_ptr<RequestHandler>& h);
                nonvirtual  void    RemoveHandler (const shared_ptr<RequestHandler>& h);
#endif

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

#if 0
            private:
                //VERY VERY SUPER PRIMITIVE FIFRST DRAFT OF CONNECTION HANDLING
                nonvirtual  void    DoMainConnectionLoop_ ();
                nonvirtual  void    DoOneConnection_ (shared_ptr<Connection> c);
#endif

            private:
                nonvirtual  void onConnect_ (Socket s);

            private:
                Execution::Synchronized<Optional<String>>               fServerHeader_;
                CORSModeSupport                                         fCORSModeSupport_ { CORSModeSupport::eDEFAULT };
                Router                                                  fRouter_;
                Execution::Synchronized<InterceptorChain>               fInterceptorChain_;

                // we may eventually want two thread pools - one for managing bookkeeping/monitoring harvests, and one for actually handling
                // connections. Or maybe a single thread for the bookkeeping, and the pool for handling ongoing connections?
                //
                // But for now - KISS
                //
                // Note - for now - we dont even handle servicing connections in the threadpool!!! - just one thread
                Execution::ThreadPool                                   fThreads_;

                // Note: this must be declared after the threadpool so its shutdown on destruction before the thread pool, and doesnt try to launch
                // new tasks into an already destroyed threadpool.
                IO::Network::Listener                                   fListener_;
                Execution::Synchronized<list<shared_ptr<Connection>>>   fActiveConnections_;
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

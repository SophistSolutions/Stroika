/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ConnectionManager_h_
#define _Stroika_Framework_WebServer_ConnectionManager_h_ 1

#include "../StroikaPreComp.h"

#include <list>
#include <memory>

#include "../../Foundation/Execution/Synchronized.h"
#include "../../Foundation/Execution/ThreadPool.h"
#include "../../Foundation/IO/Network/Listener.h"
#include "../../Foundation/IO/Network/SocketAddress.h"

#include "Connection.h"
#include "Request.h"
#include "Response.h"
#include "Router.h"

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

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            using namespace Stroika::Foundation;
            using Characters::String;
            using IO::Network::Socket;
            using IO::Network::SocketAddress;
            using IO::Network::ConnectionOrientedSocket;

            /**
             *  This class is a useful helper for managing a set of connections. You can start it and stop it
             *  (it maintains internal threads). And you can hand it Connections, along with a set of handlers,
             *  and it will monitor the connections, and when any is ready with more input, it will assign the
             *  appropriate handler to handle the request, and produce the response.
             *
             *  This doesn't CURRENTLY support keepalives.
             */
            class ConnectionManager {
            public:
                struct Options;
                static const Options kDefaultOptions;

            public:
                ConnectionManager (const SocketAddress& bindAddress, const Router& router, const Options& options = kDefaultOptions);
                ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Router& router, const Options& options = kDefaultOptions);
                _Deprecated_ ("Deprecated in 2.0a207")
                    ConnectionManager (const SocketAddress& bindAddress, const Router& router, unsigned int maxConnections);
                _Deprecated_ ("Deprecated in 2.0a207")
                    ConnectionManager (const SocketAddress& bindAddress, const Socket::BindFlags& bindFlags, const Router& router, unsigned int maxConnections = 1);
                _Deprecated_ ("Deprecated in 2.0a207")
                    ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Router& router, unsigned int maxConnections);
                _Deprecated_ ("Deprecated in 2.0a207")
                    ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Socket::BindFlags& bindFlags, const Router& router, unsigned int maxConnections = 1);
                ConnectionManager (const ConnectionManager&) = delete;
                ~ConnectionManager ()                        = default;

            public:
                nonvirtual const ConnectionManager& operator= (const ConnectionManager&) = delete;

            public:
                /**
                 */
                nonvirtual Optional<String> GetServerHeader () const;

            public:
                /**
                 */
                nonvirtual void SetServerHeader (Optional<String> server);

            public:
                /**
                 *  CORSMode
                 *
                 *  \brief  If eNone this does nothing, and if CORSModeSupport == eSuppress, CORS Headers are emitted which suppress the CORS features.
                 *
                 *  @see GetCORSModeSupport (), SetCORSModeSupport ()
                 */
                enum class CORSModeSupport {
                    eNone,
                    eSuppress,

                    eDEFAULT = eSuppress,

                    Stroika_Define_Enum_Bounds (eNone, eSuppress)
                };

            public:
                /**
                 *  @see CORSModeSupport
                 */
                nonvirtual CORSModeSupport GetCORSModeSupport () const;

            public:
                /**
                 *  @see CORSModeSupport
                 */
                nonvirtual void SetCORSModeSupport (CORSModeSupport support);

            public:
                /**
                 * @see Socket::GetLinger
                 */
                nonvirtual Optional<int> GetLinger () const;

            public:
                /**
                 *  @see GetLinger
                 */
                nonvirtual void SetLinger (const Optional<int>& linger);

            public:
                /**
                 *  This feature causes sockets to automatically flush their data - and avoid connection reset - when possible.
                 *  This makes the closing socket process more costly and slow, so is optional, but is on by default because it makes
                 *  commmunications more releable.
                 *
                 *  Turn this on - especially - if you see connection reset when clients talk to the Stroika web-server (TCP RST sent).
                 *
                 *  \note - this defaults to 5 seconds.
                 *
                 * @see Socket::GetAutomaticTCPDisconnectOnClose
                 */
                nonvirtual Optional<Time::DurationSecondsType> GetAutomaticTCPDisconnectOnClose ();

            public:
                /**
                 *  @see GetAutomaticTCPDisconnectOnClose ()
                 */
                nonvirtual void SetAutomaticTCPDisconnectOnClose (const Optional<Time::DurationSecondsType>& waitFor);

            public:
                /**
                 *  This defaults to @DefaultFaultInterceptor, but can be set to 'missing' or any other fault handler. Not also - that
                 *  all interceptors can engage in fault handling. This is just meant to provide a simple one-stop-shop for how to
                 *  handle faults in one place.
                 */
                nonvirtual Optional<Interceptor> GetDefaultErrorHandler () const;

            public:
                /**
                 *  @see GetDefaultErrorHandler
                 */
                nonvirtual void SetDefaultErrorHandler (const Optional<Interceptor>& defaultErrorHandler);

            private:
                Execution::Synchronized<Optional<Interceptor>> fDefaultErrorHandler_;

            public:
                /**
                 *  Get the list of interceptors before the private ConnectionManager interceptors (e.g. router).
                 */
                nonvirtual Sequence<Interceptor> GetBeforeInterceptors () const;

            public:
                /**
                 * @see GetBeforeInterceptors
                 */
                nonvirtual void SetBeforeInterceptors (const Sequence<Interceptor>& beforeInterceptors);

            private:
                Execution::Synchronized<Sequence<Interceptor>> fBeforeInterceptors_;

            public:
                /**
                 *  Get the list of interceptors after the private ConnectionManager interceptors (e.g. router).
                 * @see GetBeforeInterceptors
                 */
                nonvirtual Sequence<Interceptor> GetAfterInterceptors () const;

            public:
                /**
                 * @see GetAfterInterceptors
                 */
                nonvirtual void SetAfterInterceptors (const Sequence<Interceptor>& afterInterceptors);

            private:
                Execution::Synchronized<Sequence<Interceptor>> fAfterInterceptors_;

            public:
                /**
                 *  These 'before' and 'after' values are releative to the router, which towards the end of the chain.
                 */
                enum InterceptorAddRelativeTo {
                    ePrepend,
                    eAppend,
                    eAfterBeforeInterceptors,
                };

            public:
                /**
                 */
                nonvirtual void AddInterceptor (const Interceptor& i, InterceptorAddRelativeTo relativeTo);

            public:
                /**
                 */
                nonvirtual void RemoveInterceptor (const Interceptor& i);

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
                nonvirtual void AddConnection (const shared_ptr<Connection>& conn);
                nonvirtual void AbortConnection (const shared_ptr<Connection>& conn);

            public:
                /**
                 *  We need some sort of status flag on connections - saying of they are OPEN or not - or done.
                 *  But this will return just those which are not 'done'. Of course - due to asyncrhony,
                 *  by the time one looks at the list, some may already be done.
                 */
                nonvirtual vector<shared_ptr<Connection>> GetConnections () const;

#if 0
            private:
                //VERY VERY SUPER PRIMITIVE FIFRST DRAFT OF CONNECTION HANDLING
                nonvirtual  void    DoMainConnectionLoop_ ();
                nonvirtual  void    DoOneConnection_ (shared_ptr<Connection> c);
#endif

            private:
                nonvirtual void onConnect_ (ConnectionOrientedSocket s);

            private:
                nonvirtual void FixupInterceptorChain_ ();

            private:
                Execution::Synchronized<Optional<String>>                    fServerHeader_;
                CORSModeSupport                                              fCORSModeSupport_{CORSModeSupport::eDEFAULT};
                Execution::Synchronized<Optional<int>>                       fLinger_;
                Execution::Synchronized<Optional<Time::DurationSecondsType>> fAutomaticTCPDisconnectOnClose_;
                Router                                                       fRouter_;
                InterceptorChain                                             fInterceptorChain_; // no need to synchonize cuz internally synchonized

                // we may eventually want two thread pools - one for managing bookkeeping/monitoring harvests, and one for actually handling
                // connections. Or maybe a single thread for the bookkeeping, and the pool for handling ongoing connections?
                //
                // But for now - KISS
                //
                // Note - for now - we dont even handle servicing connections in the threadpool!!! - just one thread
                Execution::ThreadPool fThreads_;

                // Note: this must be declared after the threadpool so its shutdown on destruction before the thread pool, and doesnt try to launch
                // new tasks into an already destroyed threadpool.
                IO::Network::Listener                                 fListener_;
                Execution::Synchronized<list<shared_ptr<Connection>>> fActiveConnections_;
            };

            struct ConnectionManager::Options {
                Optional<unsigned int>              fMaxConnections;
                Optional<Socket::BindFlags>         fBindFlags;
                Optional<String>                    fServerHeader;
                Optional<CORSModeSupport>           fCORSModeSupport;
                Optional<Time::DurationSecondsType> fAutomaticTCPDisconnectOnClose;
                Optional<int>                       fLinger; // SO_LINGER

#if qCompilerAndStdLib_DefaultCTORNotAutoGeneratedSometimes_Buggy
                Options (const Optional<unsigned int>& mc = {}, const Optional<Socket::BindFlags>& b = {}, const Optional<String>& sh = {}, const Optional<CORSModeSupport>& cors = {}, const Optional<Time::DurationSecondsType>& atcpc = {}, const Optional<int>& li)
                    : fMaxConnections (mc)
                    , fBindFlags (b)
                    , fServerHeader (sh)
                    , fCORSModeSupport (cors)
                    , fAutomaticTCPDisconnectOnClose (atcpc)
                    , fLinger (li)
                {
                }
#endif

                static constexpr unsigned int              kDefault_MaxConnections = 1;
                static constexpr Socket::BindFlags         kDefault_BindFlags{};
                static const Optional<String>              kDefault_ServerHeader;
                static constexpr CORSModeSupport           kDefault_CORSModeSupport               = CORSModeSupport::eDEFAULT;
                static constexpr Time::DurationSecondsType kDefault_AutomaticTCPDisconnectOnClose = 5.0;
                static constexpr Optional<int>             kDefault_Linger{};
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionManager.inl"

#endif /*_Stroika_Framework_WebServer_ConnectionManager_h_*/

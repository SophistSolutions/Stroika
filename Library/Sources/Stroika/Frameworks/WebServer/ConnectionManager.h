/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ConnectionManager_h_
#define _Stroika_Framework_WebServer_ConnectionManager_h_ 1

#include "../StroikaPreComp.h"

#include <list>
#include <memory>

#include "../../Foundation/Containers/Bijection.h"
#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/Execution/Synchronized.h"
#include "../../Foundation/Execution/ThreadPool.h"
#include "../../Foundation/Execution/WaitForIOReady.h"
#include "../../Foundation/IO/Network/Listener.h"
#include "../../Foundation/IO/Network/SocketAddress.h"

#include "Connection.h"
#include "Request.h"
#include "Response.h"
#include "Router.h"

/*
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      o   REVIEW thread safety (very tricky - watch adding connection handlers!)
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            using namespace Stroika::Foundation;
            using Characters::String;
            using Containers::Bijection;
            using Containers::Collection;
            using IO::Network::ConnectionOrientedSocket;
            using IO::Network::Socket;
            using IO::Network::SocketAddress;

            /**
             *  This class is a useful helper for managing a set of connections. You can start it and stop it
             *  (it maintains internal threads). And you can hand it Connections, along with a set of handlers,
             *  and it will monitor the connections, and when any is ready with more input, it will assign the
             *  appropriate handler to handle the request, and produce the response.
             *
             *  TODO:
             *      \note Connection (and therefore also ConnectionManager) doesn't CURRENTLY support HTTP keepalives (https://stroika.atlassian.net/browse/STK-637)
             *      \note @todo https://stroika.atlassian.net/browse/STK-638 - WebServer ConnectionManager (and Connection) handling restructure - so while reading headers, only one thread used
             *
             *  \note Default ordering for interceptors:
             *        interceptors += earlyInterceptors;
             *        interceptors += beforeInterceptors;
             *        interceptors += router;
             *        interceptors += afterInterceptors;
             */
            class ConnectionManager {
            public:
                struct Options;
                static const Options kDefaultOptions;

            public:
                /**
                 */
                ConnectionManager (const SocketAddress& bindAddress, const Router& router, const Options& options = kDefaultOptions);
                ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Router& router, const Options& options = kDefaultOptions);
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
                 *  \note - this defaults to 2 seconds (kDefault_AutomaticTCPDisconnectOnClose)
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

            public:
                /**
                 *  Get the list of interceptors early interceptors. These default to:
                 *        earltInterceptors += ServerHeadersInterceptor_{serverHeader, corsSupportMode};
                 *        if (defaultFaultHandler) {
                 *          earltInterceptors += *defaultFaultHandler;
                 *        }
                 */
                nonvirtual Sequence<Interceptor> GetEarlyInterceptors () const;

            public:
                /**
                 * @see GetEarlyInterceptors
                 */
                nonvirtual void SetEarlyInterceptors (const Sequence<Interceptor>& earlyInterceptors);

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

            public:
                /**
                 *  These 'before' and 'after' values are releative to the router, which towards the end of the chain.
                 */
                enum InterceptorAddRelativeTo {
                    ePrependsToEarly,
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
                //nonvirtual void AddConnection (const shared_ptr<Connection>& conn);
                nonvirtual void AbortConnection (const shared_ptr<Connection>& conn);

            public:
                /**
                 *  We need some sort of status flag on connections - saying of they are OPEN or not - or done.
                 *  But this will return just those which are not 'done'. Of course - due to asyncrhony,
                 *  by the time one looks at the list, some may already be done.
                 */
                nonvirtual Collection<shared_ptr<Connection>> GetConnections () const;

#if 0
            private:
                //VERY VERY SUPER PRIMITIVE FIFRST DRAFT OF CONNECTION HANDLING
                nonvirtual  void    DoMainConnectionLoop_ ();
                nonvirtual  void    DoOneConnection_ (shared_ptr<Connection> c);
#endif

            private:
                nonvirtual void onConnect_ (const ConnectionOrientedSocket::Ptr& s);

            private:
                nonvirtual void FixupInterceptorChain_ ();
                nonvirtual void ReplaceInEarlyInterceptor_ (const Optional<Interceptor>& oldValue, const Optional<Interceptor>& newValue);

            private:
                Execution::Synchronized<Optional<String>>                    fServerHeader_;
                CORSModeSupport                                              fCORSModeSupport_{CORSModeSupport::eDEFAULT};
                Execution::Synchronized<Interceptor>                         fServerAndCORSEtcInterceptor_;
                Execution::Synchronized<Optional<Interceptor>>               fDefaultErrorHandler_;
                Execution::Synchronized<Sequence<Interceptor>>               fEarlyInterceptors_;
                Execution::Synchronized<Sequence<Interceptor>>               fBeforeInterceptors_;
                Execution::Synchronized<Sequence<Interceptor>>               fAfterInterceptors_;
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

                // Note: this must be declared after the threadpool so its shutdown on destruction before the thread pool, and doesn't try to launch
                // new tasks into an already destroyed threadpool.
                IO::Network::Listener                                                                                     fListener_;
                Execution::Synchronized<Bijection<shared_ptr<Connection>, Execution::WaitForIOReady::FileDescriptorType>> fActiveConnections_;
            };

            struct ConnectionManager::Options {
                Optional<unsigned int>      fMaxConnections;
                Optional<Socket::BindFlags> fBindFlags;
                Optional<String>            fServerHeader;
                Optional<CORSModeSupport>   fCORSModeSupport;

                /**
                 * @see Socket::SetAutomaticTCPDisconnectOnClose ()
                 */
                Optional<Time::DurationSecondsType> fAutomaticTCPDisconnectOnClose;

                /**
                 * @see Socket::SetLinger () - SO_LINGER
                 */
                Optional<int> fLinger;

                /**
                 * mostly for debugging - so easier to segrate names of threads if you have 
                 * multiple thread pools/connection managers
                 */
                Optional<String> fThreadPoolName;

                /**
                 *  The number of new TCP connections the kernel will buffer before the application has a chance to accept.
                 *
                 *  This can typically be left unset, and defaults to be based on fMaxConnections.
                 *
                 *  The default for tcp backlog is a little less than max # of connections. What makes
                 *  sense depends on ratio of incoming connections to the lifetime of those calls. If high, make this more than
                 *  number of connections. If low, then can be less than max# of connections.
                 *
                 *  @see http://man7.org/linux/man-pages/man2/listen.2.html
                 */
                Optional<unsigned int> fTCPBacklog;

#if qCompilerAndStdLib_DefaultCTORNotAutoGeneratedSometimes_Buggy
                Options (const Optional<unsigned int>& mc = {}, const Optional<Socket::BindFlags>& b = {}, const Optional<String>& sh = {}, const Optional<CORSModeSupport>& cors = {}, const Optional<Time::DurationSecondsType>& atcpc = {}, const Optional<int>& li = {}, const Optional<String>& threadPoolName = {})
                    : fMaxConnections (mc)
                    , fBindFlags (b)
                    , fServerHeader (sh)
                    , fCORSModeSupport (cors)
                    , fAutomaticTCPDisconnectOnClose (atcpc)
                    , fLinger (li)
                    , fThreadPoolName (threadPoolName)
                {
                }
#endif

                static constexpr unsigned int              kDefault_MaxConnections{10}; // temporarily - until we can fix https://stroika.atlassian.net/browse/STK-638
                static constexpr Socket::BindFlags         kDefault_BindFlags{};
                static const Optional<String>              kDefault_ServerHeader;
                static constexpr CORSModeSupport           kDefault_CORSModeSupport{CORSModeSupport::eDEFAULT};
                static constexpr Time::DurationSecondsType kDefault_AutomaticTCPDisconnectOnClose{2.0};
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

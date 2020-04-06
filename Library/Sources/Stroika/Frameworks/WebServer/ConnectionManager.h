/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ConnectionManager_h_
#define _Stroika_Framework_WebServer_ConnectionManager_h_ 1

#include "../StroikaPreComp.h"

#include <list>
#include <memory>

#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/Execution/Synchronized.h"
#include "../../Foundation/Execution/ThreadPool.h"
#include "../../Foundation/IO/Network/Listener.h"
#include "../../Foundation/IO/Network/SocketAddress.h"

#include "Connection.h"
#include "Request.h"
#include "Response.h"
#include "Router.h"

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Collection;
    using IO::Network::ConnectionOrientedStreamSocket;
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
        nonvirtual optional<String> GetServerHeader () const;

    public:
        /**
         */
        nonvirtual void SetServerHeader (optional<String> server);

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
        nonvirtual optional<int> GetLinger () const;

    public:
        /**
         *  @see GetLinger
         */
        nonvirtual void SetLinger (const optional<int>& linger);

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
        nonvirtual optional<Time::DurationSecondsType> GetAutomaticTCPDisconnectOnClose ();

    public:
        /**
         *  @see GetAutomaticTCPDisconnectOnClose ()
         */
        nonvirtual void SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSecondsType>& waitFor);

    public:
        /**
         *  This defaults to @DefaultFaultInterceptor, but can be set to 'missing' or any other fault handler. Not also - that
         *  all interceptors can engage in fault handling. This is just meant to provide a simple one-stop-shop for how to
         *  handle faults in one place.
         */
        nonvirtual optional<Interceptor> GetDefaultErrorHandler () const;

    public:
        /**
         *  @see GetDefaultErrorHandler
         */
        nonvirtual void SetDefaultErrorHandler (const optional<Interceptor>& defaultErrorHandler);

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

    public:
        nonvirtual void AbortConnection (const shared_ptr<Connection>& conn);

    public:
        /**
         *  We need some sort of status flag on connections - saying of they are OPEN or not - or done.
         *  But this will return just those which are not 'done'. Of course - due to asyncrhony,
         *  by the time one looks at the list, some may already be done.
         */
        nonvirtual Collection<shared_ptr<Connection>> GetConnections () const;

    private:
        nonvirtual void onConnect_ (const ConnectionOrientedStreamSocket::Ptr& s);

    private:
        nonvirtual void WaitForReadyConnectionLoop_ ();

    private:
        nonvirtual void FixupInterceptorChain_ ();

    private:
        nonvirtual void ReplaceInEarlyInterceptor_ (const optional<Interceptor>& oldValue, const optional<Interceptor>& newValue);

    private:
        Execution::Synchronized<optional<String>>                    fServerHeader_;
        CORSModeSupport                                              fCORSModeSupport_{CORSModeSupport::eDEFAULT};
        Execution::Synchronized<Interceptor>                         fServerAndCORSEtcInterceptor_;
        Execution::Synchronized<optional<Interceptor>>               fDefaultErrorHandler_;
        Execution::Synchronized<Sequence<Interceptor>>               fEarlyInterceptors_;
        Execution::Synchronized<Sequence<Interceptor>>               fBeforeInterceptors_;
        Execution::Synchronized<Sequence<Interceptor>>               fAfterInterceptors_;
        Execution::Synchronized<optional<int>>                       fLinger_;
        Execution::Synchronized<optional<Time::DurationSecondsType>> fAutomaticTCPDisconnectOnClose_;
        Router                                                       fRouter_;
        InterceptorChain                                             fInterceptorChain_; // no need to synchonize cuz internally synchronized

        // we may eventually want two thread pools - one for managing bookkeeping/monitoring harvests, and one for actually handling
        // connections. Or maybe a single thread for the bookkeeping, and the pool for handling ongoing connections?
        //
        // But for now - KISS
        //
        // Note - for now - we don't even handle servicing connections in the threadpool!!! - just one thread
        Execution::ThreadPool fActiveConnectionThreads_;

        // Note: this must be declared after the threadpool so its shutdown on destruction before the thread pool, and doesn't try to launch
        // new tasks into an already destroyed threadpool.
        IO::Network::Listener fListener_;
        // Inactive connections are those we are waiting (select/epoll) for incoming data
        Execution::RWSynchronized<Collection<shared_ptr<Connection>>> fInactiveOpenConnections_;
        // Active connections are those actively in the readheaders/readbody, dispatch/handle code
        Execution::RWSynchronized<Collection<shared_ptr<Connection>>> fActiveConnections_;
        Execution::Thread::CleanupPtr                                 fWaitForReadyConnectionThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};
    };

    struct ConnectionManager::Options {
        optional<unsigned int>      fMaxConnections;
        optional<Socket::BindFlags> fBindFlags;
        optional<String>            fServerHeader;
        optional<CORSModeSupport>   fCORSModeSupport;

        /**
         * @see Socket::SetAutomaticTCPDisconnectOnClose ()
         */
        optional<Time::DurationSecondsType> fAutomaticTCPDisconnectOnClose;

        /**
         * @see Socket::SetLinger () - SO_LINGER
         */
        optional<int> fLinger;

        /**
         * mostly for debugging - so easier to segrate names of threads if you have 
         * multiple thread pools/connection managers
         */
        optional<String> fThreadPoolName;

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
        optional<unsigned int> fTCPBacklog;

        static constexpr unsigned int              kDefault_MaxConnections{25}; // temporarily - until we can fix https://stroika.atlassian.net/browse/STK-638
        static constexpr Socket::BindFlags         kDefault_BindFlags{};
        static const optional<String>              kDefault_ServerHeader;
        static constexpr CORSModeSupport           kDefault_CORSModeSupport{CORSModeSupport::eDEFAULT};
        static constexpr Time::DurationSecondsType kDefault_AutomaticTCPDisconnectOnClose{2.0};
        static constexpr optional<int>             kDefault_Linger{};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionManager.inl"

#endif /*_Stroika_Framework_WebServer_ConnectionManager_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ConnectionManager_h_
#define _Stroika_Framework_WebServer_ConnectionManager_h_ 1

#include "../StroikaPreComp.h"

#include <list>
#include <memory>

#include "../../Foundation/Common/Property.h"
#include "../../Foundation/Containers/Collection.h"
#include "../../Foundation/Containers/Set.h"
#include "../../Foundation/Execution/Synchronized.h"
#include "../../Foundation/Execution/ThreadPool.h"
#include "../../Foundation/Execution/UpdatableWaitForIOReady.h"
#include "../../Foundation/Execution/VirtualConstant.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/Listener.h"
#include "../../Foundation/IO/Network/SocketAddress.h"

#include "CORS.h"
#include "Connection.h"
#include "Request.h"
#include "Response.h"
#include "Router.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Common::KeyValuePair;
    using Containers::Collection;
    using Containers::Set;
    using IO::Network::ConnectionOrientedStreamSocket;
    using IO::Network::HTTP::Headers;
    using IO::Network::Socket;
    using IO::Network::HTTP::Headers;
    using Traversal::Iterable;

    /**
     *  This class is a useful helper for managing a set of connections. You can start it and stop it
     *  (it maintains internal threads). And you can hand it Connections, along with a set of handlers,
     *  and it will monitor the connections, and when any is ready with more input, it will assign the
     *  appropriate handler to handle the request, and produce the response.
     *
     *  \note The connection manager supports HTTP keep-alives, to keep the connection open.
     *
     *  \note Default ordering for interceptors:
     *        interceptors += earlyInterceptors;
     *        interceptors += beforeInterceptors;
     *        interceptors += router;
     *        interceptors += afterInterceptors;
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     * 
     *  TODO:
     *      @todo We could allow updating most of these parameters. Some easily (like ServerHeader). Some would require stopping the
     *            connections, and rebuilding the list of threads etc (cuz must redo bindings). For now, KISS,
     *            and only allow modifications as needed.
     */
    class ConnectionManager {
    public:
        /**
         */
        struct Options {

            /**
             *  This is the max number of TCP connections the webserver will allow to keep around, before starting
             *  to reject new connections.
             * 
             *  \note NYI tracking and rejecting extra connections - just used as a hint for other values
             */
            optional<unsigned int> fMaxConnections;

            /**
             *  This is basically the number of threads to use. It can be automatically inferred from fMaxConnections
             *  but can be specified explicitly.
             */
            optional<unsigned int> fMaxConcurrentlyHandledConnections;

            /**
             *  If bindFlags omitted, it defaults to kDefault_BindFlags
             */
            optional<Socket::BindFlags> fBindFlags;

            /**
             *  fDefaultResponseHeaders may be used to specify initial{default} values for some HTTP headers.
             *  It can be used for specifying any non-standared HTTP headers to add to all responsed (like X-Foobar: value).
             *  It can be used to specify a default 'Server' header.
             *  This is equivilent to adding an interceptor early in the interceptor chain, and calling
             *      response.rwHeaders = *fDefaultResponseHeaders;
             * 
             *  Probably not a good idea to specify things like contentLength, etc here. That may produce bad results.
             *  @todo consider listing a stronger set of requirements for what headings can and cannot be set?)
             * 
             * This is also a good place to add defaults like:
             *      "Content-Type": DataExchange::InternetMediaTypes::kOctetStream (which was done automatically by Stroika before 2.1b10)
             */
            optional<Headers> fDefaultResponseHeaders;

            /**
             *  Options for how the HTTP Server handles CORS (mostly HTTP OPTIONS requests)
             */
            optional<CORSOptions> fCORS;

            /**
             *  This feature causes sockets to automatically flush their data - and avoid connection reset - when possible.
             *  This makes the closing socket process more costly and slow, so is optional, but is on by default because it makes
             *  commmunications more releable.
             *
             *  Turn this on - especially - if you see connection reset when clients talk to the Stroika web-server (TCP RST sent).
             *
             *  \note - this defaults to 2 seconds (kDefault_AutomaticTCPDisconnectOnClose)
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

            static constexpr unsigned int                               kDefault_MaxConnections{25};
            static constexpr Socket::BindFlags                          kDefault_BindFlags{};
            static inline const Headers                                 kDefault_Headers{Iterable<KeyValuePair<String, String>>{{IO::Network::HTTP::HeaderName::kServer, L"Stroika/2.1"sv}}};
            static inline const Execution::VirtualConstant<CORSOptions> kDefault_CORS{[] () { return kDefault_CORSOptions; }};
            static constexpr Time::DurationSecondsType                  kDefault_AutomaticTCPDisconnectOnClose{2.0};
            static constexpr optional<int>                              kDefault_Linger{nullopt}; // intentionally optional-valued
        };
        static const Options kDefaultOptions;

    public:
        /**
         */
        ConnectionManager (const SocketAddress& bindAddress, const Sequence<Route>& routes, const Options& options = kDefaultOptions);
        ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Sequence<Route>& routes, const Options& options = kDefaultOptions);
        ConnectionManager (const ConnectionManager&) = delete;
#if qDefaultTracingOn
        ~ConnectionManager ();
#else
        ~ConnectionManager () = default;
#endif

    public:
        nonvirtual ConnectionManager& operator= (const ConnectionManager&) = delete;

    public:
        /**
         *  Returns the 'effective' options after applying defaults, not (generally) the original options.
         */
        Common::ReadOnlyProperty<const Options&> options;

    public:
        /**
         *  This defaults to @DefaultFaultInterceptor, but can be set to 'missing' or any other fault handler. Not also - that
         *  all interceptors can engage in fault handling. This is just meant to provide a simple one-stop-shop for how to
         *  handle faults in one place.
         */
        Common::Property<optional<Interceptor>> defaultErrorHandler;

    public:
        /**
         *  Get the list of interceptors early interceptors. These default to:
         *      earltInterceptors += ServerHeadersInterceptor_{serverHeader, corsSupportMode};
         *      if (defaultFaultHandler) {
         *          earltInterceptors += *defaultFaultHandler;
         *      }
         * 
         *  @see beforeInterceptors, afterInterceptors, AddInterceptor, RemoveInterceptor to maintain the list of interceptors
         */
        Common::Property<Sequence<Interceptor>> earlyInterceptors;

    public:
        /**
         *  Get the list of interceptors before the private ConnectionManager interceptors (e.g. router).
         * 
         *  @see earlyInterceptors, afterInterceptors, AddInterceptor, RemoveInterceptor to maintain the list of interceptors
         */
        Common::Property<Sequence<Interceptor>> beforeInterceptors;

    public:
        /**
         *  Get the list of interceptors after the private ConnectionManager interceptors (e.g. router).
         * @see beforeInterceptors
         * 
         *  @see earlyInterceptors, beforeInterceptors, AddInterceptor, RemoveInterceptor to maintain the list of interceptors
         */
        Common::Property<Sequence<Interceptor>> afterInterceptors;

    public:
        /**
         *  These 'before' and 'after' values are releative to the router, which towards the end of the chain.
         */
        enum class InterceptorAddRelativeTo {
            ePrependsToEarly,
            ePrepend,
            eAppend,
            eAfterBeforeInterceptors,
        };
        static constexpr InterceptorAddRelativeTo ePrependsToEarly         = InterceptorAddRelativeTo::ePrependsToEarly;
        static constexpr InterceptorAddRelativeTo ePrepend                 = InterceptorAddRelativeTo::ePrepend;
        static constexpr InterceptorAddRelativeTo eAppend                  = InterceptorAddRelativeTo::eAppend;
        static constexpr InterceptorAddRelativeTo eAfterBeforeInterceptors = InterceptorAddRelativeTo::eAfterBeforeInterceptors;

    public:
        /**
         */
        nonvirtual void AddInterceptor (const Interceptor& i, InterceptorAddRelativeTo relativeTo);

    public:
        /**
         */
        nonvirtual void RemoveInterceptor (const Interceptor& i);

    public:
        /**
         */
        nonvirtual void AbortConnection (const shared_ptr<Connection>& conn);

    public:
        /**
         *  We need some sort of status flag on connections - saying of they are OPEN or not - or done.
         *  But this will return just those which are not 'done'. Of course - due to asyncrhony,
         *  by the time one looks at the list, some may already be done.
         */
        Common::ReadOnlyProperty<Collection<shared_ptr<Connection>>> connections;

    public:
        [[deprecated ("Since Stroika v2.1b10 use property connections()")]] Collection<shared_ptr<Connection>> GetConnections () const;
        [[deprecated ("Since Stroika v2.1b10 use property options()")]] Options GetOptions () const;

    private:
        nonvirtual void onConnect_ (const ConnectionOrientedStreamSocket::Ptr& s);

    private:
        nonvirtual void WaitForReadyConnectionLoop_ ();

    private:
        nonvirtual void FixupInterceptorChain_ ();

    private:
        // Inactive connections are those we are waiting (select/epoll) for incoming data; these are stored in fInactiveSockSetPoller_
        nonvirtual Collection<shared_ptr<Connection>> GetInactiveConnections_ () const;

    private:
        nonvirtual void ReplaceInEarlyInterceptor_ (const optional<Interceptor>& oldValue, const optional<Interceptor>& newValue);

    private:
        Options                                                      fEffectiveOptions_;
        Execution::Synchronized<optional<Interceptor>>               fDefaultErrorHandler_;
        Execution::Synchronized<Sequence<Interceptor>>               fEarlyInterceptors_;
        Execution::Synchronized<Sequence<Interceptor>>               fBeforeInterceptors_;
        Execution::Synchronized<Sequence<Interceptor>>               fAfterInterceptors_;
        Execution::Synchronized<optional<Time::DurationSecondsType>> fAutomaticTCPDisconnectOnClose_;
        Router                                                       fRouter_;
        InterceptorChain                                             fInterceptorChain_; // no need to synchonize cuz internally synchronized

        // Active connections are those actively in the readheaders/readbody, dispatch/handle code
        Execution::Synchronized<Collection<shared_ptr<Connection>>> fActiveConnections_;

        struct MyWaitForIOReady_Traits_ {
            using HighLevelType = shared_ptr<Connection>;
            static inline auto GetSDKPollable (const HighLevelType& t)
            {
                return t->socket ().GetNativeSocket ();
            }
        };
        // No need to lock fInactiveSockSetPoller_ since its internally synchronized;
        Execution::UpdatableWaitForIOReady<shared_ptr<Connection>, MyWaitForIOReady_Traits_> fInactiveSockSetPoller_{};

        /*
         *  SUBTLE DATA MEMBER ORDERING NOTE!
         *      We count on the THREADS that run and manipulate all the above data members are all listed AFTER those data
         *      members in this object. This is just for the covenient ordering that imposes on construction and destruction:
         *      the threads (declared below) are automatically shutdown on destruction before the data they reference (above)
         * 
         *      Same with the listener, as this is basically a thread invoking calls on the above data members.
         */

        // we may eventually want two thread pools - one for managing bookkeeping/monitoring harvests, and one for actually handling
        // connections. Or maybe a single thread for the bookkeeping, and the pool for handling ongoing connections?
        //
        // But for now - KISS
        //
        // Note - for now - we don't even handle 'accepting' connections in the threadpool!!! - just one thread
        Execution::ThreadPool fActiveConnectionThreads_;

        Execution::Thread::CleanupPtr fWaitForReadyConnectionThread_{Execution::Thread::CleanupPtr::eAbortBeforeWaiting};

        // Note: this must be declared after the threadpool so its shutdown on destruction before the thread pool, and doesn't try to launch
        // new tasks into an already destroyed threadpool.
        IO::Network::Listener     fListener_;
    };

    inline const ConnectionManager::Options ConnectionManager::kDefaultOptions;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionManager.inl"

#endif /*_Stroika_Framework_WebServer_ConnectionManager_h_*/

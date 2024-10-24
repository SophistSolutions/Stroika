/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ConnectionManager_h_
#define _Stroika_Framework_WebServer_ConnectionManager_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <list>
#include <memory>

#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/ThreadPool.h"
#include "Stroika/Foundation/Execution/UpdatableWaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/Listener.h"
#include "Stroika/Foundation/IO/Network/SocketAddress.h"

#include "Stroika/Frameworks/WebServer/CORS.h"
#include "Stroika/Frameworks/WebServer/Connection.h"
#include "Stroika/Frameworks/WebServer/Request.h"
#include "Stroika/Frameworks/WebServer/Response.h"
#include "Stroika/Frameworks/WebServer/Router.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using Characters::String;
    using Common::KeyValuePair;
    using Containers::Collection;
    using Containers::Set;
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
         *  \brief ConnectionManager::Options specify things like default headers, caching policies, binding flags (not bindings), thread count
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
             *  fDefaultResponseHeaders may be used to specify initial{default} values for some HTTP Response headers.
             *  It can be used for specifying any non-standard HTTP headers to add to all responded (like X-Foobar: value).
             *  It can be used to specify a default 'Server' header.
             *  This is equivalent to adding an interceptor early in the interceptor chain, and calling
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
             *  fDefaultGETResponseHeaders - like fDefaultResponseHeaders - may be used to specify initial{default} values for some HTTP headers,
             *  but it is only applied to GET requests (in addition to those applied from fDefaultResponseHeaders which applied to all responses).
             * 
             *  An example of something that makes sense to apply here would be Cache-Control settings (since these are unneeded on other HTTP methods
             *  typically).
             */
            optional<Headers> fDefaultGETResponseHeaders;

            /**
             *  Options for how the HTTP Server handles CORS (mostly HTTP OPTIONS requests)
             */
            optional<CORSOptions> fCORS;

            /**
             *  \brief sets the initial value for each Response. Harmless except for the slight performance cost (wont always work) - see Response::autoComputeETag
             *
             *  defaults to kDefault_AutoComputeETagResponse
             */
            optional<bool> fAutoComputeETagResponse;

            /**
             *  This feature causes sockets to automatically flush their data - and avoid connection reset - when possible.
             *  This makes the closing socket process more costly and slow, so is optional, but is on by default because it makes
             *  communications more reliable.
             *
             *  Turn this on - especially - if you see connection reset when clients talk to the Stroika web-server (TCP RST sent).
             *
             *  \note - this defaults to 2 seconds (kDefault_AutomaticTCPDisconnectOnClose)
             */
            optional<Time::DurationSeconds> fAutomaticTCPDisconnectOnClose;

            /**
             * @see Socket::SetLinger () - SO_LINGER
             */
            optional<int> fLinger;

            /**
             *  Is the TCP_NODELAY algorithm being used? Generally - this increases latency, for the benefit of better bandwidth utilization.
             * 
             *  See https://brooker.co.za/blog/2024/05/09/nagle.html for hints about if its right for you?
             */
            optional<bool> fTCPNoDelay;

            /**
             * mostly for debugging - so easier to segregate names of threads if you have 
             * multiple thread pools/connection managers
             */
            optional<String> fThreadPoolName;

            /**
             */
            bool fCollectStatistics{false};

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

            /**
             *  \brief controls whether to use Transfer-Coding: chunked or not
             * 
             *  \see WebServer::Connection::Options::fAutomaticTransferChunkSize
             *  \see WebServer::Response::automaticTransferChunkSize
             */
            optional<size_t> fAutomaticTransferChunkSize;

            /**
             *  \brief override the set of compression encodings the WebServer supports (default is all the Stroika implementation is built to support)
             * 
             *  \see WebServer::Connection::Options::fSupportedCompressionEncodings
             */
            optional<Containers::Set<HTTP::ContentEncoding>> fSupportedCompressionEncodings;

            static constexpr unsigned int      kDefault_MaxConnections{25};
            static constexpr Socket::BindFlags kDefault_BindFlags{};
            static inline const Headers kDefault_Headers{Iterable<KeyValuePair<String, String>>{{IO::Network::HTTP::HeaderName::kServer, "Stroika/3.0"sv}}};
            static inline const Common::ConstantProperty<CORSOptions> kDefault_CORS{[] () { return kDefault_CORSOptions; }};
            static constexpr bool                                     kDefault_AutoComputeETagResponse{true};
            static constexpr Time::DurationSeconds                    kDefault_AutomaticTCPDisconnectOnClose{2.0s};
            static constexpr optional<int>                            kDefault_Linger{nullopt}; // intentionally optional-valued
            static constexpr bool kDefault_TCPNoDelay{true}; // https://brooker.co.za/blog/2024/05/09/nagle.html (defaults to NO DELAY - disable Nagle - because we are carefully to only write once when the response is fully read)
        };
        static const Options kDefaultOptions;

    public:
        /**
         */
        ConnectionManager (const SocketAddress& bindAddress, const Sequence<Route>& routes, const Options& options = kDefaultOptions);
        ConnectionManager (const Traversal::Iterable<SocketAddress>& bindAddresses, const Sequence<Route>& routes, const Options& options = kDefaultOptions);
        ConnectionManager (const ConnectionManager&) = delete;
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        ~ConnectionManager ();
#else
        ~ConnectionManager () = default;
#endif

    public:
        nonvirtual ConnectionManager& operator= (const ConnectionManager&) = delete;

    public:
        /**
         *  Here active refers to being currently processed, reading data, writing data or computing answers. This means
         *  assigned into thread pool for handling.
         */
        Common::ReadOnlyProperty<Collection<shared_ptr<Connection>>> activeConnections;

    public:
        /**
         *  Get the list of interceptors after the private ConnectionManager interceptors (e.g. router).
         * @see beforeInterceptors
         * 
         *  @see earlyInterceptors, beforeInterceptors, AddInterceptor, RemoveInterceptor to maintain the list of interceptors
         * 
         *  \note ordering is  earlyInterceptors => beforeInterceptors => router => afterInterceptors;
         */
        Common::Property<Sequence<Interceptor>> afterInterceptors;

    public:
        /**
         *  Get the list of interceptors before the private ConnectionManager interceptors (e.g. router).
         * 
         *  @see earlyInterceptors, afterInterceptors, AddInterceptor, RemoveInterceptor to maintain the list of interceptors
         * 
         *  \note ordering is  earlyInterceptors => beforeInterceptors => router => afterInterceptors;
         */
        Common::Property<Sequence<Interceptor>> beforeInterceptors;

    public:
        /**
         *  \brief Return the socket addresses the webserver (connection manager) is listening on (to serve web content).
         */
        Common::ReadOnlyProperty<Traversal::Iterable<SocketAddress>> bindings;

    public:
        /**
         *  We need some sort of status flag on connections - saying of they are OPEN or not - or done.
         *  But this will return just those which are not 'done'. Of course - due to asynchrony,
         *  by the time one looks at the list, some may already be done.
         */
        Common::ReadOnlyProperty<Collection<shared_ptr<Connection>>> connections;

    public:
        /**
         *  This defaults to @DefaultFaultInterceptor, but can be set to 'missing' or any other fault handler. Not also - that
         *  all interceptors can engage in fault handling. This is just meant to provide a simple one-stop-shop for how to
         *  handle faults in one place.
         */
        Common::Property<optional<Interceptor>> defaultErrorHandler;

    public:
        /**
         *  Get/Set the list of interceptors early interceptors. These default to:
         *      earlyInterceptors += ServerHeadersInterceptor_{serverHeader, corsSupportMode};
         *      if (defaultFaultHandler) {
         *          earlyInterceptors += *defaultFaultHandler;
         *      }
         * 
         *  @see beforeInterceptors, afterInterceptors, AddInterceptor, RemoveInterceptor to maintain the list of interceptors
         * 
         *  \note ordering is  earlyInterceptors => beforeInterceptors => router => afterInterceptors;
         */
        Common::Property<Sequence<Interceptor>> earlyInterceptors;

    public:
        /**
         *  Returns the 'effective' options after applying defaults, not (generally) the original options.
         */
        Common::ReadOnlyProperty<const Options&> options;

    public:
        /**
         *  For now minimal, but perhaps expand....
         */
        struct Statistics {
            size_t                            fThreadPoolSize{};
            Execution::ThreadPool::Statistics fThreadPoolStatistics;

            /**
             *  See Characters::ToString ()
             */
            nonvirtual Characters::String ToString () const;
        };

    public:
        /**
         *  \req options.fCollectStatistics set on construction.
         * 
         *  Then this can be used to fetch the current thread pool statistics.
         */
        Common::ReadOnlyProperty<Statistics> statistics;

    public:
        /**
         *  These 'before' and 'after' values are relative to the router, which towards the end of the chain.
         * 
         *  \note ordering is  earlyInterceptors => beforeInterceptors => router => afterInterceptors;
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

    private:
        nonvirtual void DeriveConnectionDefaultOptionsFromEffectiveOptions_ ();

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
        Options                            fEffectiveOptions_;
        Traversal::Iterable<SocketAddress> fBindings_; // just to return bindings API
#if qCompilerAndStdLib_RecuriveTypeOrFunctionDependencyTooComplex_Buggy
        // BWA not too bad cuz ConnectionManager(const ConnectionManager&)=delete and op= as well.
        shared_ptr<Connection::Options> fUseDefaultConnectionOptions_BWA_{make_shared<Connection::Options> ()};
        Connection::Options&            fUseDefaultConnectionOptions_{*fUseDefaultConnectionOptions_BWA_};
#else
        Connection::Options fUseDefaultConnectionOptions_;
#endif
        Execution::Synchronized<optional<Interceptor>>           fDefaultErrorHandler_;
        Execution::Synchronized<Sequence<Interceptor>>           fEarlyInterceptors_;
        Execution::Synchronized<Sequence<Interceptor>>           fBeforeInterceptors_;
        Execution::Synchronized<Sequence<Interceptor>>           fAfterInterceptors_;
        Execution::Synchronized<optional<Time::DurationSeconds>> fAutomaticTCPDisconnectOnClose_;
        Router                                                   fRouter_;
        InterceptorChain fInterceptorChain_; // no need to synchronize cuz it's internally synchronized

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
         *      members in this object. This is just for the convenient ordering that imposes on construction and destruction:
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
        IO::Network::Listener fListener_;
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

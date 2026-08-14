/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Connection_h_
#define _Stroika_Framework_WebServer_Connection_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/HTTP/KeepAlive.h"
#include "Stroika/Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"
#include "Stroika/Foundation/IO/Network/SocketStream.h"
#include "Stroika/Foundation/Streams/TextToBinary.h"
#include "Stroika/Foundation/Time/DateTime.h"

#include "Stroika/Frameworks/WebServer/InterceptorChain.h"
#include "Stroika/Frameworks/WebServer/Message.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::IO;
    using namespace Stroika::Foundation::IO::Network;

    using Characters::String;
    using Time::TimePointSeconds;

    using Stroika::Foundation::IO::Network::HTTP::Headers;

    /**
     *  Write out files to %TEMP% dir, with logs of the details of the HTTP conversation, for debugging
     *  HTTP conversations.
     */
//#define qStroika_Framework_WebServer_Connection_DetailedMessagingLog 1
#ifndef qStroika_Framework_WebServer_Connection_DetailedMessagingLog
#define qStroika_Framework_WebServer_Connection_DetailedMessagingLog 0
#endif

    /**
     *  This has a slight cost, so you might want to compile it out of the implementation.
     *  At least useful when debugging.
     */
//#define qStroika_Framework_WebServer_Connection_TrackExtraStats 0
#ifndef qStroika_Framework_WebServer_Connection_TrackExtraStats
#define qStroika_Framework_WebServer_Connection_TrackExtraStats 1
#endif

    /**
     *  \brief  A Connection object represents the state (and socket) for an ongoing, active, HTTP Connection, managed by the ConnectionManager class
     *
     *  This tends to get used internally by the ConnectionManager, but you can use it directly. For example:
     *
     *  \par Example Usage
     *      \code
     *          Connection conn{acceptedSocketConnection,
     *                       Connection::Options{.fInterceptorChain = Sequence<Interceptor>{
     *                          Interceptor{
     *                              [=](Message* m) {
     *                                  Response& response = m->rwResponse ();
     *                                  response.rwHeaders().server = "stroika-ssdp-server-demo";
     *                                  response.write (Stroika::Frameworks::UPnP::Serialize (d, dd));
     *                                  response.contentType = DataExchange::InternetMediaTypes::kXML;
     *                               }}}}};
     *          conn.SetRemainingConnectionMessages (Connection::Remaining{0, 0}); // disable keep-alives
     *          conn.ReadAndProcessMessage ();
     *      \endcode
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Connection : Debug::AssertExternallySynchronizedChecker {
    public:
        struct Options {
            /**
             *  This is largely required (though can be provided later). Without it, you don't get notified about progress of the HTTP connection.
             */
            InterceptorChain fInterceptorChain;

            /**
             *  These are the default/baseline response headers which will be provided on all responses (except possibly GET)
             */
            Headers fDefaultResponseHeaders;

            /**
             *  These are the default response headers for GET requests (if not provided, defaults to fDefaultResponseHeaders)
             */
            optional<Headers> fDefaultGETResponseHeaders;

            /**
             *  \see WebServer::Response::autoComputeETagResponse
             */
            optional<bool> fAutoComputeETagResponse;

            /**
             *  \see WebServer::Response::automaticTransferChunkSize (default is usually fine)
             */
            optional<size_t> fAutomaticTransferChunkSize;

            /**
             *  \see WebServer::Request::bodyEncoding;
             *  This set is intersected with REQUEST.acceptEncoding headers to select encoding to use in Response bodyEncoding
             *  (if nullopt, auto-computed based on what is supported in Stroika)
             */
            optional<Containers::Set<HTTP::ContentEncoding>> fSupportedCompressionEncodings;
        };

    public:
        /**
         */
        Connection ()                  = delete;
        Connection (const Connection&) = delete;
        [[deprecated ("Since Stroika v3.0d7 - use the Options object with basically the same values")]] explicit Connection (
            const ConnectionOrientedStreamSocket::Ptr& s, const InterceptorChain& interceptorChain = {}, const Headers& defaultResponseHeaders = {},
            const optional<Headers>& defaultGETResponseHeaders = nullopt, const optional<bool> autoComputeETagResponse = nullopt);
        Connection (const ConnectionOrientedStreamSocket::Ptr& s, const Options& options);

    public:
        ~Connection ();

    public:
        nonvirtual Connection& operator= (const Connection&) = delete;

    public:
        /**
         *  This returns the (two way) connection oriented stream socket (ptr) used by this connection.
         */
        const Common::ReadOnlyProperty<ConnectionOrientedStreamSocket::Ptr> socket;

    public:
        /**
         *  Access a (read-only) reference of the underlying connection request
         */
        const Common::ReadOnlyProperty<const Request&> request;

    public:
        /**
         *  Access a (read-only) reference of the underlying connection request
         */
        const Common::ReadOnlyProperty<const Response&> response;

    public:
        /**
         *  Access a (read-only) reference to the underlying (modifiable) connection response (meaning you cannot assign to the response itself, but you can modify the response object)
         */
        Common::ReadOnlyProperty<Response&> rwResponse;

    public:
        /**
         *  Mostly for debugging, but also for ongoing operational diagnostics (late season debugging ;-)).
         * 
         *  \note these are stats about the state of this connection, not connections in general
         */
        struct Stats {
            /**
             *  Unique (at a given time) 'ID' which can be used to track the connection stats across calls to get stats.
             *      (note fUniqueID is opaque integer)
             */
            Socket::PlatformNativeHandle fSocketID;

            /**
             *  Connection being processed, with data present (if false, then connection in list of polled connections waiting for data to be available)
             */
            optional<bool> fActive;

            /**
             *  When the connection object was created
             */
            TimePointSeconds fCreatedAt;

#if qStroika_Framework_WebServer_Connection_TrackExtraStats
            /**
             * A given connection can be used for mutliple messages. Track what message number this is on this connection.
             *
             *  \note for weird cases (pathological?) - this can be larger than the actual message number. If you have several
             *        failed calls to ReadHeaders(), for example, which should basically never happen (except for abuse).
             */
            unsigned int fReadAndProcessMessageNumber{0};

            /**
             * @brief Experimental state information - dont count on details or names. Subject to change.
             *        This is intended to be used for debugging and diagnostics.
             */
            enum class State : uint8_t {
                /**
                 *  This means the socket was created in response to an Accept() returning, but we never got any bytes of data
                 *  available on the socket (so far).
                 */
                eNew,

                /**
                 *  In the middle of reading/parsing headers (data available, actively processing)
                 */
                eReadingHeaders,

                /**
                 *
                 */
                ePausedIncompleteHeaders,

                /**
                 * @brief done reading (header part), now processing the message (interceptor chain)
                 */
                eProcessingInterceptorChain,

                /**
                 * @brief Finished processing message, and in process of flushing response, and checking/updating response headers.
                 */
                eFlushing,

                /**
                 * @brief Re-using connection for next message (keep-alive).
                 */
                eReadyForNextMessage,

                /**
                 * @brief Done with connection and ready to close it down (maybe cleanly closing, or maybe with exception)
                 */
                eClosing,

                Stroika_Define_Enum_Bounds (eNew, eClosing)
            };
            State fState{State::eNew};

            /**
             */
            optional<Traversal::Range<TimePointSeconds>> fMostRecentMessage;

            /**
             */
            optional<thread::id> fHandlingThread;

            /**
             *  \brief the address of the client which is talking to the server
             */
            optional<SocketAddress> fRemotePeerAddress;

            /**
             *  \brief last request
             */
            optional<String> fRequestWebMethod;

            /**
             *  \brief last requested URI (always relative uri)
             */
            optional<URI> fRequestURI;
#endif

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

    public:
        /**
          * \brief retrieve stats about this connection, like threads used, start/end times. NB: INTERNALLY SYNCRONIZED
          * 
          *     \note   \em Thread-Safety   <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
          */
        const Common::ReadOnlyProperty<Stats> stats;

    public:
        /**
         */
        enum ReadAndProcessResult {
            eTryAgainLater, // Could mean success or some kinds of failure (like incomplete header/data), but try again later (so keep-alive results in this)
            eClose,
        };

    public:
        /**
         *  Return eTryAgainLater if 'keep alive' (or otherwise should try again - like incomplete input).
         */
        nonvirtual ReadAndProcessResult ReadAndProcessMessage () noexcept;

    public:
        /**
         *  \note set Remaining::fMessages := 0 to prevent keep-alives.
         *
         *  \par Example Usage
         *      \code
         *          conn.remainingConnectionLimits = KeepAlive{0, 0}; // disable keep-alives
         *      \endcode
         */
        Common::Property<optional<HTTP::KeepAlive>> remainingConnectionLimits;

#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    private:
        nonvirtual void WriteLogConnectionMsg_ (const String& msg) const;
#endif

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString (bool abbreviatedOutput = true) const;

    private:
        struct MyMessage_ : Message {
            MyMessage_ (const ConnectionOrientedStreamSocket::Ptr& socket, const Streams::InputOutputStream::Ptr<byte>& socketStream,
                        const Headers& defaultResponseHeaders, const optional<bool> autoComputeETagResponse);

            // Only valid until the end of a successful ReadHeaders
            HTTP::MessageStartTextInputStreamBinaryAdapter::Ptr fMsgHeaderInTextStream;

            // If result bad, throw exception
            enum ReadHeadersResult {
                eIncompleteButMoreMayBeAvailable,
                eIncompleteDeadEnd,
                eCompleteGood
            };
            nonvirtual ReadHeadersResult ReadHeaders (
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
                const function<void (const String&)>& logMsg
#endif
            );
        };

    private:
        const InterceptorChain                                 fInterceptorChain_;
        const Headers                                          fDefaultResponseHeaders_;
        const optional<Headers>                                fDefaultGETResponseHeaders_;
        const optional<bool>                                   fAutoComputeETagResponse_;
        const optional<Containers::Set<HTTP::ContentEncoding>> fSupportedCompressionEncodings_;
        const ConnectionOrientedStreamSocket::Ptr              fSocket_;
        Streams::InputOutputStream::Ptr<byte>                  fSocketStream_;
        const TimePointSeconds                                 fConnectionStartedAt_{};
        unique_ptr<MyMessage_>                                 fMessage_; // always there, but ptr so it can be replaced
        optional<HTTP::KeepAlive>                              fRemaining_;
#if qStroika_Framework_WebServer_Connection_TrackExtraStats
        enum class State_Flag_ : uint8_t {
            eNew,
            eReadingHeaders_Started,
            eFinishedReadingHeaders_Success,
            eFinishedReadingHeaders_Incomplete,
            eFinishedReadingHeaders_Failed,
            eInterceptorChain_Start,
            eInterceptorChain_Complete,
            eFlushing_Start,
            eFlushing_Done,
            eAborting,
        };
        atomic<State_Flag_> fState_{State_Flag_::eNew}; // always increases during a single ReadHeaders invocation (but it reversts between).
        atomic<unsigned int> fReadAndProcessMessageNumber_{0};
        atomic<bool>         fKeepAlive_{true};
        struct Stats2Capture_ {
            optional<TimePointSeconds> fMessageStart;
            optional<TimePointSeconds> fMessageCompleted;
            optional<SocketAddress>    fPeer;
            optional<String>           fWebMethod;
            optional<URI>              fRequestURI;
            optional<thread::id>       fHandlingThread;
        };
        static_assert (is_default_constructible_v<Stats2Capture_>);
        Execution::Synchronized<Stats2Capture_> fExtraStats_;
#endif
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
        Streams::OutputStream::Ptr<Character> fLogConnectionState_;
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Framework_WebServer_Connection_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Connection_h_
#define _Stroika_Framework_WebServer_Connection_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/IO/Network/HTTP/KeepAlive.h"
#include "Stroika/Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"
#include "Stroika/Foundation/IO/Network/SocketStream.h"
#include "Stroika/Foundation/Streams/TextWriter.h"

#include "InterceptorChain.h"
#include "Message.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::IO;
    using namespace Stroika::Foundation::IO::Network;

    using Characters::String;

    using Stroika::Foundation::IO::Network::HTTP::Headers;

    class ConnectionManager;

    /**
     *  Write out files to %TEMP% dir, with logs of the details of the HTTP conversation, for debugging
     *  HTTP conversations.
     */
//#define qStroika_Framework_WebServer_Connection_DetailedMessagingLog 1
#ifndef qStroika_Framework_WebServer_Connection_DetailedMessagingLog
#define qStroika_Framework_WebServer_Connection_DetailedMessagingLog 0
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
     *
     */
    class Connection : Debug::AssertExternallySynchronizedMutex {
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
        Common::ReadOnlyProperty<ConnectionOrientedStreamSocket::Ptr> socket;

    public:
        /**
         *  Access a (read-only) reference of the underlying connection request
         */
        Common::ReadOnlyProperty<const Request&> request;

    public:
        /**
         *  Access a (read-only) reference of the underlying connection request
         */
        Common::ReadOnlyProperty<const Response&> response;

    public:
        /**
         *  Access a (read-only) reference to the underlying (modifiable) connection response (meaning you cannot assign to the response itself, but you can modify the response object)
         */
        Common::ReadOnlyProperty<Response&> rwResponse;

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
        InterceptorChain                      fInterceptorChain_;
        Headers                               fDefaultResponseHeaders_;
        optional<Headers>                     fDefaultGETResponseHeaders_;
        optional<bool>                        fAutoComputeETagResponse_;
        ConnectionOrientedStreamSocket::Ptr   fSocket_;
        Streams::InputOutputStream::Ptr<byte> fSocketStream_;
        Time::TimePointSeconds                fConnectionStartedAt_{};
        unique_ptr<MyMessage_>                fMessage_; // always there, but ptr so it can be replaced
        optional<HTTP::KeepAlive>             fRemaining_;
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
        Streams::TextWriter::Ptr fLogConnectionState_;
#endif

    private:
        friend ConnectionManager;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Framework_WebServer_Connection_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Framework_WebService_SOAP_h_
#define _Stroika_Framework_WebService_SOAP_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <functional>
#include <stdexcept>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "Stroika/Frameworks/WebServer/Message.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

/**
 *  SOAP 1.1 (https://www.w3.org/TR/2000/NOTE-SOAP-20000508/) RPC: a client calls a method - a Request, POSTed to the server's URL
 *  - and the server answers with the method's out arguments, a Response, or says why it could not, a Fault. Each written
 *  (Serialize) and read (DeSerialize) here, and sent (Invoke) and answered (HandleRequest), so neither side writes or reads XML of
 *  its own.
 *
 *  Only what RPC with simple arguments uses: arguments as text, a fault's detail as XML text - no SOAP encoding of types, no
 *  headers, no SOAP 1.2. UPnP's control (Frameworks::UPnP::SOAP) is built on it.
 *
 *  Read with the XML DOM: in a build without an XML parser (qStroika_Foundation_DataExchange_XML_SupportDOM), DeSerialize - and
 *  so Invoke and HandleRequest - throw.
 *
 *  \par Example Usage
 *      \code
 *          // a client: call GetPrice
 *          SOAP::Response r = SOAP::Invoke (URI{"http://example.com/stock"sv}, "\"urn:example:stock#GetPrice\""sv,
 *                                           SOAP::Request{.fNamespace = "urn:example:stock"sv, .fMethod = "GetPrice"sv, .fArguments = {{"Symbol"sv, "IBM"sv}}});
 *          optional<String> price = r.LookupArgument ("Price"sv);
 *
 *          // the server: its URL's route
 *          Route{HTTP::MethodsRegEx::kPost, "stock"_RegEx, [] (Message& m) {
 *              SOAP::HandleRequest (m, [] (const SOAP::Request& request) {
 *                  if (request.fMethod != "GetPrice"sv) {
 *                      Execution::Throw (SOAP::FaultException{SOAP::Fault{.fFaultCode = SOAP::Fault::kClient, .fFaultString = "no such method"sv}});
 *                  }
 *                  return SOAP::Response{.fNamespace = request.fNamespace, .fMethod = request.fMethod, .fArguments = {{"Price"sv, "34.5"sv}}};
 *              });
 *          }}
 *      \endcode
 */
namespace Stroika::Frameworks::WebService::SOAP {

    using namespace Stroika::Foundation;

    using Characters::String;
    using Common::KeyValuePair;
    using Containers::Sequence;
    using DataExchange::InternetMediaType;
    using IO::Network::URI;

    /**
     *  What a SOAP 1.1 message is sent as: text/xml (section 6.1.1), UTF-8
     */
    inline const InternetMediaType kContentType{"text/xml; charset=\"utf-8\""sv};

    /**
     *  A method's arguments, each its name and value - in order, the order they are sent in (section 7.1). Values as text.
     */
    using Arguments = Sequence<KeyValuePair<String, String>>;

    /**
     *  \brief A call of a method (section 7.1): the Body's one element, named the method, in its namespace - its in arguments
     *         its elements
     */
    struct Request {
        /**
         *  The method's namespace (a URI)
         */
        String fNamespace;
        String fMethod;
        /**
         *  Its in arguments
         */
        Arguments fArguments;

        /**
         *  The value of the argument called name; nullopt if none is
         */
        nonvirtual optional<String> LookupArgument (const String& name) const;

        bool operator== (const Request&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief A call's answer, the method having been done (section 7.1): the Body's one element, named the method then Response
     *         - its out arguments its elements
     */
    struct Response {
        /**
         *  The method's namespace, as in the request
         */
        String fNamespace;
        /**
         *  The method, as in the request (written with Response after it)
         */
        String fMethod;
        /**
         *  Its out arguments
         */
        Arguments fArguments;

        /**
         *  The value of the argument called name; nullopt if none is
         */
        nonvirtual optional<String> LookupArgument (const String& name) const;

        bool operator== (const Response&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief A call's answer, the method having not been done: why not (section 4.4) - sent in an HTTP response whose status is
     *         500 (section 6.2).
     */
    struct Fault {
        /**
         *  The fault codes SOAP 1.1 gives (section 4.4.1): Client, the call's fault - it should not be made again as it is; Server,
         *  the server's - it may succeed later; ...
         */
        static inline const String kVersionMismatch{"VersionMismatch"sv};
        static inline const String kMustUnderstand{"MustUnderstand"sv};
        static inline const String kClient{"Client"sv};
        static inline const String kServer{"Server"sv};

        /**
         *  One of those - or one of them followed by its own, dotted: Client.Authentication, say. Without the envelope's prefix.
         */
        String fFaultCode{kServer};
        /**
         *  For a person
         */
        String fFaultString;
        /**
         *  The detail element's content - XML, as the application gives it (UPnP's UPnPError, say); nullopt for no detail.
         *  Written as given; read as the XML of its elements (a namespace prefix it uses but declares outside them is not
         *  carried: declare it within).
         */
        optional<String> fDetail;

        bool operator== (const Fault&) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief A Fault, thrown: Invoke throws the one a server answers with, and a HandleRequest handler throws one to answer with it.
     */
    class FaultException : public Execution::Exception<runtime_error> {
    public:
        /**
         */
        FaultException (const Fault& fault);

    public:
        /**
         */
        nonvirtual Fault GetFault () const;

    private:
        Fault fFault_;
    };

    /**
     *  \brief The SOAP message - its XML, UTF-8 - which sends it: a call, its answer, or a fault
     */
    Memory::BLOB Serialize (const Request& request);
    Memory::BLOB Serialize (const Response& response); ///< \brief The SOAP message - its XML, UTF-8 - which sends it
    Memory::BLOB Serialize (const Fault& fault);       ///< \brief The SOAP message - its XML, UTF-8 - which sends it

    /**
     *  \brief The SOAP message read: what Serialize writes, and any other's. Throws DataExchange::BadFormatException if it is not
     *         one - a Response read from a fault, say; and throws in a build with no XML parser.
     */
    void DeSerialize (const Memory::BLOB& b, Request* request);
    void DeSerialize (const Memory::BLOB& b, Response* response); ///< \brief The SOAP message read
    void DeSerialize (const Memory::BLOB& b, Fault* fault);       ///< \brief The SOAP message read

    /**
     *  \brief Call a method: POST request to url, with soapAction (in quotes, as SOAPAction headers are: section 6.1.1) - and
     *         return its answer. Throws FaultException if answered with a fault; another failed HTTP response as
     *         IO::Network::Transfer::Exception; an answer that is not SOAP as DataExchange::BadFormatException.
     */
    Response Invoke (const URI& url, const String& soapAction, const Request& request);

    /**
     *  \brief Answer a call, at its URL's route: 415 (Unsupported Media Type) if its body is not text/xml; else its Request read
     *         and answered with handler's Response - or, status 500, with the Fault handler throws (as FaultException), or a Client
     *         fault if the request is not a SOAP call.
     */
    void HandleRequest (WebServer::Message& m, const function<Response (const Request&)>& handler);

}

#endif /*_Stroika_Framework_WebService_SOAP_h_*/

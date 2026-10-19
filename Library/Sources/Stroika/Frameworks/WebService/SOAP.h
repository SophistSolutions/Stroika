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
#include "Stroika/Foundation/DataExchange/XML/Schema.h"
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
 *  Read and written with the XML DOM: in a build without one (qStroika_Foundation_DataExchange_XML_SupportDOM), Serialize and
 *  DeSerialize - and so Invoke and HandleRequest - throw, and there is no EnvelopeSchema.
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
         *
         *  \pre an XML name (XML::IsNCName: letters, digits, '.', '-', '_'), since Serialize writes it as a QName's local part.
         *       DeSerialize reads one that is not leniently, as its longest leading part that is a name ("Server Error" as
         *       Server), or Server - so a Fault read can always be written back; checked (Options::fValidateReadAgainst),
         *       the schema refuses it
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
     *  \brief Where Serialize writes SOAP's encodingStyle attribute (http://schemas.xmlsoap.org/soap/encoding/) - which SOAP
     *         1.1's prose says MAY go on any element, and on which its schema, its examples and the WS-I Basic Profile disagree
     *
     *      o   eOmitted - the default: legal by SOAP's schema, where the attribute is optional wherever it may go, and what
     *          the WS-I Basic Profile, which most stacks follow, requires (1.1, section 3.1.6: R1005 forbids it on SOAP's own
     *          elements, R1006 on a Body's children). The message then claims no encoding, and SOAP 1.1 defines no default
     *          (section 4.1.1), so its receiver goes by what it knows otherwise - a WSDL binding, or its protocol (UPnP 1.1
     *          takes a message with none to be SOAP-encoded)
     *      o   eOnBody - legal by SOAP's schema, but not by the WS-I Basic Profile
     *      o   eOnEnvelope - what SOAP 1.1's examples, and older RPC stacks, write, and what UPnP requires; SOAP's schema as
     *          published refuses it - EnvelopeSchema, corrected, does not - and so does the WS-I Basic Profile
     */
    enum class EncodingStyleAttribute {
        eOnBody,
        eOnEnvelope,
        eOmitted,
    };

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
    /**
     *  \brief SOAP 1.1's envelope schema, compiled by the provider o names - embedded, so nothing is fetched and no
     *         Resource::Resolver is wanted (it imports and includes nothing)
     *
     *  What it checks is the ENVELOPE: that Body holds what a Body may, that a Fault's children are the ones SOAP names
     *  and unqualified, that encodingStyle is qualified. A Body's own content is xs:any to it, so a message's payload is
     *  the caller's business, not this schema's.
     *
     *  By default CORRECTED, as SOAP 1.1 itself has it: its prose (section 4.1.1) says of encodingStyle "This attribute MAY
     *  appear on any element", and all six of its example messages put it on the Envelope - but its schema declares
     *  Envelope with <xs:anyAttribute namespace="##other">, which excludes SOAP's own namespace and so forbids exactly
     *  that. The correction is an <xs:attribute ref="tns:encodingStyle"/> on Envelope; nothing else is touched. So it
     *  accepts encodingStyle wherever a message carries it (@see EncodingStyleAttribute); allowEncodingStyleOnEnvelope
     *  false gives the schema as published, which refuses it on the Envelope.
     *
     *  Compiled afresh each call. Compiling is not free, so a caller checking many messages compiles one and holds it in
     *  its Options - which must not outlive the provider that made it.
     *
     *  It lives in a translation unit of its own, so its 6K is linked only into a program that calls it.
     *
     *  \note The schema is http://schemas.xmlsoap.org/soap/envelope/ - verbatim, but for that correction - under the W3C
     *        Software License, whose notice travels with it - @see Schemas/soap-envelope-1.1.xsd
     */
    DataExchange::XML::Schema::Ptr EnvelopeSchema (const DataExchange::XML::Options& o = {}, bool allowEncodingStyleOnEnvelope = true);
#endif

    /**
     *  \brief What SOAP must be told that has a sensible default - taken by Serialize and DeSerialize, defaulted, so a
     *         caller says only what it wants different
     *
     *  One Options for both, each field saying which it governs: fXMLOptions both, fValidateReadAgainst reading only,
     *  fEncodingStyleAttribute writing only.
     */
    struct Options {
        /**
         *  \brief How XML is read and written - by default, with Stroika's default provider; nearly always left as is (@see
         *         XML::Options)
         */
        DataExchange::XML::Options fXMLOptions;

        /**
         *  \brief Check a message being read against this schema - EnvelopeSchema - throwing where it does not hold;
         *         nullptr, the default, checks nothing
         *
         *  \pre compiled by the provider that reads: EnvelopeSchema (fXMLOptions)
         *
         *      \code
         *          // told early rather than half understanding a message
         *          const SOAP::Options kStrict_{.fValidateReadAgainst = SOAP::EnvelopeSchema ()};
         *      \endcode
         *
         *  Off by default: a reader that checked would refuse traffic other stacks send - each departs from SOAP in ways of
         *  its own - and whether to is the application's choice, not the library's. Check with EnvelopeSchema as it is by
         *  default, corrected: stacks follow SOAP 1.1's prose and examples in putting encodingStyle on the Envelope, which its
         *  schema as published forbids - Tests/54's SOAP_Others_, written from what other stacks emit, is two such messages,
         *  in two prefix styles, one carrying Microsoft's urn:schemas-microsoft-com:datatypes attributes.
         *
         *  Reading only - nothing checks what Serialize writes, because a message's data barely reaches what the schema
         *  checks. An envelope's structure - Envelope, Body, a Fault's children, where encodingStyle goes (@see
         *  fEncodingStyleAttribute) - is fixed by this code, and the schema leaves a message's content to its sender: Body
         *  and a Fault's detail are xs:any to it.
         *  The one exception is a Fault's code, written as a QName - so one that is not an XML name (with a space, say)
         *  writes a faultcode the schema refuses. Otherwise a run-time check would only re-check this code, which Tests/56's
         *  SOAP_ValidatesAgainstItsSchema_ does once.
         *
         *  \note A schema named here, rather than a flag that reaches for one, is what lets a program that never validates
         *        not carry it: nothing inside SOAP mentions EnvelopeSchema, so its 6K is linked only by a caller that asks
         *        (@see EnvelopeSchema).
         */
        DataExchange::XML::Schema::Ptr fValidateReadAgainst{nullptr};

        /**
         *  \brief Where Serialize writes encodingStyle - nowhere, the default, so what is written holds to SOAP's schema as
         *         published and to the WS-I Basic Profile (@see EncodingStyleAttribute). Writing only
         *
         *  DeSerialize reads a message whatever it carries - encodingStyle anywhere, or none; whether it may be where it
         *  is is for a schema to say (fValidateReadAgainst).
         */
        EncodingStyleAttribute fEncodingStyleAttribute{EncodingStyleAttribute::eOmitted};
    };

    /**
     *  \brief The SOAP message - its XML, UTF-8 - which sends it: a call, its answer, or a fault
     */
    Memory::BLOB Serialize (const Request& request, const Options& options = {});
    Memory::BLOB Serialize (const Response& response, const Options& options = {}); ///< \brief The SOAP message - its XML, UTF-8 - which sends it
    Memory::BLOB Serialize (const Fault& fault, const Options& options = {}); ///< \brief The SOAP message - its XML, UTF-8 - which sends it

    /**
     *  \brief The SOAP message read: what Serialize writes, and any other's. Throws DataExchange::BadFormatException if it is not
     *         one - a Response read from a fault, say; and throws in a build with no XML parser.
     */
    void DeSerialize (const Memory::BLOB& b, Request* request, const Options& options = {});
    void DeSerialize (const Memory::BLOB& b, Response* response, const Options& options = {}); ///< \brief The SOAP message read
    void DeSerialize (const Memory::BLOB& b, Fault* fault, const Options& options = {});       ///< \brief The SOAP message read

    /**
     *  \brief Call a method: POST request to url, with soapAction (in quotes, as SOAPAction headers are: section 6.1.1) - and
     *         return its answer. Throws FaultException if answered with a fault; another failed HTTP response as
     *         IO::Network::Transfer::Exception; an answer that is not SOAP as DataExchange::BadFormatException.
     */
    Response Invoke (const URI& url, const String& soapAction, const Request& request, const Options& options = {});

    /**
     *  \brief Answer a call, at its URL's route: 415 (Unsupported Media Type) if its body is not text/xml; else its Request read
     *         and answered with handler's Response - or, status 500, with the Fault handler throws (as FaultException), or a Client
     *         fault if the request is not a SOAP call (nor, with options.fValidateReadAgainst, a valid one).
     */
    void HandleRequest (WebServer::Message& m, const function<Response (const Request&)>& handler, const Options& options = {});

}

#endif /*_Stroika_Framework_WebService_SOAP_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/TypedBLOB.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#include "Stroika/Foundation/DataExchange/XML/WriterUtils.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/IO/Network/Transfer/Exception.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#if qStroika_Foundation_DataExchange_XML_SupportDOM
#include "Stroika/Foundation/DataExchange/XML/DOM.h"
#endif

#include "SOAP.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebService;
using namespace Stroika::Frameworks::WebService::SOAP;

namespace {
    constexpr string_view kEnvelopeNamespace_ = "http://schemas.xmlsoap.org/soap/envelope/"sv;

    optional<String> Lookup_ (const Arguments& arguments, const String& name)
    {
        for (const KeyValuePair<String, String>& a : arguments) {
            if (a.fKey == name) {
                return a.fValue;
            }
        }
        return nullopt;
    }

#if qStroika_Foundation_DataExchange_XML_SupportDOM
    namespace WriteElement = XML::DOM::Element;

    constexpr string_view  kEnvelopePrefix_ = "s"sv;
    constexpr string_view  kMethodPrefix_   = "m"sv;
    const IO::Network::URI kEnvelopeNamespaceURI_{String{kEnvelopeNamespace_}};
    const IO::Network::URI kEncodingNamespaceURI_{"http://schemas.xmlsoap.org/soap/encoding/"sv};

    /*
     *  The SOAP envelope round a body (section 4), and that Body to put the one element of the message in. One struct, with
     *  the Document FIRST, so the Body - a node of it - is destroyed before the document it points into.
     */
    struct Envelope_ {
        XML::DOM::Document::Ptr fDoc;
        WriteElement::Ptr       fBody;

        Envelope_ (const Options& options)
        {
            /*
             *  s: - the prefix everyone writes SOAP with - said at the root, so that it and everything under it are
             *  written with it, and not left to whatever a provider would invent. A Fault's code NAMES the prefix in
             *  text (@see Serialize (const Fault&)), so it has to be one we chose; and declaring it rather than a
             *  default namespace is what lets a Fault's unqualified children simply be unqualified, with no xmlns=""
             *  to escape a default that is not there.
             */
            fDoc = XML::DOM::Document::New (options.fXMLOptions);
            WriteElement::Ptr envelope =
                fDoc.ReplaceRootElement (XML::NameWithNamespace{kEnvelopeNamespaceURI_, "Envelope"sv}, String{kEnvelopePrefix_});
            fBody = envelope.Append ("Body"sv);
            // where options say - by default nowhere (@see EncodingStyleAttribute)
            const XML::NameWithNamespace kEncodingStyle_{kEnvelopeNamespaceURI_, "encodingStyle"sv};
            switch (options.fEncodingStyleAttribute) {
                case EncodingStyleAttribute::eOnBody:
                    fBody.SetAttribute (kEncodingStyle_, kEncodingNamespaceURI_.As<String> ());
                    break;
                case EncodingStyleAttribute::eOnEnvelope:
                    envelope.SetAttribute (kEncodingStyle_, kEncodingNamespaceURI_.As<String> ());
                    break;
                case EncodingStyleAttribute::eOmitted:
                    break;
            }
        }
        Memory::BLOB Write () const
        {
            Streams::MemoryStream::Ptr<std::byte> out = Streams::MemoryStream::New<std::byte> ();
            fDoc.Write (out);
            return out.As<Memory::BLOB> ();
        }
    };

    // a call's or answer's element: elementName, in the method's namespace, its arguments its elements (section 7.1) - which
    // are unqualified, where the method's namespace would otherwise be inherited
    void AppendMethodElement_ (WriteElement::Ptr body, const String& elementName, const String& methodNamespace, const Arguments& arguments)
    {
        // m: - what SOAP's own RPC examples call a method's namespace - declared before the element is made, so that
        // making it finds the prefix rather than inventing one
        body.SetAttribute (XML::NameWithNamespace{XML::kXMLNS.fNamespace, kMethodPrefix_}, methodNamespace);
        WriteElement::Ptr method = body.Append (XML::NameWithNamespace{IO::Network::URI{methodNamespace}, elementName});
        for (const KeyValuePair<String, String>& a : arguments) {
            method.Append (XML::NameWithNamespace{XML::kNoNamespace, a.fKey}, a.fValue);
        }
    }
#endif

#if qStroika_Foundation_DataExchange_XML_SupportDOM
    namespace Element = XML::DOM::Element;

    // f (the Body's one element - a call, an answer, or a fault), while its document is kept: its elements do not keep it
    template <typename RESULT>
    RESULT WithBodyElement_ (const Memory::BLOB& b, const Options& options, const function<RESULT (const Element::Ptr&)>& f)
    {
        using namespace XML::DOM;
        auto child = [] (const Element::Ptr& e, const optional<String>& name) -> Element::Ptr {
            for (const Element::Ptr& c : e.GetChildElements ()) {
                if (not name or c.GetName ().fName == *name) {
                    return c;
                }
            }
            return nullptr;
        };
        // a schema, where the caller named one, is checked AS it reads - so a message that does not hold throws here
        // rather than being half understood. Named, never reached for: @see Options::fValidateReadAgainst on why
        Document::Ptr doc = Document::New (b.As<Streams::InputStream::Ptr<std::byte>> (), options.fValidateReadAgainst, options.fXMLOptions);
        Element::Ptr envelope = doc.GetRootElement ();
        Element::Ptr body     = envelope == nullptr or envelope.GetName ().fName != "Envelope"sv ? nullptr : child (envelope, "Body"sv);
        Element::Ptr top      = body == nullptr ? nullptr : child (body, nullopt);
        if (top == nullptr) {
            Execution::Throw (BadFormatException{"not a SOAP message: no element in an Envelope's Body"sv});
        }
        return f (top);
    }
    String NamespaceOf_ (const Element::Ptr& e)
    {
        optional<URI> ns = e.GetName ().fNamespace;
        return ns ? ns->As<String> () : String{};
    }
    bool IsFault_ (const Element::Ptr& e)
    {
        return e.GetName ().fName == "Fault"sv and NamespaceOf_ (e) == kEnvelopeNamespace_;
    }
    // its child elements, each its name and text: a call's or answer's arguments
    Arguments ChildValues_ (const Element::Ptr& e)
    {
        Arguments result;
        for (const Element::Ptr& c : e.GetChildElements ()) {
            result.Append ({c.GetName ().fName, c.GetValue ()});
        }
        return result;
    }
#else
    [[noreturn]] void NoXMLDOM_ ()
    {
        Execution::Throw (Execution::Exception<runtime_error>{"SOAP messages cannot be read or written: this build has no XML DOM"sv});
    }
#endif
}

/*
 ********************************************************************************
 ************************* WebService::SOAP::Request ****************************
 ********************************************************************************
 */
optional<String> SOAP::Request::LookupArgument (const String& name) const
{
    return Lookup_ (fArguments, name);
}

String SOAP::Request::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Namespace: "sv << fNamespace;
    sb << ", Method: "sv << fMethod;
    sb << ", Arguments: "sv << fArguments;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************** WebService::SOAP::Response **************************
 ********************************************************************************
 */
optional<String> SOAP::Response::LookupArgument (const String& name) const
{
    return Lookup_ (fArguments, name);
}

String SOAP::Response::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Namespace: "sv << fNamespace;
    sb << ", Method: "sv << fMethod;
    sb << ", Arguments: "sv << fArguments;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 **************************** WebService::SOAP::Fault ***************************
 ********************************************************************************
 */
String SOAP::Fault::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Fault-Code: "sv << fFaultCode;
    sb << ", Fault-String: "sv << fFaultString;
    if (fDetail) {
        sb << ", Detail: "sv << *fDetail;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************ WebService::SOAP::FaultException **********************
 ********************************************************************************
 */
FaultException::FaultException (const Fault& fault)
    : Execution::Exception<runtime_error>{"SOAP fault {}: {}"_f(fault.fFaultCode, fault.fFaultString)}
    , fFault_{fault}
{
}

Fault FaultException::GetFault () const
{
    return fFault_;
}

/*
 ********************************************************************************
 ************************** WebService::SOAP::Serialize *************************
 ********************************************************************************
 */
Memory::BLOB SOAP::Serialize ([[maybe_unused]] const Request& request, [[maybe_unused]] const Options& options)
{
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    Envelope_ envelope{options};
    AppendMethodElement_ (envelope.fBody, request.fMethod, request.fNamespace, request.fArguments);
    return envelope.Write ();
#else
    NoXMLDOM_ ();
#endif
}

Memory::BLOB SOAP::Serialize ([[maybe_unused]] const Response& response, [[maybe_unused]] const Options& options)
{
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    Envelope_ envelope{options};
    AppendMethodElement_ (envelope.fBody, response.fMethod + "Response"sv, response.fNamespace, response.fArguments);
    return envelope.Write ();
#else
    NoXMLDOM_ ();
#endif
}

Memory::BLOB SOAP::Serialize ([[maybe_unused]] const Fault& fault, [[maybe_unused]] const Options& options)
{
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    Require (XML::IsNCName (fault.fFaultCode)); // it is written as a QName's local part - @see Fault::fFaultCode
    Envelope_         envelope{options};
    WriteElement::Ptr faultElt = envelope.fBody.Append ("Fault"sv);
    // SOAP 1.1 has a Fault's own children unqualified (section 4.4), where the envelope's namespace would be inherited
    // a QName, naming the envelope namespace through the prefix declared on it (section 4.4)
    faultElt.Append (XML::NameWithNamespace{XML::kNoNamespace, "faultcode"sv}, String{kEnvelopePrefix_} + ":"sv + fault.fFaultCode);
    faultElt.Append (XML::NameWithNamespace{XML::kNoNamespace, "faultstring"sv}, fault.fFaultString);
    if (fault.fDetail) {
        WriteElement::Ptr detail = faultElt.Append (XML::NameWithNamespace{XML::kNoNamespace, "detail"sv});
        // the detail is the application's own XML: parsed, so a malformed one is caught here rather than written out as a
        // malformed envelope, and imported as ELEMENTS - escaping it into text would change what the receiver reads
        // the SAME provider the envelope is being built with, since ImportNode requires that of both documents - with the
        // default here instead, a caller naming a provider got a failed Require rather than a fault
        XML::DOM::Document::Ptr detailDoc = XML::DOM::Document::New (*fault.fDetail, nullptr, options.fXMLOptions);
        detail.ImportNode (detailDoc.GetRootElement ());
    }
    return envelope.Write ();
#else
    NoXMLDOM_ ();
#endif
}

/*
 ********************************************************************************
 ************************* WebService::SOAP::DeSerialize ************************
 ********************************************************************************
 */
void SOAP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, Request* request, [[maybe_unused]] const Options& options)
{
    RequireNotNull (request);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    *request = WithBodyElement_<Request> (b, options, [] (const Element::Ptr& e) {
        if (IsFault_ (e)) {
            Execution::Throw (BadFormatException{"not a SOAP call: a fault"sv});
        }
        return Request{.fNamespace = NamespaceOf_ (e), .fMethod = e.GetName ().fName, .fArguments = ChildValues_ (e)};
    });
#else
    NoXMLDOM_ ();
#endif
}

void SOAP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, Response* response, [[maybe_unused]] const Options& options)
{
    RequireNotNull (response);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    *response = WithBodyElement_<Response> (b, options, [] (const Element::Ptr& e) {
        const String name = e.GetName ().fName;
        if (IsFault_ (e) or not name.EndsWith ("Response"sv)) {
            Execution::Throw (BadFormatException{"not a SOAP call's answer: {}"_f(name)});
        }
        return Response{.fNamespace = NamespaceOf_ (e), .fMethod = name.SubString (0, name.size () - "Response"sv.size ()), .fArguments = ChildValues_ (e)};
    });
#else
    NoXMLDOM_ ();
#endif
}

void SOAP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, Fault* fault, [[maybe_unused]] const Options& options)
{
    RequireNotNull (fault);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    *fault = WithBodyElement_<Fault> (b, options, [] (const Element::Ptr& e) {
        if (not IsFault_ (e)) {
            Execution::Throw (BadFormatException{"not a SOAP fault: {}"_f(e.GetName ().fName)});
        }
        Fault result;
        for (const Element::Ptr& c : e.GetChildElements ()) {
            const String name = c.GetName ().fName;
            if (name == "faultcode"sv) {
                const String     code  = c.GetValue ().Trim ();
                optional<size_t> colon = code.Find (':');
                String           local = colon ? code.SubString (*colon + 1) : code; // its prefix is the envelope's, whatever it is
                if (not XML::IsNCName (local)) {
                    // not a QName, as a faultcode must be - read leniently (checked, the schema has refused it already): as its
                    // longest leading part that is a name, or Server, so that what is read can always be written back
                    size_t n = local.size ();
                    while (n > 0 and not XML::IsNCName (local.SubString (0, n))) {
                        --n;
                    }
                    const String readAs = n == 0 ? Fault::kServer : local.SubString (0, n);
                    DbgTrace ("faultcode '{}' is not a QName: read as '{}'"_f, code, readAs);
                    local = readAs;
                }
                result.fFaultCode = local;
            }
            else if (name == "faultstring"sv) {
                result.fFaultString = c.GetValue ();
            }
            else if (name == "detail"sv) {
                StringBuilder sb;
                for (const Element::Ptr& d : c.GetChildElements ()) {
                    sb << d.ToString ();
                }
                result.fDetail = sb.str ();
            }
        }
        return result;
    });
#else
    NoXMLDOM_ ();
#endif
}

/*
 ********************************************************************************
 **************************** WebService::SOAP::Invoke **************************
 ********************************************************************************
 */
SOAP::Response SOAP::Invoke (const URI& url, const String& soapAction, const Request& request, const Options& options)
{
    Transfer::Connection::Ptr c = Transfer::Connection::New ();
    c.SetSchemeAndAuthority (url.GetSchemeAndAuthority ());
    Transfer::Request r;
    r.fMethod               = HTTP::Methods::kPost;
    r.fAuthorityRelativeURL = url.GetAuthorityRelativeResource<URI> ();
    r.fOverrideHeaders      = Containers::Mapping<String, String>{{String{HTTP::HeaderName::kSOAPAction}, soapAction}};
    r.SetTypedBLOB (TypedBLOB{.fData = Serialize (request, options), .fType = kContentType});
    Transfer::Response answer = c.Send (r); // not SendAndThrowOnFailure: a fault's answer - status 500 - says what the fault was
    if (answer.GetSucceeded ()) {
        Response response;
        DeSerialize (answer.GetData (), &response, options);
        return response;
    }
    if (answer.GetStatus () == HTTP::StatusCodes::kInternalError) {
        optional<Fault> fault;
        try {
            Fault f;
            DeSerialize (answer.GetData (), &f, options);
            fault = f;
        }
        catch (const BadFormatException&) {
            // a 500 that says nothing SOAP: as any other failed answer
        }
        if (fault) {
            Execution::Throw (FaultException{*fault});
        }
    }
    Execution::Throw (Transfer::Exception{answer});
}

/*
 ********************************************************************************
 ************************ WebService::SOAP::HandleRequest ***********************
 ********************************************************************************
 */
void SOAP::HandleRequest (WebServer::Message& m, const function<Response (const Request&)>& handler, const Options& options)
{
    static const InternetMediaType kTextXML_{"text/xml"sv};
    WebServer::Response&           response = m.rwResponse ();
    // its body must be text/xml (with any charset), or it is refused: 415
    if (optional<InternetMediaType> ct = m.request ().headers ().contentType (); not ct or not InternetMediaTypeRegistry::sThe->IsA (kTextXML_, *ct)) {
        response.status = HTTP::StatusCodes::kUnsupportedMediaType;
        return;
    }
    response.contentType = kContentType;
    auto answerFault     = [&] (const Fault& fault) {
        response.status = HTTP::StatusCodes::kInternalError; // how SOAP says it failed: section 6.2
        response.write (Serialize (fault, options));
    };
    Request request;
    try {
        DeSerialize (m.rwRequest ().GetBody (), &request, options);
    }
    catch (const BadFormatException& e) {
        answerFault (Fault{.fFaultCode = Fault::kClient, .fFaultString = "not a SOAP call: {}"_f(e)});
        return;
    }
    try {
        response.write (Serialize (handler (request), options));
    }
    catch (const FaultException& e) {
        answerFault (e.GetFault ());
    }
}

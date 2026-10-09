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
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"
#include "Stroika/Foundation/IO/Network/Transfer/Connection.h"
#include "Stroika/Foundation/IO/Network/Transfer/Exception.h"
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

    // the SOAP envelope round body (section 4)
    Memory::BLOB Envelope_ (const String& body)
    {
        const string utf8 =
            "<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"{}\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
            "<s:Body>{}</s:Body></s:Envelope>\r\n"_f(String{kEnvelopeNamespace_}, body)
                .AsUTF8<string> ();
        return Memory::BLOB{as_bytes (span{utf8})};
    }

    // a call's or answer's element: elementName, in the method's namespace, its arguments its elements (section 7.1)
    String MethodElement_ (const String& elementName, const String& methodNamespace, const Arguments& arguments)
    {
        StringBuilder sb;
        sb << "<m:"sv << elementName << " xmlns:m=\""sv << XML::QuoteForXMLAttributeW (methodNamespace) << "\">"sv;
        for (const KeyValuePair<String, String>& a : arguments) {
            sb << "<"sv << a.fKey << ">"sv << XML::QuoteForXMLW (a.fValue) << "</"sv << a.fKey << ">"sv;
        }
        sb << "</m:"sv << elementName << ">"sv;
        return sb;
    }

#if qStroika_Foundation_DataExchange_XML_SupportDOM
    namespace Element = XML::DOM::Element;

    // f (the Body's one element - a call, an answer, or a fault), while its document is kept: its elements do not keep it
    template <typename RESULT>
    RESULT WithBodyElement_ (const Memory::BLOB& b, const function<RESULT (const Element::Ptr&)>& f)
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
        Document::Ptr doc      = Document::New (b.As<Streams::InputStream::Ptr<std::byte>> ());
        Element::Ptr  envelope = doc.GetRootElement ();
        Element::Ptr  body     = envelope == nullptr or envelope.GetName ().fName != "Envelope"sv ? nullptr : child (envelope, "Body"sv);
        Element::Ptr  top      = body == nullptr ? nullptr : child (body, nullopt);
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
    [[noreturn]] void NoXMLParser_ ()
    {
        Execution::Throw (Execution::Exception<runtime_error>{"SOAP messages cannot be read: this build has no XML parser"sv});
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
Memory::BLOB SOAP::Serialize (const Request& request)
{
    return Envelope_ (MethodElement_ (request.fMethod, request.fNamespace, request.fArguments));
}

Memory::BLOB SOAP::Serialize (const Response& response)
{
    return Envelope_ (MethodElement_ (response.fMethod + "Response"sv, response.fNamespace, response.fArguments));
}

Memory::BLOB SOAP::Serialize (const Fault& fault)
{
    StringBuilder sb;
    sb << "<s:Fault><faultcode>s:"sv << XML::QuoteForXMLW (fault.fFaultCode) << "</faultcode><faultstring>"sv
       << XML::QuoteForXMLW (fault.fFaultString) << "</faultstring>"sv;
    if (fault.fDetail) {
        sb << "<detail>"sv << *fault.fDetail << "</detail>"sv;
    }
    sb << "</s:Fault>"sv;
    return Envelope_ (sb);
}

/*
 ********************************************************************************
 ************************* WebService::SOAP::DeSerialize ************************
 ********************************************************************************
 */
void SOAP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, Request* request)
{
    RequireNotNull (request);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    *request = WithBodyElement_<Request> (b, [] (const Element::Ptr& e) {
        if (IsFault_ (e)) {
            Execution::Throw (BadFormatException{"not a SOAP call: a fault"sv});
        }
        return Request{.fNamespace = NamespaceOf_ (e), .fMethod = e.GetName ().fName, .fArguments = ChildValues_ (e)};
    });
#else
    NoXMLParser_ ();
#endif
}

void SOAP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, Response* response)
{
    RequireNotNull (response);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    *response = WithBodyElement_<Response> (b, [] (const Element::Ptr& e) {
        const String name = e.GetName ().fName;
        if (IsFault_ (e) or not name.EndsWith ("Response"sv)) {
            Execution::Throw (BadFormatException{"not a SOAP call's answer: {}"_f(name)});
        }
        return Response{.fNamespace = NamespaceOf_ (e), .fMethod = name.SubString (0, name.size () - "Response"sv.size ()), .fArguments = ChildValues_ (e)};
    });
#else
    NoXMLParser_ ();
#endif
}

void SOAP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, Fault* fault)
{
    RequireNotNull (fault);
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    *fault = WithBodyElement_<Fault> (b, [] (const Element::Ptr& e) {
        if (not IsFault_ (e)) {
            Execution::Throw (BadFormatException{"not a SOAP fault: {}"_f(e.GetName ().fName)});
        }
        Fault result;
        for (const Element::Ptr& c : e.GetChildElements ()) {
            const String name = c.GetName ().fName;
            if (name == "faultcode"sv) {
                const String     code  = c.GetValue ().Trim ();
                optional<size_t> colon = code.Find (':');
                result.fFaultCode      = colon ? code.SubString (*colon + 1) : code; // its prefix is the envelope's, whatever it is
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
    NoXMLParser_ ();
#endif
}

/*
 ********************************************************************************
 **************************** WebService::SOAP::Invoke **************************
 ********************************************************************************
 */
SOAP::Response SOAP::Invoke (const URI& url, const String& soapAction, const Request& request)
{
    Transfer::Connection::Ptr c = Transfer::Connection::New ();
    c.SetSchemeAndAuthority (url.GetSchemeAndAuthority ());
    Transfer::Request r;
    r.fMethod               = HTTP::Methods::kPost;
    r.fAuthorityRelativeURL = url.GetAuthorityRelativeResource<URI> ();
    r.fOverrideHeaders      = Containers::Mapping<String, String>{{String{HTTP::HeaderName::kSOAPAction}, soapAction}};
    r.SetTypedBLOB (TypedBLOB{.fData = Serialize (request), .fType = kContentType});
    Transfer::Response answer = c.Send (r); // not SendAndThrowOnFailure: a fault's answer - status 500 - says what the fault was
    if (answer.GetSucceeded ()) {
        Response response;
        DeSerialize (answer.GetData (), &response);
        return response;
    }
    if (answer.GetStatus () == HTTP::StatusCodes::kInternalError) {
        optional<Fault> fault;
        try {
            Fault f;
            DeSerialize (answer.GetData (), &f);
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
void SOAP::HandleRequest (WebServer::Message& m, const function<Response (const Request&)>& handler)
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
        response.write (Serialize (fault));
    };
    Request request;
    try {
        DeSerialize (m.rwRequest ().GetBody (), &request);
    }
    catch (const BadFormatException& e) {
        answerFault (Fault{.fFaultCode = Fault::kClient, .fFaultString = "not a SOAP call: {}"_f(e)});
        return;
    }
    try {
        response.write (Serialize (handler (request)));
    }
    catch (const FaultException& e) {
        answerFault (e.GetFault ());
    }
}

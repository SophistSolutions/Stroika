/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <regex>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#include "Stroika/Foundation/DataExchange/XML/WriterUtils.h"
#include "Stroika/Foundation/Execution/Throw.h"
#if qStroika_Foundation_DataExchange_XML_SupportDOM
#include "Stroika/Foundation/DataExchange/XML/DOM.h"
#endif

#include "Stroika/Frameworks/UPnP/Private_/XMLText.h"

#include "Action.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SOAP;

namespace {
    optional<String> Lookup_ (const Arguments& arguments, const String& name)
    {
        for (const KeyValuePair<String, String>& a : arguments) {
            if (a.fKey == name) {
                return a.fValue;
            }
        }
        return nullopt;
    }

    // the SOAP envelope round body - each request and answer is one (UPnP Device Architecture 1.1, section 3.2)
    Memory::BLOB Envelope_ (const String& body)
    {
        const string utf8 = "<?xml version=\"1.0\"?>\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                            "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>{}</s:Body></s:Envelope>\r\n"_f(body)
                                .AsUTF8<string> ();
        return Memory::BLOB{as_bytes (span{utf8})};
    }

    // an action's element - the request's or response's: elementName, in the service type's namespace, its arguments its elements
    String ActionElement_ (const String& elementName, const String& serviceType, const Arguments& arguments)
    {
        StringBuilder sb;
        sb << "<u:"sv << elementName << " xmlns:u=\""sv << XML::QuoteForXMLAttributeW (serviceType) << "\">"sv;
        for (const KeyValuePair<String, String>& a : arguments) {
            sb << "<"sv << a.fKey << ">"sv << XML::QuoteForXMLW (a.fValue) << "</"sv << a.fKey << ">"sv;
        }
        sb << "</u:"sv << elementName << ">"sv;
        return sb;
    }

    // the SOAP body's element - an action's request or response, or a fault: its name and namespace, and its descendants that hold
    // text (not elements), in order - an action's arguments, or a fault's faultcode ... errorDescription - by name
    struct BodyElement_ {
        String    fName;
        String    fNamespace;
        Arguments fLeaves;
    };

#if qStroika_Foundation_DataExchange_XML_SupportDOM
    void Leaves_ (const XML::DOM::Element::Ptr& e, Arguments* leaves)
    {
        bool any = false;
        for (const XML::DOM::Element::Ptr& c : e.GetChildElements ()) {
            Leaves_ (c, leaves);
            any = true;
        }
        if (not any) {
            leaves->Append ({e.GetName ().fName, e.GetValue ()});
        }
    }
    BodyElement_ ParseBody_ (const Memory::BLOB& b)
    {
        using namespace XML::DOM;
        // the Envelope's Body, and its element
        auto child = [] (const Element::Ptr& e, const optional<String>& name) -> Element::Ptr {
            for (const Element::Ptr& c : e.GetChildElements ()) {
                if (not name or c.GetName ().fName == *name) {
                    return c;
                }
            }
            return nullptr;
        };
        Document::Ptr doc      = Document::New (b.As<Streams::InputStream::Ptr<std::byte>> ()); // kept: its elements do not keep it
        Element::Ptr  envelope = doc.GetRootElement ();
        Element::Ptr  body     = envelope == nullptr or envelope.GetName ().fName != "Envelope"sv ? nullptr : child (envelope, "Body"sv);
        Element::Ptr  top      = body == nullptr ? nullptr : child (body, nullopt);
        if (top == nullptr) {
            Execution::Throw (BadFormatException{"not a SOAP message: no element in an Envelope's Body"sv});
        }
        const XML::NameWithNamespace name = top.GetName ();
        BodyElement_                 result{.fName = name.fName, .fNamespace = name.fNamespace ? name.fNamespace->As<String> () : String{}};
        for (const Element::Ptr& c : top.GetChildElements ()) {
            Leaves_ (c, &result.fLeaves);
        }
        return result;
    }
#else
    // without an XML parser: as text - enough for the usual SOAP message (elements, attributes, namespaces and the entities of
    // QuoteForXML), though not for comments or CDATA
    BodyElement_ ParseBody_ (const Memory::BLOB& b)
    {
        const string text = b.As<string> ();
        // the Body's element: its prefix, name, attributes, and whether it is empty (/>)
        static const regex kTop_{R"(<(?:[\w.-]+:)?Body(?:\s[^>]*)?>\s*<(?:([\w.-]+):)?([\w.-]+)((?:\s[^>]*?)?)\s*(/?)>)"};
        smatch             top;
        if (not regex_search (text, top, kTop_)) {
            Execution::Throw (BadFormatException{"not a SOAP message: no element in an Envelope's Body"sv});
        }
        const string prefix = top[1].str ();
        const string name   = top[2].str ();
        BodyElement_ result{.fName = String::FromUTF8 (name)};
        // its namespace: its xmlns:prefix (or xmlns, if it has no prefix)
        const regex kNS{prefix.empty () ? string{R"(\sxmlns\s*=\s*["']([^"']*)["'])"} : R"(\sxmlns:)" + prefix + R"(\s*=\s*["']([^"']*)["'])"};
        const string attributes = top[3].str ();
        if (smatch ns; regex_search (attributes, ns, kNS)) {
            result.fNamespace = UPnP::Private_::XMLText (String::FromUTF8 (ns[1].str ()));
        }
        if (top[4].length () == 0) { // not empty: its content - up to its end tag - holds its leaves
            const size_t       start   = static_cast<size_t> (top.position (0) + top.length (0));
            const size_t       end     = text.find ("</" + (prefix.empty () ? name : prefix + ":" + name), start);
            const string       content = text.substr (start, end == string::npos ? string::npos : end - start);
            static const regex kLeaf_{R"(<(?:[\w.-]+:)?([\w.-]+)(?:\s[^>]*?)?(?:/>|>([^<]*)</(?:[\w.-]+:)?\1\s*>))"};
            for (sregex_iterator i{content.begin (), content.end (), kLeaf_}, e; i != e; ++i) {
                result.fLeaves.Append ({String::FromUTF8 ((*i)[1].str ()), UPnP::Private_::XMLText (String::FromUTF8 ((*i)[2].str ()))});
            }
        }
        return result;
    }
#endif
}

/*
 ********************************************************************************
 ************************** UPnP::SOAP::ActionRequest ***************************
 ********************************************************************************
 */
String ActionRequest::GetSOAPAction () const
{
    return "\"{}#{}\""_f(fServiceType, fAction);
}

optional<String> ActionRequest::LookupArgument (const String& name) const
{
    return Lookup_ (fArguments, name);
}

String ActionRequest::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Service-Type: "sv << fServiceType;
    sb << ", Action: "sv << fAction;
    sb << ", Arguments: "sv << fArguments;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************* UPnP::SOAP::ActionResponse ***************************
 ********************************************************************************
 */
optional<String> ActionResponse::LookupArgument (const String& name) const
{
    return Lookup_ (fArguments, name);
}

String ActionResponse::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Service-Type: "sv << fServiceType;
    sb << ", Action: "sv << fAction;
    sb << ", Arguments: "sv << fArguments;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 *************************** UPnP::SOAP::ActionError ****************************
 ********************************************************************************
 */
String ActionError::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Error-Code: "sv << fErrorCode;
    sb << ", Error-Description: "sv << fErrorDescription;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ****************************** UPnP::SOAP::Serialize ***************************
 ********************************************************************************
 */
Memory::BLOB SOAP::Serialize (const ActionRequest& request)
{
    return Envelope_ (ActionElement_ (request.fAction, request.fServiceType, request.fArguments));
}

Memory::BLOB SOAP::Serialize (const ActionResponse& response)
{
    return Envelope_ (ActionElement_ (response.fAction + "Response"sv, response.fServiceType, response.fArguments));
}

Memory::BLOB SOAP::Serialize (const ActionError& error)
{
    return Envelope_ ("<s:Fault><faultcode>s:Client</faultcode><faultstring>UPnPError</faultstring><detail>"
                      "<UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\"><errorCode>{}</errorCode><errorDescription>{}"
                      "</errorDescription></UPnPError></detail></s:Fault>"_f(error.fErrorCode, XML::QuoteForXMLW (error.fErrorDescription)));
}

/*
 ********************************************************************************
 ***************************** UPnP::SOAP::DeSerialize **************************
 ********************************************************************************
 */
void SOAP::DeSerialize (const Memory::BLOB& b, ActionRequest* request)
{
    RequireNotNull (request);
    BodyElement_ e = ParseBody_ (b);
    if (e.fName == "Fault"sv) {
        Execution::Throw (BadFormatException{"not an action's request: a fault"sv});
    }
    *request = ActionRequest{.fServiceType = e.fNamespace, .fAction = e.fName, .fArguments = e.fLeaves};
}

void SOAP::DeSerialize (const Memory::BLOB& b, ActionResponse* response)
{
    RequireNotNull (response);
    BodyElement_ e = ParseBody_ (b);
    if (not e.fName.EndsWith ("Response"sv)) {
        Execution::Throw (BadFormatException{"not an action's response: {}"_f(e.fName)});
    }
    *response = ActionResponse{
        .fServiceType = e.fNamespace, .fAction = e.fName.SubString (0, e.fName.size () - "Response"sv.size ()), .fArguments = e.fLeaves};
}

void SOAP::DeSerialize (const Memory::BLOB& b, ActionError* error)
{
    RequireNotNull (error);
    BodyElement_     e    = ParseBody_ (b);
    optional<String> code = e.fName == "Fault"sv ? Lookup_ (e.fLeaves, "errorCode"sv) : nullopt;
    if (not code) {
        Execution::Throw (BadFormatException{"not a UPnP error: no SOAP fault with an errorCode"sv});
    }
    *error = ActionError{.fErrorCode        = String2Int<unsigned int> (code->Trim ()),
                         .fErrorDescription = Lookup_ (e.fLeaves, "errorDescription"sv).value_or (String{})};
}

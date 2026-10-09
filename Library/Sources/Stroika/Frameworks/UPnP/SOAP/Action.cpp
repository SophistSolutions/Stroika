/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

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

#include "Action.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::SOAP;

namespace GenericSOAP_ = Stroika::Frameworks::WebService::SOAP;

namespace {
    optional<String> Lookup_ (const Arguments& arguments, const String& name)
    {
        for (const Common::KeyValuePair<String, String>& a : arguments) {
            if (a.fKey == name) {
                return a.fValue;
            }
        }
        return nullopt;
    }

    // a UPnPError's own namespace (UPnP Device Architecture 1.1, section 3.2.2)
    constexpr string_view kUPnPErrorNamespace_ = "urn:schemas-upnp-org:control-1-0"sv;

    // the errorDescription UPnP gives each of its error codes
    String StandardDescription_ (unsigned int errorCode)
    {
        switch (errorCode) {
            case ActionError::kInvalidAction:
                return "Invalid Action"sv;
            case ActionError::kInvalidArgs:
                return "Invalid Args"sv;
            case ActionError::kActionFailed:
                return "Action Failed"sv;
            case ActionError::kArgumentValueInvalid:
                return "Argument Value Invalid"sv;
            case ActionError::kArgumentValueOutOfRange:
                return "Argument Value Out of Range"sv;
            case ActionError::kOptionalActionNotImplemented:
                return "Optional Action Not Implemented"sv;
            case ActionError::kOutOfMemory:
                return "Out of Memory"sv;
            case ActionError::kHumanInterventionRequired:
                return "Human Intervention Required"sv;
            case ActionError::kStringArgumentTooLong:
                return "String Argument Too Long"sv;
            default:
                return String{};
        }
    }
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
GenericSOAP_::Fault ActionError::AsFault () const
{
    return GenericSOAP_::Fault{.fFaultCode   = GenericSOAP_::Fault::kClient,
                               .fFaultString = "UPnPError"sv,
                               .fDetail = "<UPnPError xmlns=\"{}\"><errorCode>{}</errorCode><errorDescription>{}</errorDescription></UPnPError>"_f(
                                   String{kUPnPErrorNamespace_}, fErrorCode, DataExchange::XML::QuoteForXMLW (fErrorDescription))};
}

optional<ActionError> ActionError::FromFault ([[maybe_unused]] const GenericSOAP_::Fault& fault)
{
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    if (not fault.fDetail) {
        return nullopt;
    }
    using namespace DataExchange::XML::DOM;
    // the detail's elements - perhaps more than one - in an element of their own, so a document
    Document::Ptr doc = Document::New ("<detail>{}</detail>"_f(*fault.fDetail)); // kept: its elements do not keep it
    for (const Element::Ptr& e : doc.GetRootElement ().GetChildElements ()) {
        if (e.GetName ().fName == "UPnPError"sv) {
            optional<String> code;
            ActionError      result;
            for (const Element::Ptr& c : e.GetChildElements ()) {
                if (c.GetName ().fName == "errorCode"sv) {
                    code = c.GetValue ().Trim ();
                }
                else if (c.GetName ().fName == "errorDescription"sv) {
                    result.fErrorDescription = c.GetValue ();
                }
            }
            if (code) {
                result.fErrorCode = String2Int<unsigned int> (*code);
                return result;
            }
        }
    }
#endif
    return nullopt;
}

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
 ************************* UPnP::SOAP::ActionException **************************
 ********************************************************************************
 */
ActionException::ActionException (const ActionError& error)
    : Execution::Exception<runtime_error>{"UPnP error {}: {}"_f(error.fErrorCode, error.fErrorDescription)}
    , fError_{error}
{
}

ActionException::ActionException (unsigned int errorCode)
    : ActionException{ActionError{.fErrorCode = errorCode, .fErrorDescription = StandardDescription_ (errorCode)}}
{
}

ActionError ActionException::GetError () const
{
    return fError_;
}

/*
 ********************************************************************************
 ****************************** UPnP::SOAP::Serialize ***************************
 ********************************************************************************
 */
Memory::BLOB SOAP::Serialize (const ActionRequest& request)
{
    return GenericSOAP_::Serialize (
        GenericSOAP_::Request{.fNamespace = request.fServiceType, .fMethod = request.fAction, .fArguments = request.fArguments});
}

Memory::BLOB SOAP::Serialize (const ActionResponse& response)
{
    return GenericSOAP_::Serialize (
        GenericSOAP_::Response{.fNamespace = response.fServiceType, .fMethod = response.fAction, .fArguments = response.fArguments});
}

Memory::BLOB SOAP::Serialize (const ActionError& error)
{
    return GenericSOAP_::Serialize (error.AsFault ());
}

/*
 ********************************************************************************
 ***************************** UPnP::SOAP::DeSerialize **************************
 ********************************************************************************
 */
void SOAP::DeSerialize (const Memory::BLOB& b, ActionRequest* request)
{
    RequireNotNull (request);
    GenericSOAP_::Request r;
    GenericSOAP_::DeSerialize (b, &r);
    *request = ActionRequest{.fServiceType = r.fNamespace, .fAction = r.fMethod, .fArguments = r.fArguments};
}

void SOAP::DeSerialize (const Memory::BLOB& b, ActionResponse* response)
{
    RequireNotNull (response);
    GenericSOAP_::Response r;
    GenericSOAP_::DeSerialize (b, &r);
    *response = ActionResponse{.fServiceType = r.fNamespace, .fAction = r.fMethod, .fArguments = r.fArguments};
}

void SOAP::DeSerialize (const Memory::BLOB& b, ActionError* error)
{
    RequireNotNull (error);
    GenericSOAP_::Fault f;
    GenericSOAP_::DeSerialize (b, &f);
    optional<ActionError> e = ActionError::FromFault (f);
    if (not e) {
        Execution::Throw (BadFormatException{"not a UPnP error: a fault with no UPnPError"sv});
    }
    *error = *e;
}

/*
 ********************************************************************************
 ******************************* UPnP::SOAP::Invoke *****************************
 ********************************************************************************
 */
ActionResponse SOAP::Invoke (const URI& controlURL, const ActionRequest& request)
{
    try {
        GenericSOAP_::Response r = GenericSOAP_::Invoke (
            controlURL, request.GetSOAPAction (),
            GenericSOAP_::Request{.fNamespace = request.fServiceType, .fMethod = request.fAction, .fArguments = request.fArguments});
        return ActionResponse{.fServiceType = r.fNamespace, .fAction = r.fMethod, .fArguments = r.fArguments};
    }
    catch (const GenericSOAP_::FaultException& e) {
        if (optional<ActionError> error = ActionError::FromFault (e.GetFault ())) {
            Execution::Throw (ActionException{*error});
        }
        Execution::ReThrow ();
    }
}

/*
 ********************************************************************************
 **************************** UPnP::SOAP::HandleAction **************************
 ********************************************************************************
 */
void SOAP::HandleAction (WebServer::Message& m, const String& serviceType, const function<Arguments (const ActionRequest&)>& doAction)
{
    GenericSOAP_::HandleRequest (m, [&] (const GenericSOAP_::Request& r) {
        try {
            if (r.fNamespace != serviceType) {
                Execution::Throw (ActionException{ActionError::kInvalidAction}); // another service's
            }
            return GenericSOAP_::Response{
                .fNamespace = r.fNamespace,
                .fMethod    = r.fMethod,
                .fArguments = doAction (ActionRequest{.fServiceType = r.fNamespace, .fAction = r.fMethod, .fArguments = r.fArguments})};
        }
        catch (const ActionException& e) {
            Execution::Throw (GenericSOAP_::FaultException{e.GetError ().AsFault ()});
        }
    });
}

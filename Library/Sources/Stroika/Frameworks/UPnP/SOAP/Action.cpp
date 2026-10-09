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
    // UPnP's arguments as SOAP has them - each its name and text - and back
    GenericSOAP_::Arguments ToGeneric_ (const Arguments& arguments)
    {
        GenericSOAP_::Arguments result;
        for (const Argument& a : arguments) {
            result.Append (a);
        }
        return result;
    }
    Arguments FromGeneric_ (const GenericSOAP_::Arguments& arguments)
    {
        Arguments result;
        for (const Common::KeyValuePair<String, String>& a : arguments) {
            result.Append (Argument{a});
        }
        return result;
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
        GenericSOAP_::Request{.fNamespace = request.fServiceType, .fMethod = request.fAction, .fArguments = ToGeneric_ (request.fArguments)});
}

Memory::BLOB SOAP::Serialize (const ActionResponse& response)
{
    return GenericSOAP_::Serialize (
        GenericSOAP_::Response{.fNamespace = response.fServiceType, .fMethod = response.fAction, .fArguments = ToGeneric_ (response.fArguments)});
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
    *request = ActionRequest{.fServiceType = r.fNamespace, .fAction = r.fMethod, .fArguments = FromGeneric_ (r.fArguments)};
}

void SOAP::DeSerialize (const Memory::BLOB& b, ActionResponse* response)
{
    RequireNotNull (response);
    GenericSOAP_::Response r;
    GenericSOAP_::DeSerialize (b, &r);
    *response = ActionResponse{.fServiceType = r.fNamespace, .fAction = r.fMethod, .fArguments = FromGeneric_ (r.fArguments)};
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
            GenericSOAP_::Request{.fNamespace = request.fServiceType, .fMethod = request.fAction, .fArguments = ToGeneric_ (request.fArguments)});
        return ActionResponse{.fServiceType = r.fNamespace, .fAction = r.fMethod, .fArguments = FromGeneric_ (r.fArguments)};
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
            return GenericSOAP_::Response{.fNamespace = r.fNamespace,
                                          .fMethod    = r.fMethod,
                                          .fArguments = ToGeneric_ (doAction (ActionRequest{
                                              .fServiceType = r.fNamespace, .fAction = r.fMethod, .fArguments = FromGeneric_ (r.fArguments)}))};
        }
        catch (const ActionException& e) {
            Execution::Throw (GenericSOAP_::FaultException{e.GetError ().AsFault ()});
        }
    });
}

void SOAP::HandleAction (WebServer::Message& m, const String& serviceType, const ServiceDescription& description,
                         const function<Arguments (const ActionRequest&)>& doAction)
{
    HandleAction (m, serviceType, [&] (const ActionRequest& request) {
        CheckRequest (request, description);
        return doAction (request);
    });
}

/*
 ********************************************************************************
 **************************** UPnP::SOAP::CheckRequest **************************
 ********************************************************************************
 */
namespace {
    // whether value is in range: each end read as value's data type, and compared as its C++ type - true if either end is not one
    bool InRange_ (const DataTypes::Value& value, const ServiceDescription::AllowedValueRange& range, DataTypes::DataType t)
    {
        optional<DataTypes::Value> lo = DataTypes::FromText (range.fMinimum, t);
        optional<DataTypes::Value> hi = DataTypes::FromText (range.fMaximum, t);
        if (not lo or not hi) {
            return true;
        }
        return visit (
            [&]<typename T> (const T& v) {
                if constexpr (is_arithmetic_v<T> and not same_as<T, bool>) {
                    return get<T> (*lo) <= v and v <= get<T> (*hi);
                }
                else {
                    return true; // a range is a number's (UPnP Device Architecture 1.1, section 2.5)
                }
            },
            value);
    }
}

void SOAP::CheckRequest (const ActionRequest& request, const ServiceDescription& description)
{
    using SD                    = ServiceDescription;
    optional<SD::Action> action = description.fActions.First ([&] (const SD::Action& a) { return a.fName == request.fAction; });
    if (not action) {
        Execution::Throw (ActionException{ActionError::kInvalidAction});
    }
    // its in arguments, in its order - each named as described, with a value of its state variable's type, and one it allows
    auto in = action->fArguments.Where ([] (const SD::Argument& a) { return a.fDirection == SD::Argument::Direction::eIn; });
    if (in.size () != request.fArguments.size ()) {
        Execution::Throw (ActionException{ActionError::kInvalidArgs});
    }
    auto given = request.fArguments.begin ();
    for (const SD::Argument& described : in) {
        const Argument a = *given;
        ++given;
        if (a.fKey != described.fName) {
            Execution::Throw (ActionException{ActionError::kInvalidArgs}); // misnamed, or out of order
        }
        optional<SD::StateVariable> v =
            description.fStateVariables.First ([&] (const SD::StateVariable& sv) { return sv.fName == described.fRelatedStateVariable; });
        optional<DataTypes::DataType> t = v ? Common::DefaultNames<DataTypes::DataType>{}.PeekValue (v->fDataType.As<wstring> ().c_str ()) : nullopt;
        if (not t) {
            continue; // a type not UPnP's - a vendor's - or none described: not checked
        }
        optional<DataTypes::Value> value = DataTypes::FromText (a.fValue, *t);
        if (not value) {
            Execution::Throw (ActionException{ActionError::kInvalidArgs});
        }
        if ((not v->fAllowedValues.empty () and not v->fAllowedValues.Contains (a.fValue)) or
            (v->fAllowedValueRange and not InRange_ (*value, *v->fAllowedValueRange, *t))) {
            Execution::Throw (ActionException{ActionError::kArgumentValueOutOfRange});
        }
    }
}

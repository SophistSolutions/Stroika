/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/StructuredStreamEvents/ObjectReader.h"
#include "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#include "Stroika/Foundation/DataExchange/XML/WriterUtils.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#if qStroika_Foundation_DataExchange_XML_SupportDOM
#include "Stroika/Foundation/DataExchange/XML/DOM.h"
#endif

#include "ServiceDescription.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

using SD = ServiceDescription;

namespace {
    // the namespace every element of an SCPD is in (UPnP Device Architecture 1.1, section 2.5)
    const IO::Network::URI kServiceNamespace_{"urn:schemas-upnp-org:service-1-0"sv};
}

/*
 ********************************************************************************
 ********************* UPnP::ServiceDescription::Argument ***********************
 ********************************************************************************
 */
String SD::Argument::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Name: "sv << fName;
    sb << ", Direction: "sv << (fDirection == Direction::eIn ? "in"sv : "out"sv);
    if (fRetval) {
        sb << ", Retval"sv;
    }
    sb << ", Related-State-Variable: "sv << fRelatedStateVariable;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ********************** UPnP::ServiceDescription::Action ************************
 ********************************************************************************
 */
String SD::Action::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Name: "sv << fName;
    sb << ", Arguments: "sv << fArguments;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 **************** UPnP::ServiceDescription::AllowedValueRange *******************
 ********************************************************************************
 */
String SD::AllowedValueRange::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Minimum: "sv << fMinimum;
    sb << ", Maximum: "sv << fMaximum;
    if (fStep) {
        sb << ", Step: "sv << *fStep;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ******************* UPnP::ServiceDescription::StateVariable ********************
 ********************************************************************************
 */
String SD::StateVariable::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Name: "sv << fName;
    sb << ", Data-Type: "sv << fDataType;
    if (fDefaultValue) {
        sb << ", Default-Value: "sv << *fDefaultValue;
    }
    sb << ", Send-Events: "sv << fSendEvents;
    if (not fAllowedValues.empty ()) {
        sb << ", Allowed-Values: "sv << fAllowedValues;
    }
    if (fAllowedValueRange) {
        sb << ", Allowed-Value-Range: "sv << *fAllowedValueRange;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************** UPnP::ServiceDescription ****************************
 ********************************************************************************
 */
String SD::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "Actions: "sv << fActions;
    sb << ", State-Variables: "sv << fStateVariables;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ********************************* UPnP::Serialize ******************************
 ********************************************************************************
 */
Memory::BLOB UPnP::Serialize ([[maybe_unused]] const ServiceDescription& sd, [[maybe_unused]] const Options& options)
{
#if qStroika_Foundation_DataExchange_XML_SupportDOM
    using namespace DataExchange::XML;
    namespace Element = DataExchange::XML::DOM::Element;
    // each element, in the order UPnP Device Architecture 1.1, section 2.5, gives them. A name with no namespace of its own
    // is in the document's, declared once on <scpd> and inherited - which is what UPnP's schema says of every one of these
    DOM::Document::Ptr doc  = DOM::Document::New (options.GetXMLProvider (), NameWithNamespace{kServiceNamespace_, "scpd"sv});
    Element::Ptr       scpd = doc.GetRootElement ();
    Element::Ptr       spec = scpd.Append ("specVersion"sv); // 1.0, not 1.1, on purpose - @see this framework's ReadMe.md
    spec.Append ("major"sv, "1"sv);
    spec.Append ("minor"sv, "0"sv);
    if (not sd.fActions.empty ()) {
        Element::Ptr actionList = scpd.Append ("actionList"sv);
        for (const SD::Action& a : sd.fActions) {
            Element::Ptr action = actionList.Append ("action"sv);
            action.Append ("name"sv, a.fName);
            if (not a.fArguments.empty ()) {
                Element::Ptr argumentList = action.Append ("argumentList"sv);
                for (const SD::Argument& arg : a.fArguments) {
                    Element::Ptr argument = argumentList.Append ("argument"sv);
                    argument.Append ("name"sv, arg.fName);
                    argument.Append ("direction"sv, String{arg.fDirection == SD::Argument::Direction::eIn ? "in"sv : "out"sv});
                    if (arg.fRetval) {
                        argument.Append ("retval"sv); // an empty element: UPnP says it by being there
                    }
                    argument.Append ("relatedStateVariable"sv, arg.fRelatedStateVariable);
                }
            }
        }
    }
    Element::Ptr serviceStateTable = scpd.Append ("serviceStateTable"sv);
    for (const SD::StateVariable& v : sd.fStateVariables) {
        Element::Ptr stateVariable = serviceStateTable.Append ("stateVariable"sv);
        stateVariable.SetAttribute ("sendEvents"sv, String{v.fSendEvents ? "yes"sv : "no"sv});
        stateVariable.Append ("name"sv, v.fName);
        stateVariable.Append ("dataType"sv, v.fDataType);
        stateVariable.AppendIf ("defaultValue"sv, v.fDefaultValue);
        if (not v.fAllowedValues.empty ()) {
            Element::Ptr allowedValueList = stateVariable.Append ("allowedValueList"sv);
            for (const String& av : v.fAllowedValues) {
                allowedValueList.Append ("allowedValue"sv, av);
            }
        }
        if (const optional<SD::AllowedValueRange>& r = v.fAllowedValueRange) {
            Element::Ptr allowedValueRange = stateVariable.Append ("allowedValueRange"sv);
            allowedValueRange.Append ("minimum"sv, r->fMinimum);
            allowedValueRange.Append ("maximum"sv, r->fMaximum);
            allowedValueRange.AppendIf ("step"sv, r->fStep);
        }
    }
    Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
    doc.Write (out, DOM::SerializationOptions{.fPrettyPrint = true});
    return out.As<Memory::BLOB> ();
#else
    Execution::Throw (Execution::Exception<runtime_error>{"A service description cannot be written: this build has no XML DOM"sv});
#endif
}

/*
 ********************************************************************************
 ******************************* UPnP::DeSerialize ******************************
 ********************************************************************************
 */
void UPnP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, ServiceDescription* sd, [[maybe_unused]] const Options& options)
{
    RequireNotNull (sd);
#if qStroika_Foundation_DataExchange_XML_SupportParsing
    using namespace DataExchange::StructuredStreamEvents;
    using Registry                                 = ObjectReader::Registry;
    static const ObjectReader::Registry kRegistry_ = [] () {
        Registry registry;
        registry.AddCommonType<String> ();
        registry.AddCommonType<optional<String>> ();
        registry.AddCommonReader_Simple<SD::Argument::Direction> (
            [] (const String& s) { return s.Trim () == "out"sv ? SD::Argument::Direction::eOut : SD::Argument::Direction::eIn; });
        registry.AddCommonReader_Class<SD::Argument> ({
            {Name{"name"sv}, &SD::Argument::fName},
            {Name{"direction"sv}, &SD::Argument::fDirection},
            {Name{"retval"sv}, &SD::Argument::fRetval, Registry::MakeCommonReader_Simple<bool> ([] (const String&) { return true; })}, // there: it is
            {Name{"relatedStateVariable"sv}, &SD::Argument::fRelatedStateVariable},
        });
        registry.AddCommonType<Sequence<SD::Argument>> (Name{"argument"sv});
        registry.AddCommonReader_Class<SD::Action> ({
            {Name{"name"sv}, &SD::Action::fName},
            {Name{"argumentList"sv}, &SD::Action::fArguments},
        });
        registry.AddCommonType<Sequence<SD::Action>> (Name{"action"sv});
        registry.AddCommonReader_Class<SD::AllowedValueRange> ({
            {Name{"minimum"sv}, &SD::AllowedValueRange::fMinimum},
            {Name{"maximum"sv}, &SD::AllowedValueRange::fMaximum},
            {Name{"step"sv}, &SD::AllowedValueRange::fStep},
        });
        registry.AddCommonType<optional<SD::AllowedValueRange>> ();
        registry.AddCommonType<Sequence<String>> (Name{"allowedValue"sv});
        registry.AddCommonReader_Class<SD::StateVariable> ({
            {Name{"sendEvents"sv, Name::eAttribute}, &SD::StateVariable::fSendEvents,
             Registry::MakeCommonReader_Simple<bool> ([] (const String& s) { return s.Trim () != "no"sv; })}, // yes, the default
            {Name{"name"sv}, &SD::StateVariable::fName},
            {Name{"dataType"sv}, &SD::StateVariable::fDataType},
            {Name{"defaultValue"sv}, &SD::StateVariable::fDefaultValue},
            {Name{"allowedValueList"sv}, &SD::StateVariable::fAllowedValues},
            {Name{"allowedValueRange"sv}, &SD::StateVariable::fAllowedValueRange},
        });
        registry.AddCommonType<Sequence<SD::StateVariable>> (Name{"stateVariable"sv});
        registry.AddCommonReader_Class<SD> ({
            {Name{"actionList"sv}, &SD::fActions},
            {Name{"serviceStateTable"sv}, &SD::fStateVariables},
        });
        return registry;
    }();
    ServiceDescription result;
    {
        ObjectReader::IConsumerDelegateToContext ctx{
            kRegistry_, Memory::MakeSharedPtr<ObjectReader::ReadDownToReader> (kRegistry_.MakeContextReader (&result), Name{"scpd"sv})};
        XML::SAXParse (options.GetXMLProvider (), b, &ctx, options.fValidateReadAgainst);
    }
    *sd = result;
#else
    Execution::Throw (Execution::Exception<runtime_error>{"A service description cannot be read: this build has no XML parser"sv});
#endif
}

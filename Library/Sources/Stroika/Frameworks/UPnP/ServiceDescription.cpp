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

#include "ServiceDescription.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

using SD = ServiceDescription;

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
Memory::BLOB UPnP::Serialize (const ServiceDescription& sd)
{
    using XML::QuoteForXMLW;
    // each element, in the order UPnP Device Architecture 1.1, section 2.5, gives them
    StringBuilder sb;
    sb << "<?xml version=\"1.0\"?>\r\n"sv;
    sb << "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n"sv;
    sb << "  <specVersion><major>1</major><minor>0</minor></specVersion>\r\n"sv;
    if (not sd.fActions.empty ()) {
        sb << "  <actionList>\r\n"sv;
        for (const SD::Action& a : sd.fActions) {
            sb << "    <action>\r\n"sv;
            sb << "      <name>"sv << QuoteForXMLW (a.fName) << "</name>\r\n"sv;
            if (not a.fArguments.empty ()) {
                sb << "      <argumentList>\r\n"sv;
                for (const SD::Argument& arg : a.fArguments) {
                    sb << "        <argument>\r\n"sv;
                    sb << "          <name>"sv << QuoteForXMLW (arg.fName) << "</name>\r\n"sv;
                    sb << "          <direction>"sv << (arg.fDirection == SD::Argument::Direction::eIn ? "in"sv : "out"sv) << "</direction>\r\n"sv;
                    if (arg.fRetval) {
                        sb << "          <retval/>\r\n"sv;
                    }
                    sb << "          <relatedStateVariable>"sv << QuoteForXMLW (arg.fRelatedStateVariable) << "</relatedStateVariable>\r\n"sv;
                    sb << "        </argument>\r\n"sv;
                }
                sb << "      </argumentList>\r\n"sv;
            }
            sb << "    </action>\r\n"sv;
        }
        sb << "  </actionList>\r\n"sv;
    }
    sb << "  <serviceStateTable>\r\n"sv;
    for (const SD::StateVariable& v : sd.fStateVariables) {
        sb << "    <stateVariable sendEvents=\""sv << (v.fSendEvents ? "yes"sv : "no"sv) << "\">\r\n"sv;
        sb << "      <name>"sv << QuoteForXMLW (v.fName) << "</name>\r\n"sv;
        sb << "      <dataType>"sv << QuoteForXMLW (v.fDataType) << "</dataType>\r\n"sv;
        if (v.fDefaultValue) {
            sb << "      <defaultValue>"sv << QuoteForXMLW (*v.fDefaultValue) << "</defaultValue>\r\n"sv;
        }
        if (not v.fAllowedValues.empty ()) {
            sb << "      <allowedValueList>\r\n"sv;
            for (const String& av : v.fAllowedValues) {
                sb << "        <allowedValue>"sv << QuoteForXMLW (av) << "</allowedValue>\r\n"sv;
            }
            sb << "      </allowedValueList>\r\n"sv;
        }
        if (const optional<SD::AllowedValueRange>& r = v.fAllowedValueRange) {
            sb << "      <allowedValueRange>\r\n"sv;
            sb << "        <minimum>"sv << QuoteForXMLW (r->fMinimum) << "</minimum>\r\n"sv;
            sb << "        <maximum>"sv << QuoteForXMLW (r->fMaximum) << "</maximum>\r\n"sv;
            if (r->fStep) {
                sb << "        <step>"sv << QuoteForXMLW (*r->fStep) << "</step>\r\n"sv;
            }
            sb << "      </allowedValueRange>\r\n"sv;
        }
        sb << "    </stateVariable>\r\n"sv;
    }
    sb << "  </serviceStateTable>\r\n"sv;
    sb << "</scpd>\r\n"sv;
    const string utf8 = sb.str ().AsUTF8<string> ();
    return Memory::BLOB{as_bytes (span{utf8})};
}

/*
 ********************************************************************************
 ******************************* UPnP::DeSerialize ******************************
 ********************************************************************************
 */
void UPnP::DeSerialize ([[maybe_unused]] const Memory::BLOB& b, ServiceDescription* sd)
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
        XML::SAXParse (b, &ctx);
    }
    *sd = result;
#else
    Execution::Throw (Execution::Exception<runtime_error>{"A service description cannot be read: this build has no XML parser"sv});
#endif
}

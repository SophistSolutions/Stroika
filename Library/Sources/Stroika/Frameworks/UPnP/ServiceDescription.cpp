/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Binding.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/XML/Writer.h"
#include "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "ServiceDescription.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

using Memory::MakeSharedPtr;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;

using SD_ = ServiceDescription;

namespace {
    // the namespace every element of an SCPD is in (UPnP Device Architecture 1.1, section 2.5)
    const IO::Network::URI kServiceNamespace_{"urn:schemas-upnp-org:service-1-0"sv};

    using VVMap_ = Containers::Mapping<String, VariantValue>;

    // a list UPnP's schema lets be missing, but not be empty - at least one item where there is one - is left out when it has none
    template <typename CLASS>
    function<void (const ObjectVariantMapper&, const CLASS*, VariantValue*)> LeaveOutIfEmpty_ (const String& member)
    {
        return [member] (const ObjectVariantMapper&, const CLASS*, VariantValue* v) {
            VVMap_ m = v->As<VVMap_> ();
            if (optional<VariantValue> list = m.Lookup (member); list and list->As<Containers::Sequence<VariantValue>> ().empty ()) {
                m.Remove (member);
                *v = VariantValue{m};
            }
        };
    }

    // a ServiceDescription's fields, by UPnP's names - and UPnP's spellings, and what its schema requires, each where it is added
    const ObjectVariantMapper& Mapper_ ()
    {
        static const ObjectVariantMapper kMapper_ = [] () {
            ObjectVariantMapper mapper;
            // a direction as UPnP spells it - in or out - not by the enum's own names
            mapper.Add<SD_::Argument::Direction> (
                [] (const ObjectVariantMapper&, const SD_::Argument::Direction* d) -> VariantValue {
                    return VariantValue{*d == SD_::Argument::Direction::eIn ? "in"sv : "out"sv};
                },
                [] (const ObjectVariantMapper&, const VariantValue& v, SD_::Argument::Direction* d) {
                    *d = v.As<String> ().Trim () == "out"sv ? SD_::Argument::Direction::eOut : SD_::Argument::Direction::eIn;
                });
            mapper.AddClass<SD_::Argument> ({
                {"name"sv, &SD_::Argument::fName},
                {"direction"sv, &SD_::Argument::fDirection},
                {"retval"sv, &SD_::Argument::fRetval},
                {"relatedStateVariable"sv, &SD_::Argument::fRelatedStateVariable},
            });
            mapper.AddCommonType<Containers::Sequence<SD_::Argument>> ();
            // an action with no arguments has no <argumentList>: UPnP's schema has no empty one
            mapper.AddClass<SD_::Action> ({{"name"sv, &SD_::Action::fName}, {"argumentList"sv, &SD_::Action::fArguments}},
                                          {.fAfterFrom = LeaveOutIfEmpty_<SD_::Action> ("argumentList"sv)});
            mapper.AddCommonType<Containers::Sequence<SD_::Action>> ();
            mapper.AddCommonType<optional<String>> ();
            mapper.AddClass<SD_::AllowedValueRange> ({{"minimum"sv, &SD_::AllowedValueRange::fMinimum},
                                                      {"maximum"sv, &SD_::AllowedValueRange::fMaximum},
                                                      {"step"sv, &SD_::AllowedValueRange::fStep}});
            mapper.AddCommonType<optional<SD_::AllowedValueRange>> ();
            mapper.AddCommonType<Containers::Sequence<String>> ();
            // sendEvents as UPnP spells it, yes or no - and yes, the default, read from anything but no
            const ObjectVariantMapper::TypeMappingDetails kYesNo{
                ObjectVariantMapper::FromObjectMapperType<bool> (
                    [] (const ObjectVariantMapper&, const bool* b) -> VariantValue { return VariantValue{*b ? "yes"sv : "no"sv}; }),
                ObjectVariantMapper::ToObjectMapperType<bool> (
                    [] (const ObjectVariantMapper&, const VariantValue& v, bool* b) { *b = v.As<String> ().Trim () != "no"sv; })};
            // a state variable with no allowed values has no <allowedValueList>, as an action with no arguments
            mapper.AddClass<SD_::StateVariable> (
                {
                    {"sendEvents"sv, &SD_::StateVariable::fSendEvents, kYesNo},
                    {"name"sv, &SD_::StateVariable::fName},
                    {"dataType"sv, &SD_::StateVariable::fDataType},
                    {"defaultValue"sv, &SD_::StateVariable::fDefaultValue},
                    {"allowedValueList"sv, &SD_::StateVariable::fAllowedValues},
                    {"allowedValueRange"sv, &SD_::StateVariable::fAllowedValueRange},
                },
                {.fAfterFrom = LeaveOutIfEmpty_<SD_::StateVariable> ("allowedValueList"sv)});
            mapper.AddCommonType<Containers::Sequence<SD_::StateVariable>> ();
            // a service with no actions has no <actionList>; and every SCPD has a specVersion, which no ServiceDescription field holds
            mapper.AddClass<SD_> ({{"actionList"sv, &SD_::fActions}, {"serviceStateTable"sv, &SD_::fStateVariables}},
                                  {.fAfterFrom = [] (const ObjectVariantMapper& m, const SD_* sd, VariantValue* v) {
                                      LeaveOutIfEmpty_<SD_> ("actionList"sv) (m, sd, v);
                                      VVMap_ vm = v->As<VVMap_> ();
                                      // 1.0, not 1.1, on purpose - @see this framework's ReadMe.md
                                      vm.Add ("specVersion"sv,
                                              VariantValue{VVMap_{{"major"sv, VariantValue{"1"sv}}, {"minor"sv, VariantValue{"0"sv}}}});
                                      *v = VariantValue{vm};
                                  }});
            return mapper;
        }();
        return kMapper_;
    }

    // the SCPD's shape (UPnP Device Architecture 1.1, section 2.5): its elements' names and order, UPnP's namespace, each list
    // and its items, sendEvents an attribute, <retval/> there or not - and what no SCPD field holds, a vendor's own elements
    // say, skipped
    //
    // @todo derive this from Mapper_ (https://github.com/SophistSolutions/Stroika/issues/1217), or from UPnP's own schema
    //       (ServiceDescriptionSchema), rather than spelling it out: what only XML says - item names, sendEvents an
    //       attribute, <retval/> a presence marker, the namespace, skipping - would be all that is left
    const Variant::XML::Binding& Binding_ ()
    {
        using Variant::XML::Binding;
        static const Binding kBinding_ = [] () {
            constexpr auto kSkip = Binding::UndeclaredMembers::eSkip;
            auto           text  = [] () { return Binding{.fType = VariantValue::eString}; };
            auto           list  = [] (const Binding& item) {
                return Binding{.fType              = VariantValue::eArray,
                               .fUndeclaredMembers = Binding::UndeclaredMembers::eSkip,
                               .fItems             = MakeSharedPtr<Binding> (item)};
            };
            Binding argument{.fName    = "argument"sv,
                             .fType    = VariantValue::eMap,
                             .fMembers = {{"name"sv, text ()},
                                          {"direction"sv, text ()},
                                          {"retval"sv, Binding{.fKind = Binding::Kind::ePresenceMarker, .fType = VariantValue::eBoolean}},
                                          {"relatedStateVariable"sv, text ()}},
                             .fUndeclaredMembers = kSkip};
            Binding action{.fName              = "action"sv,
                           .fType              = VariantValue::eMap,
                           .fMembers           = {{"name"sv, text ()}, {"argumentList"sv, list (argument)}},
                           .fUndeclaredMembers = kSkip};
            Binding allowedValue{.fName = "allowedValue"sv, .fType = VariantValue::eString};
            Binding stateVariable{
                .fName = "stateVariable"sv,
                .fType = VariantValue::eMap,
                .fMembers = {{"sendEvents"sv, Binding{.fKind = Binding::Kind::eAttribute, .fType = VariantValue::eString}},
                             {"name"sv, text ()},
                             {"dataType"sv, text ()},
                             {"defaultValue"sv, text ()},
                             {"allowedValueList"sv, list (allowedValue)},
                             {"allowedValueRange"sv, Binding{.fType = VariantValue::eMap,
                                                             .fMembers = {{"minimum"sv, text ()}, {"maximum"sv, text ()}, {"step"sv, text ()}},
                                                             .fUndeclaredMembers = kSkip}}},
                .fUndeclaredMembers = kSkip};
            Binding specVersion{.fType = VariantValue::eMap, .fMembers = {{"major"sv, text ()}, {"minor"sv, text ()}}, .fUndeclaredMembers = kSkip};
            Binding scpd{.fName = XML::NameWithNamespace{kServiceNamespace_, "scpd"sv},
                         .fType = VariantValue::eMap,
                         .fMembers = {{"specVersion"sv, specVersion}, {"actionList"sv, list (action)}, {"serviceStateTable"sv, list (stateVariable)}},
                         .fUndeclaredMembers = kSkip};
            Ensure (scpd.IsComplete ()); // so what Serialize writes, DeSerialize reads back exactly
            return scpd;
        }();
        return kBinding_;
    }
}

/*
 ********************************************************************************
 ********************* UPnP::ServiceDescription::Argument ***********************
 ********************************************************************************
 */
String SD_::Argument::ToString () const
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
String SD_::Action::ToString () const
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
String SD_::AllowedValueRange::ToString () const
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
String SD_::StateVariable::ToString () const
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
String SD_::ToString () const
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
Memory::BLOB UPnP::Serialize (const ServiceDescription& sd, [[maybe_unused]] const ServiceDescription::Options& options)
{
    // the Writer writes the text itself, with no XML provider - options' provider only checks it
    static const Variant::XML::Writer kWriter_{{.fBinding = Binding_ ()}};
    Memory::BLOB                      result = kWriter_.WriteAsBLOB (Mapper_ ().FromObject (sd));
#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
    if (options.fValidateWrite) {
        XML::SAXParse (result.As<Streams::InputStream::Ptr<byte>> (), nullptr, ServiceDescriptionSchema (options.fXMLOptions), nullptr,
                       options.fXMLOptions);
    }
#endif
    return result;
}

/*
 ********************************************************************************
 ******************************* UPnP::DeSerialize ******************************
 ********************************************************************************
 */
void UPnP::DeSerialize (const Memory::BLOB& b, ServiceDescription* sd, const ServiceDescription::Options& options)
{
    RequireNotNull (sd);
    // leniently: devices in the field are loose (@see DeviceDescription::Options::fValidateReadAgainst)
    const Variant::XML::Reader reader{
        {.fBinding = Binding_ (), .fXMLOptions = options.fXMLOptions, .fValidateAgainst = options.fValidateReadAgainst, .fLenient = true}};
    *sd = Mapper_ ().ToObject<ServiceDescription> (reader.Read (b.As<Streams::InputStream::Ptr<byte>> ()));
}

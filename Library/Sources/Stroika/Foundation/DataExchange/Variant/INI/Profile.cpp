/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/ToString.h"

#include "Profile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Variant;
using namespace Stroika::Foundation::DataExchange::Variant::INI;

/*
 ********************************************************************************
 ************************** DataExchange::INI::Section **************************
 ********************************************************************************
 */
String Section::ToString () const
{
    return "{ properties: " + Characters::ToString (fProperties) + "}";
}

/*
 ********************************************************************************
 ************************** DataExchange::INI::Profile **************************
 ********************************************************************************
 */
String Profile::ToString () const
{
    Characters::StringBuilder sb;
    sb += "{";
    if (not fUnnamedSection.fProperties.empty ()) {
        sb += "fUnnamedSection: " + Characters::ToString (fUnnamedSection) + ",";
    }
    if (not fNamedSections.empty ()) {
        sb += "fNamedSections: " + Characters::ToString (fNamedSections) + ",";
    }
    sb += "}";
    return sb.str ();
}

/*
 ********************************************************************************
 ************************** DataExchange::INI::Convert **************************
 ********************************************************************************
 */
Profile INI::Convert (const VariantValue& v)
{
    Profile                       profile;
    Mapping<String, VariantValue> mv = v.As<Mapping<String, VariantValue>> (); // throws if format mismatch
    for (const KeyValuePair<String, VariantValue>& kvi : mv) {
        if (kvi.fValue.GetType () == VariantValue::eMap) {
            Section newSection;
            for (const KeyValuePair<String, VariantValue>& namedSectionElt : kvi.fValue.As<Mapping<String, VariantValue>> ()) {
                newSection.fProperties.Add (namedSectionElt.fKey, namedSectionElt.fValue.As<String> ());
            }
            profile.fNamedSections.Add (kvi.fKey, newSection);
        }
        else {
            // goes into global unnamed section
            profile.fUnnamedSection.fProperties.Add (kvi.fKey, kvi.fValue.As<String> ());
        }
    }
    return profile;
}

VariantValue INI::Convert (const Profile& p)
{
    auto sec2VV = [] (const Section& s) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (const KeyValuePair<String, String>& k : s.fProperties) {
            m.Add (k.fKey, k.fValue);
        }
        return VariantValue{m};
    };
    Mapping<String, VariantValue> mv;
    for (const KeyValuePair<String, Section>& kvp : p.fNamedSections) {
        mv.Add (kvp.fKey, sec2VV (kvp.fValue));
    }
    for (const KeyValuePair<String, String>& kvp : p.fUnnamedSection.fProperties) {
        mv.Add (kvp.fKey, kvp.fValue);
    }
    return VariantValue{mv};
}

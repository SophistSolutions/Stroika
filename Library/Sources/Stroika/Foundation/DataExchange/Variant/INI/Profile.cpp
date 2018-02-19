/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/FloatConversion.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Streams/TextReader.h"
#include "../../BadFormatException.h"

#include "Profile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Variant;
using namespace Stroika::Foundation::DataExchange::Variant::INI;

using Characters::String_Constant;

Profile INI::Convert (VariantValue v)
{
    Profile                       profile;
    Mapping<String, VariantValue> mv = v.As<Mapping<String, VariantValue>> (); // throws if format mismatch
    for (KeyValuePair<String, VariantValue> kvi : mv) {
        if (kvi.fValue.GetType () == VariantValue::eMap) {
            Section newSection;
            for (KeyValuePair<String, VariantValue> namedSectionElt : kvi.fValue.As<Mapping<String, VariantValue>> ()) {
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

VariantValue INI::Convert (Profile p)
{
    auto sec2VV = [](Section s) -> VariantValue {
        Mapping<String, VariantValue> m;
        for (KeyValuePair<String, String> k : s.fProperties) {
            m.Add (k.fKey, k.fValue);
        }
        return VariantValue (m);
    };
    Mapping<String, VariantValue> mv;
    for (KeyValuePair<String, Section> kvp : p.fNamedSections) {
        mv.Add (kvp.fKey, sec2VV (kvp.fValue));
    }
    for (KeyValuePair<String, String> kvp : p.fUnnamedSection.fProperties) {
        mv.Add (kvp.fKey, kvp.fValue);
    }
    return VariantValue (mv);
}

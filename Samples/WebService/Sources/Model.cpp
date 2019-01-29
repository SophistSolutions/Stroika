/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Set.h"

#include "Model.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using namespace StroikaSample::WebServices;
using namespace StroikaSample::WebServices::Model;

DISABLE_COMPILER_MSC_WARNING_START (4573);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
const ObjectVariantMapper StroikaSample::WebServices::Model::kMapper = []() {
    ObjectVariantMapper mapper;

    // Read/Write real/complex numbers
    mapper.Add<Number> (
        []([[maybe_unused]] const ObjectVariantMapper& mapper, const Number* obj) -> VariantValue {
            Lambda_Arg_Unused_BWA (mapper);
            static const Float2StringOptions kFloat2StringOptions_{};
            StringBuilder                    sb;
            if (obj->real () != 0) {
                if (obj->imag () == 0) {
                    return obj->real (); // return a number in this case, not a string
                }
                sb += Float2String (obj->real (), kFloat2StringOptions_);
            }
            if (obj->imag () != 0) {
                if (not sb.empty ()) {
                    sb += ' ';
                }
                if (obj->imag () > 0 and sb.length () > 1) {
                    sb += L"+ ";
                }
                if (obj->imag () == 1) {
                    // skip
                }
                else if (obj->imag () == -1) {
                    sb += L"- "_k;
                }
                else {
                    sb += Float2String (obj->imag (), kFloat2StringOptions_);
                }
                sb += L"i"_k;
            }
            if (sb.empty ()) {
                return L"0"_k;
            }
            return sb.str ();
        },
        []([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& vv, Number* intoObj) -> void {
            Lambda_Arg_Unused_BWA (mapper);
            // Parse complex numbers of the form a + bi, handling special cases of a, and bi.
            // Trick: parse one number, and then accumulate second number (if any)
            // Works with a few minor rewrites
            String remainingNumber2Parse = vv.As<String> ();
            remainingNumber2Parse        = remainingNumber2Parse.ReplaceAll (L" "_k, String{}); // strip spaces - not needed and cause 2 + 4i to not parse (where 2+4i would) with simple trick we use (parse numbers and accum)
            Number accum{};
            for (unsigned int cnt = 0; cnt < 2 and not remainingNumber2Parse.empty (); ++cnt) {
                // special case rewrite bare 'i' as '1i' with +/- cases
                if (remainingNumber2Parse.StartsWith (L"i"_k)) {
                    remainingNumber2Parse = L"1i" + remainingNumber2Parse.Skip (1);
                }
                else if (remainingNumber2Parse.StartsWith (L"+i"_k)) {
                    remainingNumber2Parse = L"+1i" + remainingNumber2Parse.Skip (2);
                }
                else if (remainingNumber2Parse.StartsWith (L"-i"_k)) {
                    remainingNumber2Parse = L"-1i" + remainingNumber2Parse.Skip (2);
                }
                Number::value_type d = Characters::String2Float<Number::value_type> (remainingNumber2Parse, &remainingNumber2Parse);
                if (remainingNumber2Parse.StartsWith (L"i"_k)) {
                    accum += Number (0, d);
                    remainingNumber2Parse = remainingNumber2Parse.Skip (1);
                }
                else {
                    accum += Number{d};
                }
            }
            if (not remainingNumber2Parse.empty ()) {
                Execution::Throw (Execution::StringException (L"invalid complex number: " + vv.As<String> ()));
            }
            *intoObj = accum;
        });

    return mapper;
}();
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
DISABLE_COMPILER_MSC_WARNING_END (4573);

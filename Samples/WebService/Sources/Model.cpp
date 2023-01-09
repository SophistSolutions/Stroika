/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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

#if qCompilerAndStdLib_static_initializer_lambda_funct_init_Buggy
#error "You can easily rewrite this as a static function, but thats not great as a demo, so don't bother here - just dont use that compiler version"
#endif

const ObjectVariantMapper StroikaSample::WebServices::Model::kMapper = [] () {
    ObjectVariantMapper mapper;

    // Read/Write real/complex numbers
    mapper.Add<Number> (
        [] (const ObjectVariantMapper& mapper [[maybe_unused]], const Number* obj) -> VariantValue {
            static const FloatConversion::ToStringOptions kFloat2StringOptions_{};
            StringBuilder                                 sb;
            if (obj->real () != 0) {
                if (obj->imag () == 0) {
                    return obj->real (); // return a number in this case, not a string
                }
                sb += FloatConversion::ToString (obj->real (), kFloat2StringOptions_);
            }
            if (obj->imag () != 0) {
                if (not sb.empty ()) {
                    sb += ' ';
                }
                if (obj->imag () > 0 and sb.length () > 1) {
                    sb += "+ "_k;
                }
                if (obj->imag () == 1) {
                    // skip
                }
                else if (obj->imag () == -1) {
                    sb += "- "_k;
                }
                else {
                    sb += FloatConversion::ToString (obj->imag (), kFloat2StringOptions_);
                }
                sb += "i"_k;
            }
            if (sb.empty ()) {
                return "0"_k;
            }
            return sb.str ();
        },
        [] (const ObjectVariantMapper& mapper [[maybe_unused]], const VariantValue& vv, Number* intoObj) -> void {
            // Parse complex numbers of the form a + bi, handling special cases of a, and bi.
            // Trick: parse one number, and then accumulate second number (if any)
            // Works with a few minor rewrites
            String remainingNumber2Parse = vv.As<String> ();
            remainingNumber2Parse        = remainingNumber2Parse.ReplaceAll (" "_k, String{}); // strip spaces - not needed and cause 2 + 4i to not parse (where 2+4i would) with simple trick we use (parse numbers and accum)
            Number accum{};
            for (unsigned int cnt = 0; cnt < 2 and not remainingNumber2Parse.empty (); ++cnt) {
                // special case rewrite bare 'i' as '1i' with +/- cases
                if (remainingNumber2Parse.StartsWith ("i"_k)) {
                    static_assert (is_base_of_v<String, decltype (remainingNumber2Parse)>);
                    remainingNumber2Parse = "1i"_k + remainingNumber2Parse.Skip (1);
                }
                else if (remainingNumber2Parse.StartsWith ("+i"_k)) {
                    remainingNumber2Parse = "+1i"_k + remainingNumber2Parse.Skip (2);
                }
                else if (remainingNumber2Parse.StartsWith ("-i"_k)) {
                    remainingNumber2Parse = "-1i"_k + remainingNumber2Parse.Skip (2);
                }
                Number::value_type d = Characters::FloatConversion::ToFloat<Number::value_type> (remainingNumber2Parse, &remainingNumber2Parse);
                if (remainingNumber2Parse.StartsWith ("i"_k)) {
                    accum += Number{0, d};
                    remainingNumber2Parse = remainingNumber2Parse.Skip (1);
                }
                else {
                    accum += Number{d};
                }
            }
            if (not remainingNumber2Parse.empty ()) {
                Execution::Throw (Execution::Exception{"invalid complex number: "sv + vv.As<String> ()});
            }
            *intoObj = accum;
        });

    mapper.AddCommonType<Collection<String>> ();

    return mapper;
}();

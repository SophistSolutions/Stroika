/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

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
            StringBuilder sb;
            if (obj->real () != 0) {
                if (obj->imag () == 0) {
                    return obj->real (); // return a number in this case, not a string
                }
                sb += Characters::Format (L"%f", obj->real ());
            }
            if (obj->imag () != 0) {
                if (not sb.empty ()) {
                    sb += ' ';
                }
                if (obj->imag () > 0 and sb.length () > 1) {
                    sb += L"+";
                }
                sb += Characters::Format (L"%fi", obj->imag ());
            }
            if (sb.empty ()) {
                return L"0"_k;
            }
            return sb.str ();
        },
        []([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, Number* intoObj) -> void {
            //tmphack wrong - incomplete - no support for imaginary
            String tmpInBuf = d.As<String> ();
            *intoObj        = Characters::String2Float (tmpInBuf);
        });

    return mapper;
}();
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Winvalid-offsetof\"");
DISABLE_COMPILER_MSC_WARNING_END (4573);

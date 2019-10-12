/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Range.h"

using namespace Stroika;
using namespace Stroika::Foundation;

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Traversal::Openness> DefaultNames<Traversal::Openness>::k;
}
#endif
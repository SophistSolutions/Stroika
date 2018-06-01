/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Range.h"

using namespace Stroika;
using namespace Stroika::Foundation;

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy || qCompiler_InlineStaticMemberAutoDefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    constexpr EnumNames<Traversal::Openness> DefaultNames<Traversal::Openness>::k;
}
#endif
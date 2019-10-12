/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "AccessMode.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    template<>
    constexpr EnumNames<IO::AccessMode> DefaultNames<IO::AccessMode>::k;
}
#endif

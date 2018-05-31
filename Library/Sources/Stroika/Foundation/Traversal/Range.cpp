/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    constexpr EnumNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options> DefaultNames<Foundation::IO::Network::Transfer::Connection::Options::Authentication::Options>::k;
}
#endif
/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../Configuration/Enumeration.h"

#include "FileAccessMode.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::IO;

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy || qCompiler_InlineStaticMemberAutoDefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    constexpr EnumNames<IO::FileAccessMode> DefaultNames<IO::FileAccessMode>::k;
}
#endif

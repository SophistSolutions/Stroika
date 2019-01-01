/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#endif

#include "../../Characters/SDKChar.h"
#include "../../Configuration/Common.h"
#include "../../Debug/Trace.h"

#include "Name.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Resources;

#if qPlatform_Windows
extern const Resources::ResourceType PredefinedTypes::kRES = SDKSTR ("RES");
#else
extern const Resources::ResourceType PredefinedTypes::kRES = 0;
#endif

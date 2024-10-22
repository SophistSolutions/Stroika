/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_Windows
#include <Windows.h>
#endif

#include "Stroika/Foundation/Characters/SDKChar.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Name.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Resources;

#if qStroika_Foundation_Common_Platform_Windows
extern const Resources::ResourceType PredefinedTypes::kRES = SDKSTR ("RES");
#else
extern const Resources::ResourceType PredefinedTypes::kRES = 0;
#endif

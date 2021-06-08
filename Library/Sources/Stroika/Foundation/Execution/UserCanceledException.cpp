/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "UserCanceledException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 *************************** UserCanceledException ******************************
 ********************************************************************************
 */
#if qCompiler_cpp17ExplicitInlineStaticMemberOfTemplate_Buggy
const UserCanceledException UserCanceledException::kThe;
#endif

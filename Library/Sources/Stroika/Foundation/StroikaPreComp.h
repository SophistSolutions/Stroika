/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_StroikaPreComp_h_
#define _Stroika_Foundation_StroikaPreComp_h_ 1

#include "Stroika/Foundation/Common/StroikaConfig.h"

#if defined(__cplusplus)
// Declare the namespaces so code early on can freely say stuff like "uses namespace Stroika"
namespace Stroika::Foundation {
}
#endif

/*
 *  So Debug::IsRunningUnderValgrind () is available everywhere; @see Valgrind.h
 */
#if defined(__cplusplus) || defined(__STDC__)
#include "Stroika/Foundation/Debug/Valgrind.h"
#endif

#endif /*_Stroika_Foundation_StroikaPreComp_h_*/

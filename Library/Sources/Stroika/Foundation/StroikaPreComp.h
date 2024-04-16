/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_StroikaPreComp_h_
#define _Stroika_Foundation_StroikaPreComp_h_ 1

#include "Stroika/Foundation/Configuration/StroikaConfig.h"

#if defined(__cplusplus)
// Declare the namespaces so code early on can freely say stuff like "uses namespace Stroika"
namespace Stroika::Foundation {
}
#endif

/*
 *  @see Valgrind.h header for more details. But this should be SAFE and REQUIRED
 *  to include very early on.
 *
 *  Must be included before libg++ stuff (e.g. <shared_ptr>)
 */
#if defined(__cplusplus) || defined(__STDC__)
#include "Stroika/Foundation/Debug/Valgrind.h"
#endif

#endif /*_Stroika_Foundation_StroikaPreComp_h_*/

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_StroikaPreComp_h_
#define _Stroika_Foundation_StroikaPreComp_h_   1

#include    "Configuration/StroikaConfig.h"

#if     defined (__cplusplus)
// Declare the namespaces so code early on can freely say stuff like "uses namespace Stroika"
namespace   Stroika {
    namespace   Foundation {
    }
}
#endif

/*
 *  @see Valgrind.h header for more details. But this should be SAFE and REQUIRED
 *  to include very early on.
 *
 *  Must be included before libg++ stuff (e.g. <shared_ptr>)
 */
#include    "Debug/Valgrind.h"

#endif  /*_Stroika_Foundation_StroikaPreComp_h_*/

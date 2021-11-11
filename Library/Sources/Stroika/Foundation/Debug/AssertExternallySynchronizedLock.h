/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_ 1

#include "../StroikaPreComp.h"

#include "AssertExternallySynchronizedMutex.h"

namespace Stroika::Foundation::Debug {

    using AssertExternallySynchronizedLock [[deprecated ("Since Stroika 2.1b14 use AssertExternallySynchronizedMutex")]] = AssertExternallySynchronizedMutex;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_*/

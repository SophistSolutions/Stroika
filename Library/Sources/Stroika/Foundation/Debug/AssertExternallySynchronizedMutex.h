/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/AssertExternallySynchronizedChecker.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Deprecated">Deprecated</a>
 *
 *  DEPRECATED backward-compatibility header. Since Stroika v3.0d24, this class is called
 *  Debug::AssertExternallySynchronizedChecker and lives in AssertExternallySynchronizedChecker.h.
 *  Include that instead; this header exists only so existing code keeps building, and will be removed.
 *
 *  WHY THE RENAME: it was never a mutex. It does not lock, it does not block, and it does not make
 *  anything thread safe - lock () asserts that no other thread is currently in a conflicting access and
 *  then returns. Calling it a 'Mutex' invited exactly the wrong reading of the ~1200 places that declare
 *  a ReadContext or WriteContext, ie "this code is now synchronized" rather than "assert nobody else is
 *  here". 'Checker' says what it does. See the class docs in AssertExternallySynchronizedChecker.h.
 */

namespace Stroika::Foundation::Debug {

    /**
     *  \brief DEPRECATED alias - use AssertExternallySynchronizedChecker
     *
     *  \deprecated Since Stroika v3.0d24 - use Debug::AssertExternallySynchronizedChecker (it never was
     *              a mutex - see the file note above).
     */
    using AssertExternallySynchronizedMutex
        [[deprecated ("Since Stroika v3.0d24 use Debug::AssertExternallySynchronizedChecker - it is not a mutex")]] =
            AssertExternallySynchronizedChecker;

}

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_h_*/

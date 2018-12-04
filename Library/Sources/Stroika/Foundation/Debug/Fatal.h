/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Fatal_h_
#define _Stroika_Foundation_Debug_Fatal_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/SDKChar.h"
#include "../Configuration/Common.h"

/*
 * TODO:
 *      @todo   Consider adding in here the Windows Structured Error handling stuff here.
 *              I think we already have hooks to translate that into exceptions.
 *              Maybe thats better? Dunno.
 */

namespace Stroika::Foundation::Debug {

    /**
     *  set_unexpected () and set_terminate () to handlers which will abort the applicaiton and
     *  print to the tracelog.
     *
     *  If called with no argument, it installs its OWN handler. To replace with your own, either call
     *  with an explicit handler here or call the underlying C++ APIs.
     */
    void RegisterDefaultFatalErrorHandlers (void (*fatalErrorHandler) (const Characters::SDKChar* msg) noexcept = nullptr);

}

#endif /*_Stroika_Foundation_Debug_Fatal_h_*/

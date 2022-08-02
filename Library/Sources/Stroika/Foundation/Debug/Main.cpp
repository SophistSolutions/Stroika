/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Main.h"

using namespace Stroika::Foundation;

namespace {
    /*
     *  This wont work 100% of the time, but try to detect threads running before main, or after the end of main ()
     */
    bool sKnownBadBeforeMainOrAfterMain_{true};
    struct MainDetector_ {
        MainDetector_ () { sKnownBadBeforeMainOrAfterMain_ = false; }
        ~MainDetector_ () { sKnownBadBeforeMainOrAfterMain_ = true; }
    };
    MainDetector_ sMainDetector_;
}

/*
 ********************************************************************************
 ************************ Debug::AppearsDuringMainLifetime **********************
 ********************************************************************************
 */
bool Debug::AppearsDuringMainLifetime ()
{
    return not sKnownBadBeforeMainOrAfterMain_;
}
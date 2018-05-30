/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Thread.h"

#include "SharedStaticData.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ****************** Execution::Private_::SharedStaticData_DTORHelper_ ***********
 ********************************************************************************
 */
bool Execution::Private_::SharedStaticData_DTORHelper_ (
#if qStroika_Foundation_Execution_SpinLock_IsFasterThan_mutex
    SpinLock* m,
#else
    mutex* m,
#endif
    unsigned int* cu)
{
    Thread::SuppressInterruptionInContext suppressAborts;
#if qCompilerAndStdLib_make_unique_lock_IsSlow
    MACRO_LOCK_GUARD_CONTEXT (*m);
#else
    auto critSec{make_unique_lock (*m)};
#endif
    --(*cu);
    if (*cu == 0) {
        return true;
    }
    return false;
}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Thread.h"

#include "SharedStaticData.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 **************** Execution::Private_::SharedStaticData_DTORHelper_ *************
 ********************************************************************************
 */
bool Execution::Private_::SharedStaticData_DTORHelper_ (conditional_t<kSpinLock_IsFasterThan_mutex, SpinLock, mutex>* m, unsigned int* cu)
{
    Thread::SuppressInterruptionInContext suppressAborts;
    [[maybe_unused]] auto&&               critSec = lock_guard{*m};
    --(*cu);
    if (*cu == 0) {
        return true;
    }
    return false;
}

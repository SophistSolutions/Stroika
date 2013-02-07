/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     !qCompilerAndStdLib_Supports_stdatomic && qPlatform_Win32
#include    "CriticalSection.h"
#endif

#include    "AtomicOperations.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;



#if     !qCompilerAndStdLib_Supports_stdatomic && qPlatform_Win32
namespace   {
    recursive_mutex sCritSec_;
}
#endif


#if     !qCompilerAndStdLib_Supports_stdatomic && qPlatform_Win32
int64_t Execution::AtomicIncrement (volatile int64_t* p)
{
    RequireNotNull (p);
    lock_guard<recursive_mutex> critSec (sCritSec_);
    (*p)++;
    return *p;
}

uint64_t    Execution::AtomicIncrement (volatile uint64_t* p)
{
    RequireNotNull (p);
    lock_guard<recursive_mutex> critSec (sCritSec_);
    (*p)++;
    return *p;
}

int64_t Execution::AtomicDecrement (volatile int64_t* p)
{
    RequireNotNull (p);
    lock_guard<recursive_mutex> critSec (sCritSec_);
    (*p)--;
    return *p;
}

uint64_t    Execution::AtomicDecrement (volatile uint64_t* p)
{
    RequireNotNull (p);
    lock_guard<recursive_mutex> critSec (sCritSec_);
    (*p)--;
    return *p;
}

int64_t Execution::AtomicAdd (volatile int64_t* p, int64_t arg)
{
    RequireNotNull (p);
    lock_guard<recursive_mutex> critSec (sCritSec_);
    (*p) += arg;
    return *p;
}

uint64_t    Execution::AtomicAdd (volatile uint64_t* p, uint64_t arg)
{
    RequireNotNull (p);
    lock_guard<recursive_mutex> critSec (sCritSec_);
    (*p) += arg;
    return *p;
}

int64_t Execution::AtomicSubtract (volatile int64_t* p, int64_t arg)
{
    RequireNotNull (p);
    lock_guard<recursive_mutex> critSec (sCritSec_);
    (*p) -= arg;
    return *p;
}

uint64_t    Execution::AtomicSubtract (volatile uint64_t* p, uint64_t arg)
{
    RequireNotNull (p);
    lock_guard<recursive_mutex> critSec (sCritSec_);
    (*p) -= arg;
    return *p;
}
#endif

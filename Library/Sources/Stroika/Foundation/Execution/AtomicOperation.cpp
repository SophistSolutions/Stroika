/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	"CriticalSection.h"
#endif

#include	"AtomicOperations.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;



#if		qPlatform_Windows
namespace	{
	CriticalSection	sCritSec_;
}
#endif


#if		qPlatform_Win32
int64_t	Execution::AtomicIncrement (volatile int64_t* p)
{
	RequireNotNull (p);
	AutoCriticalSection	critSec (sCritSec_);
	(*p)++;
	return *p;
}

uint64_t	Execution::AtomicIncrement (volatile uint64_t* p)
{
	RequireNotNull (p);
	AutoCriticalSection	critSec (sCritSec_);
	(*p)++;
	return *p;
}

int64_t	Execution::AtomicDecrement (volatile int64_t* p)
{
	RequireNotNull (p);
	AutoCriticalSection	critSec (sCritSec_);
	(*p)--;
	return *p;
}

uint64_t	Execution::AtomicDecrement (volatile uint64_t* p)
{
	RequireNotNull (p);
	AutoCriticalSection	critSec (sCritSec_);
	(*p)--;
	return *p;
}

int64_t	Execution::AtomicAdd (volatile int64_t* p, int64_t arg)
{
	RequireNotNull (p);
	AutoCriticalSection	critSec (sCritSec_);
	(*p) += arg;
	return *p;
}

uint64_t	Execution::AtomicAdd (volatile uint64_t* p, uint64_t arg)
{
	RequireNotNull (p);
	AutoCriticalSection	critSec (sCritSec_);
	(*p) += arg;
	return *p;
}

int64_t	Execution::AtomicSubtract (volatile int64_t* p, int64_t arg)
{
	RequireNotNull (p);
	AutoCriticalSection	critSec (sCritSec_);
	(*p) += arg;
	return *p;
}

uint64_t	Execution::AtomicSubtract (volatile uint64_t* p, uint64_t arg)
{
	RequireNotNull (p);
	AutoCriticalSection	critSec (sCritSec_);
	(*p) -= arg;
	return *p;
}
#endif

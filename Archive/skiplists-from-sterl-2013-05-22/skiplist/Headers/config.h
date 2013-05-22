#pragma once

#ifndef	qSkipListDebug
	#define	qSkipListDebug	!defined(NDEBUG)
#endif


#ifndef	qSkipListStatistics
	#define	qSkipListStatistics qSkipListDebug
#endif
 
#include <assert.h>

#define	and &&
#define or	||
#define	not	!

#define	override	virtual
#define	nonvirtual	

#define	Assert(v)			assert ((v))
#define	AssertNotReached() assert (false)
#define	AssertNotNull(v)	assert ((v) != nullptr)
#define	RequireNotNull(v)	assert ((v) != nullptr)
#define	Require(v)	assert ((v))
#define	Ensure(v)	assert ((v))

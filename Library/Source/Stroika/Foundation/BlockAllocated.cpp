/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"StroikaPreComp.h"

#include	"ThreadUtils.h"

#include	"BlockAllocated.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;


using	namespace	BlockAllocation;
using	namespace	BlockAllocation::Private;

using	namespace	ThreadUtils;

CriticalSection*	BlockAllocation::Private::sCritSection_	=	NULL;


ActualModuleInit::ActualModuleInit ()
{
	Require (sCritSection_ == NULL);
	sCritSection_ = DEBUG_NEW CriticalSection ();
}

ActualModuleInit::~ActualModuleInit ()
{
	RequireNotNil (sCritSection_);
	delete sCritSection_;
	sCritSection_ = NULL;
}







#if		qAllowBlockAllocation
	void*	BlockAllocation::Private::sSizeof_4_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_8_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_12_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_16_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_20_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_24_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_28_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_32_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_36_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_40_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_44_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_48_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_52_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_56_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_60_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_64_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_68_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_72_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_76_NextLink	=	NULL;
	void*	BlockAllocation::Private::sSizeof_80_NextLink	=	NULL;
#endif


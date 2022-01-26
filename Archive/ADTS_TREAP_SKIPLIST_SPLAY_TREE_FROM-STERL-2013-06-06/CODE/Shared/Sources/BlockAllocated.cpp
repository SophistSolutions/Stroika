#include <algorithm> 

#include "../Headers/BlockAllocated.h"


void*	sSizeof_4_NextLink	=	nullptr;
void*	sSizeof_8_NextLink	=	nullptr;
void*	sSizeof_12_NextLink	=	nullptr;
void*	sSizeof_16_NextLink	=	nullptr;
void*	sSizeof_20_NextLink	=	nullptr;
void*	sSizeof_24_NextLink	=	nullptr;
void*	sSizeof_28_NextLink	=	nullptr;
void*	sSizeof_32_NextLink	=	nullptr;
void*	sSizeof_36_NextLink	=	nullptr;
void*	sSizeof_40_NextLink	=	nullptr;
void*	sSizeof_44_NextLink	=	nullptr;
void*	sSizeof_48_NextLink	=	nullptr;
void*	sSizeof_52_NextLink	=	nullptr;
void*	sSizeof_56_NextLink	=	nullptr;
void*	sSizeof_60_NextLink	=	nullptr;
void*	sSizeof_64_NextLink	=	nullptr;
void*	sSizeof_68_NextLink	=	nullptr;
void*	sSizeof_72_NextLink	=	nullptr;
void*	sSizeof_76_NextLink	=	nullptr;
void*	sSizeof_80_NextLink	=	nullptr;

void**	GetMem_Util_ (const size_t sz)
{
	Assert (sz >= sizeof (void*));	//	cuz we overwrite first sizeof(void*) for link

	const	size_t	kTargetMallocSize	=	16360;					// 16384 = 16K - leave a few bytes sluff...
	const	size_t	kChunks = std::max (static_cast<size_t> (kTargetMallocSize / sz), static_cast<size_t> (10));

	void**	newLinks	=	(void**)new char [kChunks*sz];
	void**	curLink		=	newLinks;
	for (size_t i = 1; i < kChunks; i++) {
		*curLink = &(((char*)newLinks)[i*sz]);
		curLink = (void**)*curLink;
	}
	*curLink = nullptr;		// nullptr-terminate the link list
	return (newLinks);
}


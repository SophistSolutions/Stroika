/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"Shared.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;



// Hack because of qTemplatesHasRepository problem...
Counter_Shared*		Memory::sCounterList_Shared	=	Nil;

void	Memory::GetMem_Shared ()
{
	const	unsigned int	kChunks = 1024 / sizeof (Counter_Shared);
	Assert (sizeof (Counter_Shared) >= sizeof (Counter_Shared*));	//	cuz we overwrite first sizeof(void*) for link
	/*
	 * If there are no links left, malloc some, and link them all together.
	 */
	sCounterList_Shared = (Counter_Shared*)::new char [kChunks*sizeof (Counter_Shared)];
	Counter_Shared*	curLink	=	sCounterList_Shared;
	for (register int i = 1; i < kChunks; i++) {
		*(Counter_Shared**)curLink = sCounterList_Shared + i;
		curLink = *(Counter_Shared**)curLink;
	}
	(*(Counter_Shared**)curLink) = Nil;
	EnsureNotNil (sCounterList_Shared);
}



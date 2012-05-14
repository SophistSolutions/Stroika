/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_WeakPtr_h_
#define	_Stroika_Foundation_Memory_WeakPtr_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"

#include	"SharedPtr.h"




/*
 *		TODO:


 *			o	Basic idea is - maybe single link list in SHARPEREPENVOELVEP of REPS for WeakPtr. Then critical section added to that sharedenvoelelp. Then crit section whwn degremennt refcount to zeor before 
 *				delete and then also critsectiona round SHARENVOLEOPE LOCK - which - NO - CRITSECITON MUS TGO OUTSIDE DECRMENT TO ZERO
 *
 *			o	Then actual WeakPtr class becomes trivial - its lock calls envolopelock to get SharedPtr<T>. Not much else to it.
 *			o	WeakPtr contains PLAIN BARE pointer to SharedPtr_WEAKREF_ENVOLOPE.
 *
 */






namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {



		}
	}
}
#endif	/*_Stroika_Foundation_Memory_WeakPtr_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"WeakPtr.inl"

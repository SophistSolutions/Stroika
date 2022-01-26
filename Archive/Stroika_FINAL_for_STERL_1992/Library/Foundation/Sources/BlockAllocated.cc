/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__BlockAllocated__cc
#define	__BlockAllocated__cc

/*
 * $Header: /fuji/lewis/RCS/BlockAllocated.cc,v 1.18 1992/11/20 19:16:45 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: BlockAllocated.cc,v $
 *		Revision 1.18  1992/11/20  19:16:45  lewis
 *		Added Block_Alloced_sizeof_40_sNextLink, and Block_Alloced_sizeof_44_sNextLink.
 *
 *		Revision 1.17  1992/11/16  21:57:31  lewis
 *		Added Block_Alloced_sizeof_36_sNextLink.
 *
 *		Revision 1.16  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.15  1992/11/07  16:29:42  lewis
 *		Moved Block_Alloced_GetMem_Util to header because of GenClass quirk
 *
 *		Revision 1.14  1992/11/06  02:45:28  lewis
 *		Had to hack DebugMessage () for size>32 in blockalloc since
 *		GenClass screwed up message.
 *		Also, fixed GetMem () to work rounding sizes properly for things
 *		not multiples of 4, and not directly in our re-used freepools
 *
 *		Revision 1.13  1992/11/05  22:41:44  lewis
 *		Use sizeof (T) <= N rather than == N in accessing shared
 *		freepools.
 *		Note that this breaks allocator::GetMem () for those new cases-
 *		fix that soon!!!
 *		Also, do ::new char [sizeof (T[kChunks])] instead of ::new char [kChunks*sizeof (T)]
 *		since the latter may be wrong for alignment reasons.
 *		If debug on, DebugMessage() for blockallocs not in shared freepools - just
 *		temporarily to see if there are any.
 *		Adjust kChunks so we should try to use roughtly 1K at a time.
 *
 *		Revision 1.12  1992/10/29  15:52:51  lewis
 *		Add op new / delete here from header conditioned on !qCFront_InlinesNotFullyImplemented since apples cfront 2.1 barfs.
 *
 *		Revision 1.11  1992/10/22  18:13:43  lewis
 *		Another subtly bug with BlockAllocated (old macro version was right).
 *		We forgot to Nil terminate the linked list in the freepool, so we got
 *		sporadic crashes snarfing up random memory off the end.
 *		Also, we upped default chunk count a bit since we now get more overlap
 *		amount freelists.
 *
 *		Revision 1.10  1992/10/22  04:04:16  lewis
 *		Add BlockAllocated free list head for sizes 20, 24, 28, and 32.
 *		Instead of having GetNextLink return reference, have return value, and
 *		add new SetNextLink method instead.
 *
 *		Revision 1.9  1992/10/20  21:23:28  lewis
 *		Always use GetNextLink, and use new special sized freepool heads - see
 *		header comments for more info.
 *
 *		Revision 1.7  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.6  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.5  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.3  1992/07/17  19:16:25  lewis
 *		Worked around qStaticDataMembersInTemplatesSupported bug with gcc. Now this
 *		template at least compiles here.
 *		Minor cleanups, including putting inlines at the bottom of the file in the
 *		inlines section, etc...
 *		Tried to use default arg size for blockallocated template but gcc considered
 *		it a syntax error. Not sure I got it wrong, or gcc 2.2.1. Anyhow we can wait
 *		a bit before trying this.
 *
 *		<copied from Memory.cc>
 *
 */

// Be sure this file is only included once when doing include of .cc file
#include	"Config-Foundation.hh"
#if		qRealTemplatesAvailable && !qTemplatesHasRepository
	#if		!qIncluding_BlockAllocatedCC
		#define	qIncluding_BlockAllocatedCC	1
	#endif
#endif


#include	"Debug.hh"

#include	"BlockAllocated.hh"


#if		!qIncluding_BlockAllocatedCC

	void*	Block_Alloced_sizeof_4_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_8_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_12_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_16_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_20_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_24_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_28_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_32_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_36_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_40_sNextLink	=	Nil;
	void*	Block_Alloced_sizeof_44_sNextLink	=	Nil;

#endif



/*
 ********************************************************************************
 ************************************ BlockAllocated ****************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable

template	<class	T>	void	BlockAllocated<T>::GetMem ()
{
#if		qDebug
	/*
	 * Temporary hack to verify were getting the most out of our blockallocator.
	 * If this is ever gets called, add extra sizes. NO COST, just wins!!!
	 */
	if (sizeof (T) > 44) {
		DebugMessage ("BlockAllocator<T> has a size > 44: %d", sizeof (T));
	}
#endif

	if (sizeof (T) <= 4) {
		Block_Alloced_sizeof_4_sNextLink = Block_Alloced_GetMem_Util (4);
	}
	else if (sizeof (T) <= 8) {
		Block_Alloced_sizeof_8_sNextLink = Block_Alloced_GetMem_Util (8);
	}
	else if (sizeof (T) <= 12) {
		Block_Alloced_sizeof_12_sNextLink = Block_Alloced_GetMem_Util (12);
	}
	else if (sizeof (T) <= 16) {
		Block_Alloced_sizeof_16_sNextLink = Block_Alloced_GetMem_Util (16);
	}
	else if (sizeof (T) <= 20) {
		Block_Alloced_sizeof_20_sNextLink = Block_Alloced_GetMem_Util (20);
	}
	else if (sizeof (T) <= 24) {
		Block_Alloced_sizeof_24_sNextLink = Block_Alloced_GetMem_Util (24);
	}
	else if (sizeof (T) <= 28) {
		Block_Alloced_sizeof_28_sNextLink = Block_Alloced_GetMem_Util (28);
	}
	else if (sizeof (T) <= 32) {
		Block_Alloced_sizeof_32_sNextLink = Block_Alloced_GetMem_Util (32);
	}
	else if (sizeof (T) <= 36) {
		Block_Alloced_sizeof_36_sNextLink = Block_Alloced_GetMem_Util (36);
	}
	else if (sizeof (T) <= 40) {
		Block_Alloced_sizeof_40_sNextLink = Block_Alloced_GetMem_Util (40);
	}
	else if (sizeof (T) <= 44) {
		Block_Alloced_sizeof_44_sNextLink = Block_Alloced_GetMem_Util (44);
	}
	else {
		/*
		 * NON-Shared freepool. This should rarely, if ever happen, but we fully support
		 * it anyhow.
		 */
		SetNextLink (Block_Alloced_GetMem_Util (sizeof (T)));
	}
}


#if		qStaticDataMembersInTemplatesSupported || !qRealTemplatesAvailable
	template	<class	T>	void*	BlockAllocated<T>::sNextLink = Nil;
#else
	template	<class	T>	void*&	BlockAllocated<T>::NLHack ()
	{
		static	void*	sNextLink	=	Nil;
		return (sNextLink);
	}
	template	<class	T>	void*	BlockAllocated<T>::GetNextLink ()
	{
		if (sizeof (T) <= 4) {
			return (Block_Alloced_sizeof_4_sNextLink);
		}
		else if (sizeof (T) <= 8) {
			return (Block_Alloced_sizeof_8_sNextLink);
		}
		else if (sizeof (T) <= 12) {
			return (Block_Alloced_sizeof_12_sNextLink);
		}
		else if (sizeof (T) <= 16) {
			return (Block_Alloced_sizeof_16_sNextLink);
		}
		else if (sizeof (T) <= 20) {
			return (Block_Alloced_sizeof_20_sNextLink);
		}
		else if (sizeof (T) <= 24) {
			return (Block_Alloced_sizeof_24_sNextLink);
		}
		else if (sizeof (T) <= 28) {
			return (Block_Alloced_sizeof_28_sNextLink);
		}
		else if (sizeof (T) <= 32) {
			return (Block_Alloced_sizeof_32_sNextLink);
		}
		else if (sizeof (T) <= 36) {
			return (Block_Alloced_sizeof_36_sNextLink);
		}
		else if (sizeof (T) <= 40) {
			return (Block_Alloced_sizeof_40_sNextLink);
		}
		else if (sizeof (T) <= 44) {
			return (Block_Alloced_sizeof_44_sNextLink);
		}
		else {
			return (NLHack ());
		}
	}
	template	<class	T>	void	BlockAllocated<T>::SetNextLink (void* nextLink)
	{
		if (sizeof (T) <= 4) {
			Block_Alloced_sizeof_4_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 8) {
			Block_Alloced_sizeof_8_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 12) {
			Block_Alloced_sizeof_12_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 16) {
			Block_Alloced_sizeof_16_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 20) {
			Block_Alloced_sizeof_20_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 24) {
			Block_Alloced_sizeof_24_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 28) {
			Block_Alloced_sizeof_28_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 32) {
			Block_Alloced_sizeof_32_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 36) {
			Block_Alloced_sizeof_36_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 40) {
			Block_Alloced_sizeof_40_sNextLink = nextLink;
		}
		else if (sizeof (T) <= 44) {
			Block_Alloced_sizeof_44_sNextLink = nextLink;
		}
		else {
			NLHack () = nextLink;
		}
	}
#endif


#if		qCFront_InlinesNotFullyImplemented
	template	<class	T>	void*	BlockAllocated<T>::operator new (size_t n)
	{
		Assert (sizeof (T) >= sizeof (void*));	//	cuz we overwrite first sizeof(void*) for link
		Assert (n == sizeof (T));
	
		/*
		 * To implement linked list of BlockAllocated(T)'s before they are
		 * actually alloced, re-use the begining of this as a link pointer.
		 */
		if (GetNextLink () == Nil) {
			GetMem ();
		}
		void*	result = GetNextLink ();
		SetNextLink (*(void**)GetNextLink ());

		return (result);
	}
	template	<class	T>	void	BlockAllocated<T>::operator delete (void* p)
	{
		if (p != Nil) {
			(*(void**)p) = GetNextLink ();
			SetNextLink (p);
		}
	}
#endif	/*qCFront_InlinesNotFullyImplemented*/


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


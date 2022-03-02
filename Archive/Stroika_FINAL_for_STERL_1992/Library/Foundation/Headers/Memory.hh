/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Memory__
#define	__Memory__

/*
 * $Header: /fuji/lewis/RCS/Memory.hh,v 1.9 1992/11/15 16:01:40 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Memory.hh,v $
 *		Revision 1.9  1992/11/15  16:01:40  lewis
 *		Added new method - PreferredAllocSize (size_t originalSize) - at
 *		kdj's suggested as performance tweek.
 *
 *		Revision 1.8  1992/11/12  19:14:13  lewis
 *		*** empty log message ***
 *
 *		Revision 1.7  1992/11/12  08:08:02  lewis
 *		Moved #include <new.h> here, and did our own implementation of
 *		placement operator new here if needed.
 *
 *		Revision 1.6  1992/10/09  19:45:14  lewis
 *		Readded qGCC_TroubleWithFunctionsReturningFunctionPtrs.
 *
 *		Revision 1.5  1992/10/09  18:20:53  lewis
 *		Misc cleanups.
 *
 *		Revision 1.3  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.19  92/05/22  23:46:43  23:46:43  lewis (Lewis Pringle)
 *		Moved BlockAllocated/ReferenceCounted to separate files.
 *		
 *		Revision 1.13  92/05/11  19:59:27  19:59:27  lewis (Lewis Pringle)
 *		Implemented templated BlockAllocated() class, and in both changed
 *		op new to be inline, and separated out the code to build up the array
 *		of free cells into another function (GetMem()).
 *		
 *		Revision 1.11  92/03/22  16:36:03  16:36:03  lewis (Lewis Pringle)
 *		Make BlockAllocatedDeclare(T)::operator delete inline.
 *		
 *		Revision 1.9  1992/02/21  21:03:26  lewis
 *		Use macro INHERRIT_FROM_SINGLE_OBJECT instead of class SingleObject so
 *		that crap doesnt appear in non-mac inheritance graphs.
 *
 *		Revision 1.6  1992/02/14  02:28:49  lewis
 *		Got rid of qDebug_SupportValidateOnAlloc define. Always allow if
 *		debug turned on.
 *
 *
 */

#include	<new.h>

#include	"Config-Foundation.hh"	// to get qGCC_LibgPlusPlusSharpDefineSize_tBug defined...
#if		qGCC_LibgPlusPlusSharpDefineSize_tBug
	#ifdef	size_t
		typedef	size_t	size_t_unhack;
	#undef	size_t
		typedef	size_t_unhack	size_t;
	#endif
#endif


#include	"Debug.hh"
#include	"Exception.hh"
#include	"Math.hh"





/*
 *		Anyone can set this procedure, and when a new/realloc call is about
 *	to fail, this will be called before the exception is raised.  This
 *	allows you to release storage, and allow the request to be satisfied.
 *	It is convienient in implmenting software that behaves well under low
 *	memory conditions.  If left Nil, or if unsuccessful in realeasing the
 *	given amount, a MemoryException will likely result from an unsatisfyable
 *	request.
 */
#if		qGCC_TroubleWithFunctionsReturningFunctionPtrs
	extern
#endif
Boolean	(*GetOutOfMemoryProc ()) (size_t);
void	SetOutOfMemoryProc (Boolean (*outOfMemoryProc) (size_t amountNeeded));



/*
 *		Since our memory support is not thread-safe, we must protect against
 *	access during interupts, etc. SetMemoryAllocsEnabled () returns old value.
 */
#if		qDebug
	Boolean	SetMemoryAllocsEnabled (Boolean memoryAllocsEnabled);
#endif




class	MemoryException : public Exception {
	public:
		MemoryException ();
		MemoryException (const Exception& exception);
		MemoryException (const MemoryException& memoryException);

		override	void	Raise ();

		nonvirtual	void	Raise (Int16 osErr);

		nonvirtual	Int16	GetOSErr () const;

	private:
		Int16	fErr;
};

extern	MemoryException	gMemoryException;




/*
 *		We gaurantee that new and realloc of zero sized objects works
 *	correctly - that is returns a unique pointer each time. This is important
 *	if you commonly alloc a zero sized array, and then resize it.
 */
void*	operator	new (size_t n);
void	operator	delete (void* p);

/*
 *		NB: This version of realloc promises that if the realloc fails, the
 *	old memory remains in-tact... We just throw an execption, and change
 *	nothing... Should check each lib version we use promises this!!!!
 */
void*	ReAlloc (void* p, size_t n);


#if		qDebug
	/*
	 *		Get and set flags as to whether you want to run
	 *	ValidateHeapStorage () automatically on every heap allocation,
	 *	deallocation, and reallocation.
	 */
	Boolean	GetValidateOnAlloc ();
	void	SetValidateOnAlloc (Boolean validateOnAlloc);
	
	
	/*
	 *		Print some statistics on how much memory has been allocated,
	 *	largest block etc.
	 */
	void	PrintMemoryStatistics ();
	
	/*
	 *		The Stroika version of new/delete/ReAlloc keep statistics, and a
	 *	linked list of allocated blocks with "gaurds" on either end to detect
	 *	memory overwrites. These gaurds can all be checked by
	 *	ValidateHeapStorage (). new/delete/ and ReAlloc () automatically check
	 *	the block you are workign with but they can be caused to also call
	 *	ValidateHeapStorage by setting a flag with SetValidateOnAlloc ().
	 *
	 *		Return True iff heap valid.
	 */
	Boolean	ValidateHeapStorage (Boolean reportIfGood = False);	// wind thru all alloced memory, and check magic #'s
#endif	/*qDebug*/



/*
 *		Return the best size of memory to allocate for the given malloc implementation
 *	which is greater or equal to the given size. This can be usefull to tune
 *	memory allocations. It is not gauranteed to be "correct", in that through
 *	a number of mechanisms, the malloc implementation could change (dynamic linking,
 *	for instance) but it is generally a good guess at what size of memory would
 *	be best to allocate for a particuar system.
 *
 *		Note, this memory size should be the amount the malloc system will allocate
 *	anyhow, and waste the rest. That is opposed to saying that it would tend
 *	to do better and waste less generally throwing in lots of sluff.
 *
 *		Basicly, this generally just rounds originalSize up to the blocksize used by
 *	the underly allocator, minus any overhead.
 *
 *		NB: This refers to allocations done with operator new, as opposed to those done
 *	with malloc, if they differ.
 */
size_t	PreferredAllocSize (size_t originalSize);





#if		!qPlacementOperatorNewProvided || qMPW_CFront
	// Do this for MPW Cfront also, since the do out-of-line function call - that
	// seems totally pointless...
	inline	void*	operator new (size_t /*size*/, void* placeAt) { return (placeAt); }
#endif






/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Memory__*/

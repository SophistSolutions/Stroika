/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Shared__cc
#define	__Shared__cc

/*
 * $Header: /fuji/lewis/RCS/Shared.cc,v 1.14 1992/11/12 17:39:34 lewis Exp $
 *
 * TODO:
 *		->	Get rid of GetMem_Shared - use blockallocated instead.
 *
 * Changes:
 *	$Log: Shared.cc,v $
 *		Revision 1.14  1992/11/12  17:39:34  lewis
 *		Add comments.
 *
 *		Revision 1.13  1992/11/12  08:15:30  lewis
 *		Added #if !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		around template name predeclare for GenClass.
 *
 *		Revision 1.12  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.11  1992/11/10  04:47:46  lewis
 *		Added #ifndef __Shared__cc  around file.
 *
 *		Revision 1.10  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.9  1992/10/07  23:28:31  sterling
 *		Work around MPW compiler bug with inlines with asserts in op-> break
 *		MPW compile - unspinreg bug.
 *
 *		Revision 1.8  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.7  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.5  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.3  1992/07/17  19:54:49  lewis
 *		Comment cleanups (80 columns). Change Shared<T> to Shared in
 *		a few places cuz that appeased gcc 2.2.2 somewhat - this is a known bug and
 *		supposedly will be fixed in 2.3.
 *		Make Shared(T)::(const Shared(T)&) and operator* inline
 *		in macro version.
 *		Moved non-inline templates into .cc file and added the usual
 *		"qRealTemplatesAvailable && !qTemplatesHasRepository" hack to pull them in.
 *
 *		Revision 1.2  1992/07/08  01:23:24  lewis
 *		Got rid of old commented out code, and put in new block allocation routine
 *		GetMem_ReferneceCounted, and global variable for free list.
 *
 *		Revision 1.2  1992/06/09  12:37:07  lewis
 *		Renamed SmartPointer back to Shared. And deleted old implementation.
 *
 *		<copied from Memory.cc>
 *
 */


#include	"Debug.hh"

#include	"Shared.hh"



#if		!qIncluding_SharedCC
// Hack because of qTemplatesHasRepository problem...
Counter_Shared*		sCounterList_Shared	=	Nil;

void	GetMem_Shared ()
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
#endif	/*!qIncluding_SharedCC*/





/*
 ********************************************************************************
 ************************************ Shared ************************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Shared;	// Tmp hack so GenClass will fixup following CTOR/DTORs
										// Harmless, but silly.

#endif

template <class T> Shared<T>::~Shared ()
{
	if (fCount != Nil) {	// could be Nil in presence of exceptions
		Require (*fCount >= 1);
		(*fCount)--;
		/*
		 * When reference count drops to zero, delete both the counter,
		 * and the item we are reference counting. NB: fPtr could be
		 * Nil at this point.
		 */
		if (*fCount == 0) {
			DeleteCounter_Shared (fCount);
			delete fPtr;
		}
	}
}

template <class T> Shared<T>& Shared<T>::operator= (const Shared<T>& src)
{
	/*
	 * Careful to avoid *this = *this - do nothing.
	 *
	 * Unreference old item (deleting if ours is the last reference) and
	 * add a new reference to the rhs.
	 *
	 * NB: fPtr can still be Nil here.
	 */
	if (fPtr != src.fPtr) {
		RequireNotNil (fCount);
		Require (*fCount >= 1);
		(*fCount)--;
		/*
		 * When reference count drops to zero, delete both the counter,
		 * and the item we are reference counting. NB: fPtr could be
		 * Nil at this point.
		 */
		if (*fCount == 0) {
			DeleteCounter_Shared (fCount);
			delete fPtr;
		}
		fPtr = src.fPtr;
		fCount = src.fCount;
		fCloner = src.fCloner;
		(*fCount)++;
	}
	return (*this);
}

template <class T> Shared<T>& Shared<T>::operator= (T* pointer)
{
	/*
	 * Again: careful to avoid *this = *this - do nothing.
	 *
	 * Unreference old item (deleting if ours is the last reference) and
	 * add a new reference to the rhs.
	 *
	 * NB: fPtr and pointer can still be Nil here.
	 */
	if (fPtr != pointer) {
		RequireNotNil (fCount);
		Require (*fCount >= 1);
		(*fCount)--;
		/*
		 * One subtle point here is that we only allocate a new
		 * reference count if *fCount != 0 - this is because we
		 * would be deleting that counter here, and then allocating
		 * a new one, so why bother.
		 */
		if (*fCount == 0) {
			delete fPtr;
		}
		else {
			fCount = NewCounter_Shared ();
		}
		fPtr = pointer;
		*fCount = 1;
	}
	return (*this);
}

#if		qMPW_C_COMPILER_REGISTER_OVERFLOW_BUG && qDebug
	template <class T> const T*	Shared<T>::operator-> () const
	{
		EnsureNotNil (fPtr);
		return (fPtr);
	}
	template <class T> const T&	Shared<T>::operator* () const
	{
		EnsureNotNil (fPtr);
		return (*fPtr);
	}
#endif

template <class T> T* Shared<T>::operator-> ()
{
	/*
	 * For non-const pointing, we must clone ourselves (if there are
	 * extra referneces).
	 */
	Assure1Reference ();
	EnsureNotNil (fPtr);
	return (fPtr); 
}

template <class T> T& Shared<T>::operator* ()
{
	/*
	 * For non-const dereferencing, we must clone ourselves (if there are
	 * extra referneces).
	 */
	Assure1Reference ();
	EnsureNotNil (fPtr);
	return (*fPtr); 
}

template <class T> T* Shared<T>::GetPointer ()
{
	/*
	 * For non-const pointing, we must clone ourselves (if there are
	 * extra referneces). If we are a Nil pointer, nobody could actually
	 * rereference it anyhow, so don't bother with the Assure1Reference()
	 * in that case.
	 */
	if (fPtr == Nil) {
		return (Nil);
	}
	Assure1Reference ();
	EnsureNotNil (fPtr);
	return (fPtr); 
}

template <class T> void	Shared<T>::BreakReferences ()
{
	/*
	 *		For a valid pointer that is reference counted and multiply shared,
	 *	make a copy of that pointer via our fCloner function, and assign
	 *	that cloned reference to this.
	 *
	 *		Note that by doing so, we remove any references to the current
	 *	item, and end up with our having the sole reference to the new copy of fPtr.
	 *
	 *		Since we will be cloning the given pointer, we assume(assert) that
	 *	it is non-Nil.
	 */
	RequireNotNil (fPtr);
	RequireNotNil (fCloner);
	AssertNotNil (fCount);

	Require (CountReferences () > 1);
	*this = ((*fCloner) (*fPtr));
	Ensure (CountReferences () == 1);
}

#endif	/*qRealTemplatesAvailable*/



#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

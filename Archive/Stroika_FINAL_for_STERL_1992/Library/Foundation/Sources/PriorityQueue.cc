/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__PriorityQueue__cc
#define	__PriorityQueue__cc

/*
 * $Header: /fuji/lewis/RCS/PriorityQueue.cc,v 1.9 1992/11/13 03:36:34 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PriorityQueue.cc,v $
 *		Revision 1.9  1992/11/13  03:36:34  lewis
 *		#if 0'd out everything - fix later.
 *
 *		Revision 1.8  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.7  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.6  1992/10/23  06:18:39  lewis
 *		Cleaned up some for GenClass stuff, but untested, and looks like it
 *		needs work.
 *
 *		Revision 1.5  1992/10/22  19:03:21  lewis
 *		Started (but not finished) conversion to new genclass stuff and
 *		using templates. Obviously template version was way off - no testsuite
 *		and maybe macro version worked but...
 *
 *		Revision 1.4  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.3  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.2  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.2  1992/09/11  16:32:52  sterling
 *		used new Shared implementaiton
 *
 */



#include	"Debug.hh"
#include	"PriorityQueue_LinkList.hh"

#include	"PriorityQueue.hh"




#if		qRealTemplatesAvailable && 0

template	<class	T>	class	PriorityQueueIterator : public IteratorRep<T> {
	public:
		PriorityQueueIterator (IteratorRep<PQEntry<T> >* it) :
			fIt (it)
		{
		}
		~PriorityQueueIterator ();
	
		override	Boolean	Done () const;
		override	void	Next ();
		override	T		Current () const;
	
	private:
		IteratorRep<PQEntry<T> >*	fIt;
};


/*
 ********************************************************************************
 ************************************ PriorityQueueRep **************************
 ********************************************************************************
 */

template	<class	T> 	PriorityQueueRep<T>::operator IteratorRep<T>* () const
{
	return (new PriorityQueueIterator<T> (*this));
}




/*
 ********************************************************************************
 ******************************* PriorityQueueIterator **************************
 ********************************************************************************
 */

template	<class	T> 	PriorityQueueIterator<T>::~PriorityQueueIterator ()
{
	delete fIt;
}

template	<class	T> Boolean		PriorityQueueIterator<T>::Done () const
{
	RequireNotNil (fIt);
	return (fIt->Done ());
}

template	<class	T> void		PriorityQueueIterator<T>::Next ()
{
	RequireNotNil (fIt);
	fIt->Next ();
}

template	<class	T> 	T	PriorityQueueIterator<T>::Current () const
{
	RequireNotNil (fIt);
	return (fIt->Current ().fItem);
}


template	<class T> Boolean	operator== (const PriorityQueue<PQEntry<T> >& lhs, const PriorityQueue<PQEntry<T> >& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}

	IteratorRep<T>* it1 = rhs;
	Boolean	match = True;
	ForEach (T, it, lhs) {
		if (it.Current () != it1->Current ()) {
			match = False;
			break;
		}
		it1->Next ();
	}
	delete it1;
	return (match);
}






/*
 ********************************************************************************
 ************************************ PriorityQueue *****************************
 ********************************************************************************
 */

template	<class	T>	class	PriorityQueue;	// Tmp hack so GenClass will fixup following CTOR/DTORs
												// Harmless, but silly.

template	<class T> inline	PriorityQueue<T>::PriorityQueue () :
	fRep (&Clone, Nil)
{
	*this = PriorityQueue_LinkList<T> ();
}

template	<class T> PriorityQueue<PQEntry<T> >&	PriorityQueue<PQEntry<T> >::operator+= (PriorityQueue<PQEntry<T> > q)
{
	ForEach (T, it, q) {
		GetRep ()->Add (it.Current ());
	}
	return (*this);
}

template	<class T> void	PriorityQueue<PQEntry<T> >::AddItems (const T* items, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		Add (items[i]);
	}
}

template	<class T> PriorityQueue<PQEntry<T> >::operator IteratorRep<T>* () const
{
	return (*GetRep ());
}

template	<class T> PriorityQueue<T>::operator IteratorRep<T>* () const
{
	return (new PriorityQueueIterator(T) (*this));
}

template	<class T> PriorityQueueRep<T>*	PriorityQueue<PQEntry<T> >::Clone (const PriorityQueueRep<T>& rep)
{
	return (rep.Clone ());
}

#endif	/*qRealTemplatesAvailable*/

#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


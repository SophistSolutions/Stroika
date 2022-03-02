/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Tally__cc
#define	__Tally__cc

/*
 * $Header: /fuji/lewis/RCS/Tally.cc,v 1.15 1992/12/03 05:52:55 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Tally.cc,v $
 *		Revision 1.15  1992/12/03  05:52:55  lewis
 *		Override Done() to delegate.
 *
 *		Revision 1.14  1992/11/20  19:21:29  lewis
 *		New iterator More() support.
 *
 *		Revision 1.13  1992/11/13  03:33:55  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Also, be more careful about !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		hacks.
 *
 *		Revision 1.12  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.11  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.10  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.9  1992/10/19  21:42:07  lewis
 *		Move inlines from class declaration to implemenation details section at end
 *		of header, and moved concrete reps to .cc file. Added #includes where needed
 *		for GenClass stuff. Other minor cleanups - untested yet.
 *
 *		Revision 1.8  1992/10/16  22:52:15  lewis
 *		#include shared and iterator for !qRealTemplatesAvailable (Genclass output).
 *
 *		Revision 1.7  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.6  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/10/07  23:33:34  sterling
 *		Moved inline op== TallyEntry here - out of line.
 *
 *		Revision 1.4  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.3  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.2  1992/09/16  00:28:42  sterling
 *		New Implementations.
 *
 *
 *
 */



#include	"Debug.hh"
#include	"Tally.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Tally_Array.hh"		// default implemenation




#if		qRealTemplatesAvailable



template	<class T> class	TallyIterator1Rep : public IteratorRep<T> {
	public:
		TallyIterator1Rep (IteratorRep<TallyEntry<T> >* it) : fIt (it)	{ RequireNotNil (fIt); }
		~TallyIterator1Rep () { delete fIt; }

		override	Boolean			Done () const		{ return (fIt->Done ()); }
		override	Boolean			More ()				{ return (fIt->More ()); }
		override	T				Current () const	{ return (fIt->Current ().fItem); }
		override	IteratorRep<T>*	Clone () const 		{ return (new TallyIterator1Rep<T> (fIt->Clone ()));}

	private:
		IteratorRep<TallyEntry<T> >*	fIt;
};





/*
 ********************************************************************************
 ************************************ TallyRep<T> *******************************
 ********************************************************************************
 */
template	<class T> IteratorRep<T>* TallyRep<T>::MakeIterator ()
{
	return (new TallyIterator1Rep<T> (MakeTallyIterator ()));
}



/*
 ********************************************************************************
 *************************************** Tally<T> *******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Tally;	// tmp hack so GenClass will fixup following CTOR/DTORs
										// harmless, but silly

#endif

template	<class T> Tally<T>::Tally () :
	fRep (&Clone, Nil)
{
	*this = Tally_Array<T> ();
}

template	<class T> Tally<T>::Tally (const T* items, size_t size) :
	fRep (&Clone, Nil)
{
	*this = Tally_Array<T> (items, size);
}

template	<class T> void	Tally<T>::RemoveAll (T item)
{
	Remove (item, TallyOf (item));
}

template	<class T> size_t	Tally<T>::TotalTally () const
{
	size_t sum = 0;
	ForEach (TallyEntry<T>, it, *this) {
		sum += it.Current ().fCount;
	}
	return (sum);
}
	
template	<class T> Tally<T>&	Tally<T>::operator+= (Tally<T> t)
{
	ForEach (TallyEntry<T>, it, t) {
		Add (it.Current ().fItem, it.Current ().fCount);
	}
	return (*this);
}

template	<class T> void	Tally<T>::AddItems (const T* items, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		Add (items[i]);
	}
}

template	<class T>	TallyRep<T>*	Tally<T>::Clone (const TallyRep<T>& rep)
{
	return (rep.Clone ());
}





/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */
template	<class T> Boolean	operator== (const TallyEntry<T>& lhs, const TallyEntry<T>& rhs)
{ 
	if (not (lhs.fItem == rhs.fItem))  {
		return False;
	}
	return (Boolean (lhs.fCount == rhs.fCount));	
}

template	<class T> Boolean	operator== (const Tally<T>& lhs, const Tally<T>& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}
	
	ForEach (TallyEntry<T>, it, lhs) {
		if (it.Current ().fCount != rhs.TallyOf (it.Current ().fItem)) {
			return (False);
		}
	}
	return (True);
}



#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	IteratorOfTallyEntryOf<T>_cc
	#include	SharedOfTallyRepOf<T>_cc
#endif



#endif	/*qRealTemplatesAvailable*/

#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


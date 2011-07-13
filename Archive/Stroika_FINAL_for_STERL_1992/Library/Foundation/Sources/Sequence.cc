/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Sequence__cc
#define	__Sequence__cc

/*
 * $Header: /fuji/lewis/RCS/Sequence.cc,v 1.20 1992/11/23 21:53:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Sequence.cc,v $
 *		Revision 1.20  1992/11/23  21:53:29  lewis
 *		Have conversion operators return SequenceIterator<T> instead of
 *		SequenceIteratorRep<T>*. Also, work around qGCC_ConversionOperatorInitializerConfusionBug.
 *
 *		Revision 1.19  1992/11/20  19:26:19  lewis
 *		Fix operator== to use new Iterator stuff and it.More().
 *
 *		Revision 1.18  1992/11/13  03:47:55  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Also, be more careful about !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		hacks.
 *
 *		Revision 1.17  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.16  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.15  1992/10/28  01:49:22  lewis
 *		Include SharedOfSequenceRepOf<T>_cc instead of using ! in CollectionInfo.
 *
 *		Revision 1.14  1992/10/19  01:38:20  lewis
 *		Minor cleanups, including getting rid of references to SequenceRep_Array, and
 *		just refering to Sequence_Array.
 *
 *		Revision 1.13  1992/10/14  16:21:39  lewis
 *		Include Sequence_Array.hh
 *
 *		Revision 1.12  1992/10/13  05:32:49  lewis
 *		Moved clone from header into sources - silly to have been inline
 *		since never used directly, only by address.
 *
 *		Revision 1.11  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.10  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.9  1992/10/07  23:27:18  sterling
 *		Use not == instead of != (so we only require use of one defined operator).
 *
 *		Revision 1.8  1992/09/25  21:14:26  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.7  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.6  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.2  1992/09/11  16:32:52  sterling
 *		used new Shared implementaiton
 *
 */

#include	"Debug.hh"
#include	"Sequence.hh"			// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Sequence_Array.hh"





#if		qRealTemplatesAvailable


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfSequenceRepOf<T>_cc
#endif
	
/*
 ********************************************************************************
 ************************************* Sequence *********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Sequence;	// tmp hack so GenClass will fixup following CTOR/DTORs
											// harmless, but silly

#endif

template	<class T>	Sequence<T>::Sequence ():
	fRep (&Clone, Nil)
{
	*this = Sequence_Array<T> ();
}

template	<class T>	Sequence<T>::Sequence (const T* items, size_t size):
	fRep (&Clone, Nil)
{
	*this = Sequence_Array<T> (items, size);
}

template	<class T>	Boolean	Sequence<T>::Contains (T item) const
{
	ForEach (T, it, *this) {
		if (it.Current () == item) {
			return (True);
		}
	}
	return (False);
}

template	<class T> void	Sequence<T>::SetAt (T item, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ());

	GetRep ()->SetAt (item, index);
}

template	<class T> size_t	Sequence<T>::IndexOf (T item) const
{
	ForEachT (SequenceIterator, T, it, *this) {
		if (it.Current () == item) {
			return (it.CurrentIndex ());
		}
	}
	return (kBadSequenceIndex);
}

template	<class T> size_t	Sequence<T>::IndexOf (const Sequence<T>& s) const
{
	/* cruddy implementation */
	for (size_t i = 1; i <= GetLength () - s.GetLength () + 1; i++) {
		Boolean	matched	=	True;
		for (size_t j = 1; j <= s.GetLength (); j++) {
			T	t1 = (*this)[i+j-1];
			T	t2 = s[j];
			if (not (t1 == t2)) {
				matched = False;
				break;
			}
		}
		if (matched) {
			return (i);			//	got a match
		}
	}
	return (kBadSequenceIndex);	//	got a match
}

template	<class T> void	Sequence<T>::InsertAt (T item, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ()+1);

	GetRep ()->InsertAt (item, index);
}

template	<class T> void	Sequence<T>::InsertAt (const Sequence<T>& items, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ()+1);

	/*
	 * This is a TERRIBLE implemenation. If it is at all common, we should see about
	 * putting this into the sequence rep. Only case I can think of where this is common,
	 * is for Strings.
	 */
	ForEach (T, it, items) {
		GetRep ()->InsertAt (it.Current (), index++);
	}
}

template	<class T> void	Sequence<T>::RemoveAt (size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ());

	GetRep ()->RemoveAt (index, 1);
}

template	<class T> void	Sequence<T>::RemoveAt (size_t index, size_t amountToRemove)
{
	Require (index >= 1);
	Require (index <= (GetLength ()-amountToRemove+1));

	GetRep ()->RemoveAt (index, amountToRemove);
}

template	<class T> void	Sequence<T>::Remove (T item)
{
	/*
	 * This is not too bad an implementation in Sequence_DoubleLinkList if we
	 * cache last index to link lookup. If not done yet, then doit!
	 */
	size_t index = IndexOf (item);
	if (index != kBadSequenceIndex) {
		RemoveAt (index);
	}
}

template	<class T> void	Sequence<T>::Remove (const Sequence<T>& items)
{
	size_t index = IndexOf (items);
	if (index != kBadSequenceIndex) {
		RemoveAt (index, items.GetLength ());
	}
}

template	<class T> SequenceIteratorRep<T>*	Sequence<T>::MakeSequenceIterator (SequenceDirection d) const
{
	/* 
	 * (~const) to force break references.
	 */
	return (((Sequence<T>*) this)->fRep->MakeSequenceIterator (d));
}

template	<class T> SequenceMutatorRep<T>*	Sequence<T>::MakeSequenceMutator (SequenceDirection d)
{
	return (GetRep ()->MakeSequenceMutator (d));
}

template	<class T> Sequence<T>::operator SequenceIterator<T> () const
{
	return (((Sequence<T>*) this)->fRep->MakeSequenceIterator (eSequenceForward));
//	return (GetRep ()->MakeSequenceIterator (eSequenceForward));
}

template	<class T> Sequence<T>::operator SequenceMutator<T> ()
{
	return (GetRep ()->MakeSequenceMutator (eSequenceForward));
}

template	<class T> void	Sequence<T>::AddItems (const T* items, size_t size)
{
	for (size_t index = 0; index < size; index++) {
		Append (items[index]);
	}
}

template	<class T>	SequenceRep<T>*	Sequence<T>::Clone (const SequenceRep<T>& rep)
{
	return (rep.Clone ());
}

template	<class T> Boolean	operator== (const Sequence<T>& lhs, const Sequence<T>& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}

#if		qGCC_ConversionOperatorInitializerConfusionBug
	Iterator<T>	it1 = (Iterator<T>)rhs;
#else
	Iterator<T>	it1 = rhs;
#endif
	(void)it1.More ();
	ForEach (T, it, lhs) {
		if (not (it.Current () == it1.Current ())) {
			return (False);
		}
		(void)it1.More ();
	}
	return (True);
}


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


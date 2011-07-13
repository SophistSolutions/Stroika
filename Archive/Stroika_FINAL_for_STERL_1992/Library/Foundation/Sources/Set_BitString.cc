/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Set_BitString__cc
#define	__Set_BitString__cc

/*
 * $Header: /fuji/lewis/RCS/Set_BitString.cc,v 1.18 1992/12/03 05:57:32 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Set_BitString.cc,v $
 *		Revision 1.18  1992/12/03  05:57:32  lewis
 *		Add override of Done () for iterator.
 *
 *		Revision 1.17  1992/11/20  19:31:23  lewis
 *		Got rid of iterator - just use mutator in both places.
 *		Rework mutator to support new it.More() style instead od
 *		old Next/Done ().
 *
 *		Revision 1.16  1992/11/13  03:50:12  lewis
 *		Use (T) instead of <T> in a couple places where I was casting.
 *		Also, be more careful about !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		hacks.
 *
 *		Revision 1.15  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.14  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.13  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.12  1992/10/15  02:33:56  lewis
 *		Move SetRep_BitString<T> here from the header along with iterators
 *		and a few related inlines.
 *		Also, subclass rep from CollectionRep since not done automatically in
 *		Set anymore.
 *
 *		Revision 1.11  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.10  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.9  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.8  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.7  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.6  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.4  1992/07/21  17:27:31  lewis
 *		In UpdateCurrent () we must cast data field to non-const.
 *
 *		Revision 1.3  1992/07/21  05:25:30  lewis
 *		Add cast in call to Remove - converting back and forth between
 *		our type T, and the bitstring indexes. Also, use SizeInContainer
 *		instead of Container::Size even though this hack is only needed
 *		in the macro version, we do it here to facilitate automatically
 *		generated macro version from template.
 *
 *		Revision 1.2  1992/07/19  02:33:48  lewis
 *		Made compatable with latest revs to Collection stuff, cleaning up
 *		template implemenation, and re-translating it into the macro implemenation.
 *
 *		Revision 1.2  1992/05/09  21:41:50  lewis
 *		Fixed minor syntax errors porting to BC++/templates.
 *		
 *
 */


#include	"BitString.hh"
#include	"Collection.hh"
#include	"Debug.hh"

#include	"Set_BitString.hh"






#if		qRealTemplatesAvailable

// Fix GenClass to copy #includes evenif outside #if
#include	"BitString.hh"
#include	"Collection.hh"
#include	"Debug.hh"

template	<class T>	class	SetRep_BitString : public SetRep<T>, public CollectionRep {
	public:
		SetRep_BitString ()							: fMemberMin (1) {}
		SetRep_BitString (size_t hintSize)	: fMemberMin (1) { fData.SetLength (hintSize); }

		override	size_t	GetLength () const;
		override	Boolean	Contains (T item) const;
		override	void	Compact ();
		override	SetRep<T>*	Clone () const;

	 	override	IteratorRep<T>* 	MakeIterator ();
		override	SetIteratorRep<T>*	MakeSetIterator ();
		override	SetMutatorRep<T>* 	MakeSetMutator ();

		override	void	Add (T item);
		override	void	Remove (T item);
		override	void	RemoveAll ();

		BitString	fData;
		long		fMemberMin;
};


template	<class T> class	Set_BitStringMutator : public SetMutatorRep<T>, public CollectionView {
	public:
		Set_BitStringMutator (SetRep_BitString<T>& owner);

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	T		Current () const;

		override	IteratorRep<T>*		Clone () const;

		override	void	RemoveCurrent ();

	private:
		override	void	PatchAdd (CollectionItemKey key);
		override	void	PatchRemove (CollectionItemKey key);
		override	void	PatchRemoveAll ();

		size_t					fIndex;
		SetRep_BitString<T>&	fData;
};




/*
 ********************************************************************************
 ********************************* SetRep_BitString *****************************
 ********************************************************************************
 */
template	<class T> size_t	SetRep_BitString<T>::GetLength () const
{
	size_t	count	=	0;
	for (size_t i = 1; i <= fData.GetLength (); i++) {
		if (fData.GetBit (i)) {
			count++;
		}
	}
	return (count);
}

template	<class T> void	SetRep_BitString<T>::Compact ()
{
}

template	<class T> SetRep<T>*	SetRep_BitString<T>::Clone () const
{
	SetRep_BitString<T>* rep = new SetRep_BitString<T> (GetLength ());
	rep->fData = fData;
	return (rep);
}

template	<class T> IteratorRep<T>*	SetRep_BitString<T>::MakeIterator ()
{
	return (new Set_BitStringMutator<T> (*this));
}

template	<class T> SetIteratorRep<T>*	SetRep_BitString<T>::MakeSetIterator ()
{
	return (new Set_BitStringMutator<T> (*this));
}

template	<class T> SetMutatorRep<T>*	SetRep_BitString<T>::MakeSetMutator ()
{
	return (new Set_BitStringMutator<T> (*this));
}

template	<class T> void	SetRep_BitString<T>::Add (T item)
{
	PatchViewsAdd (&item);
	if (item < fMemberMin) {
		fData.SetLength (fData.GetLength () + fMemberMin - item);
		fData.ShiftRight (fMemberMin - item);
		fMemberMin = item;
	}
	else if (item >= (fMemberMin + fData.GetLength ())) {
		if (fData.GetLength () == 0) {
			fData.SetLength (1, False);
			fMemberMin = item;
		}
		else {
			fData.SetLength (item - fMemberMin + 1, False);
		}
	}
	fData.SetBit (item - fMemberMin + 1, True);
}

template	<class T> void	SetRep_BitString<T>::Remove (T item)
{
	if (item <= fData.GetLength ()) {
		Assert (fData.GetBit (item - fMemberMin + 1));
		fData.SetBit (item - fMemberMin + 1, False);
		PatchViewsRemove (&item);
	}
}

template	<class T> void	SetRep_BitString<T>::RemoveAll ()
{
	PatchViewsRemoveAll ();
	fData.SetLength (0);
	fMemberMin = 1;
}

template	<class T> Boolean	SetRep_BitString<T>::Contains (T item) const
{
	if ((item < fMemberMin) or (item >= fMemberMin + fData.GetLength ())) {
		return (False);
	}
	else {
		Assert (item > fMemberMin - 1);
		return (fData.GetBit (item - fMemberMin + 1));
	}
}




/*
 ********************************************************************************
 ********************************* Set_BitStringMutator *************************
 ********************************************************************************
 */
template	<class T> Set_BitStringMutator<T>::Set_BitStringMutator (SetRep_BitString<T>& owner):
	CollectionView (owner),
	SetMutatorRep<T> (),
	fData (owner),
	fIndex (0)
{
}

template	<class T> Boolean	Set_BitStringMutator<T>::Done () const
{
	return Boolean (fIndex > fData.fData.GetLength ());
}

template	<class T> Boolean	Set_BitStringMutator<T>::More ()
{
	while ((++fIndex <= fData.fData.GetLength ()) and (!fData.fData.GetBit (fIndex)))
		;
	return Boolean (fIndex <= fData.fData.GetLength ());
}

template	<class T> T		Set_BitStringMutator<T>::Current () const
{
	Require (fIndex <= fData.fData.GetLength ());		// otherwise done!
	Assert (fData.fData.GetBit (fIndex));
	return (T) (fIndex - 1 + fData.fMemberMin);
}

template	<class T>	IteratorRep<T>*	Set_BitStringMutator<T>::Clone () const
{
	Set_BitStringMutator<T>*	clone	=	new Set_BitStringMutator<T> (fData);
	clone->fIndex = fIndex;
	return (clone);
}

template	<class T> void		Set_BitStringMutator<T>::RemoveCurrent ()
{
	fData.Remove (Current ());
}

template	<class T> void	Set_BitStringMutator<T>::PatchAdd (CollectionItemKey key)
{
	RequireNotNil (key);
	T*	which	=	(T*)key;
	if (*which < fData.fMemberMin) {
		fIndex += size_t (fData.fMemberMin - *which);
	}
	if (*which - fData.fMemberMin  <= fIndex) {
		fIndex++;
	}
}

template	<class T> void	Set_BitStringMutator<T>::PatchRemove (CollectionItemKey key)
{
	RequireNotNil (key);
	T*	which	=	(T*)key;
	if ((*which - fData.fMemberMin) < fIndex) {
		fIndex--;
	}
}

template	<class T> void	Set_BitStringMutator<T>::PatchRemoveAll ()
{
	fIndex = 1;
}




/*
 ********************************************************************************
 ********************************* Set_BitString ********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Set_BitString;	// tmp hack so GenClass will fixup following CTOR/DTORs
												// harmless, but silly

#endif

template	<class T> Set_BitString<T>::Set_BitString () :
	Set<T> (new SetRep_BitString<T> ())
{
}

template	<class T> Set_BitString<T>::Set_BitString (const Set<T>& src) :
	Set<T> (new SetRep_BitString<T> (src.GetLength ()))
{
	operator+= (src);
}

template	<class T> Set_BitString<T>::Set_BitString (const T* items, size_t size) :
	Set<T> (new SetRep_BitString<T> (size))
{
	AddItems (items, size);
}


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


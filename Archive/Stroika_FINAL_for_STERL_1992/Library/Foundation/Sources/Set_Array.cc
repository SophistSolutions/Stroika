/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Set_Array__cc
#define	__Set_Array__cc

/*
 * $Header: /fuji/lewis/RCS/Set_Array.cc,v 1.22 1992/12/03 05:58:09 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Set_Array.cc,v $
 *		Revision 1.22  1992/12/03  05:58:09  lewis
 *		Get rid of fFirstTime hack - now taken care of by Array iterators themselves.
 *		Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		array to get rid of fOwner field.
 *		Rename Skrunch->Compact.
 *		Use array provided iterators and mutators internally more instead of old
 *		indexing forloops.
 *
 *		Revision 1.21  1992/11/23  22:47:24  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.20  1992/11/20  19:27:37  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.19  1992/11/13  03:49:59  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Also, be more careful about !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
 *		hacks.
 *
 *		Revision 1.18  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.17  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.16  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.15  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.14  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.13  1992/10/29  15:55:50  lewis
 *		Use qCFront_InlinesOpNewDeleteMustDeclareInline to work around MPW bug, and
 *		fix accidental return of void in iterator::next().
 *
 *		Revision 1.12  1992/10/22  04:08:10  lewis
 *		Use new array iterator stuff and no longer use CollectionRep.
 *		Untested - but it does compile.
 *
 *		Revision 1.11  1992/10/15  02:32:53  lewis
 *		Moved Rep and iterator declarations here from the header, and a few
 *		header inlines here as well.
 *
 *		Revision 1.10  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.9  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.8  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.7  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.6  1992/09/04  21:09:36  sterling
 *		Added Contains override.
 *
 *		Revision 1.4  1992/07/21  05:47:54  lewis
 *		Use SizeInContainer instead of Container::Size as a workaround for
 *		CFront 2.1 bugs with nested types, and needed for macros - we do it
 *		in the template here too so we can automate coversion from templates
 *		to macros more easily.
 *
 *		Revision 1.2  1992/07/18  17:01:37  lewis
 *		Updated these classes for latest changes in Collection stuff,
 *		patching, etc, and redid macro based on template version - both
 *		should now be totally in sync, and tested (compiling) macro version
 *		under gcc.
 *
 *		Revision 1.2  1992/05/09  21:36:44  lewis
 *		Fixed minor syntax errors porting to BC++/templates.
 *		
 *
 */



#include	"Array.hh"
#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Set.hh"			// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Set_Array.hh"






#if		qRealTemplatesAvailable

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifdef	inline
			#error "Ooops - I guess my workaround wont work!!!"
		#endif
		#define	inline
	#endif

template	<class T>	class	Set_ArrayIteratorRep;
template	<class T>	class	SetRep_Array : public SetRep<T> {
	public:
		SetRep_Array ();
		~SetRep_Array ();

		override	size_t		GetLength () const;
		override	Boolean		Contains (T item) const;
		override	void		Compact ();
		override	SetRep<T>*	Clone () const;

	 	override	IteratorRep<T>* 	MakeIterator ();
		override	SetIteratorRep<T>*	MakeSetIterator ();
		override	SetMutatorRep<T>* 	MakeSetMutator ();

		override	void	Add (T item);
		override	void	Remove (T item);
		override	void	RemoveAll ();

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		Array_Patch<T>	fData;

	friend	class	Set_Array<T>;
	friend	class	Set_ArrayIteratorRep<T>;
};

template	<class T> class	Set_ArrayIteratorRep : public SetMutatorRep<T> {
	public:
		Set_ArrayIteratorRep (SetRep_Array<T>& owner);

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	T		Current () const;

		override	IteratorRep<T>*		Clone () const;

		override	void	RemoveCurrent ();

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		ForwardArrayMutator_Patch<T>	fIterator;
	friend	class	SetRep_Array<T>;
};





/*
 ********************************************************************************
 ******************************* Set_ArrayIteratorRep ***************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfSet_ArrayIteratorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Set_ArrayIteratorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Set_ArrayIteratorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Set_ArrayIteratorRep<T>::operator delete (void* p)
{
	BlockAllocated<Set_ArrayIteratorRep<T> >::operator delete (p);
}

template	<class	T>	Set_ArrayIteratorRep<T>::Set_ArrayIteratorRep (SetRep_Array<T>& owner) :
	SetMutatorRep<T> (),
	fIterator (owner.fData)
{
}

template	<class	T>	Boolean	Set_ArrayIteratorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	Set_ArrayIteratorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	Set_ArrayIteratorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class T>	IteratorRep<T>*	Set_ArrayIteratorRep<T>::Clone () const
{
	return (new Set_ArrayIteratorRep<T> (*this));
}

template	<class	T>	void	Set_ArrayIteratorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}




/*
 ********************************************************************************
 ************************************* SetRep_Array *****************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfSetRep_ArrayOf<T>_cc
#endif
template	<class	T>	inline	void*	SetRep_Array<T>::operator new (size_t size)
{
	return (BlockAllocated<SetRep_Array<T> >::operator new (size));
}

template	<class	T>	inline	void	SetRep_Array<T>::operator delete (void* p)
{
	BlockAllocated<SetRep_Array<T> >::operator delete (p);
}

template	<class T> inline	SetRep_Array<T>::SetRep_Array ():
	fData ()
{
}

template	<class T> SetRep_Array<T>::~SetRep_Array ()
{
}

template	<class T> size_t	SetRep_Array<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T> void	SetRep_Array<T>::Compact ()
{
	fData.Compact ();
}

template	<class T> SetRep<T>*	SetRep_Array<T>::Clone () const
{
	return (new SetRep_Array<T> (*this));
}

template	<class T> Boolean	SetRep_Array<T>::Contains (T item) const
{
	return (fData.Contains (item));
}

template	<class T> IteratorRep<T>*	SetRep_Array<T>::MakeIterator ()
{
	return (new Set_ArrayIteratorRep<T> (*this));
}

template	<class T> SetIteratorRep<T>*	SetRep_Array<T>::MakeSetIterator ()
{
	return (new Set_ArrayIteratorRep<T> (*this));
}

template	<class T> SetMutatorRep<T>*	SetRep_Array<T>::MakeSetMutator ()
{
	return (new Set_ArrayIteratorRep<T> (*this));
}

template	<class T> void	SetRep_Array<T>::Add (T item)
{
	for (ForwardArrayIterator<T> it (fData); it.More (); ) {
		if (it.Current () == item) {
			return;
		}
	}
	fData.InsertAt (item, GetLength () + 1);
}

template	<class T> void	SetRep_Array<T>::Remove (T item)
{
	for (ForwardArrayIterator<T> it (fData); it.More (); ) {
		if (it.Current () == item) {
			fData.RemoveAt (it.CurrentIndex ());
			return;
		}
	}
}

template	<class T> void	SetRep_Array<T>::RemoveAll ()
{
	fData.RemoveAll ();
}






/*
 ********************************************************************************
 ************************************* Set_Array ********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Set_Array;	// tmp hack so GenClass will fixup following CTOR/DTORs
											// harmless, but silly

#endif

template	<class T> Set_Array<T>::Set_Array () :
	Set<T> (new SetRep_Array<T> ())
{
}

template	<class T> Set_Array<T>::Set_Array (const Set<T>& src) :
	Set<T> (new SetRep_Array<T> ())
{
	SetSlotsAlloced (src.GetLength ());
	operator+= (src);
}

template	<class T> Set_Array<T>::Set_Array (const T* items, size_t size) :
	Set<T> (new SetRep_Array<T> ())
{
	SetSlotsAlloced (size);
	AddItems (items, size);
}

template	<class T> inline	const SetRep_Array<T>*	Set_Array<T>::GetRep () const
{
	return ((const SetRep_Array<T>*) Set<T>::GetRep ());
}

template	<class T> inline	SetRep_Array<T>*		Set_Array<T>::GetRep ()
{
	return ((SetRep_Array<T>*) Set<T>::GetRep ());
}

template	<class T> size_t	Set_Array<T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class T> void	Set_Array<T>::SetSlotsAlloced (size_t slotsAlloced)
{
	GetRep ()->fData.SetSlotsAlloced (slotsAlloced);
}

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifndef	inline
			#error	"How did it get undefined?"
		#endif
		#undef	inline
	#endif


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


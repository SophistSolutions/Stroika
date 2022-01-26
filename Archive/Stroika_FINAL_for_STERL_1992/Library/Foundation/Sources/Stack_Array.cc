/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Stack_Array__cc
#define	__Stack_Array__cc

/*
 * $Header: /fuji/lewis/RCS/Stack_Array.cc,v 1.20 1992/12/04 16:12:11 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Stack_Array.cc,v $
 *		Revision 1.20  1992/12/04  16:12:11  lewis
 *		Comment on better way to copy stacks (with arrays).
 *		Rename RemoveTop -> Pop (and return value).
 *		Reorder methods.
 *
 *		Revision 1.19  1992/12/03  19:20:54  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.18  1992/12/03  05:55:23  lewis
 *		Rename Skrunch->Compact.
 *		Get rid of fFirstTime hack - now taken care of by Array iterators themselves.
 *		Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		array to get rid of fOwner field.
 *
 *		Revision 1.17  1992/11/20  19:29:12  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.16  1992/11/13  03:44:50  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		#include "BlockAllocated.hh".
 *		Work around !qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug.
 *
 *		Revision 1.15  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.14  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.13  1992/11/05  22:12:21  lewis
 *		Fix Stack_ArrayIteratorRep<T>::Stack_ArrayIteratorRep(Stack_ArrayIteratorRep&)
 *		to be const ref.
 *
 *		Revision 1.12  1992/11/01  02:27:49  lewis
 *		Use new Array_Patch_Backward.
 *
 *		Revision 1.11  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.10  1992/10/15  02:36:00  lewis
 *		Moved class StackRep_Array<T> here from header along with related inlines.
 *		And redid CTOR X(X&) either this is wrong or other related ones are wrong - whcih
 *		order does iterator go - think out more carefully.
 *		Also, made Rep subclass from CollectionRep for the time begin.
 *
 *		Revision 1.9  1992/10/14  06:04:02  lewis
 *		Cleanups - moved inlines to implementaion section at end of header
 *		file, and moved things we could to the .cc file. Got rid of method
 *		defintions from inside class declarations.
 *
 *		Revision 1.8  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.7  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.3  1992/09/04  21:10:23  sterling
 *		Added Contains method
 *
 *
 */


#include	"Array.hh"
#include	"BlockAllocated.hh"
#include	"Stack.hh"				// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Stack_Array.hh"




#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T> class	Stack_ArrayIteratorRep;

template	<class T> class	Stack_ArrayRep : public StackRep<T> {
	public:
		Stack_ArrayRep ();

		override	size_t			GetLength () const;
		override	StackRep<T>*	Clone () const;
		override	void			RemoveAll ();
		override	void			Compact ();
		override	IteratorRep<T>*	MakeIterator ();

		override	void			Push (T item);
		override	T				Pop ();
		override	T				Top () const;

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

	friend	class	Stack_Array<T>;
	friend	class	Stack_ArrayIteratorRep<T>;
};


template	<class T>	class	Stack_ArrayIteratorRep : public IteratorRep<T> {
	public:
		Stack_ArrayIteratorRep (Stack_ArrayRep<T>& owner);

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	T		Current () const;

		override	IteratorRep<T>*		Clone () const;

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

	private:
		BackwardArrayIterator_Patch<T>	fIterator;
	friend	class	Stack_ArrayRep<T>;
};







/*
 ********************************************************************************
 ******************************* Stack_ArrayIteratorRep *************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfStack_ArrayIteratorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Stack_ArrayIteratorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Stack_ArrayIteratorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Stack_ArrayIteratorRep<T>::operator delete (void* p)
{
	BlockAllocated<Stack_ArrayIteratorRep<T> >::operator delete (p);
}

template	<class	T>	Stack_ArrayIteratorRep<T>::Stack_ArrayIteratorRep (Stack_ArrayRep<T>& owner) :
	IteratorRep<T> (),
	fIterator (owner.fData)
{
}

template	<class	T>	Boolean	Stack_ArrayIteratorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class	T>	Boolean	Stack_ArrayIteratorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class	T>	T	Stack_ArrayIteratorRep<T>::Current () const
{
	return (fIterator.Current());
}

template	<class T>	IteratorRep<T>*	Stack_ArrayIteratorRep<T>::Clone () const
{
	return (new Stack_ArrayIteratorRep<T> (*this));
}






/*
 ********************************************************************************
 ************************************* Stack_ArrayRep ***************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfStack_ArrayRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Stack_ArrayRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Stack_ArrayRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Stack_ArrayRep<T>::operator delete (void* p)
{
	BlockAllocated<Stack_ArrayRep<T> >::operator delete (p);
}

template	<class T> inline	Stack_ArrayRep<T>::Stack_ArrayRep ()
{
}

template	<class T> size_t	Stack_ArrayRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T> void	Stack_ArrayRep<T>::Compact ()
{
	fData.Compact ();
}

template	<class T> void	Stack_ArrayRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T> IteratorRep<T>*	Stack_ArrayRep<T>::MakeIterator ()
{
	return (new Stack_ArrayIteratorRep<T> (*this));
}

template	<class T> void	Stack_ArrayRep<T>::Push (T item)
{
	fData.InsertAt (item, fData.GetLength () + 1);
}

template	<class T> T	Stack_ArrayRep<T>::Pop ()
{
	T	result	=	fData.GetAt (fData.GetLength ());
 	fData.RemoveAt (fData.GetLength ());
	return (result);
}

template	<class T> T	Stack_ArrayRep<T>::Top () const
{
	return (fData.GetAt (fData.GetLength ()));
}

template	<class T> StackRep<T>*	Stack_ArrayRep<T>::Clone () const
{
	return (new Stack_ArrayRep<T> (*this));
}






/*
 ********************************************************************************
 ************************************* Stack_Array ******************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Stack_Array;	// tmp hack so GenClass will fixup below CTOR
												// harmless, but silly

#endif


template	<class T> Stack_Array<T>::Stack_Array () :
	Stack<T> (new Stack_ArrayRep<T> ())
{
}

template	<class T> Stack_Array<T>::Stack_Array (const Stack<T>& stk) :
	Stack<T> (new Stack_ArrayRep<T> ())
{
	SetSlotsAlloced (stk.GetLength ());

	/*
	 *		Use extra stack to reverse (actually, since we know this
	 *	is an array, we can do much better. Must be done with special CTOR
	 *	to the rep, however).
	 */
	Stack_Array<T>	tmp;
	tmp.SetSlotsAlloced (stk.GetLength ());
	ForEach (T, it, stk) {
		tmp.Push (it.Current ());
	}
	while (not tmp.IsEmpty ()) {
		Push (tmp.Pop ());
	}
}

template	<class T> inline	const Stack_ArrayRep<T>*	Stack_Array<T>::GetRep () const
{
	return ((const Stack_ArrayRep<T>*) Stack<T>::GetRep ());
}

template	<class T> inline	Stack_ArrayRep<T>*	Stack_Array<T>::GetRep ()
{
	return ((Stack_ArrayRep<T>*) Stack<T>::GetRep ());
}

template	<class T> size_t	Stack_Array<T>::GetSlotsAlloced () const
{
	return (GetRep ()->fData.GetSlotsAlloced ());
}

template	<class T> void	Stack_Array<T>::SetSlotsAlloced (size_t slotsAlloced)
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


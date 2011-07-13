/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Stack_LinkList__cc
#define	__Stack_LinkList__cc

/*
 * $Header: /fuji/lewis/RCS/Stack_LinkList.cc,v 1.18 1992/12/04 16:10:47 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Stack_LinkList.cc,v $
 *		Revision 1.18  1992/12/04  16:10:47  lewis
 *		Renamed RemoveTop - Pop(), and got ri dof X(X&) definition.
 *		Reorderd methods. Killed DTOR.
 *
 *		Revision 1.17  1992/12/03  19:20:54  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.16  1992/12/03  05:53:37  lewis
 *		Use new LinkList support where we cannot get at link nodes.
 *		Use X(X&) in clone methods. Let compiler generate these itself.
 *		fOwner field no longer needed.
 *
 *		Revision 1.15  1992/11/20  19:29:12  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.14  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.13  1992/11/03  17:11:11  lewis
 *		Define IteratorRep DTOR - was missing.
 *
 *		Revision 1.12  1992/11/02  19:52:32  lewis
 *		Use new LinkList iterator support. Rename Reps. Use BlockAllocator.
 *		Fixed copy CTOR.
 *
 *		Revision 1.11  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.10  1992/10/15  02:38:44  lewis
 *		Moved StackRep_LinkList<T> here from header and added base class CollectionRep,
 *		as a temp hack. Check on order of iteration. Simone says stack should go top
 *		to bottom.
 *
 *		Revision 1.9  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.8  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.7  1992/10/02  23:42:46  lewis
 *		Fixed type ->Psrcend->Prepend.
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
 *		Revision 1.3  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/04  21:11:23  sterling
 *		Added contains override.
 *
 *
 */

#include	"LinkList.hh"

#include	"Stack_LinkList.hh"




#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T>	class	Stack_LinkListIteratorRep;
template	<class T>	class	Stack_LinkListRep : public StackRep<T> {
	public:
		Stack_LinkListRep ();

		override	StackRep<T>*	Clone () const;
		override	size_t			GetLength () const;
		override	void			RemoveAll ();
		override	void			Compact ();
		override	IteratorRep<T>* MakeIterator ();

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
		LinkList_Patch<T>	fData;

	friend	class	Stack_LinkListIteratorRep<T>;
};


template	<class T> class	Stack_LinkListIteratorRep : public IteratorRep<T> {
	public:
		Stack_LinkListIteratorRep (Stack_LinkListRep<T>& owner);

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
		LinkListIterator_Patch<T>	fIterator;
	friend	class	Stack_LinkListRep<T>;
};







/*
 ********************************************************************************
 ****************************** Stack_LinkListIteratorRep ***********************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfStack_LinkListIteratorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Stack_LinkListIteratorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Stack_LinkListIteratorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Stack_LinkListIteratorRep<T>::operator delete (void* p)
{
	BlockAllocated<Stack_LinkListIteratorRep<T> >::operator delete (p);
}

template	<class T>	Stack_LinkListIteratorRep<T>::Stack_LinkListIteratorRep (Stack_LinkListRep<T>& owner) :
	fIterator (owner.fData)
{
}

template	<class T>	Boolean	Stack_LinkListIteratorRep<T>::Done () const
{
	return (fIterator.Done ());
}

template	<class T>	Boolean	Stack_LinkListIteratorRep<T>::More ()
{
	return (fIterator.More ());
}

template	<class T>	T	Stack_LinkListIteratorRep<T>::Current () const
{
	return (fIterator.Current ());
}

template	<class T>	IteratorRep<T>*	Stack_LinkListIteratorRep<T>::Clone () const
{
	return (new Stack_LinkListIteratorRep<T> (*this));
}






/*
 ********************************************************************************
 ********************************* Stack_LinkListRep ****************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfStack_LinkListRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Stack_LinkListRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Stack_LinkListRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Stack_LinkListRep<T>::operator delete (void* p)
{
	BlockAllocated<Stack_LinkListRep<T> >::operator delete (p);
}

template	<class T>	inline	Stack_LinkListRep<T>::Stack_LinkListRep () :
	StackRep<T> (),
	fData ()
{
}

template	<class T> StackRep<T>*	Stack_LinkListRep<T>::Clone () const
{
	return (new Stack_LinkListRep<T> (*this));
}

template	<class T> size_t	Stack_LinkListRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T> void	Stack_LinkListRep<T>::Compact ()
{
}

template	<class T> IteratorRep<T>*	Stack_LinkListRep<T>::MakeIterator ()
{
	return (new Stack_LinkListIteratorRep<T> (*this));
}

template	<class T> void	Stack_LinkListRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}

template	<class T> void	Stack_LinkListRep<T>::Push (T item)
{
	fData.Prepend (item);
}

template	<class T> T	Stack_LinkListRep<T>::Pop ()
{
	T	result	=	fData.GetFirst ();
 	fData.RemoveFirst ();
	return (result);
}

template	<class T> T	Stack_LinkListRep<T>::Top () const
{
	return (fData.GetFirst ());
}





/*
 ********************************************************************************
 ********************************* Stack_LinkList *******************************
 ********************************************************************************
 */

template	<class	T>	class	Stack_LinkList;	// Tmp hack so GenClass will fixup following CTOR/DTORs
												// Harmless, but silly.


template	<class T> Stack_LinkList<T>::Stack_LinkList () :
	Stack<T> (new Stack_LinkListRep<T> ())
{
}

template	<class T> Stack_LinkList<T>::Stack_LinkList (const Stack<T>& src) :
	Stack<T> (new Stack_LinkListRep<T> ())
{
	/*
	 * Since the iterators go from the top of the stack to the bottom, we must
	 * use a temporary stack to copy.
	 */
	Stack_LinkList<T>	tmp;
	ForEach (T, it, src) {
		tmp.Push (it.Current ());
	}
	while (not tmp.IsEmpty ()) {
		Push (tmp.Pop ());
	}
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


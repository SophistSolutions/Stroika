/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Bag_LinkList__cc
#define	__Bag_LinkList__cc

/*
 * $Header: /fuji/lewis/RCS/Bag_LinkList.cc,v 1.23 1992/12/03 07:06:33 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Bag_LinkList.cc,v $
 *		Revision 1.23  1992/12/03  07:06:33  lewis
 *		Get rid of fFirstTime hack - now taken care of by LinkList iterators
 *		themselves.
 *		Use X(X&) in clone methods - this, coupled with new mutator capability in
 *		linklist to get rid of fOwner field.
 *		New LinkList class gives no access to links, so that caused lots of
 *		changes.
 *		Use new iterators more internally because of this.
 *
 *		Revision 1.22  1992/11/23  21:51:47  lewis
 *		qBCC_InlinesBreakTemplates
 *
 *		Revision 1.21  1992/11/20  21:07:36  lewis
 *		Forgot to initialize fFirstTime in Bag_LinkListMutatorRep::CTOR.
 *
 *		Revision 1.20  1992/11/20  19:22:12  lewis
 *		Use fFirstTime variable as temporary hack to implement new
 *		More() support in iterators. This will go away when backends
 *		use the same technique.
 *
 *		Revision 1.19  1992/11/13  03:39:45  lewis
 *		#include "BlockAllocated.hh" and got rid of archaic qIncluding_Bag_LinkListCC hack.
 *
 *		Revision 1.17  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.16  1992/11/02  19:51:45  lewis
 *		Use new LinkList iterator support. Rename Reps. Use BlockAllocator.
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
 *		Revision 1.13  1992/10/15  02:28:34  lewis
 *		Move BagRep_LinkList<T> here from header and subclass from CollectionRep
 *		since this is not done automatically anymore in Bag.hh.
 *
 *		Revision 1.12  1992/10/14  02:26:02  lewis
 *		Cleanup format - move all inlines to the bottom of the file in
 *		implementation section. Move declarations of concrete iterators
 *		(except bag.hh) to the .cc file.
 *
 *		Revision 1.11  1992/10/13  05:30:40  lewis
 *		UpdateCurrent and RemoveCurrent () were accidentally declared inline.
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
 *		Revision 1.7  1992/09/26  22:27:48  lewis
 *		Updated for new GenClass stuff - got to compile - havent tried linking.
 *
 *		Revision 1.6  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
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
 *		Revision 1.2  1992/09/04  20:56:44  sterling
 *		Contains override.
 *
 *		
 *		
 *
 */

#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"LinkList.hh"

#include	"Bag_LinkList.hh"





#if		qRealTemplatesAvailable

#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
	#ifdef	inline
		#error "Ooops - I guess my workaround wont work!!!"
	#endif
	#define	inline
#endif

template	<class T>	class	Bag_LinkListMutatorRep;
template	<class T>	class	Bag_LinkListRep : public BagRep<T> {
	public:
		Bag_LinkListRep ();
		Bag_LinkListRep (const Bag_LinkListRep<T>& from);

		override	size_t		GetLength () const;
		override	Boolean		Contains (T item) const;
		override	void		Compact ();
		override	BagRep<T>*	Clone () const;

		override	IteratorRep<T>*		MakeIterator ();
		override	BagIteratorRep<T>*	MakeBagIterator ();
		override	BagMutatorRep<T>*	MakeBagMutator ();

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
		LinkList_Patch<T>	fData;

	friend	class	Bag_LinkListMutatorRep<T>;
};



template	<class T> class	Bag_LinkListMutatorRep : public BagMutatorRep<T> {
	public:
		Bag_LinkListMutatorRep (Bag_LinkListRep<T>& owner);
		Bag_LinkListMutatorRep (Bag_LinkListMutatorRep<T>& from);

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	T		Current () const;

		override	IteratorRep<T>*		Clone () const;

		override	void	RemoveCurrent ();
		override	void	UpdateCurrent (T newValue);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);


	private:
		LinkListMutator_Patch<T>	fIterator;
};







/*
 ********************************************************************************
 ****************************** Bag_LinkListMutatorRep **************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfBag_LinkListMutatorRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Bag_LinkListMutatorRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Bag_LinkListMutatorRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Bag_LinkListMutatorRep<T>::operator delete (void* p)
{
	BlockAllocated<Bag_LinkListMutatorRep<T> >::operator delete (p);
}

template	<class T>	Bag_LinkListMutatorRep<T>::Bag_LinkListMutatorRep (Bag_LinkListRep<T>& owner) :
	fIterator (owner.fData)
{
}

template	<class T>	Bag_LinkListMutatorRep<T>::Bag_LinkListMutatorRep (Bag_LinkListMutatorRep<T>& from) :
	fIterator (from.fIterator)
{
}

template	<class T>	Boolean	Bag_LinkListMutatorRep<T>::Done () const
{
	return (fIterator.Done());
}

template	<class T>	Boolean	Bag_LinkListMutatorRep<T>::More ()
{
	return (fIterator.More());
}

template	<class T>	T		Bag_LinkListMutatorRep<T>::Current () const
{
	return (fIterator.Current ());
}

template	<class T>	IteratorRep<T>*	Bag_LinkListMutatorRep<T>::Clone () const
{
	// (~const)...
	return (new Bag_LinkListMutatorRep<T> (*(Bag_LinkListMutatorRep<T>*)this));
}

template	<class	T>	void	Bag_LinkListMutatorRep<T>::UpdateCurrent (T newValue)
{
	fIterator.UpdateCurrent (newValue);
}

template	<class	T>	void	Bag_LinkListMutatorRep<T>::RemoveCurrent ()
{
	fIterator.RemoveCurrent ();
}







/*
 ********************************************************************************
 ********************************* Bag_LinkListRep ******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	BlockAllocatedOfBag_LinkListRepOf<T>_cc
#endif
template	<class	T>	inline	void*	Bag_LinkListRep<T>::operator new (size_t size)
{
	return (BlockAllocated<Bag_LinkListRep<T> >::operator new (size));
}

template	<class	T>	inline	void	Bag_LinkListRep<T>::operator delete (void* p)
{
	BlockAllocated<Bag_LinkListRep<T> >::operator delete (p);
}

template	<class T>	inline	Bag_LinkListRep<T>::Bag_LinkListRep () :
	fData ()
{
}

template	<class T>	inline	Bag_LinkListRep<T>::Bag_LinkListRep (const Bag_LinkListRep<T>& from) :
	fData (from.fData)
{
}

template	<class T>	size_t	Bag_LinkListRep<T>::GetLength () const
{
	return (fData.GetLength ());
}

template	<class T>	void	Bag_LinkListRep<T>::Compact ()
{
}

template	<class T>	BagRep<T>*	Bag_LinkListRep<T>::Clone () const
{
	return (new Bag_LinkListRep<T> (*this));
}

template	<class	T>	Boolean	Bag_LinkListRep<T>::Contains (T item) const
{
	return (fData.Contains (item));
}

template	<class T>	IteratorRep<T>*	Bag_LinkListRep<T>::MakeIterator ()
{
	return (new Bag_LinkListMutatorRep<T> (*this));
}

template	<class T>	BagIteratorRep<T>*	Bag_LinkListRep<T>::MakeBagIterator ()
{
	return (new Bag_LinkListMutatorRep<T> (*this));
}

template	<class T>	BagMutatorRep<T>*	Bag_LinkListRep<T>::MakeBagMutator ()
{
	return (new Bag_LinkListMutatorRep<T> (*this));
}

template	<class T>	void	Bag_LinkListRep<T>::Add (T item)
{
	fData.Prepend (item);
}

template	<class T>	void	Bag_LinkListRep<T>::Remove (T item)
{
	fData.Remove (item);
}

template	<class T>	void	Bag_LinkListRep<T>::RemoveAll ()
{
	fData.RemoveAll ();
}






/*
 ********************************************************************************
 ********************************* Bag_LinkList *********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable || !qGCC_ReDeclareTemplateClassInternalError124Bug
template	<class	T>	class	Bag_LinkList;	// tmp hack so GenClass will fixup below array CTOR
												// harmless, but silly

#endif

template	<class T>	Bag_LinkList<T>::Bag_LinkList () :
	Bag<T> (new Bag_LinkListRep<T> ())
{
}

template	<class T>	Bag_LinkList<T>::Bag_LinkList (const T* items, size_t size) :
	Bag<T> (new Bag_LinkListRep<T> ())
{
	AddItems (items, size);
}

template	<class T>	Bag_LinkList<T>::Bag_LinkList (const Bag<T>& src) :
	Bag<T> (new Bag_LinkListRep<T> ())
{
	operator+= (src);
}

template	<class T>	Bag_LinkList<T>::Bag_LinkList (const Bag_LinkList<T>& src) :
	Bag<T> (src)
{
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


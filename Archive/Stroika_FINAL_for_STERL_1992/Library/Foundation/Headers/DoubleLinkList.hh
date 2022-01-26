/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DoubleLinkList__
#define	__DoubleLinkList__

/*
 * $Header: /fuji/lewis/RCS/DoubleLinkList.hh,v 1.35 1992/12/10 07:12:18 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *		-	Consider if it makes sense for this to be a circular linked list,
 *	and exacly what methods are provided here.
 *
 * Notes:
 *
 *		We completely hide the pointer based implementation of the linked list.
 *	Use mutators to manipulate inside the linked list. This data hiding is not
 *	really necessary, but it encourages easy mixins later.
 *
 *
 * Changes:
 *	$Log: DoubleLinkList.hh,v $
 *		Revision 1.35  1992/12/10  07:12:18  lewis
 *		Totally rewrote based on LinkList and Array code. Got working with
 *		Deque and Queue, but still some problems to be debugged with Sequence_DoubleLinkList.
 *
 *		Revision 1.34  1992/12/04  18:51:35  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.33  1992/12/03  07:31:31  lewis
 *		Quick hack, half assed support for new More() API - seems to work,
 *		however.
 *
 *		Revision 1.32  1992/11/13  03:32:44  lewis
 *		Predeclare template DoubleLinkList before using it as a friend.
 *
 *		Revision 1.30  1992/11/05  22:09:57  lewis
 *		ItemToIndex should take DblLink* - not DoubleLink<T>*.
 *		And fixed mistake in CTOR of patched iterators - linking into chain.
 *
 *		Revision 1.29  1992/11/05  15:19:48  lewis
 *		Forgot to make DoubleLinkList_Patch_X publix - fixed.
 *		Make Patch iterators take const ref to list - no good reason for this
 *		just consistency - must rethink how this should work.
 *		operator new/delete need qCFront_InlinesOpNewDeleteMustDeclareInline hack
 *		for mac.
 *
 *		Revision 1.28  1992/11/04  06:37:10  lewis
 *		Fixed DoubleLinkList Directed iterator in the backwards case, and same
 *		with backwards iterator - forgot to setup so it starts at the end.
 *
 *		Revision 1.27  1992/11/03  22:52:41  lewis
 *		Switch over to new DoubleLinkList and DoubleLinkList_Patch_Forward, etc strategy
 *		for backends.
 *		Debugged (hopefully - not tested yet) iterator patching - based on what
 *		we did in LinkList that did pass testsuites.
 *
 *		Revision 1.26  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.25  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.24  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.23  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.22  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.21  1992/10/07  22:05:49  sterling
 *		Moved remaining inlines in class declaration to Inline section.
 *
 *		Revision 1.20  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.19  1992/09/25  21:13:19  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.18  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.17  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.16  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.15  1992/09/04  19:58:08  sterling
 *		Added Contains () method.
 *
 *		Revision 1.13  1992/07/21  06:17:33  lewis
 *		Elaborate effort to solve the operator new problem for macro version of DoubleLink<T>.
 *		Really must use our own inline version of the BlockAllocated since
 *		too hard to use here - (declaring BlockAllocated(DoubleLink(X))) cannot
 *		be done in macro, etc).
 *
 *		Revision 1.12  1992/07/19  08:20:11  lewis
 *		Fixed typo.
 *
 *		Revision 1.11  1992/07/19  05:39:20  lewis
 *		Added optimized version of IndexToLink (warmed over version of routine
 *		which used to be private method in Sequence_DoubleLinkList - made
 *		no reference to T so we could move it here).
 *
 *		Revision 1.8  1992/07/17  22:06:08  lewis
 *		Rewrote futher. Got rid of use of circular linked list. Added new class
 *		DblLinkList that does all the work, and now DoubleLinkList<T> is NOTHING
 *		but trivial inlines wrappers on base class, so takes up no code (in fact
 *		wrappers really do nothing but casts), with one exception - the DeleteItem()
 *		method. Everything carefully tuned - should be fast and sleek.
 *		Only remaining problem is with block allocation of DoubleLink<T> items
 *		with macros - useing BlockAllocated<T> didn't work well within the
 *		macros - I danced around it for a while, but will probably give up and
 *		just use inline old implemnation, rather than sharing the BlockAllocated<T>
 *		implementation. Sigh.
 *
 *		Revision 1.7  1992/07/17  18:23:32  lewis
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.6  1992/07/17  05:46:29  lewis
 *		Lots of changes - synronized both templated and macro versions of
 *		the class - seperated out class declarations and implementations -
 *		made much more inline. Added DblLink base class to template to
 *		allow sharing of implementation - will do same for DoubleLinkList<T>
 *		too soon. Still some work to go, but this public interface has
 *		not changed and need not immediatly, and the other changes I have in
 *		mind (save getting rid of circularity) wont affect the public interface.
 *
 *		Revision 1.4  1992/07/16  06:09:52  lewis
 *		Change magic include at end to be qRealTemplatesAvailable && qTemplatesHasRepository
 *		instead of qBorlandCPlus.
 *
 *		Revision 1.3  1992/07/08  05:25:19  lewis
 *		Changed file name to include for template .cc stuff to UNIX pathname, and
 *		use PCName mapper to rename to appropriate PC File - cuz we may need to do
 *		this with gcc/aux too.
 *
 *		Revision 1.2  1992/07/02  02:51:24  lewis
 *		Rename DoublyLinkedList to DoubleLinkList.
 *
 *		Revision 1.9  1992/05/09  00:44:32  lewis
 *		Get to compile on PC, and Borland - had to include .cc file to get borland
 *		template stuff to work...
 *
 *		Revision 1.8  92/04/02  17:41:05  17:41:05  lewis (Lewis Pringle)
 *		Did template implementation.
 *		
 *		Revision 1.6  1992/02/21  17:48:02  lewis
 *		Added standard header.
 *		Got rid of g++ workarounds.
 *		Got rid of interup 1,2,3 etc macros since whole things is pretty small
 *		and should work. If it fails, then document with a specific bug #define
 *		so we can keep track! Also, cleanup comments.
 *
 *
 */


#include	"BlockAllocated.hh"
#include	"Debug.hh"




#if		qRealTemplatesAvailable


template	<class	T>	class	DoubleLinkList;
template	<class	T>	class	DoubleLinkList_Patch;
template	<class	T>	class	DoubleLinkListIteratorBase;
template	<class	T>	class	ForwardDoubleLinkListIterator;
template	<class	T>	class	BackwardDoubleLinkListIterator;
template	<class	T>	class	ForwardDoubleLinkListMutator;
template	<class	T>	class	BackwardDoubleLinkListMutator;
template	<class	T>	class	ForwardDoubleLinkListIterator_Patch;
template	<class	T>	class	BackwardDoubleLinkListIterator_Patch;
template	<class	T>	class	ForwardDoubleLinkListMutator_Patch;
template	<class	T>	class	BackwardDoubleLinkListMutator_Patch;

/*
 *		DoubleLink<T> is a generic link node used in a doublelink list. Pardon
 *	the compiler bug workarounds - they make this look more complex
 *	than it really is. This should really be nested in DoubleLinkList<T>
 *	but no compilers I know of support nested templates.
 */
template	<class	T>	class	DoubleLink {
#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	private:
#else
	public:
#endif
		DoubleLink (T item, DoubleLink<T>* prev, DoubleLink<T>* next);

#if		qGCC_BadDefaultCopyConstructorGeneration
		DoubleLink<T> (const DoubleLink<T>& from) :
			fItem (from.fItem),
			fPrev (from.fPrev),
			fNext (from.fNext)
		{
		}
		nonvirtual	const DoubleLink<T>& operator= (const DoubleLink<T>& rhs)
		{
			fItem = rhs.fItem;
			fPrev = rhs.fPrev;
			fNext = rhs.fNext;
			return (*this);
		}
#endif

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	private:
#endif
#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
		inline
#endif
		static	void	operator delete (void* p);

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	private:
#endif
		T				fItem;
		DoubleLink<T>*	fPrev;
		DoubleLink<T>*	fNext;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	DoubleLinkList<T>;
	friend	class	DoubleLinkList_Patch<T>;
	friend	class	DoubleLinkListIteratorBase<T>;
	friend	class	ForwardDoubleLinkListIterator<T>;
	friend	class	BackwardDoubleLinkListIterator<T>;
	friend	class	ForwardDoubleLinkListMutator<T>;
	friend	class	BackwardDoubleLinkListMutator<T>;
	friend	class	ForwardDoubleLinkListIterator_Patch<T>;
	friend	class	BackwardDoubleLinkListIterator_Patch<T>;
	friend	class	ForwardDoubleLinkListMutator_Patch<T>;
	friend	class	BackwardDoubleLinkListMutator_Patch<T>;
#endif
};



/*
 *		DoubleLinkList<T> is a generic double-link (non-intrusive) list implementation.
 *	It keeps a length count so access to its length is rapid. We provide
 *	no public means to access the links themselves.
 *
 *		Since this class provides no patching support, it is not generally
 *	used - more likely you want to use DoubleLinkList_Patch<T>. Use this if you
 *	will manage all patching, or know that none is necessary.
 */
template	<class	T>	class	DoubleLinkList {
	public:
		DoubleLinkList ();
		DoubleLinkList (const DoubleLinkList<T>& from);
		~DoubleLinkList ();

	public:
		nonvirtual	DoubleLinkList<T>& operator= (const DoubleLinkList<T>& list);

	public:
		nonvirtual	size_t	GetLength () const;

	/*
	 *		Basic access to the head/tail of the list. This is what is most
	 *	commonly used, and is most efficient. For other sorts of access, use
	 *	iterators, and mutators.
	 *
	 *		NB: It is an error to access a first or last with an empty list.
	 */
	public:
		nonvirtual	T		GetFirst () const;
		nonvirtual	T		GetLast () const;
		nonvirtual	void	Prepend (T item);
		nonvirtual	void	Append (T item);
		nonvirtual	void	RemoveFirst ();
		nonvirtual	void	RemoveLast ();

	/*
	 *	Utility to search the list for the given item using operator==
	 */
	public:
		nonvirtual	Boolean	Contains (T item) const;


	/*
	 *		Remove is done using operator== to compare. If the item does not
	 *	exist, Remove (T item) has no effect - otherwise it reduces the size
	 *	of the list by one. RemoveAll () sets the size to zero unconditionally.
	 */
	public:
		nonvirtual	void	Remove (T item);
		nonvirtual	void	RemoveAll ();


	/*
	 *		Not alot of point in having these, as they are terribly slow, 
	 *	but the could be convienient.
	 */
	public:
		nonvirtual	T		GetAt (size_t i) const;
		nonvirtual	void	SetAt (T item, size_t i);

	public:
		nonvirtual	void	Invariant () const;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	protected:
#endif
		size_t			fLength;
		DoubleLink<T>*	fFirst;
		DoubleLink<T>*	fLast;

#if		qDebug
		virtual		void	Invariant_ () const;
#endif
#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	ForwardDoubleLinkListIterator<T>;
	friend	class	BackwardDoubleLinkListIterator<T>;
	friend	class	ForwardDoubleLinkListIterator_Patch<T>;
	friend	class	BackwardDoubleLinkListIterator_Patch<T>;
	friend	class	ForwardDoubleLinkListMutator_Patch<T>;
	friend	class	BackwardDoubleLinkListMutator_Patch<T>;
#endif
};



/*
 *		DoubleLinkListIteratorBase<T> is an un-advertised implementation
 *	detail designed to help in source-code sharing among various
 *	iterator implementations.
 */
template	<class T>	class	DoubleLinkListIteratorBase {
	protected:
		DoubleLinkListIteratorBase (const DoubleLinkList<T>& data);

	public:
		nonvirtual	T		Current () const;			//	Error to call if Done (), otherwise OK
		nonvirtual	size_t	CurrentIndex () const;		//	NB: This can be called if we are done - if so, it returns 1 past dir we were iterating.
		nonvirtual	Boolean	Done () const;

		nonvirtual	void	Invariant () const;

	protected:
#if		qDebug
		const DoubleLinkList<T>*	fData;
#endif
		const DoubleLink<T>*		fCurrent;
		size_t						fCurrentIndex;
		Boolean						fSupressMore;	// Indicates if More should do anything, or if were already Mored...

#if		qDebug
		virtual	void	Invariant_ () const;
#endif
};



/*
 *		Use this iterator to iterate forwards over the DoubleLinkList. Be careful
 *	not to add or remove things from the DoubleLinkList while using this iterator,
 *	since it is not safe. Use ForwardDoubleLinkListIterator_Patch for those cases.
 */
template	<class T> class	ForwardDoubleLinkListIterator : public DoubleLinkListIteratorBase<T> {
	public:
		ForwardDoubleLinkListIterator (const DoubleLinkList<T>& data);

		nonvirtual	Boolean	More ();
};




/*
 *		ForwardDoubleLinkListMutator<T> is the same as ForwardDoubleLinkListIterator<T> but
 *	adds the ability to update the contents of the DoubleLinkList as you go along.
 */
template	<class T> class	ForwardDoubleLinkListMutator : public ForwardDoubleLinkListIterator<T> {
	public:
		ForwardDoubleLinkListMutator (DoubleLinkList<T>& data);

		nonvirtual	void	UpdateCurrent (T newValue);
};




/*
 *		Use this iterator to iterate backwards over the DoubleLinkList. Be careful
 *	not to add or remove things from the DoubleLinkList while using this iterator,
 *	since it is not safe. Use BackwardDoubleLinkListIterator_Patch for those cases.
 */
template	<class T> class	BackwardDoubleLinkListIterator : public DoubleLinkListIteratorBase<T> {
	public:
		BackwardDoubleLinkListIterator (const DoubleLinkList<T>& data);

		nonvirtual	Boolean	More ();
};




/*
 *		BackwardDoubleLinkListMutator<T> is the same as BackwardDoubleLinkListIterator<T> but
 *	adds the ability to update the contents of the DoubleLinkList as you go along.
 */
template	<class T> class	BackwardDoubleLinkListMutator : public BackwardDoubleLinkListIterator<T> {
	public:
		BackwardDoubleLinkListMutator (DoubleLinkList<T>& data);

		nonvirtual	void	UpdateCurrent (T newValue);
};











/*
 *	Patching Support:
 *
 *		Here we provide Patching Versions of each iterator, and for convienience
 *	versions of DoubleLinkList that maintain a list of all Patching iterators of a given
 *	type.
 */


template	<class T>	class	DoubleLinkListIterator_PatchBase;


/*
 *		DoubleLinkList_Patch<T> is an DoubleLinkList implemantion that keeps
 *	a list of patchable iterators, and handles the patching automatically for
 *	you. Use this if you ever plan to use patchable iterators.
 */
template	<class T>	class	DoubleLinkList_Patch : public DoubleLinkList<T> {
	public:
		DoubleLinkList_Patch ();
		DoubleLinkList_Patch (const DoubleLinkList_Patch<T>& from);
		~DoubleLinkList_Patch ();

		nonvirtual	DoubleLinkList_Patch<T>& operator= (const DoubleLinkList_Patch<T>& rhs);

	/*
	 * Methods we shadow so that patching is done. If you want to circumvent the
	 * patching, thats fine - use scope resolution operator to call this's base
	 * class version.
	 */
	public:
		nonvirtual	void	Prepend (T item);
		nonvirtual	void	Append (T item);
		nonvirtual	void	Remove (T item);
		nonvirtual	void	RemoveFirst ();
		nonvirtual	void	RemoveLast ();
		nonvirtual	void	RemoveAll ();


	/*
	 *		Methods to do the patching yourself. Iterate over all
	 *	the iterators and perfrom patching.
	 */
	public:
		nonvirtual	Boolean	HasActiveIterators () const;			//	are there any iterators to be patched?
		nonvirtual	void	PatchViewsAdd (size_t index) const;		//	call after add
		nonvirtual	void	PatchViewsRemove (size_t index) const;	//	call before remove
		nonvirtual	void	PatchViewsRemoveAll () const;			//	call after removeall


	/*
	 *	Check Invariants for this class, and all the iterators we own.
	 */
	public:
		nonvirtual	void	Invariant () const;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	private:
#endif
		DoubleLinkListIterator_PatchBase<T>*	fIterators;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	DoubleLinkListIterator_PatchBase<T>;
#endif

#if		qDebug
		override	void	Invariant_ () const;
		nonvirtual	void	InvariantOnIterators_ () const;
#endif
};




/*
 *		DoubleLinkListIterator_PatchBase<T> is a private utility class designed
 *	to promote source code sharing among the patched iterator implemetnations.
 */
template	<class T>	class	DoubleLinkListIterator_PatchBase : public DoubleLinkListIteratorBase<T> {

// Want to make these CTORs protected, but gcc complained - lookup in ARM, and see
// whose right - I'm pretty sure should be fine!!!
// LGP Wednesday, December 9, 1992 11:31:08 PM
	public:
		DoubleLinkListIterator_PatchBase (const DoubleLinkList_Patch<T>& data);
		DoubleLinkListIterator_PatchBase (const DoubleLinkListIterator_PatchBase<T>& from);
		~DoubleLinkListIterator_PatchBase ();

		nonvirtual	DoubleLinkListIterator_PatchBase<T>&	operator= (const DoubleLinkListIterator_PatchBase<T>& rhs);

	/*
	 *
	 *		Note that we use index here instead of link pointer since
	 *	we will need to update the fCurrentIndex anyhow, and can only
	 *	tell if and how to do this based on the index. Then given the index
	 *	we don't need the link, since if its relevant, we'll have it handy
	 *	anyhow (fCurrent).
	 */
	public:
		nonvirtual	void	PatchAdd (size_t index);		//	call after add
		nonvirtual	void	PatchRemove (size_t index);		//	call before remove
		nonvirtual	void	PatchRemoveAll ();				//	call after removeall

	public:
#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	protected:
#endif
		const DoubleLinkList_Patch<T>* 			fData;
		DoubleLinkListIterator_PatchBase<T>*	fNext;

#if		qDebug
		override	void	Invariant_ () const;
#endif

		virtual		void	PatchRemoveCurrent ()	=	Nil;	// called from patchremove if patching current item...

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	DoubleLinkList_Patch<T>;
#endif
};




/*
 *		ForwardDoubleLinkListIterator_Patch<T> is forwards iterator that can be used
 *	while modifing its owned DoubleLinkList. It can only be used with DoubleLinkList_Patch<T>
 *	since the classes know about each other, and keep track of each other.
 *	This is intended to be a convienience in implementing concrete container
 *	mixins.
 */
template	<class T> class	ForwardDoubleLinkListIterator_Patch : public DoubleLinkListIterator_PatchBase<T> {
	public:
		ForwardDoubleLinkListIterator_Patch (const DoubleLinkList_Patch<T>& data);

	public:
		nonvirtual	Boolean	More ();

	protected:
		override	void	PatchRemoveCurrent ();
};




/*
 *		ForwardDoubleLinkListMutator_Patch<T> is the same as ForwardDoubleLinkListIterator_Patch<T> but
 *	adds the ability to update the contents of the DoubleLinkList as you go along.
 */
template	<class T> class	ForwardDoubleLinkListMutator_Patch : public ForwardDoubleLinkListIterator_Patch<T> {
	public:
		ForwardDoubleLinkListMutator_Patch (DoubleLinkList_Patch<T>& data);

	public:
		nonvirtual	void	RemoveCurrent ();
		nonvirtual	void	UpdateCurrent (T newValue);
		nonvirtual	void	AddBefore (T item);				//	NB: Can be called if done
		nonvirtual	void	AddAfter (T item);
};




/*
 *		BackwardDoubleLinkListIterator_Patch<T> is backwards iterator that can be used
 *	while modifing its owned DoubleLinkList. It can only be used with DoubleLinkList_Patch<T>
 *	since the classes know about each other, and keep track of each other.
 *	This is intended to be a convienience in implementing concrete container
 *	mixins.
 */
template	<class T> class	BackwardDoubleLinkListIterator_Patch : public DoubleLinkListIterator_PatchBase<T> {
	public:
		BackwardDoubleLinkListIterator_Patch (const DoubleLinkList_Patch<T>& data);

	public:
		nonvirtual	Boolean	More ();

	protected:
		override	void	PatchRemoveCurrent ();
};




/*
 *		BackwardDoubleLinkListMutator_Patch<T> is the same as BackwardDoubleLinkListIterator_Patch<T> but
 *	adds the ability to update the contents of the DoubleLinkList as you go along.
 */
template	<class T> class	BackwardDoubleLinkListMutator_Patch : public BackwardDoubleLinkListIterator_Patch<T> {
	public:
		BackwardDoubleLinkListMutator_Patch (DoubleLinkList_Patch<T>& data);

	public:
		nonvirtual	void	RemoveCurrent ();
		nonvirtual	void	UpdateCurrent (T newValue);
		nonvirtual	void	AddBefore (T item);
		nonvirtual	void	AddAfter (T item);				//	NB: Can be called if done
};





#endif	/*qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */




#if		qRealTemplatesAvailable

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifdef	inline
			#error "Ooops - I guess my workaround wont work!!!"
		#endif
		#define	inline
	#endif

	// class Link<T>
	template	<class T>	inline	DoubleLink<T>::DoubleLink (T item, DoubleLink<T>* prev, DoubleLink<T>* next) :
		fItem (item),
		fPrev (prev),
		fNext (next)
	{
	}
	#if		!qRealTemplatesAvailable
		#include	"TFileMap.hh"
		#include	BlockAllocatedOfDoubleLinkOf<T>_hh
	#endif
	template	<class	T>	inline	void*	DoubleLink<T>::operator new (size_t size)
	{
		return (BlockAllocated<DoubleLink<T> >::operator new (size));
	}
	template	<class	T>	inline	void	DoubleLink<T>::operator delete (void* p)
	{
		BlockAllocated<DoubleLink<T> >::operator delete (p);
	}


	// class DoubleLinkList<T>
	template	<class T>	inline	void	DoubleLinkList<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
#endif
	}
	template	<class T>	inline	DoubleLinkList<T>::DoubleLinkList () :
		fLength (0),
		fFirst (Nil),
		fLast (Nil)
	{
		Invariant ();
	}
	template	<class T>	inline	DoubleLinkList<T>::~DoubleLinkList ()
	{
		/*
		 * This could be a little cheaper - we could avoid setting fLength field,
		 * and fFirst pointer, but we must worry more about codeSize/re-use.
		 * That would involve a new function that COULD NOT BE INLINED.
		 *
		 * < I guess  I could add a hack method - unadvertised - but has to be
		 *   at least protected - and call it here to do what I've mentioned above >
		 */
		Invariant ();
		RemoveAll ();
		Invariant ();
		Ensure (fLength == 0);
		Ensure (fFirst == Nil);
		Ensure (fLast == Nil);
	}
	template	<class T>	inline	size_t	DoubleLinkList<T>::GetLength () const
	{
		return (fLength);
	}
	template	<class T>	inline	T	DoubleLinkList<T>::GetFirst () const
	{
		Require (fLength > 0);
		AssertNotNil (fFirst);
		return (fFirst->fItem);
	}
	template	<class T>	inline	T	DoubleLinkList<T>::GetLast () const
	{
		Require (fLength > 0);
		AssertNotNil (fLast);
		return (fLast->fItem);
	}
	template	<class T>	inline	void	DoubleLinkList<T>::Prepend (T item)
	{
		Invariant ();

		/*
		 * Patch items following link (or DblLinkLists value) to point to item.
		 */
		if (fFirst == Nil) {
			Assert (fLast == Nil);
			Assert (fLength == 0);
			fFirst = new DoubleLink<T> (item, Nil, Nil);
			fLast = fFirst;
		}
		else {
			/*
			 *		No need to worry about fLast since if there is at least one item,
			 *	fLast, and its previous don't change.
			 */
			Assert (fFirst->fPrev == Nil);	// awlays true of first item...
			fFirst->fPrev =  new DoubleLink<T> (item, Nil, fFirst);
			fFirst = fFirst->fPrev;
		}

		fLength++;
		Invariant ();
	}
	template	<class T>	inline	void	DoubleLinkList<T>::Append (T item)
	{
		Invariant ();

		DoubleLink<T>*	newLast = new DoubleLink<T> (item, Nil, fLast);
	
		/*
		 * Patch items previous link (or DblLinkLists value) to point to item.
		 */
		if (fLast == Nil) {
			Assert (fFirst == Nil);
			Assert (fLength == 0);
			fLast = new DoubleLink<T> (item, Nil, Nil);
			fFirst = fLast;
		}
		else {
			/*
			 *		No need to worry about fFirst since if there is at least one item,
			 *	fFirst, and its next don't change.
			 */
			Assert (fLast->fNext == Nil);	// awlays true of last item...
			fLast->fNext =  new DoubleLink<T> (item, fLast, Nil);
			fLast = fLast->fNext;
		}
	
		fLength++;
		Invariant ();
	}
	template	<class T>	inline	void	DoubleLinkList<T>::RemoveFirst ()
	{
		Require (fLength > 0);
		RequireNotNil (fFirst);
		Invariant ();

		DoubleLink<T>* victim = fFirst;
		Assert (victim->fPrev == Nil);		// cuz it was first..

		/*
		 * Before:
		 *	|        |   	|        |   	|        |
		 *	|    V   |   	|    B   |   	|   C    |
		 *	| <-prev |   	| <-prev |   	| <-prev | ...
		 *	| next-> |   	| next-> |   	| next-> |
		 *	|        |   	|        |   	|        |
		 *
		 * After:
		 *	|        |   	|        |
		 *	|   B    |   	|    C   |
		 *	| <-prev |   	| <-prev | ...
		 *	| next-> |   	| next-> |
		 *	|        |   	|        |
		 */
		fFirst = victim->fNext;				// First points to B
		if (fFirst == Nil) {
			Assert (fLength == 1);			// soon to be zero
			Assert (victim == fLast);
			fLast = Nil;
		}
		else {
			Assert (fFirst->fPrev == victim);
			Assert (fLength > 1);
			fFirst->fPrev = Nil;			// B's prev is Nil since it is new first
		}

		delete (victim);
		fLength--;

		Invariant ();
	}
	template	<class T>	inline	void	DoubleLinkList<T>::RemoveLast ()
	{
		Require (fLength > 0);
		RequireNotNil (fFirst);
		Invariant ();

		DoubleLink<T>* victim = fLast;
		Assert (victim->fNext == Nil);		// cuz it was last..

		/*
		 * Before:
		 *	    |        |   	|        |   	|        |
		 *	    |    A   |   	|    B   |   	|   V    |
		 *	... | <-prev |   	| <-prev |   	| <-prev |
		 *	    | next-> |   	| next-> |   	| next-> |
		 *	    |        |   	|        |   	|        |
		 *
		 * After:
		 *	    |        |   	|        |
		 *	    |   A    |   	|    B   |
		 *	... | <-prev |   	| <-prev |
		 *	    | next-> |   	| next-> |
		 *	    |        |   	|        |
		 */

		fLast = victim->fPrev;				// new last item
		if (fLast == Nil) {
			Assert (fLength == 1);			// soon to be zero
			Assert (fFirst == victim);
			fFirst = Nil;
		}
		else {
			Assert (fLast->fNext == victim);
			Assert (fLength > 1);
			fLast->fNext = Nil;				// B's fNext is Nil since it is new last
		}

		delete (victim);
		fLength--;

		Invariant ();
	}


	// Class DoubleLinkListIteratorBase<T>
	template	<class T>	inline	void	DoubleLinkListIteratorBase<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
#endif
	}
	template	<class T>	inline	DoubleLinkListIteratorBase<T>::DoubleLinkListIteratorBase (const DoubleLinkList<T>& data) :
#if		qDebug
		fData (&data),
#endif
		//fCurrent (),					;; initialized in subclases...
		//fCurrentIndex (),				;; initialized in subclases...
		fSupressMore (True)				// first time thru - cuz of how used in for loops...
	{
		#if		qDebug
			fCurrent = Nil;		// more likely to cause bugs...
			fCurrentIndex = 0;
		#endif
		/*
		 * Cannot call invariant () here since fCurrent/fCurrentIndex not yet setup.
		 */
	}
	template	<class T>	inline	Boolean	DoubleLinkListIteratorBase<T>::Done () const
	{
		Invariant ();
		return Boolean (fCurrent == Nil);
	}
	template	<class T>	inline	size_t	DoubleLinkListIteratorBase<T>::CurrentIndex () const
	{
		/*
		 *		NB: This can be called if we are done - if so, it returns 0 or
		 *	GetLength() + 1, depending on iteration direction.
		 */
		Invariant ();
		Ensure (fCurrentIndex <= fData->GetLength () + 1);
		return (fCurrentIndex);
	}
	template	<class T>	inline	T		DoubleLinkListIteratorBase<T>::Current () const
	{
		Require (not Done ());
		Invariant ();
		return (fCurrent->fItem);
	}


	// Class ForwardDoubleLinkListIterator<T>
	template	<class T>	inline	ForwardDoubleLinkListIterator<T>::ForwardDoubleLinkListIterator (const DoubleLinkList<T>& data) :
		DoubleLinkListIteratorBase<T>(data)
	{
		fCurrent = data.fFirst;
		fCurrentIndex = 1;
		Invariant ();
	}
	template	<class T>	inline	Boolean	ForwardDoubleLinkListIterator<T>::More ()
	{
		Invariant ();
		if (not fSupressMore and not Done ()) {
			AssertNotNil (fCurrent);
			fCurrent = fCurrent->fNext;
		}
		fSupressMore = False;
		Invariant ();
		return (not Done ());
	}


	// Class ForwardDoubleLinkListMutator<T>
	template	<class T>	inline	ForwardDoubleLinkListMutator<T>::ForwardDoubleLinkListMutator (DoubleLinkList<T>& data) :
		ForwardDoubleLinkListIterator<T>((const DoubleLinkList<T>&)data)
	{
		Invariant ();
	}
	template	<class T>	inline	void	ForwardDoubleLinkListMutator<T>::UpdateCurrent (T newValue)
	{
		Invariant ();
		Require (not Done ());
		AssertNotNil (fCurrent);
		((DoubleLink<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better
														// the (~const)
	}


	// Class BackwardDoubleLinkListIterator<T>
	template	<class T>	inline	BackwardDoubleLinkListIterator<T>::BackwardDoubleLinkListIterator (const DoubleLinkList<T>& data) :
		DoubleLinkListIteratorBase<T>(data)
	{
		fCurrent = data.fLast;
		fCurrentIndex = data.GetLength ();
		Invariant ();
	}
	template	<class T>	inline	Boolean	BackwardDoubleLinkListIterator<T>::More ()
	{
		Invariant ();
		if (fSupressMore) {
			fSupressMore = False;
		}
		else {
			if (Done ()) {
				return (False);
			}
			else {
				AssertNotNil (fCurrent);
				fCurrent = fCurrent->fPrev;
			}
		}
		return (not Done ());
	}

	// Class BackwardDoubleLinkListMutator<T>
	template	<class T>	inline	BackwardDoubleLinkListMutator<T>::BackwardDoubleLinkListMutator (DoubleLinkList<T>& data) :
		BackwardDoubleLinkListIterator<T>((const DoubleLinkList<T>&)data)
	{
		Invariant ();
	}
	template	<class T>	inline	void	BackwardDoubleLinkListMutator<T>::UpdateCurrent (T newValue)
	{
		Invariant ();
		Require (not Done ());
		AssertNotNil (fCurrent);
		((DoubleLink<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better
														// the (~const)
	}





	/*
	 **************************** Patching code ****************************
	 */

	// Class DoubleLinkListIterator_PatchBase<T>
	template	<class T>	inline	DoubleLinkListIterator_PatchBase<T>::DoubleLinkListIterator_PatchBase (const DoubleLinkList_Patch<T>& data) :
		DoubleLinkListIteratorBase<T>(data),
		fData (&data),
		fNext (data.fIterators)
	{
		//(~const)
		((DoubleLinkList_Patch<T>*)fData)->fIterators = this;
		/*
		 * Cannot call invariant () here since fCurrent not yet setup.
		 */
	}
	template	<class T>	inline	DoubleLinkListIterator_PatchBase<T>::DoubleLinkListIterator_PatchBase (const DoubleLinkListIterator_PatchBase<T>& from) :
		DoubleLinkListIteratorBase<T>(from),
		fData (from.fData),
		fNext (from.fData->fIterators)
	{
		RequireNotNil (fData);
		//(~const)
		((DoubleLinkList_Patch<T>*)fData)->fIterators = this;
		Invariant ();
	}
	template	<class T>	inline	DoubleLinkListIterator_PatchBase<T>::~DoubleLinkListIterator_PatchBase ()
	{
		Invariant ();
		AssertNotNil (fData);
		if (fData->fIterators == this) {
			//(~const)
			((DoubleLinkList_Patch<T>*)fData)->fIterators = fNext;
		}
		else {
			for (DoubleLinkListIterator_PatchBase<T>* v = fData->fIterators; v->fNext != this; v = v->fNext) {
				AssertNotNil (v);
				AssertNotNil (v->fNext);
			}
			AssertNotNil (v);
			Assert (v->fNext == this);
			v->fNext = fNext;
		}
	}
	template	<class T>	inline	DoubleLinkListIterator_PatchBase<T>&	DoubleLinkListIterator_PatchBase<T>::operator= (const DoubleLinkListIterator_PatchBase<T>& rhs)
	{
		Invariant ();

		/*
		 *		If the fData field has not changed, then we can leave alone our iterator linkage.
		 *	Otherwise, we must remove ourselves from the old, and add ourselves to the new.
		 */
		if (fData != rhs.fData) {
			AssertNotNil (fData);
			AssertNotNil (rhs.fData);

			/*
			 * Remove from old.
			 */
			if (fData->fIterators == this) {
				//(~const)
				((DoubleLinkList_Patch<T>*)fData)->fIterators = fNext;
			}
			else {
				for (DoubleLinkListIterator_PatchBase<T>* v = fData->fIterators; v->fNext != this; v = v->fNext) {
					AssertNotNil (v);
					AssertNotNil (v->fNext);
				}
				AssertNotNil (v);
				Assert (v->fNext == this);
				v->fNext = fNext;
			}

			/*
			 * Add to new.
			 */
			fData = rhs.fData;
			fNext = rhs.fData->fIterators;
			//(~const)
			((DoubleLinkList_Patch<T>*)fData)->fIterators = this;
		}

#if		qGCC_OpEqualsNotAutoDefinedSoCanBeExplicitlyCalledBug
		(*(DoubleLinkListIteratorBase<T>*)this) = (*(DoubleLinkListIteratorBase<T>*)&rhs);
#else
		DoubleLinkListIteratorBase<T>::operator=(rhs);
#endif
		Invariant ();
		return (*this);
	}
	template	<class T>	inline	void	DoubleLinkListIterator_PatchBase<T>::PatchAdd (size_t index)
	{
		Require (index >= 1);
		Require (index <= fData->GetLength ());
		/*
		 *		We never need to patch our link pointer as the result of and add.
		 *	We only have to re-adjust our fCurrentIndex field if we've changed
		 *	which index we are pointing at.
		 */
		if (index <= fCurrentIndex) {
			fCurrentIndex++;
		}
	}
	template	<class T>	inline	void	DoubleLinkListIterator_PatchBase<T>::PatchRemove (size_t index)
	{
		Require (index >= 1);
		Require (index <= fData->GetLength ());

		/*
		 *		If we are removing an item from the right of our cursor, it has no effect
		 *	on our index, and obviously not on our fCurrent field, so we ignore it.
		 *
		 *		If we are removing an item from the left of the cursor, our index
		 *	value is too great by 1.
		 *
		 *		If we are removing the current item, what we must do depends on our
		 *	subclass, so we defer to it via a virtual method call.
		 */
		if (index < fCurrentIndex) {
			fCurrentIndex--;
		}
		else if (index == fCurrentIndex) {
			PatchRemoveCurrent ();
		}
	}
	template	<class T>	inline	void	DoubleLinkListIterator_PatchBase<T>::PatchRemoveAll ()
	{
		Require (fData->GetLength () == 0);		//	since called after removeall
		fCurrent = Nil;
		fCurrentIndex = 0;
	}


	// class DoubleLinkList_Patch<T>
	template	<class	T>	inline	void	DoubleLinkList_Patch<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
		InvariantOnIterators_ ();
#endif
	}
	template	<class	T>	inline	DoubleLinkList_Patch<T>::DoubleLinkList_Patch () :
		DoubleLinkList<T> (),
		fIterators (Nil)
	{
		Invariant ();
	}
	template	<class	T>	inline	DoubleLinkList_Patch<T>::DoubleLinkList_Patch (const DoubleLinkList_Patch<T>& from) :
		DoubleLinkList<T> (from),
		fIterators (Nil)	// Don't copy the list of iterators - would be trouble with backpointers!
							// Could clone but that would do no good, since nobody else would have pointers to them
	{
		Invariant ();
	}
	template	<class	T>	inline	DoubleLinkList_Patch<T>::~DoubleLinkList_Patch ()
	{
		Require (fIterators == Nil);
		Invariant ();
	}
	template	<class	T>	inline	Boolean	DoubleLinkList_Patch<T>::HasActiveIterators () const
	{
		return Boolean (fIterators != Nil);
	}
	template	<class T>	inline	void	DoubleLinkList_Patch<T>::PatchViewsAdd (size_t index) const
	{
		for (DoubleLinkListIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchAdd (index);
		}
	}
	template	<class T>	inline	void	DoubleLinkList_Patch<T>::PatchViewsRemove (size_t index) const
	{
		for (DoubleLinkListIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRemove (index);
		}
	}
	template	<class T>	inline	void	DoubleLinkList_Patch<T>::PatchViewsRemoveAll () const
	{
		for (DoubleLinkListIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRemoveAll ();
		}
	}
	template	<class	T>	inline	DoubleLinkList_Patch<T>& DoubleLinkList_Patch<T>::operator= (const DoubleLinkList_Patch<T>& rhs)
	{
		/*
		 * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
		 * If this is to be supported at some future date, well need to work on our patching.
		 */
		Assert (not (HasActiveIterators ()));	// cuz copy of DoubleLinkList does not copy iterators...
		Invariant ();
		DoubleLinkList<T>::operator= (rhs);
		Invariant ();
		return (*this);
	}
	template	<class	T>	inline	void	DoubleLinkList_Patch<T>::Prepend (T item)
	{
		Invariant ();
		DoubleLinkList<T>::Prepend (item);
		PatchViewsAdd (1);
		Invariant ();
	}
	template	<class	T>	inline	void	DoubleLinkList_Patch<T>::Append (T item)
	{
		Invariant ();
		DoubleLinkList<T>::Append (item);
		PatchViewsAdd (GetLength ());
		Invariant ();
	}
	template	<class	T>	inline	void	DoubleLinkList_Patch<T>::RemoveFirst ()
	{
		Invariant ();
		PatchViewsRemove (1);
		DoubleLinkList<T>::RemoveFirst ();
		Invariant ();
	}
	template	<class	T>	inline	void	DoubleLinkList_Patch<T>::RemoveLast ()
	{
		Invariant ();
		PatchViewsRemove (GetLength ());
		DoubleLinkList<T>::RemoveLast ();
		Invariant ();
	}
	template	<class	T>	inline	void	DoubleLinkList_Patch<T>::RemoveAll ()
	{
		Invariant ();
		DoubleLinkList<T>::RemoveAll ();
		PatchViewsRemoveAll ();		// PatchRealloc not needed cuz removeall just destructs things,
									// it does not realloc pointers (ie does not call setslotsalloced).
		Invariant ();
	}


	// class ForwardDoubleLinkListIterator_Patch<T>
	template	<class T>	inline	ForwardDoubleLinkListIterator_Patch<T>::ForwardDoubleLinkListIterator_Patch (const DoubleLinkList_Patch<T>& data) :
		DoubleLinkListIterator_PatchBase<T> (data)
	{
		fCurrent = data.fFirst;
		fCurrentIndex = 1;
		Invariant ();
	}
	template	<class T>	inline	Boolean	ForwardDoubleLinkListIterator_Patch<T>::More ()
	{
		// This is IDENTICAL to what we do in ForwardDoubleLinkListIterator<T>::More ()
		Invariant ();
		if (not fSupressMore and not Done ()) {
			AssertNotNil (fCurrent);
			fCurrent = fCurrent->fNext;
		}
		fSupressMore = False;
		Invariant ();
		return (not Done ());
	}
	template	<class T>	inline	void	ForwardDoubleLinkListIterator_Patch<T>::PatchRemoveCurrent ()
	{
		fCurrent = fCurrent->fNext;
		fSupressMore = True;
	}


	// Class ForwardDoubleLinkListMutator<T>
	template	<class T>	inline	ForwardDoubleLinkListMutator_Patch<T>::ForwardDoubleLinkListMutator_Patch (DoubleLinkList_Patch<T>& data) :
		ForwardDoubleLinkListIterator_Patch<T>((const DoubleLinkList_Patch<T>&)data)
	{
		Invariant ();
	}
	template	<class T>	inline	void	ForwardDoubleLinkListMutator_Patch<T>::RemoveCurrent ()
	{
		Require (not Done ());
		Invariant ();
		DoubleLink<T>*	victim	=	(DoubleLink<T>*)fCurrent;	//	(~const)
		AssertNotNil (victim);									// cuz not done
		AssertNotNil (fData);
		fData->PatchViewsRemove (fCurrentIndex);
		Assert (fCurrent != victim);				// patching should  have guaranteed this


		/*
		 * Before:
		 *	    |        |   	|        |   	|        |
		 *	    |    A   |   	|    V   |   	|   C    |
		 *	... | <-prev |   	| <-prev |   	| <-prev |  ...
		 *	    | next-> |   	| next-> |   	| next-> |
		 *	    |        |   	|        |   	|        |
		 *
		 * After:
		 *	    |        |   	|        |
		 *	    |   A    |   	|    C   |
		 *	... | <-prev |   	| <-prev | ...
		 *	    | next-> |   	| next-> |
		 *	    |        |   	|        |
		 */
		if (victim->fPrev == Nil) {
			// In this case 'A' does not exist - it is Nil...
			Assert (fData->fFirst == victim);
			((DoubleLinkList_Patch<T>*)fData)->fFirst = victim->fNext;		//	(~const)- 'C' is now first
			if (fData->fFirst == Nil) {
				Assert (fData->fLength == 1);	// there was just one entry...
				((DoubleLinkList_Patch<T>*)fData)->fLast = Nil;				//	(~const)
			}
			else {
				Assert (fData->fFirst->fPrev = victim);						// Victim used to be 'C's prev
				fData->fFirst->fPrev = Nil;									// Now Nil!
			}
		}
		else {
			Assert (victim->fPrev->fNext == victim);						// In this case 'A' DOES exist
			victim->fPrev->fNext = victim->fNext;							// Make A point to C
			// Now make 'C' point back to A (careful if 'C' is Nil)
			if (victim->fNext == Nil) {
				// In this case 'C' does not exist - it is Nil...
				Assert (victim == fData->fLast);
				((DoubleLinkList_Patch<T>*)fData)->fLast = victim->fPrev;	//	(~const)- 'A' is now last
			}
			else {
				Assert (victim->fNext->fPrev = victim);						// Victim used to be 'C's prev
				victim->fNext->fPrev = victim->fPrev;						// Now 'A' is
			}
		}
		((DoubleLinkList_Patch<T>*)fData)->fLength--;			//	(~const)
		delete (victim);
		Invariant ();
		fData->Invariant ();	// calls by invariant
	}
	template	<class T>	inline	void	ForwardDoubleLinkListMutator_Patch<T>::UpdateCurrent (T newValue)
	{
		Invariant ();
		Require (not Done ());
		AssertNotNil (fCurrent);
		((DoubleLink<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better
														// the (~const)
	}
	template	<class T>	inline	void	ForwardDoubleLinkListMutator_Patch<T>::AddBefore (T newValue)
	{
		/*
		 * NB: This code works fine, even if we are done!!!
		 */

		AssertNotNil (fData);
		if (fCurrent == Nil) {
			/*
			 *		NB: If I am past the last item on the list, AddBefore() is equivilent
			 *	to Appending to the list.
			 */
			Assert (Done ());
			((DoubleLinkList_Patch<T>*)fData)->Append (newValue);		//	(~const)
			Assert (Done ());											// what is done, cannot be undone!!!
		}
		else {
			DoubleLink<T>*	prev	=	fCurrent->fPrev;
			if (prev == Nil) {
				Assert (fCurrentIndex == 1);
				((DoubleLinkList_Patch<T>*)fData)->Prepend (newValue);	//	(~const)
				Assert (fCurrentIndex == 2);							// patching gaurantees this
			}
			else {
				/*
				 *	    |        |   	|        |   	|        |
				 *	    |   PREV |   	|  NEW   |   	|   CUR  |
				 *	... | <-prev |   	| <-prev |   	| <-prev |  ...
				 *	    | next-> |   	| next-> |   	| next-> |
				 *	    |        |   	|        |   	|        |
				 */
				Assert (prev->fNext == fCurrent);
				prev->fNext = new DoubleLink<T> (newValue, prev, (DoubleLink<T>*)fCurrent);
				// Since fCurrent != Nil from above, we update its prev, and dont have
				// to worry about fLast.
				((DoubleLink<T>*)fCurrent)->fPrev = prev->fNext;		// (~const)
				Assert (fCurrent->fPrev->fPrev = prev);	// old prev is two back now...
				fData->PatchViewsAdd (fCurrentIndex);
			}
		}
		fData->Invariant ();	// will call this's Invariant()
	}
	template	<class T>	inline	void	ForwardDoubleLinkListMutator_Patch<T>::AddAfter (T newValue)
	{
		Require (not Done ());
		fData->Invariant ();	// will call this's Invariant()
		/*
		 *	    |        |   	|        |
		 *	    |  CUR   |   	|   NEW  |
		 *	... | <-prev |   	| <-prev |  ...
		 *	    | next-> |   	| next-> |
		 *	    |        |   	|        |
		 */
		AssertNotNil (fCurrent);	// cuz not done...
		((DoubleLink<T>*)fCurrent)->fNext = new DoubleLink<T> (newValue, (DoubleLink<T>*)fCurrent, fCurrent->fNext);	//	(~const)
		if (fCurrent->fNext->fNext == Nil) {
			((DoubleLinkList_Patch<T>*)fData)->fLast = fCurrent->fNext;
		}
		else {
			Assert (fCurrent->fNext->fNext->fPrev == fCurrent->fNext);		// cuz of params to new Link...
		}
		fData->PatchViewsAdd (fCurrentIndex+1);
		fData->Invariant ();	// will call this's Invariant()
	}


	// class BackwardDoubleLinkListIterator_Patch<T>
	template	<class T>	inline	BackwardDoubleLinkListIterator_Patch<T>::BackwardDoubleLinkListIterator_Patch (const DoubleLinkList_Patch<T>& data) :
		DoubleLinkListIterator_PatchBase<T> (data)
	{
		fCurrent = data.fLast;
		fCurrentIndex = data.GetLength ();
		Invariant ();
	}
	template	<class T>	inline	Boolean	BackwardDoubleLinkListIterator_Patch<T>::More ()
	{
		// This is IDENTICAL to what we do in BackwardDoubleLinkListIterator<T>::More ()
		Invariant ();
		if (fSupressMore) {
			fSupressMore = False;
		}
		else {
			if (Done ()) {
				return (False);
			}
			else {
				AssertNotNil (fCurrent);
				fCurrent = fCurrent->fPrev;
			}
		}
		return (not Done ());
	}
	template	<class T>	inline	void	BackwardDoubleLinkListIterator_Patch<T>::PatchRemoveCurrent ()
	{
		AssertNotNil (fCurrent);
		fCurrent = fCurrent->fPrev;
		fCurrentIndex--;
		fSupressMore = True;
	}


	// Class BackwardDoubleLinkListMutator<T>
	template	<class T>	inline	BackwardDoubleLinkListMutator_Patch<T>::BackwardDoubleLinkListMutator_Patch (DoubleLinkList_Patch<T>& data) :
		BackwardDoubleLinkListIterator_Patch<T>((const DoubleLinkList_Patch<T>&)data)
	{
		Invariant ();
	}
	template	<class T>	inline	void	BackwardDoubleLinkListMutator_Patch<T>::RemoveCurrent ()
	{
		// cloned exactly from ForwardDoubleLinkListMutator_Patch<T>::RemoveCurrent ()
		Require (not Done ());
		Invariant ();
		DoubleLink<T>*	victim	=	(DoubleLink<T>*)fCurrent;	//	(~const)
		AssertNotNil (victim);									// cuz not done
		AssertNotNil (fData);
		fData->PatchViewsRemove (fCurrentIndex);
		Assert (fCurrent != victim);				// patching should  have guaranteed this


		/*
		 * Before:
		 *	    |        |   	|        |   	|        |
		 *	    |    A   |   	|    V   |   	|   C    |
		 *	... | <-prev |   	| <-prev |   	| <-prev |  ...
		 *	    | next-> |   	| next-> |   	| next-> |
		 *	    |        |   	|        |   	|        |
		 *
		 * After:
		 *	    |        |   	|        |
		 *	    |   A    |   	|    C   |
		 *	... | <-prev |   	| <-prev | ...
		 *	    | next-> |   	| next-> |
		 *	    |        |   	|        |
		 */
		if (victim->fPrev == Nil) {
			// In this case 'A' does not exist - it is Nil...
			Assert (fData->fFirst == victim);
			((DoubleLinkList_Patch<T>*)fData)->fFirst = victim->fNext;		//	(~const)- 'C' is now first
			if (fData->fFirst == Nil) {
				Assert (fData->fLength == 1);	// there was just one entry...
				((DoubleLinkList_Patch<T>*)fData)->fLast = Nil;				//	(~const)
			}
			else {
				Assert (fData->fFirst->fPrev = victim);						// Victim used to be 'C's prev
				fData->fFirst->fPrev = Nil;									// Now Nil!
			}
		}
		else {
			Assert (victim->fPrev->fNext == victim);						// In this case 'A' DOES exist
			victim->fPrev->fNext = victim->fNext;							// Make A point to C
			// Now make 'C' point back to A (careful if 'C' is Nil)
			if (victim->fNext == Nil) {
				// In this case 'C' does not exist - it is Nil...
				Assert (victim == fData->fLast);
				((DoubleLinkList_Patch<T>*)fData)->fLast = victim->fPrev;	//	(~const)- 'A' is now last
			}
			else {
				Assert (victim->fNext->fPrev = victim);						// Victim used to be 'C's prev
				victim->fNext->fPrev = victim->fPrev;						// Now 'A' is
			}
		}
		((DoubleLinkList_Patch<T>*)fData)->fLength--;			//	(~const)
		delete (victim);
		Invariant ();
		fData->Invariant ();	// calls by invariant
	}
	template	<class T>	inline	void	BackwardDoubleLinkListMutator_Patch<T>::UpdateCurrent (T newValue)
	{
		Invariant ();
		Require (not Done ());
		AssertNotNil (fCurrent);
		((DoubleLink<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better
														// the (~const)
	}
	template	<class T>	inline	void	BackwardDoubleLinkListMutator_Patch<T>::AddBefore (T newValue)
	{
		Require (not Done ());
		AssertNotNil (fData);
		DoubleLink<T>*	prev	=	fCurrent->fPrev;
		if (prev == Nil) {
			Assert (fCurrentIndex == 1);
			((DoubleLinkList_Patch<T>*)fData)->Prepend (newValue);	//	(~const)
			Assert (fCurrentIndex == 2);							// patching gaurantees this
		}
		else {
			/*
			 *	    |        |   	|        |   	|        |
			 *	    |   PREV |   	|  NEW   |   	|   CUR  |
			 *	... | <-prev |   	| <-prev |   	| <-prev |  ...
			 *	    | next-> |   	| next-> |   	| next-> |
			 *	    |        |   	|        |   	|        |
			 */
			Assert (prev->fNext == fCurrent);
			prev->fNext = new DoubleLink<T> (newValue, prev, (DoubleLink<T>*)fCurrent);
			// Since fCurrent != Nil from above, we update its prev, and dont have
			// to worry about fLast.
			((DoubleLink<T>*)fCurrent)->fPrev = prev->fNext;		// (~const)
			Assert (fCurrent->fPrev->fPrev = prev);	// old prev is two back now...
			fData->PatchViewsAdd (fCurrentIndex);
		}
		fData->Invariant ();	// will call this's Invariant()
	}
	template	<class T>	inline	void	BackwardDoubleLinkListMutator_Patch<T>::AddAfter (T newValue)
	{
		/*
		 * NB: This can be called if we are done.
		 */
		fData->Invariant ();	// will call this's Invariant()
		if (fCurrent == Nil) {
			Assert (Done ());
			((DoubleLinkList_Patch<T>*)fData)->Prepend (newValue);	//	(~const)
			Assert (Done ());										// what is done, cannot be undone!!!
		}
		else {
			/*
			 *	    |        |   	|        |
			 *	    |  CUR   |   	|   NEW  |
			 *	... | <-prev |   	| <-prev |  ...
			 *	    | next-> |   	| next-> |
			 *	    |        |   	|        |
			 */
			AssertNotNil (fCurrent);	// cuz not done...
			((DoubleLink<T>*)fCurrent)->fNext = new DoubleLink<T> (newValue, (DoubleLink<T>*)fCurrent, fCurrent->fNext);	//	(~const)
			if (fCurrent->fNext->fNext == Nil) {
				((DoubleLinkList_Patch<T>*)fData)->fLast = fCurrent->fNext;
			}
			else {
				Assert (fCurrent->fNext->fNext->fPrev == fCurrent->fNext);		// cuz of params to new Link...
			}
			fData->PatchViewsAdd (fCurrentIndex+1);
		}
		fData->Invariant ();	// will call this's Invariant()
	}


	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifndef	inline
			#error	"How did it get undefined?"
		#endif
		#undef	inline
	#endif


#endif	/*qRealTemplatesAvailable*/



#if		qRealTemplatesAvailable && !qTemplatesHasRepository
	/*
	 * 		We must include our .cc file here because of this limitation.
	 *	But, under some systems (notably UNIX) we cannot compile some parts
	 *	of our .cc file from the .hh.
	 *
	 *		The other problem is that there is sometimes some untemplated code
	 *	in the .cc file, and to detect this fact, we define another magic
	 *	flag which is looked for in those files.
	 */
	#define	qIncluding_DoubleLinkListCC	1
		#include	"../Sources/DoubleLinkList.cc"
	#undef	qIncluding_DoubleLinkListCC
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__DoubleLinkList__*/


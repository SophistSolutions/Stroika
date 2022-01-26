/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__LinkList__
#define	__LinkList__

/*
 * $Header: /fuji/lewis/RCS/LinkList.hh,v 1.28 1992/12/10 05:56:45 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *			+	Consider adding LinkListMutator<T> since otherwise we cannot do InsertAt,
 *				/ AddAfter() etc without using a LinkList_Patch.
 *
 * Notes:
 *
 *		Slightly unusual behaviour for LinkListMutator_Patch<T>::AddBefore () -
 *	allow it to be called when we are Done() - otherwise there is NO
 *	WAY TO APPEND TO A LINK LIST!!!
 *
 *
 * Changes:
 *	$Log: LinkList.hh,v $
 *		Revision 1.28  1992/12/10  05:56:45  lewis
 *		typo in gcc bug workaround.
 *
 *		Revision 1.27  1992/12/08  22:04:26  lewis
 *		My work around for qGCC_BadDefaultCopyConstructorGeneration forgot
 *		to copy the link pointers.
 *
 *		Revision 1.26  1992/12/04  19:43:59  lewis
 *		LinkListMutator_Patch<T>::operator= needs to return (*this);
 *
 *		Revision 1.25  1992/12/03  07:37:44  lewis
 *		Nearly total rewrite. Got rid of base class LinkedList. Just do
 *		everything directly - kdjs idea - bad part is increased obj code size
 *		but increase is flungable. Good part is no more virtaul function to
 *		delete nodes.
 *		Most of the change in API was getting all Link*'s out of the API.
 *		Updates in the middle are not done totally with mutators. Greatly
 *		simpifies usage in mixin classes (for the most part). May reduce flexability
 *		slightly, but it hasn't hurt yet.
 *		Rewrite iterators to conform to new Iterator SUMMIT descisions.
 *		Support it.More() if that wasnt done allready.
 *
 *		Revision 1.24  1992/11/29  02:43:39  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.22  1992/11/05  22:14:25  lewis
 *		ItemToIndex() takes LinkNode instead of Link<T>*.
 *
 *		Revision 1.21  1992/11/05  15:24:47  lewis
 *		Use qCFront_InlinesOpNewDeleteMustDeclareInline workaround.
 *		LinkList_Patch<T> should be PUBLIC subclass.
 *		LinkListIterator_Patch<T>::LinkListIterator_Patch(LinkList_Patch<T>&) now takes
 *		const ref - not well thought out - just being constisnent - think
 *		out better.
 *
 *		Revision 1.20  1992/11/03  22:51:10  lewis
 *		Deleted commented out code.
 *		ItemToIndex takes const ptr rather than ptr.
 *
 *		Revision 1.19  1992/11/03  17:26:51  lewis
 *		Got rid of Math.hh/Memory.hh includes.
 *
 *		Revision 1.18  1992/11/03  17:05:18  lewis
 *		Fixed lots of bugs in patching, and related code. Also, moved fDeleted
 *		Cur field to patching iterator.
 *		Use qGCC_InternalError95InInlineExpansionAndScopeResoluti instead of #if qGCC.
 *		Got rid of peek method.
 *		Made fData a ptr instead of a reference - since we want to be able to copy
 *		in op= for iterators.
 *
 *		Revision 1.17  1992/11/02  19:48:02  lewis
 *		Cleaned up by moving inlines to implementation section. Also, added _Patch
 *		classes, like  we did in Array, and no longer include Collection/Iterator.
 *
 *		Revision 1.16  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.15  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.14  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.13  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.12  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.11  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.10  1992/09/25  21:13:45  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.9  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.8  1992/09/21  05:30:53  sterling
 *		Added Peek () method to Link and used in a bunch of places for
 *		performance imrpovment.
 *		(LGP Checked in - as I checked in, I made Peek return const T& and
 *		made it a const method. Also, I think its probably an anti-tweek on
 *		most decent compilers, and probably should be removed. Maybe
 *		we should experiment a bit.
 *
 *		Revision 1.7  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.5  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.4  1992/09/04  20:04:46  sterling
 *		Added Contains () method.
 *
 *
 *
 */


#include	"BlockAllocated.hh"
#include	"Debug.hh"





#if		qRealTemplatesAvailable

template	<class	T>	class	LinkList;
template	<class	T>	class	LinkListIterator;
template	<class	T>	class	LinkListIterator_Patch;
template	<class	T>	class	LinkListMutator_Patch;

/*
 *		Link<T> is a generic link node used in a link list. Pardon
 *	the compiler bug workarounds - they make this look more complex
 *	than it really is. This should really be nested in LinkList<T>
 *	but no compilers I know of support nested templates.
 */
template	<class	T>	class	Link {
#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	private:
#else
	public:
#endif
		Link (T item, Link<T>* next);

#if		qGCC_BadDefaultCopyConstructorGeneration
		Link<T> (const Link<T>& from) :
			fItem (from.fItem),
			fNext (from.fNext)
		{
		}
		nonvirtual	const Link<T>& operator= (const Link<T>& rhs)
		{
			fItem = rhs.fItem;
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
		T			fItem;
		Link<T>*	fNext;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	LinkList<T>;
	friend	class	LinkListIterator<T>;
	friend	class	LinkListIterator_Patch<T>;
	friend	class	LinkListMutator_Patch<T>;
#endif
};



/*
 *		LinkList<T> is a generic link (non-intrusive) list implementation.
 *	It keeps a length count so access to its length is rapid. We provide
 *	no public means to access the links themselves.
 *
 *		Since this class provides no patching support, it is not generally
 *	used - more likely you want to use LinkList_Patch<T>. Use this if you
 *	will manage all patching, or know that none is necessary.
 */
template	<class	T>	class	LinkList {
	public:
		LinkList ();
		LinkList (const LinkList<T>& from);
		~LinkList ();

	public:
		nonvirtual	LinkList<T>& operator= (const LinkList<T>& list);

	public:
		nonvirtual	size_t	GetLength () const;

	/*
	 *		Basic access to the head of the list. This is what is most
	 *	commonly used, and is most efficient.
	 */
	public:
		nonvirtual	T		GetFirst () const;
		nonvirtual	void	Prepend (T item);
		nonvirtual	void	RemoveFirst ();

	/*
	 *	Utility to search the list for the given item using operator==
	 */
	public:
		nonvirtual	Boolean	Contains (T item) const;


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
		size_t		fLength;
		Link<T>*	fFirst;

#if		qDebug
		virtual		void	Invariant_ () const;
#endif
#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	LinkListIterator<T>;
	friend	class	LinkListIterator_Patch<T>;
	friend	class	LinkListMutator_Patch<T>;
#endif
};



/*
 *		LinkListIterator<T> allows you to iterate over a LinkList<T>. Its API
 *	is designed to make easy implemenations of subclasses of IteratorRep<T>.
 *	It is unpatched - use LinkListIterator_Patch<T> or LinkListMutator_Patch<T>
 *	for that.
 */
template	<class T>	class	LinkListIterator {
	public:
		LinkListIterator (const LinkListIterator<T>& from);
		LinkListIterator (const LinkList<T>& data);

	public:
		nonvirtual	LinkListIterator<T>& operator= (const LinkListIterator<T>& list);

	public:
		nonvirtual	Boolean	Done () const;
		nonvirtual	Boolean	More ();
		nonvirtual	T		Current () const;

		nonvirtual	void	Invariant () const;

	protected:
		const Link<T>*	fCurrent;
		Boolean			fSupressMore;	// Indicates if More should do anything, or if were already Mored...

#if		qDebug
		virtual	void	Invariant_ () const;
#endif
};






/*
 *	Patching Support:
 *
 *		Here we provide Patching Versions of each iterator, and for convienience
 *	versions of LinkList that maintain a list of all Patching iterators.
 */



/*
 *		LinkList_Patch<T> is a LinkList<T> with the ability to keep track of
 *	owned patching iterators. These patching iterators will automatically be
 *	adjusted when the link list is adjusted. This is the class of LinkList
 *	most likely to be used in implementing a concrete container class.
 */
template	<class T>	class	LinkListIterator_Patch;
template	<class	T>	class	LinkList_Patch : public LinkList<T> {
	public:
		LinkList_Patch ();
		LinkList_Patch (const LinkList_Patch<T>& from);
		~LinkList_Patch ();

		nonvirtual	LinkList_Patch<T>& operator= (const LinkList_Patch<T>& list);

	/*
	 * Methods we shadow so that patching is done. If you want to circumvent the
	 * patching, thats fine - use scope resolution operator to call this's base
	 * class version.
	 */
	public:
		nonvirtual	void	Prepend (T item);
		nonvirtual	void	Remove (T item);
		nonvirtual	void	RemoveFirst ();
		nonvirtual	void	RemoveAll ();

	/*
	 * Methods to do the patching yourself. Iterate over all the iterators and
	 * perfrom patching.
	 */
	public:
		nonvirtual	Boolean	HasActiveIterators () const;					//	are there any iterators to be patched?
		nonvirtual	void	PatchViewsAdd (const Link<T>* link) const;		//	call after add
		nonvirtual	void	PatchViewsRemove (const Link<T>* link) const;	//	call before remove
		nonvirtual	void	PatchViewsRemoveAll () const;					//	call after removeall

	/*
	 *	Check Invariants for this class, and all the iterators we own.
	 */
	public:
		nonvirtual	void	Invariant () const;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	protected:
#endif
		LinkListIterator_Patch<T>*	fIterators;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	LinkListIterator_Patch<T>;
#endif
#if		qDebug
		override	void	Invariant_ () const;
		nonvirtual	void	InvariantOnIterators_ () const;
#endif
};




/*
 *		LinkListIterator_Patch<T> is a LinkListIterator_Patch<T> that allows
 *	for updates to the LinkList<T> to be dealt with properly. It maintains a
 *	link list of iterators headed by the LinkList_Patch<T>, and takes care
 *	of all patching details.
 */
template	<class T>	class	LinkListIterator_Patch : public LinkListIterator<T> {
	public:
		LinkListIterator_Patch (const LinkList_Patch<T>& data);
		LinkListIterator_Patch (const LinkListIterator_Patch<T>& from);
		~LinkListIterator_Patch ();

		nonvirtual	LinkListIterator_Patch<T>&	operator= (const LinkListIterator_Patch<T>& rhs);

		/*
		 * Shadow more to keep track of prev.
		 */
		nonvirtual	Boolean	More ();

		nonvirtual	void	PatchAdd (const Link<T>* link);		//	call after add
		nonvirtual	void	PatchRemove (const Link<T>* link);	//	call before remove
		nonvirtual	void	PatchRemoveAll ();					//	call after removeall


#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	protected:
#endif
		const LinkList_Patch<T>*	fData;
		LinkListIterator_Patch<T>*	fNext;
		const Link<T>*				fPrev;		// keep extra previous link for fast patchremove
												// Nil implies fCurrent == fData->fFirst or its invalid,
												// and must be recomputed (it was removed itself)...

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	LinkList_Patch<T>;
#endif
#if		qDebug
		override	void	Invariant_ () const;
#endif
};




/*
 *		LinkListMutator_Patch<T> is a LinkListIterator_Patch<T> that allows
 *	for changes to the LinkList<T> relative to the position we are at
 *	currently in the iteration. This is be used most commonly in
 *	implementing concrete iterators for Stroika containers.
 */
template	<class T>	class	LinkListMutator_Patch : public LinkListIterator_Patch<T> {
	public:
		LinkListMutator_Patch (LinkList_Patch<T>& data);
		LinkListMutator_Patch (const LinkListMutator_Patch<T>& from);

		nonvirtual	LinkListMutator_Patch<T>&	operator= (LinkListMutator_Patch<T>& rhs);

		nonvirtual	void	RemoveCurrent ();
		nonvirtual	void	UpdateCurrent (T newValue);
		nonvirtual	void	AddBefore (T item);
		nonvirtual	void	AddAfter (T item);
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
	template	<class T>	inline	Link<T>::Link (T item, Link<T>* next) :
		fItem (item),
		fNext (next)
	{
	}
	#if		!qRealTemplatesAvailable
		#include	"TFileMap.hh"
		#include	BlockAllocatedOfLinkOf<T>_hh
	#endif
	template	<class	T>	inline	void*	Link<T>::operator new (size_t size)
	{
		return (BlockAllocated<Link<T> >::operator new (size));
	}
	template	<class	T>	inline	void	Link<T>::operator delete (void* p)
	{
		BlockAllocated<Link<T> >::operator delete (p);
	}


	// class LinkList<T>
	template	<class T>	inline	void	LinkList<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
#endif
	}
	template	<class T>	inline	LinkList<T>::LinkList () :
		fLength (0),
		fFirst (Nil)
	{
		Invariant ();
	}
	template	<class T>	inline	LinkList<T>::~LinkList ()
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
	}
	template	<class T>	inline	size_t	LinkList<T>::GetLength () const
	{
		return (fLength);
	}
	template	<class T>	inline	T	LinkList<T>::GetFirst () const
	{
		Require (fLength > 0);
		AssertNotNil (fFirst);
		return (fFirst->fItem);
	}
	template	<class T>	inline	void	LinkList<T>::Prepend (T item)
	{
		Invariant ();
		fFirst = new Link<T> (item, fFirst);
		fLength++;
		Invariant ();
	}
	template	<class T>	inline	void	LinkList<T>::RemoveFirst ()
	{
		RequireNotNil (fFirst);
		Invariant ();

		Link<T>* victim = fFirst;
		fFirst = victim->fNext;
		delete (victim);
		fLength--;

		Invariant ();
	}



	// class LinkListIterator<T>
	template	<class T>	inline	void	LinkListIterator<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
#endif
	}
	template	<class T>	inline	LinkListIterator<T>::LinkListIterator (const LinkList<T>& data) :
		fCurrent (data.fFirst),
		fSupressMore (True)
	{
	}
	template	<class T>	inline	LinkListIterator<T>::LinkListIterator (const LinkListIterator<T>& from) :
		fCurrent (from.fCurrent),
		fSupressMore (from.fSupressMore)
	{
	}
	template	<class T>	inline	LinkListIterator<T>&	LinkListIterator<T>::operator= (const LinkListIterator<T>& rhs)
	{
		Invariant ();
		fCurrent = rhs.fCurrent;
		fSupressMore = rhs.fSupressMore;
		Invariant ();
		return (*this);
	}
	template	<class T>	inline	Boolean	LinkListIterator<T>::Done () const
	{
		Invariant ();
		return Boolean (fCurrent == Nil);
	}
	template	<class T>	inline	Boolean	LinkListIterator<T>::More ()
	{
		Invariant ();
		/*
		 * We could already be done since after the last Done() call, we could
		 * have done a removeall.
		 */
		if (not fSupressMore and fCurrent != Nil) {
			fCurrent = fCurrent->fNext;
		}
		fSupressMore = False;
		Invariant ();
		return (not Done ());
	}
	template	<class T>	inline	T	LinkListIterator<T>::Current () const
	{
		Require (not (Done ()));
		Invariant ();
		AssertNotNil (fCurrent);
		return (fCurrent->fItem);
	}





	/*
	 **************************** Patching code ****************************
	 */


	// Class LinkListIterator_Patch<T>
	template	<class T>	inline	LinkListIterator_Patch<T>::LinkListIterator_Patch (const LinkList_Patch<T>& data) :
		LinkListIterator<T> (data),
		fPrev (Nil),			// means invalid or fData->fFirst == fCurrent ...
		fNext (data.fIterators),
		fData (&data)
	{
		((LinkList_Patch<T>*)&data)->fIterators = this;			// (~const)
		Invariant ();
	}
	template	<class T>	inline	LinkListIterator_Patch<T>::LinkListIterator_Patch (const LinkListIterator_Patch<T>& from) :
		LinkListIterator<T> (from),
		fPrev (from.fPrev),
		fNext (from.fData->fIterators),
		fData (from.fData)
	{
		from.Invariant ();
		((LinkList_Patch<T>*)fData)->fIterators = this;			// (~const)
		Invariant ();
	}
	template	<class T>	inline	LinkListIterator_Patch<T>::~LinkListIterator_Patch ()
	{
		Invariant ();
		AssertNotNil (fData);
		if (fData->fIterators == this) {
			((LinkList_Patch<T>*)fData)->fIterators = fNext;		//(~const)
		}
		else {
			for (LinkListIterator_Patch<T>* v = fData->fIterators; v->fNext != this; v = v->fNext) {
				AssertNotNil (v);
				AssertNotNil (v->fNext);
			}
			AssertNotNil (v);
			Assert (v->fNext == this);
			v->fNext = fNext;
		}
	}
	template	<class T>	inline	LinkListIterator_Patch<T>&	LinkListIterator_Patch<T>::operator= (const LinkListIterator_Patch<T>& rhs)
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
				((LinkList_Patch<T>*)fData)->fIterators = fNext;
			}
			else {
				for (LinkListIterator_Patch<T>* v = fData->fIterators; v->fNext != this; v = v->fNext) {
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
			((LinkList_Patch<T>*)fData)->fIterators = this;
		}

		fData = rhs.fData;
		fPrev = rhs.fPrev;

		LinkListIterator<T>::operator= (rhs);

		Invariant ();
		return (*this);
	}
	template	<class T>	inline	Boolean	LinkListIterator_Patch<T>::More ()
	{
		Invariant ();
		/*
		 * We could already be done since after the last Done() call, we could
		 * have done a removeall.
		 */
		if (not fSupressMore and fCurrent != Nil) {
			fPrev = fCurrent;
			fCurrent = fCurrent->fNext;
		}
		fSupressMore = False;
		Invariant ();
		return (not Done ());
	}
	template	<class T>	inline	void	LinkListIterator_Patch<T>::PatchAdd (const Link<T>* link)
	{
		/*
		 *		link is the new link just added. If it was just after current, then
		 *	there is no problem - we will soon hit it. If it was well before current
		 *	(ie before prev) then there is still no problem. If it is the new
		 *	previous, we just adjust our previous.
		 */
		RequireNotNil (link);
		if (link->fNext == fCurrent) {
			fPrev = link;
		}
	}
	template	<class T>	inline	void	LinkListIterator_Patch<T>::PatchRemove (const Link<T>* link)
	{
		RequireNotNil (link);

		/*
		 *	There are basicly three cases:
		 *
		 *	(1)		We remove the current. In this case, we just advance current to the next
		 *			item (prev is already all set), and set fSupressMore since we are advanced
		 *			to the next item.
		 *	(2)		We remove our previous. Technically this poses no problems, except then
		 *			our previos pointer is invalid. We could recompute it, but that would
		 *			involve rescanning the list from the beginning - slow. And we probably
		 *			will never need the next pointer (unless we get a remove current call).
		 *			So just set it to Nil, which conventionally means no valid value.
		 *			It will be recomputed if needed.
		 *	(3)		We are deleting some other value. No probs.
		 */
		if (fCurrent == link) {
			fCurrent = fCurrent->fNext;
			// fPrev remains the same - right now it points to a bad item, since
			// PatchRemove() called before the actual removal, but right afterwards
			// it will point to our new fCurrent.
			fSupressMore = True;			// Since we advanced cursor...
		}
		else if (fPrev == link) {
			fPrev = Nil;					// real value recomputed later, if needed
		}
	}
	template	<class T>	inline	void	LinkListIterator_Patch<T>::PatchRemoveAll ()
	{
		fCurrent = Nil;
		fPrev = Nil;
		Ensure (Done ());
	}


	// class LinkList_Patch<T>
	template	<class	T>	inline	void	LinkList_Patch<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
		InvariantOnIterators_ ();
#endif
	}
	template	<class	T>	inline	LinkList_Patch<T>::LinkList_Patch () :
		LinkList<T> (),
		fIterators (Nil)
	{
		Invariant ();
	}
	template	<class	T>	inline	LinkList_Patch<T>::LinkList_Patch (const LinkList_Patch<T>& from) :
		LinkList<T> (from),
		fIterators (Nil)	// Don't copy the list of iterators - would be trouble with backpointers!
							// Could clone but that would do no good, since nobody else would have pointers to them
	{
		Invariant ();
	}
	template	<class	T>	inline	LinkList_Patch<T>::~LinkList_Patch ()
	{
		Require (fIterators == Nil);
	}
	template	<class	T>	inline	Boolean	LinkList_Patch<T>::HasActiveIterators () const
	{
		return Boolean (fIterators != Nil);
	}
	template	<class T>	inline	void	LinkList_Patch<T>::PatchViewsAdd (const Link<T>* link) const
	{
		RequireNotNil (link);
		for (LinkListIterator_Patch<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchAdd (link);
		}
	}
	template	<class T>	inline	void	LinkList_Patch<T>::PatchViewsRemove (const Link<T>* link) const
	{
		RequireNotNil (link);
		for (LinkListIterator_Patch<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRemove (link);
		}
	}
	template	<class T>	inline	void	LinkList_Patch<T>::PatchViewsRemoveAll () const
	{
		for (LinkListIterator_Patch<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRemoveAll ();
		}
	}
	template	<class	T>	inline	LinkList_Patch<T>& LinkList_Patch<T>::operator= (const LinkList_Patch<T>& rhs)
	{
		/*
		 * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
		 * If this is to be supported at some future date, well need to work on our patching.
		 */
		Assert (not (HasActiveIterators ()));	// cuz copy of LinkList does not copy iterators...
		LinkList<T>::operator= (rhs);
		return (*this);
	}
	template	<class	T>	inline	void	LinkList_Patch<T>::Prepend (T item)
	{
		Invariant ();
		LinkList<T>::Prepend (item);
		PatchViewsAdd (fFirst);
		Invariant ();
	}
	template	<class	T>	inline	void	LinkList_Patch<T>::RemoveFirst ()
	{
		Invariant ();
		PatchViewsRemove (fFirst);
		LinkList<T>::RemoveFirst ();
		Invariant ();
	}
	template	<class	T>	inline	void	LinkList_Patch<T>::RemoveAll ()
	{
		Invariant ();
		LinkList<T>::RemoveAll ();
		PatchViewsRemoveAll ();
		Invariant ();
	}



	template	<class T>	inline	LinkListMutator_Patch<T>::LinkListMutator_Patch (LinkList_Patch<T>& data) :
		LinkListIterator_Patch<T> ((const LinkList_Patch<T>&)data)
	{
	}
	template	<class T>	inline	LinkListMutator_Patch<T>::LinkListMutator_Patch (const LinkListMutator_Patch<T>& from) :
		LinkListIterator_Patch<T> ((const LinkListIterator_Patch<T>&)from)
	{
	}
	template	<class T>	inline	LinkListMutator_Patch<T>&	LinkListMutator_Patch<T>::operator= (LinkListMutator_Patch<T>& rhs)
	{
		LinkListIterator_Patch<T>::operator= ((const LinkListIterator_Patch<T>&)rhs);
			return (*this);
	}
	template	<class T>	inline	void	LinkListMutator_Patch<T>::RemoveCurrent ()
	{
		Require (not Done ());
		Invariant ();
		Link<T>*	victim	=	(Link<T>*)fCurrent;	//	(~const)
		AssertNotNil (fData);
		fData->PatchViewsRemove (victim);
		Assert (fCurrent != victim);				// patching should  have guaranteed this
		/*
		 *		At this point we need the fPrev pointer. But it may have been lost
		 *	in a patch. If it was, its value will be Nil (NB: Nil could also mean
		 *	fCurrent == fData->fFirst). If it is Nil, recompute. Be careful if it
		 *	is still Nil, that means update fFirst.
		 */
		if ((fPrev == Nil) and (fData->fFirst != victim)) {
			AssertNotNil (fData->fFirst);	// cuz there must be something to remove current
			for (fPrev = fData->fFirst; fPrev->fNext != victim; fPrev = fPrev->fNext) {
				AssertNotNil (fPrev);	// cuz that would mean victim not in LinkList!!!
			}
		}
		if (fPrev == Nil) {
			((LinkList_Patch<T>*)fData)->fFirst = victim->fNext;	//	(~const)
		}
		else {
			Assert (fPrev->fNext == victim);
			((Link<T>*)fPrev)->fNext = victim->fNext; 				//	(~const)
		}
		((LinkList_Patch<T>*)fData)->fLength--;						//	(~const)
		delete (victim);
		Invariant ();
		fData->Invariant ();	// calls by invariant
	}
	template	<class T>	inline	void	LinkListMutator_Patch<T>::UpdateCurrent (T newValue)
	{
		RequireNotNil (fCurrent);
		((Link<T>*)fCurrent)->fItem = newValue;		// (~const)
	}
	template	<class T>	inline	void	LinkListMutator_Patch<T>::AddBefore (T newValue)
	{
		/*
		 * NB: This code works fine, even if we are done!!!
		 */

		/*
		 *		At this point we need the fPrev pointer. But it may have been lost
		 *	in a patch. If it was, its value will be Nil (NB: Nil could also mean
		 *	fCurrent == fData->fFirst). If it is Nil, recompute. Be careful if it
		 *	is still Nil, that means update fFirst.
		 */
		AssertNotNil (fData);
		if ((fPrev == Nil) and (fData->fFirst != Nil) and (fData->fFirst != fCurrent)) {
			for (fPrev = fData->fFirst; fPrev->fNext != fCurrent; fPrev = fPrev->fNext) {
				AssertNotNil (fPrev);	// cuz that would mean fCurrent not in LinkList!!!
			}
		}
		if (fPrev == Nil) {
			Assert (fData->fFirst == fCurrent);		// could be Nil, or not...
			((LinkList_Patch<T>*)fData)->fFirst = new Link<T> (newValue, fData->fFirst);	//	(~const)
			((LinkList_Patch<T>*)fData)->fLength++;											//	(~const)
			fData->PatchViewsAdd (fData->fFirst);		// Will adjust fPrev
		}
		else {
			Assert (fPrev->fNext == fCurrent);
			((Link<T>*)fPrev)->fNext = new Link<T> (newValue, fPrev->fNext); 				//	(~const)
			((LinkList_Patch<T>*)fData)->fLength++;											//	(~const)
			fData->PatchViewsAdd (fPrev->fNext);		// Will adjust fPrev
		}
		fData->Invariant ();	// will call this's Invariant()
	}
	template	<class T>	inline	void	LinkListMutator_Patch<T>::AddAfter (T newValue)
	{
		Require (not Done ());
		AssertNotNil (fCurrent);	// since not done...
		((Link<T>*)fCurrent)->fNext = new Link<T> (newValue, fCurrent->fNext);		// (~const)
		((LinkList_Patch<T>*)fData)->fLength++;										//	(~const)
		fData->PatchViewsAdd (fCurrent->fNext);
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
	#define	qIncluding_LinkListCC	1
		#include	"../Sources/LinkList.cc"
	#undef	qIncluding_LinkListCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__LinkList__*/


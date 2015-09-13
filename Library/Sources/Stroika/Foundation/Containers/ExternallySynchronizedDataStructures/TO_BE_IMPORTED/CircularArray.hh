/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CircularArray__
#define	__CircularArray__

/*
 * $Header: /fuji/lewis/RCS/CircularArray.hh,v 1.18 1992/12/09 20:57:22 lewis Exp $
 *
 * Description:
 *		This is a class whose external API is just like a regular dynamic
 *	array. The only difference is that its internal represenation is tuned
 *	to a particular pattern of access, where you tend to modify the
 *	array from the ends (useful in Q, DQ, etc).
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: CircularArray.hh,v $
 *		Revision 1.18  1992/12/09  20:57:22  lewis
 *		Use fLength instead of GetLength() in an assert since its an inline and
 *		defined afterwards, and this makes CFront 2.1 barf.
 *
 *		Revision 1.17  1992/12/03  19:16:59  lewis
 *		Quick hack support for it.More().
 *
 *		Revision 1.16  1992/12/03  07:28:42  lewis
 *		Rename Skrunch -> Compact.
 *
 *		Revision 1.15  1992/11/25  02:57:20  lewis
 *		Fix CircularArrayIterator_PatchBase::operator= to deal with fData changing.
 *
 *		Revision 1.14  1992/11/19  05:06:33  lewis
 *		Totally rewritten. Was buggy - constructor calls done at wrong time,
 *		read/wrote off end of array - very messy. Now MUCH faster, and hopefully
 *		fully correct.
 *
 *		Revision 1.13  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.12  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.11  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.10  1992/10/10  20:16:36  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.9  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.8  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.7  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.6  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/11  15:26:58  sterling
 *		bug fixes
 *
 *		Revision 1.3  1992/09/11  12:48:31  lewis
 *		#include <new.h>, and then workaround qGCC_LibgPlusPlusSharpDefineSize_tBug bug.
 *		Use flag qMPW_CFront_SloppyCodeGenWithoutOpNewOverloadForPlacementOp to workaround
 *		this CFront bug (just bindly did before but the workaround broke gcc 2.2.1).
 *		Also workaround separate qGCC_PlacementOpNewGeneratesSyntaxError bug.
 *
 *		Revision 1.2  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *
 *
 */

#include	"Debug.hh"



#if		qRealTemplatesAvailable
		
template	<class	T>	class	CircularArray;
template	<class	T>	class	CircularArrayIteratorBase;

template	<class	T>	class	CircularArrayNode {
	public:
		CircularArrayNode (T item);
		~CircularArrayNode ();

#if		qMPW_CFront_SloppyCodeGenWithoutOpNewOverloadForPlacementOp
		static	void*	operator new (size_t, void* p) 	{ return (p); }
		static	void	operator delete (void*)			{}
#endif

#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStarPlusPlusBug
		CircularArrayNode (const CircularArrayNode<T>& from);
		nonvirtual	const CircularArrayNode<T>& operator= (const CircularArrayNode<T>& rhs);
#endif

#if		!qBCC_TemplateFriendsNotSoFriendlyBug
	private:
#endif
		T	fItem;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug
	friend	class	CircularArray<T>;
	friend	class	CircularArrayIteratorBase<T>;
#endif
};


template	<class	T>	class	CircularArray {
	public:
		CircularArray ();
		CircularArray (const CircularArray<T>& from);
		~CircularArray ();

		nonvirtual	CircularArray<T>& operator= (const CircularArray<T>& list);

		nonvirtual	T		GetAt (size_t i) const;
		nonvirtual	void	SetAt (T item, size_t i);
		nonvirtual	T&		operator[] (size_t i);
		nonvirtual	T		operator[] (size_t i) const;

		nonvirtual	size_t	GetLength () const;
		nonvirtual	void	SetLength (size_t newLength, T fillValue);

		nonvirtual	void	InsertAt (T item, size_t index);
		nonvirtual	void	RemoveAt (size_t index);
		nonvirtual	void	RemoveAll ();
		
		nonvirtual	Boolean	Contains (T item) const;

		/*
		 * Memory savings/optimization methods.  Use this to tune useage
		 * of arrays so that they dont waste time in Realloc's.
		 */
		nonvirtual	size_t	GetSlotsAlloced () const;
		nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);

		nonvirtual	void	Compact ();

		/*
		 *		Call only for debugging purposes. Can call with debug off, but
		 *	then it expands to nothing.
		 */
		nonvirtual	void	Invariant () const;

	private:
		size_t					fSlotsAllocated;	//	#items alloced
		size_t					fFirst;				//	0 based offset to first item
		size_t					fLength;
		CircularArrayNode<T>*	fItems;


		/*
		 *	zero-based index of the last item. This is undefined if GetLength () == 0.
		 */
		nonvirtual	size_t	GetLast () const;

		/*
		 *		Calculate zero based C array offset given external "index"
		 *	which is one based.
		 */
		nonvirtual	size_t	CalcIndex (size_t index) const;

		/*
		 *		Normalize the array so that fFirst is 0. This involves
		 *	shuffling items around.
		 */
		nonvirtual	void	Normalize ();
};




template	<class T>	class	CircularArrayIteratorBase {
	public:
		CircularArrayIteratorBase (const CircularArray<T>& data);
		CircularArrayIteratorBase (const CircularArrayIteratorBase<T>& from);

		nonvirtual	CircularArrayIteratorBase<T>&	operator= (const CircularArrayIteratorBase<T>& rhs);

		nonvirtual	T		Current () const;
		nonvirtual	size_t	CurrentIndex () const;
		nonvirtual	Boolean	Done () const;

		nonvirtual	void	Invariant () const;

	protected:
		const CircularArray<T>*	fData;
		size_t					fIndex;		// one based external array iterface...
		size_t					fLength;
};




template	<class T> class	ForwardCircularArrayIterator : public CircularArrayIteratorBase<T> {
	public:
		ForwardCircularArrayIterator (const CircularArray<T>& data);
		ForwardCircularArrayIterator (const ForwardCircularArrayIterator<T>& from);

		nonvirtual	ForwardCircularArrayIterator<T>&	operator= (const ForwardCircularArrayIterator<T>& rhs);

		nonvirtual	void	Next ();
};




template	<class T> class	BackwardCircularArrayIterator : public CircularArrayIteratorBase<T> {
	public:
		BackwardCircularArrayIterator (const CircularArray<T>& data);
		BackwardCircularArrayIterator (const BackwardCircularArrayIterator<T>& from);

		nonvirtual	BackwardCircularArrayIterator<T>&	operator= (const BackwardCircularArrayIterator<T>& rhs);

		nonvirtual	void	Next ();
};







/*
 *	Patching Support:
 << MOSTLY COPIED FROM ARRAY.hh>>
 */

template	<class T>	class	CircularArrayIterator_PatchBase;
template	<class T>	class	CircularArray_Patch : public CircularArray<T> {
	public:
		CircularArray_Patch ();
		CircularArray_Patch (const CircularArray_Patch<T>& from);
		~CircularArray_Patch ();

		nonvirtual	CircularArray_Patch<T>& operator= (const CircularArray_Patch<T>& rhs);

	/*
	 * Methods we shadow so that patching is done. If you want to circumvent the
	 * patching, thats fine - use scope resolution operator to call this's base
	 * class version.
	 */
	public:
		nonvirtual	void	SetLength (size_t newLength, T fillValue);
		nonvirtual	void	InsertAt (T item, size_t index);
		nonvirtual	void	RemoveAt (size_t index);
		nonvirtual	void	RemoveAll ();

	/*
	 * Methods to do the patching yourself. Iterate over all the iterators and
	 * perfrom patching.
	 */
	public:
		nonvirtual	Boolean	HasActiveIterators () const;			//	are there any iterators to be patched?
		nonvirtual	void	PatchViewsAdd (size_t index) const;		//	call after add
		nonvirtual	void	PatchViewsRemove (size_t index) const;	//	call before remove
		nonvirtual	void	PatchViewsRemoveAll () const;			//	call after removeall

#if		!qBCC_TemplateFriendsNotSoFriendlyBug
	private:
#endif
		CircularArrayIterator_PatchBase<T>*	fIterators;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug
	friend	class	CircularArrayIterator_PatchBase<T>;
#endif
};





template	<class T>	class	CircularArrayIterator_PatchBase : public CircularArrayIteratorBase<T> {
	public:
		CircularArrayIterator_PatchBase (const CircularArray_Patch<T>& data);
		CircularArrayIterator_PatchBase (const CircularArrayIterator_PatchBase<T>& from);
		~CircularArrayIterator_PatchBase ();

		nonvirtual	CircularArrayIterator_PatchBase<T>&	operator= (const CircularArrayIterator_PatchBase<T>& rhs);

	public:
		nonvirtual	void	PatchAdd (size_t index);		//	call after add
		nonvirtual	void	PatchRemove (size_t index);		//	call before remove
		nonvirtual	void	PatchRemoveAll ();				//	call after removeall

		nonvirtual	void	Invariant () const;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug
	protected:
#endif
		const CircularArray_Patch<T>* 		fData;
		CircularArrayIterator_PatchBase<T>*	fNext;
		Boolean								fSupressMore;	// Indicates if next should do anything, or if were already forwarded...

#if		!qBCC_TemplateFriendsNotSoFriendlyBug
	friend	class	CircularArray_Patch<T>;
#endif
};



template	<class T>	class	ForwardCircularArrayIterator_Patch : public CircularArrayIterator_PatchBase<T> {
	public:
		ForwardCircularArrayIterator_Patch (const CircularArray_Patch<T>& data);
		ForwardCircularArrayIterator_Patch (const ForwardCircularArrayIterator_Patch<T>& from);

		nonvirtual	ForwardCircularArrayIterator_Patch<T>&	operator= (const ForwardCircularArrayIterator_Patch<T>& rhs);

	public:
		nonvirtual	Boolean	More ();
};




template	<class T> class	BackwardCircularArrayIterator_Patch : public CircularArrayIterator_PatchBase<T> {
	public:
		BackwardCircularArrayIterator_Patch (const CircularArray_Patch<T>& data);
		BackwardCircularArrayIterator_Patch (const BackwardCircularArrayIterator_Patch<T>& from);

		nonvirtual	BackwardCircularArrayIterator_Patch<T>&	operator= (const BackwardCircularArrayIterator_Patch<T>& rhs);

	public:
		nonvirtual	Boolean	More ();
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

	// class	CircularArray<T>
	template	<class	T>	inline	CircularArrayNode<T>::CircularArrayNode (T item) :
		fItem (item)
	{
	}
	template	<class	T>	inline	CircularArrayNode<T>::~CircularArrayNode ()
	{
	}
#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStarPlusPlusBug
	template	<class	T>	inline	CircularArrayNode<T>::CircularArrayNode (const CircularArrayNode<T>& from) :
		fItem (from.fItem)
	{
	}
	template	<class	T>	inline	const CircularArrayNode<T>& CircularArrayNode<T>::operator= (const CircularArrayNode<T>& rhs)
	{
		fItem = rhs.fItem;
		return (*this);
	}
#endif

	// class	CircularArray<T>
	template	<class	T>	inline	size_t	CircularArray<T>::GetLast () const
	{
		Require (fLength != 0);
		/*
		 *		Since this could wrap around, we must subtract fSlotsAlloced if it does...
		 *
		 *		NB: This could have been written using % fSlotsAllocated but that would involve
		 *	a division which is much slow that the test and subtract.
		 */
		size_t	i	=	fFirst + fLength - 1;
		if (i >= fSlotsAllocated) {
			i -= fSlotsAllocated;
		}
		Ensure ((fSlotsAllocated == 0) or ((fFirst + fLength - 1)%fSlotsAllocated) == i);
		return (i);
	}
	template	<class	T>	inline	void	CircularArray<T>::Invariant () const
	{
		#if		qDebug
			Assert ((fSlotsAllocated == 0) == (fItems == Nil));
			Assert ((fSlotsAllocated == 0) or (fFirst < fSlotsAllocated));
			Assert (fLength <= fSlotsAllocated);
			Assert ((fLength == 0) or (fLength == 1) or (fFirst != GetLast ()));
		#endif
	}
	template	<class	T>	inline	CircularArray<T>::CircularArray () :
		fSlotsAllocated (0),
		fFirst (0),
		fLength (0),
		fItems (Nil)
	{
		Invariant ();
	}
	template	<class	T>	inline	CircularArray<T>::~CircularArray ()
	{
		Invariant ();
		RemoveAll ();
		delete (char*)fItems;
	}
	template	<class	T>	inline	size_t	CircularArray<T>::GetLength () const
	{
		return (fLength);
	}
	template	<class	T>	inline	size_t	CircularArray<T>::CalcIndex (size_t index) const
	{
		Require (index >= 1);
		Require (index <= GetLength ());
	
		/*
		 *		Since this could wrap around, we must subtract fSlotsAlloced if it does...
		 *
		 *		NB: This could have been written using % fSlotsAllocated but that would involve
		 *	a division which is much slow that the test and subtract.
		 */
		size_t	i	=	(index-1) + fFirst;
		if (i >= fSlotsAllocated) {
			i -= fSlotsAllocated;
		}
		return (i);
	}
	template	<class	T>	inline	T	CircularArray<T>::GetAt (size_t i) const
	{
		Require (i >= 1);
		Require (i <= GetLength ());
		Invariant ();
		return (fItems [CalcIndex (i)].fItem);
	}
	template	<class	T>	inline	void	CircularArray<T>::SetAt (T item, size_t i)
	{
		Require (i >= 1);
		Require (i <= GetLength ());
		Invariant ();
		fItems [CalcIndex (i)].fItem = item;
	}
	template	<class	T>	inline	T&	CircularArray<T>::operator[] (size_t i)
	{
		Require (i >= 1);
		Require (i <= GetLength ());
		Invariant ();
		return (fItems [CalcIndex (i)].fItem);
	}
	template	<class	T>	inline	T	CircularArray<T>::operator[] (size_t i) const
	{
		Require (i >= 1);
		Require (i <= GetLength ());
		Invariant ();
		return (fItems [CalcIndex (i)].fItem);
	}
	template	<class	T>	inline	size_t	CircularArray<T>::GetSlotsAlloced () const
	{
		return (fSlotsAllocated);
	}
	template	<class	T>	inline	void	CircularArray<T>::Compact ()
	{
		Invariant ();
		SetSlotsAlloced (GetLength ());
	}


	// Class CircularArrayIteratorBase<T>
	template	<class T>	inline	void	CircularArrayIteratorBase<T>::Invariant () const
	{
		AssertNotNil (fData);
	}
	template	<class T>	inline	CircularArrayIteratorBase<T>::CircularArrayIteratorBase (const CircularArray<T>& data) :
		fData (&data),
//		fIndex (),
		fLength (data.GetLength ())
	{
		#if		qDebug
			fIndex = 0;	// more likely to cause bugs...(leave the xtra newline cuz of genclass bug...)

		#endif
	}
	template	<class T>	inline	CircularArrayIteratorBase<T>::CircularArrayIteratorBase (const CircularArrayIteratorBase<T>& from) :
		fData (from.fData),
		fIndex (from.fIndex),
		fLength (from.fLength)
	{
		from.Invariant ();
		Invariant ();
	}
	template	<class T>	inline	CircularArrayIteratorBase<T>&	CircularArrayIteratorBase<T>::operator= (const CircularArrayIteratorBase<T>& rhs)
	{
		Require (fData == rhs.fData);
		rhs.Invariant ();
		Invariant ();
		fIndex = rhs.fIndex;
		fLength = rhs.fLength;
		return (*this);
	}
	template	<class T>	inline	Boolean	CircularArrayIteratorBase<T>::Done () const
	{
		Invariant ();
		return Boolean ((fIndex == 0) or (fIndex > fLength));
	}
	template	<class T>	inline	size_t	CircularArrayIteratorBase<T>::CurrentIndex () const
	{
		Invariant ();
		return (fIndex);
	}
	template	<class T>	inline	T		CircularArrayIteratorBase<T>::Current () const
	{
		Require (not Done ());
		Invariant ();
		return (fData->GetAt (fIndex));
	}


	// Class ForwardCircularArrayIterator<T>
	template	<class T>	inline	ForwardCircularArrayIterator<T>::ForwardCircularArrayIterator (const CircularArray<T>& data) :
		CircularArrayIteratorBase<T>(data)
	{
		fIndex = 1;
		Invariant ();
	}
	template	<class T>	inline	ForwardCircularArrayIterator<T>::ForwardCircularArrayIterator (const ForwardCircularArrayIterator<T>& from) :
		CircularArrayIteratorBase<T>(from)
	{
		Invariant ();
	}
	template	<class T>	inline	ForwardCircularArrayIterator<T>&	ForwardCircularArrayIterator<T>::operator= (const ForwardCircularArrayIterator<T>& rhs)
	{
		CircularArrayIteratorBase<T>::operator= (rhs);
		Invariant ();
		return (*this);
	}
	template	<class T>	inline	void	ForwardCircularArrayIterator<T>::Next ()
	{
		Invariant ();
		fIndex++;
		Invariant ();
	}


	// Class BackwardCircularArrayIterator<T>
	template	<class T>	inline	BackwardCircularArrayIterator<T>::BackwardCircularArrayIterator (const CircularArray<T>& data) :
		CircularArrayIteratorBase<T>(data)
	{
		fIndex = data.GetLength ();
		Invariant ();
	}
	template	<class T>	inline	BackwardCircularArrayIterator<T>::BackwardCircularArrayIterator (const BackwardCircularArrayIterator<T>& from) :
		CircularArrayIteratorBase<T>(from)
	{
		Invariant ();
	}
	template	<class T>	inline	BackwardCircularArrayIterator<T>&	BackwardCircularArrayIterator<T>::operator= (const BackwardCircularArrayIterator<T>& rhs)
	{
		CircularArrayIteratorBase<T>::operator= (rhs);
		Invariant ();
		return (*this);
	}
	template	<class T>	inline	void	BackwardCircularArrayIterator<T>::Next ()
	{
		Invariant ();
		if (fIndex > 0) {
			fIndex--;
		}
		Invariant ();
	}




	/*
	 **************************** Patching code ****************************
	 */

	// Class CircularArrayIterator_PatchBase<T>
	template	<class T>	inline	void	CircularArrayIterator_PatchBase<T>::Invariant () const
	{
		CircularArrayIteratorBase<T>::Invariant ();
		Assert (fData == CircularArrayIteratorBase<T>::fData);
	}
	template	<class T>	inline	CircularArrayIterator_PatchBase<T>::CircularArrayIterator_PatchBase (const CircularArray_Patch<T>& data) :
		CircularArrayIteratorBase<T>(data),
		fData (&data),
		fNext (data.fIterators),
		fSupressMore (True)
	{
		//(~const)
		((CircularArray_Patch<T>*)fData)->fIterators = this;
	}
	template	<class T>	inline	CircularArrayIterator_PatchBase<T>::CircularArrayIterator_PatchBase (const CircularArrayIterator_PatchBase<T>& from) :
		CircularArrayIteratorBase<T>(from),
		fData (from.fData),
		fNext (from.fData->fIterators),
		fSupressMore (from.fSupressMore)
	{
		RequireNotNil (fData);
		//(~const)
		((CircularArray_Patch<T>*)fData)->fIterators = this;
	}
	template	<class T>	inline	CircularArrayIterator_PatchBase<T>::~CircularArrayIterator_PatchBase ()
	{
		AssertNotNil (fData);
		if (fData->fIterators == this) {
			//(~const)
			((CircularArray_Patch<T>*)fData)->fIterators = fNext;
		}
		else {
			for (CircularArrayIterator_PatchBase<T>* v = fData->fIterators; v->fNext != this; v = v->fNext) {
				AssertNotNil (v);
				AssertNotNil (v->fNext);
			}
			AssertNotNil (v);
			Assert (v->fNext == this);
			v->fNext = fNext;
		}
	}
	template	<class T>	inline	CircularArrayIterator_PatchBase<T>&	CircularArrayIterator_PatchBase<T>::operator= (const CircularArrayIterator_PatchBase<T>& rhs)
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
				((CircularArray_Patch<T>*)fData)->fIterators = fNext;
			}
			else {
				for (CircularArrayIterator_PatchBase<T>* v = fData->fIterators; v->fNext != this; v = v->fNext) {
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
			((CircularArray_Patch<T>*)fData)->fIterators = this;
		}

		CircularArrayIteratorBase<T>::operator=(rhs);
		fSupressMore = rhs.fSupressMore;
		Invariant ();
		return (*this);
	}
	template	<class T>	inline	void	CircularArrayIterator_PatchBase<T>::PatchAdd (size_t index)
	{
		Require (index >= 1);

		fLength++;
		AssertNotNil (fData);
		Invariant ();										//	since called after add

		/*
		 *		If we added an item to the right of our cursor, it has no effect
		 *	on our - by index - addressing, and so ignore it. We will eventually
		 *	reach that new item.
		 *		On the other hand, if we add the item to the left of our cursor,
		 *	things are more complex:
		 *		If we added an item left of the cursor, then we are now pointing to
		 *	the item before the one we used to, and so incrementing (ie Next)
		 *	would cause us to revisit (in the forwards case, or skip one in the
		 *	reverse case). To correct our index, we must increment it so that
		 *	it.Current () refers to the same entity.
		 *
<< COMMENT OUT OF DATE >>
		 *		Note that this should indeed by <=, since (as opposed to <) since
		 *	if we are a direct hit, and someone tries to insert something at
		 *	the position we are at, the same argument as before applies - we
		 *	would be revisiting, or skipping forwards an item.
		 */
		if (index < CurrentIndex ()) {
			fIndex++;
		}
		else if (index == CurrentIndex ()) {
			fSupressMore = True;
		}
		Invariant ();
	}
	template	<class T>	inline	void	CircularArrayIterator_PatchBase<T>::PatchRemove (size_t index)
	{
		Require (index >= 1);
		AssertNotNil (fData);
		Require (index <= fData->GetLength ());
		Invariant ();										//	since called before remove

		/*
		 *		If we are removing an item from the right of our cursor, it has no effect
		 *	on our - by index - addressing, and so ignore it.
		 *		On the other hand, if we are removing the item from the left of our cursor,
		 *	things are more complex:
		 *		If we are removing an item from the left of the cursor, then we are now
		 *	pointing to the item after the one we used to, and so decrementing (ie Next)
		 *	would cause us to skip one. To correct our index, we must decrement it so that
		 *	it.Current () refers to the same entity.
		 *
		 *		Just like in the PatchAdd case <= is appropriate.
		 */
		if (index < CurrentIndex ()) {
			Assert (CurrentIndex () >= 2);		// cuz then index would be <= 0, and thats imposible
			fIndex--;
		}
		else if (index == CurrentIndex ()) {
			fSupressMore = True;
		}
		fLength--;
	}
	template	<class T>	inline	void	CircularArrayIterator_PatchBase<T>::PatchRemoveAll ()
	{
		Require (fData->GetLength () == 0);		//	since called after removeall
		fIndex = 1;
		fLength = 0;
		fSupressMore = True;
		Invariant ();
	}


	// class CircularArray_Patch<T>
	template	<class	T>	inline	CircularArray_Patch<T>::CircularArray_Patch () :
		CircularArray<T> (),
		fIterators (Nil)
	{
	}
	template	<class	T>	inline	CircularArray_Patch<T>::CircularArray_Patch (const CircularArray_Patch<T>& from) :
		CircularArray<T> (from),
		fIterators (Nil)	// Don't copy the list of iterators - would be trouble with backpointers!
							// Could clone but that would do no good, since nobody else would have pointers to them
	{
	}
	template	<class	T>	inline	CircularArray_Patch<T>::~CircularArray_Patch ()
	{
		Require (fIterators == Nil);
	}
	template	<class	T>	inline	Boolean	CircularArray_Patch<T>::HasActiveIterators () const
	{
		return Boolean (fIterators != Nil);
	}
	template	<class T>	inline	void	CircularArray_Patch<T>::PatchViewsAdd (size_t index) const
	{
		for (CircularArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchAdd (index);
		}
	}
	template	<class T>	inline	void	CircularArray_Patch<T>::PatchViewsRemove (size_t index) const
	{
		for (CircularArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRemove (index);
		}
	}
	template	<class T>	inline	void	CircularArray_Patch<T>::PatchViewsRemoveAll () const
	{
		for (CircularArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRemoveAll ();
		}
	}
	template	<class	T>	inline	CircularArray_Patch<T>& CircularArray_Patch<T>::operator= (const CircularArray_Patch<T>& rhs)
	{
		/*
		 * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
		 * If this is to be supported at some future date, well need to work on our patching.
		 */
		Assert (not (HasActiveIterators ()));	// cuz copy of CircularArray does not copy iterators...
		CircularArray<T>::operator= (rhs);
		return (*this);
	}
	template	<class	T>	inline	void	CircularArray_Patch<T>::SetLength (size_t newLength, T fillValue)
	{
		// For now, not sure how to patch the iterators, so just Assert out - fix later ...
		AssertNotReached ();
	}
	template	<class	T>	inline	void	CircularArray_Patch<T>::InsertAt (T item, size_t index)
	{
		CircularArray<T>::InsertAt (item, index);
		PatchViewsAdd (index);
	}
	template	<class	T>	inline	void	CircularArray_Patch<T>::RemoveAt (size_t index)
	{
		PatchViewsRemove (index);
		CircularArray<T>::RemoveAt (index);
	}
	template	<class	T>	inline	void	CircularArray_Patch<T>::RemoveAll ()
	{
		CircularArray<T>::RemoveAll ();
		PatchViewsRemoveAll ();		// PatchRealloc not needed cuz removeall just destructs things,
									// it does not realloc pointers (ie does not call setslotsalloced).
	}



	// class ForwardCircularArrayIterator_Patch<T>
	template	<class T>	inline	ForwardCircularArrayIterator_Patch<T>::ForwardCircularArrayIterator_Patch (const CircularArray_Patch<T>& data) :
		CircularArrayIterator_PatchBase<T> (data)
	{
		fIndex = 1;
		Invariant ();
	}
	template	<class T>	inline	ForwardCircularArrayIterator_Patch<T>::ForwardCircularArrayIterator_Patch (const ForwardCircularArrayIterator_Patch<T>& from) :
		CircularArrayIterator_PatchBase<T> (from)
	{
		Invariant ();
	}
	template	<class T>	inline	ForwardCircularArrayIterator_Patch<T>&	ForwardCircularArrayIterator_Patch<T>::operator= (const ForwardCircularArrayIterator_Patch<T>& rhs)
	{
		CircularArrayIterator_PatchBase<T>::operator= (rhs);
		Invariant ();
		return (*this);
	}
	template	<class T>	inline	Boolean	ForwardCircularArrayIterator_Patch<T>::More ()
	{
		Invariant ();
		/*
		 * We must do this check since it is illegal to point past end, and
		 * we could be at the end of the CircularArray because of a patchremove of the
		 * last item.
		 */
		if (fSupressMore) {
			fSupressMore = False;
		}
		else {
			Assert (fIndex <= fLength);
			fIndex++;
		}
		Invariant ();
		return (not Done ());
	}


	// class BackwardCircularArrayIterator_Patch<T>
	template	<class T>	inline	BackwardCircularArrayIterator_Patch<T>::BackwardCircularArrayIterator_Patch (const CircularArray_Patch<T>& data) :
		CircularArrayIterator_PatchBase<T> (data)
	{
		fIndex = data.GetLength ();
		Invariant ();
	}
	template	<class T>	inline	BackwardCircularArrayIterator_Patch<T>::BackwardCircularArrayIterator_Patch (const BackwardCircularArrayIterator_Patch<T>& from) :
		CircularArrayIterator_PatchBase<T> (from)
	{
		Invariant ();
	}
	template	<class T>	inline	BackwardCircularArrayIterator_Patch<T>&	BackwardCircularArrayIterator_Patch<T>::operator= (const BackwardCircularArrayIterator_Patch<T>& rhs)
	{
		CircularArrayIterator_PatchBase<T>::operator= (rhs);
		Invariant ();
		return (*this);
	}
	template	<class T>	inline	Boolean	BackwardCircularArrayIterator_Patch<T>::More ()
	{
		/*
		 * NB: It is intentional that we ignore fSupressMore.
		 */
		Invariant ();
		Require (fIndex != 0);
		fIndex--;
		Invariant ();
		return (not Done ());
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
	 * 		We must include our .cc file here because of this limitation. But, under some
	 *	systems (notably UNIX) we cannot compile some parts of our .cc file from the
	 *	.hh.
	 *		The other problem is that there is sometimes some untemplated code in the .cc file,
	 *	and to detect this fact, we define another magic flag which is looked for in those files.
	 */
	#define	qIncluding_CircularArrayCC	1
		#include	"../Sources/CircularArray.cc"
	#undef	qIncluding_CircularArrayCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__CircularArray__*/

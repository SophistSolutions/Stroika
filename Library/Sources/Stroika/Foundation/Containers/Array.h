/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Array__
#define	__Array__

/*
 * $Header: /fuji/lewis/RCS/Array.hh,v 1.52 1992/12/10 05:55:06 lewis Exp $
 *
 *	Description:
 *
 *		Array<T> is a backend implementation. It is not intended to be directly
 *	used by programmers, except in implementing concrete container reps.
 *
 *		Array<T> is a template which provides a dynamic array class. Elements
 *	of type T can be assigned, and accessed much like a normal array, except
 *	that when debug is on, accesses are range-checked.
 *
 *		Array<T> also provides a dynamic sizing capability. It reallocs its
 *	underlying storgage is such a ways as to keep a buffer of n(currently 5)%
 *	extra, so that reallocs on resizes only occur logn times on n appends.
 *	To save even this space, you can call Compact().
 *
 *		Unlike other dynamic array implementations, when an item is removed,
 *	it is destructed then. So the effects of buffering have no effects on the
 *	semantics of the Array.
 *
 *
 *
 *	Notes:
 *
 *	Warning:
 *		This implemenation prohibits use of internal pointers within T
 *	since we sometimes copy the contents of the array without excplicitly using
 *	the X(X&) and T::operator= functions.
 *
 *
 *	C++/StandardC arrays and segmented architectures:
 *
 *		Our iterators use address arithmatic since that is faster than
 *	array indexing, but that requires care in the presence of patching,
 *	and in iterating backwards.
 *
 *		The natural thing to do in iteration would be to have fCurrent
 *	point to the current item, but that would pose difficulties in the
 *	final test at the end of the iteration when iterating backwards. The
 *	final test would be fCurrent < fStart. This would be illegal in ANSI C.
 *
 *		The next possible trick is for backwards iteration always point one
 *	past the one you mean, and have it.Current () subtract one before
 *	dereferncing. This works pretty well, but makes source code sharing between
 *	the forwards and backwards cases difficult.
 *
 *		The next possible trick, and the one we use for now, is to have
 *	fCurrent point to the current item, and in the Next() code, when
 *	going backwards, reset fCurrent to fEnd - bizzare as this may seem
 *	then the test code can be shared among the forwards and backwards
 *	implemenations, all the patching code can be shared, with only this
 *	one minor check. Other potential choices are presented in the TODO
 *	below.
 *
 *
 *	TODO:
 *		->	Use qCanAlwaysExplicitlyCallDTOR to conditionally use ArrayNode
 *			crap.
 *
 * Changes:
 *	$Log: Array.hh,v $
 *		Revision 1.52  1992/12/10  05:55:06  lewis
 *		Got rid of a few more CTORs and op= that we can leave it to the
 *		compiler to autogenerate for us.
 *		One causes a gcc bug, so we worked around that (qGCC_OpEqualsNotAutoDefinedSoCanBeExplicitlyCalledBug).
 *		Added private ArrayIteratorBase::CTOR.
 *
 *		Revision 1.51  1992/12/04  16:06:52  lewis
 *		Work around qCFront_SorryNotImplementedStatementAfterReturnInline.
 *
 *		Revision 1.50  1992/12/03  07:36:16  lewis
 *		Lots of new support for Invariants.
 *		Rewrote patching to conform to new Design for most recent
 *		Iterator SUMMIT.
 *		Add mutator support here.
 *		Lots of changes to fix the patching...
 *
 *		Revision 1.49  1992/11/25  02:55:10  lewis
 *		Fix ArrayIterator_PatchBase::operator= to deal with fData changing.
 *
 *		Revision 1.48  1992/11/21  09:14:19  lewis
 *		Add workaround of qGCC_TemplatesCTORWithArgMustBeInlineBug. Not quite
 *		enuf to be useful however. Still other bugs prevent templates from
 *		being usable. Try sending mail to source of this workaround to
 *		see if he has others.
 *
 *		Revision 1.47  1992/11/20  19:15:21  lewis
 *		Get rid of a couple of invariant calls and calls to public
 *		API methods from patch routines since during patching things
 *		sometimes in an inconsistent state.
 *
 *		Revision 1.46  1992/11/19  05:00:39  lewis
 *		Lots of comment cleanups, and revisions. Fixed PatchAdd.
 *
 *		Revision 1.45  1992/11/17  05:32:03  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.44  1992/11/16  21:52:10  lewis
 *		Added invarient method to iterators - mostly to assure that
 *		fCurrent >= fStart and <= fEnd - ALWAYs. Also, had to add fCurDeleted
 *		variable to ArrayIterator_PatchBase to make forwards iterators
 *		with deletion work properly. Not 100% sure I got it right, but
 *		it pasted seqence testsuite at least. Revisit in a month or
 *		so, when everything else is done to decide once and for all how
 *		we will define iterators when it.Current () is deleted!!! I think
 *		sterl is right, but not possitive.
 *
 *		Revision 1.43  1992/11/16  04:40:05  lewis
 *		kdjs fixes broke iterator with removeall in the middle.
 *		I believe the fix is to only advance the current pointer in
 *		patchable array if fCurrent < fEnd.
 *		Also, add lots of invariants asserting fCurrent >= fStart
 *		and fCurrnet <= fEnd.
 *		ForwardArrayIterator_Patch and BackwardArrayIterator_Patch SB public
 *		subclasses - oops - only borlands compiler caught this?
 *
 *		Revision 1.42  1992/11/15  15:59:18  lewis
 *		Merge in kdjs changes. Fixed iterators in the backward case -
 *		now portable test.
 *		Changed two divisions to multiplies (tricky - faster).
 *		Common base class for Array Iterators.
 *
 *		Revision 1.41  1992/11/12  08:00:11  lewis
 *		Got rid of include of <new.h> in header - do in source now.
 *		Add predeclares of templates like: template      <class  T>      class   Array;
 *		> in hopes of getting template versions to work - VERY bad luck
 *		so far... also worked around what MAYBE qBCC_TemplateFriendsNotSoFriendlyBug.
 *
 *		Revision 1.40  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.39  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.38  1992/11/05  15:18:26  lewis
 *		Forgot to return *this in op=. Fixed.
 *		Forgot to make Array_Patch_X public subclass - fixed.
 *
 *		Revision 1.37  1992/11/04  06:50:40  lewis
 *		Cleanup comments. Move ArrayNode inlines to implemtation section.
 *		Got rid of PatchViewsRealloc() after removeall - since no call to setslotsalloced()
 *		made there. Similarly with RemoveAt(). Be careful about changing these things -
 *		patches must be updated if that changes.
 *
 *		Revision 1.36  1992/11/03  17:02:39  lewis
 *		Make fData a ptr instead of a refernece since in op= for iterators
 *		we try to copy all the fields, and copying the fData copied the data,
 *		not the reference. Really we might just assert they came
 *		from the same fData, but I'm not sure we want to be that restrictive.
 *
 *		Revision 1.35  1992/11/02  19:46:31  lewis
 *		Renamed ActiveIterators() to HasActiveIterators().
 *		Decided it was OK to copy Array_Patch_X - and note that we dont
 *		copy iterators. For now assert this has no iterators when copied over.
 *		We could support this by adding a new kind of patch, but right now
 *		there seems to be no need.
 *
 *		Revision 1.34  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.33  1992/10/31  00:38:20  lewis
 *		Do X(X&) and op= for new iterators.
 *
 *		Revision 1.32  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.31  1992/10/29  16:34:48  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.30  1992/10/29  15:51:24  lewis
 *		Move CurrentIndex () inline definitions above Current() definitions since
 *		the latter calls on the former, and CFront 2.1 barfs otherwise.
 *
 *		Revision 1.29  1992/10/19  01:42:23  lewis
 *		Comments + moved CTOR/DTOR here to be inline - CTOR should be as small
 *		inlined as the function call sequence. And DTOR slightly larger - two calls
 *		vs 1, but so small a difference what the heck.
 *
 *		Revision 1.28  1992/10/16  22:45:41  lewis
 *		Cleanup assertions with new iterator stuff - wasnt quite right before.
 *
 *		Revision 1.27  1992/10/15  20:17:46  lewis
 *		Move fEnd-- to end of PatchRemove (in all 3 iterators) because we call public
 *		API method CurrentIndex () which does assertion, and we've temporarily
 *		left things in bad state at that point unless we do decrement at the end.
 *
 *		Revision 1.26  1992/10/14  15:46:16  lewis
 *		Cleanups, and fixed backward iterator, and backwards case
 *		of nu directediterator to be segmented architecture friendly-
 *		dont point before beginning of array!!!
 *
 *		Revision 1.25  1992/10/13  05:20:45  lewis
 *		Lots of changes. Added three new iterators - not based on subclassing
 *		from IteratorRep, and all inline. Also, made them ptr based - not
 *		indexing based. Changed op= to not call RemoveAll () unless necessary.
 *		Change SetLength() code to change setslotsalloced by scale of 1.1,
 *		instead of +32/sizeof(T). Use op new {} synax with g++ instead of old
 *		hack with new and memcpy. Try getting rid of lhs++ workaround in
 *		InserAt/RemoveAt - instead use new bug workaround for
 *		op= and X(X&) problem in ArrayNode for gcc.
 *
 *		Revision 1.24  1992/10/10  20:16:36  lewis
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
 *		Revision 1.21  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.20  1992/09/25  21:12:28  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.19  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.18  1992/09/23  01:12:27  lewis
 *		Workaround g++ 2.2.1 bug in insertat/removeat *lhs++ = *rhs--
 *		failed to compile under some circumstances (with mappingElt as
 *		type T). Have not yet simplified and reported bug - do later.
 *
 *		Revision 1.17  1992/09/21  05:14:42  sterling
 *		Breakup Implement macro into more parts.
 *
 *		Revision 1.16  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.14  1992/09/11  12:45:57  lewis
 *		Use qGCC_PlacementOpNewGeneratesSyntaxError define instead of qGCC.
 *
 *		Revision 1.13  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.12  1992/09/04  19:27:23  sterling
 *		Add Contains () method, and unconditionally delete array in DTOR (dont
 *		check for Nil).
 *
 *		Revision 1.11  1992/09/03  19:29:54  lewis
 *		Got rid of debugging declare/implement stuff at bottom of file.
 *		Added qGCC_LibgPlusPlusSharpDefineSize_tBug workaround.
 *		Added qMPW_CFront_SloppyCodeGenWithoutOpNewOverloadForPlacementOp for
 *		hack we do for cfront, cuz gcc 2.2.1 was barfing on it - didnt bother
 *		defing a gcc bug for this since we really dont want to do it
 *		anyhow. Also, worked around another - so far unnamed - bug with gcc and
 *		operator new occuring in Array(T)::SetLength () - just broke up
 *		declaration macro to fix this one trouble (placement operator broken)
 *
 *		Revision 1.9  1992/07/21  19:20:42  sterling
 *		Do buffering on the SetLength() call - bump slots alloced by
 *		an arguable but reasonable amount.
 *
 *		Revision 1.8  1992/07/21  05:19:38  lewis
 *		Forgot to say inline in front of inline array CTOR/DTOR and SetLength.
 *
 *		Revision 1.7  1992/07/17  18:21:27  lewis
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.6  1992/07/17  17:29:28  lewis
 *		Changed arg to InsertAt from const T& to T - just an oversight when
 *		we made this change to all the other container clases before.
 *		Made CTOR/DTOR and SetLength inline - this is important becasue
 *		of getting rid of Sequence_ArrayPtr - also they are VERY short, and
 *		there are no virtual methods.
 *		General cleanups, and scrunching of the macro version. Made GetSlotsAlloced const.
 *		Rewrote SetSlotsAlloced to use size_t rather than int as index - so gauranteed sizes
 *		right (sizeof (int) not necessarily = sizeof (size_t)).
 *
 *		Revision 1.5  1992/07/17  04:04:30  lewis
 *		Cleanup comments, and skrunch macro version, stripping unneeded whitespace,
 *		and comments, and making identical to the Template version (ie use
 *		size_t instead of UInt32).
 *
 *		Revision 1.4  1992/07/16  07:46:28  lewis
 *		Change magic include at end to be depend on "qRealTemplatesAvailable && !!qTemplatesHasRepository"
 *		instead of qBorlandCPlus.
 *
 *		Revision 1.3  1992/07/08  05:22:51  lewis
 *		Changed file name to include for template .cc stuff to UNIX pathname, and
 *		use PCName mapper to rename to appropriate PC File - cuz we may need to do
 *		this with gcc/aux too.
 *
 *		Revision 1.2  1992/07/01  07:18:10  lewis
 *		Get rid of arg to delete[] - size arg no longer needed on any system (since
 *		CFront21) and recieves warnings on all now.
 *
 *		Revision 1.10  92/05/21  17:20:53  17:20:53  lewis (Lewis Pringle)
 *		Modify template implmentation to use size_t rather than UInt32.
 *		
 *		Revision 1.9  92/05/08  21:25:30  21:25:30  lewis (Lewis Pringle)
 *		Ported to PC/Templates.
 *		
 *		Revision 1.8  92/03/30  16:08:29  16:08:29  lewis (Lewis Pringle)
 *		Implemented Templates version.
 *		
 *		Revision 1.6  1992/01/21  20:51:48  lewis
 *		Add Peek method.
 *
 *
 */

#include	"Debug.hh"



#if		qRealTemplatesAvailable
		
template	<class	T>	class	Array;
template	<class	T>	class	ArrayIteratorBase;
template	<class	T>	class	ArrayIterator_PatchBase;
template	<class	T>	class	ForwardArrayMutator;
template	<class	T>	class	BackwardArrayMutator;
template	<class	T>	class	ForwardArrayMutator_Patch;
template	<class	T>	class	BackwardArrayMutator_Patch;


/*
 *		We use this class ArrayNode as a hack to work around bugs in many
 *	C++ compilers (most notewory is CFront 2.1 - I'm not sure about others).
 *	The trouble is we must be able to call T::~T on builtin types like
 *	void* and int, and some compilers dont allow this. So we wrap a dummy
 *	class around T, and our problems go away. Everything is inline so
 *	this has no cost. When the last braindead compiler has been safely supplanted
 *	on all the machines we care about, we can loose this class.
 *	(LGP Tuesday, November 17, 1992 1:32:25 PM)
 */
template	<class	T>	class	ArrayNode {
	public:
#if		qGCC_TemplatesCTORWithArgMustBeInlineBug
		ArrayNode (T item) : fItem (item) {}
#else
		ArrayNode (T item);
#endif
		~ArrayNode ();

#if		qMPW_CFront_SloppyCodeGenWithoutOpNewOverloadForPlacementOp
		static	void*	operator new (size_t, void* p) 	{ return (p); }
		static	void	operator delete (void*)			{}
#endif

#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStarPlusPlusBug
		ArrayNode (const ArrayNode<T>& from);
		nonvirtual	const ArrayNode<T>& operator= (const ArrayNode<T>& rhs);
#endif

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	private:
#endif
		T	fItem;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	Array<T>;
	friend	class	ArrayIteratorBase<T>;
	friend	class	ArrayIterator_PatchBase<T>;
	friend	class	ForwardArrayMutator<T>;
	friend	class	BackwardArrayMutator<T>;
	friend	class	ForwardArrayMutator_Patch<T>;
	friend	class	BackwardArrayMutator_Patch<T>;
#endif
};


template	<class	T>	class	ArrayIteratorBase;
template	<class	T>	class	ArrayIterator_PatchBase;


/*
 *		This class is the main core of the implementation. It provides
 *	an array abstraction, where the size can be set dynamically, and
 *	extra sluff is maintained off the end to reduce copying from reallocs.
 *	Only items 1..GetLength () are kept constructed. The rest (GetLength()+1
 *	..fSlotsAlloced) are uninitialized memory. This is important because
 *	it means you can count on DTORs of your T being called when you
 *	remove them from contains, not when the caches happen to empty.
 */
template      <class  T>      class   Array {
	public:
		Array ();
		Array (const Array<T>& from);
		~Array ();

		nonvirtual	Array<T>& operator= (const Array<T>& rhs);

	public:
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

		nonvirtual	void	Invariant () const;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	protected:
#endif
		size_t			fLength;			// #items advertised/constructed
		size_t			fSlotsAllocated;	// #items allocated (though not necessarily initialized)
		ArrayNode<T>*	fItems;

#if		qDebug
		virtual	void	Invariant_ () const;
#endif

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	ArrayIteratorBase<T>;
	friend	class	ArrayIterator_PatchBase<T>;
#endif
};



/*
 *		ArrayIteratorBase<T> is an un-advertised implementation
 *	detail designed to help in source-code sharing among various
 *	iterator implementations.
 */
template	<class T>	class	ArrayIteratorBase {
	private:
		ArrayIteratorBase ();		// not defined - do not call.

	public:
		ArrayIteratorBase (const Array<T>& data);

		nonvirtual	T		Current () const;			//	Error to call if Done (), otherwise OK
		nonvirtual	size_t	CurrentIndex () const;		//	NB: This can be called if we are done - if so, it returns GetLength() + 1.
		nonvirtual	Boolean	Done () const;

		nonvirtual	void	Invariant () const;

	protected:
#if		qDebug
		const Array<T>*		fData;
#endif
		const ArrayNode<T>*	fStart;			// points to FIRST elt
		const ArrayNode<T>*	fEnd;			// points 1 PAST last elt
		const ArrayNode<T>*	fCurrent;		// points to CURRENT elt (SUBCLASSES MUST INITIALIZE THIS!)
		Boolean				fSupressMore;	// Indicates if More should do anything, or if were already Mored...

#if		qDebug
		virtual	void	Invariant_ () const;
#endif
};



/*
 *		Use this iterator to iterate forwards over the array. Be careful
 *	not to add or remove things from the array while using this iterator,
 *	since it is not safe. Use ForwardArrayIterator_Patch for those cases.
 */
template	<class T> class	ForwardArrayIterator : public ArrayIteratorBase<T> {
	public:
		ForwardArrayIterator (const Array<T>& data);

		nonvirtual	Boolean	More ();
};




/*
 *		ForwardArrayMutator<T> is the same as ForwardArrayIterator<T> but
 *	adds the ability to update the contents of the array as you go along.
 */
template	<class T> class	ForwardArrayMutator : public ForwardArrayIterator<T> {
	public:
		ForwardArrayMutator (Array<T>& data);

		nonvirtual	void	UpdateCurrent (T newValue);
};




/*
 *		Use this iterator to iterate backwards over the array. Be careful
 *	not to add or remove things from the array while using this iterator,
 *	since it is not safe. Use BackwardArrayIterator_Patch for those cases.
 */
template	<class T> class	BackwardArrayIterator : public ArrayIteratorBase<T> {
	public:
		BackwardArrayIterator (const Array<T>& data);

		nonvirtual	Boolean	More ();
};




/*
 *		BackwardArrayMutator<T> is the same as BackwardArrayIterator<T> but
 *	adds the ability to update the contents of the array as you go along.
 */
template	<class T> class	BackwardArrayMutator : public BackwardArrayIterator<T> {
	public:
		BackwardArrayMutator (Array<T>& data);

		nonvirtual	void	UpdateCurrent (T newValue);
};









/*
 *	Patching Support:
 *
 *		Here we provide Patching Versions of each iterator, and for convienience
 *	versions of array that maintain a list of all Patching iterators of a given
 *	type.
 */


template	<class T>	class	ArrayIterator_PatchBase;


/*
 *		Array_Patch<T> is an array implemantion that keeps a list of patchable
 *	iterators, and handles the patching automatically for you. Use this if
 *	you ever plan to use patchable iterators.
 */
template	<class T>	class	Array_Patch : public Array<T> {
	public:
		Array_Patch ();
		Array_Patch (const Array_Patch<T>& from);
		~Array_Patch ();

		nonvirtual	Array_Patch<T>& operator= (const Array_Patch<T>& rhs);

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
		nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);
		nonvirtual	void	Compact ();

	/*
	 * Methods to do the patching yourself. Iterate over all the iterators and
	 * perfrom patching.
	 */
	public:
		nonvirtual	Boolean	HasActiveIterators () const;			//	are there any iterators to be patched?
		nonvirtual	void	PatchViewsAdd (size_t index) const;		//	call after add
		nonvirtual	void	PatchViewsRemove (size_t index) const;	//	call before remove
		nonvirtual	void	PatchViewsRemoveAll () const;			//	call after removeall
		nonvirtual	void	PatchViewsRealloc () const;				//	call after realloc could have happened


	/*
	 *	Check Invariants for this class, and all the iterators we own.
	 */
	public:
		nonvirtual	void	Invariant () const;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	private:
#endif
		ArrayIterator_PatchBase<T>*	fIterators;

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	ArrayIterator_PatchBase<T>;
#endif

#if		qDebug
		override	void	Invariant_ () const;
		nonvirtual	void	InvariantOnIterators_ () const;
#endif
};




/*
 *		ArrayIterator_PatchBase<T> is a private utility class designed
 *	to promote source code sharing among the patched iterator implemetnations.
 */
template	<class T>	class	ArrayIterator_PatchBase : public ArrayIteratorBase<T> {
	public:
		ArrayIterator_PatchBase (const Array_Patch<T>& data);
		ArrayIterator_PatchBase (const ArrayIterator_PatchBase<T>& from);
		~ArrayIterator_PatchBase ();

		nonvirtual	ArrayIterator_PatchBase<T>&	operator= (const ArrayIterator_PatchBase<T>& rhs);

	public:
		nonvirtual	void	PatchAdd (size_t index);		//	call after add
		nonvirtual	void	PatchRemove (size_t index);		//	call before remove
		nonvirtual	void	PatchRemoveAll ();				//	call after removeall
		nonvirtual	void	PatchRealloc ();				//	call after realloc could have happened

	public:
#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	protected:
#endif
		const Array_Patch<T>* 		fData;
		ArrayIterator_PatchBase<T>*	fNext;

#if		qDebug
		override	void	Invariant_ () const;
#endif

		virtual		void	PatchRemoveCurrent ()	=	Nil;	// called from patchremove if patching current item...

#if		!qBCC_TemplateFriendsNotSoFriendlyBug || !qRealTemplatesAvailable
	friend	class	Array_Patch<T>;
#endif
};




/*
 *		ForwardArrayIterator_Patch<T> is forwards iterator that can be used
 *	while modifing its owned array. It can only be used with Array_Patch<T>
 *	since the classes know about each other, and keep track of each other.
 *	This is intended to be a convienience in implementing concrete container
 *	mixins.
 */
template	<class T> class	ForwardArrayIterator_Patch : public ArrayIterator_PatchBase<T> {
	public:
		ForwardArrayIterator_Patch (const Array_Patch<T>& data);

	public:
		nonvirtual	Boolean	More ();

	protected:
		override	void	PatchRemoveCurrent ();
};




/*
 *		ForwardArrayMutator_Patch<T> is the same as ForwardArrayIterator_Patch<T> but
 *	adds the ability to update the contents of the array as you go along.
 */
template	<class T> class	ForwardArrayMutator_Patch : public ForwardArrayIterator_Patch<T> {
	public:
		ForwardArrayMutator_Patch (Array_Patch<T>& data);

	public:
		nonvirtual	void	RemoveCurrent ();
		nonvirtual	void	UpdateCurrent (T newValue);
		nonvirtual	void	AddBefore (T item);				//	NB: Can be called if done
		nonvirtual	void	AddAfter (T item);
};




/*
 *		BackwardArrayIterator_Patch<T> is backwards iterator that can be used
 *	while modifing its owned array. It can only be used with Array_Patch<T>
 *	since the classes know about each other, and keep track of each other.
 *	This is intended to be a convienience in implementing concrete container
 *	mixins.
 */
template	<class T> class	BackwardArrayIterator_Patch : public ArrayIterator_PatchBase<T> {
	public:
		BackwardArrayIterator_Patch (const Array_Patch<T>& data);

	public:
		nonvirtual	Boolean	More ();

	protected:
		override	void	PatchRemoveCurrent ();
};




/*
 *		BackwardArrayMutator_Patch<T> is the same as BackwardArrayIterator_Patch<T> but
 *	adds the ability to update the contents of the array as you go along.
 */
template	<class T> class	BackwardArrayMutator_Patch : public BackwardArrayIterator_Patch<T> {
	public:
		BackwardArrayMutator_Patch (Array_Patch<T>& data);

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

	// class	ArrayNode<T>
#if		!qGCC_TemplatesCTORWithArgMustBeInlineBug
	template	<class	T>	inline	ArrayNode<T>::ArrayNode (T item) :
		fItem (item)
	{
	}
#endif
	template	<class	T>	inline	ArrayNode<T>::~ArrayNode ()
	{
	}
#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStarPlusPlusBug
	template	<class	T>	inline	ArrayNode<T>::ArrayNode (const ArrayNode<T>& from) :
		fItem (from.fItem)
	{
	}
	template	<class	T>	inline	const ArrayNode<T>& ArrayNode<T>::operator= (const ArrayNode<T>& rhs)
	{
		fItem = rhs.fItem;
		return (*this);
	}
#endif


	// class	Array<T>
	template	<class	T>	inline	void	Array<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
#endif
	}
	template	<class	T>	inline	Array<T>::Array () :
		fLength (0),
		fSlotsAllocated (0),
		fItems (Nil)
	{
	}
	template	<class	T>	inline	Array<T>::~Array ()
	{
		RemoveAll ();
		delete (char*)fItems;
	}
	template	<class	T>	inline	T	Array<T>::GetAt (size_t i) const
	{
		Require (i >= 1);
		Require (i <= fLength);
		return (fItems [i-1].fItem);
	}
	template	<class	T>	inline	void	Array<T>::SetAt (T item, size_t i)
	{
		Require (i >= 1);
		Require (i <= fLength);
		fItems [i-1].fItem = item;
	}
	template	<class	T>	inline	T&	Array<T>::operator[] (size_t i)
	{
		Require (i >= 1);
		Require (i <= fLength);
		return (fItems [i-1].fItem);
	}
	template	<class	T>	inline	T	Array<T>::operator[] (size_t i) const
	{
		Require (i >= 1);
		Require (i <= fLength);
		return (fItems [i-1].fItem);
	}
	template	<class	T>	inline	size_t	Array<T>::GetLength () const
	{
		return (fLength);
	}
	template	<class	T>	inline	size_t	Array<T>::GetSlotsAlloced () const
	{
		return (fSlotsAllocated);
	}
	template	<class	T>	inline	void	Array<T>::Compact ()
	{
		SetSlotsAlloced (GetLength ());
	}


	// Class ArrayIteratorBase<T>
	template	<class T>	inline	void	ArrayIteratorBase<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
#endif
	}
	template	<class T>	inline	ArrayIteratorBase<T>::ArrayIteratorBase (const Array<T>& data) :
#if		qDebug
		fData (&data),
#endif
		fStart (&data.fItems[0]),
		fEnd (&data.fItems[data.GetLength ()]),
		//fCurrent ()							dont initialize - done in subclasses...
		fSupressMore (True)				// first time thru - cuz of how used in for loops...
	{
		#if		qDebug
			fCurrent = Nil;	// more likely to cause bugs...(leave the xtra newline cuz of genclass bug...)

		#endif
		/*
		 * Cannot call invariant () here since fCurrent not yet setup.
		 */
	}
	template	<class T>	inline	Boolean	ArrayIteratorBase<T>::Done () const
	{
		Invariant ();
		return Boolean (fCurrent == fEnd);
	}
	template	<class T>	inline	size_t	ArrayIteratorBase<T>::CurrentIndex () const
	{
		/*
		 * NB: This can be called if we are done - if so, it returns GetLength() + 1.
		 */
		Invariant ();
		return ((fCurrent-fStart)+1);
	}
	template	<class T>	inline	T		ArrayIteratorBase<T>::Current () const
	{
		Ensure (fData->GetAt (CurrentIndex ()) == fCurrent->fItem);
		Invariant ();
		return (fCurrent->fItem);
	}


	// Class ForwardArrayIterator<T>
	template	<class T>	inline	ForwardArrayIterator<T>::ForwardArrayIterator (const Array<T>& data) :
		ArrayIteratorBase<T>(data)
	{
		fCurrent = fStart;
		Invariant ();
	}
	template	<class T>	inline	Boolean	ForwardArrayIterator<T>::More ()
	{
		Invariant ();
		if (not fSupressMore and not Done ()) {
			Assert (fCurrent < fEnd);
			fCurrent++;
		}
		fSupressMore = False;
		Invariant ();
		return (not Done ());
	}


	// Class ForwardArrayMutator<T>
	template	<class T>	inline	ForwardArrayMutator<T>::ForwardArrayMutator (Array<T>& data) :
		ForwardArrayIterator<T>((const Array<T>&)data)
	{
		Invariant ();
	}
	template	<class T>	inline	void	ForwardArrayMutator<T>::UpdateCurrent (T newValue)
	{
		Invariant ();
		Require (not Done ());
		AssertNotNil (fCurrent);
		((ArrayNode<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better
														// the (~const)
	}


	// Class BackwardArrayIterator<T>
	template	<class T>	inline	BackwardArrayIterator<T>::BackwardArrayIterator (const Array<T>& data) :
		ArrayIteratorBase<T>(data)
	{
		if (data.GetLength () == 0) {
			fCurrent = fEnd;	// magic to indicate done
		}
		else {
			fCurrent = fEnd-1;	// last valid item
		}
		Invariant ();
	}
#if		!qCFront_SorryNotImplementedStatementAfterReturnInline
	template	<class T>	inline	Boolean	BackwardArrayIterator<T>::More ()
	{
		Invariant ();
		if (fSupressMore) {
			fSupressMore = False;
			return (not Done ());
		}
		else {
			if (Done ()) {
				return (False);
			}
			else {
				if (fCurrent == fStart) {
					fCurrent = fEnd;	// magic to indicate done
					Ensure (Done ());
					return (False);
				}
				else {
					fCurrent--;
					Ensure (not Done ());
					return (True);
				}
			}
		}
		AssertNotReached ();	return (False);
	}
#endif	/*!qCFront_SorryNotImplementedStatementAfterReturnInline*/

	// Class BackwardArrayMutator<T>
	template	<class T>	inline	BackwardArrayMutator<T>::BackwardArrayMutator (Array<T>& data) :
		BackwardArrayIterator<T>((const Array<T>&)data)
	{
		Invariant ();
	}
	template	<class T>	inline	void	BackwardArrayMutator<T>::UpdateCurrent (T newValue)
	{
		Invariant ();
		Require (not Done ());
		AssertNotNil (fCurrent);
		((ArrayNode<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better
														// the (~const)
	}



	/*
	 **************************** Patching code ****************************
	 */

	// Class ArrayIterator_PatchBase<T>
	template	<class T>	inline	ArrayIterator_PatchBase<T>::ArrayIterator_PatchBase (const Array_Patch<T>& data) :
		ArrayIteratorBase<T>(data),
		fData (&data),
		fNext (data.fIterators)
	{
		//(~const)
		((Array_Patch<T>*)fData)->fIterators = this;
		/*
		 * Cannot call invariant () here since fCurrent not yet setup.
		 */
	}
	template	<class T>	inline	ArrayIterator_PatchBase<T>::ArrayIterator_PatchBase (const ArrayIterator_PatchBase<T>& from) :
		ArrayIteratorBase<T>(from),
		fData (from.fData),
		fNext (from.fData->fIterators)
	{
		RequireNotNil (fData);
		//(~const)
		((Array_Patch<T>*)fData)->fIterators = this;
		Invariant ();
	}
	template	<class T>	inline	ArrayIterator_PatchBase<T>::~ArrayIterator_PatchBase ()
	{
		Invariant ();
		AssertNotNil (fData);
		if (fData->fIterators == this) {
			//(~const)
			((Array_Patch<T>*)fData)->fIterators = fNext;
		}
		else {
			for (ArrayIterator_PatchBase<T>* v = fData->fIterators; v->fNext != this; v = v->fNext) {
				AssertNotNil (v);
				AssertNotNil (v->fNext);
			}
			AssertNotNil (v);
			Assert (v->fNext == this);
			v->fNext = fNext;
		}
	}
	template	<class T>	inline	ArrayIterator_PatchBase<T>&	ArrayIterator_PatchBase<T>::operator= (const ArrayIterator_PatchBase<T>& rhs)
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
				((Array_Patch<T>*)fData)->fIterators = fNext;
			}
			else {
				for (ArrayIterator_PatchBase<T>* v = fData->fIterators; v->fNext != this; v = v->fNext) {
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
			((Array_Patch<T>*)fData)->fIterators = this;
		}

#if		qGCC_OpEqualsNotAutoDefinedSoCanBeExplicitlyCalledBug
		(*(ArrayIteratorBase<T>*)this) = (*(ArrayIteratorBase<T>*)&rhs);
#else
		ArrayIteratorBase<T>::operator=(rhs);
#endif
		Invariant ();
		return (*this);
	}
	template	<class T>	inline	void	ArrayIterator_PatchBase<T>::PatchAdd (size_t index)
	{
		/*
		 *		NB: We cannot call invariants here because this is called after the add
		 *	and the PatchRealloc has not yet happened.
		 */
		Require (index >= 1);

		fEnd++;
		AssertNotNil (fData);

		/*
		 *		If we added an item to the right of our cursor, it has no effect
		 *	on our - by index - addressing, and so ignore it. We will eventually
		 *	reach that new item.
		 *
		 *		On the other hand, if we add the item to the left of our cursor,
		 *	things are more complex:
		 *
		 *		If we added an item left of the cursor, then we are now pointing to
		 *	the item before the one we used to, and so incrementing (ie Next)
		 *	would cause us to revisit (in the forwards case, or skip one in the
		 *	reverse case). To correct our index, we must increment it so that
		 *	it.Current () refers to the same entity.
		 *
		 *		Note that this should indeed by <=, since (as opposed to <) since
		 *	if we are a direct hit, and someone tries to insert something at
		 *	the position we are at, the same argument as before applies - we
		 *	would be revisiting, or skipping forwards an item.
		 */
		Require (index <= (fEnd-fStart));
		if (&fStart[index-1] <= fCurrent) {		// index <= CurrentIndex () - only faster
												// Cannot call CurrentIndex () since invariants
												// might fail at this point
			fCurrent++;
		}
	}
	template	<class T>	inline	void	ArrayIterator_PatchBase<T>::PatchRemove (size_t index)
	{
		Require (index >= 1);
		Require (index <= fData->GetLength ());

		/*
		 *		If we are removing an item from the right of our cursor, it has no effect
		 *	on our - by index - addressing, and so ignore it.
		 *
		 *		On the other hand, if we are removing the item from the left of our cursor,
		 *	things are more complex:
		 *
		 *		If we are removing an item from the left of the cursor, then we are now
		 *	pointing to the item after the one we used to, and so decrementing (ie Next)
		 *	would cause us to skip one. To correct our index, we must decrement it so that
		 *	it.Current () refers to the same entity.
		 *
		 *		In the case where we are directly hit, just set fSupressMore
		 *	to true. If we are going forwards, are are already pointing where
		 *	we should be (and this works for repeat deletions). If we are
		 *	going backwards, then fSupressMore will be ignored, but for the
		 *	sake of code sharing, its tough to do much about that waste.
		 */
		Assert ((&fStart[index-1] <= fCurrent) == (index <= CurrentIndex ()));		// index <= CurrentIndex () - only faster
		if (&fStart[index-1] < fCurrent) {
			Assert (CurrentIndex () >= 2);		// cuz then index would be <= 0, and thats imposible
			fCurrent--;
		}
		else if (&fStart[index-1] == fCurrent) {
			PatchRemoveCurrent ();
		}
		// Decrement at the end since CurrentIndex () calls stuff that asserts (fEnd-fStart) == fData->GetLength ()
		Assert ((fEnd-fStart) == fData->GetLength ());		//	since called before remove

		/*
		 * At this point, fCurrent could be == fEnd - must not lest fCurrent point past!
		 */
		if (fCurrent == fEnd) {
			Assert (fCurrent > fStart);	// since we are removing something start!=end
			fCurrent--;
		}
		fEnd--;
	}
	template	<class T>	inline	void	ArrayIterator_PatchBase<T>::PatchRemoveAll ()
	{
		Require (fData->GetLength () == 0);		//	since called after removeall
		fCurrent = fData->fItems;
		fStart = fData->fItems;
		fEnd = fData->fItems;
		fSupressMore = True;
	}
	template	<class T>	inline	void	ArrayIterator_PatchBase<T>::PatchRealloc ()
	{
		/*
		 *		NB: We can only call invariant after we've fixed things up, since realloc
		 * has happened by now, but things don't point to the right places yet.
		 */
		if (fStart != fData->fItems) {
			const	ArrayNode<T>*	oldStart	=	fStart;
			fStart = fData->fItems;
			fCurrent = fData->fItems + (fCurrent-oldStart);
			fEnd = fData->fItems + (fEnd-oldStart);
		}
	}


	// class Array_Patch<T>
	template	<class	T>	inline	void	Array_Patch<T>::Invariant () const
	{
#if		qDebug
		Invariant_ ();
		InvariantOnIterators_ ();
#endif
	}
	template	<class	T>	inline	Array_Patch<T>::Array_Patch () :
		Array<T> (),
		fIterators (Nil)
	{
		Invariant ();
	}
	template	<class	T>	inline	Array_Patch<T>::Array_Patch (const Array_Patch<T>& from) :
		Array<T> (from),
		fIterators (Nil)	// Don't copy the list of iterators - would be trouble with backpointers!
							// Could clone but that would do no good, since nobody else would have pointers to them
	{
		Invariant ();
	}
	template	<class	T>	inline	Array_Patch<T>::~Array_Patch ()
	{
		Require (fIterators == Nil);
		Invariant ();
	}
	template	<class	T>	inline	Boolean	Array_Patch<T>::HasActiveIterators () const
	{
		return Boolean (fIterators != Nil);
	}
	template	<class T>	inline	void	Array_Patch<T>::PatchViewsAdd (size_t index) const
	{
		/*
		 *		Must call PatchRealloc before PatchAdd() since the test of currentIndex
		 *	depends on things being properly adjusted.
		 */
		for (ArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRealloc ();
			v->PatchAdd (index);
		}
	}
	template	<class T>	inline	void	Array_Patch<T>::PatchViewsRemove (size_t index) const
	{
		for (ArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRemove (index);
		}
	}
	template	<class T>	inline	void	Array_Patch<T>::PatchViewsRemoveAll () const
	{
		for (ArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRemoveAll ();
		}
	}
	template	<class T>	inline	void	Array_Patch<T>::PatchViewsRealloc () const
	{
		for (ArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
			v->PatchRealloc ();
		}
	}
	template	<class	T>	inline	Array_Patch<T>& Array_Patch<T>::operator= (const Array_Patch<T>& rhs)
	{
		/*
		 * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
		 * If this is to be supported at some future date, well need to work on our patching.
		 */
		Assert (not (HasActiveIterators ()));	// cuz copy of array does not copy iterators...
		Invariant ();
		Array<T>::operator= (rhs);
		Invariant ();
		return (*this);
	}
	template	<class	T>	inline	void	Array_Patch<T>::SetLength (size_t newLength, T fillValue)
	{
		// For now, not sure how to patch the iterators, so just Assert out - fix later ...
		AssertNotReached ();
	}
	template	<class	T>	inline	void	Array_Patch<T>::InsertAt (T item, size_t index)
	{
		Invariant ();
		Array<T>::InsertAt (item, index);
		PatchViewsAdd (index);			// PatchRealloc done in PatchViewsAdd()
		Invariant ();
	}
	template	<class	T>	inline	void	Array_Patch<T>::RemoveAt (size_t index)
	{
		Invariant ();
		PatchViewsRemove (index);
		Array<T>::RemoveAt (index);
		// Dont call PatchViewsRealloc () since removeat does not do a setslotsalloced, it
		// just destructs things.
		Invariant ();
	}
	template	<class	T>	inline	void	Array_Patch<T>::RemoveAll ()
	{
		Invariant ();
		Array<T>::RemoveAll ();
		PatchViewsRemoveAll ();		// PatchRealloc not needed cuz removeall just destructs things,
									// it does not realloc pointers (ie does not call setslotsalloced).
		Invariant ();
	}
	template	<class	T>	inline	void	Array_Patch<T>::SetSlotsAlloced (size_t slotsAlloced)
	{
		Invariant ();
		Array<T>::SetSlotsAlloced (slotsAlloced);
		PatchViewsRealloc ();
		Invariant ();
	}
	template	<class	T>	inline	void	Array_Patch<T>::Compact ()
	{
		Invariant ();
		Array<T>::Compact ();
		PatchViewsRealloc ();
		Invariant ();
	}



	// class ForwardArrayIterator_Patch<T>
	template	<class T>	inline	ForwardArrayIterator_Patch<T>::ForwardArrayIterator_Patch (const Array_Patch<T>& data) :
		ArrayIterator_PatchBase<T> (data)
	{
		fCurrent = fStart;
		Invariant ();
	}
	template	<class T>	inline	Boolean	ForwardArrayIterator_Patch<T>::More ()
	{
		Invariant ();
		if (not fSupressMore and not Done ()) {
			Assert (fCurrent < fEnd);
			fCurrent++;
		}
		fSupressMore = False;
		Invariant ();
		return (not Done ());
	}
	template	<class T>	inline	void	ForwardArrayIterator_Patch<T>::PatchRemoveCurrent ()
	{
		Assert (fCurrent < fEnd);	// cannot remove something past the end
		fSupressMore = True;
	}


	// Class ForwardArrayMutator<T>
	template	<class T>	inline	ForwardArrayMutator_Patch<T>::ForwardArrayMutator_Patch (Array_Patch<T>& data) :
		ForwardArrayIterator_Patch<T>((const Array_Patch<T>&)data)
	{
		Invariant ();
	}
	template	<class T>	inline	void	ForwardArrayMutator_Patch<T>::RemoveCurrent ()
	{
		Invariant ();
		AssertNotNil (fData);
		((Array_Patch<T>*)fData)->RemoveAt (CurrentIndex ());				// the (~const)
		Invariant ();
	}
	template	<class T>	inline	void	ForwardArrayMutator_Patch<T>::UpdateCurrent (T newValue)
	{
		Invariant ();
		Require (not Done ());
		AssertNotNil (fCurrent);
		((ArrayNode<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better
														// the (~const)
	}
	template	<class T>	inline	void	ForwardArrayMutator_Patch<T>::AddBefore (T newValue)
	{
		/*
		 * NB: This can be called if we are done.
		 */
		Invariant ();
		AssertNotNil (fData);
		((Array_Patch<T>*)fData)->InsertAt (newValue, CurrentIndex ());		// the (~const)
		Invariant ();
	}
	template	<class T>	inline	void	ForwardArrayMutator_Patch<T>::AddAfter (T newValue)
	{
		Require (not Done ());
		Invariant ();
		AssertNotNil (fData);
		((Array_Patch<T>*)fData)->InsertAt (newValue, CurrentIndex ()+1);	// the (~const)
		Invariant ();
	}


	// class BackwardArrayIterator_Patch<T>
	template	<class T>	inline	BackwardArrayIterator_Patch<T>::BackwardArrayIterator_Patch (const Array_Patch<T>& data) :
		ArrayIterator_PatchBase<T> (data)
	{
		if (data.GetLength () == 0) {
			fCurrent = fEnd;	// magic to indicate done
		}
		else {
			fCurrent = fEnd-1;	// last valid item
		}
		Invariant ();
	}
#if		!qCFront_SorryNotImplementedStatementAfterReturnInline
	// Careful to keep hdr and src copies identical...
	template	<class T>	inline	Boolean	BackwardArrayIterator_Patch<T>::More ()
	{
		Invariant ();
		if (fSupressMore) {
			fSupressMore = False;
			return (not Done ());
		}
		else {
			if (Done ()) {
				return (False);
			}
			else {
				if (fCurrent == fStart) {
					fCurrent = fEnd;	// magic to indicate done
					Ensure (Done ());
					return (False);
				}
				else {
					fCurrent--;
					Ensure (not Done ());
					return (True);
				}
			}
		}
		AssertNotReached ();	return (False);
	}
#endif	/*!qCFront_SorryNotImplementedStatementAfterReturnInline*/
	template	<class T>	inline	void	BackwardArrayIterator_Patch<T>::PatchRemoveCurrent ()
	{
		if (fCurrent == fStart) {
			fCurrent = fEnd;	// magic to indicate done
		}
		else {
			Assert (fCurrent > fStart);
			fCurrent--;
		}
		fSupressMore = True;
	}


	// Class BackwardArrayMutator<T>
	template	<class T>	inline	BackwardArrayMutator_Patch<T>::BackwardArrayMutator_Patch (Array_Patch<T>& data) :
		BackwardArrayIterator_Patch<T>((const Array_Patch<T>&)data)
	{
		Invariant ();
	}
	template	<class T>	inline	void	BackwardArrayMutator_Patch<T>::RemoveCurrent ()
	{
		Invariant ();
		AssertNotNil (fData);
		((Array_Patch<T>*)fData)->RemoveAt (CurrentIndex ());				// (~const)
	}
	template	<class T>	inline	void	BackwardArrayMutator_Patch<T>::UpdateCurrent (T newValue)
	{
		Invariant ();
		Require (not Done ());
		AssertNotNil (fCurrent);
		((ArrayNode<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better
														// the (~const)
	}
	template	<class T>	inline	void	BackwardArrayMutator_Patch<T>::AddBefore (T newValue)
	{
		Require (not Done ());
		Invariant ();
		AssertNotNil (fData);
		((Array_Patch<T>*)fData)->InsertAt (newValue, CurrentIndex ());		// (~const)
		Invariant ();
	}
	template	<class T>	inline	void	BackwardArrayMutator_Patch<T>::AddAfter (T newValue)
	{
		/*
		 * NB: This can be called if we are done.
		 */
		Invariant ();
		AssertNotNil (fData);
		((Array_Patch<T>*)fData)->InsertAt (newValue, CurrentIndex ()+1);	// (~const)
		Invariant ();
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
	#define	qIncluding_ArrayCC	1
		#include	"../Sources/Array.cc"
	#undef	qIncluding_ArrayCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Array__*/


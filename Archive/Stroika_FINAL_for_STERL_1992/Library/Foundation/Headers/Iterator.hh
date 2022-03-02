/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Iterator__
#define	__Iterator__

/*
 * $Header: /fuji/lewis/RCS/Iterator.hh,v 1.21 1992/12/04 18:40:57 lewis Exp $
 *
 * Description:
 *
 *		Iterators allow ordered traversal of a Collection. In general, the only
 *	guarantee about iteration order is that it will remain the same as long as
 *	the Collection has not been modified. Difference subclasses of Collection
 *	more narrowly specify the details of traversal order: for example a Stack
 *	will always iterate from the top-most to the bottom-most item.
 *
 *		Iterators are robust against changes to their collection. Adding or
 *	removing items from a collection will not invalidate the iteration. The
 *	only exception is that removing the item currently being iterated over
 *	will invalidate the results of the Current () method (until the next call
 *	to the Next () method). Subclasses of Collection can make further
 *	guarantees about the behavior of iterators in the presence of Collection
 *	modifications. For example a SequenceIterator will always traverse any
 *	items added after the current traversal index, and will never traverse
 *	items added with an index before the current traversal index.
 *
 *		IteratorRep is an envelope class over IteratorRep. Iterators can be
 *	directly used as follows:
 *		for (IteratorRep<int> it (fIntList); not it.Done (); it.Next ()) {
 *			int	current = it.Current ();	// etc
 *		}
 *
 *		Two macros, ForEach and ForEachT (stands for ForEachTyped) can be used
 *	to minimize typing. The ForEach macro takes three arguments: the type (T)
 *	of the collection, the name of the iterator, and the collection to be
 *	iterated over. Thus the equivalent to the above is:
 *		ForEach (int, it, fList) {
 *			int current = it.Current ();	// etc
 *		}
 *
 *		The ForEachT macro takes one additional argument as the first
 *	parameter: the type of iterator. This is used in places where a subclass
 *	of the class IteratorRep is desired. Common subclasses include
 *	SequenceIterator, which provides a CurrentIndex method and can be
 *	traversed in either direction, and Mutators, which are subclasses of
 *	Iterators that allow Collection modifications through methods such as
 *	RemoveCurrent () or UpdateCurrent (). Sample Usage:
 *		ForEachT (SequenceIterator, int, it, fList) {
 *			int current = it.Current ();
 *			size_t	index = it.CurrentIndex ();		// etc
 *		}
 *
 *		Note that we utilize a for-loop based approach to iteration. A
 *	somewhat popular alternative is modeled on Lisp usage: iterating over a
 *	passed in function. This style is sometimes refereed to as "passive"
 *	iteration. However, given C++ lack of support for lambda expressions
 *	(anonymous code blocks) the for-loop based approach seems superior,
 *	since it is always at least as convenient and at least as efficient,
 *	and often is slightly more convenient and slightly more convenient.
 *	DieHards can write ForEach style macros to support the passive style.
 *	For example:
 *		#define	Apply(T,Init,F)\
 			{for (IteratorRep<T> it (Init); not it.Done (); it.Next ())	{ (*F) (it.Current ()); }}
 *	allows usages like Apply(int, fList, PrintInt); 
 *	Note: the extra braces above avoid variable name collisions, a minor
 *	benefit of the passive approach.
 *
 *
 * MORE RULES ABOUT ITERATORS (TO BE INTEGRATED INTO DOCS BETTER)
 *
 *		(1)		What is Done() cannot be UnDone()
 *				That is, once an iterator is done, it cannot be restarted.
 *		(2)		Iterators can be copied. They always refer to the same
 *				place they did before the copy, and the old iterator is unaffected
 *				by iteration in the new (though it can be affected indirectly
 *				thru mutations).
 *		(3)		Whether or not you encounter items added after an addition is
 *				undefined.
 *		(4)		A consequence of the above, is that items added after you are done,
 *				are not encountered. I'm not sure if this is currently true. We
 *				may want to rethink definitions, or somehow make this true.
 *
 *
 *	TODO:
 *		->	Should we keep the done variable in Iterator, and assert not
 *			done in Current(), and maybe allow Done () be called on iterators.
 *			Now sure will ever be used given usual More() interface, but it
 *			could be useful. Trouble is that it would cost some to store this
 *			variable, and it might never be used??? Might be OK if its use
 *			could be optimized away???
 *
 *		->	Merge Current virtual call into More() call? Trouble is constructing
 *			T. We could make fields char fCurrent[sizeof(T)] but that poses problems
 *			for copying iterators. On balance, probably best to bag it!!!
 *
 *			Another possability here is having IteratorRep have a ptr to T (ie T*),
 *			that can be peeked at by Current(). This would still involve extra indirection,
 *			but at least no virtual function overhead on the call. That is probably
 *			the best compromise!!!
 *
 * Changes:
 *	$Log: Iterator.hh,v $
 *		Revision 1.21  1992/12/04  18:40:57  lewis
 *		Disable hack for #define Iterator(T) Iterator<T> since failed with BCC -
 *		investigate later.
 *
 *		Revision 1.20  1992/12/03  08:06:17  lewis
 *		Work around GenClass quirks.
 *
 *		Revision 1.19  1992/12/03  07:29:05  lewis
 *		Got rid of our computation of Done() based on More calls - add
 *		Done() virtual to IteratorRep() - based on last Itertion SUMMIT MEETING.
 *		Now ALWAYS legal to ask DONE() and IFF DONE() TRUE legal to call Current().
 *		Added Template/Macro compatability #defines for Iterator etc on
 *		a trail basis.
 *
 *		Revision 1.18  1992/11/25  02:58:50  lewis
 *		Added workaround for qMPW_CFront_ConversionOperatorInitializerCodeGenBug.
 *		Get rid of DebugMessages() about copy of iterators.
 *
 *		Revision 1.17  1992/11/23  22:48:13  lewis
 *		Work around qGCC_ConversionOperatorInitializerConfusionBug.
 *
 *		Revision 1.16  1992/11/20  19:17:56  lewis
 *		Big change - get rid of Next and use More() returing Boolean instead.
 *		Somewhat controversial choice is keeping Done() method - it
 *		seems to only be used in assertions. Not too expensive though,
 *		and could plausibly be optimized away by a good compiler.
 *		Change ForEach macro appropriately.
 *
 *		Revision 1.15  1992/11/17  05:29:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.14  1992/11/12  08:05:29  lewis
 *		Try to get template based iterators working with composite
 *		types like MapElement<Key,T> but didn't quite get there.
 *
 *		Revision 1.13  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.12  1992/11/03  17:04:40  lewis
 *		Add comments on some rules we should consider about iterators.
 *
 *		Revision 1.11  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.10  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.9  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.8  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.7  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.6  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.4  1992/09/20  01:13:12  lewis
 *		Added prototype code suggested by kdj to test his new ForEach
 *		macro paradigm. Not tested yet, but put code in place to test later.
 *		Should be innocuous.
 *
 *		Revision 1.3  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.2  1992/09/04  20:00:55  sterling
 *		Added Description.
 *
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"Debug.hh"



#if		qRealTemplatesAvailable

/*
 *		Iterator are used primarily to get auto-destruction
 *	at the end of a scope where they are used. They are not intended to be used
 *	directly, but just from the ForEach macros.
 *
 *		Logically, it should be a subclass of IteratorRep<T>, but we
 *	avoid this as a performance hack since they we'd generate vtables, etc...
 *	(at least on all known - imho lousy - compilers).
 *
 */
template	<class T> class	IteratorRep;
template	<class T> class	Iterator {
	public:
		Iterator (IteratorRep<T>* it);
		Iterator (const Iterator<T>& from);
		~Iterator ();

		nonvirtual	Iterator<T>&	operator= (const Iterator<T>& rhs);

	private:
		Iterator ();	// Never implemented - illegal

	public:
		nonvirtual	Boolean	Done () const;
		nonvirtual	Boolean	More ();
		nonvirtual	T		Current () const;

	protected:
		IteratorRep<T>*	fIterator;
};



/*
 *		An iterator is a helper class, that allows ordered access to the
 *	elements of an ADT. Iterators cannot alter the contents of their ADT,
 *	but are robust over changes to their ADT.
 */
template	<class T> class	IteratorRep {
	protected:
		IteratorRep ();

	public:
		virtual	~IteratorRep ();

// COMMENTS OUT OF DATE!!!
	/*
	 *		Done () can be called anytime, and it tells if the iteration
	 *	is finished.
	 *
	 *		Next () can only be called if Not Done, OR if you were not done
	 *	before, and you deleted the current item. The reason for this
	 *	apparantly bizarre exception is because a natural use of these
	 *	methods is in a C for loop, with the Next () being called in the
	 *	re-initalization portion, and Done () in the test portion of the for
	 *	loop. If you get into the body of the for loop, and delete the current
	 *	item - causing your iteration to be completed, then you WILL call Next
	 *	before Done in this case. This is the only case we allow however. It
	 *	is not - in general - allowed to call Next when we are already done.
	 *
	 *		Current () can be called anytime Done () is false. It returns the
	 *	current item in the iteration process. It is undefined what Current ()
	 *	will return if it is deleted while current - however - if anything
	 *	else is deleted, Current () is guaranteed to be valid, and remain
	 *	the same.
	 *
	 */
	public:
		virtual	Boolean			Done () const		=	Nil;
		virtual	Boolean			More ()				=	Nil;
		virtual	T				Current () const	=	Nil;
		virtual	IteratorRep<T>*	Clone () const		=	Nil;
};

#endif	/*qRealTemplatesAvailable*/



/*
 * ForEach Macros.
 */
#if		qRealTemplatesAvailable

	#if		qGCC_ConversionOperatorInitializerConfusionBug || qMPW_CFront_ConversionOperatorInitializerCodeGenBug
		#define	ForEach(T,It,Init)			for (Iterator<T > It = (Iterator<T >)(Init); It.More ();)
		#define	ForEachT(ItT,T,It,Init)		for (ItT<T > It = (ItT<T >)(Init); It.More ();)
	#else
		#define	ForEach(T,It,Init)			for (Iterator<T > It (Init); It.More ();)
		#define	ForEachT(ItT,T,It,Init)		for (ItT<T > It (Init); It.More ();)
	#endif

#endif	/*qRealTemplatesAvailable*/
#if		!qRealTemplatesAvailable

	#if		qGCC_ConversionOperatorInitializerConfusionBug || qMPW_CFront_ConversionOperatorInitializerCodeGenBug
		#define	ForEach(T,It,Init)			for (Iterator(T) It = (Iterator(T))(Init); It.More ();)
		#define	ForEachT(ItT,T,It,Init)		for (ItT(T) It = (ItT(T))(Init); It.More ();)
	#else
		#define	ForEach(T,It,Init)			for (Iterator(T) It (Init); It.More ();)
		#define	ForEachT(ItT,T,It,Init)		for (ItT(T) It (Init); It.More ();)
	#endif

#endif	/*!qRealTemplatesAvailable*/




/*
 *	Template/Macro compatability.
 *		Use macro notation so that the code will compiler properly whether
 *	using macros or templates. This will go away when we stop supporting
 *	macros-based templates altogether.
 *
 *	NB:	&& 1 below cuz of GenClass quirks
 */
#if		qRealTemplatesAvailable && 0
// Disabled for now - did not work with BCC 3.1 - not sure why - investigate
// later. -- LGP Friday, December 4, 1992 1:22:47 PM
	#define	Iterator(T)		Iterator<T>
	#define	IteratorRep(T)	IteratorRep<T>
#endif





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

	// class IteratorRep<T>
	template	<class T> inline	IteratorRep<T>::IteratorRep ()
	{
	}
	template	<class T> inline	IteratorRep<T>::~IteratorRep ()
	{
	}

	// class Iterator<T>
	template	<class T> inline	Iterator<T>::Iterator (const Iterator<T>& from) :
		fIterator (Nil)
	{
		RequireNotNil (from.fIterator);
		fIterator = from.fIterator->Clone ();
		EnsureNotNil (fIterator);
	}
	template	<class T> inline	Iterator<T>::Iterator (IteratorRep<T>* it) :
		fIterator (it)
	{
		RequireNotNil (it);
		EnsureNotNil (fIterator);
	}
	template	<class T> inline	Iterator<T>::~Iterator ()
	{
		delete fIterator;
	}
	template	<class T> inline	Iterator<T>&	Iterator<T>::operator= (const Iterator<T>& rhs)
	{
		RequireNotNil (fIterator);
		RequireNotNil (rhs.fIterator);
		if (fIterator != rhs.fIterator) {
			delete fIterator;
			fIterator = rhs.fIterator->Clone ();
			EnsureNotNil (fIterator);
		}
		return (*this);
	}
	template	<class T> inline	Boolean	Iterator<T>::Done () const
	{
		RequireNotNil (fIterator);
		return (fIterator->Done ());
	}
	template	<class T> inline	Boolean	Iterator<T>::More ()
	{
		RequireNotNil (fIterator);
		return (fIterator->More ());
	}
	template	<class T> inline	T	Iterator<T>::Current () const
	{
		Require (not Done ());
		RequireNotNil (fIterator);
		return (fIterator->Current ());
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
	 *		We must include our .cc file here because of this limitation.
	 *	But, under some systems (notably UNIX) we cannot compile some parts
	 *	of our .cc file from the .hh.
	 *
	 *		The other problem is that there is sometimes some untemplated code
	 *	in the .cc file, and to detect this fact, we define another magic
	 *	flag which is looked for in those files.
	 */
	#define qIncludingIteratorCC	1
		#include	"../Sources/Iterator.cc"
	#undef  qIncludingIteratorCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Iterator__*/

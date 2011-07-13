/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Collection__
#define	__Collection__

/*
 * $Header: /fuji/lewis/RCS/Collection.hh,v 1.19 1992/10/30 22:37:35 lewis Exp $
 *
 * Description:
 *
 *		The Collection class is originally based on SmallTalk-80, The Language &
 *	Its Implementation, page 133 (apx).
 *
 *		"A collection represents a group of objects.  These objects are colled
 *	elements of the collection..."
 *
 *		According to the Smalltalk documentation, "Collections support four
 *	categories of messages for accessing elements:
 *		->	messages for adding new elements
 *		->	messages for removing elements
 *		->	messages for testing occurences of elements
 *		->	messages for enumerating elements.
 *
 *		The lack of lambda expressions, or passable code blocks prohibits
 *	us from making much use of the latter two categories of functionalities,
 *	so we replace it with our own for-loop based iterator classes.
 *
 *		Another deviation from the SmallTalk standard is related to the fact
 *	that in SmallTalk, they assume everything is an object, and can be
 *	treated uniformly.  So collections contain OBJECTS.  In C++, we choose
 *	not to so restrict ourselves, and therefore use an approximation of 
 *	Stroustrups Paramaterized types facility, or real templates where available.
 *
 *		We omit some of the virtual methods which are esoteric, and unlikely
 *	to be used much, since they would GREATLY increase code size, due to
 *	the inability of the linkers to eliminate dead code which appears
 *	in vTables.
 *
 *		TO BE WORKED OUT, is how we will deal with copying subcollections-
 *	either methods to extract subcollections, or with overloaded constructors...
 *
 *
 *		REQUIREMENTS ON TYPE<T>:
 *			->	Since we must compare elements for equality in  Contains (), we require 
 *				the definition of operator== (const T&, const T&).  For consistency sake,
 *				you should probably also define operator!=.
 *
 *			->	Since we must copy these guys, an operator=, and an X(const X&)
 *				must be defined.
 *
 *
 * NOTEWORTHY DESIGN DECISION:
 *			I use type T as argument to these guys (e.g. Add) rather than
 *		const T&. The advantage of const T& is that in many cases (though not
 *		all) it is more efficient that T.  These cases are principly when the
 *		sizeof (T) > sizeof (void*).  For cases where the opposite is true, it
 *		is somewhat less efficent.  Perhaps I should allow for this to be
 *		parameterized, but I don't think C++ templates make this possible.
 *
 *			Given that this choice cannot be parameterized, there is one
 *		overpowering reason to choose this approach, and that is that
 *		C++ 2.0 does not deal very nicely with references to pointers.  This
 *		has to do with how type promotion works in C++ with pointers to objects.
 *		EG, if you have a list of View*, and assign a Window* (where Window
 *		derives from View), this operation might involve a subtraction of the
 *		pointer value as part of its implementation.  If we called
 *		Insert (const View*& v), with a Window*, its not clear how this could
 *		be well accompished in general.  Creating a temporary might work but
 *		would be expensive, and the current crop of compilers does not do
 *		this (2.0b?). This whole issue should be revisited, but we probably
 *		need to just wait for parameterized types...
 *
 *			Decided against using Updated() method to notify subclasses on
 *		changes to collection since approach seemed unweiledly (subclassing)
 *		since not enuf info provided by Updated() call and too much overhead
 *		to justify something that has yet to prove adequate.  For now dont
 *		support subclassing collection to implement things like PickList.
 *		Not sure about polygon.
 *
 *
 * Changes:
 *	$Log: Collection.hh,v $
 *		Revision 1.19  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.18  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.17  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.16  1992/09/11  15:27:57  sterling
 *		used new Shared stuff
 *
 *		Revision 1.15  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.14  1992/09/04  19:52:22  sterling
 *		Made Contains () pure virtual instead of implemented once here.
 *
 *		Revision 1.13  1992/09/03  19:39:00  lewis
 *		Work around gcc 2.2.1 compiler bug by having GetRep () const
 *		return fRep.GetPointer () rather than rep.operator-> since that
 *		made it barf.
 *		Should have done more proplerly with named bug...
 *
 *		Revision 1.11  1992/07/21  05:22:54  lewis
 *		Moved class Container into new file Container.hh - include it.
 *		Added virtual ~CollectionIterator (); Updated macro version to
 *		reflect all the changes made to the templated stuff - I guess I
 *		forgot before. (Not a replace with translation of template version-
 *		do that soon).
 *
 *		Revision 1.10  1992/07/18  15:47:06  lewis
 *		Add TODO suggestions about Patching speed/PatchMany, etc.
 *
 *		Revision 1.8  1992/07/18  15:30:30  lewis
 *		Fix typo - add to TODO, and clenaup.
 *
 *		Revision 1.7  1992/07/17  18:22:00  lewis
 *		Added kdj's performance hack to iterators suggestions to the TODO
 *		list, and added GetCollection () methods to indirect iterators.
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.6  1992/07/17  03:48:11  lewis
 *		Got rid of class Mutator, and consider non-const CollectionIterator to
 *		be a Mutator. Very careful line by line analysis, and bugfixing, and
 *		implemented and tested (compiling) the macro version under GCC.
 *
 *		Revision 1.5  1992/07/16  07:47:25  lewis
 *		Change magic include at end to be depend on "qRealTemplatesAvailable && !!qTemplatesHasRepository"
 *		instead of qBorlandCPlus.
 *
 *		Revision 1.4  1992/07/08  05:24:11  lewis
 *		Changed file name to include for template .cc stuff to UNIX pathname,
 *		and use PCName mapper to rename to appropriate PC File - cuz we may need
 *		to do this with gcc/aux too.
 *
 *		Revision 1.3  1992/07/08  04:27:26  lewis
 *		Fixed a number of my own bugs with the templated version, and made
 *		a number of changes that I think are workarounds for BC++ template
 *		bugs but I am sufficiently unsure of my definitions, that I am just
 *		trying to get things to work - for now.
 *
 *		Revision 1.2  1992/06/25  02:52:01  lewis
 *		Fix templates bugs, and move CollectionItemKey out of scope of template 
 *		to work around bug with gcc 2.2.1 templates.
 *
 *		Revision 1.24  1992/05/21  17:22:57  lewis
 *		Whole raft of changes to the template version. Including added
 *		CollectionView, CollectionMutator, and appropraite changes to
 *		Collection. Added methods to it to keep list of CollectionViews,
 *		and to patch on adds/removes etc.
 *
 *		Revision 1.23  92/05/19  11:49:24  11:49:24  lewis (Lewis Pringle)
 *		Implemented iostream inserters/extractors for Collection. Template
 *		ones just fine, but had to comment out macro extractor because it
 *		uses a temporary of type T, with a no-arg CTOR which may not always
 *		be available. This is fine with templates where the template is only
 *		expanded when used, but unaccpeptable for macros when its always
 *		defined. Partly for this reason, I also put the macro versions in
 *		a separate macro.
 *		
 *		Revision 1.22  92/05/13  00:29:32  00:29:32  lewis (Lewis Pringle)
 *		Lots of cleanups and commenting - especaily cleanup Templates - since
 *		thats all we hope to support soon. Start implementing Mutator stuff.
 *		Fix bug in op== for Collections - my prefivous hacks to workaround
 *		compiler bugs broke things - fixed now with both tmeplates and macros.
 *		Moved inlines out of struct down in Inline section where they belong.
 *		
 *		Revision 1.21  92/05/09  00:35:47  00:35:47  lewis (Lewis Pringle)
 *		Ported to BC++ and fixed templates stuff.
 *		
 *		Revision 1.20  92/04/14  07:31:58  07:31:58  lewis (Lewis Pringle)
 *		Added comment about qSoleInlineVirtualsNotStripped as why we added
 *		out of line virtual dtor.
 *		
 *		Revision 1.19  92/04/13  21:15:21  21:15:21  lewis (Lewis Pringle)
 *		Made Collection(T), and CollectionIterator(T) DTORs out-of-line when
 *		defined from within a macro, since under UNIX (at least HPs) the
 *		compiler/linker generates 1 dtor and probably vtable per .o file
 *		compiled, and these are all included redundently in the final
 *		executable. This makes things MUCH bigger. I am hopping we dont need
 *		to do this kind of crap with the template version, since hopefully
 *		any system sophisticated enuf to support templates, and strip these
 *		redundent defintiions too. If not, its easier to development-environment
 *		by development-environment, make these decisions wiht ifdefs in the
 *		template version.
 *		
 *		Revision 1.18  92/03/30  13:37:16  13:37:16  lewis (Lewis Pringle)
 *		Fixup some syntax errors with templates.
 *		
 *		Revision 1.17  92/03/28  06:11:53  06:11:53  lewis (Lewis Pringle)
 *		Slightly better guess at template syntax.
 *		
 *		Revision 1.16  1992/03/28  03:25:28  lewis
 *		First cut template for Collection.
 *
 *		Revision 1.14  1992/02/21  21:01:20  lewis
 *		Use macro INHERRIT_FROM_SINGLE_OBJECT instead of class SingleObject so
 *		that crap doesnt appear in non-mac inheritance graphs.
 *
 *		Revision 1.13  1992/02/21  18:20:53  lewis
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *		Added comment that we had worked around bug qMPW_C_COMPILER_REGISTER_OVERFLOW_BUG
 *		and added to config.hh.
 *
 *		Revision 1.12  1992/01/25  06:47:52  lewis
 *		Hack to hack over MPW compiler bug workaruond cuz it made snake CPP
 *		hang. I just love stable development tools :-).
 *		Hopefully apple will relase a compile without these register-lockup
 *		bugs, and we can get rid of this crap!
 *
 *		Revision 1.11  1992/01/25  02:23:36  lewis
 *		Work around mac compiler bug with register overflow spillage... in
 *		op== for Collections.
 *
 *		Revision 1.10  1992/01/22  06:47:06  lewis
 *		Use not (a=b) instead of a!=b because we only advertise the need
 *		for a == method (in implementation of op==(Collection,Collection).
 *
 *		Revision 1.9  1992/01/22  06:05:16  lewis
 *		Implemeted ==, != for Collections.
 *
 *		Revision 1.8  1992/01/22  05:24:00  lewis
 *		Get rid of const in front of T as arg to most methods (relic from
 *		const T& days).
 *
 *		Revision 1.7  1992/01/21  20:52:13  lewis
 *		Get rid of it.More(), and it++ - I kinda liked 'em but the ForEach()
 *		macro has won out, and its just confusing and wasteful to have
 *		multiple interfaces.
 *
 *
 */

#include	"BlockAllocated.hh"
#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"




/*
 *		CollectionItemKey is an opaque type whose interpretations varies among
 *	subclasses of Collection. It could plausibly be an index, or a pointer to
 *	an element. It is a private, implemenation detail of Collections, for the
 *	purpose of patching CollectionViews when their concrete Collection changes.
 */
typedef	void*	CollectionItemKey;

class	CollectionRep {
	protected:
		CollectionRep ()	: fViews (Nil)	{}

	public:
		/*
		 *	Iterate over CollectionViews and call their patch method. Called from
		 *	subclasses of Collection<T> when the list is changed. 
		 */
		nonvirtual	void	PatchViewsAdd (CollectionItemKey key) const;
		nonvirtual	void	PatchViewsRemove (CollectionItemKey key) const;
		nonvirtual	void	PatchViewsRemoveAll () const;

	private:
		class CollectionView*	fViews;

	friend	CollectionView;		// patches fViews
};

class CollectionView {
	protected:
		CollectionView (const CollectionRep& owner);
	
	public:
		virtual	~CollectionView ();
	
		/*
		 *		Called when the underlying Collection has been modified.
		 */
		virtual	void	PatchAdd (CollectionItemKey key)     = Nil;	// called after the addition
		virtual	void	PatchRemove (CollectionItemKey key)  = Nil;	// called before the removal
		virtual	void	PatchRemoveAll ()  					 = Nil;	// called before the removals

// hack - made public to make cloning easier - OK for now since this class will soon
// go away...
// LGP	Thursday, October 29, 1992 10:34:20 PM
		const CollectionRep&	fOwner;
	private:
		CollectionView*			fNext;

	friend	CollectionRep;	// peeks at fNext
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Collection__*/

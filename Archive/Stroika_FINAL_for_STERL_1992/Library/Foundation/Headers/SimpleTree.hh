/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SimpleTree__
#define	__SimpleTree__

/*
 * $Header: /fuji/lewis/RCS/SimpleTree.hh,v 1.17 1992/12/03 19:17:50 lewis Exp $
 *
 * Description:
 *
 *
 *
 * TODO:
 *		+		
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: SimpleTree.hh,v $
 *		Revision 1.17  1992/12/03  19:17:50  lewis
 *		*** empty log message ***
 *
 *		Revision 1.16  1992/12/03  07:27:44  lewis
 *		Added fSupressMore field and support More() instead of Next().
 *
 *		Revision 1.15  1992/11/17  05:29:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.14  1992/11/13  03:37:10  lewis
 *		Predeclare templates before using names as friends.
 *
 *		Revision 1.13  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.12  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.11  1992/10/15  20:21:23  lewis
 *		Lots of cleanups, including moving stuff from headers to sources,
 *		and getting to compile with GenClass.
 *		(also passed its testsuite - though that doesn mean much yet).
 *
 *		Revision 1.10  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.9  1992/10/07  23:04:04  sterling
 *		Fixed a bunch of minor typos in template version, and used new
 *		support in GenClass to hopefulyl get dependency on LinkList working
 *
 *		Revision 1.8  1992/10/02  21:45:09  lewis
 *		Tried to get working with GenClass but ran into trouble
 *		with LinkList(NodePtr) - must think out better how to deal with
 *		this. Fixed some other GenClass type problems.
 *
 *		Revision 1.7  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.6  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.5  1992/09/20  01:24:45  lewis
 *		Converted to using < instead of <=.
 *		Fixed any bugs with addition/removal (they all pass testsuites
 *		now - at least so far). Implemented forward iterator. This was
 *		trickier than I expected. Backwards should be a simple cloning,
 *		and use not (<) where I used not, and LHS<->RHS swapped.
 *
 *		Revision 1.4  1992/09/18  03:14:36  lewis
 *		Got enuf working to get testsuite working - I think all workable except for
 *		iteration - not started yet.
 *
 *		Revision 1.3  1992/09/16  21:58:43  lewis
 *		Worked a bit more on implemenation. Close to being able to test.
 *
 *
 */

#include	<stdlib.h>

#include	"Debug.hh"
#include	"LinkList.hh"



#if		qRealTemplatesAvailable

template	<class	T>	class	SimpleTree;
template	<class	T>	class	SimpleTreeIterator;

// SimpleTreeNode<T> should be scoped inside SimpleTree, but nested classes with templates
// fails on every compiler I know of - someday... LGP Thursday, October 15, 1992 2:54:14 PM
template	<class	T>	class	SimpleTreeNode {
	public:
		SimpleTreeNode (T item, SimpleTreeNode<T>* lhs, SimpleTreeNode<T>* rhs);
		~SimpleTreeNode ();

	private:
		T					fItem;
		SimpleTreeNode<T>*	fLHS;
		SimpleTreeNode<T>*	fRHS;

	friend	class	SimpleTree<T>;
	friend	class	SimpleTreeIterator<T>;
};

template	<class	T>	class	SimpleTree {
	public:
		SimpleTree ();
		~SimpleTree ();

		nonvirtual	SimpleTree<T>& operator= (const SimpleTree<T>& rhs);

		nonvirtual	size_t	GetLength () const;

		nonvirtual	Boolean	Contains (T item);
		nonvirtual	void	Add (T item);
		nonvirtual	void	Remove (T item);
		nonvirtual	void	RemoveAll ();

	private:
		size_t				fLength;			// #items in tree
		SimpleTreeNode<T>*	fRoot;

		nonvirtual	size_t	Depth (SimpleTreeNode<T>* from = Nil);
	friend	class	SimpleTreeIterator<T>;
};

#if		!qRealTemplatesAvailable
	template	<class	T>	typedef	SimpleTreeNode<T>*	SimpleTreeNodePtr<T>;
	#include	"TFileMap.hh"
	#include	LinkListOfSimpleTreeNodePtrOf<T>_hh
#endif

template	<class	T>	class	SimpleTreeIterator {
	public:
		SimpleTreeIterator (SimpleTree<T>& tree);
		~SimpleTreeIterator ();

		nonvirtual	Boolean	Done () const;
		nonvirtual	Boolean	More ();
		nonvirtual	T		Current () const;

	private:
		nonvirtual	void	PatchAdd ();
		nonvirtual	void	PatchRemove ();
		nonvirtual	void	PatchRemoveAll ();

	private:
		SimpleTree<T>&					fTree;
		SimpleTreeNode<T>*				fCurNode;
#if		!qRealTemplatesAvailable
		LinkList<SimpleTreeNodePtr<T> >	fParents;
#else
		LinkList<SimpleTreeNode<T>*>	fParents;
#endif
		Boolean							fSupressMore;
	friend	class	SimpleTree<T>;
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

	// class SimpleTreeNode<T>
	template	<class	T>	inline	SimpleTreeNode<T>::SimpleTreeNode (T item, SimpleTreeNode<T>* lhs, SimpleTreeNode<T>* rhs) :
		fItem (item),
		fLHS (lhs),
		fRHS (rhs)
	{
	}
	template	<class	T>	inline	SimpleTreeNode<T>::~SimpleTreeNode ()
	{
		delete (fRHS);
		delete (fLHS);
	}

	// class SimpleTree<T>
	template	<class	T>	inline	size_t	SimpleTree<T>::GetLength () const
	{
		return (fLength);
	}

	// class SimpleTreeIterator<T>
	template	<class	T>	inline	SimpleTreeIterator<T>::~SimpleTreeIterator ()
	{
	}
	template	<class	T>	inline	Boolean	SimpleTreeIterator<T>::Done () const
	{
		return (Boolean (fCurNode == Nil));
	}
	template	<class	T>	inline	T	SimpleTreeIterator<T>::Current () const
	{
		Require (not Done ());
		AssertNotNil (fCurNode);
		return (fCurNode->fItem);
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
	#define	qIncluding_SimpleTreeCC	1
		#include	"../Sources/SimpleTree.cc"
	#undef	qIncluding_SimpleTreeCC
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__SimpleTree__*/


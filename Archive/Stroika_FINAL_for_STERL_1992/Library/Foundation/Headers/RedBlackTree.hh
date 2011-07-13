/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__RedBlackTree__
#define	__RedBlackTree__

/*
 * $Header: /fuji/lewis/RCS/RedBlackTree.hh,v 1.3 1992/09/25 01:28:14 lewis Exp $
 *
 * Description:
 *
 *
 *
 * TODO:
 *		Clone SimpleTree to get this started, and then merge back in the
 *		rotate stuff, etc...
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: RedBlackTree.hh,v $
 *		Revision 1.3  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.2  1992/09/08  15:00:16  lewis
 *		Renamed NULL -> Nil.
 *
 *
 *
 */

#include	<stdlib.h>

#include	"Debug.hh"
#include	"Iterator.hh"





#if		qRealTemplatesAvailable && 0

// This should be scoped...
template	<class	T>	class	TreeNode {
	public:
		TreeNode (T item, TreeNode<T>* lhs, TreeNode<T>* rhs):
						fItem (item), fLHS (lhs), fRHS (rhs)	{}
		~TreeNode ()	{}


		size_t GetDepth () const;// recursively find out how deep this tree is
								// assume balanced?

	private:
		T				fItem;
		TreeNode<T>*	fLHS;
		TreeNode<T>*	fRHS;
	friend	class	RedBlackTree<T>;
};

template	<class	T>	class	RedBlackTree {
	public:
		RedBlackTree ();
		~RedBlackTree ();

		RedBlackTree<T>& operator= (const RedBlackTree<T>& rhs);

		nonvirtual	T		GetAt (size_t i) const;
		nonvirtual	void	SetAt (T item, size_t i);
		nonvirtual	T&		operator[] (size_t i);

		nonvirtual	size_t	GetLength () const				{	return (fLength);	}

		nonvirtual	void	InsertAt (T item, size_t index);
		nonvirtual	void	RemoveAt (size_t index);
		nonvirtual	void	RemoveAll ();


		nonvirtual	void	Insert (T item);
	private:
		size_t			fLength;			// #items in tree
		TreeNode<T>*	fRoot;


		nonvirtual	void			Split (T item);
		nonvirtual	TreeNode<T>*	Rotate (T item, TreeNode<T>* y);
};

template	<class T> void	RedBlackTree<T>::Insert (T item)
{
	TreeNode<T>*	x	=	fRoot;
	TreeNode<T>*	g	=	fRoot;
	TreeNode<T>*	p	=	fRoot;
	TreeNode<T>*	gg	=	Nil;

	while (x != Nil) {
		gg = g;
		g = p;
		p = x;
		x = (item < x->key)? x->fLHS: x->fRHS;
		if (x->fLHS->fIsRed and x->fRHS->fIsRed) {
			Split (item, x, gg, g, p);
		}
	}
	x = new TreeNode<T> (item, 1, Nil, Nil);
	if (item < p->key) {
		p->fLHS = x;
	}
	else {
		p->fRHS = x;
	}
	Split (item, x, gg, g, p);
	head->fRHS->fIsRed = False;
}

template	<class T> void	RedBlackTree<T>::Split (T item, TreeNode<T>* x, TreeNode<T>* gg, TreeNode<T>* g, TreeNode<T>* p)
{
	x->fIsRed = True;
	x->fLHS->fIsRed = False;
	x->fRHS->fIsRed = False;
	if (p->fIsRed) {
		g->fIsRed = True;
		if ((item < g->key) != (item < p->key)) {
			p = Rotate (item, g);
			x = Rotate (item, gg);
			x->fIsRed = False;
		}
	}
}

template	<class T> TreeNode<T>*	RedBlackTree<T>::Rotate (T item, TreeNode<T>* y)
{
	TreeNode<T>*	c		=	(item < y->key)? y->fLHS: y->fRHS;
	Boolean			less	=	(item < c->key);
	TreeNode<T>*	gc		=	(less? c->fLHS: c->fRHS);
	if (less) {
		c->fLHS = gc->fRHS;
		gc->fRHS = c;
	}
	else {
		c->fRHS = gc->fLHS;
		gc->fLHS = c;
	}
	if (item < y->key) {
		y->fLHS = gc;
	}
	else {
		y->fRHS = gc;
	}
	return (gc);
}

#endif	/*qRealTemplatesAvailable && 0*/





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__RedBlackTree__*/


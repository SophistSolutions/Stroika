/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__SimpleTree__cc
#define	__SimpleTree__cc

/*
 * $Header: /fuji/lewis/RCS/SimpleTree.cc,v 1.12 1992/12/03 19:19:59 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SimpleTree.cc,v $
 *		Revision 1.12  1992/12/03  19:19:59  lewis
 *		it.More() should return not Done() - not Done().
 *
 *		Revision 1.11  1992/12/03  05:56:44  lewis
 *		Use new fSupressMore stuff and support new it.More() interface.
 *		Also, react to changes in LinkList for iterator.
 *
 *		Revision 1.10  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.7  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.6  1992/10/15  20:21:23  lewis
 *		Lots of cleanups, including moving stuff from headers to sources,
 *		and getting to compile with GenClass.
 *		(also passed its testsuite - though that doesn mean much yet).
 *
 *		Revision 1.5  1992/10/15  02:23:43  lewis
 *		Change use of != to not (==).
 *
 *		Revision 1.4  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.3  1992/09/20  01:24:45  lewis
 *		Converted to using < instead of <=.
 *		Fixed any bugs with addition/removal (they all pass testsuites
 *		now - at least so far). Implemented forward iterator. This was
 *		trickier than I expected. Backwards should be a simple cloning,
 *		and use not (<) where I used not, and LHS<->RHS swapped.
 *
 *		Revision 1.2  1992/09/18  03:14:36  lewis
 *		Got enuf working to get testsuite working - I think all workable except for
 *		iteration - not started yet.
 *
 *		
 *
 */



#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Memory.hh"

#include	"SimpleTree.hh"





#if		qRealTemplatesAvailable


#if		!qRealTemplatesAvailable
		#include	"TFileMap.hh"
		#include	LinkListOfSimpleTreeNodePtrOf<T>_cc
#endif


/*
 ********************************************************************************
 ********************************** SimpleTree **********************************
 ********************************************************************************
 */
template	<class	T>	class	SimpleTree;	// Tmp hack so GenClass will fixup following CTOR/DTORs
											// Harmless, but silly.

template	<class T> SimpleTree<T>::SimpleTree ():
	fLength (0),
	fRoot (Nil)
{
}

template	<class T> SimpleTree<T>::~SimpleTree ()
{
	delete (fRoot);
}

template	<class T> Boolean	SimpleTree<T>::Contains (T item)
{
	/*
	 * Search from the root, down to the node which is equal to item.
	 */
	for (register SimpleTreeNode<T>* cur = fRoot;
		 cur != Nil;
		 cur = ((item < cur->fItem)? cur->fLHS: cur->fRHS)) {
		if (item == cur->fItem) {
			return (True);
		}
	}
	return (False);
}

template	<class T> void	SimpleTree<T>::Add (T item)
{
	/*
	 * From Sedgewick p205-6.
	 * 		"To insert a node into a tree, we do an unsucessful search for it,
	 * then attach it in place of [Nil] at the point where the unsuccessful
	 * search terminated. To do the insertion, the following code keeps track
	 * of the the parent [parent] of [cur] as it procedes down the tree."
	 *		...
	 *		"In this implementation, when a new node whose key is equal to
	 * some key already in the tree is inserted, it will be inserted to the
	 * right of the node already in the tree. This means that nodes with
	 * equal keys can be found by simply continuing the search from the point
	 * where [this procedure] terminates."
	 *
	 *		One thing that we do differently from Sedgewick is that we do
	 * not use a sentinal value, so we must be careful of the case where we
	 * must update root!
	 */
	register SimpleTreeNode<T>* parent = Nil;
	for (register SimpleTreeNode<T>* cur = fRoot;
		 cur != Nil;
		 parent = cur, cur = ((item < cur->fItem)? cur->fLHS: cur->fRHS))
		;

	if (parent == Nil) {
		Assert (fRoot == Nil);
		Assert (fLength == 0);
		fRoot = new SimpleTreeNode<T> (item, Nil, Nil);
	}
	else {
		AssertNotNil (fRoot);
		Assert (fLength != 0);
		/*
		 * Here we have parent pointing to a NON-ROOT node which has at MOST one
		 * child, and in fact NONE on the side in which we must insert
		 * (this is gauranteed by the way we searched in the for loop above).
		 * So just insert new leaf node, in our already empty slot of parent.
		 */
		if (item < parent->fItem) {
			Assert (parent->fLHS == Nil);
			parent->fLHS = new SimpleTreeNode<T> (item, Nil, Nil);
		}
		else {
			Assert (parent->fRHS == Nil);
			parent->fRHS = new SimpleTreeNode<T> (item, Nil, Nil);
		}
	}
	fLength++;
}

template	<class T> void	SimpleTree<T>::Remove (T item)
{
	/*
	 * From Sedgewick p210-1.
	 * 		"The program first searches the tree in the normal way to get
	 * the location of [itemsNode] in the tree (Actually the main purpose
	 * of this is to set [parent], so that another node can be linked in
	 * after [itemsNode] is gone.) Next, the program checks three cases:
	 * if [itemsNode] has no right child, then the child of [parent] after
	 * the deletion will be the left child of [itemsNode][sic]; if [itemsNode]
	 * has a right child with no left child then that right child will be the
	 * left child of [parent] after the deletion, with its left link copied from
	 * [itemsNode] [sic]; otherwise [itemsNode] is set to the node with the smallest
	 * key in the subtree to the right of [itemsNode]; that node's right
	 * link is copied to the link link of its parent, and both of its links
	 * are set from [itemsNode][sic]."
	 *	...
	 *		"the algorithm above can tend to leave a tree slightly unbalanced
	 * (average height of [sqrt(n)]) if subjected to a very large number of
	 * random delete-insert pairs."
	 *
	 *		Again we do not use a sentinal value, so we must be careful of
	 * the case where we must update root!
	 */
	register SimpleTreeNode<T>* parent = Nil;
	for (register SimpleTreeNode<T>* itemsNode = fRoot;
		 not (itemsNode->fItem == item);
		 parent = itemsNode, itemsNode = ((item < itemsNode->fItem)? itemsNode->fLHS: itemsNode->fRHS)) {
		Require (itemsNode != Nil);	// Would mean node node found!!!
	}
	RequireNotNil (itemsNode);	// item must be found... usage error, otherwise
	Assert (item == itemsNode->fItem);
	Assert ((parent != Nil) or (fRoot == itemsNode));										// item could be root of tree
	Assert ((parent == Nil) or ((item < parent->fItem) == (parent->fLHS == itemsNode)));	// item on correct side of parent


	SimpleTreeNode<T>*	deleteMe	=	itemsNode;
	if (deleteMe->fRHS == Nil) {
		itemsNode = itemsNode->fLHS;
	}
	else if (deleteMe->fRHS->fLHS == Nil) {
		itemsNode = itemsNode->fRHS;
		itemsNode->fLHS = deleteMe->fLHS;
	}
	else {
		AssertNotNil (deleteMe->fRHS->fLHS);
		for (SimpleTreeNode<T>* c = itemsNode->fRHS;
			 c->fLHS->fLHS != Nil;
			 c = c->fLHS)
			;
		itemsNode = c->fLHS;
		c->fLHS = itemsNode->fRHS;
		itemsNode->fLHS = deleteMe->fLHS;
		itemsNode->fRHS = deleteMe->fRHS;
	}

	// Set left/right ptrs to nil so children not deleted, then delete node...
	deleteMe->fLHS = Nil;
	deleteMe->fRHS = Nil;
	delete (deleteMe);

	if (parent == Nil) {
		fRoot = itemsNode;
	}
	else {
		if (item < parent->fItem) {
			parent->fLHS = itemsNode;
		}
		else {
			parent->fRHS = itemsNode;
		}
	}

	fLength--;
	Ensure ((fRoot == Nil) == (fLength == 0));
}

template	<class T> void	SimpleTree<T>::RemoveAll ()
{
	/*
	 * This works since we make the ~SimpleTreeNode<T> () delete its children.
	 * This is a complex routine without recursion, as we need to maintain
	 * a seperate stack. We could introduce a seperate private utility routine
	 * to do the recursion, or just take advantage of the DTOR. This just
	 * seemed easier- its all encapsulated privately, however, so it can
	 * be changed easily.
	 */
	delete (fRoot);
	fRoot = Nil;
	fLength = 0;
}

template	<class T> size_t	SimpleTree<T>::Depth (SimpleTreeNode<T>* from)
{
	if (from == Nil) {
		from = fRoot;
	}
	if (from == Nil) { return 0; };
	return (1 + Max ((from->fLHS==Nil)? 0: Depth (from->fLHS), (from->fRHS==Nil)? 0: Depth (from->fRHS)));
}




/*
 ********************************************************************************
 ********************************** SimpleTreeIterator **************************
 ********************************************************************************
 */
template	<class	T>	class	SimpleTreeIterator;	// Tmp hack so GenClass will fixup following CTOR/DTORs
													// Harmless, but silly.

template	<class	T>	SimpleTreeIterator<T>::SimpleTreeIterator (SimpleTree<T>& tree):
	fTree (tree),
	fCurNode (tree.fRoot),
	fParents (),
	fSupressMore (True)
{
	/*
	 * Position myself on the left-most node (IE FIRST).
	 */
	if (fCurNode != Nil) {
		while (fCurNode->fLHS != Nil) {
			fParents.Prepend (fCurNode);
			fCurNode = fCurNode->fLHS;
		}
	}
}

template	<class	T>	Boolean	SimpleTreeIterator<T>::More ()
{
	if (fSupressMore) {
		fSupressMore = False;
	}
	else {
		RequireNotNil (fCurNode);	// otherwise were done!
		/*
		 * We want to implement an in-order traversal. This is usually implemented
		 * recursively as traverse-left, visit, traverse-right. But here, we must
		 * not use recursion, and further, our algorithm must save state, and terminate
		 * after each visit (thats just how the interface to iterators works). So,
		 * we maintain an auxiliary stack for our parent nodes.
		 */
		SimpleTreeNode<T>*	lastNode = fCurNode;
		if (fCurNode->fRHS != Nil) {
			fParents.Prepend (fCurNode);
			fCurNode = fCurNode->fRHS;
			while (fCurNode->fLHS != Nil) {
				fParents.Prepend (fCurNode);
				fCurNode = fCurNode->fLHS;
			}
		}
		/*
		 * 		If all this has failed to move us, we have finished either
		 * a left subtree or a right subtree (either the traverse-left or
		 * traverse-right recursion in the simple recursive case). We
		 * can tell which by comparing the old node with the LHS/RHS of
		 * the node we are popping back to.
		 *
		 *		Then, if we finished the left subtree, pop once, and we
		 * are at the right point for a visit. If we finsihed a right-subtree
		 * then we want to keep popping.
		 */
		if (lastNode == fCurNode) {
			for (; fParents.GetLength () != 0;) {
				SimpleTreeNode<T>*	newNode = fParents.GetFirst ();
				fParents.RemoveFirst ();
				AssertNotNil (newNode);
				if (newNode->fLHS == fCurNode) {
					/*
					 * Then we finsihed a traverse left, and we want to visit this
					 * node.
					 */
					fCurNode = newNode;
					break;
				}
				else {
					/*
					 * Then we finsihed a traverse right, and we want to keep popping
					 * the stack. We update fCurNode each time so that the above
					 * if test properly terminates.
					 */
					Assert (newNode->fRHS == fCurNode);
					if (fParents.GetLength () == 0) {
						fCurNode = Nil;	// finished rhs recursion so were all done...
					}
					else {
						fCurNode = newNode;
					}
				}
			}
		}
		/*
		 * Could still happen if we have a tree of size 1.
		 */
		if (lastNode == fCurNode) {
			fCurNode = Nil;
		}
	}
	Assert (not fSupressMore);
	return (not Done ());
}


#endif	/*qRealTemplatesAvailable*/


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


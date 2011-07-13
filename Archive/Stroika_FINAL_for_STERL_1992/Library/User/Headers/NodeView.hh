/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/NodeView.hh,v 1.7 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 * The purpose of NodeView is to display acyclic directed graphs in a visually compelling mannor.
 * The class automatically handles the layout of nodes, and draws the arc lines between nodes. Graphs
 * can be displayed either vertically or horizontally.
 * 
 * NodeView is designed to display any existing View classes as leaf nodes. This allows for great
 * flexibility in how particular leaf nodes appear. For example, one can mix any number of different
 * views together in a NodeView, and it will automatically lay them out in a directed graph. In order
 * to work correctly with NodeView, a view should
 * 	1) implement some kind of drawing behavior in its Draw method
 * 	2) calculate a reasonable size in its CalcDefaultSize method
 * 
 * Graphs are constructed by creating a heirarchy of NodeViews. All non-leaf nodes in the graph, shoul
 * be NodeViews, while no leaf nodes should be NodeViews. Nodes (leaf or otherwise) are added
 * to a NodeView by calling AddSubView.
 * 
 * Some graphical attributes of NodeViews are parameterized. More sweeping changes should be done
 * by subclassing, and overriding either the Draw or the DrawArc method. The parameterized attributes
 * are:
 * 	Direction:  whether the graph is layed out left to right or top to bottom
 * 	Spacing:	the gap left between two nodes
 * 	Arc Length:	the length of the arc lines connecting nodes
 * 
 * A final parameterized attribute is expandability. If a NodeView is expandable it responds to
 * "Open" mouse clicks by either hiding or showing its subnodes. When contracted, a large circle 
 * is displayed to guide the user as to where to click to re-expand.	
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: NodeView.hh,v $
 *		Revision 1.7  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  21:02:57  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/02  05:09:34  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.9  1992/04/06  14:51:08  sterling
 *		major surgery
 *
 *
 *
 */


#ifndef	__NodeView__
#define	__NodeView__

#include	"Sequence.hh"

#include	"View.hh"


#if		!qRealTemplatesAvailable
	typedef	class	NodeView*	NodeViewPtr;
	Declare (Iterator, NodeViewPtr);
	Declare (Collection, NodeViewPtr);
	Declare (AbSequence, NodeViewPtr);
#endif

class	NodeView : public View {
	public:
		static	Point	kDefaultSpacing;
		static	Point	kDefaultArcLength;
	
	protected:
		NodeView ();
	
	public:
		virtual	Iterator(NodeViewPtr)*	MakeNodeIterator (SequenceDirection d = eSequenceForward)	= Nil;
		virtual	CollectionSize	CountNodes () const	= Nil;	
};

// subclass this to make inexpensive nodes that must be leaves
class	LeafNode : public NodeView {
	protected:
		LeafNode ();
	
	public:
		override	Iterator(NodeViewPtr)*	MakeNodeIterator (SequenceDirection d = eSequenceForward);
		override	CollectionSize	CountNodes () const;	
};

// should be scoped within LeadNode
// always returns that the iteration is complete
class	LeafNodeIterator : public Iterator(NodeViewPtr) {
	public:
		LeafNodeIterator ();
		
		override	Boolean			Done () const;
		override	void			Next ();
		override	NodeViewPtr		Current () const;
		
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);

	private:		
		static	LeafNodeIterator*	sThis;
};

// subclass this for branch nodes, typically a subclass will add public access for adding nodes,
// but will keep more accurate type information than is generically possible
class	BranchNode : public NodeView {
	protected:
		BranchNode ();
		
	public:
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	void	Draw (const Region& update);
	
		// gap that should be left between subnodes
		nonvirtual	Point	GetSpacing () const;
		nonvirtual	void	SetSpacing (const Point& spacing, UpdateMode update = eDelayedUpdate);

		// length or arc lines that connect nodes
		nonvirtual	Point	GetArcLength () const;
		nonvirtual	void	SetArcLength (const Point& arcLength, UpdateMode update = eDelayedUpdate);
		
		// an expanded NodeView display all of its subnodes
		nonvirtual	Boolean	GetExpanded () const;
		nonvirtual	void	SetExpanded (Boolean expanded, UpdateMode update = eDelayedUpdate); 

		// an expandable NodeView responds to mouse open clicks by expanding or contracting
		nonvirtual	Boolean	GetExpandable () const;
		nonvirtual	void	SetExpandable (Boolean expandable); 
	
		nonvirtual	Point::Direction	GetDirection () const;
		nonvirtual	void				SetDirection (Point::Direction direction, UpdateMode update = eDelayedUpdate);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		virtual		Point	CalcNodeAnchor (NodeView& node);					// find end point of arc
		virtual		void	DrawArc (const Point& from, const Point& to);	

		virtual	void	AddNode (NodeView* node, CollectionSize index = 1);
		virtual	void	ReorderNode (NodeView* node, CollectionSize index = 1);
		virtual	void	RemoveNode (NodeView* node);

		virtual	Point	GetSpacing_ () const	= Nil;
		virtual	void	SetSpacing_ (const Point& spacing, UpdateMode update)	= Nil;

		virtual	Point	GetArcLength_ () const	= Nil;
		virtual	void	SetArcLength_ (const Point& arcLength, UpdateMode update)	= Nil;
		
		virtual	Boolean	GetExpanded_ () const	= Nil;
		virtual	void	SetExpanded_ (Boolean expanded, UpdateMode update)	= Nil; 

		virtual	Boolean	GetExpandable_ () const	= Nil;
		virtual	void	SetExpandable_ (Boolean expandable)	= Nil;
	
		virtual	Point::Direction	GetDirection_ () const	= Nil;
		virtual	void				SetDirection_ (Point::Direction direction, UpdateMode update)	= Nil;

		virtual	void	Resize (UpdateMode update);
		virtual	Point	LayoutNodes ();
};

// more convenient but potentially more expensive than above, depending on
// the degree of flexibility desired
class SimpleBranch : public BranchNode {
	protected:
		SimpleBranch ();
	
		override	Point	GetSpacing_ () const;
		override	void	SetSpacing_ (const Point& spacing, UpdateMode update);

		override	Point	GetArcLength_ () const;
		override	void	SetArcLength_ (const Point& arcLength, UpdateMode update);
		
		override	Boolean	GetExpanded_ () const;
		override	void	SetExpanded_ (Boolean expanded, UpdateMode update); 

		override	Boolean	GetExpandable_ () const;
		override	void	SetExpandable_ (Boolean expandable); 
	
		override	Point::Direction	GetDirection_ () const;
		override	void				SetDirection_ (Point::Direction direction, UpdateMode update);

	private:
		Point				fSpacing;
		Point				fArcLength;
		Boolean				fExpandable;
		Boolean				fExpanded;
		Point::Direction 	fDirection;
};

/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



#endif	/* __NodeView__ */


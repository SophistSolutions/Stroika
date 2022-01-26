/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DemoNodes__
#define	__DemoNodes__
/*
 * $Header: /fuji/lewis/RCS/DemoNodes.hh,v 1.6 1992/09/08 16:48:46 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: DemoNodes.hh,v $
 *		Revision 1.6  1992/09/08  16:48:46  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:30:27  sterling
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/07/22  01:09:41  lewis
 *		Use Sequence instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *
 *
 */

#include "Sequence.hh"

#include "KeyHandler.hh"
#include "MenuCommandHandler.hh"

#include "NodeView.hh"


typedef	class	DemoLeafNode*	DemoLeafNodePtr;
typedef	class	DemoNode*		DemoNodePtr;
typedef	class	DemoTrunkNode*	DemoTrunkNodePtr;



Declare (Iterator, DemoNodePtr);
Declare (Collection, DemoNodePtr);
Declare	(AbSequence, DemoNodePtr);
Declare	(Array, DemoNodePtr);
Declare	(Sequence_Array, DemoNodePtr);
Declare	(Sequence, DemoNodePtr);

Declare (Iterator, DemoTrunkNodePtr);
Declare (Collection, DemoTrunkNodePtr);
Declare	(AbSequence, DemoTrunkNodePtr);
Declare	(Array, DemoTrunkNodePtr);
Declare	(Sequence_Array, DemoTrunkNodePtr);
Declare	(Sequence, DemoTrunkNodePtr);

Declare (Iterator, DemoLeafNodePtr);


class	NodeViewDocument;
class	DemoTrunkNode;
class	DemoBranchNode;
class	DemoNode : public NodeView {
	protected:
		DemoNode (NodeViewDocument& doc);

	public:
		nonvirtual	NodeViewDocument&	GetDocument () const;

		virtual	Iterator(DemoLeafNodePtr)*	MakeDemoLeafIterator () = Nil;
		nonvirtual	DemoTrunkNode&	GetTrunkNode () const;
		nonvirtual	DemoBranchNode&	GetParentNode () const;
		
	private:
		NodeViewDocument& 	fDocument;
		DemoBranchNode*		fParentNode;
		
		friend	class	DemoBranchNode;	// sets fParentNode
};

class	DemoLeafNode : public DemoNode {
	public:
		DemoLeafNode (const String& title, NodeViewDocument& doc);
		~DemoLeafNode ();
		
		override	SequenceIterator(NodeViewPtr)*	MakeNodeIterator (SequenceDirection traversalDirection = eSequenceForward);
		override	CollectionSize	CountNodes () const;	

		nonvirtual	Boolean	GetSelected () const;
		nonvirtual	void	SetSelected (Boolean selected, UpdateMode update = eDelayedUpdate);
				
		override	Iterator(DemoLeafNodePtr)*	MakeDemoLeafIterator ();

		nonvirtual	String	GetText () const;
		nonvirtual	void	SetText (const String& text, UpdateMode update = eDelayedUpdate);

	protected:
		override	void	Layout ();
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	private:
		class TextView*		fTextView;
		Boolean				fSelected;
};

class	DemoBranchNode : public DemoNode {
	public:
		DemoBranchNode (NodeViewDocument& doc);

		nonvirtual	void	AddNode (DemoNode* node, DemoNode* neighbor = Nil);
		nonvirtual	void	RemoveNode (DemoNode* node);

		override	SequenceIterator(NodeViewPtr)*	MakeNodeIterator (SequenceDirection traversalDirection = eSequenceForward);
		override	CollectionSize	CountNodes () const;
		
		nonvirtual	BranchNode&	GetBranchNode () const;

		override	Iterator(DemoLeafNodePtr)*	MakeDemoLeafIterator ();

	protected:
		override	void	Layout ();
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	private:
		class MyBranchNode*		fBranchNode;
};

class	DemoTrunkNode : public DemoBranchNode, public MenuCommandHandler, public KeyHandler {
	public:
		DemoTrunkNode (NodeViewDocument& doc);
		~DemoTrunkNode ();
		
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);
		
		nonvirtual	void	DeselectAll (UpdateMode update);
		
		nonvirtual	CollectionSize	GetSelectedCount () const;
		nonvirtual	void			SetSelectedCount (CollectionSize count);
		
		nonvirtual	void	DragSelected (const MousePressInfo& mouseInfo);
	
		static	Sequence(DemoTrunkNodePtr)	sTrunkNodes;

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	private:
		CollectionSize	fSelectedCount;
};

#endif	/* __DemoNodes__ */

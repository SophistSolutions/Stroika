/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DemoNodes.cc,v 1.7 1992/09/08 16:48:46 lewis Exp $
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
 *	$Log: DemoNodes.cc,v $
 *		Revision 1.7  1992/09/08  16:48:46  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  17:31:57  sterling
 *		*** empty log message ***
 *
 *		Revision 1.5  1992/07/22  01:09:41  lewis
 *		Use Sequence instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.3  92/04/16  13:04:44  13:04:44  lewis (Lewis Pringle)
 *		Added Implement (Collection, NemoNodePtr) - inadvertantly left out before, and was innocuous cuz there
 *		was nothing in it - but there is now...
 *		
 *
 *
 */
 
#include "Debug.hh"
#include "StreamUtils.hh"
#include "Time.hh"

#include "Shape.hh"

#include "DeskTop.hh"
#include "MenuOwner.hh"
#include "TextView.hh"
#include "Tracker.hh"

#include "DemoNodes.hh"
#include "NodeViewCommands.hh"
#include "NodeViewDocument.hh"
#include "NodeViewWindow.hh"



Implement (Iterator, DemoNodePtr);
Implement (Collection, DemoNodePtr);
Implement	(AbSequence, DemoNodePtr);
Implement	(Array, DemoNodePtr);
Implement	(Sequence_Array, DemoNodePtr);
Implement	(Sequence, DemoNodePtr);

Implement (Iterator, DemoTrunkNodePtr);
Implement (Collection, DemoTrunkNodePtr);
Implement	(AbSequence, DemoTrunkNodePtr);
Implement	(Array, DemoTrunkNodePtr);
Implement	(Sequence_Array, DemoTrunkNodePtr);
Implement	(Sequence, DemoTrunkNodePtr);

Implement (Iterator, DemoLeafNodePtr);


class	MyBranchNode : public BranchNode {
	public:
		MyBranchNode (DemoBranchNode& owner);
	
		override	void	AddNode (DemoNode* node, CollectionSize index = 1);
		override	void	RemoveNode (DemoNode* node);
		
		override	SequenceIterator(NodeViewPtr)*	MakeNodeIterator (SequenceDirection traversalDirection = eSequenceForward);
		override	CollectionSize	CountNodes () const;	

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

		nonvirtual	NodeViewDocument&	GetDocument () const;
		
	private:
		Sequence(DemoNodePtr)	fNodes;
		Boolean					fExpanded;
		DemoBranchNode& 		fOwner;
		
		friend	class DemoBranchIterator;
};

/*
 ********************************************************************************
 ************************************ DemoNode **********************************
 ********************************************************************************
 */
DemoNode::DemoNode (NodeViewDocument& doc) :
	fDocument (doc),
	fParentNode (Nil)
{
}

NodeViewDocument&	DemoNode::GetDocument () const
{
	return (fDocument);
}

DemoTrunkNode&	DemoNode::GetTrunkNode () const
{
	return (GetDocument ().GetNodeWindow ().GetNodeView ());
}

DemoBranchNode&	DemoNode::GetParentNode () const
{
	RequireNotNil (fParentNode);
	return (*fParentNode);
}

/*
 ********************************************************************************
 ************************************ MyTextView ********************************
 ********************************************************************************
 */
class MyTextView : public TextView {
	public:
		MyTextView (const String& text, AbstractTextView::Justification justification, DemoLeafNode& node);
		
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	void	Draw (const Region& update);
		override	Boolean	GetLive () const;

	private:
		DemoLeafNode&	fNode;
};

MyTextView::MyTextView (const String& text, AbstractTextView::Justification justification, DemoLeafNode& node) :
	TextView (text, justification),
	fNode (node)
{
}

Boolean	MyTextView::GetLive () const
{
	return (View::GetLive ());	// TextView by default is never live, but we want to select
}
	
Boolean	MyTextView::TrackPress (const MousePressInfo& mouseInfo)
{
	if ((not fNode.GetSelected ()) or (mouseInfo.IsExtendedSelection ())) {
		if (not mouseInfo.IsExtendedSelection ()) {
			fNode.GetTrunkNode ().DeselectAll (eImmediateUpdate);
		}
		fNode.SetSelected (not fNode.GetSelected (), eImmediateUpdate);
		return (True);
	}
	else {
		fNode.GetTrunkNode ().DragSelected (MousePressInfo (mouseInfo, LocalToAncestor (mouseInfo.fPressAt, &DeskTop::Get ())));
	}
	
	return (True);
}
		
void	MyTextView::Draw (const Region& update)
{
	TextView::Draw (update);
	if (fNode.GetSelected ()) {
		Invert ();
	}
}

/*
 ********************************************************************************
 ********************************** DemoLeafNode ******************************
 ********************************************************************************
 */
DemoLeafNode::DemoLeafNode (const String& title, NodeViewDocument& doc) :
	DemoNode (doc),
	fTextView (Nil),
	fSelected (False)
{
	AddSubView (fTextView = new MyTextView (title, AbstractTextView::eJustRight, *this));
}

DemoLeafNode::~DemoLeafNode ()
{
	RemoveSubView (fTextView);
	delete fTextView;
}

String	DemoLeafNode::GetText () const
{
	return (fTextView->GetText ());
}

void	DemoLeafNode::SetText (const String& text, Panel::UpdateMode update)
{
	fTextView->SetText (text, update);
}

SequenceIterator(NodeViewPtr)*	DemoLeafNode::MakeNodeIterator (SequenceDirection traversalDirection)
{
	return (new LeafNodeIterator (traversalDirection));
}

CollectionSize	DemoLeafNode::CountNodes () const
{
	return (0);
}

Boolean	DemoLeafNode::GetSelected () const
{
	return (fSelected);
}

void	DemoLeafNode::SetSelected (Boolean selected, Panel::UpdateMode update)
{
	if (selected != GetSelected ()) {
		fSelected = selected;
		CollectionSize selectedCount = GetTrunkNode ().GetSelectedCount ();
		GetTrunkNode ().SetSelectedCount ((fSelected) ? ++selectedCount : --selectedCount);
		
		if (GetEffectiveVisibility ()) {
			if (update == eImmediateUpdate) {
				Invert ();
			}
			else {
				Refresh (update);
			}
		}
	}
	Ensure (selected == GetSelected ());
}
		
void	DemoLeafNode::Layout ()
{
	fTextView->SetSize (GetSize ());
	View::Layout ();
}

Point	DemoLeafNode::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	size = fTextView->CalcDefaultSize (defaultSize);
	if (GetDocument ().GetDirection () == Point::eHorizontal) {
		size += Point (0, 3);
	}
	return (size);
}

class	DemoLeafIterator : public Iterator(DemoLeafNodePtr) {
	public:
		DemoLeafIterator (DemoLeafNode* leaf);
		
		override	Boolean			Done () const;
		override	void			Next ();
		override	DemoLeafNodePtr	Current () const;
	
	private:
		Boolean			fDone;
		DemoLeafNode*	fLeaf;
};

DemoLeafIterator::DemoLeafIterator (DemoLeafNode* leaf) : 
	Iterator(DemoLeafNodePtr) (),
	fDone (False),
	fLeaf (leaf)
{
	RequireNotNil (leaf);
}

Boolean	DemoLeafIterator::Done () const
{
	return (fDone);
}

void	DemoLeafIterator::Next ()
{
	fDone = True;
}

DemoLeafNodePtr		DemoLeafIterator::Current () const
{
	Assert (not fDone);
	return (fLeaf);
}

SequenceIterator(DemoLeafNodePtr)*	DemoLeafNode::MakeDemoLeafIterator (SequenceDirection /*traversalDirection*/)
{
	return (new DemoLeafIterator (this));
}

/*
 ********************************************************************************
 ********************************* DemoBranchNode *******************************
 ********************************************************************************
 */
DemoBranchNode::DemoBranchNode (NodeViewDocument& doc) :
	DemoNode (doc),
	fBranchNode (Nil)
{
	fBranchNode = new MyBranchNode (*this);
	AddSubView (fBranchNode);
}

void	DemoBranchNode::Layout ()
{
	GetBranchNode ().SetSize (GetSize ());
	DemoNode::Layout ();
}

Point	DemoBranchNode::CalcDefaultSize_ (const Point& defaultSize) const
{
	return (GetBranchNode ().CalcDefaultSize (defaultSize));
}

SequenceIterator(NodeViewPtr)*	DemoBranchNode::MakeNodeIterator (SequenceDirection traversalDirection)
{
	return (GetBranchNode ().MakeNodeIterator (traversalDirection));
}

CollectionSize	DemoBranchNode::CountNodes () const
{
	return (GetBranchNode ().CountNodes ());
}

void	DemoBranchNode::AddNode (DemoNode* node, DemoNode* neighbor)
{
	CollectionSize index = 1;
	if (neighbor != Nil) {
		ForEachS (NodeViewPtr, it, MakeNodeIterator ()) {
			if (it.Current () == neighbor) {
				index = it.CurrentIndex ();
				break;
			}
		}
	}
	
	fBranchNode->AddNode (node, index);
	node->fParentNode = this;
}

void	DemoBranchNode::RemoveNode (DemoNode* node)
{
	fBranchNode->RemoveNode (node);
	node->fParentNode = Nil;
}

BranchNode&	DemoBranchNode::GetBranchNode () const
{
	EnsureNotNil (fBranchNode);
	return (*fBranchNode);
}

class	DemoBranchIterator : public SequenceIterator(DemoLeafNodePtr) {
	public:
		DemoBranchIterator (MyBranchNode& branch, SequenceDirection traversalDirection);
		~DemoBranchIterator ();
		
		override	Boolean			Done ();
		override	void			Next ();
		override	DemoLeafNodePtr	Current ();
		override	CollectionSize	CurrentIndex ();
	
	private:
		MyBranchNode&						fBranch;
		SequenceDirection			fDirection;
		SequenceIterator(DemoNodePtr)*		fTopIterator;
		SequenceIterator(DemoLeafNodePtr)*	fCurrentIterator;
		CollectionSize						fCurrentIndex;
};

DemoBranchIterator::DemoBranchIterator (MyBranchNode& branch, SequenceDirection traversalDirection) :
	SequenceIterator(DemoLeafNodePtr) (traversalDirection),
	fBranch (branch),
	fDirection (traversalDirection),
	fTopIterator (Nil),
	fCurrentIterator (Nil),
	fCurrentIndex (1)
{
	fTopIterator = fBranch.fNodes.MakeSequenceIterator (traversalDirection);
	if (not fTopIterator->Done ()) {
		fCurrentIterator = fTopIterator->Current ()->MakeDemoLeafIterator (fDirection);
	}
}

Boolean	DemoBranchIterator::Done ()
{
	return (Boolean (fCurrentIterator == Nil));
}

void	DemoBranchIterator::Next ()
{
	AssertNotNil (fCurrentIterator);
	
	fCurrentIterator->Next ();
	if (fCurrentIterator->Done ()) {
		delete fCurrentIterator;
		fCurrentIterator = Nil;
	}
	while (fCurrentIterator == Nil) {
		if (fTopIterator->Done ()) {
			break;
		}

		fTopIterator->Next ();
		
		if (not fTopIterator->Done ()) {
			fCurrentIterator = fTopIterator->Current ()->MakeDemoLeafIterator (fDirection);
			if (fCurrentIterator->Done ()) {
				delete fCurrentIterator;
				fCurrentIterator = Nil;
			}
		}
	}
}

DemoLeafNodePtr	DemoBranchIterator::Current ()
{
	AssertNotNil (fCurrentIterator);
	Assert (not fCurrentIterator->Done ());
	return (fCurrentIterator->Current ());
}

CollectionSize	DemoBranchIterator::CurrentIndex ()
{
	return (fCurrentIndex);
}

DemoBranchIterator::~DemoBranchIterator ()
{
	delete fCurrentIterator;
	delete fTopIterator;
}


SequenceIterator(DemoLeafNodePtr)*	DemoBranchNode::MakeDemoLeafIterator (SequenceDirection traversalDirection)
{
	return (new DemoBranchIterator (*fBranchNode, traversalDirection));
}

/*
 ********************************************************************************
 ********************************** MyBranchNode *******************************
 ********************************************************************************
 */
MyBranchNode::MyBranchNode (DemoBranchNode& owner) :
	BranchNode (),
	fOwner (owner),
	fNodes (),
	fExpanded (True)
{
}
	
NodeViewDocument&	MyBranchNode::GetDocument () const
{
	return (fOwner.GetDocument ());
}

void	MyBranchNode::AddNode (DemoNode* node, CollectionSize index)
{
	BranchNode::AddNode (node, index);
	fNodes.InsertAt (node, index);
}

void	MyBranchNode::RemoveNode (DemoNode* node)
{
	fNodes.Remove (node);
	BranchNode::RemoveNode (node);
}

SequenceIterator(NodeViewPtr)*	MyBranchNode::MakeNodeIterator (SequenceDirection traversalDirection)
{
	return ((SequenceIterator(NodeViewPtr)*) fNodes.MakeSequenceIterator (traversalDirection));
}

CollectionSize	MyBranchNode::CountNodes () const
{
	return (fNodes.GetLength ());
}

Point	MyBranchNode::GetSpacing_ () const
{
	return (GetDocument ().GetSpacing ());
}

void	MyBranchNode::SetSpacing_ (const Point& spacing, Panel::UpdateMode /*update*/)
{
	GetDocument ().SetSpacing (spacing);
}

Point	MyBranchNode::GetArcLength_ () const
{
	return (GetDocument ().GetArcLength ());
}

void	MyBranchNode::SetArcLength_ (const Point& arcLength, Panel::UpdateMode /*update*/)
{
	GetDocument ().SetArcLength (arcLength);
}
		
Boolean	MyBranchNode::GetExpanded_ () const
{
	return (fExpanded);
}

void	MyBranchNode::SetExpanded_ (Boolean expanded, Panel::UpdateMode /*update*/) 
{
	fExpanded = expanded;
}

Boolean	MyBranchNode::GetExpandable_ () const
{
	return (GetDocument ().GetExpandable ());
}

void	MyBranchNode::SetExpandable_ (Boolean expandable)
{
	GetDocument ().SetExpandable (expandable);
}
	
Point::Direction	MyBranchNode::GetDirection_ () const
{
	return (GetDocument ().GetDirection ());
}

void	MyBranchNode::SetDirection_ (Point::Direction direction, Panel::UpdateMode /*update*/)
{
	GetDocument ().SetDirection (direction);
}


/*
 ********************************************************************************
 ********************************** MyBranchNode *******************************
 ********************************************************************************
 */
Sequence(DemoTrunkNodePtr)	DemoTrunkNode::sTrunkNodes;

DemoTrunkNode::DemoTrunkNode (NodeViewDocument& doc) :	
	DemoBranchNode (doc),
	fSelectedCount (0)
{
	sTrunkNodes.Append (this);
}

DemoTrunkNode::~DemoTrunkNode ()
{
	sTrunkNodes.Remove (this);
}

CollectionSize	DemoTrunkNode::GetSelectedCount () const
{
	return (fSelectedCount);
}

void	DemoTrunkNode::SetSelectedCount (CollectionSize count)
{
	fSelectedCount = count;
	if (count <= 2) {
		MenuOwner::SetMenusOutOfDate ();
	}
}
		
Boolean	DemoTrunkNode::TrackPress (const MousePressInfo& mouseInfo)
{
	if (DemoBranchNode::TrackPress (mouseInfo)) {
		return (True);
	}
	DeselectAll (eImmediateUpdate);
	
	Selector	selector = Selector (*this, Rectangle (), GetLocalExtent (), True);
	selector.SetTimeOut (0);
	(void) selector.TrackPress (mouseInfo);
	Rect selection = selector.GetSelection ();
	ForEach (DemoLeafNodePtr, it, MakeDemoLeafIterator ()) {
		AssertNotNil (it.Current ());
		Rect extent = it.Current ()->LocalToLocal (it.Current ()->GetLocalExtent (), this);
		if (Intersects (extent, selection)) {
			it.Current ()->SetSelected (True);
		}
	}
	Update ();

	return (False);
}

Boolean	DemoTrunkNode::HandleKeyStroke (const KeyStroke& keyStroke)
{
	static	Time oldTime = Time ();
	static	String	pendingString = kEmptyString;
	
	Time newTime = GetCurrentTime ();
	Time delta = newTime - oldTime;
	if (delta > 1) {
		pendingString = kEmptyString;
	}
	pendingString += keyStroke.GetCharacter ();
	CollectionSize pendingStringLength = pendingString.GetLength ();
	ForEach (DemoLeafNodePtr, it, MakeDemoLeafIterator (eSequenceBackward)) {
		AssertNotNil (it.Current ());
		String text = it.Current ()->GetText ();
		if ((text.GetLength () >= pendingStringLength) and (text.IndexOf (pendingString) == 1)) {
			if (not it.Current ()->GetSelected ()) {
				DeselectAll (eImmediateUpdate);
				it.Current ()->SetSelected (True, eImmediateUpdate);
			}
			break;
		}
	}
	oldTime = newTime;
	return (True);
}

Point	DemoTrunkNode::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point newSize = DemoNode::CalcDefaultSize_ (defaultSize);
	if (newSize == kZeroPoint) {
		newSize = Point (100, 100);
	}
	return (newSize);
}

void	DemoTrunkNode::DoSetupMenus ()
{
	MenuCommandHandler::DoSetupMenus ();
	
	EnableCommand (eAddNode, Boolean (GetSelectedCount () <= 1));
	EnableCommand (eRemoveNode, Boolean (GetSelectedCount () > 0));
	EnableCommand (eShiftLeft, Boolean (GetSelectedCount () == 1));
	EnableCommand (eShiftRight, Boolean (GetSelectedCount () == 1));
}

Boolean	DemoTrunkNode::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eAddNode:
			{
				if (GetSelectedCount () == 0) {
	// change to add dialog
					DemoLeafNode* newNode = new DemoLeafNode ("Hi Mom", GetDocument ());
					AddNode (newNode);
				}
				else {
					ForEach (DemoLeafNodePtr, it, MakeDemoLeafIterator ()) {
						AssertNotNil (it.Current ());
						DemoLeafNode* current = it.Current ();
						if (current->GetSelected ()) {
	// change to add dialog
							DemoLeafNode* newNode = new DemoLeafNode ("Hi Mom", GetDocument ());
							current->GetParentNode ().AddNode (newNode, current);
							break;
						}
					}
				}
				GetDocument ().SetDirty ();
				SetSize (CalcDefaultSize ());
			}
			return (True);
			
		case eRemoveNode:
			{
				Sequence(DemoNodePtr)	deadPool;
				ForEach (DemoLeafNodePtr, it, MakeDemoLeafIterator ()) {
					AssertNotNil (it.Current ());
					DemoLeafNode* current = it.Current ();
					if (current->GetSelected ()) {
						SetSelectedCount (GetSelectedCount () -1);
						DemoBranchNode* parent = &current->GetParentNode ();
						current->GetParentNode ().RemoveNode (current);
						delete current;
						if (parent->CountNodes () == 0) {
							parent->GetParentNode ().RemoveNode (parent);
							// can't just delete cuz our iterating over his list!!!
							// instead, add to deadpool and kill at end
							deadPool.Append (parent);
						}
					}
				}
				ForEach (DemoNodePtr, it1, deadPool) {
					DemoNode* current = it1.Current ();
					delete current;
				}
				GetDocument ().SetDirty ();
				SetSize (CalcDefaultSize ());
			}
			return (True);
			
		case eShiftLeft:
			{
				Sequence(DemoNodePtr)	deadPool;
				ForEach (DemoLeafNodePtr, it, MakeDemoLeafIterator ()) {
					AssertNotNil (it.Current ());
					DemoLeafNode* current = it.Current ();
					if (current->GetSelected ()) {
						DemoBranchNode* parent = &current->GetParentNode ();
						parent->RemoveNode (current);
						parent->GetParentNode ().AddNode (current, parent);
						if (parent->CountNodes () == 0) {
							parent->GetParentNode ().RemoveNode (parent);
							// can't just delete cuz our iterating over his list!!!
							// instead, add to deadpool and kill at end
							deadPool.Append (parent);
						}
					}
				}
				ForEach (DemoNodePtr, it1, deadPool) {
					DemoNode* current = it1.Current ();
					delete current;
				}
				GetDocument ().SetDirty ();
				SetSize (CalcDefaultSize ());
			}
			return (True);
			
		case eShiftRight:
			{
				Sequence(DemoLeafNodePtr)	selectPool;
				ForEach (DemoLeafNodePtr, it, MakeDemoLeafIterator ()) {
					AssertNotNil (it.Current ());
					DemoLeafNode* current = it.Current ();
					if (current->GetSelected ()) {
						current->SetSelected (False);
						selectPool.Append (current);
						
						DemoBranchNode& parent = current->GetParentNode ();
						
						DemoBranchNode* newParent = new DemoBranchNode (GetDocument ());
						parent.AddNode (newParent, current);
						parent.RemoveNode (current);
						newParent->AddNode (current);
					}
				}
				ForEach (DemoLeafNodePtr, it1, selectPool) {
					DemoLeafNode* current = it1.Current ();
					current->SetSelected (True);
				}
				GetDocument ().SetDirty ();
				SetSize (CalcDefaultSize ());
			}
			return (True);
		default:
			return (MenuCommandHandler::DoCommand (selection));
	}
	AssertNotReached (); return (False);
}

void	DemoTrunkNode::DeselectAll (Panel::UpdateMode update)
{
	if (GetSelectedCount () > 0) {
		Panel::UpdateMode tempUpdate = update;
		if (tempUpdate == eImmediateUpdate) {
			tempUpdate = eDelayedUpdate;
		}
		
		ForEach (DemoLeafNodePtr, it, MakeDemoLeafIterator ()) {
			it.Current ()->SetSelected (False, tempUpdate);
		}
		if (update == eImmediateUpdate) {
			Update ();
		}
	}
}

void	DemoTrunkNode::DragSelected (const MousePressInfo& mouseInfo)
{
	Region	region = kEmptyRegion;
	ForEach (DemoLeafNodePtr, it, MakeDemoLeafIterator ()) {
		DemoLeafNode&	current = *it.Current ();
		if (current.GetSelected ()) {
			Rect bounds = current.LocalToAncestor (current.GetLocalExtent (), &DeskTop::Get ());
			region += bounds;
		}
	}
	Assert (region != kEmptyRegion);
	
	Rect	bounds = region.GetBounds ();
	ShapeDragger shapeDragger = ShapeDragger (RegionShape (region), bounds, DeskTop::Get (),
			*GetDocument ().GetNodeWindow ().GetPlane ().GetTablet (), kZeroPoint);
	(void) shapeDragger.TrackPress (mouseInfo);
	
	Rect myBounds = LocalToAncestor (GetLocalExtent (), &DeskTop::Get ());
	Point	delta = shapeDragger.GetNewShapeExtent ().GetOrigin () - shapeDragger.GetOldShapeExtent ().GetOrigin ();
	Point	dropPoint = mouseInfo.fPressAt + delta;
	if (not myBounds.Contains (dropPoint)) {
		ForEach (DemoTrunkNodePtr, it, sTrunkNodes) {
			if (it.Current () != this) {
				DemoTrunkNode& current = *it.Current ();
				Rect currentBounds = current.LocalToAncestor (current.GetLocalExtent (), &DeskTop::Get ());
				if (currentBounds.Contains (dropPoint)) {
					ForEach (DemoLeafNodePtr, it1, MakeDemoLeafIterator (eSequenceBackward)) {
						AssertNotNil (it1.Current ());
						DemoLeafNode& oldLeaf = *it1.Current ();
						if (oldLeaf.GetSelected ()) {
							DemoLeafNode* newNode = new DemoLeafNode (oldLeaf.GetText (), GetDocument ());
							current.AddNode (newNode);
						}
					}
					
					break;
				}
			}
		}
	}
}

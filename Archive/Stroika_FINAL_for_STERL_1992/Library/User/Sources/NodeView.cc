/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/NodeView.cc,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: NodeView.cc,v $
 *		Revision 1.6  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/04  14:32:40  lewis
 *		Shapes now pass by value using new letter / env stuff. As side effect of that
 *		I have (at least temporarily gotten rid of Setter methods for fields of shapes
 *		So, I changed updates of line with SetFrom/SetTo to line = Line (from,to).
 *		I think thats better anyhow, but I could add back the setter methods?
 *
 *		Revision 1.3  1992/07/03  02:29:05  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.8  1992/04/06  14:51:45  sterling
 *		major surgery
 *
 *
 *
 */

#include	"Shape.hh"

#include	"NodeView.hh"

#if		!qRealTemplatesAvailable
	Implement (Iterator, NodeViewPtr);
	Implement (Collection, NodeViewPtr);
	Implement (AbSequence, NodeViewPtr);
#endif

/*
 ********************************************************************************
 ******************************** NodeView ******************************
 ********************************************************************************
 */
Point	NodeView::kDefaultSpacing 	= Point (5, 5);
Point	NodeView::kDefaultArcLength = Point (20, 20);

NodeView::NodeView ()
{
}


/*
 ********************************************************************************
 ************************************ LeafNode **********************************
 ********************************************************************************
 */
LeafNode::LeafNode ()
{
}
	
Iterator(NodeViewPtr)*	LeafNode::MakeNodeIterator (SequenceDirection d)
{
	return (new LeafNodeIterator ());
}

CollectionSize	LeafNode::CountNodes () const
{
	return (0);
}

/*
 ********************************************************************************
 ******************************** LeafNodeIterator ******************************
 ********************************************************************************
 */
LeafNodeIterator::LeafNodeIterator () :
	Iterator(NodeViewPtr) ()	
{
}

Boolean		LeafNodeIterator::Done () const
{
	return (True);
}

void	LeafNodeIterator::Next ()
{
}

NodeViewPtr		LeafNodeIterator::Current () const
{
	AssertNotReached ();	return (Nil);
}

void*	LeafNodeIterator::operator new (size_t n)
{
	Require (n == sizeof (LeafNodeIterator));
	if (sThis == Nil) {
		sThis = (LeafNodeIterator*)::new char [sizeof (LeafNodeIterator)];
	}
	return (sThis);
}

void	LeafNodeIterator::operator delete (void* p)
{
	RequireNotNil (p);
	Ensure (sThis == (LeafNodeIterator*)p);
}


LeafNodeIterator*	LeafNodeIterator::sThis	=	Nil;		

/*
 ********************************************************************************
 *********************************** BranchNode *********************************
 ********************************************************************************
 */
BranchNode::BranchNode ()
{
}
		
Boolean	BranchNode::TrackPress (const MousePressInfo& mouseInfo)
{
	if (NodeView::TrackPress (mouseInfo)) {
		return (True);
	}
	else if (GetExpandable () and mouseInfo.IsOpenSelection ()) {
		if (not GetExpanded ()) {
			SetExpanded (not GetExpanded (), eImmediateUpdate);
			return (True);
		}
		else {
			Point::Direction dir = GetDirection ();
			Point	anchor = CalcRectPoint (GetLocalExtent (), 
				((dir == Point::eVertical) ? 0   : 0.5), 
				((dir == Point::eVertical) ? 0.5 : 0));
			Rect anchorRect (anchor - Point (2, 2), Point (5, 5));
			if (anchorRect.Contains (mouseInfo.fPressAt)) {
				SetExpanded (not GetExpanded (), eImmediateUpdate);
				return (True);
			}
		}
	}
	return (False);
}

void	BranchNode::Draw (const Region& /*update*/)
{
	Point::Direction	dir = GetDirection ();
	
	Line	line = Line (kZeroPoint, kZeroPoint);
	NodeView*	topView  = Nil;
	NodeView*	bottomView = Nil;
	if (GetExpanded ()) {
		ForEach (NodeViewPtr, it, MakeNodeIterator (eSequenceBackward)) {
			register	NodeView*	v	=	it.Current ();
			AssertNotNil (v);
			if (v->GetVisible ()) {
				if (topView == Nil) {
					topView = v;
				}
				bottomView = v;
				
				Point	anchor = CalcNodeAnchor (*v);			
				Point	startP = anchor;
				startP.SetVH (dir, 0);
				DrawArc (startP, anchor);
			}
		}
	}
	
	if (topView == Nil) {
		Point	anchor = CalcRectPoint (GetLocalExtent (), 
			((dir == Point::eVertical) ? 0   : 0.5), 
			((dir == Point::eVertical) ? 0.5 : 0));
		line = Line (anchor, line.GetTo ());
		anchor = CalcRectPoint (GetLocalExtent (), 
					((dir == Point::eVertical) ? .5 : 0.5), 
					((dir == Point::eVertical) ? 0.5 : .5));
		line = Line (line.GetFrom (), anchor);
		OutLine (line);	
		if (not GetExpanded ()) {
			Fill (Oval (), Rect (anchor - Point (2, 2), Point (5, 5)), kBlackTile);
		}
	}
	else if (topView != bottomView) {
		Assert (bottomView != Nil);
		Point	anchor = CalcNodeAnchor (*topView);			
		anchor.SetVH (dir, 0);
		line = Line (anchor, line.GetTo ());
		anchor = CalcNodeAnchor (*bottomView);			
		anchor.SetVH (dir, 0);
		line = Line (line.GetFrom (), anchor);
		OutLine (line);			
	}
}

Point	BranchNode::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	return (LayoutNodes ());
}
	
Point	BranchNode::GetSpacing () const
{
	return (GetSpacing_ ());
}

void	BranchNode::SetSpacing (const Point& spacing, Panel::UpdateMode update)
{
	if (GetSpacing () != spacing) {
		SetSpacing_ (spacing, update);
		Resize (update);
	}
	Ensure (GetSpacing () == spacing);
}

Point	BranchNode::GetArcLength () const
{
	return (GetArcLength_ ());
}

void	BranchNode::SetArcLength (const Point& arcLength, Panel::UpdateMode update)
{
	if (GetArcLength () != arcLength) {
		SetArcLength_ (arcLength, update);
		Resize (update);
	}
	Ensure (GetArcLength () == arcLength);
}

Boolean	BranchNode::GetExpanded () const
{
	return (GetExpanded_ ());
}

void	BranchNode::SetExpanded (Boolean expanded, Panel::UpdateMode update) 
{
	if (GetExpanded () != expanded) {
		SetExpanded_ (expanded, update);
		ForEach (NodeViewPtr, it, MakeNodeIterator ()) {
			it.Current ()->SetVisible (expanded, eNoUpdate);
		}
		Resize (update);
		if (update == eImmediateUpdate) {
			Update ();
		}
	}
	Ensure (GetExpanded () == expanded);
}

Boolean	BranchNode::GetExpandable () const
{
	return (GetExpandable_ ());
}

void	BranchNode::SetExpandable (Boolean expandable)
{
	if (GetExpandable () != expandable) {
		SetExpandable_ (expandable);
	}
	Ensure (GetExpandable () == expandable);
}

Point::Direction	BranchNode::GetDirection () const
{
	return (GetDirection_ ());
}

void	BranchNode::SetDirection (Point::Direction direction, Panel::UpdateMode update)
{
	if (GetDirection () != direction) {
		SetDirection_ (direction, update);
		Resize (update);
	}
	Ensure (GetDirection () == direction);
}

void	BranchNode::Layout ()
{
	LayoutNodes ();
//	Refresh ();
	NodeView::Layout ();
}
		
Point	BranchNode::CalcNodeAnchor (NodeView& node)
{
	Require (node.GetParentView () == this);
	
	Point::Direction	dir = GetDirection ();
	return (CalcRectPoint (node.GetExtent (), 
		((dir == Point::eVertical) ? 0   : 0.5), 
		((dir == Point::eVertical) ? 0.5 : 0)));
}

void	BranchNode::DrawArc (const Point& from, const Point& to)
{
	OutLine (Line (from, to));			
}

void	BranchNode::AddNode (NodeView* node, CollectionSize index)
{
	AddSubView (node, index);
	Resize (eDelayedUpdate);
}

void	BranchNode::ReorderNode (NodeView* node, CollectionSize index)
{
	ReorderSubView (node, index);
	Resize (eDelayedUpdate);
}

void	BranchNode::RemoveNode (NodeView* node)
{
	RemoveSubView (node);
	Resize (eDelayedUpdate);
}

void	BranchNode::Resize (Panel::UpdateMode update)
{
	Point	p = CalcDefaultSize ();
	if (GetSize () != p) {
		SetSize (p, eNoUpdate);
		View*	parent = GetParentView ();
		while (parent != Nil) {
			parent->InvalidateLayout ();
			parent = parent->GetParentView ();
		}
	}
	else {
		Refresh (update);
	}
}

Point	BranchNode::LayoutNodes ()
{
	Point::Direction	dir 	 = GetDirection ();
	Point::Direction	otherDir = ((GetDirection () == Point::eVertical) ? Point::eHorizontal : Point::eVertical);
	
	Point	origin = kZeroPoint;
	Point	spacing = kZeroPoint;
	Point	maxSize = kZeroPoint;
	Int16	subViewCount = 0;

	origin.SetVH (dir, GetArcLength ()[dir]);
	spacing.SetVH (otherDir, GetSpacing ()[otherDir]);
	
	if (GetExpanded ()) {
		ForEach (NodeViewPtr, it, MakeNodeIterator (eSequenceBackward)) {
			register	NodeView*	v	=	it.Current ();
			AssertNotNil (v);
			if (v->GetVisible ()) {
				v->SetSize (v->CalcDefaultSize (), eNoUpdate);
				v->SetOrigin (origin, eNoUpdate);
				
				Point	offset = kZeroPoint;
				offset.SetVH (otherDir, v->GetSize ()[otherDir]);
				origin += (offset + spacing);
				
				if (maxSize[dir] < v->GetSize ()[dir]) {
					maxSize = v->GetSize ();
				}
				subViewCount++;
			}
		}
	}
	else {
		return (origin + spacing);
	}
	
	if (subViewCount > 0) {
		origin -= spacing;	// go one to far
	}
	
	Point	result = origin;
	result.SetVH (dir, maxSize[dir] + origin[dir]);
	return (result);
}





/*
 ********************************************************************************
 ********************************** SimpleBranch ********************************
 ********************************************************************************
 */
SimpleBranch::SimpleBranch () :
	fDirection (Point::eHorizontal),
	fSpacing (kDefaultSpacing),
	fArcLength (kDefaultArcLength),
	fExpandable (True),
	fExpanded (True)
{
}

Point	SimpleBranch::GetSpacing_ () const
{
	return (fSpacing);
}

void	SimpleBranch::SetSpacing_ (const Point& spacing, Panel::UpdateMode /*update*/)
{
	fSpacing = spacing;
}

Point	SimpleBranch::GetArcLength_ () const
{
	return (fArcLength);
}

void	SimpleBranch::SetArcLength_ (const Point& arcLength, Panel::UpdateMode /*update*/)
{
	fArcLength = arcLength;
}
		
Boolean	SimpleBranch::GetExpanded_ () const
{
	return (fExpanded);
}

void	SimpleBranch::SetExpanded_ (Boolean expanded, Panel::UpdateMode /*update*/)
{
	fExpanded = expanded;
}

Boolean	SimpleBranch::GetExpandable_ () const
{
	return (fExpandable);
}

void	SimpleBranch::SetExpandable_ (Boolean expandable)
{
	fExpandable = expandable;
}

Point::Direction	SimpleBranch::GetDirection_ () const
{
	return (fDirection);
}

void	SimpleBranch::SetDirection_ (Point::Direction direction, Panel::UpdateMode /*update*/)
{
	fDirection = direction;
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


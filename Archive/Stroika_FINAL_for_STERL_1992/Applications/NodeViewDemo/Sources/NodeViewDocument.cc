/*
 * $Header: /fuji/lewis/RCS/NodeViewDocument.cc,v 1.4 1992/09/08 16:48:46 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: NodeViewDocument.cc,v $
 *		Revision 1.4  1992/09/08  16:48:46  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:31:57  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  92/05/01  23:24:08  23:24:08  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 *
 *
 */

#include "Debug.hh"
#include "StreamUtils.hh"

#include "TextView.hh"

#include "DemoNodes.hh"
#include "NodeViewDocument.hh"
#include "NodeViewWindow.hh"


/*
 ********************************************************************************
 ******************************* NodeViewDocument ***************************
 ********************************************************************************
 */

NodeViewDocument::NodeViewDocument ():
	FileBasedDocument (1),
	fNodeWindow (Nil),
	fDirection (Point::eHorizontal),
	fSpacing (NodeView::kDefaultSpacing),
	fArcLength (NodeView::kDefaultArcLength),
	fExpandable (True)
{
#if	qMacOS
	SetFileType ('TEXT');
#endif
}

NodeViewDocument::~NodeViewDocument ()
{
}

NodeViewWindow&	NodeViewDocument::GetNodeWindow () const
{
	EnsureNotNil (fNodeWindow);
	return (*fNodeWindow);
}

void	NodeViewDocument::DoNewState ()
{
	fNodeWindow = new NodeViewWindow (*this);
	SetDirty ();

	fNodeWindow->SetTitle (GetPrintName ());


	fNodeWindow->GetNodeView ().SetSize (fNodeWindow->GetNodeView ().CalcDefaultSize ());
//	fNodeWindow->GetScroller ()->ScrolledViewChangedSize ();

	fNodeWindow->Select ();
}

DemoLeafNode*	NodeViewDocument::BuildNode (const String& nodeString)
{
	return (new DemoLeafNode (nodeString, *this));
}

DemoBranchNode*	NodeViewDocument::BuildGroupNode (DemoBranchNode& parent)
{
	DemoBranchNode* newNode = new DemoBranchNode (*this);
	parent.AddNode (newNode);
	
	EnsureNotNil (newNode);
	return (newNode);
}

Int16	NodeViewDocument::ReadOneNode (class istream& from, Int16 tabCount, String& nodeTitle)
{
	nodeTitle = kEmptyString;
	char c = ' ';
	
	if (from) {
		from.get (c);
		if (c != '\n') {
			return (-1);
		}
		
		Int16	newTabCount = 0;
		while (from.get (c)) {
			if (c == '\t') {
				newTabCount++;
			}
			else {
				from.putback (c);
				break;
			}
		}
		if (newTabCount < tabCount) {
			for (int i = 1; i <= newTabCount; i++) {
				from.putback ('\t');
			}
			from.putback ('\n');
			return (-1);
		}
		ReadString (from, nodeTitle);
		return (newTabCount);
	}
	return (-1);
}

void	NodeViewDocument::ReadInNodes (class istream& from, DemoBranchNode* parent, Int16 tabCount)
{
	String	nodeString = kEmptyString;
	while (from) {
		RequireNotNil (parent);
	
		Int16	newTabCount = ReadOneNode (from, tabCount, nodeString);
		if (newTabCount == tabCount) {
			parent->AddNode (BuildNode (nodeString));
		}
		else if (newTabCount > tabCount) {
			DemoBranchNode* foo = BuildGroupNode (*parent);
			foo->AddNode (BuildNode (nodeString));
			ReadInNodes (from, foo, tabCount+1);
		}
		else {
			break;
		}
	}
}

void	NodeViewDocument::WriteOutNodes (class ostream& to)
{
	ForEach (DemoLeafNodePtr, it, GetNodeWindow ().GetNodeView ().MakeDemoLeafIterator (eSequenceBackward)) {
		WriteOneNode (to, *it.Current ());
	}
}

void	NodeViewDocument::WriteOneNode (class ostream& to, const DemoLeafNode& node)
{
	int tabCount = 0;
	DemoBranchNode* trunk = &node.GetTrunkNode ();
	DemoBranchNode* parent = &node.GetParentNode ();
	while (parent != trunk) {
		AssertNotNil (parent);
		parent = &parent->GetParentNode ();
		tabCount++;
	}
	to << tab (tabCount-1);
	WriteString (to, node.GetText ());
	to << newline;
}

void	NodeViewDocument::DoRead_ (class istream& from)
{
	char	vertical = ' ';
	
	from >> vertical;
	SetDirection ((vertical == 'V') ? Point::eVertical : Point::eHorizontal);
	
	if (fNodeWindow == Nil) {
		fNodeWindow = new NodeViewWindow (*this);
	}

	DemoBranchNode* foo = BuildGroupNode (fNodeWindow->GetNodeView ());
	ReadInNodes (from, foo, 0);
	fNodeWindow->GetNodeView ().SetSize (fNodeWindow->GetNodeView ().CalcDefaultSize ());
//	fNodeWindow->GetScroller ()->ScrolledViewChangedSize ();

	fNodeWindow->SetTitle (GetPrintName ());
	fNodeWindow->Select ();
}

void	NodeViewDocument::DoWrite_ (class ostream& to, int /*tabCount*/) const
{
	AssertNotNil (fNodeWindow);
	Assert (to.good ());
	
	to << ((GetDirection () == Point::eVertical) ? 'V' : 'H');
	to << newline;
	WriteOutNodes (to);
	
	fNodeWindow->SetTitle (GetPrintName ());
}

off_t	NodeViewDocument::EstimatedFileSize ()
{
	AssertNotNil (fNodeWindow);
	return (1000);
}

Point	NodeViewDocument::GetSpacing () const
{
	return (fSpacing);
}

void	NodeViewDocument::SetSpacing (const Point& spacing)
{
	fSpacing = spacing;
}

Boolean	NodeViewDocument::GetExpandable () const
{
	return (fExpandable);
}

void	NodeViewDocument::SetExpandable (Boolean expandable) 
{
	fExpandable = expandable;
}

Point	NodeViewDocument::GetArcLength () const
{
	return (fArcLength);
}

void	NodeViewDocument::SetArcLength (const Point& arcLength)
{
	fArcLength = arcLength;
}
	
Point::Direction	NodeViewDocument::GetDirection () const
{
	return (fDirection);
}

void	NodeViewDocument::SetDirection (Point::Direction direction)
{
	fDirection = direction;
}
/*
 * $Header: /fuji/lewis/RCS/NodeViewWindow.cc,v 1.4 1992/09/08 16:48:46 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: NodeViewWindow.cc,v $
 *		Revision 1.4  1992/09/08  16:48:46  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:31:57  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Dialog.hh"

#include	"NodeView.hh"

#include	"ConfigureNodeView.hh"
#include 	"DemoNodes.hh"
#include	"NodeViewCommands.hh"
#include	"NodeViewDocument.hh"
#include	"NodeViewWindow.hh"
#include	"NodeViewWindow.hh"

/*
 ********************************************************************************
 ******************************* NodeViewWindow *********************************
 ********************************************************************************
 */
NodeViewWindow::NodeViewWindow (NodeViewDocument& doc):
	Window (),
	fNodeView (Nil),
	fDocument (doc)
{		
	fNodeView = new DemoTrunkNode (doc);
	SetMainViewAndTargets (fNodeView, fNodeView, fNodeView);
	SetWindowController (&doc);
}

DemoTrunkNode&	NodeViewWindow::GetNodeView () const
{
	EnsureNotNil (fNodeView);
	return (*fNodeView);
}

void	NodeViewWindow::DoSetupMenus ()
{
	Window::DoSetupMenus ();

	EnableCommand (eConfigureNodes, True);
}

class	ConfigureNodeCommand : public Command {
	public:
		ConfigureNodeCommand (ConfigureNodeView& query, NodeViewDocument& doc, NodeView& nodeView) :
			Command (eConfigureNodes, True),
			fDocument (doc),
			fOldDirection (doc.GetDirection ()),
			fNewDirection (query.GetDirection ()),
			fOldExpandable (doc.GetExpandable ()),
			fNewExpandable (query.GetExpandable ()),
			fOldArcLength (doc.GetArcLength ()),
			fNewArcLength (query.GetArcLength ()),
			fOldSpacing (doc.GetSpacing ()),
			fNewSpacing (query.GetSpacing ()),
			fNodeView (nodeView)
		{
		}
		
		override	void	DoIt ()
		{
			fDocument.SetDirection (fNewDirection);
			fDocument.SetExpandable (fNewExpandable);
			fDocument.SetArcLength (fNewArcLength);
			fDocument.SetSpacing (fNewSpacing);
			fNodeView.InvalidateLayout ();
			fNodeView.Update ();
			
			Command::DoIt ();
		}

		override	void	UnDoIt ()
		{
			fDocument.SetDirection (fOldDirection);
			fDocument.SetExpandable (fOldExpandable);
			fDocument.SetArcLength (fOldArcLength);
			fDocument.SetSpacing (fOldSpacing);
			fNodeView.InvalidateLayout ();
			fNodeView.Update ();
			
			Command::UnDoIt ();
		}
	
	private:
		NodeView&			fNodeView;
		NodeViewDocument&	fDocument;
		Point::Direction	fNewDirection;
		Point::Direction	fOldDirection;
		Boolean				fNewExpandable;
		Boolean				fOldExpandable;
		Point				fNewArcLength;
		Point				fOldArcLength;
		Point				fNewSpacing;
		Point				fOldSpacing;
};

Boolean		NodeViewWindow::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eConfigureNodes: 
			{
				ConfigureNodeView query = ConfigureNodeView (fNodeView->GetBranchNode ());
				Dialog d = Dialog (&query, &query, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
				d.SetDefaultButton (d.GetOKButton ());
				if (d.Pose ()) {
					PostCommand (new ConfigureNodeCommand (query, fDocument, fNodeView->GetBranchNode ()));
					PostCommand (new DocumentDirtier (fDocument));
				}
			}
			return (True);

		default:
			return (Window::DoCommand (selection));
	}
	AssertNotReached (); return (True);
}



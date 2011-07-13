/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__NodeViewWindow__
#define	__NodeViewWindow__

/*
 * $Header: /fuji/lewis/RCS/NodeViewWindow.hh,v 1.2 1992/09/01 17:30:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: NodeViewWindow.hh,v $
 *		Revision 1.2  1992/09/01  17:30:27  sterling
 *		*** empty log message ***
 *
 *
 *
 */

#include	"Window.hh"

class	NodeViewDocument;
class	DemoTrunkNode;
class	NodeViewWindow : public Window {
	public:
		NodeViewWindow (NodeViewDocument& doc);

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);
		
		nonvirtual	DemoTrunkNode&	GetNodeView () const;

	private:
		DemoTrunkNode*		fNodeView;
		NodeViewDocument&	fDocument;
};



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__NodeViewWindow__*/


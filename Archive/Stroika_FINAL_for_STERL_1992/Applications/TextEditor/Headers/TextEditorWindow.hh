/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TextEditorWindow__
#define	__TextEditorWindow__

/*
 * $Header: /fuji/lewis/RCS/TextEditorWindow.hh,v 1.2 1992/09/01 17:55:02 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: TextEditorWindow.hh,v $
 *		Revision 1.2  1992/09/01  17:55:02  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  1992/06/20  18:37:23  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/03/26  17:57:13  sterling
 *		Initial revision
 *
 *
 *
 *
 */

#include	"Window.hh"

class TextEditorDocument;
class AbstractTextEdit;
class	TextEditorWindow : public Window {
	public:
		TextEditorWindow (TextEditorDocument& myDocument);
		~TextEditorWindow ();

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);

		nonvirtual	AbstractTextEdit&	GetTE () const;

	private:
		AbstractTextEdit*	fTE;
		class TextScroller* fScroller;
};




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__TextEditorWindow__*/


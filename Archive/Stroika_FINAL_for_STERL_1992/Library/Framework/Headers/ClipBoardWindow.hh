/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ClipBoardWindow__
#define	__ClipBoardWindow__

/*
 * $Header: /fuji/lewis/RCS/ClipBoardWindow.hh,v 1.2 1992/07/02 23:47:48 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: ClipBoardWindow.hh,v $
 *		Revision 1.2  1992/07/02  23:47:48  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/03/26  09:36:15  lewis
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *
 *		Revision 1.6  1992/02/15  04:50:37  sterling
 *		added back removing idle task when invisible
 *
 *		Revision 1.5  1991/12/27  16:48:53  lewis
 *		Use window instead of TitledWindow.
 *
 *		Revision 1.4  1991/12/12  02:59:29  lewis
 *		Got rid of dependency on ScrollerWindow.
 *
 *
 *
 */

#include	"View.hh"
#include	"Window.hh"


class	ClipCountChecker;
class	PictView;
class	Scroller;
class	ClipBoardWindow : public Window {
	public:
		ClipBoardWindow ();
		virtual	~ClipBoardWindow ();

	protected:
		override	void	EffectiveVisibilityChanged (Boolean newVisible, Panel::UpdateMode updateMode);

	private:
		Scroller*			fScroller;
		ClipCountChecker*	fClipCountChecker;
		PictView*			fPictView;
		UInt32				fLastScrapChangeCount;

		nonvirtual	void	CheckClipState ();

	friend	class	ClipCountChecker;
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



#endif	/*__ClipBoardWindow__*/


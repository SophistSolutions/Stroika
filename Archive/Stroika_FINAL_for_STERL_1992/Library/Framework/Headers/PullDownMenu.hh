/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PullDownMenu__
#define	__PullDownMenu__

/*
 * $Header: /fuji/lewis/RCS/PullDownMenu.hh,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: PullDownMenu.hh,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:49:53  sterling
 *		Got rid of include of sequence_Dll.hh
 *
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/02  04:36:19  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  06:18:35  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_ (+more?).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.10  1992/04/07  15:58:45  sterling
 *		CalcDefaultSize for Motif
 *
 *		Revision 1.9  92/03/26  18:34:29  18:34:29  lewis (Lewis Pringle)
 *		Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *		
 *		Revision 1.7  1992/03/19  16:56:39  lewis
 *		Got rid of hack override of SetTitle_.
 *
 *		Revision 1.6  1992/03/17  17:04:55  lewis
 *		Hacks to try to get emily menu bar support working - undo and cleanup!!!
 *
 *		Revision 1.5  1992/03/09  23:56:30  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.4  1992/03/05  17:52:04  lewis
 *		Add class PullDownMenuTitle instead of #define.
 *		In PullDownMenuTitle_MacUI_Portable:  override Draw, and CalcDefaultSize() so you can place theys guys in Views,
 *		and get resonable results (still not really working yet - but closer).
 *		Got rid of SequencePtr (PullDownMenuTitle_MotifUIPtr) declaration since unused.
 *
 *		Revision 1.3  1992/02/27  21:35:22  lewis
 *		Got rid of MenuTitle_Mac_Native - moved relevant mac specific functionality into MenuTitle.
 *		PullDownMenuTitle_MotifUI and PullDownMenuTitle_MacUI_Portable no longer inherit from View, and EnableView respectively since
 *		that functionality is inherited in a common base.
 *
 *		Revision 1.2  1992/02/15  06:49:56  lewis
 *		Dont need fNativeMenu anymore.
 *
 *		Revision 1.1  1992/02/15  00:41:19  lewis
 *		Initial revision
 *
 *
 */

#include	"Button.hh"
#include	"EnableItem.hh"
#include	"MenuTitle.hh"




class	Menu;
class	AbstractPullDownMenuTitle : public MenuTitle, public ButtonController {
	protected:
		AbstractPullDownMenuTitle (const String& title, Menu* menu, ButtonController* controller);

		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected) = Nil;
};




#if		qMacToolkit
class	PullDownMenuTitle_MacUI_Portable : public AbstractPullDownMenuTitle {
	public:
		PullDownMenuTitle_MacUI_Portable (const String& title, ButtonController* controller = Nil);
		PullDownMenuTitle_MacUI_Portable (const String& title, Menu* menu, ButtonController* controller = Nil);

	protected:
		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected);

	/*
	 * We don't actually get to the menu title draw routine in most native situtations right now, though we should.
	 * LGP - March 4, 1992
	 */
	protected:
		override	void	Draw (const Region& update);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

	private:
		class AbstractPushButton*	fButton;
};


#elif   qXmToolkit


struct	osWidget;
class	PullDownMenuTitle_MotifUI : public AbstractPullDownMenuTitle {
	public:
		PullDownMenuTitle_MotifUI (const String& title, Menu* menu = Nil);
		~PullDownMenuTitle_MotifUI ();

 	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected);

		override 	void		Realize (osWidget* parent);
		override 	void		UnRealize ();
		override	osWidget*	GetWidget () const;

	protected:
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

	private:
		osWidget* 	fWidget;
};


#endif	/*Toolkit*/




class	PullDownMenuTitle : public
#if		qMacToolkit
	PullDownMenuTitle_MacUI_Portable
#elif	qXmToolkit
	PullDownMenuTitle_MotifUI
#endif
	{
		public:
			PullDownMenuTitle (const String& title, Menu* menu = Nil);
};





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__PullDownMenu__*/


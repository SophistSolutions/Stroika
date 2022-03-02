/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PopUpMenu__
#define	__PopUpMenu__

/*
 * $Header: /fuji/lewis/RCS/PopUpMenu.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * PopUpMenuTitle		-- LabelAdornment, MenuTitle, keeps old MenuSelection	(Mac PopUpMenu, Motif OptionMenu)
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: PopUpMenu.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/06/25  06:16:15  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_ (+more?).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.11  1992/05/18  15:53:39  lewis
 *		Made a few private methods const.
 *
 *		Revision 1.10  92/04/30  13:23:43  13:23:43  sterling (Sterling Wight)
 *		overrode GetLive to ensure that EffectiveLiveChanged is always called on Add or removesubview
 *		
 *		Revision 1.9  92/03/26  18:33:16  18:33:16  lewis (Lewis Pringle)
 *		Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *		
 *		Revision 1.8  1992/03/26  09:36:15  lewis
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *
 *		Revision 1.6  1992/03/09  23:56:08  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.5  1992/02/28  16:26:03  lewis
 *		Override GetLive to disambiguate for motif popups.
 *
 *		Revision 1.4  1992/02/27  21:31:32  lewis
 *		PopUpMenuTitle_MacUI_Portable no longer inherits from EnableView since it
 *		gets EnableItem, and View from base classes.
 *
 *		Revision 1.3  1992/02/15  06:58:18  sterling
 *		use EffectiveLiveChanged
 *
 *		Revision 1.2  1992/02/15  06:47:25  lewis
 *		Added class PopUpMenuTitle instead of #defines, and tried to rationalize their seconds args. Decided
 *		what we did for mac was better (sterl says so), and later we must fix usage of second arg in
 *		motif version.
 *
 *		Revision 1.1  1992/02/15  00:41:19  lewis
 *		Initial revision
 *
 *
 *
 */


#include	"Button.hh"
#include	"EnableItem.hh"
#include	"MenuTitle.hh"





class	AbstractPopUpMenuTitle : public MenuTitle, public ButtonController {
	protected:
		AbstractPopUpMenuTitle (const String& title, Menu* menu);
	
	public:
		nonvirtual	MenuItem*	GetCurrentItem () const;
		nonvirtual	void		SetCurrentItem (MenuItem* item);
		
		override	Boolean	GetLive () const;

	protected:
		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected) = Nil;
		override	void	HandleMenuSelection (const MenuItem& item);
	
		virtual		void	SetCurrentItem_ (MenuItem* item);
	
	private:
		MenuItem*	fCurrentItem;
};




#if		qMacToolkit
class	PopUpMenuTitle_MacUI_Portable : public AbstractPopUpMenuTitle {
	public:
		PopUpMenuTitle_MacUI_Portable (const String& title, ButtonController* controller);
		~PopUpMenuTitle_MacUI_Portable ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		override	void	Draw (const Region& update);

		nonvirtual	Boolean	GetAutoResize () const;
		nonvirtual	void	SetAutoResize (Boolean autoResize);
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected);
		override	void	HandleMenuSelection (const MenuItem& item);
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

	private:
		Boolean			fAutoResize;

		nonvirtual	Coordinate	GetItemOffset () const;
		nonvirtual	Rect		CalcMenuRect () const;
		nonvirtual	void		DrawPopUpBox ();
		nonvirtual	String		GetCurrentItemText () const;
};


#elif	qXmToolkit


class	MotifOSControl;
class	PopUpMenuTitle_MotifUI : public AbstractPopUpMenuTitle, public FocusItem {
	public:
		PopUpMenuTitle_MotifUI (const String& title, Menu* menu);
		~PopUpMenuTitle_MotifUI ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	// since both AbstractPopUpMenuTitle and FocusItem are liveitems we need to disambiguate - really both calls same...
	// also, not too sure why FocusItem is a liveitem???
		nonvirtual	Boolean	GetLive () const { return (AbstractPopUpMenuTitle::GetLive ()); }

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected);
		override	void	Layout ();
		override	void	SetTitle_ (const String& title);
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);
		override	void	Realize (osWidget* parent);
		override	void	UnRealize ();

	private:
		MotifOSControl*	fOSControl;

		nonvirtual	String		GetCurrentItemText () const;
		nonvirtual	Coordinate	GetItemOffset () const;
		nonvirtual	Rect		CalcMenuRect () const;
};
#endif	/*Toolkit*/




class	PopUpMenuTitle : public
#if		qMacToolkit
	PopUpMenuTitle_MacUI_Portable
#elif	qXmToolkit
	PopUpMenuTitle_MotifUI
#endif	/*Toolkit*/
{
	public:
		PopUpMenuTitle (const String& title, ButtonController* buttonController = Nil);
};






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__PopUpMenu__*/


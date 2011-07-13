/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ButtonItem__
#define	__ButtonItem__

/*
 * $Header: /fuji/lewis/RCS/ButtonItem.hh,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ButtonItem.hh,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

#include	"ViewItem.hh"

class	Button;
class	ButtonItem : public ViewItem {
	protected:
		ButtonItem (ItemType& type);

		virtual	void	SetButton (Button* button);

	public:
		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
	
		override	String	GetHeaderFileName () = Nil;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	SetOwnedEnabled (Boolean enabled, UpdateMode updateMode);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
	
	private:
		Button*	fButton;
		Boolean	fDefaultOn;
		Boolean	fDefaultCanStayOn;
};

#endif	/* __ButtonItem__ */

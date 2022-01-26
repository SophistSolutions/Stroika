/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextViewInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextViewInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "TextView.hh"
#include "TextEdit.hh"
#include "CheckBox.hh"
#include "ViewItemInfo.hh"


class TextViewInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		TextViewInfoX ();
		~TextViewInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		TextEdit			fText;
		CheckBox			fWordWrap;
		TextView			fTextLabel;
		ViewItemInfo		fViewInfo;

	private:
#if   qMacUI
		nonvirtual void	BuildForMacUI ();
#elif qMotifUI
		nonvirtual void	BuildForMotifUI ();
#else
		nonvirtual void	BuildForUnknownGUI ();
#endif /* GUI */

};



// text past here will be retained: Do not remove or modify this line!!!
class	TextViewItem;

class TextViewInfo : public TextViewInfoX {
	public:
		TextViewInfo (TextViewItem& view);

		nonvirtual	CheckBox&	GetWordWrapField ()
		{
			return (fWordWrap);
		}
		
		nonvirtual	TextEdit&	GetTextField ()
		{
			return (fText);
		}
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
};


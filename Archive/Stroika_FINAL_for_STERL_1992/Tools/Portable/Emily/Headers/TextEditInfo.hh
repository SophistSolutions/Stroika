/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextEditInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "NumberText.hh"
#include "TextEdit.hh"
#include "CheckBox.hh"
#include "TextView.hh"
#include "ViewItemInfo.hh"


class TextEditInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		TextEditInfoX ();
		~TextEditInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		NumberText			fMaxLength;
		TextEdit			fText;
		CheckBox			fHorizontalSBar;
		CheckBox			fRequireText;
		TextView			fTitle;
		TextView			fField1;
		TextView			fField2;
		CheckBox			fMultiLine;
		CheckBox			fVerticalSBar;
		CheckBox			fWordWrap;
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
class	TextEditItem;

class TextEditInfo : public TextEditInfoX {
	public:
		TextEditInfo (TextEditItem& view);

		nonvirtual	CheckBox&	GetWordWrapField ()
		{
			return (fWordWrap);
		}
		
		nonvirtual	CheckBox&	GetRequireTextField ()
		{
			return (fRequireText);
		}
		
		nonvirtual	CheckBox&	GetMultiLineField ()
		{
			return (fMultiLine);
		}
		
		nonvirtual	CheckBox&	GetVSBarField ()
		{
			return (fVerticalSBar);
		}
		
		nonvirtual	CheckBox&	GetHSBarField ()
		{
			return (fHorizontalSBar);
		}
				
		nonvirtual	NumberText&		GetMaxLengthField ()
		{
			return (fMaxLength);
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


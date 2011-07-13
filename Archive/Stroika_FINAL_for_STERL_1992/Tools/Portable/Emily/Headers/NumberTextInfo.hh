/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/NumberTextInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: NumberTextInfo.hh,v $
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
#include "ViewItemInfo.hh"
#include "NumberText.hh"
#include "CheckBox.hh"


class NumberTextInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		NumberTextInfoX ();
		~NumberTextInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		TextView			fValueLabel;
		TextView			fMaximumLabel;
		TextView			fPrecisionLabel;
		TextView			fMinimumLabel;
		ViewItemInfo		fViewInfo;
		NumberText			fPrecision;
		CheckBox			fMultiLine;
		CheckBox			fWordWrap;
		CheckBox			fRequireText;
		NumberText			fMinimum;
		NumberText			fMaximum;
		NumberText			fDefault;

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
class	NumberTextItem;
class NumberTextInfo : public NumberTextInfoX {
	public:
		NumberTextInfo (NumberTextItem& view);

		nonvirtual	CheckBox&	GetRequireTextField ()
		{
			return (fRequireText);
		}
		
		nonvirtual	NumberText&	GetPrecisionField ()
		{
			return (fPrecision);
		}
		
		nonvirtual	NumberText&	GetMaxValueField ()
		{
			return (fMaximum);
		}
		
		nonvirtual	NumberText&	GetMinValueField ()
		{
			return (fMinimum);
		}
		
		nonvirtual	NumberText&	GetDefaultField ()
		{
			return (fDefault);
		}
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
};


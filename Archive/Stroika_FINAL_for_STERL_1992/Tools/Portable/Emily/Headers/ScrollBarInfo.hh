/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScrollBarInfo.hh,v 1.2 1992/07/21 18:33:08 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollBarInfo.hh,v $
 *		Revision 1.2  1992/07/21  18:33:08  sterling
 *		added
 *
# Revision 1.1  1992/07/16  15:27:55  sterling
# hi
#
 *
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "TextView.hh"
#include "GroupView.hh"
#include "RadioButton.hh"
#include "NumberText.hh"
#include "ViewItemInfo.hh"


class ScrollBarInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		ScrollBarInfoX ();
		~ScrollBarInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		TextView			fMaximumLabel;
		LabeledGroup		fField1;
		RadioBank			fField1RadioBank;
		FocusOwner			fField1TabLoop;
		RadioButton			fVertical;
		RadioButton			fHorizontal;
		NumberText			fMaximum;
		NumberText			fValue;
		TextView			fMinLabel;
		TextView			fValueLabel;
		TextView			fStepSizeLabel;
		TextView			fPageSizeLabel;
		NumberText			fStepSize;
		NumberText			fPageSize;
		NumberText			fMinimum;
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
class	ScrollBarItem;

class ScrollBarInfo : public ScrollBarInfoX {
	public:
		ScrollBarInfo (ScrollBarItem& view);

		nonvirtual	NumberText&	GetMaxValueField ()
		{
			return (fMaximum);
		}
		
		nonvirtual	NumberText&	GetMinValueField ()
		{
			return (fMinimum);
		}
		
		nonvirtual	NumberText&	GetValueField ()
		{
			return (fValue);
		}
		
		nonvirtual	NumberText&	GetStepSizeField ()
		{
			return (fStepSize);
		}
		
		nonvirtual	NumberText&	GetPageSizeField ()
		{
			return (fPageSize);
		}
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
		
		nonvirtual	AbstractScrollBar::Orientation	GetOrientation () const
		{
			return ((fVertical.GetOn ()) ? AbstractScrollBar::eVertical : AbstractScrollBar::eHorizontal);
		}
};


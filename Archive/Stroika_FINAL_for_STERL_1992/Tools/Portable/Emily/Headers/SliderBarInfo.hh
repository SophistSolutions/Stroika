/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SliderBarInfo.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderBarInfo.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "TextView.hh"
#include "NumberText.hh"
#include "ViewItemInfo.hh"


class SliderBarInfoX : public View, public FocusOwner, public TextController {
	public:
		SliderBarInfoX ();
		~SliderBarInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		NumberText			fSubTicks;
		NumberText			fTicks;
		TextView			fField1;
		TextView			fField2;
		NumberText			fMinimum;
		NumberText			fValue;
		TextView			fMaximumLabel;
		TextView			fMinLabel;
		TextView			fValueLabel;
		NumberText			fMaximum;
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
class	SliderBarItem;

class SliderBarInfo : public SliderBarInfoX {
	public:
		SliderBarInfo (SliderBarItem& view);

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
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}

		nonvirtual	Real	GetTicks ()
		{
			return (fTicks.GetValue ());
		}

		nonvirtual	Real	GetSubTicks ()
		{
			return (fSubTicks.GetValue ());
		}
};


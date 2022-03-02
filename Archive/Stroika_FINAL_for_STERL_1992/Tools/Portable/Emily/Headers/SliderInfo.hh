/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SliderInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
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


class SliderInfoX : public View, public FocusOwner, public TextController {
	public:
		SliderInfoX ();
		~SliderInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		TextView			fMaximumLabel;
		TextView			fMinLabel;
		TextView			fValueLabel;
		NumberText			fMinimum;
		NumberText			fMaximum;
		NumberText			fValue;
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
class	SliderItem;

class SliderInfo : public SliderInfoX {
	public:
		SliderInfo (SliderItem& view);

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
};


/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScaleInfo.hh,v 1.2 1992/07/21 18:33:08 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScaleInfo.hh,v $
 *		Revision 1.2  1992/07/21  18:33:08  sterling
 *		added
 *
# Revision 1.1  1992/07/16  15:27:55  sterling
# hi
#
 *		
 *
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "CheckBox.hh"
#include "NumberText.hh"
#include "TextView.hh"
#include "ViewItemInfo.hh"


class ScaleInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		ScaleInfoX ();
		~ScaleInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		CheckBox			fSensitive;
		NumberText			fThumbLength;
		TextView			fThumbLengthLabel;
		TextView			fValueLabel;
		NumberText			fValue;
		TextView			fTitle;
		TextView			fMaximumLabel;
		TextView			fMinLabel;
		NumberText			fMinimum;
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
class	ScaleItem;

class ScaleInfo : public ScaleInfoX {
	public:
		ScaleInfo (ScaleItem& view);

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
		
		nonvirtual	Boolean	GetSensitive () const
		{
			return (fSensitive.GetOn ());
		}
		
		nonvirtual	Coordinate	GetThumbLength () const
		{
			return ((Coordinate) fThumbLength.GetValue ());
		}
};


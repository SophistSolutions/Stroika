/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapButtonInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapButtonInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.12  92/04/08  17:21:57  17:21:57  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif.
 *		
 *
 *
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "View.hh"

#include "TextView.hh"
#include "PushButton.hh"
#include "PixelMapButton.hh"
#include "ViewItemInfo.hh"


class PixelMapButtonInfoX : public View, public ButtonController, public FocusOwner {
	public:
		PixelMapButtonInfoX ();
		~PixelMapButtonInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		PushButton			fEditPixelMap;
		PixelMapButton		fPixelMapDisplay;
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
class	PixelMapButtonItem;

class PixelMapButtonInfo : public PixelMapButtonInfoX {
	public:
		PixelMapButtonInfo (PixelMapButtonItem& view);

		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}

		nonvirtual	PixelMap	GetPixelMap () const;
	
	protected:
		override	void	ButtonPressed (AbstractButton* button);
};


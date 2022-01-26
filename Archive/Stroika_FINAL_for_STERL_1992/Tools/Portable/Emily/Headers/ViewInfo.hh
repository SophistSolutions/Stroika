/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ViewInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ViewInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "FocusItem.hh"
#include "View.hh"

#include "TextView.hh"
#include "ViewItemInfo.hh"


class ViewInfoX : public View, public FocusOwner {
	public:
		ViewInfoX ();
		~ViewInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
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

#include	"FocusItem.hh"

class ViewInfo : public ViewInfoX {
	public:
		ViewInfo (ViewItem& view);

		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
};


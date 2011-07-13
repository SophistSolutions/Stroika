/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/LabeledGroupInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: LabeledGroupInfo.hh,v $
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

#include "TextEdit.hh"
#include "TextView.hh"
#include "ViewItemInfo.hh"


class LabeledGroupInfoX : public GroupView, public FocusOwner, public TextController {
	public:
		LabeledGroupInfoX ();
		~LabeledGroupInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextEdit			fLabel;
		TextView			fLabelLabel;
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
class	LabeledGroupItem;
class LabeledGroupInfo : public LabeledGroupInfoX {
	public:
		LabeledGroupInfo (LabeledGroupItem& view);
		
	nonvirtual	TextEdit&	LabeledGroupInfo::GetLabelField ()
	{
		return (fLabel);
	}
	
	nonvirtual	ViewItemInfo&	GetViewItemInfo ()
	{
		return (fViewInfo);
	}
};


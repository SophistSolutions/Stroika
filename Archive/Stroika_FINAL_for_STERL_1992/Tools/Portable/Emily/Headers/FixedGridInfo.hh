/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FixedGridInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: FixedGridInfo.hh,v $
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

#include "TextView.hh"
#include "GroupView.hh"
#include "RadioButton.hh"
#include "NumberText.hh"
#include "ViewItemInfo.hh"
#include "CheckBox.hh"


class FixedGridInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		FixedGridInfoX ();
		~FixedGridInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		LabeledGroup		fAlignGroup;
		RadioBank			fAlignGroupRadioBank;
		FocusOwner			fAlignGroupTabLoop;
		RadioButton			fAlignLeft;
		RadioButton			fAlignCenter;
		RadioButton			fAlignRight;
		TextView			fField1;
		TextView			fField2;
		TextView			fColCountLabel;
		TextView			fRowCountLabel;
		TextView			fColGapLabel;
		TextView			fRowGapLabel;
		NumberText			fColumnGap;
		NumberText			fRowGap;
		NumberText			fRowCount;
		ViewItemInfo		fViewInfo;
		CheckBox			fDrawGridLines;
		CheckBox			fForceSize;
		NumberText			fColumnCount;
		NumberText			fRowHeight;
		NumberText			fColumnWidth;

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
class GridItem;
class FixedGridInfo : public FixedGridInfoX {
	public:
		FixedGridInfo (GridItem& view);

		nonvirtual	AbstractTextView::Justification	GetJustification () const;

		nonvirtual	NumberText&	GetColumnCountField ()
		{
			return (fColumnCount);
		}
		
		nonvirtual	NumberText&	GetRowCountField ()
		{
			return (fRowCount);
		}
		
		nonvirtual	NumberText&	GetColumnGapField ()
		{
			return (fColumnGap);
		}
		
		nonvirtual	NumberText&	GetRowGapField ()
		{
			return (fRowGap);
		}
		
		nonvirtual	NumberText&	GetColumnWidthField ()
		{
			return (fColumnWidth);
		}
		
		nonvirtual	NumberText&	GetRowHeightField ()
		{
			return (fRowHeight);
		}
		
		nonvirtual	CheckBox&	GetGridLinesField ()
		{
			return (fDrawGridLines);
		}
		
		nonvirtual	CheckBox&	GetForceSizeField ()
		{
			return (fForceSize);
		}

		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
};


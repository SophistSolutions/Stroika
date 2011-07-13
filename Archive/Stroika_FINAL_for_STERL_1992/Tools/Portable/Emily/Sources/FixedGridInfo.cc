/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FixedGridInfo.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: FixedGridInfo.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.15  1992/05/19  11:35:47  sterling
 *		hi
 *
 *		Revision 1.14  92/05/13  18:47:05  18:47:05  lewis (Lewis Pringle)
 *		STERL.
 *		
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "FixedGridInfo.hh"


FixedGridInfoX::FixedGridInfoX () :
	fTitle (),
	fAlignGroup (kEmptyString),
	fField1 (),
	fField2 (),
	fColCountLabel (),
	fRowCountLabel (),
	fColGapLabel (),
	fRowGapLabel (),
	fColumnGap (),
	fRowGap (),
	fRowCount (),
	fViewInfo (),
	fDrawGridLines (),
	fForceSize (),
	fColumnCount (),
	fRowHeight (),
	fColumnWidth ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

FixedGridInfoX::~FixedGridInfoX ()
{
	RemoveFocus (&fColumnCount);
	RemoveFocus (&fRowCount);
	RemoveFocus (&fColumnGap);
	RemoveFocus (&fRowGap);
	RemoveFocus (&fColumnWidth);
	RemoveFocus (&fRowHeight);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fAlignGroup);
		fAlignGroup.RemoveSubView (&fAlignLeft);
		fAlignGroupRadioBank.RemoveRadioButton (&fAlignLeft);
		fAlignGroup.RemoveSubView (&fAlignCenter);
		fAlignGroupRadioBank.RemoveRadioButton (&fAlignCenter);
		fAlignGroup.RemoveSubView (&fAlignRight);
		fAlignGroupRadioBank.RemoveRadioButton (&fAlignRight);
	RemoveSubView (&fField1);
	RemoveSubView (&fField2);
	RemoveSubView (&fColCountLabel);
	RemoveSubView (&fRowCountLabel);
	RemoveSubView (&fColGapLabel);
	RemoveSubView (&fRowGapLabel);
	RemoveSubView (&fColumnGap);
	RemoveSubView (&fRowGap);
	RemoveSubView (&fRowCount);
	RemoveSubView (&fViewInfo);
	RemoveSubView (&fDrawGridLines);
	RemoveSubView (&fForceSize);
	RemoveSubView (&fColumnCount);
	RemoveSubView (&fRowHeight);
	RemoveSubView (&fColumnWidth);
}

#if   qMacUI

void	FixedGridInfoX::BuildForMacUI ()
{
	SetSize (Point (330, 359), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 350, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Fixed Grid Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fAlignGroup.SetExtent (23, 260, 79, 72, eNoUpdate);
	fAlignGroup.SetBorder (1, 1, eNoUpdate);
	fAlignGroup.SetMargin (1, 1, eNoUpdate);
	fAlignGroup.SetFont (&kSystemFont);
	fAlignGroup.SetLabel ("Align", eNoUpdate);
		fAlignGroupRadioBank.SetController (this);

		fAlignLeft.SetLabel ("Left", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignLeft);
		fAlignLeft.SetExtent (22, 2, 14, 44, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignLeft);

		fAlignCenter.SetLabel ("Center", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignCenter);
		fAlignCenter.SetExtent (41, 2, 14, 61, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignCenter);

		fAlignRight.SetLabel ("Right", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignRight);
		fAlignRight.SetExtent (60, 2, 14, 51, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignRight);

	AddSubView (&fAlignGroup);

	fField1.SetExtent (23, 122, 15, 90, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Column Width");
	AddSubView (&fField1);

	fField2.SetExtent (44, 137, 16, 75, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Row Height");
	AddSubView (&fField2);

	fColCountLabel.SetExtent (23, 1, 15, 87, eNoUpdate);
	fColCountLabel.SetFont (&kSystemFont);
	fColCountLabel.SetText ("# of Columns");
	AddSubView (&fColCountLabel);

	fRowCountLabel.SetExtent (43, 20, 15, 67, eNoUpdate);
	fRowCountLabel.SetFont (&kSystemFont);
	fRowCountLabel.SetText ("# of Rows");
	AddSubView (&fRowCountLabel);

	fColGapLabel.SetExtent (65, 11, 15, 76, eNoUpdate);
	fColGapLabel.SetFont (&kSystemFont);
	fColGapLabel.SetText ("Column Gap");
	AddSubView (&fColGapLabel);

	fRowGapLabel.SetExtent (86, 31, 15, 56, eNoUpdate);
	fRowGapLabel.SetFont (&kSystemFont);
	fRowGapLabel.SetText ("Row Gap");
	AddSubView (&fRowGapLabel);

	fColumnGap.SetExtent (85, 88, 17, 31, eNoUpdate);
	fColumnGap.SetFont (&kApplicationFont);
	fColumnGap.SetController (this);
	fColumnGap.SetMultiLine (False);
	fColumnGap.SetMinValue (0);
	AddSubView (&fColumnGap);

	fRowGap.SetExtent (64, 88, 17, 31, eNoUpdate);
	fRowGap.SetFont (&kApplicationFont);
	fRowGap.SetController (this);
	fRowGap.SetMultiLine (False);
	fRowGap.SetMinValue (0);
	AddSubView (&fRowGap);

	fRowCount.SetExtent (43, 88, 16, 31, eNoUpdate);
	fRowCount.SetFont (&kApplicationFont);
	fRowCount.SetController (this);
	fRowCount.SetMultiLine (False);
	fRowCount.SetMaxValue (100);
	fRowCount.SetMinValue (1);
	AddSubView (&fRowCount);

	fViewInfo.SetExtent (108, 4, 219, 352, eNoUpdate);
	AddSubView (&fViewInfo);

	fDrawGridLines.SetLabel ("Grid Lines", eNoUpdate);
	fDrawGridLines.SetExtent (84, 135, 14, 82, eNoUpdate);
	fDrawGridLines.SetController (this);
	AddSubView (&fDrawGridLines);

	fForceSize.SetLabel ("Force Size", eNoUpdate);
	fForceSize.SetExtent (65, 135, 14, 84, eNoUpdate);
	fForceSize.SetController (this);
	AddSubView (&fForceSize);

	fColumnCount.SetExtent (23, 89, 16, 30, eNoUpdate);
	fColumnCount.SetFont (&kApplicationFont);
	fColumnCount.SetController (this);
	fColumnCount.SetMultiLine (False);
	fColumnCount.SetMaxValue (100);
	fColumnCount.SetMinValue (1);
	AddSubView (&fColumnCount);

	fRowHeight.SetExtent (43, 213, 17, 33, eNoUpdate);
	fRowHeight.SetFont (&kApplicationFont);
	fRowHeight.SetController (this);
	fRowHeight.SetMultiLine (False);
	fRowHeight.SetMinValue (0);
	AddSubView (&fRowHeight);

	fColumnWidth.SetExtent (22, 213, 17, 33, eNoUpdate);
	fColumnWidth.SetFont (&kApplicationFont);
	fColumnWidth.SetController (this);
	fColumnWidth.SetMultiLine (False);
	fColumnWidth.SetMinValue (0);
	AddSubView (&fColumnWidth);

	AddFocus (&fColumnCount);
	AddFocus (&fRowCount);
	AddFocus (&fColumnGap);
	AddFocus (&fRowGap);
	AddFocus (&fColumnWidth);
	AddFocus (&fRowHeight);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	FixedGridInfoX::BuildForMotifUI ()
{
	SetSize (Point (371, 359), eNoUpdate);

	fTitle.SetExtent (5, 5, 21, 349, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Fixed Grid Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fAlignGroup.SetExtent (23, 260, 103, 81, eNoUpdate);
	fAlignGroup.SetBorder (1, 1, eNoUpdate);
	fAlignGroup.SetMargin (1, 1, eNoUpdate);
	fAlignGroup.SetFont (&kSystemFont);
	fAlignGroup.SetLabel ("Align", eNoUpdate);
		fAlignGroupRadioBank.SetController (this);

		fAlignLeft.SetLabel ("Left", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignLeft);
		fAlignLeft.SetExtent (22, 2, 25, 59, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignLeft);

		fAlignCenter.SetLabel ("Center", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignCenter);
		fAlignCenter.SetExtent (48, 2, 25, 75, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignCenter);

		fAlignRight.SetLabel ("Right", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignRight);
		fAlignRight.SetExtent (74, 2, 25, 65, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignRight);

		fAlignGroupTabLoop.AddFocus (&fAlignLeft);
		fAlignGroupTabLoop.AddFocus (&fAlignCenter);
		fAlignGroupTabLoop.AddFocus (&fAlignRight);
	AddSubView (&fAlignGroup);

	fField1.SetExtent (38, 119, 21, 94, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Column Width");
	AddSubView (&fField1);

	fField2.SetExtent (65, 138, 21, 75, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Row Height");
	AddSubView (&fField2);

	fColCountLabel.SetExtent (39, 8, 21, 80, eNoUpdate);
	fColCountLabel.SetFont (&kSystemFont);
	fColCountLabel.SetText ("# of Columns");
	AddSubView (&fColCountLabel);

	fRowCountLabel.SetExtent (66, 25, 21, 61, eNoUpdate);
	fRowCountLabel.SetFont (&kSystemFont);
	fRowCountLabel.SetText ("# of Rows");
	AddSubView (&fRowCountLabel);

	fColGapLabel.SetExtent (91, 9, 21, 78, eNoUpdate);
	fColGapLabel.SetFont (&kSystemFont);
	fColGapLabel.SetText ("Column Gap");
	AddSubView (&fColGapLabel);

	fRowGapLabel.SetExtent (120, 25, 21, 61, eNoUpdate);
	fRowGapLabel.SetFont (&kSystemFont);
	fRowGapLabel.SetText ("Row Gap");
	AddSubView (&fRowGapLabel);

	fColumnGap.SetExtent (118, 88, 26, 31, eNoUpdate);
	fColumnGap.SetFont (&kApplicationFont);
	fColumnGap.SetController (this);
	fColumnGap.SetMultiLine (False);
	fColumnGap.SetMinValue (0);
	AddSubView (&fColumnGap);

	fRowGap.SetExtent (91, 88, 26, 31, eNoUpdate);
	fRowGap.SetFont (&kApplicationFont);
	fRowGap.SetController (this);
	fRowGap.SetMultiLine (False);
	fRowGap.SetMinValue (0);
	AddSubView (&fRowGap);

	fRowCount.SetExtent (64, 89, 26, 30, eNoUpdate);
	fRowCount.SetFont (&kApplicationFont);
	fRowCount.SetController (this);
	fRowCount.SetMultiLine (False);
	fRowCount.SetMaxValue (100);
	fRowCount.SetMinValue (1);
	AddSubView (&fRowCount);

	fViewInfo.SetExtent (149, 2, 219, 352, eNoUpdate);
	AddSubView (&fViewInfo);

	fDrawGridLines.SetLabel ("Grid Lines", eNoUpdate);
	fDrawGridLines.SetExtent (119, 137, 25, 92, eNoUpdate);
	fDrawGridLines.SetController (this);
	AddSubView (&fDrawGridLines);

	fForceSize.SetLabel ("Force Size", eNoUpdate);
	fForceSize.SetExtent (93, 137, 25, 94, eNoUpdate);
	fForceSize.SetController (this);
	AddSubView (&fForceSize);

	fColumnCount.SetExtent (37, 89, 26, 30, eNoUpdate);
	fColumnCount.SetFont (&kApplicationFont);
	fColumnCount.SetController (this);
	fColumnCount.SetMultiLine (False);
	fColumnCount.SetMaxValue (100);
	fColumnCount.SetMinValue (1);
	AddSubView (&fColumnCount);

	fRowHeight.SetExtent (63, 213, 26, 33, eNoUpdate);
	fRowHeight.SetFont (&kApplicationFont);
	fRowHeight.SetController (this);
	fRowHeight.SetMultiLine (False);
	fRowHeight.SetMinValue (0);
	AddSubView (&fRowHeight);

	fColumnWidth.SetExtent (36, 213, 26, 33, eNoUpdate);
	fColumnWidth.SetFont (&kApplicationFont);
	fColumnWidth.SetController (this);
	fColumnWidth.SetMultiLine (False);
	fColumnWidth.SetMinValue (0);
	AddSubView (&fColumnWidth);

	AddFocus (&fColumnCount);
	AddFocus (&fRowCount);
	AddFocus (&fColumnGap);
	AddFocus (&fRowGap);
	AddFocus (&fColumnWidth);
	AddFocus (&fRowHeight);
	AddFocus (&fDrawGridLines);
	AddFocus (&fForceSize);
	AddFocus (&fAlignGroupTabLoop);
	AddFocus (&fViewInfo);
}

#else

void	FixedGridInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (330, 359), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 350, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Fixed Grid Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fAlignGroup.SetExtent (23, 260, 79, 72, eNoUpdate);
	fAlignGroup.SetBorder (1, 1, eNoUpdate);
	fAlignGroup.SetMargin (1, 1, eNoUpdate);
	fAlignGroup.SetFont (&kSystemFont);
	fAlignGroup.SetLabel ("Align", eNoUpdate);
		fAlignGroupRadioBank.SetController (this);

		fAlignLeft.SetLabel ("Left", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignLeft);
		fAlignLeft.SetExtent (22, 2, 14, 44, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignLeft);

		fAlignCenter.SetLabel ("Center", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignCenter);
		fAlignCenter.SetExtent (41, 2, 14, 61, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignCenter);

		fAlignRight.SetLabel ("Right", eNoUpdate);
		fAlignGroupRadioBank.AddRadioButton (&fAlignRight);
		fAlignRight.SetExtent (60, 2, 14, 51, eNoUpdate);
		fAlignGroup.AddSubView (&fAlignRight);

	AddSubView (&fAlignGroup);

	fField1.SetExtent (23, 122, 15, 90, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Column Width");
	AddSubView (&fField1);

	fField2.SetExtent (44, 137, 16, 75, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Row Height");
	AddSubView (&fField2);

	fColCountLabel.SetExtent (23, 1, 15, 87, eNoUpdate);
	fColCountLabel.SetFont (&kSystemFont);
	fColCountLabel.SetText ("# of Columns");
	AddSubView (&fColCountLabel);

	fRowCountLabel.SetExtent (43, 20, 15, 67, eNoUpdate);
	fRowCountLabel.SetFont (&kSystemFont);
	fRowCountLabel.SetText ("# of Rows");
	AddSubView (&fRowCountLabel);

	fColGapLabel.SetExtent (65, 11, 15, 76, eNoUpdate);
	fColGapLabel.SetFont (&kSystemFont);
	fColGapLabel.SetText ("Column Gap");
	AddSubView (&fColGapLabel);

	fRowGapLabel.SetExtent (86, 31, 15, 56, eNoUpdate);
	fRowGapLabel.SetFont (&kSystemFont);
	fRowGapLabel.SetText ("Row Gap");
	AddSubView (&fRowGapLabel);

	fColumnGap.SetExtent (85, 88, 17, 31, eNoUpdate);
	fColumnGap.SetFont (&kApplicationFont);
	fColumnGap.SetController (this);
	fColumnGap.SetMultiLine (False);
	fColumnGap.SetMinValue (0);
	AddSubView (&fColumnGap);

	fRowGap.SetExtent (64, 88, 17, 31, eNoUpdate);
	fRowGap.SetFont (&kApplicationFont);
	fRowGap.SetController (this);
	fRowGap.SetMultiLine (False);
	fRowGap.SetMinValue (0);
	AddSubView (&fRowGap);

	fRowCount.SetExtent (43, 88, 16, 31, eNoUpdate);
	fRowCount.SetFont (&kApplicationFont);
	fRowCount.SetController (this);
	fRowCount.SetMultiLine (False);
	fRowCount.SetMaxValue (100);
	fRowCount.SetMinValue (1);
	AddSubView (&fRowCount);

	fViewInfo.SetExtent (108, 4, 219, 352, eNoUpdate);
	AddSubView (&fViewInfo);

	fDrawGridLines.SetLabel ("Grid Lines", eNoUpdate);
	fDrawGridLines.SetExtent (84, 135, 14, 82, eNoUpdate);
	fDrawGridLines.SetController (this);
	AddSubView (&fDrawGridLines);

	fForceSize.SetLabel ("Force Size", eNoUpdate);
	fForceSize.SetExtent (65, 135, 14, 84, eNoUpdate);
	fForceSize.SetController (this);
	AddSubView (&fForceSize);

	fColumnCount.SetExtent (23, 89, 16, 30, eNoUpdate);
	fColumnCount.SetFont (&kApplicationFont);
	fColumnCount.SetController (this);
	fColumnCount.SetMultiLine (False);
	fColumnCount.SetMaxValue (100);
	fColumnCount.SetMinValue (1);
	AddSubView (&fColumnCount);

	fRowHeight.SetExtent (43, 213, 17, 33, eNoUpdate);
	fRowHeight.SetFont (&kApplicationFont);
	fRowHeight.SetController (this);
	fRowHeight.SetMultiLine (False);
	fRowHeight.SetMinValue (0);
	AddSubView (&fRowHeight);

	fColumnWidth.SetExtent (22, 213, 17, 33, eNoUpdate);
	fColumnWidth.SetFont (&kApplicationFont);
	fColumnWidth.SetController (this);
	fColumnWidth.SetMultiLine (False);
	fColumnWidth.SetMinValue (0);
	AddSubView (&fColumnWidth);

	AddFocus (&fColumnCount);
	AddFocus (&fRowCount);
	AddFocus (&fColumnGap);
	AddFocus (&fRowGap);
	AddFocus (&fColumnWidth);
	AddFocus (&fRowHeight);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	FixedGridInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (330, 359));
#elif   qMotifUI
	return (Point (371, 359));
#else
	return (Point (330, 359));
#endif /* GUI */
}

void	FixedGridInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"Grid.hh"
#include	"GridItem.hh"
#include	"EmilyWindow.hh"

FixedGridInfo::FixedGridInfo (GridItem& view)
{
	fViewInfo.SetUpFromView (view);
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	
	AbstractGrid_Portable&	grid = view.GetGrid ();
	
	GetGridLinesField ().SetOn (grid.GetGridLinesDrawn (), eNoUpdate);
	GetForceSizeField ().SetOn (grid.GetForceSize (), eNoUpdate);
	if (grid.GetSortByColumns ()) {
		GetColumnCountField ().SetValue (grid.GetColumnCount (), eNoUpdate);
	}
	else {
		GetRowCountField ().SetValue (grid.GetRowCount (), eNoUpdate);
	}
	GetColumnGapField ().SetValue (grid.GetColumnGap (), eNoUpdate);
	GetRowGapField ().SetValue (grid.GetRowGap (), eNoUpdate);
	GetColumnWidthField ().SetValue (grid.GetColumnWidth (1), eNoUpdate);
	GetRowHeightField ().SetValue (grid.GetRowHeight (1), eNoUpdate);
	
	switch (grid.GetColumnJustification (1)) {
		case AbstractTextView::eJustLeft:
			fAlignLeft.SetOn (Toggle::kOn, eNoUpdate);
			break;
			
		case AbstractTextView::eJustCenter:
			fAlignCenter.SetOn (Toggle::kOn, eNoUpdate);
			break;
			
		case AbstractTextView::eJustRight:
			fAlignRight.SetOn (Toggle::kOn, eNoUpdate);
			break;
		
		default:
			AssertNotReached ();
	}

	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetGridLinesField ().SetEnabled (fullEditing);
	GetForceSizeField ().SetEnabled (fullEditing);
	GetColumnCountField ().SetEnabled (fullEditing);
	GetRowCountField ().SetEnabled (fullEditing);
}


AbstractTextView::Justification	FixedGridInfo::GetJustification () const
{
	AbstractRadioButton* rb = fAlignGroupRadioBank.GetCurrentButton ();
	RequireNotNil (rb);
	
	if (rb == &fAlignLeft) {
		return (AbstractTextView::eJustLeft);
	}
	else if (rb == &fAlignCenter) {
		return (AbstractTextView::eJustCenter);
	}
	else if (rb == &fAlignRight) {
		return (AbstractTextView::eJustRight);
	}
	AssertNotReached ();	return (AbstractTextView::eJustLeft);
}


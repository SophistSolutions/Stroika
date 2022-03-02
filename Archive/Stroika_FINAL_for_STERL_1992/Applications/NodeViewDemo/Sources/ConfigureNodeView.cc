/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ConfigureNodeView.cc,v 1.4 1992/09/08 16:48:46 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: ConfigureNodeView.cc,v $
 *		Revision 1.4  1992/09/08  16:48:46  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:31:57  sterling
 *		*** empty log message ***
 *
 *
 *		Revision 1.1  1992/03/26  18:48:41  sterling
 *		Initial revision
 *		
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "GroupView.hh"
#include "Language.hh"
#include "Shape.hh"
#include "CheckBox.hh"
#include "NumberText.hh"
#include "TextView.hh"
#include "RadioButton.hh"

#include "ConfigureNodeView.hh"


ConfigureNodeViewX::ConfigureNodeViewX () :
	fExpandable (Nil),
	fLineSegmentGroup (Nil),
	fLSVertical (Nil),
	fLSHorizontal (Nil),
	fHLineSpacingLabel (Nil),
	fVLineSpacingLabel (Nil),
	fDirectionGroup (Nil),
	fVertical (Nil),
	fHorizontal (Nil),
	fSpacingGroup (Nil),
	fHSpacing (Nil),
	fVSpacing (Nil),
	fVSpacingLabel (Nil),
	fHSpacingLabel (Nil),
	fField1 (Nil),
	fInitialized (False)
{
#if   qMacGUI
	BuildForMacGUI ();
#else
	BuildForMacGUI ();
#endif /* GUI */
}

#if   qMacGUI || 1
void	ConfigureNodeViewX::BuildForMacGUI ()
{
	SetSize (Point (138, 233), eNoUpdate);

	fExpandable = new CheckBox ("Expandable", this);
	fExpandable->SetExtent (85, 132, 15, 92, eNoUpdate);
	AddSubView (fExpandable);

	fLineSegmentGroup = new LabeledGroup ("Line Segment");
	fLineSegmentGroup->SetExtent (78, 8, 54, 108, eNoUpdate);
	fLineSegmentGroup->SetFont (&kSystemFont);
		fLineSegmentGroupTabLoop = new FocusOwner ();

		fLSVertical = new NumberText (this);
		fLSVertical->SetExtent (16, 82, 16, 16, eNoUpdate);
		fLSVertical->SetMaxValue (50);
		fLSVertical->SetMinValue (2);
		fLineSegmentGroup->AddSubView (fLSVertical);

		fLSHorizontal = new NumberText (this);
		fLSHorizontal->SetExtent (34, 82, 16, 16, eNoUpdate);
		fLSHorizontal->SetMaxValue (50);
		fLSHorizontal->SetMinValue (2);
		fLineSegmentGroup->AddSubView (fLSHorizontal);

		fHLineSpacingLabel = new TextView ();
		fHLineSpacingLabel->SetExtent (34, 5, 16, 73, eNoUpdate);
		fHLineSpacingLabel->SetText ("Horizontal:");
		fLineSegmentGroup->AddSubView (fHLineSpacingLabel);

		fVLineSpacingLabel = new TextView ();
		fVLineSpacingLabel->SetExtent (16, 22, 16, 56, eNoUpdate);
		fVLineSpacingLabel->SetText ("Vertical:");
		fLineSegmentGroup->AddSubView (fVLineSpacingLabel);

		fLineSegmentGroupTabLoop->AddFocus (fLSHorizontal);
		fLineSegmentGroupTabLoop->AddFocus (fLSVertical);
	AddSubView (fLineSegmentGroup);

	fDirectionGroup = new LabeledGroup ("Direction");
	fDirectionGroup->SetExtent (23, 132, 54, 93, eNoUpdate);
	fDirectionGroup->SetFont (&kSystemFont);
		fDirectionGroupRadioBank = new RadioBank ();
		fDirectionGroupRadioBank->SetController (this);

		fVertical = new RadioButton ("Vertical");
		fVertical->SetExtent (33, 4, 15, 68, eNoUpdate);
		fDirectionGroupRadioBank->AddRadioButton (fVertical);
		fDirectionGroup->AddSubView (fVertical);

		fHorizontal = new RadioButton ("Horizontal");
		fHorizontal->SetExtent (17, 4, 15, 85, eNoUpdate);
		fDirectionGroupRadioBank->AddRadioButton (fHorizontal);
		fDirectionGroup->AddSubView (fHorizontal);

	AddSubView (fDirectionGroup);

	fSpacingGroup = new LabeledGroup ("Spacing");
	fSpacingGroup->SetExtent (23, 8, 52, 107, eNoUpdate);
	fSpacingGroup->SetFont (&kSystemFont);
		fSpacingGroupTabLoop = new FocusOwner ();

		fHSpacing = new NumberText (this);
		fHSpacing->SetExtent (33, 86, 16, 16, eNoUpdate);
		fHSpacing->SetMaxValue (50);
		fHSpacing->SetMinValue (2);
		fSpacingGroup->AddSubView (fHSpacing);

		fVSpacing = new NumberText (this);
		fVSpacing->SetExtent (15, 86, 16, 16, eNoUpdate);
		fVSpacing->SetMaxValue (50);
		fVSpacing->SetMinValue (2);
		fSpacingGroup->AddSubView (fVSpacing);

		fVSpacingLabel = new TextView ();
		fVSpacingLabel->SetExtent (15, 26, 16, 56, eNoUpdate);
		fVSpacingLabel->SetText ("Vertical:");
		fSpacingGroup->AddSubView (fVSpacingLabel);

		fHSpacingLabel = new TextView ();
		fHSpacingLabel->SetExtent (33, 9, 16, 73, eNoUpdate);
		fHSpacingLabel->SetText ("Horizontal:");
		fSpacingGroup->AddSubView (fHSpacingLabel);

		fSpacingGroupTabLoop->AddFocus (fVSpacing);
		fSpacingGroupTabLoop->AddFocus (fHSpacing);
	AddSubView (fSpacingGroup);

	fField1 = new TextView ();
	fField1->SetExtent (1, 1, 16, 231, eNoUpdate);
	fField1->SetFont (&kSystemFont);
	fField1->SetText ("Configure NodeView");
	fField1->SetJustification (AbstractTextView::eJustCenter);
	AddSubView (fField1);

	AddFocus (fSpacingGroupTabLoop);
	AddFocus (fLineSegmentGroupTabLoop);
}
#endif /* qMacGUI */

Point	ConfigureNodeViewX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacGUI
	return (Point (138, 233));
#else
	return (Point (138, 233));
#endif /* GUI */
}

void	ConfigureNodeViewX::Layout ()
{
	const Point kSizeDelta = Point (138, 233) - GetSize ();
		static const Point	kOriginalfField1Size = fField1->GetSize ();
		fField1->SetSize (kOriginalfField1Size - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"NodeView.hh"

ConfigureNodeView::ConfigureNodeView (BranchNode& nodeView)
{
	if (nodeView.GetDirection () == Point::eVertical) {
		fVertical->SetOn (True);
	}
	else {
		fHorizontal->SetOn (True);
	}
	fExpandable->SetOn (nodeView.GetExpandable ());
	fLSVertical->SetValue (nodeView.GetArcLength ().GetV ());
	fLSHorizontal->SetValue (nodeView.GetArcLength ().GetH ());
	fVSpacing->SetValue (nodeView.GetSpacing ().GetV ());
	fHSpacing->SetValue (nodeView.GetSpacing ().GetH ());
}

Point::Direction	ConfigureNodeView::GetDirection () const
{
	RequireNotNil (fVertical);
	return ((fVertical->GetOn ()) ? Point::eVertical : Point::eHorizontal);
}

Point	ConfigureNodeView::GetArcLength () const
{
	RequireNotNil (fLSVertical);
	RequireNotNil (fLSHorizontal);
	return (Point (fLSVertical->GetValue (), fLSHorizontal->GetValue ()));
}

Point	ConfigureNodeView::GetSpacing () const
{
	RequireNotNil (fVSpacing);
	RequireNotNil (fHSpacing);
	return (Point (fVSpacing->GetValue (), fHSpacing->GetValue ()));
}

Boolean	ConfigureNodeView::GetExpandable () const
{
	RequireNotNil (fExpandable);
	return (fExpandable->GetOn ());
}

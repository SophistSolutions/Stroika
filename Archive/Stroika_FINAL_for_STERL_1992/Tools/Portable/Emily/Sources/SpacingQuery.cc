/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SpacingQuery.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: SpacingQuery.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.16  1992/05/19  11:36:12  sterling
 *		hi
 *
 *		Revision 1.15  92/05/13  18:47:30  18:47:30  lewis (Lewis Pringle)
 *		STERL.
 *		
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "SpacingQuery.hh"


SpacingQueryX::SpacingQueryX () :
	fSpacingLabel (),
	fSpacing (),
	fTitle ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

SpacingQueryX::~SpacingQueryX ()
{
	RemoveFocus (&fSpacing);

	RemoveSubView (&fSpacingLabel);
	RemoveSubView (&fSpacing);
	RemoveSubView (&fTitle);
}

#if   qMacUI

void	SpacingQueryX::BuildForMacUI ()
{
	SetSize (Point (110, 200), eNoUpdate);

	fSpacingLabel.SetExtent (88, 30, 15, 50, eNoUpdate);
	fSpacingLabel.SetFont (&kSystemFont);
	fSpacingLabel.SetText ("Spacing");
	AddSubView (&fSpacingLabel);

	fSpacing.SetExtent (87, 82, 18, 85, eNoUpdate);
	fSpacing.SetFont (&kApplicationFont);
	fSpacing.SetController (this);
	fSpacing.SetMultiLine (False);
	fSpacing.SetMaxValue (500);
	fSpacing.SetMinValue (0);
	AddSubView (&fSpacing);

	fTitle.SetExtent (5, 5, 70, 190, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("What spacing would you like to place between the selected items?");
	AddSubView (&fTitle);

	AddFocus (&fSpacing);
}

#elif qMotifUI

void	SpacingQueryX::BuildForMotifUI ()
{
	SetSize (Point (110, 200), eNoUpdate);

	fSpacingLabel.SetExtent (80, 14, 21, 57, eNoUpdate);
	fSpacingLabel.SetFont (&kSystemFont);
	fSpacingLabel.SetText ("Spacing");
	AddSubView (&fSpacingLabel);

	fSpacing.SetExtent (79, 76, 26, 85, eNoUpdate);
	fSpacing.SetFont (&kApplicationFont);
	fSpacing.SetController (this);
	fSpacing.SetMaxValue (500);
	fSpacing.SetMinValue (0);
	AddSubView (&fSpacing);

	fTitle.SetExtent (5, 5, 70, 190, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("What spacing would you like to place between the selected items?");
	AddSubView (&fTitle);

	AddFocus (&fSpacing);
}

#else

void	SpacingQueryX::BuildForUnknownGUI ();
{
	SetSize (Point (110, 200), eNoUpdate);

	fSpacingLabel.SetExtent (88, 30, 15, 50, eNoUpdate);
	fSpacingLabel.SetFont (&kSystemFont);
	fSpacingLabel.SetText ("Spacing");
	AddSubView (&fSpacingLabel);

	fSpacing.SetExtent (87, 82, 18, 85, eNoUpdate);
	fSpacing.SetFont (&kApplicationFont);
	fSpacing.SetController (this);
	fSpacing.SetMultiLine (False);
	fSpacing.SetMaxValue (500);
	fSpacing.SetMinValue (0);
	AddSubView (&fSpacing);

	fTitle.SetExtent (5, 5, 70, 190, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("What spacing would you like to place between the selected items?");
	AddSubView (&fTitle);

	AddFocus (&fSpacing);
}

#endif /* GUI */

Point	SpacingQueryX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (110, 200));
#elif   qMotifUI
	return (Point (110, 200));
#else
	return (Point (110, 200));
#endif /* GUI */
}

void	SpacingQueryX::Layout ()
{
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
SpacingQuery::SpacingQuery () :
	SpacingQueryX ()
{
}

UInt16	SpacingQuery::GetSpacing () const
{
	return ((UInt16) fSpacing.GetValue ());
}

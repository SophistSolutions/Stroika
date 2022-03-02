/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DeleteCustomizations.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: DeleteCustomizations.cc,v $
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
 *		Revision 1.9  1992/05/19  11:35:43  sterling
 *		hi
 *
 *		Revision 1.8  92/05/13  18:47:01  18:47:01  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.3  1992/03/06  21:52:58  sterling
 *		motif
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "DeleteCustomizations.hh"


DeleteCustomizationsX::DeleteCustomizationsX () :
	fField1 (),
	fCustomizations ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

DeleteCustomizationsX::~DeleteCustomizationsX ()
{
	RemoveSubView (&fField1);
	RemoveSubView (&fCustomizations);
}

#if   qMacUI

void	DeleteCustomizationsX::BuildForMacUI ()
{
	SetSize (Point (184, 230), eNoUpdate);

	fField1.SetExtent (2, -1, 35, 229, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fField1.SetBorder (0, 0, eNoUpdate);
	fField1.SetMargin (0, 0, eNoUpdate);
	fField1.SetText ("Which customizations would you like to delete?");
	AddSubView (&fField1);

	fCustomizations.SetExtent (37, 1, 143, 226, eNoUpdate);
	fCustomizations.SetAllowMultipleSelections (True);
	fCustomizations.SetController (this);
	AddSubView (&fCustomizations);

}

#elif qMotifUI

void	DeleteCustomizationsX::BuildForMotifUI ()
{
	SetSize (Point (184, 230), eNoUpdate);

	fField1.SetExtent (2, -1, 35, 229, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fField1.SetBorder (0, 0, eNoUpdate);
	fField1.SetMargin (0, 0, eNoUpdate);
	fField1.SetText ("Which customizations would you like to delete?");
	AddSubView (&fField1);

	fCustomizations.SetExtent (37, 1, 143, 226, eNoUpdate);
	fCustomizations.SetAllowMultipleSelections (True);
	fCustomizations.SetController (this);
	AddSubView (&fCustomizations);

	AddFocus (&fCustomizations);
}

#else

void	DeleteCustomizationsX::BuildForUnknownGUI ();
{
	SetSize (Point (184, 230), eNoUpdate);

	fField1.SetExtent (2, -1, 35, 229, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fField1.SetBorder (0, 0, eNoUpdate);
	fField1.SetMargin (0, 0, eNoUpdate);
	fField1.SetText ("Which customizations would you like to delete?");
	AddSubView (&fField1);

	fCustomizations.SetExtent (37, 1, 143, 226, eNoUpdate);
	fCustomizations.SetAllowMultipleSelections (True);
	fCustomizations.SetController (this);
	AddSubView (&fCustomizations);

}

#endif /* GUI */

Point	DeleteCustomizationsX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (184, 230));
#elif   qMotifUI
	return (Point (184, 230));
#else
	return (Point (184, 230));
#endif /* GUI */
}

void	DeleteCustomizationsX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfField1Size = fField1.GetSize ();
		fField1.SetSize (kOriginalfField1Size - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfCustomizationsSize = fCustomizations.GetSize ();
		fCustomizations.SetSize (kOriginalfCustomizationsSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include "Dialog.hh"
#include "Shell.hh"
#include "PushButton.hh"

DeleteCustomizations::DeleteCustomizations ()
{
}
		
DeleteCustomizationsDialog::DeleteCustomizationsDialog () :
	Dialog (new StandardDialogWindowShell ()),
	fDeleteCustomizations (Nil),
	fDialogMainView (Nil)
{
	fDeleteCustomizations = new DeleteCustomizations ();
	fDeleteCustomizations->SetController (this);

	SetMainView (fDialogMainView = new DialogMainView (*this, *fDeleteCustomizations, "Delete", AbstractPushButton::kCancelLabel));
	SetDefaultButton (GetOKButton ());
	GetOKButton ()->SetEnabled (False);
}

DeleteCustomizationsDialog::~DeleteCustomizationsDialog ()
{
	SetMainView (Nil);
	delete fDialogMainView;
	delete fDeleteCustomizations;
}
		
StringPickList&	DeleteCustomizationsDialog::GetCustomizationList () const
{
	AssertNotNil (fDeleteCustomizations);
	return (fDeleteCustomizations->GetCustomizationList ());
}

void	DeleteCustomizationsDialog::ButtonPressed (AbstractButton* button)
{
	GetOKButton ()->SetEnabled (Boolean (GetCustomizationList ().CountSelected () > 0));
	Dialog::ButtonPressed (button);
}

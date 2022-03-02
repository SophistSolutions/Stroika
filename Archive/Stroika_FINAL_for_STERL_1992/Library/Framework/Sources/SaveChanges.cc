/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SaveChanges.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: SaveChanges.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.14  1992/05/18  17:23:40  lewis
 *		Dont include Language.hh since defined in User - shound be moved tooFoundation.
 *
 *		Revision 1.12  92/05/13  17:32:32  17:32:32  lewis (Lewis Pringle)
 *		Missing includes.
 *		
 *
 */

#include	"StreamUtils.hh"

// text before here will be retained: Do not remove or modify this line!!!


//#include "Language.hh"
#include "Shape.hh"

#include "SaveChanges.hh"


SaveChangesX::SaveChangesX () :
	fSaveName (),
	fField1 (),
	fCancel (),
	fNo (),
	fYes ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

SaveChangesX::~SaveChangesX ()
{
	RemoveSubView (&fSaveName);
	RemoveSubView (&fField1);
	RemoveSubView (&fCancel);
	RemoveSubView (&fNo);
	RemoveSubView (&fYes);
}

#if   qMacUI

void	SaveChangesX::BuildForMacUI ()
{
	SetSize (Point (136, 251), eNoUpdate);

	fSaveName.SetExtent (34, 68, 16, 169, eNoUpdate);
	fSaveName.SetFont (&kSystemFont);
	fSaveName.SetText ("File Name:");
	AddSubView (&fSaveName);

	fField1.SetExtent (10, 40, 16, 113, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Save Changes To:");
	AddSubView (&fField1);

	fCancel.SetExtent (90, 140, 20, 81, eNoUpdate);
	fCancel.SetLabel (AbstractPushButton::kCancelLabel, eNoUpdate);
	fCancel.SetController (this);
	AddSubView (&fCancel);

	fNo.SetExtent (90, 30, 20, 81, eNoUpdate);
	fNo.SetLabel (AbstractPushButton::kNoLabel, eNoUpdate);
	fNo.SetController (this);
	AddSubView (&fNo);

	fYes.SetExtent (60, 30, 20, 81, eNoUpdate);
	fYes.SetLabel (AbstractPushButton::kYesLabel, eNoUpdate);
	fYes.SetController (this);
	AddSubView (&fYes);

}

#elif qMotifUI

void	SaveChangesX::BuildForMotifUI ()
{
	SetSize (Point (136, 251), eNoUpdate);

	fSaveName.SetExtent (34, 68, 16, 177, eNoUpdate);
	fSaveName.SetFont (&kSystemFont);
	fSaveName.SetText ("File Name:");
	AddSubView (&fSaveName);

	fField1.SetExtent (10, 40, 16, 113, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Save Changes To:");
	AddSubView (&fField1);

	fCancel.SetExtent (90, 140, 23, 80, eNoUpdate);
	fCancel.SetLabel (AbstractPushButton::kCancelLabel, eNoUpdate);
	fCancel.SetController (this);
	AddSubView (&fCancel);

	fNo.SetExtent (90, 30, 23, 80, eNoUpdate);
	fNo.SetLabel (AbstractPushButton::kNoLabel, eNoUpdate);
	fNo.SetController (this);
	AddSubView (&fNo);

	fYes.SetExtent (60, 30, 23, 80, eNoUpdate);
	fYes.SetLabel (AbstractPushButton::kYesLabel, eNoUpdate);
	fYes.SetController (this);
	AddSubView (&fYes);

	AddFocus (&fYes);
	AddFocus (&fNo);
	AddFocus (&fCancel);
}

#else

void	SaveChangesX::BuildForUnknownGUI ();
{
	SetSize (Point (136, 251), eNoUpdate);

	fSaveName.SetExtent (34, 68, 16, 169, eNoUpdate);
	fSaveName.SetFont (&kSystemFont);
	fSaveName.SetText ("File Name:");
	AddSubView (&fSaveName);

	fField1.SetExtent (10, 40, 16, 113, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Save Changes To:");
	AddSubView (&fField1);

	fCancel.SetExtent (90, 140, 20, 81, eNoUpdate);
	fCancel.SetLabel (AbstractPushButton::kCancelLabel, eNoUpdate);
	fCancel.SetController (this);
	AddSubView (&fCancel);

	fNo.SetExtent (90, 30, 20, 81, eNoUpdate);
	fNo.SetLabel (AbstractPushButton::kNoLabel, eNoUpdate);
	fNo.SetController (this);
	AddSubView (&fNo);

	fYes.SetExtent (60, 30, 20, 81, eNoUpdate);
	fYes.SetLabel (AbstractPushButton::kYesLabel, eNoUpdate);
	fYes.SetController (this);
	AddSubView (&fYes);

}

#endif /* GUI */

Point	SaveChangesX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (136, 251));
#elif   qMotifUI
	return (Point (136, 251));
#else
	return (Point (136, 251));
#endif /* GUI */
}

void	SaveChangesX::Layout ()
{
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
class	SaveChanges : public SaveChangesX {
	public:
		SaveChanges (const String& fileName) :
			SaveChangesX ()
		{
			OStringStream	buf;
			buf << "'" << fileName << "'?";
			fSaveName.SetText (buf, eNoUpdate);
		}
		
		nonvirtual	PushButton&	GetYesButton ()	 	{	return (fYes);		}
		nonvirtual	PushButton&	GetNoButton ()	 	{	return (fNo);		}
		nonvirtual	PushButton&	GetCancelButton () 	{	return (fCancel);	}

};

SaveChangesDialog::SaveChangesDialog (const String& fileName) :
	Dialog (Nil, Nil),
	fSaveChanges (Nil),
	fShouldSave (False),
	fDialogMainView (Nil)
{
	fSaveChanges = new SaveChanges (fileName);
	SetMainView (fDialogMainView = new DialogMainView (*this, *fSaveChanges));
	SetOKButton (&fSaveChanges->GetYesButton ());
	SetCancelButton (&fSaveChanges->GetCancelButton ());
	SetDefaultButton (&fSaveChanges->GetYesButton ());
	fSaveChanges->SetController (this);
}

SaveChangesDialog::~SaveChangesDialog ()
{
	SetMainView (Nil);
	delete fDialogMainView;
	delete fSaveChanges;
}
	
void	SaveChangesDialog::ButtonPressed (AbstractButton* button)
{
	if (button == &fSaveChanges->GetNoButton ()) {
		Dismiss (not kCancel);
		return;
	}
	else if (button == &fSaveChanges->GetYesButton ()) {
		fShouldSave = True;
	}
	
	Dialog::ButtonPressed (button);
}
		
Boolean	SaveChangesDialog::GetSaveChanges () const
{
	return (fShouldSave);
}

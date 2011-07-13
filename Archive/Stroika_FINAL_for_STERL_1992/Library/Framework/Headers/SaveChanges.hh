/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SaveChanges.hh,v 1.4 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: SaveChanges.hh,v $
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.9  1992/05/14  15:07:57  lewis
 *		Rerun Emily to get codegen bug fixed.
 *
 *		
 *
 */

// text before here will be retained: Do not remove or modify this line!!!

#ifndef __SaveChangesX__
#define __SaveChangesX__

#include "Button.hh"
#include "FocusItem.hh"
#include "View.hh"

#include "TextView.hh"
#include "PushButton.hh"


class SaveChangesX : public View, public ButtonController, public FocusOwner {
	public:
		SaveChangesX ();
		~SaveChangesX ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();

		TextView			fSaveName;
		TextView			fField1;
		PushButton			fCancel;
		PushButton			fNo;
		PushButton			fYes;

	private:
#if   qMacUI
		nonvirtual void	BuildForMacUI ();
#elif qMotifUI
		nonvirtual void	BuildForMotifUI ();
#else
		nonvirtual void	BuildForUnknownUI ();
#endif /* UI */

};

#endif /* __SaveChangesX__ */


// text past here will be retained: Do not remove or modify this line!!!
#include	"Dialog.hh"

class	SaveChanges;
class SaveChangesDialog : public Dialog {
	public:
		SaveChangesDialog (const String& fileName);
		~SaveChangesDialog ();
		
		override	void	ButtonPressed (AbstractButton* button);
		
		nonvirtual	Boolean	GetSaveChanges () const;
		
	private:
		SaveChanges*	fSaveChanges;
		Boolean			fShouldSave;
		DialogMainView*	fDialogMainView;
};

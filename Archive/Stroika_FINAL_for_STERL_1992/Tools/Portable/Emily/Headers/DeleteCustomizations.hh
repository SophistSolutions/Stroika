/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DeleteCustomizations.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: DeleteCustomizations.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "View.hh"

#include "TextEdit.hh"
#include "PickList.hh"


class DeleteCustomizationsX : public View, public ButtonController, public FocusOwner {
	public:
		DeleteCustomizationsX ();
		~DeleteCustomizationsX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextEdit			fField1;
		StringPickList		fCustomizations;

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
#include "Dialog.hh"

class	DeleteCustomizations : public DeleteCustomizationsX {
	public:
		DeleteCustomizations ();
		
		nonvirtual	StringPickList&	GetCustomizationList ()
		{
			return (fCustomizations);
		}
};

class	DeleteCustomizationsDialog : public Dialog {
	public:
		DeleteCustomizationsDialog ();
		~DeleteCustomizationsDialog ();
		
		nonvirtual	StringPickList&	GetCustomizationList () const;

	protected:
		override	void	ButtonPressed (AbstractButton* button);
	
	private:
		DeleteCustomizations*	fDeleteCustomizations;
		DialogMainView*			fDialogMainView;
};


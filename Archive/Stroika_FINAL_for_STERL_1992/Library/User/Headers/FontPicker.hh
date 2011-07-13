/*
 * $Header: /fuji/lewis/RCS/FontPicker.hh,v 1.4 1992/09/01 15:54:46 sterling Exp $
 *
 * Description:
 *
 *
 * To Do:
 *
 * Notes:
 *
 *	$Log: FontPicker.hh,v $
 *		Revision 1.4  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:19:50  sterling
 *		change qGUI to qUI, supported qWinUi
 *
 *
 */


#ifndef	__FontPicker__
#define	__FontPicker__


#include 	"OptionMenu.hh"

class	FontStylePopUp : public OptionMenuButton {
	public:
		FontStylePopUp ();
};


// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "OptionMenu.hh"
#include "PickList.hh"
#include "CheckBox.hh"
#include "NumberText.hh"
#include "TextEdit.hh"
#include "TextView.hh"


class FontPickerX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		FontPickerX ();
		~FontPickerX ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();

		FontStylePopUp		fStyle;
		StringPickList		fFontSizeList;
		StringPickList		fFontList;
		CheckBox			fInheritFont;
		NumberText			fFontSize;
		TextEdit			fSampleText;
		TextView			fSizeLabel;

	private:
#if   qMacUI
		nonvirtual void	BuildForMacUI ();
#elif qMotifUI
		nonvirtual void	BuildForMotifUI ();
#else
		nonvirtual void	BuildForUnknownUI ();
#endif /* UI */

};



// text past here will be retained: Do not remove or modify this line!!!
class	SetFontCommand : public Command {
	public:
		SetFontCommand (View& view, const Font* oldFont, const Font* newFont);
		SetFontCommand (View& view, const String& fontName);
		SetFontCommand (View& view, FontSize fontSize);
		SetFontCommand (View& view, CommandNumber fontStyleCommand);
		
		~SetFontCommand ();		
		override	void	DoIt ();
		override	void	UnDoIt ();
	
	private:		
		View&	fView;
		Font*	fNewFont;
		Font*	fOldFont;
};

class	FontPicker : public FontPickerX {
	public:
		FontPicker (const Font& font);
		FontPicker (const Font* font); 
		
		override	void	ButtonPressed (AbstractButton* button);
		nonvirtual	CheckBox&	GetInheritFont ()	{	return (fInheritFont);	}
		
		static	SetFontCommand*	PickFont (View* view);
	
	protected:	
		nonvirtual	void	UpdateFontSizeChange (Boolean updatePickList = True, UpdateMode update = eDelayedUpdate);
		override	void	SetCurrentFocus_ (FocusItem* newFocus, UpdateMode update, Boolean validate = True);		
		nonvirtual	void	Initialize ();

	public:
		Font	fFont;
};

#endif	/* __FontPicker__ */



// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "PickList.hh"
#include "GroupView.hh"
#include "NumberText.hh"
#include "TextView.hh"


class ColorPickerX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		ColorPickerX ();
		~ColorPickerX ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();

		StringPickList		fNamedColors;
		GroupView			fColorDisplay;
		NumberText			fRed;
		NumberText			fBlue;
		NumberText			fGreen;
		TextView			fBlueLabel;
		TextView			fRedLabel;
		TextView			fGreenLabel;

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

#include "Color.hh"

class	ColorPicker : public ColorPickerX {
	public:
		ColorPicker (const Color& c);
		
		nonvirtual	Color	GetStartColor () const;
		
		nonvirtual	Color	GetColor () const;
		nonvirtual	void	SetColor (const Color& c, UpdateMode update = eDelayedUpdate);

		override	void	ButtonPressed (AbstractButton* b);
		override	void 	SetCurrentFocus_ (FocusItem* newFocus, UpdateMode update, Boolean validate = True);
		
	private:
		Color	fStartColor;
};

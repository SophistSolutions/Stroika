

// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "TextView.hh"
#include "CheckBox.hh"
#include "TextEdit.hh"
#include "ViewItemInfo.hh"


class RadioButtonInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		RadioButtonInfoX ();
		~RadioButtonInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		TextView			fLabelLabel;
		CheckBox			fOn;
		TextEdit			fLabel;
		ViewItemInfo		fViewInfo;

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
class	RadioButtonItem;
class RadioButtonInfo : public RadioButtonInfoX {
	public:
		RadioButtonInfo (RadioButtonItem& view);

		nonvirtual	CheckBox&	GetOnField ()
		{
			return (fOn);
		}
		
		nonvirtual	TextEdit&	GetLabelField ()
		{
			return (fLabel);
		}
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
};


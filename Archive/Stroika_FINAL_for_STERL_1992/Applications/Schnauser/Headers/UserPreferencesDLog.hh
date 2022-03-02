/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/UserPreferencesDLog.hh,v 1.1 1992/06/30 23:05:21 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: UserPreferencesDLog.hh,v $
 *		Revision 1.1  1992/06/30  23:05:21  lewis
 *		Initial revision
 *
# Revision 1.1  1992/06/30  03:57:05  lewis
# Initial revision
#
 *
 */

#include	"SchnauserConfig.hh"

// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "Slider.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "TextView.hh"
#include "GroupView.hh"
#include "NumberText.hh"
#include "CheckBox.hh"
#include "Scale.hh"
#include "OptionMenu.hh"


class UserPreferencesDLogX : public View, public ButtonController, public SliderController, public FocusOwner, public TextController {
	public:
		UserPreferencesDLogX ();
		~UserPreferencesDLogX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fField1;
		LabeledGroup		fField2;
		FocusOwner			fField2TabLoop;
		TextView			fField3;
		NumberText			fField4;
		TextView			fField5;
		CheckBox			fField6;
		LabeledGroup		fField7;
		FocusOwner			fField7TabLoop;
		CheckBox			fField8;
		CheckBox			fField9;
		CheckBox			fField10;
		GroupView			fField11;
		FocusOwner			fField11TabLoop;
		TextView			fField12;
		NumberText			fField13;
		LabeledGroup		fField14;
		FocusOwner			fField14TabLoop;
		Scale				fField15;
		TextView			fField16;
		CheckBox			fField17;
		OptionMenuButton	fOnStartup;

	private:
#if   qMacGUI
		nonvirtual void	BuildForMacGUI ();
#else
		nonvirtual void	BuildForUnknownGUI ();
#endif /* GUI */

};



// text past here will be retained: Do not remove or modify this line!!!

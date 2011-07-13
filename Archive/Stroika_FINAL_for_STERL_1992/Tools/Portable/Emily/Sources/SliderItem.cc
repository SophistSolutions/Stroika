/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SliderItem.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderItem.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"Slider.hh"
#include	"NumberText.hh"

#include	"SliderItem.hh"
#include	"CommandNumbers.hh"
#include	"ViewItemInfo.hh"


/*
 ********************************************************************************
 ********************************** SliderItem **********************************
 ********************************************************************************
 */
SliderItem::SliderItem (ItemType& type) :
	ViewItem (type),
	fSlider (Nil),
	fDefaultMin (0),
	fDefaultMax (0),
	fDefaultValue (0)
{
}

void	SliderItem::SetSlider (Slider* slider)
{
	if (fSlider != Nil) {
		SetOwnedView (Nil);
		fSlider = Nil;
	}
	fSlider = slider;
	if (fSlider != Nil) {
		SetOwnedView (fSlider);
		fDefaultMin = GetSlider ().GetMin ();
		fDefaultMax = GetSlider ().GetMax ();
		fDefaultValue = GetSlider ().GetValue ();
	}
}

Slider&	SliderItem::GetSlider () const
{
	RequireNotNil (fSlider);
	return (*fSlider);
}

Boolean	SliderItem::ItemCanBeEnabled ()
{
	return (True);
}

void	SliderItem::SetOwnedEnabled (Boolean enabled, Panel::UpdateMode updateMode)
{
	GetSlider ().SetEnabled (enabled, updateMode);
}

void	SliderItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
	Real	value;
	Real	minimum;
	Real	maximum;
	from >> ReadReal (value) >> ReadReal (minimum) >> ReadReal (maximum);
	GetSlider ().SetBounds (minimum, maximum, value, eNoUpdate);
}

void	SliderItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
	to << tab (tabCount) << WriteReal (GetSlider ().GetValue ()) << ' ' <<  WriteReal (GetSlider ().GetMin ()) << ' ' <<
		 WriteReal (GetSlider ().GetMax ()) << ' ';
	to << newline;
}

void	SliderItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
	if (fDefaultMin != GetSlider ().GetMin ()) {
		to << tab (tabCount) << GetFieldName () << ".SetMin (" << GetSlider ().GetMin () << ", eNoUpdate);" << newline;
	}
	if (fDefaultMax != GetSlider ().GetMax ()) {
		to << tab (tabCount) << GetFieldName () << ".SetMax (" << GetSlider ().GetMax () << ", eNoUpdate);" << newline;
	}
	if (fDefaultValue != GetSlider ().GetValue ()) {
		to << tab (tabCount) << GetFieldName () << ".SetValue (" << GetSlider ().GetValue () << ", eNoUpdate);" << newline;
	}
	to << tab (tabCount) << GetFieldName () << ".SetSliderController (this);" << newline;
}

Boolean	SliderItem::IsButton ()
{
	return (False);
}

Boolean	SliderItem::IsSlider ()
{
	return (True);
}

Boolean	SliderItem::IsText ()
{
	return (False);
}

Boolean	SliderItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}


void	SliderItem::SetItemInfo ()
{
#if 0
	SliderInfo info = SliderInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetSliderInfoCommand (*this, info));
		DirtyDocument ();
	}
#endif
}

SetSliderInfoCommand::SetSliderInfoCommand (SliderItem& item, Real newMin, Real newMax, Real newValue, ViewItemInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewMin (newMin),
	fOldMin (item.GetSlider ().GetMin ()),
	fNewMax (newMax),
	fOldMax (item.GetSlider ().GetMax ()),
	fNewValue (newValue),
	fOldValue (item.GetSlider ().GetValue ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info);
}

	
void	SetSliderInfoCommand::DoIt ()
{
	fItemInfoCommand->DoIt ();

	fItem.GetSlider ().SetBounds (fNewMin, fNewMax, fNewValue);
	Command::DoIt ();
}

void	SetSliderInfoCommand::UnDoIt ()
{
	fItemInfoCommand->UnDoIt ();

	fItem.GetSlider ().SetBounds (fOldMin, fOldMax, fOldValue);
	Command::UnDoIt ();
}

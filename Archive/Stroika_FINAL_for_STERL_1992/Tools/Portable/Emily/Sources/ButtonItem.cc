/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ButtonItem.cc,v 1.5 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ButtonItem.cc,v $
 *		Revision 1.5  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 *
 */

#include	"StreamUtils.hh"
#include	"Dialog.hh"

#include	"Button.hh"

#include	"ButtonItem.hh"
#include	"CommandNumbers.hh"





/*
 ********************************************************************************
 ********************************** ButtonItem **********************************
 ********************************************************************************
 */
ButtonItem::ButtonItem (ItemType& type) :
	ViewItem (type),
	fButton (Nil),
	fDefaultOn (False),
	fDefaultCanStayOn (False)
{
}

void	ButtonItem::SetButton (Button* button)
{
	fButton = button;
	SetOwnedView (fButton);
}

Boolean	ButtonItem::ItemCanBeEnabled ()
{
	return (True);
}

void	ButtonItem::SetOwnedEnabled (Boolean enabled, Panel::UpdateMode updateMode)
{
	fButton->SetEnabled (enabled, updateMode);
}

void	ButtonItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
	Boolean on, canStayOn;
	from >> on >> canStayOn;

	RequireNotNil (fButton);
}

void	ButtonItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
	RequireNotNil (fButton);
	to << tab (tabCount) << /*fButton->GetOn () << fButton->GetCanStayOn ()*/ False << False << newline;
}

void	ButtonItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
	
#if 0
	RequireNotNil (fButton);
	if (fButton->GetOn () != fDefaultOn) {
		to << tab (tabCount) << GetFieldName () << ".SetOn (" << GetBooleanAsText (fButton->GetOn ()) << ");" << newline;
	}
	if (fButton->GetCanStayOn () != fDefaultCanStayOn) {
		to << tab (tabCount) << GetFieldName () << ".SetCanStayOn (" << GetBooleanAsText (fButton->GetCanStayOn ()) << ");" << newline;
	}
#endif
}

Boolean	ButtonItem::IsButton ()
{
	return (True);
}

Boolean	ButtonItem::IsSlider ()
{
	return (False);
}

Boolean	ButtonItem::IsText ()
{
	return (False);
}

Boolean	ButtonItem::IsFocusItem (CommandNumber gui)
{
	if (gui == eMacUI) {
		return (False);
	}
	return (True);
}

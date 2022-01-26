/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ArrowButtonItem.cc,v 1.5 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *		$Log: ArrowButtonItem.cc,v $
 *		Revision 1.5  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
// Revision 1.4  1992/09/01  17:25:44  sterling
// Lots of Foundation changes.
//
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"ArrowButton.hh"

#include	"ArrowButtonItem.hh"
#include	"ArrowButtonInfo.hh"
#include	"EmilyWindow.hh"
#include	"CommandNumbers.hh"
#include	"ViewItemInfo.hh"







/*
 ********************************************************************************
 **************************** ArrowButtonItemType ****************************
 ********************************************************************************
 */
ArrowButtonItemType::ArrowButtonItemType ():
#if		qHP_BadTypedefForStaticProcs
	ItemType (eBuildButton, "ArrowButton", (ItemBuilderProc)&ArrowButtonItemBuilder)
#else
	ItemType (eBuildButton, "ArrowButton", &ArrowButtonItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
ArrowButtonItemType&	ArrowButtonItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	ArrowButtonItemType::ArrowButtonItemBuilder ()
{
	return (new ArrowButtonItem (Get ()));
}

ArrowButtonItemType*	ArrowButtonItemType::sThis = Nil;



static	AbstractArrowButton*	BuildArrowButton (CommandNumber gui, AbstractArrowButton::ArrowDirection d)
{
	if (gui == eMacUI) {
		return (new ArrowButton_MacUI (d, Nil));
	}
	else if (gui == eMotifUI) {
		return (new ArrowButton_MotifUI (AbstractArrowButton::eLeft, Nil));
	}
	else if (gui == eWindowsGUI) {
		return (new ArrowButton_WinUI (AbstractArrowButton::eLeft, Nil));
	}
	AssertNotReached ();	return (Nil);
}

/*
 ********************************************************************************
 **************************** ArrowButtonItem ********************************
 ********************************************************************************
 */
ArrowButtonItem::ArrowButtonItem (ItemType& type):
	ButtonItem (type),
	fArrowButton (Nil)
{
	SetArrowButton (BuildArrowButton (EmilyWindow::GetGUI (), AbstractArrowButton::eLeft));
}

AbstractArrowButton&	ArrowButtonItem::GetArrowButton () const
{
	RequireNotNil (fArrowButton);
	return (*fArrowButton);
}

void	ArrowButtonItem::SetArrowButton (AbstractArrowButton* arrow)
{
	SetButton (arrow);
	fArrowButton = arrow;
}

void	ArrowButtonItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	SetArrowButton (BuildArrowButton (newGUI, GetArrowButton ().GetDirection ()));
	ApplyCurrentParams ();
}


String	ArrowButtonItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "ArrowButton.hh";
	return (kHeaderFileName);
}

void	ArrowButtonItem::DoRead_ (class istream& from)
{
	ButtonItem::DoRead_ (from);
	Int32 direction;
	from >> direction;
	GetArrowButton ().SetDirection ((AbstractArrowButton::ArrowDirection) direction);
}

void	ArrowButtonItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ButtonItem::DoWrite_ (to, tabCount);

	to << tab (tabCount) << Int32 (GetArrowButton ().GetDirection ()) << ' ' << newline;
}

void	ArrowButtonItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	switch (GetArrowButton ().GetDirection ()) {
		case AbstractArrowButton::eLeft:
			to << "AbstractArrowButton::eLeft" << newline;
			break;
			
		case AbstractArrowButton::eUp:
			to << "AbstractArrowButton::eUp" << newline;
			break;
			
		case AbstractArrowButton::eRight:
			to << "AbstractArrowButton::eRight" << newline;
			break;
			
		case AbstractArrowButton::eDown:
			to << "AbstractArrowButton::eDown" << newline;
			break;
			
		default:
			AssertNotReached ();
	}
}

void	ArrowButtonItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ButtonItem::WriteParameters (to, tabCount, language, gui);
	to << tab (tabCount) << GetFieldName () << ".SetController (this);" << newline;
}


void	ArrowButtonItem::SetItemInfo ()
{
	ArrowButtonInfo info = ArrowButtonInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetArrowButtonInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetArrowButtonInfoCommand::SetArrowButtonInfoCommand (ArrowButtonItem& item, class ArrowButtonInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewDirection (info.GetDirection ()),
	fOldDirection (item.GetArrowButton ().GetDirection ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}
		
void	SetArrowButtonInfoCommand::DoIt ()
{
	fItem.GetArrowButton ().SetDirection (fNewDirection);
	fItemInfoCommand->DoIt ();
	
	Command::DoIt ();
}

void	SetArrowButtonInfoCommand::UnDoIt ()
{
	fItem.GetArrowButton ().SetDirection (fOldDirection);
	fItemInfoCommand->UnDoIt ();
	
	Command::UnDoIt ();
}

// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


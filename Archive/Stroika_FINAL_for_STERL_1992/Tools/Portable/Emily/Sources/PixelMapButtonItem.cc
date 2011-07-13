/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapButtonItem.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *		$Log: PixelMapButtonItem.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/02/18  03:50:39  lewis
 *		Had to change type name of Version to SaveableVersion to avoid name conflict.
 *
 *		Revision 1.3  1992/02/05  06:06:37  lewis
 *		cleanup, and use PixelMapFromData () instead of direct PixelMap constructor. Create new
 *		savable version, and dont read/write pixelmap since it doesnt work right under X yet.
 *
 *		Revision 1.2  1992/02/03  19:41:08  sterling
 *		switched constructor arfs
 *
 *		Revision 1.1  1992/02/03  17:39:17  sterling
 *		Initial revision
 *	
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"PushButton.hh"
#include	"PixelMapButton.hh"

#include	"PixelMapButtonItem.hh"
#include	"CommandNumbers.hh"
#include	"ViewItemInfo.hh"
#include	"PixelMapButtonInfo.hh"







/*
 ********************************************************************************
 **************************** PixelMapButtonItemType ****************************
 ********************************************************************************
 */
PixelMapButtonItemType::PixelMapButtonItemType ():
#if		qHP_BadTypedefForStaticProcs
	ItemType (eBuildButton, "PixelMapButton", (ItemBuilderProc)&PixelMapButtonItemBuilder)
#else
	ItemType (eBuildButton, "PixelMapButton", &PixelMapButtonItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
PixelMapButtonItemType&	PixelMapButtonItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	PixelMapButtonItemType::PixelMapButtonItemBuilder ()
{
	return (new PixelMapButtonItem (Get ()));
}

PixelMapButtonItemType*	PixelMapButtonItemType::sThis = Nil;



static	AbstractPixelMapButton*	BuildPixelMapButton (CommandNumber gui, const PixelMap& pm)
{
	if (gui == eMacUI) {
		return (new PixelMapButton_MacUI_Portable (pm, Nil));
	}
	else if (gui == eMotifUI) {
		return (new PixelMapButton_MotifUI_Portable (pm, Nil));
	}
	else if (gui == eWindowsGUI) {
		return (new PixelMapButton_WinUI_Portable (pm, Nil));
	}
	AssertNotReached ();	return (Nil);
}

/*
 ********************************************************************************
 **************************** PixelMapButtonItem ********************************
 ********************************************************************************
 */

const	SaveableVersion	kOne		=		1;		// original version reads/writes old format pixelmaps
const	SaveableVersion	kTwo		=		2;		// dont write pixelmaps cuz doesnt work under X yet (Feb 4, 1992)
const	SaveableVersion	kThree		=		3;		// allow pixelmaps read/write again

PixelMapButtonItem::PixelMapButtonItem (ItemType& type):
	ButtonItem (type),
	fPixelMapButton (Nil)
{
	static	UInt8	kPIXELMAPBUTTON_Data [] = {
		0xFF, 0xFF, 
		0x55, 0xD5, 
		0xFF, 0xBF, 
		0x5, 0xE0, 
		0xF7, 0xAF, 
		0x55, 0xED, 
		0xF7, 0xAB, 
		0x55, 0xEE, 
		0x77, 0xAA, 
		0xD5, 0xEF, 
		0xB7, 0xAA, 
		0xF5, 0xEF, 
		0x7, 0xA0, 
		0xFD, 0xFF, 
		0xAB, 0xAA, 
		0xFF, 0xFF, 
	};
	const	PixelMap	kPIXELMAPBUTTON		=	PixelMapFromData (Point (16, 16), kPIXELMAPBUTTON_Data, sizeof (kPIXELMAPBUTTON_Data));

	SetButton (fPixelMapButton = BuildPixelMapButton (EmilyWindow::GetGUI (), kPIXELMAPBUTTON));
	SetMaxVersion (kThree);
}

void	PixelMapButtonItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	SetButton (fPixelMapButton = BuildPixelMapButton (newGUI, fPixelMapButton->GetPixelMap ()));
	ApplyCurrentParams ();
}


String	PixelMapButtonItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "PixelMapButton.hh";
	return (kHeaderFileName);
}

AbstractPixelMapButton&	PixelMapButtonItem::GetPixelMapButton () const
{
	RequireNotNil (fPixelMapButton);
	return (*fPixelMapButton);
}

void	PixelMapButtonItem::DoRead_ (class istream& from)
{
	ButtonItem::DoRead_ (from);

    if (GetVersion () != kTwo) {
		PixelMap	pm = PixelMap (kZeroRect);
		from >> pm;
		fPixelMapButton->SetPixelMap (pm);
	}
}

void	PixelMapButtonItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ButtonItem::DoWrite_ (to, tabCount);

	to << tab (tabCount) << fPixelMapButton->GetPixelMap () << ' ' << newline;
}

void	PixelMapButtonItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	PixelMapButtonItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ButtonItem::WriteParameters (to, tabCount, language, gui);
	to << tab (tabCount) << GetFieldName () << ".SetController (this);" << newline;
}

void	PixelMapButtonItem::SetItemInfo ()
{
	PixelMapButtonInfo info = PixelMapButtonInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetPixelMapButtonInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetPixelMapButtonInfoCommand::SetPixelMapButtonInfoCommand (PixelMapButtonItem& item, class PixelMapButtonInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewPixelMap (info.GetPixelMap ()),
	fOldPixelMap (item.GetPixelMapButton ().GetPixelMap ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}
		
void	SetPixelMapButtonInfoCommand::DoIt ()
{
	fItemInfoCommand->DoIt ();
	fItem.GetPixelMapButton ().SetPixelMap (fNewPixelMap);

	Command::DoIt ();
}

void	SetPixelMapButtonInfoCommand::UnDoIt ()
{
	fItemInfoCommand->UnDoIt ();
	fItem.GetPixelMapButton ().SetPixelMap (fOldPixelMap);

	Command::UnDoIt ();
}








// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


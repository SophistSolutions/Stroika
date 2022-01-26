/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PictureButton.cc,v 1.6 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PictureButton.cc,v $
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:45:09  sterling
 *		changed GUI to UI
 *
 *		Revision 1.3  1992/07/03  01:05:23  lewis
 *		Minor cleanups, and Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:22:52  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_. Changes in border drawing for different GUIs, and
 *		more+++.
 *
 *		Revision 1.9  1992/03/26  09:49:57  lewis
 *		Get rid of EffecitveFontChanged - did nothing and that is now default.
 *
 *		Revision 1.7  1992/03/05  20:40:54  sterling
 *		support for Borders
 *
 *		Revision 1.6  1992/02/15  05:33:06  sterling
 *		used EffectiveLiveChanged
 *
 */



#include	"PictureButton.hh"






/*
 ********************************************************************************
 ******************************* AbstractPictureButton **************************
 ********************************************************************************
 */
AbstractPictureButton::AbstractPictureButton (ButtonController* controller):
	Toggle (controller)
{
}






/*
 ********************************************************************************
 ****************************** AbstractPictureButton_MacUI ********************
 ********************************************************************************
 */
AbstractPictureButton_MacUI::AbstractPictureButton_MacUI (ButtonController* controller):
	AbstractPictureButton (controller)
{
}



/*
 ********************************************************************************
 ************************** PictureButton_MacUI_Portable ***********************
 ********************************************************************************
 */
PictureButton_MacUI_Portable::PictureButton_MacUI_Portable (ButtonController* controller, const Picture& picture):
	AbstractPictureButton_MacUI (controller),
	fPicture (picture)
{
}

PictureButton_MacUI_Portable::PictureButton_MacUI_Portable (ButtonController* controller):
	AbstractPictureButton_MacUI (controller),
	fPicture ()
{
}

void	PictureButton_MacUI_Portable::Draw (const Region& /*update*/)
{
	DrawPicture (fPicture);
	if (GetOn ()) {
		Invert ();
	}
	DrawMacBorder (GetLocalExtent (), GetBorder ());
	if (not GetEffectiveLive ()) {
		Gray ();
	}
}

Point	PictureButton_MacUI_Portable::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	p	=	fPicture.GetFrame ().GetSize ();
	return (p + GetBorder ()*2 + GetMargin ()*2);
}

void	PictureButton_MacUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	Require (GetOn () != on);
	AbstractPictureButton_MacUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		Invert ();
	}
	else {
		Refresh (updateMode);
	}
}

Picture	PictureButton_MacUI_Portable::GetPicture () const
{
	return (fPicture);
}

void	PictureButton_MacUI_Portable::SetPicture (const Picture& picture, Panel::UpdateMode updateMode)
{
	fPicture = picture;
	Refresh (updateMode);
}

Boolean	PictureButton_MacUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	Invert ();
	return (AbstractPictureButton_MacUI::Track (phase, mouseInButton));
}



/*
 ********************************************************************************
 ****************************** AbstractPictureButton_WinUI ********************
 ********************************************************************************
 */
AbstractPictureButton_WinUI::AbstractPictureButton_WinUI (ButtonController* controller):
	AbstractPictureButton (controller)
{
}



/*
 ********************************************************************************
 ************************** PictureButton_WinUI_Portable ***********************
 ********************************************************************************
 */
PictureButton_WinUI_Portable::PictureButton_WinUI_Portable (ButtonController* controller, const Picture& picture):
	AbstractPictureButton_WinUI (controller),
	fPicture (picture)
{
}

PictureButton_WinUI_Portable::PictureButton_WinUI_Portable (ButtonController* controller):
	AbstractPictureButton_WinUI (controller),
	fPicture ()
{
}

void	PictureButton_WinUI_Portable::Draw (const Region& /*update*/)
{
	DrawPicture (fPicture);
	if (GetOn ()) {
		Invert ();
	}
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawWinBorder (GetLocalExtent (), GetBorder (), c, GetPlane ());
	if (not GetEffectiveLive ()) {
		Gray ();
	}
}

Point	PictureButton_WinUI_Portable::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	p	=	fPicture.GetFrame ().GetSize ();
	return (p + GetBorder ()*2 + GetMargin ()*2);
}

void	PictureButton_WinUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	Require (GetOn () != on);
	AbstractPictureButton_WinUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		Invert ();
	}
	else {
		Refresh (updateMode);
	}
}

Picture	PictureButton_WinUI_Portable::GetPicture () const
{
	return (fPicture);
}

void	PictureButton_WinUI_Portable::SetPicture (const Picture& picture, Panel::UpdateMode updateMode)
{
	fPicture = picture;
	Refresh (updateMode);
}

Boolean	PictureButton_WinUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	Invert ();
	return (AbstractPictureButton_WinUI::Track (phase, mouseInButton));
}





PictureButton_MacUI::PictureButton_MacUI (ButtonController* controller, const Picture& picture) :
	PictureButton_MacUI_Portable (controller, picture)
{
}

PictureButton_MacUI::PictureButton_MacUI (ButtonController* controller) :
	PictureButton_MacUI_Portable (controller)
{
}

#if		qSoleInlineVirtualsNotStripped
PictureButton_MacUI::~PictureButton_MacUI ()
{
}
#endif


PictureButton_MotifUI::PictureButton_MotifUI (ButtonController* controller, const Picture& picture) :
	PictureButton_WinUI_Portable (controller, picture)
{
}

PictureButton_MotifUI::PictureButton_MotifUI (ButtonController* controller) :
	PictureButton_WinUI_Portable (controller)
{
}

#if		qSoleInlineVirtualsNotStripped
PictureButton_MotifUI::~PictureButton_MotifUI ()
{
}
#endif


PictureButton_WinUI::PictureButton_WinUI (ButtonController* controller, const Picture& picture) :
	PictureButton_WinUI_Portable (controller, picture)
{
}

PictureButton_WinUI::PictureButton_WinUI (ButtonController* controller) :
	PictureButton_WinUI_Portable (controller)
{
}

#if		qSoleInlineVirtualsNotStripped
PictureButton_WinUI::~PictureButton_WinUI ()
{
}
#endif




PictureButton::PictureButton (ButtonController* controller, const Picture& picture) :
#if		qMacUI
	PictureButton_MacUI (controller, picture)
#elif	qMotifUI
	PictureButton_MotifUI (controller, picture)
#elif	qWinUI
	PictureButton_WinUI (controller, picture)
#endif
{
}

PictureButton::PictureButton (ButtonController* controller) :
#if		qMacUI
	PictureButton_MacUI (controller)
#elif	qMotifUI
	PictureButton_MotifUI (controller)
#elif	qWinUI
	PictureButton_WinUI (controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
PictureButton::~PictureButton ()
{
}
#endif











// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


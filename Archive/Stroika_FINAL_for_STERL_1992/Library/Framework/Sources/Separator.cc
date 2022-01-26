/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Separator.cc,v 1.7 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Separator.cc,v $
 *		Revision 1.7  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.5  1992/07/14  19:57:53  lewis
 *		Renamed file and includes to Separator from Seperator.hh.
 *
 *		Revision 1.4  1992/07/08  03:36:17  lewis
 *		Use AssertNotReached instead of SwitchFallThru.
 *
 *		Revision 1.3  1992/07/03  02:15:17  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:36:17  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.2  1992/05/01  01:38:23  lewis
 *		Get rid of manual creation of tiles since triggered HP CFront compiler bug - better to use
 *		PalletManager::Get ().MakeTileFromColor (kLightGrayColor); anyhow.
 *
 *		Revision 1.1  92/04/16  23:43:20  23:43:20  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"

#include    "Pallet.hh"
#include	"Shape.hh"

#include	"Separator.hh"






/*
 ********************************************************************************
 **************************** AbstractSeparator *********************************
 ********************************************************************************
 */
AbstractSeparator::AbstractSeparator () :
	View ()
{
}

AbstractSeparator::~AbstractSeparator ()
{
}

Point::Direction	AbstractSeparator::GetOrientation () const
{
	return (GetOrientation_ ());
}

void	AbstractSeparator::SetOrientation (Point::Direction orientation, Panel::UpdateMode update)
{
	if (GetOrientation () != orientation) {
		SetOrientation_ (orientation, update);
	}
	Ensure (GetOrientation () == orientation);
}








/*
 ********************************************************************************
 **************************** AbstractSeparator_MacUI **************************
 ********************************************************************************
 */

AbstractSeparator_MacUI::AbstractSeparator_MacUI () :
	AbstractSeparator ()
{
}

Point	AbstractSeparator_MacUI::CalcDefaultSize_ (const Point& hintSize) const
{
	const	Coordinate	kLineThickness = 2;
	
	if (GetOrientation () == Point::eHorizontal) {
		Coordinate	width = (hintSize.GetH () == 0) ? 100 : hintSize.GetH ();
		return (Point (kLineThickness, width));
	}
	
	Assert (GetOrientation () == Point::eVertical);
	Coordinate	height = (hintSize.GetV () == 0) ? 100 : hintSize.GetV ();
	return (Point (height, kLineThickness));
}






/*
 ********************************************************************************
 ************************* AbstractSeparator_MacUI_Portable ********************
 ********************************************************************************
 */
Separator_MacUI_Portable::Separator_MacUI_Portable () :
	AbstractSeparator_MacUI (),
	fOrientation (Point::eHorizontal)
{
}

Separator_MacUI_Portable::Separator_MacUI_Portable (Point::Direction orientation) :
	AbstractSeparator_MacUI (),
	fOrientation (orientation)
{
}

void	Separator_MacUI_Portable::Draw (const Region& update)
{
	Paint (Rectangle (), GetLocalExtent (), kBlackTile);
	AbstractSeparator_MacUI::Draw (update);
}

Point::Direction	Separator_MacUI_Portable::GetOrientation_ () const
{
	return (fOrientation);
}

void	Separator_MacUI_Portable::SetOrientation_ (Point::Direction orientation, Panel::UpdateMode update)
{
	fOrientation = orientation;
	Refresh (update);
}










/*
 ********************************************************************************
 ***************************** AbstractSeparator_MotifUI ***********************
 ********************************************************************************
 */

AbstractSeparator_MotifUI::AbstractSeparator_MotifUI () :
	AbstractSeparator ()
{
}

Point	AbstractSeparator_MotifUI::CalcDefaultSize_ (const Point& hintSize) const
{
	const	Coordinate	kLineThickness = 4;
	
	if (GetOrientation () == Point::eHorizontal) {
		Coordinate	width = (hintSize.GetH () == 0) ? 100 : hintSize.GetH ();
		return (Point (kLineThickness, width));
	}
	
	Assert (GetOrientation () == Point::eVertical);
	Coordinate	height = (hintSize.GetV () == 0) ? 100 : hintSize.GetV ();
	return (Point (height, kLineThickness));
}

AbstractSeparator_MotifUI::Etching	AbstractSeparator_MotifUI::GetEtching () const
{
	return (GetEtching_ ());
}

void	AbstractSeparator_MotifUI::SetEtching (Etching etching, Panel::UpdateMode update)
{
	if (GetEtching () != etching) {
		SetEtching_ (etching, update);
	}
	Ensure (GetEtching () == etching);
}




/*
 ********************************************************************************
 ************************ AbstractSeparator_MotifUI_Portable *******************
 ********************************************************************************
 */
Separator_MotifUI_Portable::Separator_MotifUI_Portable () :
	AbstractSeparator_MotifUI (),
	fOrientation (Point::eHorizontal),
	fEtching (eEtchedIn)
{
}

Separator_MotifUI_Portable::Separator_MotifUI_Portable (Point::Direction orientation) :
	AbstractSeparator_MotifUI (),
	fOrientation (orientation),
	fEtching (eEtchedIn)
{
}

void	Separator_MotifUI_Portable::Draw (const Region& /*update*/)
{
   	const Tile kLightGrayTile = PalletManager::Get ().MakeTileFromColor (kLightGrayColor);
	/*
	 * Drawing algorithm: we divide ourselves into two rectangles (top and bottom or
	 * left and right depending on orientation). We draw one rectangle in black, the
	 * other in light gray, depending on wether we are etched in or out. If simply
	 * filled, than make everything black (this is the default on Mac GUI).
	 */
	Rect halfRect1 = GetLocalExtent ();
	Rect halfRect2 = kZeroRect;
	if (GetOrientation () == Point::eVertical) {
		halfRect1.SetSize (Point (halfRect1.GetSize ().GetV (), halfRect1.GetSize ().GetH () / 2));
		halfRect2 = halfRect1 + Point (0, halfRect1.GetSize ().GetH ());
	}
	else {
		halfRect1.SetSize (Point (halfRect1.GetSize ().GetV () / 2, halfRect1.GetSize ().GetH ()));
		halfRect2 = halfRect1 + Point (halfRect1.GetSize ().GetV (), 0);
	}

	switch (GetEtching ()) {
		case eEtchedIn:
			Paint (Rectangle (), halfRect1, kBlackTile);
			Paint (Rectangle (), halfRect2, kLightGrayTile);
			break;

		case eEtchedOut:
			Paint (Rectangle (), halfRect1, kLightGrayTile);
			Paint (Rectangle (), halfRect2, kBlackTile);
			break;
			
		default:
			AssertNotReached ();
	}
}

Point::Direction	Separator_MotifUI_Portable::GetOrientation_ () const
{
	return (fOrientation);
}

void	Separator_MotifUI_Portable::SetOrientation_ (Point::Direction orientation, Panel::UpdateMode update)
{
	fOrientation = orientation;
	Refresh (update);
}

AbstractSeparator_MotifUI::Etching	Separator_MotifUI_Portable::GetEtching_ () const
{
	return (fEtching);
}

void	Separator_MotifUI_Portable::SetEtching_ (AbstractSeparator_MotifUI::Etching etching, Panel::UpdateMode update)
{
	fEtching = etching;
	Refresh (update);
}






/*
 ********************************************************************************
 **************************** AbstractSeparator_WinUI **************************
 ********************************************************************************
 */

AbstractSeparator_WinUI::AbstractSeparator_WinUI () :
	AbstractSeparator ()
{
}

Point	AbstractSeparator_WinUI::CalcDefaultSize_ (const Point& hintSize) const
{
	const	Coordinate	kLineThickness = 2;
	
	if (GetOrientation () == Point::eHorizontal) {
		Coordinate	width = (hintSize.GetH () == 0) ? 100 : hintSize.GetH ();
		return (Point (kLineThickness, width));
	}
	
	Assert (GetOrientation () == Point::eVertical);
	Coordinate	height = (hintSize.GetV () == 0) ? 100 : hintSize.GetV ();
	return (Point (height, kLineThickness));
}






/*
 ********************************************************************************
 ************************* AbstractSeparator_WinUI_Portable ********************
 ********************************************************************************
 */
Separator_WinUI_Portable::Separator_WinUI_Portable () :
	AbstractSeparator_WinUI (),
	fOrientation (Point::eHorizontal)
{
}

Separator_WinUI_Portable::Separator_WinUI_Portable (Point::Direction orientation) :
	AbstractSeparator_WinUI (),
	fOrientation (orientation)
{
}

void	Separator_WinUI_Portable::Draw (const Region& update)
{
	Paint (Rectangle (), GetLocalExtent (), kBlackTile);
	AbstractSeparator_WinUI::Draw (update);
}

Point::Direction	Separator_WinUI_Portable::GetOrientation_ () const
{
	return (fOrientation);
}

void	Separator_WinUI_Portable::SetOrientation_ (Point::Direction orientation, Panel::UpdateMode update)
{
	fOrientation = orientation;
	Refresh (update);
}




/*
 ********************************************************************************
 ********************************* Separator_MacUI ******************************
 ********************************************************************************
 */
Separator_MacUI::Separator_MacUI () :
	Separator_MacUI_Portable ()
{
}

Separator_MacUI::Separator_MacUI (Point::Direction orientation) :
	Separator_MacUI_Portable (orientation)
{
}

#if		qSoleInlineVirtualsNotStripped
Separator_MacUI::~Separator_MacUI ()
{
}
#endif





/*
 ********************************************************************************
 ******************************* Separator_MotifUI ******************************
 ********************************************************************************
 */
Separator_MotifUI::Separator_MotifUI () :
	Separator_MotifUI_Portable ()
{
}

Separator_MotifUI::Separator_MotifUI (Point::Direction orientation) :
	Separator_MotifUI_Portable (orientation)
{
}

#if		qSoleInlineVirtualsNotStripped
Separator_MotifUI::~Separator_MotifUI ()
{
}
#endif





/*
 ********************************************************************************
 ********************************* Separator_WinUI ******************************
 ********************************************************************************
 */
Separator_WinUI::Separator_WinUI () :
	Separator_WinUI_Portable ()
{
}

Separator_WinUI::Separator_WinUI (Point::Direction orientation) :
	Separator_WinUI_Portable (orientation)
{
}

#if		qSoleInlineVirtualsNotStripped
Separator_WinUI::~Separator_MacUI ()
{
}
#endif




/*
 ********************************************************************************
 *************************************** Separator ******************************
 ********************************************************************************
 */
Separator::Separator (Point::Direction orientation) :
#if		qMacUI
	Separator_MacUI (orientation)
#elif	qMotifUI
	Separator_MotifUI (orientation)
#elif	qMotifUI
	Separator_WinUI (orientation)
#endif	/*GUI*/
{
}

Separator::Separator () :
#if		qMacUI
	Separator_MacUI ()
#elif	qMotifUI
	Separator_MotifUI ()
#elif	qMotifUI
	Separator_WinUI ()
#endif	/*GUI*/
{
}

#if		qSoleInlineVirtualsNotStripped
Separator::~Separator ()
{
}
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


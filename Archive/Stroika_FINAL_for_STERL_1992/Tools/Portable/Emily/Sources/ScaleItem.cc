/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScaleItem.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScaleItem.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  92/04/30  14:18:32  14:18:32  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"Scale.hh"

#include	"ScaleItem.hh"
#include	"ScaleInfo.hh"
#include	"CommandNumbers.hh"






/*
 ********************************************************************************
 ****************************** ScaleItemType *******************************
 ********************************************************************************
 */
ScaleItemType::ScaleItemType () :
#if		qHP_BadTypedefForStaticProcs
	ItemType (eBuildSlider, "Scale", (ItemBuilderProc)&ScaleItemBuilder)
#else
	ItemType (eBuildSlider, "Scale", &ScaleItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
ScaleItemType&	ScaleItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	ScaleItemType::ScaleItemBuilder ()
{
	return (new ScaleItem (Get ()));
}

ScaleItemType*	ScaleItemType::sThis = Nil;





static	AbstractScale*	BuildScale (CommandNumber gui)
{
	if (gui == eMacUI) {
		return (new Scale_MacUI_Portable (Nil));
	}
	else if (gui == eMotifUI) {
		return (new Scale_MotifUI_Portable (Nil));
	}
	else if (gui == eWindowsGUI) {
		return (new Scale_WinUI_Portable (Nil));
	}
	AssertNotReached ();	return (Nil);
}


/*
 ********************************************************************************
 ********************************** ScaleItem ***********************************
 ********************************************************************************
 */
ScaleItem::ScaleItem (ItemType& type) :
	SliderItem (type),
	fScale (Nil),
	fDefaultSensitive (False),
	fDefaultThumbLength (0)
{
	SetScale (BuildScale (EmilyWindow::GetGUI ()));
}

AbstractScale&	ScaleItem::GetScale () const
{
	AssertNotNil (fScale);
	return (*fScale);
}

void	ScaleItem::SetScale (AbstractScale* scale)
{
	SetSlider (scale);
	fScale = scale;
	if (fScale != Nil) {
		fDefaultSensitive	= GetScale ().GetSensitive ();
		fDefaultThumbLength	= GetScale ().GetThumbLength ();
	}
}


void	ScaleItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	SetScale (BuildScale (newGUI));
	ApplyCurrentParams ();
}

String	ScaleItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "Scale.hh";
	return (kHeaderFileName);
}

void	ScaleItem::WriteBuilder (class ostream& to, int tabCount)
{
	SliderItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	ScaleItem::DoRead_ (class istream& from)
{
	SliderItem::DoRead_ (from);
	Boolean	sensitive;
	Coordinate	thumbLength;
	from >> sensitive >> thumbLength;
	GetScale ().SetSensitive (sensitive);
	GetScale ().SetThumbLength (thumbLength, eNoUpdate);
}

void	ScaleItem::DoWrite_ (class ostream& to, int tabCount) const
{
	SliderItem::DoWrite_ (to, tabCount);
	to << tab (tabCount) << GetScale ().GetSensitive () << ' ' << GetScale ().GetThumbLength ();
	to << newline;
}

void	ScaleItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	SliderItem::WriteParameters (to, tabCount, language, gui);
	if (fDefaultSensitive != GetScale ().GetSensitive ()) {
		to << tab (tabCount) << GetFieldName () << ".SetSensitive ("
		   << GetBooleanAsText (GetScale ().GetSensitive ()) << ");" << newline;
	}
	if (fDefaultThumbLength != GetScale ().GetThumbLength ()) {
		to << tab (tabCount) << GetFieldName () << ".SetThumbLength ("
		   << GetScale ().GetThumbLength () << ");" << newline;
	}
}

void	ScaleItem::SetItemInfo ()
{
	ScaleInfo info = ScaleInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetScaleInfoCommand (*this, info));
		DirtyDocument ();
	}
}


/*
 ********************************************************************************
 ***************************** SetScaleInfoCommand ******************************
 ********************************************************************************
 */

SetScaleInfoCommand::SetScaleInfoCommand (ScaleItem& item, class ScaleInfo& info) :
	SetSliderInfoCommand (
		item, 
		info.GetMinValueField ().GetValue (),
		info.GetMaxValueField ().GetValue (),
		info.GetValueField ().GetValue (),
		info.GetViewItemInfo ()),
	fScale (item),
	fNewSensitive (info.GetSensitive ()),
	fOldSensitive (item.GetScale ().GetSensitive ()),
	fNewThumbLength (info.GetThumbLength ()),
	fOldThumbLength (item.GetScale ().GetThumbLength ())
{
}
		
void	SetScaleInfoCommand::DoIt ()
{
	fScale.GetScale ().SetSensitive (fNewSensitive);
	fScale.GetScale ().SetThumbLength (fNewThumbLength);
	SetSliderInfoCommand::DoIt ();
}

void	SetScaleInfoCommand::UnDoIt ()
{
	fScale.GetScale ().SetSensitive (fOldSensitive);
	fScale.GetScale ().SetThumbLength (fOldThumbLength);
	SetSliderInfoCommand::UnDoIt ();
}
